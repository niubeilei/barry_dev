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
// 10/22/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SearchEngineAdmin_XmlDoc2_h
#define AOS_SearchEngineAdmin_XmlDoc2_h

#include "aosUtil/Types.h"
#include "IdGen/IdGenMgr.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "XmlInterface/XmlRc.h"


#define eAosInvFseqno 0xffffffff
#define eAosInvFoffset 0xffffffff
#define eAosInvDocSize 0
#define eAosInvDiskCap 0
#define eAosIndStatus ' '
#define eAosInvTime 0
#define eAosInvDoctype ""
#define eAosInvSubtype ""

class AosBuff;
class OmnConnBuff;

class AosXmlDoc2 : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eVersion10 = 10,
		eCurrentVersion = 11,
		eMaxDocFiles = 100,
		eMaxDocHeaderFiles = 50,
		eDocHeaderSize = 296,
		eDocHeaderFileSize = 100000000,
		eMaxHeaderPerFile = eDocHeaderFileSize/eDocHeaderSize,
		eInvVersion = 0,
		eDelVersion = 0xffffffff,
		eInvSeqno = 0x0fffffff,
		eInvOffset = 0,
		eInvDocsize = 0,
		eDocStatus_Active = 'A',
		eDocStatus_Deleted = 'D',
		eInvStorageFlag = 'v',
		eVersion11HeaderSize = 56
	};

	//static AosDocFileMgrPtr smFileMgr;
	static OmnMutexPtr 		smLock;
public:
	AosXmlDoc2();
	~AosXmlDoc2();

	static bool	staticInit(const AosXmlTagPtr &config);
	static bool	stop(const AosDocFileMgrPtr &filemgr);

	static bool saveToFile(
			const u64 &docid, 
			const AosXmlTagPtr &doc, 
			const bool savenew, 
			const AosDocFileMgrPtr &docfilemgr,
			const AosRundataPtr &rdata);
	static bool deleteDoc(
			const u64 &docid, 
			const AosDocFileMgrPtr &docfilemgr, 
			const AosRundataPtr &rdata);
	static bool isDocDeleted(
			const u64 &docid, 
			const AosDocFileMgrPtr &docfilemgr,
			const AosRundataPtr &rdata);
	static AosXmlTagPtr readFromFile(
			const u64 &docid,
			const AosDocFileMgrPtr &docfilemgr,
			const AosRundataPtr &rdata);	
	static OmnString	readHeader(
			const u64 &docid, 
			const AosDocFileMgrPtr &docfilemgr, 
			const AosRundataPtr &rdata);
   
	static int	getMaxHeaderPerFile() {return eMaxHeaderPerFile;}

	// Chen Ding, 02/29/2012, Moved to XmlUtil/DocTypes.h
	// static AosDocType getDocType(const u64 &docid)
	// {
	// 	u32 type =  (u32)(docid >> 56);
	// 	switch(type)
	// 	{
	// 		case 0: return eAosNormalDoc;
	// 		case 1: return eAosAdminDoc;
	// 		case 2: return eAosAccessDoc;
	// 		case 3: return eAosLogDoc;
	// 		case 4: return eAosCounterDoc;
	// 		default:
	// 				OmnAlarm << "Unknown doc found: " << type << enderr;
	// 				return  eAosUnkonwDoc;
	// 	}
	// }

	static u64 getOwnDocid(const u64 &docid);

	static u32 getDocidSeqno(const u64 docid)
	{
		return u32(docid >> 32) & 0x00ffffff ;
	}

	static u32 getDocidOffset(const u64 docid)
	{
		return (u32)docid;
	}

	static void version10to11(char *bb, int &length);

	static bool readFromBuff(
		AosBuff &buff, 
		const u64 &did,
		u32 &seqno, 
		u64 &offset, 
		u64 &docsize, 
		u32 &seqSys, 
		u64 &offSys, 
		u64 &sizeSys, 
		const bool savenew,
		const AosRundataPtr &rdata);
};

#endif
