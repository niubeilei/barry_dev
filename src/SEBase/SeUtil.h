////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 10/15/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEBase_SeUtil_h
#define Omn_SEBase_SeUtil_h

// #include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "XmlUtil/Ptrs.h"

#include <set>

enum
{
	eAosMaxContainers = 100,
	eAosMaxArgs = 8,
	eAosMaxSiteidLength = 30,
	eAosMaxCloudidLength = 40
	//eAosMaxObjidLength = 70
};

inline bool AosCompU64(const u64 &w1, const u32 w2, const u32 w3)
{
	u32 w11 = (u32)w1;
	u32 w12 = (w1 >> 32);
	return (w11 == w2 && w12 == w3);
}


// Chen Ding, 11/24/2012
// Moved to XmlUtil/SeXmlUtil.h
// extern bool AosNextXmlWord(
// 		char *data, 
// 		const char *delis,
// 		const int delis_len,
// 		const int start_idx, 
// 		const int datalen,
// 		char *buff,
// 		u32 &len, 
// 		int &wordlen);

extern bool AosCheckDataid(const OmnString &dataid);

extern OmnString AosGetXmlAttr(
		const AosXmlTagPtr &cmd, 
		const AosXmlTagPtr &doc,
		const OmnString &attrname);

extern u32 AosGetEpochForm1(const AosXmlTagPtr &def);
extern u32 AosGetEpochForm2(const OmnString &def);

extern OmnString AosParseArgs(const OmnString &args, const OmnString &name1);

extern bool AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1);

extern bool AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2);

extern bool AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3);

extern bool AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3,
		const OmnString &name4, OmnString &value4);

extern bool AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3,
		const OmnString &name4, OmnString &value4,
		const OmnString &name5, OmnString &value5);

extern bool AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3,
		const OmnString &name4, OmnString &value4,
		const OmnString &name5, OmnString &value5,
		const OmnString &name6, OmnString &value6);

extern bool AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3,
		const OmnString &name4, OmnString &value4,
		const OmnString &name5, OmnString &value5,
		const OmnString &name6, OmnString &value6,
		const OmnString &name7, OmnString &value7);

extern OmnString AosGetVersionUID();
extern OmnString AosMD5Encrypt(const OmnString &str);
extern OmnString AosCalSHA1(const OmnString &str);

//ken 2011/12/15
extern bool AosInlineSearchSplit(
				const OmnString &value,
				const OmnString &splittype,
				set<OmnString> &str);

#endif
