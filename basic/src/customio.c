#include <stdlib.h>
#include "customio.h"

int customio_is_ws(char c) {
	switch (c) {
		/* tab */
		case 9:
		/* line feed */
		case 10:
		/* vertical tab */
		case 11:
		/* form feed */
		case 12:
		/* carriage return */
		case 13:
		/* space */
		case 32:
			return 1;

		default:
			return 0;
	}
}

int customio_get_line(FILE *stream, char **ptr, int *num) {
	char *temp_ptr = NULL;
	char *cont_ptr = NULL;
	int buf_size = 0;
	int str_size = 0;

	buf_size = CUSTOM_INIT_BUF_SIZE;
	temp_ptr = (char *)malloc(buf_size);
	fgets(temp_ptr, CUSTOM_INIT_BUF_SIZE, stream);
	if (ferror(stream)) {
		return CUSTOMIO_READ_ERROR;
	}
	str_size = strlen(temp_ptr);

	while (!feof(stream) && str_size < buf_size-1) {
		buf_size += CUSTOMIO_BUF_INC;
		cont_ptr = realloc(temp_ptr, buf_size);
		if (cont_ptr == NULL) {
			return CUSTOMIO_ALLOC_ERROR;
		}
		temp_ptr = cont_ptr;
		
	}