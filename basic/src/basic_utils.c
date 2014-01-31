#include <stdio.h>
#include <basic_utils.h>

void dump(const unsigned char *data_buffer, const unsigned int length) {
	unsigned char byte;
	unsigned int i, j;

	for (i = 0; i < length; i++) {
		byte = data_buffer[i];
		/* display byte in hex */
		printf("%02x ", data_buffer[i]);

		if (((i%16) == 15) || (i == length-1)) {
			/* padding for the last line */
			for (j = 0; j < 15-(i%16); j++)
				printf("   ");


			printf("| ");
			for(j = (i-(i%16)); j <= i; j++) {
				byte = data_buffer[j];

				/* print the character */
				if((byte > 31) && (byte < 127))
					printf("%c", byte);
				else
					printf(".");
			}

			/* done a line */
			printf("\n");
		}
	}
}