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
// 02/29/2012 Created by Chen Ding
// 06/03/2012 Moved from StorageEngineObj.h
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_StorageEngineObj_h
#define Aos_SEInterfaces_StorageEngineObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/DocTypes.h"

#include <vector>

class AosStorageEngineObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum E
 	{
		eDftMaxGroupedDocFileSize = 4000000,	// 4M
		eMaxDocPerGroup = 4096
	};

protected:
	AosDocType::E		mType;
	static u64			smMaxGroupedDocFileSize;
	static bool			smInited;

public:
	AosStorageEngineObj(const AosDocType::E type, const bool flag);

	//virtual bool		saveToFile(
	//						const u64 &docid, 
	//						const AosXmlTagPtr &doc,
	//						const bool savenew, 
	//						const AosDocFileMgrObjPtr &docfilemgr, 
	//						const AosRundataPtr &rdata) = 0;

	//virtual AosXmlTagPtr readFromFile(
	//						const u64 &docid, 
	//						const int record_len,
	//						const AosDocFileMgrObjPtr &docfilemgr, 
	//						const AosRundataPtr &rdata) = 0;
	
	virtual AosXmlTagPtr serializeToXmlDoc(
							const AosXmlTagPtr &xml,
							const AosDataRecordObjPtr rcd,
							const AosRundataPtr &rdata) = 0;

	//virtual bool		deleteDoc(
	//						const u64 &docid, 
	//						const AosDocFileMgrObjPtr &docfilemgr, 
	//						const AosRundataPtr &rdata) = 0;

	virtual AosBuffPtr readFixedDocsFromFile(
							const u64 &docid, 
							int &num,
							const AosDocFileMgrObjPtr &docfilemgr, 
							const AosRundataPtr &rdata) = 0;
	
	virtual AosBuffPtr		readFixedDocsFromFile(
								const AosRundataPtr &rdata,
								const vector<u64> &docids,
								const int record_size) = 0;

	virtual AosBuffPtr		readCSVDocsFromFile(
								const AosRundataPtr &rdata,
								const vector<u64> &docids,
								const int record_size) = 0;

	virtual AosBuffPtr		readCommonDocsFromFile(
								const AosRundataPtr &rdata,
								const vector<u64> &docids,
								const int record_size) = 0;


	//virtual bool		deleteDocsFromFile(
	//						const u64 &sizeid, 
	//						const int &num_records,
	//						const int &record_size,
	//						const u32 &seqno,
	//						const u64 &offset,
	//						const AosDocFileMgrObjPtr &docfilemgr,
	//						const AosRundataPtr &rdata) = 0; 

	virtual bool		addReq(
							const AosSengineDocReqObjPtr &req,
							const AosRundataPtr &rdata) = 0;

	virtual AosDocFileMgrObjPtr retrieveDocFileMgrByKey(
							const u32   virtual_id,
							const OmnString dfm_key,
							const bool create_flag,
							const AosRundataPtr &rdata) = 0;

	virtual void flushContents() = 0;

	static AosStorageEngineObjPtr getStorageEngine(const AosDocType::E type);
	// Chen Ding, 2013/03/24
	inline static u64 getGroupedDocGroupSize() {return smMaxGroupedDocFileSize;}

	//static void storageEngineInited() {smInited = true;}
	static void storageEngineInited(const AosXmlTagPtr &config);

private:
	bool registerStorageEngineObj(const AosStorageEngineObjPtr &storage_eng);
};
#endif

