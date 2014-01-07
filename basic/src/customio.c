#include <stdlib.h>
#include <string.h>
#include "customio.h"

int __customio_is_delim(char c, const char *delims, int ws) {
	int i = 0;

	if (ws) {
		if (customio_is_ws(c))
			return 1;
	}

	while (delims[i] != NULL) {
		if (c == delims[i])
			return 1;
		i++;
	}
}

int __customio_get_before_delim(FILE *stream, const char *delims, int ws, char **ptr, int *num, char *delim) {
	char *ptr1 = NULL;
	char *temp_ptr = NULL;
	char temp_chr = 0;
	int buf_size = CUSTOMIO_INIT_BUF_SIZE;
	int str_size = 0;

	/* initial allocation */
	ptr1 = (char *)malloc(buf_size);
	temp_ptr = ptr1;

	/* continue until EOF or a delimiter is reached */
	while ((temp_chr = fgetc(stream)) != EOF) {
		/* if a delimiter is reached, stop reading */
		if (__customio_is_delim(temp_chr, delims, ws)) {
			fseek(stream, -1, SEEK_CUR);
			break;
		}

		/* if the buffer is full, expand the buffer */
		if (str_size+1 >= buf_size) {
			buf_size += CUSTOMIO_BUF_INC;
			temp_ptr = (char *)realloc(ptr1, buf_size);
			if (temp_ptr == NULL) {
				free(ptr1);
				return CUSTOMIO_ALLOC_ERROR;
			}
			ptr1 = temp_ptr;
		}

		/* store the character and move on */
		ptr1[str_size] = temp_chr;
		str_size++;
	}

	/* check for read error */
	if (ferror(stream)) {
		free(ptr1);
		return CUSTOMIO_READ_ERROR;
	}

	/* reallocate one last time to save space */
	temp_ptr = realloc(temp_ptr, str_size+1);
	if (temp_ptr == NULL) {
		free(ptr1);
		return CUSTOMIO_ALLOC_ERROR;
	}
	ptr1 = temp_ptr;

	/* save the results and return */
	ptr1[str_size] = '\0';
	*delim = temp_chr;
	*num = str_size;
	*ptr = ptr1;

	return 0;
}

int customio_get_till_delim(FILE *stream, const char *delims, char **ptr, int *num) {
	char *ptr1 = NULL;
	char *temp_ptr = NULL;
	char temp_chr = 0;
	int buf_size = CUSTOMIO_INIT_BUF_SIZE;
	int str_size = 0;

	/* initial allocation */
	ptr1 = (char *)malloc(buf_size);
	temp_ptr = ptr1;

	/* continue until EOF or a delimiter is reached */
	while ((temp_chr = fgetc(stream)) != EOF) {
		/* if the buffer is full, expand the buffer */
		if (str_size+1 >= buf_size) {
			buf_size += CUSTOMIO_BUF_INC;
			temp_ptr = (char *)realloc(ptr1, buf_size);
			if (temp_ptr == NULL) {
				free(ptr1);
				return CUSTOMIO_ALLOC_ERROR;
			}
			ptr1 = temp_ptr;
		}

		/* store the character */
		ptr1[str_size] = temp_chr;
		str_size++;

		/* if a delimiter is reached, stop reading */
		if (__customio_is_delim(temp_chr)) {
			break;
		}
	}

	/* check for read error */
	if (ferror(stream)) {
		free(ptr1);
		return CUSTOMIO_READ_ERROR;
	}

	/* reallocate one last time to save space */
	temp_ptr = realloc(temp_ptr, str_size+1);
	if (temp_ptr == NULL) {
		free(ptr1);
		return CUSTOMIO_ALLOC_ERROR;
	}
	ptr1 = temp_ptr;

	/* save the results and return */
	ptr1[str_size] = '\0';
	*num = str_size;
	*ptr = ptr1;

	return 0;
}

void customio_trim_before(char *str) {
	int i = 0, j = 0;

	/* find the first non-whitespace character */
	while (customio_is_ws(str[i]) && str[i] != '\0') {
		i++;
	}
	if (i == 0) {
		return 0;
	}
	if (str[i] == '\0') {
		str[0] = '\0';
		return 0;
	}

	/* rotate the string left */
	while(str[i] != '\0') {
		str[j] = str[i];
		i++;
		j++;
	}
	str[j] = '\0';

	return 0;
}

void customio_trim_after(char *str) {
	int i = strlen(str)-1;

	if (i == -1) {
		return 0;
	}

	/* find the first non-whitespace character from the right */
	while (customio_is_ws(str[i]) && i != -1) {
		i--;
	}
	/* end the string there */
	str[i+1] = '\0';

	return 0;
}

void customio_trim(char *str) {
	customio_trim_before(str);
	customio_trim_after(str);
	return 0;
}