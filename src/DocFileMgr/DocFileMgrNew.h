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
// 2015/05/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DocFileMgrNew_h
#define AOS_DocFileMgr_DocFileMgrNew_h



class AosDocFileMgrNew : public AosDocFileMgrObj_3_1
{
	OmnDefineRCObject;

private:
	u32				mVirtualId;
	u32				mDFMID;
	AosDfmConfig	mConfig;

public:
	AosDocFileMgrNew(
		AosRundata *rdata,
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmConfig &config);
	~AosDocFileMgrNew();
	
	virtual u32	 getVirtualId() const {return mVirtualId;};
	virtual bool stop();
	virtual bool startStop();
	virtual AosDfmDocType::E getDocType(){ return mConfig.mDocType; };

	// Doc Operations
	virtual bool createDoc(AosRundata* rdata, 
					const u64 docid,
					const u64 snap_id,
					const AosBuff &body);
	virtual bool modifyDoc(AosRundata* rdata, const AosDfmDocPtr &doc);
	virtual bool deleteDoc(AosRundata *rdata, const AosDfmDocPtr &doc);
	virtual bool readDoc(AosRundata *rdata, AosBuffPtr &body_data);
	virtual bool removeAllFiles(AosRundata *rdata);

	// Snapshot Operations
	virtual u64 createSnapshot(AosRundata *rdata, const u64 snap_id);	
	virtual bool commitSnapshot(AosRundata *rdata, const u64 snap_id);
	virtual bool rollbackSnapshot(AosRundata *rdata, const u64 snap_id);
	virtual bool mergeSnapshot(AosRundata *rdata, const u64 target_snap_id, const u64 merger_snap_id);
};
#endif
