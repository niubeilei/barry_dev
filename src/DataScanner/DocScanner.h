////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 08/07/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataScanner_DocScanner_h
#define AOS_DataScanner_DocScanner_h

#include "DataScanner/DataScanner.h"
#include "SEUtil/AsyncRespCaller.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BuffData.h"
#include "SEInterfaces/Ptrs.h"
#include <vector>

class AosDocScanner : public AosDataScanner  , public AosAsyncRespCaller 
{
	OmnDefineRCObject;

	enum
	{
		eMaxNumDocsPerTrans = 1000000,
		eBlockSize = 10000000,
		eMaxMemory = 1000000000
	};

	struct StDocids
	{
		bool			loading;
		vector<u64> 	docids;

		StDocids()
		:
		loading(false)
		{
		}

		void set(const u64 &docid)
		{
			docids.push_back(docid);
		}
		u32 size(){return docids.size();}
		bool isLoading() {return loading;}

		bool serializeToBuff(const AosBuffPtr &buff, u32 &num_docs)
		{
			// This function serializes its contents into 'buff'
			loading = true;
			if (num_docs > docids.size()) num_docs = docids.size();
			aos_assert_r(num_docs > 0, false);
			buff->setBuff((char *)docids.data(), num_docs * sizeof(u64)); 	
			return true;
		}

		void removeDocids(const u64 &end_docid)
		{
			loading = false;
			vector<u64>::iterator itr = docids.begin();
			u32 num = 0;
			for (; itr != docids.end(); ++itr)
			{
				num++;
				if (*itr == end_docid) break;
			}
			aos_assert(*itr == end_docid);

			u32 size = docids.size();
			docids.erase(docids.begin(), ++itr);
			aos_assert(num + docids.size() == size);
		}
	};

	OmnMutexPtr 				    mLock;
	OmnCondVarPtr  				 	mCondVar;
	AosBuffPtr						mFieldsInfo;
	map<int, StDocids>				mDocids;
	queue<AosBuffPtr>				mQueue;
	u64								mTotalReceivedSize;
	u32 							mCrtVidIdx;
	AosBuffPtr						mSizeIdLen;

public:
	AosDocScanner(const bool flag);
	AosDocScanner(const bool flag, const AosRundataPtr &rdata);
	AosDocScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosDocScanner();

	virtual bool   getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	// Chen Ding, 2013/11/25
	// virtual int64_t getTotalFileLength();
	virtual int64_t getTotalSize() const;
	virtual	bool initDocScanner(    
					const vector<u64> &docids,       
					const AosRundataPtr &rdata);   
	virtual AosJimoPtr cloneJimo()  const;
	virtual int getPhysicalId() const;
	bool    initDocScanner(      
			const AosRundataPtr &rdata,
			const u64 *docids,
			const int64_t num_docids,
			const AosBuffPtr &field_info);

	bool	getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata);

	bool	signal(const int threadLogicId);

	virtual void 	callback(
				const AosTransPtr &trans,
				const AosBuffPtr &resp,
				const bool svr_death);
private:
	bool	config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	
	bool	createDocScanner(
				vector<AosDataScannerObjPtr> &scanners,
				const AosRundataPtr &rdata);

	bool 	readData(const AosRundataPtr &rdata, const int &vid);
	
	void	addRequestLocked(const AosRundataPtr &rdata);

	bool	sanityCheck();
	bool    shuffer(      
			map<u64, int> &sizeidmap,        
			const u64 &docid,
			const AosRundataPtr &rdata); 
};
#endif

