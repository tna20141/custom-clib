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

/*
 * customio_is_ws - check if a character is whitespace or not
 * @c: character to check
 * @return: 1 if c is whitespace, 0 otherwise
 */
int customio_is_ws(char c);

/*
 * customio_get_line - read a line
 * @stream: the input stream to read from
 * @ptr: the pointer to save the address of the read data
 * @num: the integer to save the number of characters read
 * @return: error code
 */
int customio_get_line(FILE* stream, char** ptr, int* num);

/*
 * customio_get_before_ws - read a series of character until (and exclude) a whitespace
 * @stream: the input stream to read from
 * @ptr: the pointer to save the address of the read data
 * @num: the integer to save the number of characters read
 * @return: error code
 */
int customio_get_before_ws(FILE* stream, char** ptr, int* num);

/*
 * customio_get_before_delim - read a series of character until (and exclude) a delimeter
 * @stream: the input stream to read from
 * @delims: NULL terminated list of delimiter characters
 * @ptr: the pointer to save the address of the read data
 * @num: the integer to save the number of characters read
 * @delim: the delim that is met
 * @return: error code
 */
int customio_get_before_delim(FILE* stream, const char* delims, char** ptr, int* num, char* delim);

/*
 * customio_get_till_delim - read a series of character until (and include) a delimeter
 * @stream: the input stream to read from
 * @delims: NULL terminated list of delimiter characters
 * @ptr: the pointer to save the address of the read data
 * @num: the integer to save the number of characters read
 * @delim: the delim that is met
 * @return: error code
 */
int customio_get_till_delim(FILE* stream, const char* delims, char** ptr, int* num, char* delim);

/*
 * customio_eat_ws - eat a series of whitespace characters
 * @stream: the input stream to read from
 * @return: error code
 */
int customio_eat_ws(FILE* stream);

/*
 * customio_get_line - eat a series of whitespace characters (error if none is found)
 * @stream: the input stream to read from
 * @return: error code
 */
int customio_eat_ws_must(FILE* stream);

#endif