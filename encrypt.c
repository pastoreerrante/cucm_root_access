#include <stdio.h>
#include <string.h> // for strlen()
#include <stdint.h> // for uint8_t
#include <stdlib.h> // for malloc
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

unsigned char* encrypt(unsigned char* iv, 
		       unsigned char* aesKey, 
		       unsigned char* plaintext,
		       int* ciphertext_len) { 

	// Initialise the library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);

	int plaintext_len = strlen((char*) plaintext);
	printf("plaintext_len = %d\n", plaintext_len);

	// Create and initialise the context
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();
                                                  
	EVP_EncryptInit(ctx, EVP_aes_128_cbc(), aesKey, iv);

	int len;
	int ol = 0;
	unsigned char* ret = (unsigned char *)
		malloc(plaintext_len + EVP_CIPHER_CTX_block_size(ctx));

	for (int i = 0; i < plaintext_len / 50; i++) {
		EVP_EncryptUpdate(ctx, 
				  &ret[ol], &len, 
				  &plaintext[ol], 50);
		ol += len;
		printf ("partial len of encrypted text in Update is %d\n", ol);
	}
	                                                                              
	printf("plaintext_len %% 50 = %d\n", plaintext_len % 50);
	                                                                              
	if (plaintext_len % 50) {
		EVP_EncryptUpdate(ctx, 
				  &ret[ol], &len, 
				  &plaintext[ol], plaintext_len % 50);
		ol += len;
		printf ("partial len of encrypted text in last update is %d\n", len);
	}
	                                                                              
	EVP_EncryptFinal(ctx, &ret[ol], &len);
	                                                                              
	printf ("partial len of encrypted text after final is %d\n", len);
	                                                                              
	*ciphertext_len = ol + len;
	                                                                              
	return ret;

}
