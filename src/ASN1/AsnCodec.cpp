////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AsnCodec.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ASN1/AsnCodec.h"

#include "aos/aosKernelAlarm.h"
#include "aosUtil/Types.h"
#include "aosUtil/Tracer.h"
#include "alarm/Alarm.h"
#include "ASN1/ObjId.h"
#include "KernelSimu/string.h"
#include "PKCS/PkcsDefs.h"
#include "PKCS/DN.h"
#include "PKCS/ReturnCode.h"



int aosAsn1Codec_setid(unsigned int id,
						   unsigned char theclass,
						   char isPrimitive,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *theCursor)
{
	// 
	// Short form: [0, 30] 
	//	Bit 8 and 7:	Class
	//  Bit 6:			Primitive (0) or Constructed (1)
	//	Bit 5-1:		The id
	//
	// Long form: > 30: 
	//	First octet is as in low-tag-number form except that bit 
	//  5-1 all '1'. Second and following octets give the tag number, 
	//  base 128, most significant digit first, with as few digits as 
	//  possible, and with the bit 8 of each octet except the last set to '1'.
	//
	int index = 0;
	char tmp[10];

	aosKernelAssert(*theCursor + 1 < buflen, -eAosRc_BufferTooShort);
	if (id <= 30)
	{

		if (isPrimitive)
		{
			buffer[(*theCursor)++] = theclass + id;
			return 0;
		}

		buffer[(*theCursor)++] = ((theclass + id) | 0x20);
		return 0;
	}

	// 
	// Long form
	//
	buffer[(*theCursor)++] = (theclass | ((isPrimitive)?0x1f:0x3f));

	index = 0;
	while (index < 5 && id)
	{
		tmp[index++] = (char)id;
		id = (id >> 7);
	}

	aosKernelAssert(*theCursor + index < buflen, -eAosRc_BufferTooShort);

	index--;
	while (index >= 0)
	{
		if (index == 0)
		{
			buffer[(*theCursor)] = tmp[index];
			return 0;
		}

		buffer[(*theCursor)++] = (tmp[index] | 0x80);
		index--;
	}

	return 0;
}


int aosAsn1Codec_adjustLength(char *buffer, 
							  unsigned int buflen, 
							  unsigned int *cursor, 
							  unsigned int lengthPos,
							  unsigned int newLength)
{
	// 
	// buffer[lengthPos] is a single byte length. If the newLength > 127, 
	// it needs to expand the length field. Otherwise, it simply sets 
	// the length.
	//
	int ret = 0;
	if (newLength < 128)
	{
		// 
		// No need to expand the buffer.
		//
		buffer[lengthPos] = newLength;
	}
	else
	{
		ret = aosAsn1Codec_expandLength(newLength, lengthPos, buffer, buflen, cursor);
		aosKernelAssert(ret == 0, ret);
	}

	return ret;
}


static int aosAsn1Codec_setLength(char *buffer, 
						   unsigned int buflen, 
						   unsigned int length, 
						   unsigned int *theCursor)
{
	// 
	// There are two forms: short (for lengths between 0 and 127) 
	// and long definite (for others). 
	// Short form. One octet. Bit 8 is 0 and bits 7-1 give the length.
	// Long form. Two to 127 octets. Bit 8 of first octet has value 1 
	//		and bits 7-1 give the number of additional length octets. 
	//		Second and following octets give the length, base 256, 
	//		most significant digit first.
	//
	// Note: in the definition, length [0, 127] can be encoded in either
	// the short or long form. In our implementation, we always encode
	// [0, 127] in the short form, which is what DER requires.
	//
	// Note: in our implementation, we assume no length is longer
	// than an unsigned int.
	//
	// Note: 
	// 
	unsigned int cursor = *theCursor;
	if (length <= 127)
	{
		aosKernelAssert(*theCursor + 1 < buflen, -eAosRc_BufferTooShort);
		buffer[cursor++] = length;
		(*theCursor)++;
		return 0;
	}

	if ((length & 0xffff0000) == 0)
	{
		// 
		// There is only two octets in the length
		//
		aosKernelAssert(*theCursor + 3 < buflen, -eAosRc_BufferTooShort);
		buffer[cursor++] = (char)0x82;
		buffer[cursor++] = (char)(length >> 8);
		buffer[cursor++] = (char)length;
		*theCursor = cursor;
		return 0;
	}

	if ((length & 0xff000000) == 0)
	{
		// 
		// There is only three octets in the length
		//
		aosKernelAssert(*theCursor + 4 < buflen, -eAosRc_BufferTooShort);
		buffer[cursor++] = (char)0x83;
		buffer[cursor++] = (char)(length >> 16);
		buffer[cursor++] = (char)(length >> 8);
		buffer[cursor++] = (char)length;
		*theCursor = cursor;
		return 0;
	}

	// 
	// There is four octets in the length
	//
	aosKernelAssert(*theCursor + 5 < buflen, -eAosRc_BufferTooShort);
	buffer[cursor++] = (char)0x84;
	buffer[cursor++] = (char)(length >> 24);
	buffer[cursor++] = (char)(length >> 16);
	buffer[cursor++] = (char)(length >> 8);
	buffer[cursor++] = (char)length;
	*theCursor = cursor;
	return 0;
}


int aosAsn1Codec_expandLength(unsigned int newLength, 
						   unsigned int lengthPos,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor)
{
	// 
	// The length has two forms:
	// Short form: One octet. Bit 8 has value "0" and bits 7-1 give the length. 
	// Long form: Two to 127 octets. Bit 8 of first octet has value "1" and 
	// bits 7-1 give the number of additional length octets. Second and 
	// following octets give the length, base 256, most significant digit first. 
	// 
	// It will shift the entire array to make room, if needed, to expand the
	// length. 
	//
	unsigned char tmp[4];
	int index = 0;
	int i;

	if (newLength < 128)
	{
		// 
		// No need to expand.
		//
		return 0;
	}

	index = 0;
	while (newLength)
	{
		tmp[index++] = newLength;
		newLength = newLength >> 8;
	}

	// 
	// To make room
	//
	aosKernelAssert(*cursor + index < buflen, -eAosRc_BufferTooShort);
	memmove(&buffer[lengthPos+index+1], &buffer[lengthPos+1], *cursor-lengthPos);

	buffer[lengthPos++] = (index | 0x80);

	for (i=index-1; i>=0; i--)
	{
		buffer[lengthPos++] = tmp[i];
	}

	*cursor += index;
	return 0;
}


int aosAsn1Codec_getLength(char *buffer, 
						   unsigned int *cursor, 
						   unsigned int *length)
{
	// 
	// 'cursor' points to the first length byte.
	//
	// if (buffer[*cursor] >= 0 && buffer[*cursor] < 128)
	int bytes;
	int i;

	if (buffer[*cursor] >= 0)
	{
		*length = (unsigned int)buffer[(*cursor)++];
		return 0;
	}

	bytes = (buffer[(*cursor)++] & 0x7f);
	aosKernelAssert(bytes <= 4, -eAosRc_LengthTooLong);

	*length = 0;
	for (i=0; i<bytes; i++)
	{
		*length = ((*length) << 8) + (unsigned char)buffer[(*cursor)++];
	}

	return 0;
}


int aosAsn1Codec_getSeqLen(char *buffer,
						   unsigned int *cursor, 
						   unsigned int *length)
{
	if (buffer[(*cursor)++] != 0x30)
	{
		return -eAosRc_DecodeFailed;
	}

	return aosAsn1Codec_getLength(buffer, cursor, length);
}


int aosAsn1Codec_getSetLen(char *buffer,
						   unsigned int *cursor, 
						   unsigned int *length)
{
	if (buffer[(*cursor)++] != 0x31)
	{
		return -eAosRc_DecodeFailed;
	}

	return aosAsn1Codec_getLength(buffer, cursor, length);
}


int aosAsn1Codec_objId_encode(unsigned int id,
							  unsigned char theclass,
							  char isPrimitive,
							  struct aosAsnObjId *objid,
							  char *buffer, 
							  unsigned int buflen,
							  unsigned int *theCursor)
{
	// 
	// OBJECT IDENTIFIER is a built-in type. It is defined as 
	//	   {[identifier] component_1 ... component_n}
	//	   component_i = identifier | identifier (value) | value
	// where identifier are names and value are optional
	// integer values.
	// 
	// The form without the [identifier] is the 'complete' form that
	// contains all its components (such as {1 2 840 113549}) and 
	// the one with identifier abbreviates the beginning components 
	// with another object identifier. 
	// 
	// Example:
	//		{iso(1) member-body(2) 840 113549}
	//		{1 2 840 113459}
	// 
	// BER Encoding:
	//	1. The first octet has vlue 40 * value1 + value2 (note: value1
	//     is limited to 0, 1, and 2; value2 is limited to [0..39]
	//     when value1 is 0 or 1.
	//  2. The following octets, if any, encode value2, ..., value-n
	//     each value is encoded based 128, most significant digit
	//     first, with as few digits as possible, and 
	//     the most significant bit of each octet except the last 
	//     in the value's encoding set to '1'. 
	//
	int ret;
	unsigned int compIdx = 2;
	unsigned int numComp = 0;
	char tmp[4];
	char result[eMaxObjectIdLength];
	int rsltIdx = 1;

	while (numComp < eAsnMaxObjComps && objid->mComps[numComp] >= 0) numComp++;

	aosKernelAssert(numComp >= 2, -eAosRc_IncorrectObjId);
	aosKernelAssert(buffer, -eAosRc_NullPointer);

	ret = aosAsn1Codec_setid(id, theclass, isPrimitive, buffer, buflen, theCursor);
	aosKernelAssert(ret == 0, -eAosRc_BufferTooShort);

	result[0] = 40 * objid->mComps[0] + objid->mComps[1];	// The first two values
	while (rsltIdx < eMaxObjectIdLength && compIdx < numComp)
	{
		unsigned int c = objid->mComps[compIdx++];
		int index = 0;
		while (index < 4 && c != 0)
		{
			tmp[index++] = (c & 0x7f);
			c = (c >> 7);
		}

		while (index > 0)
		{
			if (index == 1)
			{
				//
				// The last byte. 
				//
				result[rsltIdx++] = tmp[index-1];
			}
			else
			{
				result[rsltIdx++] = tmp[index-1] + 0x80;
			}

			index--;
		}
	}

	ret = aosAsn1Codec_setLength(buffer, buflen, rsltIdx, theCursor);
	aosKernelAssert(ret == 0, -eAosRc_BufferTooShort);
	memcpy(&buffer[*theCursor], result, rsltIdx);
	*theCursor += rsltIdx;

	return 0;
}


//
// This function can be used to encode:
//	OCTET STRING
//	PrintableString
//	IA5String
//
int aosAsn1Codec_string_encode(unsigned int id,
							  unsigned char theclass,
							  char isPrimitive,
							  char *str, 
							  unsigned int len,
							  char *buffer, 
							  unsigned int buflen,
							  unsigned int *theCursor)
{
	// 
	// The difference between octet and printable strings is that
	// octet strings may contain 0's in them. It encode the printable 
	// string "str" using DER:
	//		0x04
	//		Length
	//		The string
	//
	int ret;

	ret = aosAsn1Codec_setid(id, theclass, isPrimitive, buffer, buflen, theCursor);
	aosKernelAssert(ret == 0, -eAosRc_BufferTooShort);

	ret = aosAsn1Codec_setLength(buffer, buflen, len, theCursor);
	aosKernelAssert(ret == 0, -eAosRc_BufferTooShort);
	aosKernelAssert((*theCursor) + len < buflen, -eAosRc_BufferTooShort);

	memcpy(&buffer[*theCursor], str, len);
	*theCursor += len;
	return 0;
}


int aosAsn1Codec_integer_encode(unsigned int id,
							  unsigned char theclass,
							  char isPrimitive,
							  int value,
							  char *buffer, 
							  unsigned int buflen,
							  unsigned int *theCursor)
{
	// 
	// INTEGER is encoded as follows:
	//	0x02			The identifier
	//	Length			The number of bytes of the integer
	//	The value		network order (significant bytes first)
	//
	// This implementation currently does not handle long long (integer64).
	//
	int index = 0;
	int ret;
	unsigned char bytes[4];

	aosKernelAssert(*theCursor + 3 < buflen, -eAosRc_BufferTooShort);
	ret = aosAsn1Codec_setid(id, theclass, isPrimitive, buffer, buflen, theCursor);
	aosKernelAssert(ret == 0, -eAosRc_BufferTooShort);

	if (value == 0)
	{
		buffer[(*theCursor)++] = 0x01;
		buffer[(*theCursor)++] = 0x00;
		return 0;
	}

	bytes[0] = (char)(value >> 24); 
	bytes[1] = (char)(value >> 16); 
	bytes[2] = (char)(value >> 8);
	bytes[3] = (char) value;

	if (value > 0)
	{
		// 
		// Find the first non-zero high bytes.
		//
		while (bytes[index] == 0) index++;

		if (bytes[index] > 0x7f)
		{
			index--;
		}

	}
	else
	{
		// 
		// Find the first non-0xff byte
		//
		while (bytes[index] == 0xff) index++;

		if ((bytes[index] & 0x80) == 0)
		{
			// 
			// The first non-0xff byte's Bit 8 is not set. Need to back off one byte
			//
			index--;
		}
	}

	aosKernelAssert(*theCursor + 4 - index + 1 < buflen, -eAosRc_BufferTooShort);
	buffer[(*theCursor)++] = 4 - index;
	if (index < 0)
	{
		buffer[(*theCursor)++] = 0;
		index++;
	}

	while (index < 4)
	{
		buffer[(*theCursor)++] = bytes[index];
		index++;
	}
	return 0;
}


int aosAsn1Codec_encodeAlgId(unsigned int id,
						   unsigned char theclass,
						   char primitive,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor, 
						   struct aosPkcsAlg *algId)
{
	// 
	// AlgorithmIdentifier  ::=  SEQUENCE  {
    // algorithm               OBJECT IDENTIFIER,
    // parameters              ANY DEFINED BY algorithm OPTIONAL  }
	//
	int ret;
	unsigned int lengthPos;

	buffer[(*cursor)++] = 0x10;
	lengthPos = (*cursor)++;

	ret = aosAsn1Codec_objId_encode(id, theclass, primitive,
			&algId->mAlgId, buffer, buflen, cursor);

	if (algId->mParms)
	{
		// 
		// Not implemented yet
		//
		return aosAlarmStr(-eAosRc_NotImplementedYet, "Encoding algorithm parms not implemented yet");
	}

	return ret;
}


int aosAsn1Codec_integer_decode(char *buffer, 
							  unsigned int buflen,
							  unsigned int *cursor, 
							  int *value)
{
	// 
	// INTEGER is encoded as follows:
	//	0x02			The identifier
	//	Length			The number of bytes of the integer
	//	The value		network order (significant bytes first)
	//
	// This implementation currently does not handle long long (integer64).
	//
	unsigned int bytes;
	u32 i;
	aosKernelAssert(*cursor + 3 < buflen, -eAosRc_BufferTooShort);

	if (buffer[(*cursor)++] != 0x02)
	{
		return aos_alarm("It is not an integer: %d", *(cursor-1));
	}

	bytes = (unsigned int)buffer[(*cursor)++];
	if (bytes > 4)
	{
		return aos_alarm("Too many bytes for an integer: %d", bytes);
	}

	*value = 0;
	for (i=0; i<bytes; i++)
	{
		*value = ((*value) << 8);
		*value |= buffer[(*cursor)++];
	}

	return 0;
}


// 
// It decodes an INTEGER as a binary string
//
int aosAsn1Codec_integer_decode1(
			char *buffer, 
			unsigned int buflen,
			unsigned int *cursor, 
			char *value, 
			u32 value_len)
{
	// 
	// INTEGER is encoded as follows:
	//	0x02			The identifier
	//	Length			The number of bytes of the integer
	//	The value		network order (significant bytes first)
	//
	unsigned int length;
	int ret;
	aos_assert_r(*cursor + 3 < buflen,0);

	if (buffer[(*cursor)++] != 0x02)
	{
		return -eAosRc_DecodeFailed;
	}

	if ((ret = aosAsn1Codec_getLength(buffer, cursor, &length)))
	{
		return ret;
	}

	if (length + *cursor > buflen)
	{
		return aos_alarm("Data out of bound: %d, %d, %d", length, *cursor, buflen);
	}
	
	if (length > value_len)
	{
		return aos_alarm("Value too long: %d, %d", length, value_len);
	}

	memcpy(value, &buffer[*cursor], length);
	*cursor += length;
	return length;
}


int aosAsn1Codec_bitstring_encode(unsigned int id,
							  unsigned char theclass,
							  char isPrimitive,
							  char *value,
							  unsigned int numbits,
							  char *buffer, 
							  unsigned int buflen,
							  unsigned int *theCursor)
{
	// 
	// BIT STRING is encoded as follows:
	//		0x03				The identifier byte
	//		Length				The number of bytes to follow
	//		xx					Number of unused bits
	//		The string itself	
	//
	// This function will encode it as primitive and pad the string
	// with 0's, if needed. This is equivalent to DER.
	//

	int ret;
	int padSize = 8 - (numbits & 0x07);
	int numbytes;

	ret = aosAsn1Codec_setid(id, theclass, isPrimitive, buffer, buflen, theCursor);
	aosKernelAssert(ret == 0, -eAosRc_BufferTooShort);

	if (padSize == 8) padSize = 0;

	numbits += padSize;
	numbytes = (numbits >> 3);

	aosKernelAssert((*theCursor) + numbytes + 3 < buflen, -eAosRc_BufferTooShort);

	ret = aosAsn1Codec_setLength(buffer, buflen, numbytes + 1, theCursor);
	if (ret) return ret;

	buffer[(*theCursor)++] = padSize;
	aosKernelAssert((*theCursor) + numbytes < buflen, -eAosRc_BufferTooShort);
	memcpy(&buffer[*theCursor], value, numbytes);
	(*theCursor) += numbytes;

	return 0;
}


int aosAsn1Codec_decodeDN(char *buffer, 
					   unsigned int buflen, 
					   unsigned int *cursor, 
					   struct aosPkcsDN *name)
{
	//
	// Name ::= CHOICE { RDNSequence }
	// RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
	// RelativeDistinguishedName ::= SET OF AttributeTypeAndValue
	// AttributeTypeAndValue ::= SEQUENCE {
    //	 type     AttributeType,
    //	 value    AttributeValue }
	// AttributeType ::= OBJECT IDENTIFIER
	// AttributeValue ::= ANY DEFINED BY AttributeType
	// 
	// DirectoryString ::= CHOICE {
    //     teletexString           TeletexString (SIZE (1..MAX)),
    //     printableString         PrintableString (SIZE (1..MAX)),
    //     universalString         UniversalString (SIZE (1..MAX)),
    //     utf8String              UTF8String (SIZE (1.. MAX)),
    //     bmpString               BMPString (SIZE (1..MAX)) }	return -1;
	// 
	// In the current implementation, only printable string is supported. 
	//
	int ret;
	unsigned int tagid;
	unsigned char theclass;
	unsigned char primitive;
	unsigned int len;
	unsigned index = 0;
	unsigned int finish;

	ret = aosAsn1Codec_getSeqLen(buffer, cursor, &len);
	if (ret)
	{
		aosAlarmStr(-eAosRc_DecodeFailed, "Failed to retrieve length");
		return ret;
	}

	index = 0;
	finish = *cursor + len;
	while (*cursor < finish)
	{
		// 
		// Decode one DN
		//
		unsigned int len1;
		unsigned int len2;

		ret = aosAsn1Codec_getSetLen(buffer, cursor, &len1);
		if (ret)
		{
			aosAlarmStr(-eAosRc_DecodeFailed, "Failed to retrieve Set length");
			return ret;
		}

		ret = aosAsn1Codec_getSeqLen(buffer, cursor, &len2);
		if (ret)
		{
			aosAlarmStr(-eAosRc_DecodeFailed, "Failed to retrieve Sequence length");
			return ret;
		}

		// 
		// Retrieve the OBJECT IDENTIFIER
		//
		ret = aosAsn1Codec_objId_decode(buffer, *cursor + len2, 
			cursor, &name->mComps[index].mType, eAsnMaxObjComps);
		if (ret)
		{
			aosAlarmStr(-eAosRc_DecodeFailed, "Failed to retrieve OBJECT IDENTIFIER");
			return ret;
		}
		unsigned int dataLen = 0;
		ret = aosAsn1Codec_string_decode(&tagid, &theclass, &primitive, 
			name->mComps[index].mValue, ePkcsMaxDnValueLen, &dataLen,buffer, buflen, cursor);
		if (ret)
		{
			aosAlarmStr(-eAosRc_DecodeFailed, "Failed to retrieve name");
			return ret;
		}

		index++;
	}

	name->mNumComps = index;
	return 0;
}


int aosAsn1Codec_decodeAlgId(char *buffer, 
					   unsigned int buflen, 
					   unsigned int *cursor, 
					   struct aosPkcsAlg *alg)
{
	// 
	// AlgorithmIdentifier  ::=  SEQUENCE  {
    // algorithm               OBJECT IDENTIFIER,
    // parameters              ANY DEFINED BY algorithm OPTIONAL  }
	//
	int ret;
	unsigned int len;

	if (buffer[(*cursor)++] != 0x30)
	{
		return aosAlarmStr(-eAosRc_DecodeFailed, "Failed to decode Algorithm ID");
	}

	ret = aosAsn1Codec_getLength(buffer, cursor, &len);
	if (ret)
	{
		aosAlarmStr(-eAosRc_DecodeFailed, "Failed to retrieve the length");
		return ret;
	}

	aosKernelAssert(*cursor + len < buflen, -eAosRc_DecodeFailed);

	// 
	// Decode the Algorithm ID (OBJECT IDENTIFIER)
	//
	ret = aosAsn1Codec_objId_decode(buffer, *cursor + len, cursor, &alg->mAlgId, eAsnMaxObjComps);
	if (ret)
	{
		return ret;
	}

	// 
	// Decode the parameters. Currently, only null is supported
	//
	if (buffer[(*cursor)++] == 0x05 && buffer[(*cursor)++] == 0x00)
	{
		return 0;
	}

	return aosAlarmStr(-eAosRc_NotImplementedYet, "Not implemented yet");
}


int aosAsn1Codec_objId_decode(char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor, 
						   struct aosAsnObjId *oid, 
						   unsigned int oidsize)
{
	//
	// BER Encoding:
	//	1. The first octet has vlue 40 * value1 + value2 (note: value1
	//     is limited to 0, 1, and 2; value2 is limited to [0..39]
	//     when value1 is 0 or 1.
	//  2. The following octets, if any, encode value2, ..., value-n
	//     each value is encoded based 128, most significant digit
	//     first, with as few digits as possible, and 
	//     the most significant bit of each octet except the last 
	//     in the value's encoding set to '1'. 
	//
	int ret;
	unsigned int len;
	unsigned char tmp1;
	unsigned int index = 0;

	if (buffer[(*cursor)++] != 0x06)
	{
		return aosAlarmStr(-eAosRc_DecodeFailed, "Not an Object ID");
	}

	ret = aosAsn1Codec_getLength(buffer, cursor, &len);
	if (ret)
	{
		aosAlarmStr(-eAosRc_DecodeFailed, "Failed to retrieve length");
		return ret;
	}

	tmp1 = (unsigned char)buffer[(*cursor)++];
	index = 0;
	oid->mComps[index++] = tmp1 / 40;
	oid->mComps[index++] = tmp1 % 40;
	len--;

	while (len > 0 && index < oidsize)
	{
		unsigned int value = 0;
		unsigned char byte = (unsigned char)buffer[(*cursor)++]; len--;
		while (len >= 0)
		{
			if (byte & 0x80)
			{
				value = (value << 7) + (byte & 0x7f);
				byte = (unsigned char)buffer[(*cursor)++]; len--;
				continue;
			}
			else
			{
				value = (value << 7) + byte;
				break;
			}
		}

		aosKernelAssert((byte & 0x80) == 0, -eAosRc_DecodeFailed);
		oid->mComps[index++] = value;
	}

	aosKernelAssert(index < oidsize, -eAosRc_DecodeFailed);
	oid->mComps[index] = -1;
	return 0;
}


//
// This function can be used to encode:
//	OCTET STRING
//	PrintableString
//	IA5String
//
int aosAsn1Codec_string_decode(unsigned int *id, 
							  unsigned char *theclass,
							  unsigned char *primitive,
							  char *str, 
							  unsigned int strBufflen,
							  unsigned int *strlen,
							  char *buffer, 
							  unsigned int buflen,
							  unsigned int *cursor)
{
	// 
	// The difference between octet and printable strings is that
	// octet strings may contain 0's in them. It encode the printable 
	// string "str" using DER:
	//		0x04
	//		Length
	//		The string
	//
	int ret;

	// Decode the Tag ID
	ret = aosAsn1Codec_decodeTagId(id, theclass, primitive, buffer, buflen, cursor);
	aosKernelAssert(ret == 0, -eAosRc_DecodeFailed);

	// Decode the length
	ret = aosAsn1Codec_getLength(buffer, cursor, strlen);
	aosKernelAssert(ret == 0, -eAosRc_DecodeFailed);
	aosKernelAssert((*cursor) + (*strlen) <= buflen, -eAosRc_BufferTooShort);
	aosKernelAssert((*strlen) < strBufflen, -eAosRc_BufferTooShort);

	memcpy(str, &buffer[*cursor], *strlen);
	*cursor += *strlen;
	return 0;
}


int aosAsn1Codec_decodeTagId(unsigned int *id,
						   unsigned char *theclass,
						   unsigned char *isPrimitive,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor)
{
	// 
	// Short form: [0, 30] 
	//	Bit 8 and 7:	Class
	//  Bit 6:			Primitive (0) or Constructed (1)
	//	Bit 5-1:		The id
	//
	// Long form: > 30: 
	//	First octet is as in low-tag-number form except that bit 
	//  5-1 all '1'. Second and following octets give the tag number, 
	//  base 128, most significant digit first, with as few digits as 
	//  possible, and with the bit 8 of each octet except the last set to '1'.
	//

	unsigned char byte = (unsigned char)buffer[(*cursor)++];
	*theclass = (unsigned char)(byte >> 6);
	*isPrimitive = (unsigned char)((byte >> 5) & 0x01);

	if ((byte & 0x1f) != 0x1f)
	{
		// 
		// It is a single byte ID
		//
		*id = (byte & 0x1f);
		return 0;
	}

	// 
	// Long form
	//
	byte = (unsigned char)buffer[(*cursor)++];
	*id = 0;
	while ((byte & 0x80))
	{
		*id = ((*id) << 7) + (byte & 0x7f);
		byte = (unsigned char)buffer[(*cursor)++];
	}

	*id = ((*id) << 7) + byte;
	return 0;
}


int aosAsn1Codec_utctime_decode(unsigned int *id, 
						   unsigned char *theclass,
						   unsigned char *primitive,
						   char *str, 
						   unsigned int strlen,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor)
{
	// 
	// The UTCTime type denotes a "coordinated universal time" or 
	// Greenwich Mean Time (GMT) value. A UTCTime value includes the 
	// local time precise to either minutes or seconds, and an offset 
	// from GMT in hours and minutes. It takes any of the following forms: 
	//		YYMMDDhhmmZ
	//		YYMMDDhhmm+hh'mm'
	//		YYMMDDhhmm-hh'mm'
	//		YYMMDDhhmmssZ
	//		YYMMDDhhmmss+hh'mm'
	//		YYMMDDhhmmss-hh'mm' 
	// where: 
	//	Z indicates that local time is GMT, + indicates that local time is 
	//  later than GMT, and - indicates that local time is earlier than GMT 
	//  hh' is the absolute value of the offset from GMT in hours 
	//  mm' is the absolute value of the offset from GMT in minutes 
	// This type is a string type. 
	//
	// Example: The time 4:45:40 p.m. Pacific Daylight Time on May 6, 1991 can 
	// be represented with either of the following UTCTime values, among others: 
	//		"910506164540-0700" 
	//		"910506234540Z" 
	// These values have the following BER encodings, among others: 
	//		17 0d 39 31 30 35 30 36 32 33 34 35 34 30 5a
	//		17 11 39 31 30 35 30 36 31 36 34 35 34 30 2D 30 37 30 30
	// DER encoding. Primitive. Contents octets are as for a primitive BER encoding. 
	// 
	unsigned int len;
	int ret = aosAsn1Codec_decodeTagId(id, theclass, primitive, 
		buffer, buflen, cursor);
	if (ret)
	{
		return ret;
	}

	ret = aosAsn1Codec_getLength(buffer, cursor, &len);
	if (ret)
	{
		return ret;
	}

	aosKernelAssert(len < strlen, -eAosRc_BufferTooShort);
	memcpy(str, &buffer[(*cursor)], len);
	str[len] = 0;
	(*cursor) += len;
	return 0;
}


int aosAsn1Codec_bitstr_decode(unsigned int *id, 
						   unsigned char *theclass,
						   unsigned char *primitive,
						   char *str, 
						   unsigned int strBufflen,
						   unsigned int *strlen,
						   unsigned int *numbits,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor)
{
	unsigned char bitsNotUsed;
	int ret = aosAsn1Codec_decodeTagId(id, theclass, primitive, 
		buffer, buflen, cursor);

	if (ret)
	{
		return ret;
	}

	ret = aosAsn1Codec_getLength(buffer, cursor, strlen);
	if (ret)
	{
		return ret;
	}

	aosKernelAssert((*strlen) > 1, -eAosRc_DecodeFailed);

	(*strlen)--;
	bitsNotUsed = (unsigned char)buffer[(*cursor)++];
	aosKernelAssert(*strlen < strBufflen, -eAosRc_DecodeFailed);
	aosKernelAssert((*cursor) + (*strlen) <= buflen, -eAosRc_DecodeFailed);
	memcpy(str, &buffer[(*cursor)], *strlen);
	*cursor += *strlen;
	*numbits = (*strlen << 3) - bitsNotUsed;
	return 0;
}




