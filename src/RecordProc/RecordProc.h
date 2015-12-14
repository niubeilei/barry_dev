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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RecordProc_RecordProc_h
#define Aos_RecordProc_RecordProc_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"

#include <vector>
#include <map>
using namespace std;


class AosRecordProc : virtual public AosDataRecordObj,
					  public AosMemoryCheckerObj
{

protected:
	enum
	{
		eMaxDocids = 50,
		eBuffInitSize = 500,
		eBuffIncSize = 500,
	};

	AosDataRecordType::E		mType;
	OmnString					mName;
	AosXmlTagPtr				mRecordDoc;
	AosXmlTagPtr				mExtensionDoc;
	AosTaskObjPtr				mTask;
	u64							mDocid;
	vector<u64>					mDocids;
	int							mFieldLoopIdx;
	vector<AosDataFieldObjPtr>	mFields;
	map<int, AosValueRslt>		mFieldValues;
	OmnString   mInternalData;
	char      * mData;
	int         mDataLen;           // Current value actual length
	int         mRecordLen;         // The record length

	int			mProcRecordIndex;
	OmnString	mBuff;	
	int			mBuffSize;

public:
	AosRecordProc(
			const OmnString &name,
			const AosDataRecordType::E type,
			const AosTaskObjPtr &task,
			const bool flag AosMemoryCheckDecl);
	AosRecordProc();
	AosRecordProc(
			const AosRecordProc &rhs,
			const AosRundataPtr &rdata AosMemoryCheckDecl);
	~AosRecordProc();

	bool 	config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	
	// AosRecordProc interface
	virtual int			getRecordLen() const {return mRecordLen;}
	virtual int			getDataLen() const {return mDataLen;}
	virtual char *		getData() {return mData;}
	virtual const char *getData() const {return mData;}
	virtual AosDataRecordType::E	getType() const {return mType;}
	virtual AosXmlTagPtr			getRecordDoc() const {return mRecordDoc;}
	virtual int						getNumFields() const {return mFields.size();}
	virtual void					setTask(const AosTaskObjPtr &task){mTask = task;}
	virtual u64						getDocid() const {return mDocid;}
	virtual void 					setDocid(const u64 &docid) {mDocid = docid;}
	virtual bool 					setDocidByIdx(const int idx, const u64 &docid)
	{
		aos_assert_r(idx >= 0 && idx < eMaxDocids, false);
		if ((u32)idx >= mDocids.size()) mDocids.resize(idx+1);
		mDocids[idx] = docid;
		return true;
	}
	
	virtual u64 					getDocidByIdx(const int idx) const
	{
		aos_assert_r(idx >= 0 || (u32)idx < mDocids.size(), 0);
		return mDocids[idx];
	}

	virtual AosDataRecordObjPtr		createDataRecord(
										const AosXmlTagPtr &def,
										const AosTaskObjPtr &task,
										const AosRundataPtr &rdata AosMemoryCheckDecl);
	
	virtual AosXmlTagPtr			serializeToXmlDoc(
			                			const char *data,
										const int data_len,
										const AosRundataPtr &rdata);

	virtual AosBuffPtr				serializeToBuff(
			                			const AosXmlTagPtr &doc,
										const AosRundataPtr &rdata);

	virtual int						getFieldIdx(
										const OmnString &name,
										const AosRundataPtr &rdata);
	
	virtual AosXmlTagPtr 			getDataFieldConfig(
										const OmnString &name,
										const AosRundataPtr &rdata);

	virtual bool 					getRecordENV(
										const OmnString &name,
										OmnString &record_env,
										const AosRundataPtr &rdata);

	virtual void	clear();
	virtual AosDataRecordObjPtr     clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const;
	virtual AosDataRecordObjPtr     create(
                                    	const AosXmlTagPtr &def,
                                    	const AosTaskObjPtr &task,
                                    	const AosRundataPtr &rdata AosMemoryCheckDecl) const;
     virtual bool                    getField(
                                         const int idx,
                                         AosValueRslt &value,
                                         const bool copy_flag,
                                         const AosRundataPtr &rdata);
     virtual bool                    setField(
                                         const int idx,
                                         const AosValueRslt &value,
                                         const AosRundataPtr &rdata);
    virtual bool                    reset(const AosRundataPtr &rdata);
     virtual bool                    setData(
                                         char *data,
                                         const int len,
                                         const bool need_copy = false);
     virtual char *                  generateRecord(
                                         int &data_len,
                                         const AosRundataPtr &rdata);
	virtual OmnString               toString() const;
    bool                createRandomDoc(
                            const AosBuffPtr &buff,
                            const AosRundataPtr &rdata);

	static bool		staticInit();
	AosDataFieldType::E getFieldType(const int idx) const {return mFields[idx]->getType();}
	OmnString getFieldName(int &fieldindex);
	void	setIndexIncrease(){mProcRecordIndex += 1;}
	int 	getIndex(){return mProcRecordIndex;}
	bool	setRecordToBuff(OmnString &record)
	{
		mBuff<<record;
		mBuffSize++;
		return true;
	}
	int 	getRecordSize(){return mBuffSize;}
	OmnString getBuff(){OmnString str(mBuff.data(), mBuff.length()-1);return str;}
	bool resetBuff(const AosRundataPtr &rdata);

protected:
	bool registerRecord(const OmnString &name, AosRecordProc *record);
};

#endif

