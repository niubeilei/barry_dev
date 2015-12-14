////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This class creates a container of data records. It is a subclass
// of AosDataRecord. Records in the container must be of the same
// type.
//
//
// Modification History:
// 2012/05/05 Created by Ice Yu
// 2014/11/19 Modified by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordContainer_h
#define Aos_DataRecord_RecordContainer_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "DataRecord/DataRecord.h"
#include "DataRecord/Recordset.h"
#include "Util/Ptrs.h"


class AosRecordContainer : public AosDataRecord
{
	friend class AosRecordset;
	friend class AosStreamRecordset;
	enum
	{
		eDftBuffSize = 100 * 1000,
		eDftIncreaseSize = 1000 * 1000 * 1000,
		eMaxBuffSize = 2 * 1000 * 1000 * 1000
	};

	AosBuffPtr			mBuff;
	AosBuff				*mBuffRaw;

	int64_t				mCrtRecordIdx;
	int 				mNumRecord;
	AosDataRecordObjPtr	mRecord;
	AosDataRecordObj*	mRecordRaw;
	vector<int>			mRecordLens;

	AosDataAssemblerObj* mAssembler;

public:
	AosRecordContainer(const bool flag AosMemoryCheckDecl);
	AosRecordContainer(
			const AosRecordContainer &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordContainer();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosRecordContainer interface
	virtual bool		isFixed() const {return mRecordRaw->isFixed();}

	virtual int			getRecordLen() {return mRecordRaw->getRecordLen();}
	virtual int			getEstimateRecordLen() {return mRecordRaw->getEstimateRecordLen();} 

	virtual char *		getData(AosRundata *rdata) {return mBuffRaw->data();}
	virtual bool 		setData(
							char *data, 
							const int len, 
							AosMetaData *metaData, 
							int &status);
							//const int64_t offset);
	virtual void 		clear();

	virtual AosDataRecordObjPtr clone(AosRundata *rdata AosMemoryCheckDecl) const;
	virtual AosDataRecordObjPtr create(
							const AosXmlTagPtr &def,
							const u64 task_docid,
							AosRundata *rdata AosMemoryCheckDecl) const;

	virtual bool		setFieldValue(
							const int idx, 
							AosValueRslt &value, 
							bool &outofmem,
							AosRundata* rdata);

	virtual bool		setFieldValue(
							const OmnString &field_name, 
							AosValueRslt &value,
							bool &outofmem,
							AosRundata* rdata);

	virtual bool		getFieldValue(
							const int idx,
							AosValueRslt &value,
							const bool copy_flag,
							AosRundata* rdata);

	/*
	//yang
	virtual bool 				replaceField(
									const int idx,
									const AosDataFieldObjPtr& newfield);
	*/


	virtual int			getFieldIdx(
							const OmnString &name,
							AosRundata *rdata);

	virtual void		flush(const bool clean_memory = false) 
	{ 
		if (mIsDirty)
		{
			//aos_assert(mRecord);
			if (!((AosDataRecord*)mRecordRaw)->isDirty())
			{
				OmnAlarm << enderr;
			}
			mNumRecord ++; 
			int rcd_len = mRecordRaw->getRecordLen();
			mCrtRecordIdx += rcd_len;
			mBuffRaw->setDataLen(mCrtRecordIdx);
			mRecordLens.push_back(rcd_len);
			mRecordRaw->flush();
			mIsDirty = false;
			if (clean_memory) mRecordRaw->clear();	// Ken Lee, 2014/10/21
		}
	}

	virtual void			reset()
	{
		mRecordRaw->flush();
	}

	virtual void		flushRecord(AosRundata *rdata_raw);

	virtual void				setDataAssembler(AosDataAssemblerObj* assemble)
	{
		mAssembler = assemble;
	}

	virtual int	 		getNumRecord() {return mNumRecord;}
	virtual bool		getRecordLens(vector<int> &rcd_lens){rcd_lens = mRecordLens; return true;}
	virtual int			getNumFields() const {return mRecordRaw->getNumFields();}		// Ketty 2014/05/08
	virtual vector<AosDataFieldObjPtr> getFields() const {return mRecordRaw->getFields();}
	virtual AosDataFieldObj* getFieldByIdx1(const u32 idx){return mRecordRaw->getFieldByIdx1(idx);};

	virtual u64       getDocid() const {return mRecordRaw->getDocid();}
	virtual void 				setDocid(const u64 &docid) {mDocid = docid;}
	virtual AosDataRecordObj*   getInsideRecord() { return mRecordRaw; }

	AosXmlTagPtr serializeToXmlDoc(
			const char *data, 
			const int data_len,
			AosRundata* rdata);

private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);

	bool	increaseMemory();
};

#endif

