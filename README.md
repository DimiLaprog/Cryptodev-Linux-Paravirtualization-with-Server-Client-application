# Cryptodev-Linux-Module-with-Paravirtualisation-application
Hardware Accelerated Cryptography through Cryptodev Module used in Client-Server Communication. Implementation also done in GUEST OS userspace using VIRTIO to use HOST hardware. 
## Server Client Userspace Application 
It is written in a modular way and it consists of two main parts
1. First Part consists of establishing TCP connection over IPV4 and checking the arguments of the program (Hostname,Port for Client)
2. Second Part consists of the function chat_init() which implements the Encryption,Decryption,Reading and Writing of the Data that is communicated between Server and Client.
## Encryption and Decrytpion of the Data
Cryptodev Module API is used for encryption/decryption of the data using the AES 128 Encryption Standard. This allows the cryptographic process to be faster by being implement in the hardware accelerators of the host machine or in the kernel code of the host machine in case such accelerators are not available.
