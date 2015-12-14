////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 02/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocUtil_DocProcUtil_h
#define Aos_DocUtil_DocProcUtil_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILClientObj.h"
#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "Util/StrHashFixed.h"
#include "Util/StrPairHash.h"
#include "WordParser/WordParser.h"
#include "WordParser/WordNorm.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosDocProcUtil
{
public:
	typedef OmnStrHashFixed<AosEntryMark::E, 260, 30> AosSeWordHash;
	typedef OmnStrHashFixed<OmnString, 260, 30> AosSeWordAttrHash;
	typedef OmnStrPairHash<AosEntryMark::E, 0x3ff> AosSeAttrHash;

public:
	static bool addMetaAttrs( 
						//vector<AosBuffPtr> &buffs,
						vector<AosTransPtr>  *allTrans,
						const u32 arr_len,
						const AosXmlTagPtr &doc,
						const u64 &docId,
						const AosRundataPtr &rdata);
	// static bool removeMetaAttrs(const AosXmlTagPtr &doc);
	static bool removeMetaAttrs(
						//vector<AosBuffPtr> &buffs,
						vector<AosTransPtr>  *allTrans,
						const u32 arr_len,
						const AosXmlTagPtr &doc,
						const u64 &docid,
						const AosRundataPtr &rdata);
	static bool modifyMetaAttrs(
						//vector<AosBuffPtr> &buffs,
						vector<AosTransPtr>  *allTrans,
						const u32 arr_len,
						const AosXmlTagPtr &olddoc, 
						const AosXmlTagPtr &newdoc, 
						const u64 &docid,
						const AosRundataPtr &rdata);

	static bool addValueToIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &iilname, 
			const OmnString &value,
			const u64 &docid, 
			const bool value_unique,
			const bool docid_unique,
			const AosRundataPtr &rdata)
	{
		int physical_id;
		return addValueToIIL(allTrans, arr_len, iilname, value, docid, value_unique, docid_unique,
				physical_id, rdata);
	}

	static bool addValueToIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &iilname, 
			const OmnString &value,
			const u64 &docid, 
			const bool value_unique,
			const bool docid_unique,
			int &pid,
			const AosRundataPtr &rdata)
	{
		AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
		aos_assert_r(iilclient, false);
		int attrtype = (int)eAosAttrType_Str;
		if (AosDocUtil::isTypedAttr(iilname.data(), iilname.length()))
		{
			attrtype = (int)iilname.data()[iilname.length()-1];
		}
		switch (attrtype)
		{
		case eAosAttrType_U64:
		case eAosAttrType_Date:
			 iilclient->addU64ValueDoc(allTrans, arr_len, iilname, 
				 atoll(value.data()), docid, value_unique, docid_unique, pid, rdata); 
			 break;

		default:
		 	 iilclient->addStrValueDoc(allTrans, arr_len, iilname, 
					 value, docid, value_unique, docid_unique, pid, rdata); 
			 break;
		}
		return true;
	}

	static bool addValueToIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &iilname, 
			const u64 &value,
			const u64 &docid, 
			const bool value_unique,
			const bool docid_unique,
			const AosRundataPtr &rdata)
	{
		int physical_id;
		return addValueToIIL(allTrans, arr_len, iilname, value, docid, value_unique, docid_unique,
				physical_id, rdata);
	}

	static bool addValueToIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &iilname, 
			const u64 &value,
			const u64 &docid, 
			const bool value_unique,
			const bool docid_unique,
			int &pid,
			const AosRundataPtr &rdata)
	{
		AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
		aos_assert_r(iilclient, false);
		int attrtype = (int)eAosAttrType_Str;
		if (AosDocUtil::isTypedAttr(iilname.data(), iilname.length()))
		{
			attrtype = (int)iilname.data()[iilname.length()-1];
		}
		switch (attrtype)
		{
		case eAosAttrType_U64:
		case eAosAttrType_Date:
			 iilclient->addU64ValueDoc(allTrans, arr_len, iilname, 
				 value, docid, value_unique, docid_unique, pid, rdata); 
			 break;

		default:
			 {
				 OmnString vv;
				 vv << value;
		 	 	 iilclient->addStrValueDoc(allTrans, arr_len, iilname, 
					 vv, docid, value_unique, docid_unique, pid, rdata); 
		  	 }
			 break;
		}
		return true;
	}

	static bool removeValueFromIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &iilname, 
			const OmnString &value, 
			const u64 &docid,
			const AosRundataPtr &rdata)
	{
		int physical_id;
		return removeValueFromIIL(allTrans, arr_len, iilname, value, docid, physical_id, rdata);
	}

	static bool removeValueFromIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &iilname, 
			const OmnString &value, 
			const u64 &docid,
			int &physical_id,
			const AosRundataPtr &rdata)
	{
		AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
		aos_assert_r(iilclient, false);
		int attrtype = (int)eAosAttrType_Str;
		if (AosDocUtil::isTypedAttr(iilname))
		{
			attrtype = (int)iilname.data()[iilname.length()-1];
		}
		switch (attrtype)
		{
		case eAosAttrType_U64:
		case eAosAttrType_Date:
			 // if (smShowLog) OmnScreen << "Remove value doc: " << iilname 
			 //	 << ":" << value << ":" << docid << endl;
			 iilclient->removeU64ValueDoc(allTrans, arr_len, iilname, 
					 atoll(value.data()), docid, physical_id, rdata);
			 break;

		default:
			 // if (smShowLog) OmnScreen << "Remove value doc: " << iilname 
			 //	 << ":" << value << ":" << docid << endl;
			 iilclient->removeStrValueDoc(allTrans, arr_len,
				iilname, value, docid, physical_id, rdata);
			 break;
		}
		return true;
	}

	inline static bool addValueToIIL(
			const AosBuffPtr &buff,
			const AosAttrType &attrtype,
			const OmnString &iilname, 
			const AosValueRslt &value,
			const u64 &docid, 
			const bool value_unique,
			const bool docid_unique,
			AosRundata *rdata)
	{
		// Format is:
		// 	value			(either u64 or string, depending on iil type)
		// 	docid			(u64)
		// 	value_unique	(char)
		// 	docid_unique	(char, shared)
		static AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
		aos_assert_r(iilclient, false);

		u8 cc = 0;
		if (value_unique) cc |= 0x01;
		if (docid_unique) cc |= 0x02;
		switch (attrtype)
		{
		case eAosAttrType_U64:
		case eAosAttrType_Date:
			 {
				 u64 vv;
				 if (!value.getU64()) return false;
			 	 buff->setU64(vv);
			 }
			 break;

		default:
			 {
				 bool rslt;
				 OmnString vv = value.getStr();
				 aos_assert_r(rslt, false);
			 	 buff->setOmnStr(vv);
			 }
			 break;
		}
		buff->setU64(docid);
		buff->setChar(cc);
		return true;
	}
	static bool collectWords(
			AosSeWordHash &wordHash,
			const AosWordParserPtr &wordParser,
			const AosWordNormPtr &wordNorm,
			const AosXmlTagPtr &parent_doc,
			const AosXmlTagPtr &xml,
			const bool marked); 
	static bool collectWords(
			AosSeWordHash &attrWordHash,
			const AosWordParserPtr &wordParser,
			const AosWordNormPtr &wordNorm,
			const OmnString &value);
	static bool collectAttrs(
			AosSeAttrHash &attrs,
			const AosXmlTagPtr &parent_doc,
			const AosXmlTagPtr &xml,
			const bool marked);
};
#endif

