////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: asn1_decode.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef __KERNEL__

#include <linux/kernel.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#endif

#include "PKCS/asn1.h"
#include "PKCS/x509.h"
//#include "x509_attr.h"


/****************************************************************************
*                                                                           *
*         Utility Routines                                                  *
*                                                                           *
*****************************************************************************/

/* When specifying a tag, we can use either the default tag for the object
   (given with DEFAULT_TAG) or a special-case tag.  The following macro
   selects the correct value.  Since these are all primitive objects, we
   force the tag type to a primitive tag */

#define asn_select_tag(tag, default)	\
		(((tag) == DEFAULT_TAG) ? (default) : (MAKE_CTAG_PRIMITIVE(tag)))

/* Read the length octets for an ASN.1 data type, with special-case handling
   for long and short lengths and indefinite-length encodings.  The short-
   length read is limited to 32K, the limit for most PKI data and one that
   doesn't cause type conversion problems on systems where sizeof(int) !=
   sizeof(long).  If the caller indicates that indefinite lengths are OK,
   we return OK_SPECIAL if we encounter one.  Long length reads always allow
   indefinite lengths since these are quite likely for large objects */

typedef enum {
	READLENGTH_NONE,		/* No length read behaviour */
	READLENGTH_SHORT,		/* Short length, no indef.allowed */
	READLENGTH_SHORT_INDEF,		/* Short length, indef.to OK_SPECIAL */
	READLENGTH_LONG_INDEF,		/* Long length, indef.to OK_SPECIAL */
	READLENGTH_LAST			/* Last possible read type */
} READLENGTH_TYPE;


/*******************************************************************
 * All functions return
 *      error : -1,
 *	OK:	0
 *******************************************************************/


static long
asn_decode_length_value(DATA_OBJECT *object, const READLENGTH_TYPE type) 	{
	BYTE    buffer[16], *bufPtr = buffer;
	long    length, length_octets;
	BOOLEAN is_short = (type == READLENGTH_SHORT || type == READLENGTH_SHORT_INDEF);
	int     ret;

	/* Read the first byte of length data.  If it's a short length, we're
	   done */
	length = asn_getc(object);
	if (length < 0 || !(length & 0x80))
		return length;

	/* Read the actual length octets */
	length_octets = length & 0x7F;
	if(length_octets <= 0) {
		/* If indefinite lengths aren't allowed, signal an error */
		if(type != READLENGTH_SHORT_INDEF && \
			type != READLENGTH_LONG_INDEF)
			return -1;

		/* Indefinite length encoding, warn the caller */
		return OK_SPECIAL;
	}

	if (length_octets > 8)
		return -1;
	else
		ret = asn_read(object, buffer, length_octets);
	if(ret  < 0)
		return -1;

	/* Handle leading zero octets.  Since BER lengths can be encoded in
	   peculiar ways (at least one text uses a big-endian 32-bit encoding
	   for everything) we allow up to 8 bytes of non-DER length data, but
	   only the last 2 or 4 of these (for short or long lengths
	   respectively) can be nonzero */
	if(!buffer[0]) {
		int i;

		/* Oddball length encoding with leading zero(es) */
		for(i = 0; i < length_octets && !buffer[i]; i++);

		length_octets -= i;
		if(length_octets <= 0)
			return(0);		/* Very broken encoding of a zero length */

		bufPtr += i;			/* Skip leading zero(es) */
	}

	/* Make sure that the length size is reasonable */
	if (is_short && length_octets > 2)
		return -1;
	if(length_octets > 4)
		return -1;

	/* Read BIGENDIAN encoded value and check the length value */
	length = 0;
	while(length_octets-- > 0)
		length = length << 8 | *bufPtr++;

	if(is_short) {
		if(length & 0xFFFF8000UL)
			/* Length must be < 32K for short lengths */
			return -1;
	} else	if((length & 0x80000000UL) || length > MAX_INTLENGTH)
			/* Length must be < MAX_INTLENGTH for standard data */
			return -2;
	if(length < 0)
		/* Shouldn't happen since the above check catches it, but we check
		   again just to be safe */
		return -1;

	return length;
}

/* Read the header for a (signed) integer value */

int
asn_decode_integer_header(DATA_OBJECT *object, const int tag) {
	int length;

	/* Read the identifier field if necessary and the length */
	if (tag != NO_TAG && asn_get_tag(object) != asn_select_tag(tag, BER_INTEGER))
		return -1;
	length = asn_decode_length_value(object, READLENGTH_SHORT);
	if(length <= 0)
		return length;	/* Error or zero length */

	/* ASN.1 encoded values are signed while the internal representation is
	   unsigned, so we skip any leading zero bytes needed to encode a value
	   that has the high bit set.  If we get a value with the (supposed)
	   sign bit set we treat it as an unsigned value, since a number of
	   implementations get this wrong */
	while(length > 0 && asn_peek(object) == 0) {
		int ret;

		ret = asn_getc(object);
		if(ret < 0)
			return -1;
		length--;
	}

	return(length);
}

/* Read the header for a constructed object */

static int
asn_decode_object_tag(DATA_OBJECT *object, const int tag)	{
	int tag_value;

	tag_value = asn_get_tag(object);
	if (tag_value < 0)
		return tag_value;

	if(tag == ANY_TAG) 		{
		/* Even if we're prepared to accept (almost) any tag, we still have
		   to check for valid universal tags: BIT STRING, primitive or
		   constructed OCTET STRING, SEQUENCE, or SET */
		if((tag_value & BER_CLASS_MASK) != BER_CONTEXT_SPECIFIC && \
			tag_value != BER_BITSTRING && tag_value != BER_OCTETSTRING && \
			tag_value != (BER_OCTETSTRING | BER_CONSTRUCTED) && \
			tag_value != BER_SEQUENCE && tag_value != BER_SET)
			return -1;
		}
	else
		if(tag_value != tag)
			return -1;

	return 0;
}

static int
asn_decode_object_header(DATA_OBJECT *object, int *length, const int tag,
		const BOOLEAN isBitString, const BOOLEAN indefOK) {
	int data_length, ret;

	/* Clear return value */
	if(length != NULL)
		*length = 0;

	/* Read the object tag */
	ret = asn_decode_object_tag(object, tag);
	if(ret < 0)
		return(ret);

	/* Read the length.  If the indefiniteOK flag is set or the length is
	   being ignored by the caller we allow indefinite lengths.  The latter
	   is because it makes handling of infinitely-nested SEQUENCEs and
	   whatnot easier if we don't have to worry about definite vs.
	   indefinite-length encoding, and if indefinite lengths really aren't
	   OK then they'll be picked up when the caller runs into the EOC at the
	   end of the object */
	data_length = asn_decode_length_value(object, (indefOK || length == NULL) ? \
								  READLENGTH_SHORT_INDEF : READLENGTH_SHORT);
	if (data_length < 0) {
		/* If we've asked for an indication of indefinite-length values and we
		   got one, convert the length to CRYPT_UNUSED */
		if(indefOK && data_length == OK_SPECIAL)
			data_length = ASN_UNUSED;
		else
			return -1;
	}

	/* If it's a bit string there's an extra unused-bits count */
	if (isBitString)	{
		int value;

		if(data_length != ASN_UNUSED) {
			if(data_length <= 0)
				return -1;
			data_length--;
		}

		value = asn_getc(object);
		if(value < 0)
			return -1;
	}

	if(length != NULL)
		*length = data_length;
	return 0;
}

static int
asn_decode_long_object_header(DATA_OBJECT *object, long *length, const int tag) {
	long data_length;
	int ret;

	/* Clear return value */
	if(length != NULL)
		*length = 0;

	/* Read the object tag */
	ret = asn_decode_object_tag(object, tag);
	if(ret < 0)
		return -1;

	/* Read the length */
	data_length = asn_decode_length_value(object, READLENGTH_LONG_INDEF);
	if (data_length < 0) {
		/* We've asked for an indication of indefinite-length values, if we
		   got one convert the length to ASN_UNUSED */
		if(data_length == OK_SPECIAL)
			data_length = ASN_UNUSED;
		else
			return(data_length);
		}
	if(length != NULL)
		*length = data_length;

	return 0;
}

/* Read a (short) numeric value, used by several routines */
static int
asn_decode_numeric(DATA_OBJECT *object, long *value) {
	BYTE buffer[8], *bufPtr = buffer;
	int length, ret;

	/* Clear return value */
	if(value != NULL)
		*value = 0L;

	/* Read the length field and make sure that it's a short value */
	length = asn_decode_integer_header(object, NO_TAG);
	if(length <= 0)
		return(length);	/* Error or zero length */
	if(length > 4)
		return -1;

	/* Read the data */
	ret = asn_read(object, buffer, length);
	if(ret < 0 || value == NULL)
		return ret;

	while(length-- > 0)
		*value = (*value << 8) | *bufPtr++;

	return 0;
}

/* Read a constrained-length data value, used by several routines */
static int
asn_decode_constrained_data(DATA_OBJECT *object, BYTE *buffer,
			int *buffer_length, const int length,
			const int max_length) {
	int data_length = length, remainder = 0, ret;

	if (length <= 0 || max_length <= 0)
		return -1;

	if (buffer_length && buffer)
		*buffer_length = data_length;
	else
		/* If we don't care about the return value, skip it and exit */
		return asn_skip(object, data_length);

	/* Read the object, limiting the size to the maximum buffer size */
	if (data_length > max_length) {
		remainder = data_length - max_length;
		data_length = max_length;
	}

	ret = asn_read(object, buffer, data_length);
	*buffer_length = data_length;

	/* Skip any remaining data if necessary */
	if (remainder > 0 && ret == 0)
		ret = asn_skip(object, remainder);

	return ret;
}

/****************************************************************************
*                                                                           *
*            Decoding  Routines for Primitive Objects                       *
*                                                                           *
*****************************************************************************/

/* peek object and get the next object length */
long
asn_peek_object_length(DATA_OBJECT *object) {
	DATA_OBJECT nobject;
	int length;

	if (!object || !asn_object_data(object))
		return -1;
	if (asn_object_length(object) == 0)
		return 0;

	/* get a temp object in order to not move the object's data pointer*/
	asn_make_object(&nobject, asn_object_cur_ptr(object),
				asn_object_length(object) - asn_object_pos(object));

	asn_get_tag(&nobject);
	length = asn_decode_length_value(&nobject, READLENGTH_SHORT);
	return (length < 0) ? length : (length + asn_object_pos(&nobject));
}


/* Check for constructed data end-of-contents octets */

BOOLEAN
asn_check_EOC(DATA_OBJECT *object) {
	/* Read the tag and check for an EOC octet pair */
	if(asn_peek_tag(object) != BER_EOC)
		return(FALSE);

	asn_get_tag(object);
	if(asn_getc(object) != 0)
		/* After finding an EOC tag we need to have a length of zero */
		return FALSE;

	return TRUE;
}

/* Read a short (<= 256 bytes) raw object without decoding it.  This is used
   to read short data blocks like object identifiers, which are only ever
   handled in encoded form */

int
asn_decode_raw_object_tag(DATA_OBJECT *object, BYTE *buffer, int *buffer_length,
			const int max_length, const int tag) {
	int length, offset = 0;

	if (!buffer || !buffer_length)
		return -1;

	/* Clear return value */
	*buffer = '\0';
	*buffer_length = 0;

	/* Read the identifier field and length.  Since we need to remember each
	   byte as it's read we can't just call asn_decode_length_value() for the length,
	   but since we only need to handle lengths that can be encoded in one
	   or two bytes this isn't a problem */
	if (tag != NO_TAG) 		{
		const int object_tag = asn_get_tag(object);

		if(tag != ASN_UNUSED && tag != object_tag)
			return -1;
		buffer[offset++] = object_tag;
	}

	length = asn_getc(object);
	if (length < 0)
		return -1;

	buffer[offset++] = length;
	if(length & 0x80) {
		/* If the object is indefinite-length or longer than 256 bytes (i.e.
		   the length-of-length is anything other than 1), we don't want to
		   handle it */
		if(length != 0x81)
			return -1;
		length = asn_getc(object);
		if(length < 0)
			return -1;
		buffer[offset++] = length;
	}

	if (offset + length > max_length)
		return -1;

	/* Read in the rest of the data */
	*buffer_length = offset + length;
	return ((length <= 0) ? 0 : asn_read(object, buffer + offset, length));
}

/* Read a large integer value */
int
asn_decode_integer_tag(DATA_OBJECT *object, BYTE *integer, int *integer_length,
		const int max_length, const int tag) {
	int length;

	if (!integer || !integer_length || max_length <= 0)
		return -1;

	/* Clear return value */
	if (integer && integer_length) {
		*integer = '\0';
		*integer_length = 0;
	}

	/* Read the integer header info */
	length = asn_decode_integer_header(object, tag);
	if(length <= 0)
		return length;	/* Error or zero length */

	/* Read in the numeric value, limiting the size to the maximum buffer
	   size.  This is safe because the only situation where this can occur
	   is when reading some blob (whose value we don't care about) dressed
	   up as an integer rather than for any real integer */
	return asn_decode_constrained_data(object, integer,
			integer_length, length, max_length);
}

/* Read a bignum integer value */
int
asn_decode_bignum_tag(DATA_OBJECT *object, asn_bin_t *bignum, const int tag) {
	int length, ret;

	if (!bignum)
		return -1;

	/* Read the integer header info */
	length = asn_decode_integer_header(object, tag);
	if(length <= 0)
		return(length);	/* Error or zero length */

	/* Read the value into a fixed buffer */
	if(length > ASN_MAX_PKCSIZE)
		return -1;
	bignum->data = asn_object_cur_ptr(object);
	bignum->len  = length;
	ret = asn_skip(object, length);

	return ret;
}

/* Read a universal length_octets and discard it (used to skip unknown or unwanted
   types) */
int
asn_decode_universal_data(DATA_OBJECT *object) {
	int length;

	length = asn_decode_length_value(object, READLENGTH_SHORT);
	if(length <= 0)
		return(length);	/* Error or zero length */

	return asn_skip(object, length);
}

int
asn_decode_universal(DATA_OBJECT *object) {
	int ret;

	ret = asn_get_tag(object);
	if (ret < 0)
		return ret;
	else
		return asn_decode_universal_data(object);
}

/* Read a short integer value */
int
asn_decode_short_integer_tag(DATA_OBJECT *object, long *value, const int tag) {
	/* Clear return value */
	if(value != NULL)
		*value = 0L;

	if(tag != NO_TAG && asn_get_tag(object) != asn_select_tag(tag, BER_INTEGER))
		return -1;
	return asn_decode_numeric(object, value);
}

/* Read an enumerated value.  This is encoded like an ASN.1 integer so we
   just read it as such */
int
asn_decode_enumerated_tag(DATA_OBJECT *object, int *enumeration, const int tag) {
	long value;
	int ret;

	/* Clear return value */
	if(enumeration != NULL)
		*enumeration = 0;

	if(tag != NO_TAG && asn_get_tag(object) != asn_select_tag(tag, BER_ENUMERATED))
		return -1;

	ret = asn_decode_numeric(object, &value);
	if(ret == 0 && enumeration != NULL)
		*enumeration = (int) value;

	return ret;
}

/* Read a null value */

int
asn_decode_null_tag(DATA_OBJECT *object, const int tag) {

	/* Read the identifier if necessary */
	if(tag != NO_TAG && asn_get_tag(object) != asn_select_tag(tag, BER_NULL))
		return -1;

	if(asn_getc(object) != 0)
		return -1;
	return 0;
}

/* Read a boolean value */

int
asn_decode_boolean_tag(DATA_OBJECT *object, BOOLEAN *boolean, const int tag) {
	int value;

	/* Clear return value */
	if(boolean != NULL)
		*boolean = FALSE;

	if(tag != NO_TAG && asn_get_tag(object) != asn_select_tag(tag, BER_BOOLEAN))
		return -1;

	/* boolean length is always 1, so skip it */
	if(asn_getc(object) != 1)
		return -1;

	value = asn_getc(object);
	if(value < 0)
		return value;

	if(boolean != NULL)
		*boolean = value ? TRUE : FALSE;

	return 0;
}

/* Read an OID and check it against a permitted value or a selection of
   permitted values */

int
asn_decode_oid_ex(DATA_OBJECT *object, const OID_INFO *oid_selection,
		const OID_INFO **oid_selection_value)  {
	static const OID_INFO null_oid_selection = { NULL, -1, NULL };
	BYTE buffer[ASN_MAX_OID_SIZE + 8];
	int length, oid_entry, ret;

	if (!oid_selection || !oid_selection_value)
		return -1;

	/* Clear return value */
	if (oid_selection_value)
		*oid_selection_value = &null_oid_selection;

	/* Read the OID data */
	ret = asn_decode_raw_object_tag(object, buffer, &length,
					ASN_MAX_OID_SIZE, BER_OBJECT_IDENTIFIER);
	if (ret < 0)
		return -1;
	if(length != sizeof_oid(buffer))
		return -1;

	/* Try and find the entry for the OID */
	for(oid_entry = 0; oid_selection[oid_entry].oid != NULL; oid_entry++)
		if(length == sizeof_oid(oid_selection[oid_entry].oid) && \
			!memcmp(buffer, oid_selection[oid_entry].oid, length))
			break;
	if (oid_selection[oid_entry].oid == NULL)
		return -1;

	if (oid_selection_value != NULL)
		*oid_selection_value = &oid_selection[oid_entry];
	return 0;
}

int
asn_decode_oid(DATA_OBJECT *object, const OID_INFO *oid_info, int *selection_id) {
	const OID_INFO *oid_selection_info;
	int ret;

	/* Clear return value */
	if (selection_id)
		*selection_id = -1;

	ret = asn_decode_oid_ex(object, oid_info, &oid_selection_info);
	if (ret == 0 && selection_id)
		*selection_id = oid_selection_info->id;

	return ret;
}

int
asn_decode_fixed_oid(DATA_OBJECT *object, const BYTE *oid) {
	const OID_INFO *dummy;
	const OID_INFO oid_info[2] = {
		{oid, 0},
		{NULL,0}
	};

	if (!oid)
		return -1;

	/* Set up a one-entry OID_INFO list to pass down to asn_decode_oid() */
	//memset(oid_info, 0, sizeof(OID_INFO) * 2);
	//oid_info[0].oid = oid);

	return asn_decode_oid_ex(object, oid_info, &dummy);
}

/* Read an octet string value */

int
asn_decode_octet_string_tag(DATA_OBJECT *object, BYTE *string, int *string_length,
			const int max_length, const int tag) {
	int length;

	if (max_length <= 0)
		return -1;

	/* Clear return value */
	if (string && string_length) {
		*string = '\0';
		*string_length = 0;
	}

	/* Read the string, limiting the size to the maximum buffer size */
	if(tag != NO_TAG && asn_get_tag(object) != asn_select_tag(tag, BER_OCTETSTRING))
		return -1;

	length = asn_decode_length_value(object, READLENGTH_SHORT);
	if(length <= 0)
		return length ;	/* Error or zero length */

	return asn_decode_constrained_data(object, string, string_length,
					length, max_length);
}

/* Read a character string.  This handles any of the myriad ASN.1 character
   string types.  The handling of the tag works somewhat differently here to
   the usual manner in that since the function is polymorphic, the tag
   defines the character string length_octets and is always used (there's no
   NO_TAG or DEFAULT_TAG like the other functions use).  This works because
   the plethora of string types means that the higher-level routines that
   read them invariably have to sort out the valid tag types themselves */

int
asn_decode_character_string(DATA_OBJECT *object, BYTE *string, int *string_length,
			 const int max_length, const int tag) {
	int length;

	if (max_length <= 0)
		return -1;

	if (tag == NO_TAG || tag == DEFAULT_TAG)
		return -1;

	/* Clear return value */
	if (string && string_length)  {
		*string = '\0';
		*string_length = 0;
	}

	/* Read the string, limiting the size to the maximum buffer size */
	if(asn_get_tag(object) != tag)
		return -1;

	length = asn_decode_length_value(object, READLENGTH_SHORT);
	if(length <= 0)
		return length;	/* Error or zero length */

	return asn_decode_constrained_data(object, string, string_length,
					length, max_length);
}

/* Read a bit string */
int
asn_decode_bitstring_tag(DATA_OBJECT *object, int *bitstring, const int tag) {
	unsigned int data, mask = 0x80;
	int flag = 1, value = 0, no_bits, i;
	u32 length;

	/* Clear return value */
	if (bitstring)
		*bitstring = 0;

	/* Make sure that we have a bitstring with between 0 and sizeof(int)
	   bits.  This isn't as machine-dependant as it seems, the only place
	   where bit strings longer than one or two bytes are used is with the
	   more obscure CMP error subcodes that just provide further information
	   above and beyond the main error code and text message, which are
	   unlikely to be used on a 16-bit machine */
	if(tag != NO_TAG && asn_get_tag(object) != asn_select_tag(tag, BER_BITSTRING))
		return -1;
	length = asn_getc(object) - 1;
	no_bits = asn_getc(object);
	if(length < 0 || length > sizeof(int) || no_bits < 0 || no_bits > 7)
		return -1;
	if(length <= 0)
		return 0;	/* Zero value */

	no_bits = (length * 8) - no_bits;

	/* ASN.1 bitstrings start at bit 0, so we need to reverse the order of
	   the bits before we return the value */
	data = asn_getc(object);
	for(i = no_bits - 8; i > 0; i -= 8) {
		int ret;

		ret = asn_getc(object);
		if (ret < 0)	return -1;

		data = (data << 8) | ret;
		mask <<= 8;
	}

	for(i = 0; i < no_bits; i++) {
		if(data & mask)
			value |= flag;
		flag <<= 1;
		data <<= 1;
	}

	if (bitstring)
		*bitstring = value;

	return 0;
}

static int
asn_decode_time(DATA_OBJECT *object, time_t *tm, const BOOLEAN is_UTC_time) {
	BYTE buffer[32];
	int length, i, ret;

	/* Read the length field and make sure that it's of the correct size.
	   There's only one encoding allowed although in theory the encoded
	   value could range in length from 11 to 17 bytes for UTCTime and 13 to
	   19 bytes for GeneralizedTime.  In practice we also have to allow 11-
	   byte UTCTimes since an obsolete encoding rule allowed the time to be
	   encoded without seconds, and Sweden Post haven't realised that this
	   has changed yet */
	length = asn_getc(object);

	if((is_UTC_time && length != 13 && length != 11) || \
		(!is_UTC_time && length != 15))
		return -1;

	/* Read the encoded time data and make sure that the contents are valid */
	memset(buffer, 0, 32);
	ret = asn_read(object, buffer, length);
	if(ret < 0)
		return -1;
	for(i = 0; i < length - 1; i++)
		if(buffer[i] < '0' || buffer[i] > '9')
			return -1;
	if(buffer[length - 1] != 'Z')
		return -1;

	if (!tm)
		return 0;

	*tm = asntime_to_time(buffer, length, is_UTC_time);

	return 0;
}

int
asn_decode_UTC_time_tag(DATA_OBJECT *object, time_t *tm, const int tag) {

	/* Clear return value */
	if (tm)
		*tm = 0;

	if(tag != NO_TAG && asn_get_tag(object) != asn_select_tag(tag, BER_TIME_UTC))
		return -1;
	return asn_decode_time(object, tm, TRUE);
}

int
asn_decode_generalized_time_tag(DATA_OBJECT *object, time_t *tm, const int tag) {

	/* Clear return value */
	if (tm)
		*tm = 0;

	if(tag != NO_TAG && asn_get_tag(object) != asn_select_tag(tag, BER_TIME_GENERALIZED))
		return -1;
	return asn_decode_time(object, tm, FALSE);
}

/*****************************************************************************
 *                                                                           *
 *                   Decoding Routines for Constructed Objects               *
 *                                                                           *
 *****************************************************************************/

/* Read an encapsulating SEQUENCE or SET or BIT STRING/OCTET STRING hole */

int
asn_decode_sequence(DATA_OBJECT *object, int *length) {
	return asn_decode_object_header(object, length, BER_SEQUENCE, FALSE, FALSE);
}

int
asn_decode_sequenceI(DATA_OBJECT *object, int *length) {
	return asn_decode_object_header(object, length, BER_SEQUENCE, FALSE, TRUE);
}

int
asn_decode_set(DATA_OBJECT *object, int *length) {
	return asn_decode_object_header(object, length, BER_SET, FALSE, FALSE);
}

int
asn_decode_setI(DATA_OBJECT *object, int *length) {
	return asn_decode_object_header(object, length, BER_SET, FALSE, TRUE);
}

int
asn_decode_constructed(DATA_OBJECT *object, int *length, const int tag) {
	int ctag;

	ctag = (tag == DEFAULT_TAG) ? BER_SEQUENCE : MAKE_CTAG(tag);
	return asn_decode_object_header(object, length, ctag, FALSE, FALSE);
}

int
asn_decode_constructedI(DATA_OBJECT *object, int *length, const int tag) {
	int ctag;

	ctag = (tag == DEFAULT_TAG) ? BER_SEQUENCE : MAKE_CTAG(tag);
	return asn_decode_object_header(object, length, ctag, FALSE, TRUE);
}

int
asn_decode_octet_string_hole(DATA_OBJECT *object, int *length, const int tag) {
	int ctag;

	ctag = (tag == DEFAULT_TAG) ?  BER_OCTETSTRING : MAKE_CTAG_PRIMITIVE(tag);
	return asn_decode_object_header(object, length, ctag, FALSE, FALSE);
}

int
asn_decode_bitstring_hole(DATA_OBJECT *object, int *length, const int tag) {
	int ctag;

	ctag = (tag == DEFAULT_TAG) ? BER_BITSTRING : MAKE_CTAG_PRIMITIVE(tag);
	return asn_decode_object_header(object, length, ctag, TRUE, FALSE);
}

int
asn_decode_generic_hole(DATA_OBJECT *object, int *length, const int tag) {
	int ctag;

	ctag = (tag == DEFAULT_TAG) ? ANY_TAG : tag;
	return asn_decode_object_header(object, length, ctag, FALSE, FALSE);
}

int
asn_decode_generic_holeI(DATA_OBJECT *object, int *length, const int tag) {
	int ctag;

	ctag = (tag == DEFAULT_TAG) ? ANY_TAG : tag;
	return asn_decode_object_header(object, length, ctag, FALSE, TRUE);
}

/* Read an abnormally-long encapsulating SEQUENCE or OCTET STRING hole.
   This is used in place of the usual read in places where potentially huge
   data quantities would fail the sanity check enforced by the standard
   read.  This form always allows indefinite lengths, which are likely for
   large objects */

int
asn_decode_long_sequence(DATA_OBJECT *object, long *length)	{
	return asn_decode_long_object_header(object, length, BER_SEQUENCE);
}

int
asn_decode_long_constructed(DATA_OBJECT *object, long *length, const int tag) {
	int ctag;

	ctag = (tag == DEFAULT_TAG) ? BER_SEQUENCE : MAKE_CTAG(tag);
	if (length == NULL)
		return -1;

	return asn_decode_long_object_header(object, length, ctag);
}

int
asn_decode_long_generic_hole(DATA_OBJECT *object, long *length, const int tag) {
	int ctag;

	ctag = (tag == DEFAULT_TAG) ? ANY_TAG : tag;
	return asn_decode_long_object_header(object, length, ctag);
}
