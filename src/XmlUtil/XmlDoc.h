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
#ifndef Aos_XmlUtil_XmlDoc_h
#define Aos_XmlUtil_XmlDoc_h

#include "aosUtil/Types.h"
#include "IdGen/IdGenMgr.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "Util/TransId.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/MemoryChecker.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/DocTypes.h"


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

class AosXmlDoc : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eVersion10 = 10,
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
		eVersion11HeaderSize = 64,
		eCompressSize = 10000 //10K
	
	};

	static OmnMutexPtr 		smLock;
	static bool				smNeedCompress;	// Chen Ding DDDDD, 2013/01/26
	static u32				smCompressSize;	// Chen Ding DDDDD, 2013/01/26

public:
	AosXmlDoc();
	~AosXmlDoc();

	static bool	staticInit(const AosXmlTagPtr &config);
	static bool	stop();

	static bool	saveToFile(      
			const u64 &docid,   
			const char *doc,
			const int64_t &data_len,
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const AosTransId &trans_id,
			const AosRundataPtr &rdata);

	static bool saveToFile(
			const u64 &docid, 
			const AosXmlTagPtr &doc, 
			const bool savenew, 
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const AosTransId &trans_id,
			const AosRundataPtr &rdata);

	static bool deleteDoc(
			const u64 &docid, 
			const AosDocFileMgrObjPtr &docfilemgr, 
			const u64 &snap_id,
			const AosTransId &trans_id,
			const AosRundataPtr &rdata);

	static bool isDocDeleted(
			const u64 &docid, 
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const AosRundataPtr &rdata);

	static AosXmlTagPtr readFromFile(
			const u64 &docid,
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const AosRundataPtr &rdata AosMemoryCheckDecl);	

	static OmnString	readHeader(
			const u64 &docid, 
			const AosDocFileMgrObjPtr &docfilemgr, 
			const u64 &snap_id,
			const AosRundataPtr &rdata);
   
	static int	getMaxHeaderPerFile() {return eMaxHeaderPerFile;}
	static int  getHeaderSize() {return eDocHeaderSize;}

	// Chen Ding, 02/29/2012, Moved to DocTypes.h
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

	static inline u64 getOwnDocid(const u64 docid)
	{
 		return (docid & 0x00ffffffffffffffULL);
	}

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

	static bool saveBinaryDoc(
			const u64 &docid,
			const AosBuffPtr &docBuff,
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const AosTransId &trans_id,
			const AosRundataPtr &rdata);

	static AosBuffPtr readBinaryDoc(
			const u64 &docid,
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const AosRundataPtr &rdata);

	static AosBuffPtr readFromFileTester(
			const u64 &docid,
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const AosRundataPtr &rdata AosMemoryCheckDecl);	

private:
	static bool saveToFilePriv(
					const u64 loc_did,
					const AosDocType::E type,
					const u32 siteid,
					const AosBuffPtr &doc_buff, 
					const AosDocFileMgrObjPtr &docfilemgr,
					const AosTransId &trans_id,
					const AosRundataPtr &rdata);


	static bool saveToSnapshotFilePriv(
					const u64 loc_did,
					const AosDocType::E type,
					const u32 siteid,
					const AosBuffPtr &doc_buff, 
					const AosDocFileMgrObjPtr &docfilemgr,
					const u64 &snap_id,
					const AosTransId &trans_id,
					const AosRundataPtr &rdata);

	static bool isCompress(const u64 &docsize);
};

#endif
