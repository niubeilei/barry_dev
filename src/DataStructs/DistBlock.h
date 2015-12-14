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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStructs_DistBlock_h
#define AOS_DataStructs_DistBlock_h

#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosDistBlock : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eAosInvBlockId = 0xffffffff,
	    //eDfmDistBlockSize = 400000 // 400K
	    eDfmDistBlockSize = 10000000 // 10M
	};
private:
	AosBuffPtr				mDistributionBlock;
	char *					mDistBlockData;
	u64						mDistBlockDocid;
	u32						mDistBlockId;
	int						mDistBlockVid;
	u64						mStartDocid;

	int						mStatDocSize;
	int 					mDocsPerDistBlock;
	bool					mNewDistBlock;
	int						mDftValue;

public:
	AosDistBlock();
	AosDistBlock(const int stat_doc_size, const int dft);
	~AosDistBlock();

	void set(
			const AosBuffPtr &buff, 
			const u64 &dist_block_did, 
			const u32 dist_block_id,
			const bool &new_dist_block);

	void clear(); 
	char * getStatDoc(const u64 &docid);
	void setStatDoc(const u64 &docid, const AosBuffPtr &buff);
	int getStatDocSize() const {return mStatDocSize;}
	u64 getDocid() const {return mDistBlockDocid;}
	u32 getGroupId() {return mDistBlockId;}
	u64 getStartDocid() {return mStartDocid;}
	int getDistBlockVid() const {return mDistBlockVid;}

	int64_t dataLen() const 
	{
		if (!mDistributionBlock) return 0;
		return mDistributionBlock->dataLen();
	}
	AosBuffPtr getDistributionBlock() const {return mDistributionBlock;}

	int getDocsPerDistBlock() const {return mDocsPerDistBlock;}

	static int calculateDocPerDistBlock(const int stat_doc_size) 
	{
		return  eDfmDistBlockSize / stat_doc_size;
	};

	bool isNew(){return mNewDistBlock;}

	static void setStatDocDftValue(
			char *data,
			const int dft,
			const int64_t &len);
};
#endif
