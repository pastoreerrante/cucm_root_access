#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char* sanitizeString(char* input, size_t len) {

	char* buffer = malloc(len +1);
	strcpy(buffer, input);

	for (int i = 14; i <= 23; i++) {

		printf("%c", buffer[i]);

		if (buffer[i] == 'O') {  // ascii 'O' maiuscola 
			buffer[i] = '8'; // ascii '8'
			continue;
		}

		if (buffer[i] == 'o') {   // ascii 'o' minuscola 
			buffer[i] = 'S';  // ascii 'S' 
			continue;
		}

		if (buffer[i] == '+') {   // ascii '+' 
			buffer[i] = 'J';  // ascii 'J' 
			continue;
		}

		if (buffer[i] == '/') {   // ascii '/'  
			buffer[i] = 'P';  // ascii 'P' 
			continue;
		}

		if (islower(buffer[i])) {
			buffer[i] = toupper(buffer[i]);
		}

	}
	
	return buffer;

}


