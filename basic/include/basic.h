#ifndef _BASIC_H
#define _BASIC_H

/*
 * Contains general macros and utilities
 */

#ifndef NULL
#define NULL 0
#endif

#ifndef EOF
#define EOF -1
#endif

/**
 * error codes
 */
typedef enum basic_ec {
	BE_OK,
	BE_INVAL,
	BE_FAULT
} basic_ec;

/**
 * offset_of - determine the location offset of a member in a struct
 * @type:	the type of the container struct
 * @member:	the name of the member within the struct
 *
 */
#ifndef offset_of
#define offset_of(type, member) ((size_t) &((type *)0)->member)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member
 * @type:       the type of the container struct this is embedded in
 * @member:     the name of the member within the struct
 *
 */
#ifndef container_of
#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offset_of(type,member) ); })
#endif

/**
 * container_of_safe - same as container_of, but also takes care of NULL ptr
 * @ptr:        the pointer to the member
 * @type:       the type of the container struct this is embedded in
 * @member:     the name of the member within the struct
 *
 */
#ifndef container_of_safe
#define container_of_safe(ptr, type, member) ((type *)((ptr) ? container_of(ptr, type, member) : NULL))
#endif

/**
 * member_of - access the member of a structure at a certain offset
 * @ptr:		the pointer to the struct
 * @mtype:		the type of the member within the struct
 * @offset:		the offset of the member within the struct
 *
 */
#ifndef member_of
#define member_of(ptr, mtype, offset) ((mtype *)((char *)ptr + offset))
#endif

/**
 * member_of_safe - same as member_of, but also takes care of NULL ptr
 * @ptr:		the pointer to the struct
 * @mtype:		the type of the member within the struct
 * @offset:		the offset of the member within the struct
 *
 */
#ifndef member_of_safe
#define member_of_safe(ptr, mtype, offset) ((mtype *)((ptr) ? member_of(ptr, mtype, offset) : NULL))
#endif

/**
 * deref - safely dereference a pointer
 * @ptr:		the pointer to dereference
 *
 */
#ifndef deref
#define deref(ptr) ((ptr) ? (*(ptr)) : NULL)
#endif

/**
 * swap - swap 2 instances of the same type
 * @inst1:		instance number 1
 * @inst2:		instance number 2
 *
 */
#ifndef swap
#define swap(inst1, inst2) ({		\
	typeof(inst1) __tmp_inst;		\
	__tmp_inst = inst1; inst1 = inst2; inst2 = __tmp_inst; })
#endif

#endif
