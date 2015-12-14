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
// 2010/06/07	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Security_SessionMgr_h
#define Omn_Security_SessionMgr_h

#include "Base64/Base64.h"
#include "Config/ConfigEntity.h"
#include "Debug/Rslt.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Security/Ptrs.h"
#include "Security/Session.h"
#include "Security/SessionMap.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/StrHash.h"
#include "XmlUtil/Ptrs.h"
#include "XmlParser/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "Rundata/Ptrs.h"
#include <vector>
#include <hash_map>

using namespace std;

OmnDefineSingletonClass(AosSessionMgrSingleton, 
						AosSessionMgr,
						AosSessionMgrSelf,
						OmnSingletonObjId::eSessionMgr, 
						"SessionMgr");


class AosSessionMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

	typedef hash_map<const OmnString, AosSessionPtr, Omn_Str_hash, compare_str> AosSessionMap_t;
	typedef hash_map<const OmnString, AosSessionPtr, Omn_Str_hash, compare_str>::iterator AosSessionMapItr_t;

private:
	enum
	{
		eSeqnoByte = 4,
		eRecordId1 = 7,
		eRecordId2 = 17,
		eRecordId3 = 9,
		eSsid1 = 23,
		eSsid2 = 15,
		eSsid3 = 20,
		eSsid4 = 13,
		eSsid5 = 19,
		eSsid6 = 29,
		eSsid7 = 2,
		eSsid8 = 8,
		eMinLoginNum = 5,
		eDftAcctDur = 1,		// one hour
		eDftTimedUnit = 3600,	// one hour
		eMaxEntries = 20
	};

	OmnMutexPtr		mLock;
	AosXmlTagPtr	mSmgrDoc;
	int				mDftAcctDur;
	int				mMinLoginNum;
	int				mTimedUnit;
	AosSessionMap	mSessions;
	u32				mStartSsid;	
	u32				mEndSsid;	

public:
	AosSessionMgr();
	~AosSessionMgr();

	static AosSessionMgr *	getSelf();
	bool		start();
	bool		stop();
	bool		start(const AosXmlTagPtr &def);
	bool		config(const AosXmlTagPtr &def);

	AosSessionPtr
	getSession1(const OmnString &ssid, const AosRundataPtr &rdata)
	{
		OmnString hpvpd;
		return getSession1(ssid, hpvpd, rdata);
	}

	AosSessionPtr
	getSession1(const OmnString &ssid, OmnString &hpvpd, const AosRundataPtr &rdata);

	OmnString	createSession(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &userdoc,
		const OmnString &loginvpd,
		const OmnString &hpvpd,
		AosSessionObjPtr &session);

	static void displaySession(const OmnString &ssid);
	static bool decodeSsid1(
		const OmnString &ssid, 
		u32 &seqno, 
		u32 &recordid, 
		u64 &sd)
	{
		// SessionID is encoded as:
		// 		seqno		(one byte)
		// 		recordid	(three bytes for 18 bits)
		// 		sd			(eight bytes for 48 bits)
		const char *data = ssid.data();
		char c = AosSSIDBase64CharToChar(data[eSeqnoByte]);
		if (c == -1) return false;
		seqno = c;

		char v1, v2, v3, v4, v5, v6, v7, v8;
		v1 = AosSSIDBase64CharToChar(data[eRecordId1]);
		v2 = AosSSIDBase64CharToChar(data[eRecordId2]);
		v3 = AosSSIDBase64CharToChar(data[eRecordId3]);
		if (v1 == -1 || v2 == -1 || v3 == -1) return false;
		recordid = (u32)v1 + ((u32)v2 << 6) + ((u32)v3 << 12);
		v1 = AosSSIDBase64CharToChar(data[eSsid1]);
		v2 = AosSSIDBase64CharToChar(data[eSsid2]);
		v3 = AosSSIDBase64CharToChar(data[eSsid3]);
		v4 = AosSSIDBase64CharToChar(data[eSsid4]);
		v5 = AosSSIDBase64CharToChar(data[eSsid5]);
		v6 = AosSSIDBase64CharToChar(data[eSsid6]);
		v7 = AosSSIDBase64CharToChar(data[eSsid7]);
		v8 = AosSSIDBase64CharToChar(data[eSsid8]);
		if (v1 == -1 || v2 == -1 || v3 == -1 || v4 == -1 ||
			v5 == -1 || v6 == -1 || v7 == -1 || v8 == -1) return false;

		sd =  (u64)v1 + 
			 ((u64)v2 << 6) + 
			 ((u64)v3 << 12) + 
			 ((u64)v4 << 18) + 
			 ((u64)v5 << 24) + 
			 ((u64)v6 << 30) + 
			 ((u64)v7 << 36) + 
			 ((u64)v8 << 42);
		return true;
	}

	static void encodeSsid1(
		OmnString &ssid, 
		const u32 seqno, 
		const u32 recordid, 
		const u64 &sd)
	{
		// The format:
		// 	seqno		(char)
		// 	recordid	(three bytes for 18 bits)
		// 	sd			(8 bytes for 48 bits)
		char *data = (char *)ssid.data();
		data[eSeqnoByte] = AosSSIDBase64U8ToChar(seqno);
		data[eRecordId1] = AosSSIDBase64U8ToChar((u8)recordid);
		data[eRecordId2] = AosSSIDBase64U8ToChar((u8)(recordid >> 6));
		data[eRecordId3] = AosSSIDBase64U8ToChar((u8)(recordid >> 12));
		data[eSsid1] = AosSSIDBase64U8ToChar((u8)sd);
		data[eSsid2] = AosSSIDBase64U8ToChar((u8)(sd >> 6));
		data[eSsid3] = AosSSIDBase64U8ToChar((u8)(sd >> 12));
		data[eSsid4] = AosSSIDBase64U8ToChar((u8)(sd >> 18));
		data[eSsid5] = AosSSIDBase64U8ToChar((u8)(sd >> 24));
		data[eSsid6] = AosSSIDBase64U8ToChar((u8)(sd >> 30));
		data[eSsid7] = AosSSIDBase64U8ToChar((u8)(sd >> 36));
		data[eSsid8] = AosSSIDBase64U8ToChar((u8)(sd >> 42));
	}

private:
	OmnString getNextSsidLocked(
			const AosRundataPtr &rdata,
			const u32 seqno, 
			const u32 recordid, 
			const AosXmlTagPtr &userdoc,
			const AosSessionPtr &session);

	 bool setSessionToIIL(
			 u64 &userid,
			 const u32 seqno,
			 const u32 recordid,
			 const AosRundataPtr &rdata);
};
#endif
