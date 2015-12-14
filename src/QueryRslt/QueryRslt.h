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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryRslt_QueryRslt_h
#define Aos_QueryRslt_QueryRslt_h

#include "DataTypes/DataColOpr.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"
#include "XmlUtil/Ptrs.h"



class AosQueryRslt : public AosQueryRsltObj
{
	OmnDefineRCObject;
	enum
	{
		eInitMemSize = 10,
		eSmallDocNum = 300,
		eLargeDocNum = 10000,
		
		eDefaultBlockSize = 30,
		eBlockSize = 500000,
		eBuffInitSize = 10000,
		eBuffIncSize = 100000,
		eDefaultRcdLen = 100,
		eDefaultRcdSize = 100,
	};

private:
	i64				mCrtIdx;
	u64*			mDocids;
	i64				mNumDocs;
	i64				mMemCap;
	bool			mDataReverse;

	bool			mWithValues;			// Chen Ding, 08/12/2011
	
	AosBuffArrayPtr	mValues;
//	AosBuffPtr		mValueBuff;
	AosDataColOpr::E        mOperator;      // felicia, 2013/03/13
	u32				mRcdLen;
	OmnString 		mValueType;
public:
	AosQueryRslt();
//	AosQueryRslt(const i64 &memsize);
	~AosQueryRslt();

	// static AosQueryRsltObjPtr getQueryRslt();
	// static void returnQueryRslt(const AosQueryRsltPtr &rslt);

	AosCompareFunPtr createCompareFunc(const OmnString &type, const int &len);
	virtual AosQueryRsltObjPtr getQueryRslt();
	virtual void returnQueryRslt(const AosQueryRsltObjPtr &rslt);

	virtual void	setRcdLen(const u32 len){mRcdLen = len;}
	virtual bool isEmpty()const{return mNumDocs == 0;}
	virtual void reset()
	{
		mCrtIdx = 0;
	}	

	
	virtual void	setWithValues(const bool withvalue)
	{
		mWithValues = withvalue;
		AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
		comp_func->setReverse(mDataReverse);
		mValues = OmnNew AosBuffArray(
		        comp_func, false, false, eDefaultRcdSize*mRcdLen);
		mValues->setAutoExtend(true,10);
	}
	virtual bool	fillValueWithDocid();
	
	virtual bool	isDataReverse() const {return mDataReverse;}

//	virtual AosBuffPtr getValueBuff() const {return mValueBuff;}
	virtual bool	serializeFromXml(const AosXmlTagPtr &xml);
	virtual bool 	isWithValues() const {return mWithValues;}
	virtual void 	clear();
	virtual void 	setDataReverse(const bool reverse);
	virtual i64		getNumDocs(){return mNumDocs;}
	virtual i64		getNumLeft();
	virtual bool 	mergeFrom(const AosQueryRsltObjPtr &rslt);
	virtual bool	mergeFrom(const AosQueryRsltObjPtr &rhs_rslt,const u64 &startidx, const u64 &len);

	virtual bool 	appendDocid(const u64 docid);
	virtual void 	setNumDocs(const i64 &numDocs) { mNumDocs = numDocs;}
	virtual i64 	getTotal(const AosRundataPtr &rdata) const {return mNumDocs;}
	virtual bool 	checkDocid(const u64 &docid, const AosRundataPtr &rdata);
	virtual u64* 	getDocidsPtr()const{return mDocids;}
	virtual bool 	appendBlock( const u64* docs, const i64 &num_docs);
	virtual u64  	nextDocid(bool &finished);
	virtual void 	clean(){clear();}
	virtual bool 	appendBlockRev(const u64* docs, const i64 &num_docs);
	virtual bool 	nextDocidValue(
						u64 &docid, 
						OmnString &value, 
						const AosRundataPtr &rdata);
	virtual bool 	nextDocidValue(
						u64 &docid, 
						OmnString &value, 
						bool &finished, 
						const AosRundataPtr &rdata);
	virtual bool 	nextDocidValue(
						u64 &docid, 
						u64 &value, 
						bool &finished, 
						const AosRundataPtr &rdata);
	virtual bool 	nextDocidValue(
						u64 &docid, 
						d64 &value, 
						bool &finished, 
						const AosRundataPtr &rdata);
	virtual bool 	nextDocidValue(
						u64 &docid, 
						i64 &value, 
						bool &finished, 
						const AosRundataPtr &rdata);
	virtual AosBuffArrayPtr getValues()const;

	
	void			setNoData();

	// Chen Ding, 2013/01/27
	virtual bool 	exchangeContent(
						const AosQueryRsltObjPtr &rslt1,
						const AosQueryRsltObjPtr &rslt2);
	
	virtual OmnString	getLastValue()const;
	virtual bool		getEntriesBeforeValue(
							const AosBuffArrayPtr &buff_array,
							const OmnString last_value,
							const bool &reverse);

	virtual bool 	moveTo(const i64 &pos, bool &finished, const AosRundataPtr &rdata);
	bool 			filterByBitmap(const AosBitmapObjPtr &bitmap);
	bool 			backOffStep(const i64 &step);
	virtual bool 	nextDocid(u64 &docid, bool &finished, const AosRundataPtr &rdata);
	virtual bool	hasMoreDocid();
	bool 			nextU64DocidValue(u64 &docid, u64 &value, bool &finished, const AosRundataPtr &rdata);
//	bool 			nextValue(OmnString &value,const AosRundataPtr &rdata);
//	bool 			nextU64Value(u64 &value,const AosRundataPtr &rdata);

	virtual bool	serializeToXml(AosXmlTagPtr &xml);

	virtual void resetByValue()
	{
		reset();
//		if (mValues) mValues->resetRecordLoop();
//		if (mValueBuff) mValueBuff->reset();
	}
	// Lynch 07/19/2011
	void resetByIndex()
	{
		for (i64 crtIndex = 0; crtIndex < mNumDocs; crtIndex ++)
		{
			mDocids[crtIndex] = crtIndex + 1;
		}
	}

	virtual AosBuffArrayPtr getValueBuff() const{return mValues;}
	virtual bool appendStrValue(const char *str)
	{
		if(!mValues)
		{
			AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
			comp_func->setReverse(mDataReverse);
			mValues = OmnNew AosBuffArray(
			        comp_func, false, false, eDefaultRcdSize*mRcdLen);
			mValues->setAutoExtend(true,10);
		}

		aos_assert_r(mValues, false);
		aos_assert_r(str, false);

		u32 len = strlen(str)+1;
		//if(len > mRcdLen)len = mRcdLen;
		if(len > mRcdLen)mRcdLen = len;
		mValues->appendEntry(str, len, 0);
	 	return true;
	}

	virtual bool appendI64Value(const i64 &val)
	{
		mRcdLen = sizeof(i64);
		if(!mValues)
		{
			mValueType = "i64";
			AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
			comp_func->setReverse(mDataReverse);
			mValues = OmnNew AosBuffArray(                                                                           
			        comp_func, false, false, eDefaultRcdSize*mRcdLen);
			mValues->setAutoExtend(true,10);
		}
		aos_assert_r(mValues,false);
		mValues->appendEntry((char*)(&val), sizeof(i64), 0);
	 	return true;
	}
	
	virtual bool appendD64Value(const d64 &val)
	{
		mRcdLen = sizeof(d64);
		if(!mValues)
		{
			mValueType = "double";
			AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
			comp_func->setReverse(mDataReverse);
			mValues = OmnNew AosBuffArray(                                                                           
			        comp_func, false, false, eDefaultRcdSize*mRcdLen);
			mValues->setAutoExtend(true,10);
		}
		aos_assert_r(mValues,false);
		mValues->appendEntry((char*)(&val), sizeof(d64), 0);
	 	return true;
	}

	virtual bool appendU64Value(const u64 &val)
	{
		mRcdLen = sizeof(u64);
		if(!mValues)
		{
			AosCompareFunPtr comp_func = createCompareFunc(mValueType, mRcdLen);
			comp_func->setReverse(mDataReverse);
			mValues = OmnNew AosBuffArray(                                                                           
			        comp_func, false, false, eDefaultRcdSize*mRcdLen);
			mValues->setAutoExtend(true,10);
		}
		aos_assert_r(mValues,false);
		mValues->appendEntry((char*)(&val), sizeof(u64), 0);
	 	return true;
	}

	u64 getDocid(const i64 &idx)const
	{
		aos_assert_r(idx >= 0 && idx < mNumDocs, 0);
		return mDocids[idx];
	}

	//felicia, 2013/03/13
	virtual void setOperator(const AosDataColOpr::E opr){mOperator = opr;}
	virtual AosDataColOpr::E getOperator() const {return mOperator;}


private:
	void	expandMem()
	{
		i64 newsize = mMemCap * 3;

		if(mMemCap >= eSmallDocNum)
		{
			if(mMemCap >= eLargeDocNum)
			{
				newsize = mMemCap * 1.5;
			}else
			{
				newsize = mMemCap * 2;
			}
		}
		expandMemPriv(newsize);
	}
	
	void	expandMem(const i64 &size)
	{
		if(size <= mMemCap) return;
		i64 newsize = mMemCap * 3;

		if(mMemCap >= eSmallDocNum)
		{
			if(mMemCap >= eLargeDocNum)
			{
				newsize = mMemCap * 1.5;
			}else
			{
				newsize = mMemCap * 2;
			}
		}
		if(newsize < size)
		{
			newsize = size + eInitMemSize;
		}
		expandMemPriv(newsize);
	}


	void	expandMemPriv(const i64 &size);
	virtual bool removeOldData(const i64 &pos);
	virtual u64 removeOldData();

public:
	// Chen Ding, 04/03/2012
	bool serializeFromBuff(const AosBuffPtr &buff);
	bool serializeToBuff(const AosBuffPtr &buff);
	
	// Chen Ding, 11/28/2012
	static bool initQueryRslt();
	virtual bool merge(const AosQueryRsltObjPtr &rhs_rslt);
	virtual bool merge(
				const AosQueryRsltObjPtr &rhs, 
				const bool merge_all);
	virtual bool merge(
				const AosQueryRsltObjPtr &rhs,
				const bool merge_all_left,
				const bool merge_all_right,
				bool &cut_left,
				bool &cut_right,
				OmnString &edge_value,
				u64 &edge_u64value,
				u64 &edge_docid);
	virtual void toString(const AosRundataPtr &rdata);
	virtual bool appendEntry(const u64 docid, const OmnString &value);
	virtual bool checkAllNum();
};
#endif



