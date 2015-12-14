////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: common.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __SYSTEM_DEFINED
#define __SYSTEM_DEFINED

#include "KernelSimu/string.h"


#ifdef __KERNEL__
/*
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
*/
#else
#include <stdlib.h>
#endif

#ifndef TRUE
	#define FALSE		0
	#define TRUE		(!FALSE)
#endif

typedef unsigned char		BYTE;
typedef int			BOOLEAN;

typedef unsigned short		WORD;
typedef unsigned long 		UINT32;
typedef unsigned int		UINT;


#ifdef __KERNEL__
	typedef unsigned short int	wchar_t;
//	#define 		printf	printk
#endif

#define WCSIZE			(sizeof(wchar_t))

#define ASCII_ALPHA		0x01
#define ASCII_LOWER		0x02
#define ASCII_NUMERIC		0x04
#define ASCII_SPACE		0x08
#define ASCII_UPPER		0x10
#define ASCII_HEX		0x20

extern const BYTE 		ascii_c_type_tbl[];

#define isAlnum(ch)		(ascii_c_type_tbl[ch] & (ASCII_ALPHA | ASCII_NUMERIC))
#define isAlpha(ch)		(ascii_c_type_tbl[ch] & ASCII_ALPHA)
#define isDigit(ch)		(ascii_c_type_tbl[ch] & ASCII_NUMERIC)
#define isPrint(ch)		((ch) >= 0x20 && (ch) <= 0x7E)
#define isXDigit(ch)		(ascii_c_type_tbl[ch] & ASCII_HEX)
#define toLower( ch )		((ascii_c_type_tbl[ch] & ASCII_UPPER) ? (ch) + 32 : (ch))
#define toUpper(ch)		((ascii_c_type_tbl[ch] & ASCII_LOWER) ? (ch) - 32 : (ch))

typedef enum {
	STRINGTYPE_NONE,				/* No string type */

	/* 8-bit character types */
	STRINGTYPE_PRINTABLE,				/* PrintableString */
	STRINGTYPE_IA5,					/* IA5String */
		STRINGTYPE_VISIBLE = STRINGTYPE_IA5,	/* VisibleString */
							/* VisibleString as Unicode */
	STRINGTYPE_T61,					/* T61 (8859-1) string */

	/* 8-bit types masquerading as Unicode */
	STRINGTYPE_UNICODE_PRINTABLE,			/* PrintableString as Unicode */
	STRINGTYPE_UNICODE_IA5,				/* IA5String as Unicode */
		STRINGTYPE_UNICODE_VISIBLE = STRINGTYPE_UNICODE_IA5,
	STRINGTYPE_UNICODE_T61,				/* 8859-1 as Unicode */

	/* Unicode/UTF-8 */
	STRINGTYPE_UNICODE,				/* Unicode string */
	STRINGTYPE_UTF8					/* UTF-8 string */
} ASN1_STRINGTYPE;


#ifdef __KERNEL__
	#define ASN_ERROR( fmt,x... ) \
		printk(KERN_ERR"[%s][%s](%d):"fmt,__FILE__,__FUNCTION__,__LINE__, ##x)

	#define zassert(expr) \
		if(!(expr)) {					\
	        	printk( "Assertion failed! %s,%s,%s,line=%d\n",	\
	        		#expr,__FILE__,__FUNCTION__,__LINE__);		\
	        }
#else
	#define ASN_ERROR(fmt, x...) \
		fprintf(stderr, "[%s][%s](%d):"fmt,__FILE__,__FUNCTION__,__LINE__, ##x)
	#include <assert.h>

#endif

static inline int is_space(BYTE c) {
	if (c == ' '  ||
	    c == '\f' ||
	    c == '\n' ||
	    c == '\r' ||
	    c == '\t' ||
	    c == '\v')
		return TRUE;
	else
		return FALSE;
}

static inline int is_digit(BYTE c) {
	if (c >= '0' && c <= '9')
		return TRUE;
	else
		return FALSE;
}


#define DECLARE_VARSTRUCT_VARS \
		int storage_size; \
		BYTE storage[1]

#endif
