/*
 * chat.h
 *
 * Giannis Giortzis <giortzis.giannis@gmail.com>
 * Dimitris Lampros <dim.lambros@gmail.com>
 */

#ifndef _CHAT_H

#define _CHAT_H
#define DATA_SIZE       256
#define BLOCK_SIZE      16
#define KEY_SIZE	16  /* AES128 */

ssize_t insist_read(int fd, void *buf, size_t cnt);
ssize_t insist_write(int fd, const void *buf, size_t cnt);
int fill_urandom_buf(unsigned char *buf, size_t cnt);
int encryption_init(void);
int encrypt(int cfd);
int decrypt(int cfd);
int chat_init();
#endif
