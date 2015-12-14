////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: asn1_encode.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                                                                           *
*                    ASN.1 encode Routines                                  *
*                                                                           *
*****************************************************************************/


#ifdef __KERNEL__

#include <linux/kernel.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#endif

#include "PKCS/asn1.h"
#include "PKCS/x509.h"
#include "PKCS/x509_util.h"




/****************************************************************************
 *                   ASN1 encode Routines                                   *
 ****************************************************************************/

/* Calculate the size of the encoded length octets */
static inline int
asn_length_size(const long length) {
	if(length <= 0x7F)
		return 1;

	/* max size is 4 */
	if (length <= 0xFF)
		return 2;
	if (length <= 0xFFFFL )
		return 3;
	return 1 + ((length > 0xFFFFFFL) ? 4 : 3);
}

static int asn_length_octets(const long length) {
	return ( length <= 0xFF ) ? 1 : \
		   ( length <= 0xFFFFL ) ? 2 : \
		   ( length <= 0xFFFFFFL ) ? 3 : 4;
}
/* Encode the length octets for an ASN.1 item */
static int
asn_encode_length(DATA_OBJECT *object, const long length) {
	BYTE buffer[8];
	const int octets_of_length = asn_length_octets(length);
	int pos = 1;


	/* Use the short form of the length octets if possible */
	if (length <= 0x7f)
		return asn_putc(object, (BYTE)length);

	/* Encode the number of length octets followed by the octets themselves */
	buffer[0] = 0x80 | octets_of_length;
	if (octets_of_length > 3)
		buffer[pos++] = (BYTE)(length >> 24);
	if (octets_of_length > 2)
		buffer[pos++] = (BYTE)(length >> 16);
	if (octets_of_length > 1)
		buffer[pos++] = (BYTE)(length >> 8);
	buffer[pos++] = (BYTE)length;

	return asn_write(object, buffer, pos);
}

/* Encode a numeric value represented by integer */
static int
asn_encode_numeric(DATA_OBJECT *object, const long integer) {
	BYTE buffer[16];
	long int_value = integer;
	u32 length = 0; 
	int i;

	/* The value 0 is handled specially */
	if (int_value == 0)
		return asn_write(object, "\x01\x00", 2);

	/* Assemble the encoded value in little-endian order */
	if (int_value > 0) {
		while(int_value > 0) {
			buffer[length++] = int_value & 0xFF;
			int_value >>= 8;
		}

		/* Make sure that we don't inadvertently set the sign bit if  the
		   high bit of the value is set */
		if (buffer[length - 1] & 0x80)
			buffer[length++] = 0x00;
	} else	{
		do {
			buffer[length++] = int_value & 0xFF;
			int_value >>= 8;
		} while(int_value != -1 && length < sizeof(int));

		/* Make sure that we don't inadvertently clear the sign bit if  the
		   high bit of the value is clear */
		if (!(buffer[length - 1] & 0x80))
			buffer[length++] = 0xFF;
	}

	/* Output the value in reverse (big-endian)order */
	asn_putc(object, length);
	for (i = length - 1; i > 0; i--)
		asn_putc(object, buffer[i]);

	return asn_putc(object, buffer[0]);
}

/****************************************************************************
 *            Sizeof Routines                                               *
 ****************************************************************************/


/* Determine the encoded size of an object given only a length.  */
long
asn_sizeof_object(const long length) {
	return (length < 0) ? length : sizeof(BYTE)+ asn_length_size(length)+ length;
}

/* Determine the size of a bignum.  When we're writing these we can't use
   sizeof_object()directly because the internal representation is unsigned
   whereas the encoded form is signed */

int asn_signed_bignum_size(const void *bignum) {
	asn_bin_t * bn = (asn_bin_t *)bignum;
	return bn->len;
}

/****************************************************************************
 *           Encode Routines for Primitive Objects                          *
 ****************************************************************************/

/* Encode a short/large/bignum integer value */
int
asn_encode_short_integer(DATA_OBJECT *object, const long integer, const int tag) {
	int ctag;
	int ret;

	ctag = (tag == DEFAULT_TAG) ? BER_INTEGER : BER_CONTEXT_SPECIFIC | tag;
	ret = asn_encode_tag(object, ctag);
	if (ret == 0)
		ret = asn_encode_numeric(object, integer);
	return ret;
}

int
asn_encode_integer(DATA_OBJECT *object, const BYTE *integer,
			const int integer_length, const int tag) {
	const BOOLEAN is_leading_zero = integer_length && ((*integer & 0x80)? 1 : 0);
	int ctag;
	int ret;

	ctag = (tag == DEFAULT_TAG) ? BER_INTEGER : BER_CONTEXT_SPECIFIC | tag;
	ret = asn_encode_tag(object, ctag);

	if (ret == 0)
		ret = asn_encode_length(object, integer_length + is_leading_zero);
	if (ret == 0 && is_leading_zero)
		ret = asn_putc(object, 0);
	if (ret == 0)
		ret =  asn_write(object, integer, integer_length);
	return ret;
}

int
asn_encode_bignum_tag(DATA_OBJECT *object, const asn_bin_t *bn, const int tag) {
	if (!bn || !bn->data || bn->len <= 0)
		return -1;

	return asn_encode_integer(object, bn->data, bn->len, tag);
}

/* Encode an enumerated value */
int
asn_encode_enumerated(DATA_OBJECT *object, const int enumerated, const int tag) {
	int ctag;
	int ret;

	ctag = (tag == DEFAULT_TAG) ? BER_ENUMERATED : BER_CONTEXT_SPECIFIC | tag;
	ret = asn_encode_tag(object, ctag);

	if (ret == 0)
		ret = asn_encode_numeric(object, (long)enumerated);

	return ret;
}

/* Encode a null value */
int asn_encode_null(DATA_OBJECT *object, const int tag) {
	BYTE buffer[8];

	buffer[0] = (tag == DEFAULT_TAG) ? BER_NULL : BER_CONTEXT_SPECIFIC | tag;
	buffer[1] = 0;

	return asn_write(object, buffer, 2);
}

/* Encode a boolean value */
int
asn_encode_boolean(DATA_OBJECT *object, const BOOLEAN boolean, const int tag) {
	BYTE buffer[8];

	buffer[0] = (tag == DEFAULT_TAG) ? BER_BOOLEAN : BER_CONTEXT_SPECIFIC | tag;
	buffer[1] = 1;
	buffer[2] = boolean  ? 0xFF : 0;

	return asn_write(object, buffer, 3);
}

/* Encode an octet string */

int
asn_encode_octet_string(DATA_OBJECT *object, const BYTE *string, const int length,
					  const int tag) {
	int ctag;
	int ret;

	ctag = (tag == DEFAULT_TAG) ? BER_OCTETSTRING : BER_CONTEXT_SPECIFIC | tag;

	ret = asn_encode_tag(object, ctag);
	if (ret == 0)
		ret = asn_encode_length(object, length);

	if (ret == 0)
		ret = asn_write(object, string, length);

	return ret;
}

/* Encode a character string.  This handles any of the myriad ASN.1 character
   string types.  The handling of the tag works somewhat differently here to
   the usual manner in that since the function is polymorphic, the tag
   defines the character string type and is always used (there's no
   DEFAULT_TAG like the other functions use)*/

int
asn_encode_character_string(DATA_OBJECT *object, const BYTE *string,
				const int length, const int tag) {
	int ret;

	ret = asn_encode_tag(object, tag);
	if (ret == 0)
		ret = asn_encode_length(object, length);
	if (ret == 0)
		ret = asn_write(object, string, length);

	return ret;
}

/* Encode a bit string */
int
asn_encode_bitstring(DATA_OBJECT *object, const int bitstring, const int tag) {
	BYTE buffer[16];
	unsigned int value = 0;
	int data = bitstring, nobits = 0, i;

	if (bitstring < 0) {
		ASN_ERROR("bitstring < 0 ?\n");
		return -1;
	}

	/* ASN.1 bitstrings start at bit 0, so we need to reverse the order of
	  the bits before we write them out */
	for (i = 0; i < (sizeof(int)> 2  ? 32 : 16); i++) {
		/* Update the number of significant bits */
		if (data)
			nobits++;

		/* Reverse the bits */
		value <<= 1;
		if (data & 1)
			value |= 1;
		data >>= 1;
	}

	/* Encode the data as an ASN.1 BITSTRING.  This has the potential to lose
	   some bits on 16-bit systems, but this only applies to the more obscure
	   CMP error codes and it's unlikely too many people will be running a
	   CMP server on a DOS box */
	buffer[0] = (tag == DEFAULT_TAG) ? BER_BITSTRING :  BER_CONTEXT_SPECIFIC | tag;
	buffer[1] = 1 + ((nobits + 7)>> 3);
	buffer[2] = ~((nobits - 1)& 7)& 7;
	buffer[3] = value >> 24;
	buffer[4] = value >> 16;
	buffer[5] = value >> 8;
	buffer[6] = value;

	return asn_write(object, buffer, 3 + ((nobits + 7)>> 3));
}

/* Encode a canonical UTCTime and GeneralizedTime value */

static int
asn_encode_time(DATA_OBJECT *object, const UINT32 t, const int tag,
					  const BOOLEAN is_UTC_time) {
	char buffer[20];
	const int length = is_UTC_time  ? 13 : 15; 
	int ret;

	buffer[0] = (tag != DEFAULT_TAG) ? BER_CONTEXT_SPECIFIC | tag : \
				  (is_UTC_time  ? BER_TIME_UTC : BER_TIME_GENERALIZED);
	buffer[1] = length;
	if (is_UTC_time)
		ret = time_to_UTC_time(t, (BYTE *)(buffer + 2));
	else
		ret = time_to_generalized_time(t, (BYTE *)(buffer + 2));
	if (ret >= 0)
	ret = asn_write(object, buffer, length + 2);

	return ret;
}

int
asn_encode_UTC_time(DATA_OBJECT *object, const UINT32 t, const int tag) {

	return asn_encode_time(object, t, tag, TRUE);
}

int
asn_encode_generalized_time(DATA_OBJECT *object, const UINT32 t, const int tag) {

	return asn_encode_time(object, t, tag, FALSE);
}

/****************************************************************************
 *              Encode Routines for  Constructed Objects                    *
 ****************************************************************************/

/* Encode the start of an encapsulating SEQUENCE, SET, or generic tagged
   constructed object.  The difference between encode_octet/bitstring_hole()and
   encode_generic_hole()is that the octet/bit-string versions create a normal
   or context-specific-tagged primitive string while the generic version
   creates a pure hole with no processing of tags */

int
asn_encode_sequence(DATA_OBJECT *object, const int length) {
	int ret;

	ret = asn_encode_tag(object, BER_SEQUENCE);
	if (ret == 0)
		ret = asn_encode_length(object, length);
	return ret;
}

int
asn_encode_set(DATA_OBJECT *object, const int length) {
	int ret;

	ret = asn_encode_tag(object, BER_SET);
	if (ret == 0)
		ret = asn_encode_length(object, length);
	return ret;
}

int
asn_encode_constructed(DATA_OBJECT *object, const int length, const int tag) {
	int ret;

	ret = asn_encode_tag(object, (tag == DEFAULT_TAG) ? \
			  BER_SEQUENCE : MAKE_CTAG(tag));
	if (ret == 0)
		ret = asn_encode_length(object, length);
	return ret;
}

int
asn_encode_octet_string_hole(DATA_OBJECT *object, const int length, const int tag) {
	int ret;

	ret = asn_encode_tag(object, (tag == DEFAULT_TAG) ? \
			  BER_OCTETSTRING : MAKE_CTAG_PRIMITIVE(tag));
	if (ret == 0)
		ret = asn_encode_length(object, length);
	return ret;
}

int
asn_encode_bitstring_hole(DATA_OBJECT *object, const int length, const int tag) {
	int ret;

	ret = asn_encode_tag(object, (tag == DEFAULT_TAG) ? \
			  BER_BITSTRING : MAKE_CTAG_PRIMITIVE(tag));
	if (ret == 0)
		ret = asn_encode_length(object, length + 1);	/* +1 for  bit count */
	if (ret == 0)
		asn_putc(object, 0);
	return ret;
}

int
asn_encode_generic_hole(DATA_OBJECT *object, const int length, const int tag) {
	int ret;

	ret = asn_encode_tag(object, tag);
	if (ret == 0)
		ret = asn_encode_length(object, length);
	return ret;
}



/*****************************************************************************
 *          asn object routine
 *****************************************************************************/

 /* merge two object to one, return new object, if success,
  * else return NULL
  */
DATA_OBJECT *
asn_merge_objects(DATA_OBJECT **object1, DATA_OBJECT **object2, const int tag) {
	UINT		length = 0;
	DATA_OBJECT *	object = NULL;
	UINT 		object_length = 0;
	int 		ret = 0,
			items;

	/* one or two must be valid */
	items = (object1 ? 1 : 0) + (object2 ? 1 : 0);

	switch (items) {
		case 0:
			return NULL;
		case 1:
			/* if only one object is valid and no tag to add, just return it */
			if (tag == NO_TAG)
				return (object1 ? *object1 : *object2);
			break;
		default:
			break;
	}

	/* caculate the length */
	if (object1)
		length += asn_object_datalen(*object1);
	if (object2)
		length += asn_object_datalen(*object2);
	object_length = (tag <= 0) ? length : asn_sizeof_object(length);
	object = asn_malloc_object(object_length);
	if (!object)
		goto failed;

	if (tag != NO_TAG) {
		ret = asn_encode_tag(object, tag);
		if (ret == 0)
			ret = asn_encode_length(object, length);
	}

	if (ret == 0 && object1)
		ret = asn_write(object, asn_object_data(*object1), asn_object_datalen(*object1));
	if (ret == 0 && object2)
		ret = asn_write(object, asn_object_data(*object2), asn_object_datalen(*object2));
	/* set data length of object */

	//asn_object_datalen(object) = object_length;
failed:
	/* whether success or not, free old object */
	if (object1)		{
		asn_free_object(*object1);
		*object1 = NULL;
	}
	if (object2)		{
		asn_free_object(*object2);
		*object2 = NULL;
	}
	if (ret < 0 && object)
		asn_free_object(object);

	return (ret == 0) ? object : NULL;
}

