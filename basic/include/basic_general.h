#ifndef _BASIC_GENERAL_H
#define _BASIC_GENERAL_H

#ifndef NULL
#define NULL 0
#endif

#ifndef EOF
#define EOF -1
#endif

/**
 * OFFSET_OF - determine the location offset of a member in a struct
 * @TYPE:	the type of the container struct
 * @MEMBER:	the name of the member within the struct
 *
 */
#ifndef OFFSET_OF
#define OFFSET_OF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * CONTAINER_OF - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member
 * @type:       the type of the container struct this is embedded in
 * @member:     the name of the member within the struct
 *
 */
#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - OFFSET_OF(type,member) );})
#endif

#endif
