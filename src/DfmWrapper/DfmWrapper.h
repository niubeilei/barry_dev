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
// 2015/09/02 Created by Gavin
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DfmWrapper_DfmWrapper_h
#define Aos_DfmWrapper_DfmWrapper_h

#include "SEInterfaces/DocFileMgrObj.h"
#include "DfmUtil/DfmConfig.h"
#include "alarm_c/alarm.h"      
#include "Rundata/Ptrs.h"
#include "DfmUtil/DfmDoc.h"
#include "DocFileMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
//#include "ReliableFile/Ptrs.h"
#include "Util/TransId.h"
#include "Util/MemoryChecker.h"
#include "Util/Ptrs.h"
#include "Util/Ptrs.h"
#include "DfmWrapper/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "JimoAPI/Jimo.h"
#include "DocFileMgr/DfmLog.h"



class AosDfmWrapper : public AosDocFileMgrObj
{
	OmnDefineRCObject;

private:
	AosDfmDocType::E	mAseId;
	AosDfmConfig    	mConfig;
	AosDfmLogPtr    	mDfmLog;
	u32					mCubeID;
	
public:
	AosDfmWrapper(
			AosRundata			*rdata,
			const u32			cubeid,
			const AosDfmConfig	&config);
	virtual ~AosDfmWrapper();
	
	AosDfmWrapperPtr nextEntry() const;
	AosDfmWrapperPtr prevEntry() const;
	void resetPointers();
	void setPointers();
	bool moveToFront(const AosDfmWrapperPtr &docFileMgr);
	bool insertAt(const AosDfmWrapperPtr &docFileMgr);
	bool removeFromList();

	virtual bool init();

	virtual bool saveDoc(const AosRundataPtr &rdata,
					vector<AosTransId> &trans_ids,
					const AosDfmDocPtr &doc,
					const bool flushflag = false);

	virtual bool deleteDoc(const AosRundataPtr &rdata,
			        vector<AosTransId> &trans_ids,
					const AosDfmDocPtr &doc,
					const bool flushflag = false);
	
	virtual bool saveDoc(const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const AosDfmDocPtr &doc,
					const bool flushflag = false);
	
	virtual bool deleteDoc(const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const AosDfmDocPtr &doc,
					const bool flushflag = false);

	virtual AosDfmDocPtr readDoc(const AosRundataPtr &rdata,
						const u64 docid,
						const bool read_body = true);

	virtual bool addHeaderFile(
					const AosRundataPtr &rdata,
					const AosDfmHeaderFilePtr &file);
	virtual bool addBodyFile(
					const AosRundataPtr &rdata,
					const AosDfmFilePtr &file);
	virtual bool removeAllFiles(const AosRundataPtr &rdata);

	virtual bool 	recoverDfmLog(const AosRundataPtr &rdata, const u64 &file_id);

	virtual bool		stop();
	virtual bool		startStop();
	virtual u32			getId();
	virtual u32			getVirtualId();
	
	virtual AosDfmDocType::E getDocType() {return mAseId;}
	virtual u64 getASEID();

	virtual bool saveDocs(
			const AosRundataPtr &rdata,
			map<u64, u64> &index_map,
			const AosBuffPtr &buff);

	virtual bool saveHeader(const AosRundataPtr &rdata, 
					const AosDfmDocPtr &doc);

	virtual u64 createSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosTransId &trans_id);

	virtual bool commitSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosTransId &trans_id);

	virtual bool rollbackSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosTransId &trans_id);
	
	virtual bool mergeSnapshot(
				const AosRundataPtr &rdata,
				const u64 target_snap_id,
				const u64 merger_snap_id,
				const AosTransId &trans_id);

	virtual AosDfmDocPtr readDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const u64 docid,
				const bool read_body = true);

	virtual bool saveDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosDfmDocPtr &doc,
				vector<AosTransId> &trans_ids);

	virtual bool deleteDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosDfmDocPtr &doc,
				vector<AosTransId> &trans_ids);

	virtual AosDfmDocPtr readDocFromDfm(
				const AosRundataPtr &rdata,
				const u64 docid,
				const bool read_body);

private:
	u64 getGlobalDocidByLocalDocid(const u64 docid);

};

#endif

