#ifndef _CUSTOM_CLIB_UTILS_H
#define _CUSTOM_CLIB_UTILS_H

/**
 * offsetof - determine the location offset of a member in a struct
 * @TYPE:	the type of the container struct
 * @MEMBER:	the name of the member within the struct
 *
 */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member
 * @type:       the type of the container struct this is embedded in
 * @member:     the name of the member within the struct
 *
 */
#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

#ifndef NULL
#define NULL 0
#endif

#endif
