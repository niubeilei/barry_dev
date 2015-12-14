////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AsnCodec.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Config_AsnCodec_h
#define Omn_Config_AsnCodec_h

#include "aosUtil/Types.h"
#include "ASN1/AsnDefs.h"

struct aosPkcsAlg;
struct aosPkcsDN;
struct aosAsnObjId;

extern int aosAsn1Codec_getLength(char *buffer, unsigned int *cursor, unsigned int *);
extern int aosAsn1Codec_getSeqLen(char *buffer, unsigned int *cursor, unsigned int *);
extern int aosAsn1Codec_getSetLen(char *buffer, unsigned int *cursor, unsigned int *);
extern int aosAsn1Codec_adjustLength(char *buffer, 
							  unsigned int buflen, 
							  unsigned int *cursor, 
							  unsigned int lengthPos,
							  unsigned int newLength);
extern int aosAsn1Codec_expandLength(unsigned int newLength, 
						   unsigned int lengthPos,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *theCursor);
extern int aosAsn1Codec_setid(unsigned int id,
						   unsigned char theclass,
						   char isPrimitive,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *theCursor);

extern int aosAsn1Codec_objId_encode(unsigned int id,
							  unsigned char theclass,
							  char isPrimitive,
							  struct aosAsnObjId *objid,
							  char *buffer, 
							  unsigned int buflen,
							  unsigned int *theCursor);
extern int aosAsn1Codec_string_encode(unsigned int id,
							  unsigned char theclass,
							  char isPrimitive,
							  char *str, 
							  unsigned int len,
							  char *buffer, 
							  unsigned int buflen,
							  unsigned int *theCursor);
extern int aosAsn1Codec_integer_encode(unsigned int id,
							  unsigned char theclass,
							  char isPrimitive,
							  int value,
							  char *buffer, 
							  unsigned int buflen,
							  unsigned int *theCursor);
extern int aosAsn1Codec_bitstring_encode(unsigned int id,
							  unsigned char theclass,
							  char isPrimitive,
							  char *value,
							  unsigned int numbits,
							  char *buffer, 
							  unsigned int buflen,
							  unsigned int *theCursor);
extern int aosAsn1Codec_encodeAlgId(unsigned int id,
                           unsigned char theclass,
                           char primitive,
                           char *buffer,
                           unsigned int buflen,
                           unsigned int *cursor,
                           struct aosPkcsAlg *algId);

extern int aosAsn1Codec_decodeTagId(unsigned int *id,
						   unsigned char *theclass,
						   unsigned char *isPrimitive,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor);
extern int aosAsn1Codec_decodeDN(char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor, 
						   struct aosPkcsDN *name);
extern int aosAsn1Codec_decodeAlgId(char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor, 
						   struct aosPkcsAlg *alg);
extern int aosAsn1Codec_integer_decode(char *buffer, 
						   unsigned int bufle, 
						   unsigned int *cursor, 
						   int *value);
extern int aosAsn1Codec_integer_decode1(char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor, 
						   char *value, 
						   u32 value_len);
extern int aosAsn1Codec_objId_decode(char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor, 
						   struct aosAsnObjId *oid, 
						   unsigned int oidsize);
extern int aosAsn1Codec_string_decode(unsigned int *id, 
						   unsigned char *theclass,
						   unsigned char *primitive,
						   char *str, 
						   unsigned int strBufflen,
						   unsigned int *strlen,
						   char *buffer, 
						   unsigned int buflen,
						   unsigned int *cursor);
extern int aosAsn1Codec_utctime_decode(unsigned int *id, 
						   unsigned char *theclass,
						   unsigned char *primitive,
						   char *str, 
						   unsigned int strlen,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor);
extern int aosAsn1Codec_bitstr_decode(unsigned int *id, 
						   unsigned char *theclass,
						   unsigned char *primitive,
						   char *str, 
						   unsigned int strBufflen,
						   unsigned int *strlen,
						   unsigned int *numbits,
						   char *buffer, 
						   unsigned int buflen, 
						   unsigned int *cursor);


#endif

