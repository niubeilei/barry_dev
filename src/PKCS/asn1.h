////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: asn1.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __BER_DEFINED

#define __BER_DEFINED

#include "CertVerify/common.h"
#include "PKCS/x509_object.h"
#include "PKCS/x509_util.h"

#define MKOID(value)	((BYTE *)value)

/* Special-case tags.  If DEFAULT_TAG is given the basic type (e.g. INTEGER,
   ENUMERATED) is used, otherwise the value is used as a context-specific
   tag.  If NO_TAG is given, processing of the tag is skipped.  If ANY_TAG
   is given, the tag is ignored */

#define DEFAULT_TAG			-1
#define NO_TAG				-2
#define ANY_TAG				-3

/* The maximum allowed size for an (encoded) object identifier */

#define ASN_MAX_OID_SIZE		32

/* Definitions for the ISO 8825:1990 Basic Encoding Rules */

/* Tag class */

#define BER_UNIVERSAL			0x00
#define BER_APPLICATION			0x40
#define BER_CONTEXT_SPECIFIC		0x80
#define BER_PRIVATE			0xC0

/* Whether the encoding is constructed or primitive */

#define BER_CONSTRUCTED			0x20
#define BER_PRIMITIVE			0x00


/* The ID's for universal tag numbers 0-31.  Tag number 0 is reserved for
   encoding the end-of-contents value when an indefinite-length encoding
   is used
 */
enum {
	BER_ID_RESERVED = 0,
	BER_ID_BOOLEAN,
	BER_ID_INTEGER,
	BER_ID_BITSTRING,
	BER_ID_OCTETSTRING,
	BER_ID_NULL,			//5
	BER_ID_OBJECT_IDENTIFIER,
	BER_ID_OBJECT_DESCRIPTOR,
	BER_ID_EXTERNAL,
	BER_ID_REAL,
	BER_ID_ENUMERATED,		//10
	BER_ID_EMBEDDED_PDV,
	BER_ID_STRING_UTF8,
	BER_ID_13,
	BER_ID_14,
	BER_ID_15,			//15
	BER_ID_SEQUENCE,
	BER_ID_SET,
	BER_ID_STRING_NUMERIC,
	BER_ID_STRING_PRINTABLE,
	BER_ID_STRING_T61,		//20
	BER_ID_STRING_VIDEOTEX,
	BER_ID_STRING_IA5,
	BER_ID_TIME_UTC,
	BER_ID_TIME_GENERALIZED,
	BER_ID_STRING_GRAPHIC,		//25
	BER_ID_STRING_ISO646,
	BER_ID_STRING_GENERAL,
	BER_ID_STRING_UNIVERSAL,
	BER_ID_29,
	BER_ID_STRING_BMP,		//30
	BER_ID_LAST			//31
};

/* The encodings for the universal types */

#define BER_EOC				0	/* Pseudo-type for first EOC octet */
#define BER_RESERVED		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_RESERVED )
#define BER_BOOLEAN		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_BOOLEAN )
#define BER_INTEGER		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_INTEGER )
#define BER_BITSTRING		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_BITSTRING )
#define BER_OCTETSTRING		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_OCTETSTRING )
#define BER_NULL		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_NULL )
#define BER_OBJECT_IDENTIFIER	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_OBJECT_IDENTIFIER )
#define BER_OBJECT_DESCRIPTOR	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_OBJECT_DESCRIPTOR )
#define BER_EXTERNAL		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_EXTERNAL )
#define BER_REAL		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_REAL )
#define BER_ENUMERATED		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_ENUMERATED )
#define BER_EMBEDDED_PDV	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_EMBEDDED_PDV )
#define BER_STRING_UTF8		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_UTF8 )
#define BER_13			( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_13 )
#define BER_14			( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_14 )
#define BER_15			( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_15 )
#define BER_SEQUENCE		( BER_UNIVERSAL | BER_CONSTRUCTED | BER_ID_SEQUENCE )
#define BER_SET			( BER_UNIVERSAL | BER_CONSTRUCTED | BER_ID_SET )
#define BER_STRING_NUMERIC	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_NUMERIC )
#define BER_STRING_PRINTABLE	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_PRINTABLE )
#define BER_STRING_T61		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_T61 )
#define BER_STRING_VIDEOTEX	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_VIDEOTEX )
#define BER_STRING_IA5		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_IA5 )
#define BER_TIME_UTC		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_TIME_UTC )
#define BER_TIME_GENERALIZED	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_TIME_GENERALIZED )
#define BER_STRING_GRAPHIC	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_GRAPHIC )
#define BER_STRING_ISO646	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_ISO646 )
#define BER_STRING_GENERAL	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_GENERAL )
#define BER_STRING_UNIVERSAL	( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_UNIVERSAL )
#define BER_29			( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_BER29 )
#define BER_STRING_BMP		( BER_UNIVERSAL | BER_PRIMITIVE | BER_ID_STRING_BMP )

/* The encodings for constructed, indefinite-length tags and lengths */

#define BER_OCTETSTRING_INDEF	( ( BYTE * ) "\x24\x80" )
#define BER_SEQUENCE_INDEF	( ( BYTE * ) "\x30\x80" )
#define BER_SET_INDEF		( ( BYTE * ) "\x31\x80" )
#define BER_CTAG0_INDEF		( ( BYTE * ) "\xA0\x80" )
#define BER_END_INDEF		( ( BYTE * ) "\x00\x00" )

/* Masks to extract information from a tag number */

#define BER_CLASS_MASK			0xC0
#define BER_CONSTRUCTED_MASK		0x20
#define BER_SHORT_ID_MASK		0x1F

/* The maximum size for the short tag number encoding, and the magic value
   which indicates that a long encoding of the number is being used */

#define MAX_SHORT_BER_ID		30
#define LONG_BER_ID			0x1F

/* Turn an identifier into a context-specific tag, and extract the value from
   a tag.  Normally these are constructed, but in a few special cases they
   are primitive */

#define MAKE_CTAG(identifier) \
		(BER_CONTEXT_SPECIFIC | BER_CONSTRUCTED | (identifier))
#define MAKE_CTAG_PRIMITIVE( identifier ) \
		(BER_CONTEXT_SPECIFIC | (identifier))
#define EXTRACT_CTAG(tag) \
		((tag) & ~(BER_CONTEXT_SPECIFIC | BER_CONSTRUCTED))


/***************************************************************************/
typedef struct {
	int			len;
	BYTE		     *	data;
}	asn_bin_t;

#define ASN_SMALL_DATA_LEN	32
typedef struct {
	int 			len;
	BYTE			data[ASN_SMALL_DATA_LEN];
}	asn_data_t;

typedef struct {
	const BYTE * oid;		/* OID */
	const int    id;		/* Value to return for this OID */
	const void * extra_info;	/* Additional info for this selection */
} OID_INFO;

#define sizeof_null()			(sizeof(BYTE) + sizeof(BYTE))
#define sizeof_oid(oid)			(1 + 1 + (int)oid[1])
#define sizeof_UTC_time()		(1 + 1 + 13)
#define sizeof_generalized_time()	(1 + 1 + 15)

#define OK_SPECIAL		-4321
#define ASN_UNUSED		-11

#define MAX_INTLENGTH_DELTA	1048576
#define MAX_INTLENGTH		(INT_MAX - MAX_INTLENGTH_DELTA)

/* The maximum public-key component size - 4096 bits */
#define ASN_MAX_PKCSIZE		512

/***************************************************************************/

/* Decode Routines for Primitive Objects */
extern int asn_decode_integer_header(DATA_OBJECT *object, const int tag);
extern BOOLEAN asn_check_EOC(DATA_OBJECT *object);
extern int asn_decode_raw_object_tag(DATA_OBJECT *object, BYTE *buffer, int *buffer_length, const int max_length, const int tag);
extern int asn_decode_integer_tag(DATA_OBJECT *object, BYTE *integer, int *integer_length, const int max_length, const int tag);
extern int asn_decode_bignum_tag(DATA_OBJECT *object, asn_bin_t *bignum, const int tag);
extern int asn_decode_universal_data(DATA_OBJECT *object);
extern int asn_decode_universal(DATA_OBJECT *object);
extern int asn_decode_short_integer_tag(DATA_OBJECT *object, long *value, const int tag);
extern int asn_decode_enumerated_tag(DATA_OBJECT *object, int *enumeration, const int tag);
extern int asn_decode_null_tag(DATA_OBJECT *object, const int tag);
extern int asn_decode_boolean_tag(DATA_OBJECT *object, BOOLEAN *boolean, const int tag);
extern int asn_decode_oid_ex(DATA_OBJECT *object, const OID_INFO *oid_selection, const OID_INFO **oid_selection_value);
extern int asn_decode_oid(DATA_OBJECT *object, const OID_INFO *oid_info, int *selection_id);
extern int asn_decode_fixed_oid(DATA_OBJECT *object, const BYTE *oid);
extern int asn_decode_octet_string_tag(DATA_OBJECT *object, BYTE *string, int *string_length, const int max_length, const int tag);
extern int asn_decode_character_string(DATA_OBJECT *object, BYTE *string, int *string_length, const int max_length, const int tag);
extern int asn_decode_bitstring_tag(DATA_OBJECT *object, int *bitstring, const int tag);
extern int asn_decode_UTC_time_tag(DATA_OBJECT *object, time_t *tm , const int tag);
extern int asn_decode_generalized_time_tag(DATA_OBJECT *object, time_t *tm, const int tag);

/* Decode Routines for Constructed Objects */
extern int asn_decode_sequence(DATA_OBJECT *object, int *length);
extern int asn_decode_sequenceI(DATA_OBJECT *object, int *length);
extern int asn_decode_set(DATA_OBJECT *object, int *length);
extern int asn_decode_setI(DATA_OBJECT *object, int *length);
extern int asn_decode_constructed(DATA_OBJECT *object, int *length, const int tag);
extern int asn_decode_constructedI(DATA_OBJECT *object, int *length, const int tag);
extern int asn_decode_octet_string_hole(DATA_OBJECT *object, int *length, const int tag);
extern int asn_decode_bitstring_hole(DATA_OBJECT *object, int *length, const int tag);
extern int asn_decode_generic_hole(DATA_OBJECT *object, int *length, const int tag);
extern int asn_decode_generic_holeI(DATA_OBJECT *object, int *length, const int tag);
extern int asn_decode_long_sequence(DATA_OBJECT *object, long *length);
extern int asn_decode_long_constructed(DATA_OBJECT *object, long *length, const int tag);
extern int asn_decode_long_generic_hole(DATA_OBJECT *object, long *length, const int tag);

/* macro for decoding object */

#define asn_get_tag(object)		asn_getc(object)
#define asn_peek_tag(object)		asn_peek(object)

#define asn_decode_integer_data(object, integer, integer_length, max_length)	\
		asn_decode_integer_tag(object, integer, integer_length, max_length, NO_TAG)
#define asn_decode_integer(object, integer, integer_length, max_length )	\
		asn_decode_integer_tag(object, integer, integer_length, max_length, DEFAULT_TAG)
#define asn_decode_enumerated_data(object, enumeration) \
		asn_decode_enumerated_tag(object, enumeration, NO_TAG)
#define asn_decode_enumerated(object, enumeration) \
		asn_decode_enumerated_tag(object, enumeration, DEFAULT_TAG)
#define asn_decode_boolean_data(object, boolean) \
		asn_decode_boolean_tag(object, boolean, NO_TAG)
#define asn_decode_boolean(object, boolean) \
		asn_decode_boolean_tag(object, boolean, DEFAULT_TAG)
#define asn_decode_null_data(object) \
		asn_decode_null_tag(object, NO_TAG)
#define asn_decode_null(object) \
		asn_decode_null_tag(object, DEFAULT_TAG)
#define asn_decode_octet_string_data(object, string, string_length, max_length) \
		asn_decode_octet_string_tag(object, string, string_length, max_length, NO_TAG)
#define asn_decode_octet_string(object, string, string_length, max_length) \
		asn_decode_octet_string_tag(object, string, string_length, max_length, DEFAULT_TAG)
#define asn_decode_bitstring_data(object, bitString ) \
		asn_decode_bitstring_tag(object, bitstring, NO_TAG )
#define asn_decode_bitstring(object, bitString ) \
		asn_decode_bitstring_tag(object, bitstring, DEFAULT_TAG)
#define asn_decode_raw_object(object, buffer, buffer_length, max_length, tag ) \
		asn_decode_raw_object_tag(object, buffer, buffer_length, max_length, tag)
#define asn_decode_raw_object_data(object, buffer, buffer_length, max_length) \
		asn_decode_raw_object_tag(object, buffer, buffer_length, max_length, NO_TAG)
#define asn_decode_UTC_time_data(object, time)	asn_decode_UTC_time_tag(object, time, NO_TAG)
#define asn_decode_UTC_time(object, time)	asn_decode_UTC_time_tag(object, time, DEFAULT_TAG)
#define asn_decode_generalized_time_data(object, time)	\
		asn_decode_generalized_time_tag(object, time, NO_TAG)
#define asn_decode_generalized_time(object, time)	\
		asn_decode_generalized_time_tag(object, time, DEFAULT_TAG)
#define asn_decode_bignum(object, bignum) \
		asn_decode_bignum_tag(object, bignum, DEFAULT_TAG)
#define asn_decode_short_integer_data(object, integer)	\
		asn_decode_short_integer_tag(object, integer, NO_TAG)
#define asn_decode_short_integer(object, integer)	\
		asn_decode_short_integer_tag(object, integer, DEFAULT_TAG)


#define asn_encode_tag(object, tag)	asn_putc(object, tag)

/* Sizeof Routines */
extern long asn_peek_object_length(DATA_OBJECT *object);
extern long asn_sizeof_object(const long length);
extern int asn_signed_bignum_size(const void *bignum);

/* Encode Routines for Primitive Objects */
extern int asn_encode_short_integer(DATA_OBJECT *object, const long integer, const int tag);
extern int asn_encode_integer(DATA_OBJECT *object, const BYTE *integer,
				const int integer_length, const int tag);
extern int asn_encode_bignum_tag(DATA_OBJECT *object, const asn_bin_t *bignum, const int tag);
extern int asn_encode_enumerated(DATA_OBJECT *object, const int enumerated, const int tag);
extern int asn_encode_null(DATA_OBJECT *object, const int tag);
extern int asn_encode_boolean(DATA_OBJECT *object, const BOOLEAN boolean, const int tag);
extern int asn_encode_octet_string(DATA_OBJECT *object, const BYTE *string, const int length,
					  const int tag);
extern int asn_encode_character_string(DATA_OBJECT *object, const BYTE *string,
				const int length, const int tag);
extern int asn_encode_bitstring(DATA_OBJECT *object, const int bitstring, const int tag);
extern int asn_encode_UTC_time(DATA_OBJECT *object, const UINT32 t, const int tag);
extern int asn_encode_generalized_time(DATA_OBJECT *object, const UINT32 t, const int tag);

/* Encode Routines for  Constructed Objects */
extern int asn_encode_sequence(DATA_OBJECT *object, const int length);
extern int asn_encode_set(DATA_OBJECT *object, const int length);
extern int asn_encode_constructed(DATA_OBJECT *object, const int length, const int tag);
extern int asn_encode_octet_string_hole(DATA_OBJECT *object, const int length, const int tag);
extern int asn_encode_bitstring_hole(DATA_OBJECT *object, const int length, const int tag);
extern int asn_encode_generic_hole(DATA_OBJECT *object, const int length, const int tag);

/* object util routine */
extern DATA_OBJECT * asn_merge_objects(DATA_OBJECT **object1, DATA_OBJECT **object2, int tag);

/* Macros for encode object */
#define asn_encode_octet_string_indef(object)	asn_write(object, BER_OCTETSTRING_INDEF, 2)
#define asn_encode_sequence_indef(object)	asn_write(object, BER_SEQUENCE_INDEF, 2)
#define asn_encode_set_indef(object)		asn_write(object, BER_SET_INDEF, 2)
#define asn_encode_ctag0_indef(object)		asn_write(object, BER_CTAG0_INDEF, 2)
#define asn_encode_end_indef(object)		asn_write(object, BER_END_INDEF, 2)

#define asn_encode_raw_object(object, buffer, size) \
			asn_write(object, buffer, size)

#define asn_encode_oid(object, oid) \
			asn_write((object), (oid), sizeof_oid(oid))
#define asn_sizeof_bignum(bignum) \
		((int)sizeof_object(asn_signed_bignum_size(bignum)))
#define asn_encode_bignum(object, bignum) \
		asn_encode_bignum_tag(object, bignum, DEFAULT_TAG)

#define asn_sizeof_short_integer(value)	\
	(((value) < 128) ? 3 : \
	  (((long) value) < 32768L ) ? 4 : \
	  (((long) value) < 8388608L ) ? 5 : \
	  (((long) value) < 2147483648UL ) ? 6 : 7)

#define asn_sizeof_bitstring(value)	\
	(3 + ((((long) value) > 0xFFFFFFL ) ? 4 : \
			(((long) value) > 0xFFFFL ) ? 3 : \
			((value) > 0xFF) ? 2 : (value) ? 1 : 0))


#define asn_sizeof_UTC_time()			(1 + 1 + 13)
#define asn_sizeof_generalized_time()		(1 + 1 + 15)



/*
static inline void x509_printc(BYTE *data, int length) {
	int i;

	for (i=0;i<length;i++) printf("%c", (char)data[i]);
	printf("\n");
}

static inline void x509_printx(BYTE *data, int length) {
	int i;

	for (i=0;i<length;i++) printf("%02x ", data[i]);
	printf("\n");
}

static inline void x509_printx_format(BYTE *data, int length, char *format) {
	int i;

	for (i=0;i<length;i++) {
		if (i % 16 == 0) {
			if (i == 0)
				printf("%s", format);
			else
				printf("\n%s", format);
		}
		printf("%02x ", data[i]);
	}
	printf("\n");
}
*/

#endif //end of __BER_DEFINED
