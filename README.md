# Cryptodev-Linux-Module-with-Paravirtualisation-application
Hardware Accelerated Cryptography through Cryptodev Module used in Client-Server Communication. Implementation also done in GUEST OS userspace using VIRTIO to use HOST hardware. 
## Server Client Userspace Application 
It is written in a modular way and it consists of two main parts
1. First Part consists of establishing TCP connection over IPV4 and checking the arguments of the program (Hostname,Port for Client)
2. Second Part consists of the function chat_init() which implements the Encryption,Decryption,Reading and Writing of the Data that is communicated between Server and Client.For non-blocking and bidirectional operation, "SELECT()" system call is used.
## Encryption and Decrytpion of the Data
Cryptodev Module API is used for encryption/decryption of the data using the AES 128 Encryption Standard. This allows the cryptographic process to be faster by being implement in the hardware accelerators of the host machine or in the kernel code of the host machine in case such accelerators are not available.
## QEMU-KVM Split Driver Model with VIRTIO
> It must be noted that, because third parties other than the authors stated at the end of this README were involved in the driver implementation, for Intellectual Property reasons code will not be uploaded regarding the front end and back end drivers for VIRTIO. However, below is described an overview of the whole implementation which should give a clear idea of what was used and how.
### Data Transfer Between Host and Guest
Virtio standard allows us to use "virtqueues" and their ring buffers to transfer information or system call requests between guest OS and host OS. What we did is use scatterlists as a way to DMA access a virtio PCI device, more specifically the cryptodevice we would like to use.
* Frontend Driver:
Frontend driver was implemented in the guest os kernel (host userspace) and used VIRTIO API calls of add, get and kick to "add" information to the ring buffer, "get" information from the ring buffer or "kick" the backend driver (host qemu process) to notify it of available data in the ring buffers. The "getting" of information was implemented with busy waiting-polling, thought it could be arguably better to utilise interrupts. Moreover in the guest OS , in order to use this part of the driver we needed to create some userspace special files to interract with our /dev/cryptodev devices in userspace.
* Backend Driver:
It is implemented in HOST OS and it is running as a process(QEMU-KVM supervisor). Here we implement the actual system calls to /cryptodev in order to encrypt/decrypt our data. The corresponding system calls used are bind to virtqueue of the crypto dev as "pop", to acquire the first virtqueue element and "push" to return the data requested to the virt ring. 
