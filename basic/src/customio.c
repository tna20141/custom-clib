#include <stdlib.h>
#include <string.h>
#include "customio.h"

int __customio_is_delim(char c, const char *delims, int ws) {
	int i = 0;

	if (ws) {
		if (customio_is_ws(c))
			return 1;
	}

	while (delims[i] != '\0') {
		if (c == delims[i])
			return 1;
		i++;
	}
	return 0;
}

int __customio_get_before_delim(FILE *stream, const char *delims, int ws, char **ptr, int size, int *num, char *delim, int ignore) {
	char *ptr1 = *ptr;
	char *temp_ptr = NULL;
	char temp_chr = 0;
	int buf_size = size;
	int str_size = 0;
	int reallocate = 0;

	/* initial allocation if necessary */
	if (ptr1 == NULL) {
		buf_size = CUSTOMIO_INIT_BUF_SIZE;
		ptr1 = (char *)malloc(buf_size);
		if (ptr1 == NULL) {
			return CUSTOMIO_ALLOC_ERROR;
		}
		reallocate = 1;
	}

	/* continue until EOF or a delimiter is reached */
	while ((temp_chr = fgetc(stream)) != EOF) {
		/* if ignoring leading whitespaces */
		if (ignore) {
			if (customio_is_ws(temp_chr)) {
				continue;
			} else {
				ignore = 0;
			}
		}
		/* if a delimiter is reached, stop reading */
		if (__customio_is_delim(temp_chr, delims, ws)) {
			fseek(stream, -1, SEEK_CUR);
			break;
		}

		/* if the buffer is full, expand the buffer */
		if (str_size+2 >= buf_size) {
			buf_size += CUSTOMIO_BUF_INC;
			temp_ptr = (char *)realloc(ptr1, buf_size);
			if (temp_ptr == NULL) {
				if (reallocate)
					free(ptr1);
				return CUSTOMIO_ALLOC_ERROR;
			}
			ptr1 = temp_ptr;
			reallocate = 1;
		}

		/* store the character and move on */
		ptr1[str_size] = temp_chr;
		str_size++;
	}

	/* check for read error */
	if (ferror(stream)) {
		if (reallocate)
			free(ptr1);
		return CUSTOMIO_READ_ERROR;
	}

	/* reallocate one last time to save space, if necessary */
	if (reallocate) {
		temp_ptr = (char *)realloc(ptr1, str_size+1);
		if (temp_ptr == NULL) {
			if (reallocate)
				free(ptr1);
			return CUSTOMIO_ALLOC_ERROR;
		}
		ptr1 = temp_ptr;
	}

	/* save the results and return */
	ptr1[str_size] = '\0';
	if (delim != NULL)
		*delim = temp_chr;
	if (num != NULL)
		*num = str_size;
	*ptr = ptr1;

	return 0;
}

int customio_get_till_delim(FILE *stream, const char *delims, char **ptr, int size, int *num) {
	int rv;
	int n = 0;
	char delim = '\0';
	char *ptr1 = *ptr;
	char *temp_ptr = NULL;

	/* get before delim */
	rv = __customio_get_before_delim(stream, delims, 0, &ptr1, size, &n, &delim, 0);
	if (rv)
		return rv;

	/* if EOF is met, return */
	if (delim == EOF) {
		goto ret;
	}

	/* else, resize memory if necessary and read the delimiter */
	if (n+2 > size) {
		temp_ptr = (char *)realloc(ptr1, n+2);
		if (temp_ptr == NULL) {
			free(ptr1);
			return CUSTOMIO_ALLOC_ERROR;
		}
		ptr1 = temp_ptr;
		ptr1[n] = fgetc(stream);
		if (ferror(stream)) {
			free(ptr1);
			return CUSTOMIO_READ_ERROR;
		}
		ptr1[n+1] = '\0';
	}

	/* save the results and return */
	ret:

	if (num != NULL)
		*num = n+1;
	*ptr = ptr1;

	return 0;
}

int customio_eat_ws(FILE *stream, int *count) {
	char temp = ' ';
	int temp_count = 0;

	/* eat whitespaces */
	while ((temp = fgetc(stream)) != EOF) {
		if (customio_is_ws(temp)) {
			temp_count++;
			continue;
		}
		fseek(stream, -1, SEEK_CUR);
		break;
	}

	/* check for read error */
	if (ferror(stream)) {
		return CUSTOMIO_READ_ERROR;
	}

	/* save result and return */
	if (count != NULL)
		*count = temp_count;

	return 0;
}

void customio_trim_before(char *str) {
	int i = 0, j = 0;

	/* find the first non-whitespace character */
	while (customio_is_ws(str[i]) && str[i] != '\0') {
		i++;
	}
	if (i == 0) {
		return;
	}
	if (str[i] == '\0') {
		str[0] = '\0';
		return;
	}

	/* rotate the string left */
	while(str[i] != '\0') {
		str[j] = str[i];
		i++;
		j++;
	}
	str[j] = '\0';
}

void customio_trim_after(char *str) {
	int i = strlen(str)-1;

	if (i == -1) {
		return;
	}

	/* find the first non-whitespace character from the right */
	while (customio_is_ws(str[i]) && i != -1) {
		i--;
	}
	/* end the string there */
	str[i+1] = '\0';
}

void customio_trim(char *str) {
	customio_trim_before(str);
	customio_trim_after(str);
}