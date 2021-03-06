#ifndef _BASIC_GENERAL_H
#define _BASIC_GENERAL_H

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
	(type *)( (char *)__mptr - OFFSET_OF(type,member) ); })
#endif

/**
 * CONTAINER_OF_SAFE - same as CONTAINER_OF, but also takes care of NULL ptr
 * @ptr:        the pointer to the member
 * @type:       the type of the container struct this is embedded in
 * @member:     the name of the member within the struct
 *
 */
#ifndef CONTAINER_OF_SAFE
#define CONTAINER_OF_SAFE(ptr, type, member) ((type *)((ptr) ? CONTAINER_OF(ptr, type, member) : NULL))
#endif

/**
 * MEMBER_OF - access the member of a structure at a certain offset
 * @ptr:		the pointer to the struct
 * @mtype:		the type of the member within the struct
 * @offset:		the offset of the member within the struct
 *
 */
#ifndef MEMBER_OF
#define MEMBER_OF(ptr, mtype, offset) ((mtype *)((char *)ptr + offset))
#endif

/**
 * MEMBER_OF_SAFE - same as MEMBER_OF, but also takes care of NULL ptr
 * @ptr:		the pointer to the struct
 * @mtype:		the type of the member within the struct
 * @offset:		the offset of the member within the struct
 *
 */
#ifndef MEMBER_OF_SAFE
#define MEMBER_OF_SAFE(ptr, mtype, offset) ((mtype *)((ptr) ? MEMBER_OF(ptr, mtype, offset) : NULL))
#endif

/**
 * DEREF - safely dereference a pointer
 * @ptr:		the pointer to dereference
 *
 */
#ifndef DEREF
#define DEREF(ptr) ((ptr) ? (*(ptr)) : NULL)
#endif

/**
 * SWAP - swap 2 instances of the same type
 * @inst1:		instance number 1
 * @inst2:		instance number 2
 *
 */
#ifndef SWAP
#define SWAP(inst1, inst2) ({		\
	typeof(inst1) __tmp_inst;		\
	__tmp_inst = inst1; inst1 = inst2; inst2 = __tmp_inst; })
#endif

#endif