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
// File format is:
// 		length 			(4 bytes)
// 		entry type		(char)
// 		front poison	(u32)
// 		reference count	(u32)
// 		flags			(u32)
// 		signature		(optional)
// 		data			(variable)
// 		length			(4 bytes)
//
// Modification History:
// 06/23/2011	Created by Tom
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogSvr_SeLogEntry_h
#define AOS_SeLogSvr_SeLogEntry_h

#include "XmlUtil/XmlTag.h"
#include "ReliableFile/Ptrs.h"
#include "SeLogSvr/Ptrs.h"
#include "SeLogUtil/LogType.h"
#include "XmlUtil/SeXmlUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosSeLogEntry : public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum
	{
		eDeleteFlag = 1,
		eSignatureFlag = 0x02,
		
		eSignatureLen = 16,
		ePosition = 6,
		eHeaderLength = 17,	// Length(4), Type(1), Poison(4), RefCount(4), Flags(4)
		eMaxHeaderLength = 30,

		eDftMaxLogLength = 1000000,
		eDftMaxVersions = 100,
		eEndFlag = 0xffffffff
	};

protected:
	u32							mRefCount;
	u32							mFlags;
	u32							mEntryLength;
	AosXmlTagPtr				mDoc;
	OmnString					mIILName;
	AosLogType::E				mLogType;
	OmnString 					mSignature;
	bool						mCanDeletable;
	int 						mMaxEntrySize;
	u32 						mSignatureFlag;

	static AosSeLogEntryPtr		smVersionReader;
	static AosSeLogEntryPtr		smNormReader;

public:
	AosSeLogEntry(const AosLogType::E type);
	
	~AosSeLogEntry();

	static AosSeLogEntryPtr 			getLogEntry(
										const int maxEntrySize,
										AosBuff &buff,
										const AosLogType::E type);
	
	static AosSeLogEntryPtr 			getLogEntry(
										const AosXmlTagPtr &ctnr_doc,		
										const OmnString &container,
										const OmnString &logname,
										const AosLogType::E type,
										const AosRundataPtr &rdata);
	
	static AosSeLogEntryPtr 			getLogEntry(
										const AosLogType::E type, 
										const AosRundataPtr &rdata);
	
	// AosSeLogEntry Interface
	virtual AosSeLogEntryPtr 			clone() = 0;
	
	virtual void 						setContentsToBuff(AosBuff &buff) = 0;
	
	virtual u64 						appendToFile(
										const AosSeLogPtr &selog,
										const u32 &seqno,
										const AosXmlTagPtr &log,
										//const OmnFilePtr &file,
										const AosReliableFilePtr &file,
										u64 &writePos,
										const AosRundataPtr &rdata);

	virtual AosXmlTagPtr				readBody(
										//const OmnFilePtr &file,
										const AosReliableFilePtr &file,
										const u64 &offset,
										const u32 length,
										const AosRundataPtr &rdata);

	// This function reads in the log identified by 'logid'.
	static AosXmlTagPtr					readFromFile(
									   	const u64 &logid,
										const AosRundataPtr &rdata);

	static AosXmlTagPtr					readFromFile(
										//const OmnFilePtr &file, 
										const AosReliableFilePtr &file,
										const u64 &offset,
										u32 &length,
										const AosRundataPtr &rdata);
    
	bool 								deleteLog(
										const u64 &logid,
										const OmnString &ctnr_objid,
										const AosXmlTagPtr &log,
										const AosRundataPtr &rdata);

	bool 								canDelete() const 
										{
											return (mRefCount == 0) && (mFlags == 0) && mCanDeletable;
										}

	AosXmlTagPtr 						getDoc() const;
	
	AosLogType::E 						getType() const 
										{
											return mLogType;
										}

	bool 								isVersionEntry()
										{
											return mLogType == AosLogType::eVersion;
										}
	
	u32 								getHeaderLength() const 
										{
											return eHeaderLength;
										}

	u32 								getSignatureLen() const 
										{
											return (withSignature()) ? eSignatureLen : 0;
										}
	
	 void 								setIILName(const OmnString &iilname)
	 									{
 											mIILName = iilname;
										}
protected:
	void 								setSignature(
										AosBuff &buff, 
										const OmnString &logstr);

private:
	
	static AosXmlTagPtr					readHeaderStatic(
										//const OmnFilePtr &file,
										const AosReliableFilePtr &file,
										const u64 &offset,
										u32 &length,
										const AosRundataPtr &rdata);
	
	bool 								calculateSignature(
										AosBuff &buff, 
										const OmnString &logstr);
	
	bool inline 						withSignature() const 
										{
											return (mFlags & eSignatureFlag);
										}
	
	void inline 						setNotDeletable()
										{
											mCanDeletable = false;
										}
};
#endif
