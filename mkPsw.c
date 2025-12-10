#include <stdio.h>
#include <string.h> // for strlen()
#include <stdint.h> // for uint8_t
#include <stdlib.h> // malloc and //free
#include <openssl/hmac.h> // for hmac()
#include "base64.h"
#include "encrypt.h"
#include "sanitize.h"

int main(int arg, char *argv[]) {

	// initial input
	char passphrase[] = "WGW6ARAT8D";

	size_t passphrase_len = strlen(passphrase);
	uint8_t shifted_passphrase[passphrase_len];

	// 1° transform: shift left 3
	for (int i = 0; i < strlen(passphrase); i++) { 
		// printf("%c --> ", passphrase[i]);
		// printf("%#x --> ", passphrase[i]);
		shifted_passphrase[i] = passphrase[i] << 3;
		// printf("%#x\n", shifted_passphrase[i]);
	}

	// 2° transform: convert shifted_passphrase bytes to base64
	int outLen; // Length of the resultant string is stored in flen
	char* base64EncodedBytes = base64(shifted_passphrase, 
					  passphrase_len, 
					  &outLen);
	// printf("\noutput len is %d\n", outLen);
	// printf("%s\n", base64EncodedBytes); 

        char decoder_entropy1[] = 
	"GTGK+/UNjnBxejpm2aiwz3EvESgQNrhSqoEwARF+cPcLfLHBKX9TXniD0vkD+LV";

	// make room in order to concatenate base64EncodedBytes 
	// with decoder_entropy1
        size_t bufSize = strlen(base64EncodedBytes) +
		         strlen(decoder_entropy1) +
			 1; // add space for the trailing null byte
	char buf[bufSize];
	strcpy(buf, base64EncodedBytes);

	//free(base64EncodedBytes);

	// printf("buf before strncat:\n%s\n", buf); 
	// printf("sizeof(buf) before strncat: %lu\n", sizeof(buf));
	// printf("strlen(buf) before strncat: %lu\n", strlen(buf));

	// 3° transform: concatenate base64EncodedBytes with decoder_entropy1
        strncat(buf, 		  // destination 
		decoder_entropy1, // source 
		strlen(decoder_entropy1));

	// printf("buf after strncat:\n%s\n", buf); 
	// printf("sizeof(buf) after strncat: %lu\n", sizeof(buf));
	// printf("strlen(buf) after strncat: %lu\n", strlen(buf));

	// 4° transform: append char '3' to buf
	char c = '3';
	size_t len = strlen(buf);
	// printf("%lu\n", len);
	char result[len + 1 + 1]; // space for '3' and trailing null
	strcpy(result, buf);
	result[len] = c;
	result[len + 1] = '\0';

	// printf( "%s\n", result);
	// printf("sizeof(result) after append: %lu\n", sizeof(result));
	// printf("strlen(result) after append: %lu\n", strlen(result));

	// 5° transform: SHL 3 and ADD 4
	uint8_t shifted_result[strlen(result)];
	                                                    
	for (int i = 0; i < strlen(result); i++) { 
		// printf("%c --> ", result[i]);
		// printf("%#x --> ", result[i]);
		shifted_result[i] = result[i] << 3;
		shifted_result[i] += 4;
		// printf("%#x\n", shifted_result[i]);
	}

	// 6° transform: convert shifted_result bytes to base64
	char* plaintext = base64(shifted_result,
				 strlen(result),
				 &outLen);
	// printf("\noutput len is %d\n", outLen);
	printf("plaintext is: %s\n", plaintext);

	// only the first 0x10 char seem to be used. They represent the key
	// of the next HMAC function
        char decoder_entropy2[] = 
	"tcYuu+IDtwj6xDG5"
	"Fhc4qoml0VPaxFECIGr0IX/C7x1YS3ASdMPqCvr6Ev7XoaitRBlpxigNM7";

	char hmacKey[] = "tcYuu+IDtwj6xDG5";
	uint8_t output[EVP_MAX_MD_SIZE];
	unsigned int output_len;

	// 7° transform: HMAC of plaintext
	HMAC(EVP_sha1(), 
	     hmacKey, strlen(hmacKey), 
	     // base64EncodedBytes is signed. 
	     // We cast to unsigned to avoid the warning 
	     // from compiler
	     (unsigned char*) plaintext, 
	     // strlen input should be signed. 
	     // If we pass an unsigned value, the 
	     // compiler emits a warning. 
	     // Cast is needed to avoid this warning 	
	     strlen((char*) plaintext), 
	     output, &output_len);	

	// for (int i = 0; i < output_len; i++)
	// 	printf("%#x\n", output[i]);

	//free(plaintext);
	// 8° transform: convert output bytes to base64
	char* aesKey = base64(output,     // this is really the input
			      output_len, // input len 
			      &outLen);	  // outlen is the length of the
					  // generated base64 string
	// printf("\noutput len is %d\n", outLen);
	printf("\nhmac output (aes key) in base 64: %s\n", aesKey);

	// 9° transform: encrypting the plaintext 
	// key is the content of base64EncodedBytes 
	// iv  is "tcYuu+IDtwj6xDG5" (first 0x10 bytes of decoder_entropy2)
	// plaintext is the result of various byte manipulation. the last manip 
	// is base64 conversion of the 5° transfor (SHL 3 and ADD 4) 
	
	unsigned char iv[] = "tcYuu+IDtwj6xDG5";
	int ciphertext_len;

	unsigned char* ciphertext = encrypt(iv, 
					    (unsigned char *) aesKey, 
					    (unsigned char *) plaintext,
					    &ciphertext_len);

	for (int i = 0; i < ciphertext_len; i++) { 
		printf("ciphertext[%d] = %#x\n", i, ciphertext[i]);
	}

	// 10° transform: convert ciphertext bytes to base64
	char* base64ciphertext = base64(ciphertext,   
			      	 ciphertext_len,
			      	 &outLen); // outlen is the length of
					   // generated base64 string
	printf("\nciphertext in base 64: %s\n", base64ciphertext);

	char* sanitizedString = sanitizeString(base64ciphertext);

	printf("ciphertext after sanitize: %s\n", sanitizedString );

	return 0;

}
