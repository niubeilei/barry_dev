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
// 2014-11-24 Created by White Wu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RawSE_RawSE_H_
#define Aos_RawSE_RawSE_H_

#include "RawSE/MediaType.h"
#include "RawSE/Ptrs.h"
#include "RawSE/RawFile.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosRawSE : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	struct AosRawFileTagHash
	{
		inline size_t operator()(const AosRawFile::Identifier &entry) const
		{
			return (u32)entry.aseID + (entry.aseID >> 32) + (u32)entry.rawFileID + (entry.rawFileID >> 32)
					+ entry.siteID + entry.cubeID;
		}
	};

	struct aos_compare_raw_file_tag
	{
		inline bool operator()(const AosRawFile::Identifier &e1, const AosRawFile::Identifier &e2) const
		{
			return e1 == e2;
		}
	};

	typedef std::hash_map<const AosRawFile::Identifier, AosRawFilePtr, AosRawFileTagHash, aos_compare_raw_file_tag> map_t;
	typedef map_t::iterator itr_t;

private:

	struct config
	{
		u16 level1size;
		u16 level2size;
		u16 level3size;
		u16 cacheSize;
	};

	OmnMutex*				mLockRaw;
	OmnMutexPtr				mLock;
	AosRawfidMgrPtr 		mRawfidMgr;
	AosPartitionIDMgrPtr	mPartitionIDMgr;
	AosRawFileHeaderPtr		mRawFileHeader;
	map_t					mRawFileMap;
	AosRawFilePtr			mpHead;				//for aging
	AosRawFilePtr			mpTail;				//for aging
	config					mConfig;
	bool					mIsReady;

public:
	AosRawSE(AosXmlTagPtr	&conf);
	virtual ~AosRawSE();

	bool config();

	bool isReady(){return mIsReady;};

	AosRawFilePtr createRawFile(
			AosRundata					*rdata,
			const u32					siteid,
			const u32					cubeid,
			const u64					aseid,
			const AosMediaType::Type	media_type,
			const AosRawFile::Type		eType,
			u64 						&rawfid);

	AosRawFilePtr createRawFile(
			AosRundata					*rdata,
			const u32					siteid,
			const u32					cubeid,
			const u64					aseid,
			const u64 					rawfid,
			const AosMediaType::Type	media_type,
			const AosRawFile::Type		eType);

	int readRawFile(
			AosRundata				*rdata,
			const u32				siteid,
			const u32				cubeid,
			const u64				aseid,
			const u64 				rawfid,
			const u64				offset,
			const u64				length,
			AosBuffPtr				&buff,
			const AosRawFile::Type	eType = AosRawFile::eReadWrite,
			const bool				bShowAlarm = true);

    int appendData(
     		AosRundata				*rdata,
     		const u32				siteid,
     		const u32				cubeid,
     		const u64				aseid,
     		const u64 				rawfid,
     		const char *const 		data,
     		const u64 				length,
			const AosRawFile::Type	eType = AosRawFile::eReadWrite);

    int deleteFile(
    		AosRundata				*rdata,
			const u32				siteid,
			const u32				cubeid,
			const u64				aseid,
			const u64 				rawfid,
			const AosRawFile::Type	eType = AosRawFile::eReadWrite);

    AosRawFilePtr getRawFile(
    		AosRundata				*rdata,
			const u32				siteid,
			const u32				cubeid,
			const u64				aseid,
			const u64				rawfid,
			const AosRawFile::Type	eType);

    int overwriteRawFile(
    		AosRundata			*rdata,
    		const u32			siteid,
    		const u32			cubeid,
    		const u64			aseid,
    		const u64			rawfid,
    		const char *const 	data,
    		const u64 			length);

    bool fileExists(
    		AosRundata	*rdata,
    		const u32	siteid,
    		const u32	cubeid,
    		const u64	aseid,
    		const u64	rawfid);

    void dumpRawfileMap(const char* pCaller);

	OmnString getBaseDir();

private:
    int agingOut(
    		AosRundata		*rdata,
    		AosRawFilePtr	pRawFile);

    int moveToHeadInDLinkedList(
    		AosRundata		*rdata,
    		AosRawFilePtr	pRawFile);
};
#endif
