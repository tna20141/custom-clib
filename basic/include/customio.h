#ifndef _CUSTOMIO_H
#define _CUSTOMIO_H

#include <stdio.h>

/*
 * Library for custom IO functions using file streams.
 *
 * This library deals with the problem of memory allocation and several problems concerning
 * delimiter and whitespace characters. All the pointers passed from external modules must be
 * freed explicitly by the users.
 *
 * Note: current version only supports ASCII character set
 */

/* the initial buffer size */
#define CUSTOMIO_INIT_BUF_SIZE 256

/* the buffer increment value */
#define CUSTOMIO_BUF_INC 256

/* enum type containing error codes */
enum customio_error {
	CUSTOMIO_READ_ERROR  = -2,
	CUSTOMIO_ALLOC_ERROR = -3
};

/*
 * customio_is_ws - check if a character is whitespace or not
 * @c: character to check
 * @return: 1 if c is whitespace, 0 otherwise
 */
static inline int customio_is_ws(char c);

/*
 * customio_get_before_ws - read a series of character until (and exclude) a whitespace
 * @stream: the input stream to read from
 * @ptr: the pointer to save the address of the read data
 * @num: the integer to save the number of characters read
 * @ws: the whitespace character that is met
 * @return: error code
 */
static inline int customio_get_before_ws(FILE *stream, char **ptr, int size, int *num, char *ws);

/*
 * customio_get_before_delim - read a series of character until (and exclude) a delimeter
 * @stream: the input stream to read from
 * @delims: string containing delimiter characters
 * @ptr: the pointer to save the address of the read data
 * @size: buffer size if ptr initially points to allocated data
 * @num: the integer to save the number of characters read
 * @delim: the delim that is met
 * @return: error code
 */
static inline int customio_get_before_delim(FILE *stream, const char *delims, char **ptr, int size, int *num, char *delim);

/*
 * customio_get_before_delim_or_ws - read a series of character until (and exclude) a delimeter or a whitespace
 * @stream: the input stream to read from
 * @delims: string containing delimiter characters
 * @ptr: the pointer to save the address of the read data
 * @size: buffer size if ptr initially points to allocated data
 * @num: the integer to save the number of characters read
 * @delim: the delim that is met
 * @return: error code
 */
static inline int customio_get_before_delim_or_ws(FILE *stream, const char *delims, char **ptr, int size, int *num, char *delim);

/*
 * customio_get_till_delim - read a series of character until (and include) a delimeter
 * @stream: the input stream to read from
 * @delims: string containing delimiter characters
 * @ptr: the pointer to save the address of the read data
 * @size: buffer size if ptr initially points to allocated data
 * @num: the integer to save the number of characters read
 * @return: error code
 */
int customio_get_till_delim(FILE *stream, const char *delims, char **ptr, int size, int *num);

/*
 * customio_get_before_ws - read a series of character until (and exclude) a whitespace, ignore leading whitespaces
 * @stream: the input stream to read from
 * @ptr: the pointer to save the address of the read data
 * @size: buffer size if ptr initially points to allocated data
 * @num: the integer to save the number of characters read
 * @ws: the whitespace character that is met
 * @return: error code
 */
static inline int customio_get_before_ws_ignore(FILE *stream, char **ptr, int size, int *num, char *ws);

/*
 * customio_get_before_delim - read a series of character until (and exclude) a delimeter, ignore leading whitespaces
 * @stream: the input stream to read from
 * @delims: string containing delimiter characters
 * @ptr: the pointer to save the address of the read data
 * @size: buffer size if ptr initially points to allocated data
 * @num: the integer to save the number of characters read
 * @delim: the delim that is met
 * @return: error code
 */
static inline int customio_get_before_delim_ignore(FILE *stream, const char *delims, char **ptr, int size, int *num, char *delim);

/*
 * customio_get_before_delim_or_ws - read a series of character until (and exclude) a delimeter or a whitespace, ignore leading whitespaces
 * @stream: the input stream to read from
 * @delims: string containing delimiter characters
 * @ptr: the pointer to save the address of the read data
 * @size: buffer size if ptr initially points to allocated data
 * @num: the integer to save the number of characters read
 * @delim: the delim that is met
 * @return: error code
 */
static inline int customio_get_before_delim_or_ws_ignore(FILE *stream, const char *delims, char **ptr, int size, int *num, char *delim);

/*
 * customio_eat_ws - eat a series of whitespace characters
 * @stream: the input stream to read from
 * @count: save the number of characters eaten
 * @return: error code
 */
int customio_eat_ws(FILE *stream, int *count);

/*
 * customio_trim_before - trim all leading whitespaces of a string
 * @str: the string to trim
 */
void customio_trim_before(char *str);

/*
 * customio_trim_before - trim all trailing whitespaces of a string
 * @str: the string to trim
 */
void customio_trim_after(char *str);

/*
 * customio_trim_before - trim all leading and trailing whitespaces of a string
 * @str: the string to trim
 */
void customio_trim(char *str);

/*
 * private functions
 */
int __customio_is_delim(char c, const char *delims, int ws);
int __customio_get_before_delim(FILE *stream, const char *delims, int ws, char **ptr, int size, int *num, char *delim, int ignore);

/*
 * inline function definitions
 */
static inline int customio_is_ws(char c) {
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

static inline int customio_get_before_ws(FILE *stream, char **ptr, int size, int *num, char *ws) {
	return __customio_get_before_delim(stream, "", 1, ptr, size, num, ws, 0);
}

static inline int customio_get_before_delim(FILE *stream, const char *delims, char **ptr, int size, int *num, char *delim) {
	return __customio_get_before_delim(stream, delims, 0, ptr, size, num, delim, 0);
}

static inline int customio_get_before_delim_or_ws(FILE *stream, const char *delims, char **ptr, int size, int *num, char *delim) {
	return __customio_get_before_delim(stream, delims, 1, ptr, size, num, delim, 0);
}

static inline int customio_get_before_ws_ignore(FILE *stream, char **ptr, int size, int *num, char *ws) {
	return __customio_get_before_delim(stream, "", 1, ptr, size, num, ws, 1);
}

static inline int customio_get_before_delim_ignore(FILE *stream, const char *delims, char **ptr, int size, int *num, char *delim) {
	return __customio_get_before_delim(stream, delims, 0, ptr, size, num, delim, 1);
}

static inline int customio_get_before_delim_or_ws_ignore(FILE *stream, const char *delims, char **ptr, int size, int *num, char *delim) {
	return __customio_get_before_delim(stream, delims, 1, ptr, size, num, delim, 1);
}

#endif