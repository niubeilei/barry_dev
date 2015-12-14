////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 08/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlUtil_SeXmlUtil_h
#define Aos_XmlUtil_SeXmlUtil_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/IILName.h"
// #include "SEUtil/DocOTypes.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/DocTypes.h"


extern bool AosNextXmlWordChar(
		char *data, 
		const int datalen,
		char &ch,
		int &idx);

extern bool AosNextXmlWord(
 		char *data, 
 		const char *delis,
 		const int delis_len,
 		const int start_idx, 
 		const int datalen,
 		char *buff,
 		u32 &len, 
 		int &wordlen);

struct AosDocMetaAttr
{
	OmnString	attrname;
	OmnString 	iilname;
	bool		value_unique;
	bool		docid_unique;
	void set(const OmnString &aname, const bool v, const bool d)
	{
		attrname = aname;
		AosXmlTag::addMetaAttr(aname);
		iilname = AosIILName::composeAttrIILName(aname);
		value_unique = v;
		docid_unique = d;
	}
};
class AosXmlUtil
{
public:
	enum
	{
		eMaxMetaAttrs = 50
	};
	static AosDocMetaAttr smMetaAttrs[eMaxMetaAttrs];
	static int smNumMetaAttrs;
public:
	static bool isUserHomeContainer(const OmnString &ctnr_objid)
	{
		aos_assert_r(ctnr_objid != "", false);
		OmnString prefix, cid;
		AosObjid::decomposeObjid(ctnr_objid, prefix, cid);
		if (cid == "") return false;
		return prefix == AOSOBJIDRES_USER_HOMEFOLDER;
	}

	static bool verifySysAcct(const AosXmlTagPtr &doc);

	static bool isUserHomeContainer(const AosXmlTagPtr &doc)
	{
		aos_assert_r(doc, false);
		OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
		aos_assert_r(objid != "", false);
		OmnString prefix, cid;
		AosObjid::decomposeObjid(objid, prefix, cid);
		if (cid == "") return false;
		return prefix == AOSOBJIDRES_USER_HOMEFOLDER;
	}

	static bool isAccessRecord(const AosXmlTagPtr &doc)
	{
		aos_assert_r(doc, false);
		return doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD;
	}

	static bool signAttr(
			const AosXmlTagPtr &doc,
			const OmnString &aname,
			const OmnString &sign_name, 
			const AosRundataPtr &rdata);

	static bool verifySignedAttr(
			const AosXmlTagPtr &doc,
			const OmnString &aname, 
			const OmnString &sign_name, 
			const AosRundataPtr &rdata);

	static bool verifySignedAttr(
			const AosXmlTagPtr &doc,
			OmnString &fields,
			const OmnString &aname,
			const OmnString &sign_name, 
			const AosRundataPtr &rdata);

	//static inline u64 setDocidType(AosDocType type, const u64 &docid)
	//{
	//	if (type != eAosUnkonwDoc)
	//	{
	//		u8 * typemask = (u8*)&docid;
	//		typemask[7] = (u8)type; //set the first 8bytes doctype
	//		return docid;
	//	}
	//	return 0;
	//}

	// Chen Ding, 02/29/2012, Moved to DocTypes.h
	static inline void setDocidType(AosDocType::E type, u64 &docid)
	{
		// Chen Ding, 02/29/2012
		// if (type != eAosUnkonwDoc)
		if (AosDocType::isValid(type))
		{
			u8 * typemask = (u8*)&docid;
			typemask[7] = (u8)type; //set the first 8bytes doctype
		}
		else
			docid = 0;
	}

	static bool removeMetaAttrs(const AosXmlTagPtr &doc);
	static void setMetaAttrs();
	static u64 getOwnDocid(const u64 &docid);
};
#endif
