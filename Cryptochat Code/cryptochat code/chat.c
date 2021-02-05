/*
 * chat.c
 * Simple char using cryptography
 *
 * Giannis Giortzis <giortzis.giannis@gmail.com>
 * Dimitris Lampros <dim.lambros@gmail.com>
 */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <crypto/cryptodev.h>
#include "socket-common.h"

#define CYAN "\e[1;34m"
#define WHITE "\033[37m"

#define DATA_SIZE       256
#define BLOCK_SIZE      16
#define KEY_SIZE	16  /* AES128 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

//-----------------------------------------------------------------ENCRYPTION PRIVATE DATA BEGIN--------------------------

struct session_op sess;
struct crypt_op cryp;
struct {
	unsigned char 	in[DATA_SIZE],
			encrypted[DATA_SIZE],
			decrypted[DATA_SIZE],
			iv[BLOCK_SIZE],
			key[KEY_SIZE];
} data;

int cryptodev_fd;
//-----------------------------------------------------------------ENCRYPTION PRIVATE DATA END--------------------------
/* Insist until all of the data has been read */
ssize_t insist_read(int fd, void *buf, size_t cnt)
{
        ssize_t ret;
        size_t orig_cnt = cnt;

        while (cnt > 0) {
                ret = read(fd, buf, cnt);
                if (ret < 0)
                        return ret;
                buf += ret;
                cnt -= ret;
        }

        return orig_cnt;
}

/* Insist until all of the data has been written */
ssize_t insist_write(int fd, const void *buf, size_t cnt)
{
	ssize_t ret;
	size_t orig_cnt = cnt;

	while (cnt > 0) {
	        ret = write(fd, buf, cnt);
	        if (ret < 0)
	                return ret;
	        buf += ret;
	        cnt -= ret;
	}

	return orig_cnt;
}

int encryption_init(void){
  cryptodev_fd = open("/dev/crypto", O_RDWR);
  if (cryptodev_fd < 0) {
    perror("open(/dev/crypto)");
    return 1;
  }
	memset(&sess, 0, sizeof(sess));
	memset(&cryp, 0, sizeof(cryp));
	*data.key=(intptr_t)"123456ABA";
	*data.iv=(intptr_t)"ABACBADEC";
	/*
	 * Get crypto session for AES128
	 */
	sess.cipher = CRYPTO_AES_CBC;
	sess.keylen = KEY_SIZE;
	sess.key = data.key;
	return 0;
}

int encrypt(int cfd){
	if (ioctl(cfd, CIOCGSESSION, &sess)) {
		perror("ioctl(CIOCGSESSION)");
		return 1;
	}

	/*
	 * Encrypt data.in to data.encrypted
	 */
	cryp.ses = sess.ses;
	cryp.len = sizeof(data.in);
	cryp.src = data.in;
	cryp.dst = data.encrypted;
	cryp.iv = data.iv;
	cryp.op = COP_ENCRYPT;

	if (ioctl(cfd, CIOCCRYPT, &cryp)) {
		perror("ioctl(CIOCCRYPT)");
		return 1;
	}

	/* Finish crypto session */
	if (ioctl(cfd, CIOCFSESSION, &sess.ses)) {
		perror("ioctl(CIOCFSESSION)");
		return 1;
	}

	// pass encrypted data to global buffer
	//sprintf(encrypted_data, "%s", data.encrypted);

	return 0;
}

int decrypt(int cfd){
	if (ioctl(cfd, CIOCGSESSION, &sess)) {
		perror("ioctl(CIOCGSESSION)");
		return 1;
	}


	/*
	 * decrypt data.in
	 */
	cryp.ses = sess.ses;
	cryp.len = sizeof(data.encrypted);
	cryp.dst = data.decrypted;
	cryp.src = data.encrypted;
	cryp.iv = data.iv;
	cryp.op = COP_DECRYPT;

	if (ioctl(cfd, CIOCCRYPT, &cryp)) {
		perror("ioctl(CIOCCRYPT)");
		return 1;
	}

	/* Finish crypto session */
	if (ioctl(cfd, CIOCFSESSION, &sess.ses)) {
		perror("ioctl(CIOCFSESSION)");
		return 1;
	}
	return 0;
}

int chat_init(int sd){
    encryption_init();
  	for (;;) {
  		fd_set inset;
      int maxfd, n_read;
      FD_ZERO(&inset);                // we must initialize before each call to select
      FD_SET(STDIN_FILENO, &inset);   // select will check for input from stdin
      FD_SET(sd, &inset);  // select will check for input from socket descriptor (Client)
      // select only considers file descriptors that are smaller than maxfd
      maxfd = MAX(STDIN_FILENO,sd) + 1;

      // wait until any of the input file descriptors are ready to receive
      int ready_fds = select(maxfd, &inset, NULL, NULL, NULL);
      if (ready_fds <= 0) {
          perror("select");
          continue;                                       // just try again
      }

      // user has typed something, we can read from stdin without blocking
      if (FD_ISSET(STDIN_FILENO, &inset)){
        n_read = read(STDIN_FILENO, data.in, sizeof(data.in));   //error checking!
        if (n_read == -1) // read() attempts to read up to count bytes from STDIN into the buffer starting at buffer.
        {
            perror("read");
            exit(-1);
        }
  			data.in[n_read] = '\0';
  			encrypt(cryptodev_fd);
    	if (insist_write(sd, data.encrypted, sizeof(data.encrypted)) != sizeof(data.encrypted)) {
  				perror("write");
  				exit(1);
  			}

  		}
  		else if (FD_ISSET(sd, &inset)){
  			n_read = read(sd, data.encrypted, sizeof(data.encrypted));
  			if (n_read < 0) {
  				perror("read");
  				exit(1);
  			}
  			if (n_read <= 0)
  				break;
  			decrypt(cryptodev_fd);
  			printf(CYAN"%s"WHITE, data.decrypted);
        fflush(stdout);
  		}
  	}
    /* Make sure we don't leak open files */
		if (close(cryptodev_fd) < 0) {
		    perror("close(cryptodev_fd)");
		    return 1;
	  }
    return 0;
}
