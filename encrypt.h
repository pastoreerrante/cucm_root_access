#ifndef ENCRYPT_H_INCLUDED
#define ENCRYPT_H_INCLUDED

unsigned char* encrypt(unsigned char* iv, 
		       unsigned char* aesKey, 
		       unsigned char* plaintext,
		       int* ciphertext_len); 

#endif
