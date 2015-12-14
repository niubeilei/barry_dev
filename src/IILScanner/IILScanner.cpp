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
// How to ignore: it is possible that multiple IILs contribute to the 
// same field. When one IIL sets the matched value, if the other does
// not match the value, it should ignore it. That is, when setting 
// not-matched value, a selector may set a default value or do nothing.
//
// Modification History:
// 05/10/2012 Created by Chen Ding
// 2012/11/12 Move From IILUtil By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILScanner/IILScanner.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataRecord/DataRecord.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILScannerListener.h"
#include "SEInterfaces/ValueSelObj.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/BuffArray.h"
#include "Util/DataTable.h"
#include "XmlUtil/XmlTag.h"


extern int gAosIILLogLevel;
	

AosIILScanner::AosIILScanner()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mSeqno(0),
mStatus(AosDataProcStatus::eNeedRetrieveData),
mNoMoreData(false),
mCouldSetQueryContext(true),
mPagesize(eDftPagesize),	
mMatchType(AosIILMatchType::eInvalid),
mMatchedFieldIdx(-1),
mStartPos(-1),
mEndPos(-1),
mProgress(0),
mTotalRecordNum(0),
mProcRecordNum(0),
mIsControllingScanner(false),
mMatchReject(false),
mNoMatchReject(false),
mIgnoreMatchError(false),
mIgnoreMatchErrorMaxNum(0),
mIgnoreMatchErrorNum(0),
mNumDocs(0),
mCrtIdx(-1),
mCrtEndIdx(-1),
mCrtMatchIdx(-1)
{
}


AosIILScanner::AosIILScanner(
		const AosIILScannerListenerPtr &caller, 
		const int seqno,
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mCaller(caller),
mSeqno(seqno),
mStatus(AosDataProcStatus::eNeedRetrieveData),
mNoMoreData(false),
mCouldSetQueryContext(true),
mPagesize(eDftPagesize),	
mMatchType(AosIILMatchType::eInvalid),
mMatchedFieldIdx(-1),
mStartPos(-1),
mEndPos(-1),
mProgress(0),
mTotalRecordNum(0),
mProcRecordNum(0),
mIsControllingScanner(false),
mMatchReject(false),
mNoMatchReject(false),
mIgnoreMatchError(false),
mIgnoreMatchErrorMaxNum(0),
mIgnoreMatchErrorNum(0),
mNumDocs(0),
mCrtIdx(-1),
mCrtEndIdx(-1),
mCrtMatchIdx(-1)
{
}


AosIILScanner::~AosIILScanner()
{
	//OmnScreen << "delete iil scanner" << endl; 
}


AosIILScannerObjPtr
AosIILScanner::createIILScanner(
		const AosIILScannerListenerPtr &caller,
		const int seqno, 
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	AosIILScanner *scanner = OmnNew AosIILScanner(caller, seqno, def, rdata);
	bool rslt = scanner->config(def, rdata);
	aos_assert_r(rslt, 0);
	return scanner;
}


bool 
AosIILScanner::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	
	mCouldSetQueryContext = def->getAttrBool(AOSTAG_COULDSETQUERYCONTEXT, true);
	mIsControllingScanner = def->getAttrBool(AOSTAG_IS_CONTROLLING_SCANNER, false);
	mMatchReject = def->getAttrBool(AOSTAG_MATCH_REJECT, false);
	mNoMatchReject = def->getAttrBool(AOSTAG_NOMATCH_REJECT, false);
	mIgnoreMatchError = def->getAttrBool(AOSTAG_IGNORE_MATCHERROR, false);
	mIgnoreMatchErrorMaxNum = def->getAttrU32(AOSTAG_IGNORE_MATCHERROR_MAXNUM, eDftIgnoreMatchErrorMaxNum);

	// Retrieve mIILName
	mIILName = def->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(mIILName != "", false);

	mFieldSep = def->getAttrStr(AOSTAG_FIELD_SEP);
	
	// Retrieve mMatchType
	mMatchType = AosIILMatchType::toEnum(def->getAttrStr(AOSTAG_MATCH_TYPE));
	bool rslt = AosIILMatchType::isValid(mMatchType);
	aos_assert_r(rslt, false);

	switch (mMatchType)
	{
	case AosIILMatchType::ePrefix:
		 aos_assert_r(mFieldSep != "", false);
		 mMatchedFieldIdx = def->getAttrInt(AOSTAG_MATCH_FIELD_IDX, -1);
		 aos_assert_r(mMatchedFieldIdx >= 0, false);
		 break;

	case AosIILMatchType::eSubstr:
		 mStartPos = def->getAttrInt(AOSTAG_START_POS, -1);
		 mEndPos = def->getAttrInt(AOSTAG_END_POS, -1);
		 aos_assert_r(mStartPos >= 0 && mEndPos > mStartPos, false);
		 break;

	default:
		 break;	
	}

	// Retrieve mIgnoreMatchError. When doing the matching, if it failed retrieving
	// the next value (normally only when match type is eField), this flag indicates
	// whether to abort the operation or not.

	// Retrieve mPagesize
	mPagesize = def->getAttrInt(AOSTAG_PAGESIZE, -1);
	if (mPagesize <= 0) mPagesize = eDftPagesize;
	
	// Retrieve value condition.
	AosXmlTagPtr val_cond = def->getFirstChild(AOSTAG_VALUE_COND);
	if (val_cond)
	{
		rslt = parseValCond(val_cond, rdata);
		aos_assert_r(rslt, false);
	}

	// Create the query context
	mQueryContext = AosQueryContextObj::createQueryContextStatic();
	mQueryContext->setOpr(mValueCond.mOpr);
	mQueryContext->setReverse(false);
	mQueryContext->setBlockSize(mPagesize);

	// The following sets the query condition
	mQueryContext->setStrValue(mValueCond.mValue1);
	mQueryContext->setStrValue2(mValueCond.mValue2);
	
	AosIILIdx iilidx;
	mQueryContext->setIILIndex2(iilidx);
	
	if (mValueCond.mIsGroup)
	{
		mQueryContext->setGrouping(true);
		mQueryContext->setKeyGroupingType(mValueCond.mKeyGroupType);
		mQueryContext->setValueGroupingType(mValueCond.mValueGroupType);
		mQueryContext->setSaperator(mFieldSep);

		if (mValueCond.mNeedFilter)
		{
			if (mValueCond.mFilterValueType == "u64")
			{
				// Chen Ding, 2014/02/26
				// mQueryContext->addGroupFilter(mValueCond.mFilterType,
				// 	mValueCond.mFilterOpr, atoll(mValueCond.mFilterValue.data()), false);
				mQueryContext->addGroupFilter(mValueCond.mFilterType,
					mValueCond.mFilterOpr, 
					atoll(mValueCond.mFilterValue1.data()),
					atoll(mValueCond.mFilterValue2.data()), false);
			}
			else
			{
				// Chen Ding, 2014/02/26
				// mQueryContext->addGroupFilter(mValueCond.mFilterType,
				// 	mValueCond.mFilterOpr, mValueCond.mFilterValue, false);
				mQueryContext->addGroupFilter(mValueCond.mFilterType,
					mValueCond.mFilterOpr, 
					mValueCond.mFilterValue1, mValueCond.mFilterValue2, false); 
			}
		}
	}

	// Create the Composor
	// 	<doc ...>
	// 		<AOSTAG_STR_COMPOSOR 
	// 			AOSTAG_ORIG_SEP="xxx"
	// 			AOSTAG_TARGET_SEP="xxx"
	// 			AOSTAG_COMPOSOR_MAXLEN="xxx">
	// 			<AOSTAG_COMPOSOR 
	// 				AOSTAG_ELEM_INDEX="xxx" AOSTAG_ELEM_TYPE="xxx">xxx
	// 			</AOSTAG_COMPOSOR>
	// 			...
	// 		</AOSTAG_STR_COMPOSOR>
	// 		...
	// 	</doc>
	AosXmlTagPtr strcomposor = def->getFirstChild(AOSTAG_STR_COMPOSOR);
	if (strcomposor)
	{
		int max_len = strcomposor->getAttrInt(AOSTAG_COMPOSOR_MAXLEN, -1);
		aos_assert_r(max_len > 0 && max_len < eMaxComposorStrLen, false);

		mComposorVector.clear();
		mComposorInfo.clear();
		mOrigSep = strcomposor->getAttrStr(AOSTAG_ORIG_SEP);
		mTargetSep = strcomposor->getAttrStr(AOSTAG_TARGET_SEP);
		AosXmlTagPtr tag = strcomposor->getFirstChild();
		while (tag)
		{
			mComposorVector.push_back("");
			ComposorInfo composor;

			// Retrieve Type
			composor.mType = AosStrElemType::toEnum(tag->getAttrStr(AOSTAG_ELEM_TYPE));
			aos_assert_r(AosStrElemType::isValid(composor.mType), false);

			// Retrieve Elem Index
			switch (composor.mType)
			{
			case AosStrElemType::eElem:
				 composor.mIndex = tag->getAttrInt(AOSTAG_ELEM_INDEX, -1);
				 aos_assert_r(composor.mIndex >= 0, false);
				 break;

			case AosStrElemType::eConst:
				 composor.mConstants = tag->getNodeText();
				 break;

			case AosStrElemType::eSubstr:
				 composor.mStartPos = tag->getAttrInt(AOSTAG_START_POS, -1);
				 aos_assert_r(composor.mStartPos >= 0, false);
				 
				 composor.mEndPos = tag->getAttrInt(AOSTAG_END_POS, -1);
				 aos_assert_r(composor.mEndPos > composor.mStartPos, false);
				 break;

			default:
				 break;
			}

			// Retrieve Constant, as needed
			mComposorInfo.push_back(composor);
			tag = strcomposor->getNextChild();
		}
	}

	// Retrieve the selectors
	AosXmlTagPtr selectors = def->getFirstChild(AOSTAG_SELECTORS);
	aos_assert_r(selectors, false);

	AosXmlTagPtr selector = selectors->getFirstChild();
	AosXmlTagPtr tag;
	while (selector)
	{
		AosIILSelector sel;
		sel.mSelectorType = AosIILEntrySelType::toEnum(selector->getAttrStr(AOSTAG_ENTRYSELECTOR));
		aos_assert_r(AosIILEntrySelType::isValid(sel.mSelectorType), false);

		sel.mValueType = AosIILValueType::toEnum(selector->getAttrStr(AOSTAG_VALUE_TYPE));
		aos_assert_r(AosIILValueType::isValid(sel.mValueType), false);

		sel.mFieldIdx = selector->getAttrInt(AOSTAG_FIELD_IDX, -1);
		sel.mCreateFlag = selector->getAttrBool(AOSTAG_CREATE_FLAG, false);
		if (sel.mCreateFlag)
		{
			tag = selector->getFirstChild(AOSTAG_DEFAULT_VALUE);
			aos_assert_r(tag, false);
			
			OmnAlarm << enderr;
			//rslt = sel.mDftValue.configValue(tag, rdata.getPtr());
			aos_assert_r(rslt, false);
		}

		sel.mSetDocid = selector->getAttrBool(AOSTAG_SET_DOCID, false);
		sel.mSetRundata = selector->getAttrBool(AOSTAG_SET_RUNDATA, false);

		sel.mSourceFieldIdx = selector->getAttrInt(AOSTAG_SOURCE_FIELD_IDX, -1);
		sel.mSourceSeparator = selector->getAttrStr(AOSTAG_SEPARATOR);

		mSelectors.push_back(sel);
		selector = selectors->getNextChild();
	}
	
	AosXmlTagPtr scanvalue = def->getFirstChild(AOSTAG_IILSCANVALUE);
	if (scanvalue)
	{
		mScanValue = AosIILScanValue::createIILScanValue(scanvalue, rdata); 
	}

	return true;
}


bool
AosIILScanner::start(const AosRundataPtr &rdata)
{
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "iilscanner", 0, false, true, __FILE__, __LINE__);
	mStatus = AosDataProcStatus::eNeedRetrieveData;
	mThread->start();
	return true;
}


bool	
AosIILScanner::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	aos_assert_r(mCaller, false);
	AosIILScannerObjPtr thisptr(this, false);
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		switch (mStatus)
		{
		case AosDataProcStatus::eDataRetrieved:
			 // The next data is retrieved. 
			 mStatus = AosDataProcStatus::eOk;
			 mLock->unlock();
			 if (mCaller) mCaller->dataRetrieved(thisptr, mRundata);
			 break;
		
		case AosDataProcStatus::eRetrievingData:
			 mCondVar->wait(mLock);
			 mLock->unlock();
			 break;
		
		case AosDataProcStatus::eNeedRetrieveData:
			 // It need to retrieve data
			 mStatus = AosDataProcStatus::eRetrievingData;
			 mLock->unlock();
			 retrieveData();
			 break;
		
		case AosDataProcStatus::eOk:
			 mCondVar->wait(mLock);
			 mLock->unlock();
			 break;
		
		case AosDataProcStatus::eNoMoreData:
			 mLock->unlock();
			 if (mCaller) mCaller->dataRetrieved(thisptr, mRundata);
			 //state = OmnThrdStatus::eExit;
			 return true;

		case AosDataProcStatus::eExit:
			 state = OmnThrdStatus::eExit;
			 return true;

		default:
			 OmnAlarm << "Unrecognized status: " << mStatus << enderr;
			 mLock->unlock();
			 state = OmnThrdStatus::eExit;
			 return true;
		}
	}
	state = OmnThrdStatus::eExit;

	return true;
}


bool	
AosIILScanner::signal(const int threadLogicId)
{
	return true;
}


bool    
AosIILScanner::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

	
bool
AosIILScanner::retrieveData()
{
	aos_assert_r(mCaller, false);
	// This function queries the data. This is called by the thread. 
	mLock->lock();
	if (mStatus == AosDataProcStatus::eNoMoreData)
	{
		mLock->unlock();
		return true;
	}

	if (mNoMoreData)
	{
		mStatus = AosDataProcStatus::eNoMoreData;
		mLock->unlock();
		return true;
	}
	mLock->unlock();
	
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);

	AosIILScannerObjPtr thisptr(this, false);
	bool rslt = AosQueryColumn(mIILName, query_rslt, 0, mQueryContext, mRundata);

	if (gAosIILLogLevel > 1)
	{
/*		// Debugging only. Will remove
		OmnScreen << "Query returned: " << mIILName.data() << endl;
		AosBuffPtr chending_buff = query_rslt->getValueBuff();
		u64 *chending_docids = query_rslt->getDocidsPtr();
		int chending_num_docs = query_rslt->getNumDocs();
		int chending_len;
		if (chending_num_docs > 5000) chending_num_docs = 5000;
		for (int i=0; i<chending_num_docs; i++)
		{
			char *ddd = chending_buff->getCharStr(chending_len);
			OmnScreen << "Data: " 
				<< chending_docids[i] << ":"
				<< ddd << ":" << chending_len << endl;
		}
		chending_buff->reset();
*/
	}

	mLock->lock();
	if (!rslt)
	{
		mStatus = AosDataProcStatus::eError;
	}
	else
	{
		mNextData = query_rslt;
		mStatus = AosDataProcStatus::eDataRetrieved;
		
		if (mQueryContext->finished())
		{
			mNoMoreData = true;
		}
	
		if (query_rslt->isEmpty())
		{
			mNextData = 0;
		}

		if (mTotalRecordNum == 0)
		{
			mTotalRecordNum = mQueryContext->getTotalDocInRslt();
		}
	}
	
	mCondVar->signal();
	mLock->unlock();
	
	if (rslt)
	{
		mCaller->dataRetrieved(thisptr, mRundata);
	}
	else
	{
		mCaller->dataRetrieveFailed(thisptr, mRundata);
	}
	return rslt;
}

	
AosDataProcStatus::E
AosIILScanner::setupNewData(const AosRundataPtr &rdata)
{
	// This function is called when the scanner starts. After that, 
	// the scanner moves by calling the member function:
	// 		setMatchedValueAndMove(...)
	aos_assert_r(mCrtData, AosDataProcStatus::eError);

	// The data is available. Retrieve the first value from mCrtData
	mNumDocs = mCrtData->getNumDocs();
	if (mNumDocs == 0) 
	{
		return AosDataProcStatus::eNoMoreData;
	}
	
	mProcRecordNum += mNumDocs;

	AosBuffArrayPtr keys = mCrtData->getValueBuff();
	aos_assert_r(keys, AosDataProcStatus::eError);
	
	u64 * values = mCrtData->getDocidsPtr();
	aos_assert_r(values, AosDataProcStatus::eError);
	
	char * key;
	int len = keys->getRecordLen();
	for(u64 i=0; i<mNumDocs; i++)
	{
		key = keys->getEntry(i);
		OmnString vv(key, len);
		aos_assert_r(vv != "", AosDataProcStatus::eError);
		mCrtKeys.push_back(vv);
		mCrtValues.push_back(values[i]);
	}

	mCrtIdx = -1;
	mCrtEndIdx = -1;
	return AosDataProcStatus::eOk;
}


AosDataProcStatus::E
AosIILScanner::moveNext(
		const char *&value, 
		int &len,
		const int total_matched,
		const AosRundataPtr &rdata)
{
	// This function moves to the next. Check whether it is waiting for new data.
	// The loop below ensures that it will not stop until it can successfully
	// find the next matched value (the loop is needed only when 'mIgnoreMatchedError'
	// is true.
	aos_assert_r(mCaller, AosDataProcStatus::eError);
	mLock->lock();
	if (mStatus == AosDataProcStatus::eError)
	{
		mLock->unlock();
		return mStatus;
	}

	mCrtIdx = mCrtEndIdx;
	while (1)
	{
		if (mCrtIdx < 0 || (u64)mCrtIdx+1 >= mNumDocs)
		{
			if (mMatchType == AosIILMatchType::eMapper &&
				mCrtIdx > 0 && total_matched != 1)
			{
				OmnScreen << "match type is mapper and the crt is the last." << endl;	
			}
			else
			{
				// It runs out of the current value. 
				mCrtData = 0;
				mCrtKeys.clear();
				mCrtValues.clear();
				mCrtIdx = -1;
				mCrtEndIdx = -1;

				if (mStatus == AosDataProcStatus::eRetrievingData)
				{
					// It is retrieving the next data. Need to wait.
					mLock->unlock();
					return AosDataProcStatus::eRetrievingData;
				}
				
				if (mNextData)
				{
					// Data has been retrieved. Need to switch it.
					mCrtData = mNextData;
					mNextData = 0;
	
					AosDataProcStatus::E status = setupNewData(rdata);
					if (status != AosDataProcStatus::eOk)
					{
						mLock->unlock();
						return status;
					}
					
					mStatus = AosDataProcStatus::eNeedRetrieveData;
					mCondVar->signal();
				}
				else
				{
					// It is not retrieving data and 'mNextData' is null. 
					// Check whether there are no more data.
					if (mNoMoreData)
					{
						mStatus = AosDataProcStatus::eNoMoreData;
						mCondVar->signal();
						mLock->unlock();
						return AosDataProcStatus::eNoMoreData;
					}
	
					// It needs to retrieve data.
					mStatus = AosDataProcStatus::eNeedRetrieveData;
					mCondVar->signal();
					mLock->unlock();
					return AosDataProcStatus::eRetrievingData;
				}
			}
		}
			
		// When it comes to this point, there are data in mCrtData;
			
		// Need to retrieve the matched value.
		bool rslt = true;
		switch (mMatchType)
		{
		case AosIILMatchType::eKey:
			 // The next value is 'mCrtStrValue'.
			 mCrtIdx++;
			 value = mCrtKeys[mCrtIdx].data();
			 len = mCrtKeys[mCrtIdx].length();
			 break;

		case AosIILMatchType::eValue:
			 // The next value is 'mCrtValues[mCrtIdx]'. 
			 mCrtIdx++;
			 mCrtValueStr = "";
			 mCrtValueStr << mCrtValues[mCrtIdx];
			 value = mCrtValueStr.data();
			 len = strlen(value);
			 break;
				 
		case AosIILMatchType::ePrefix:
			 // It retrieves a specific field in the key column
			 mCrtIdx++;
			 rslt = AosGetField(value, len, mCrtKeys[mCrtIdx].data(),
				 mCrtKeys[mCrtIdx].length(), mMatchedFieldIdx, mFieldSep, rdata.getPtr());
			 break;
		
		case AosIILMatchType::eSubstr:
			 mCrtIdx++;
			 if (mEndPos >= mCrtKeys[mCrtIdx].length())
			 {
				 rslt = false;
				 break;
			 }
			 value = mCrtKeys[mCrtIdx].data();
			 len = mEndPos - mStartPos + 1;
			 break;

		case AosIILMatchType::eMapper:
			 // If there are more than one matched, do not move.
			 if (total_matched != 1)
			 {
				if (mCrtIdx == 0)
			 	{
			 		mCrtIdx++;
			 	}
			 }
			 else
			 {
			 	// Otherwise, it moves to the next and reject the current match.
			 	mCrtIdx++;
			 }
			 value =  mCrtKeys[mCrtIdx].data();
			 len = mCrtKeys[mCrtIdx].length();
			 break;

		default:
			 mCrtIdx++;
			 rslt = false;
			 break;
		}
		
		if (rslt)
		{
			rslt = getCrtEndIdx(rdata);
			if (rslt)
			{
				if (mCrtMatchKeys.size() == 0) break;
				mLock->unlock();
				return AosDataProcStatus::eOk;
			}
		}
			
		if (!mIgnoreMatchError) break;
		mIgnoreMatchErrorNum++;
		if (mIgnoreMatchErrorMaxNum < mIgnoreMatchErrorNum) break;
	}

	mLock->unlock();
	AosSetErrorU(rdata, "internal_error");
	OmnAlarm << rdata->getErrmsg() << enderr;
	return AosDataProcStatus::eError;
}


AosDataProcStatus::E
AosIILScanner::moveNext(
		OmnString &data,
		bool &has_more,
		const AosRundataPtr &rdata)
{
	// This function moves to the next. Check whether it is waiting for new data.
	// The loop below ensures that it will not stop until it can successfully
	// find the next matched value (the loop is needed only when 'mIgnoreMatchedError'
	// is true.
	aos_assert_r(mCaller, AosDataProcStatus::eError);
	mLock->lock();
	if (mStatus == AosDataProcStatus::eError)
	{
		mLock->unlock();
		return mStatus;
	}
	
	mCrtIdx = mCrtEndIdx;
	while (1)
	{
		if (mCrtIdx < 0 || (u64)mCrtIdx+1 >= mNumDocs)
		{
			// It runs out of the current value. 
			mCrtData = 0;
			mCrtKeys.clear();
			mCrtValues.clear();
			mCrtIdx = -1;
			mCrtEndIdx = -1;

			if (mStatus == AosDataProcStatus::eRetrievingData)
			{
				// It is retrieving the next data. Need to wait.
				mLock->unlock();
				return AosDataProcStatus::eRetrievingData;
			}
				
			if (mNextData)
			{
				// Data has been retrieved. Need to switch it.
				mCrtData = mNextData;
				mNextData = 0;
	
				AosDataProcStatus::E status = setupNewData(rdata);
				if (status != AosDataProcStatus::eOk)
				{
					mLock->unlock();
					return status;
				}
				
				mStatus = AosDataProcStatus::eNeedRetrieveData;
				mCondVar->signal();
			}
			else
			{
				// It is not retrieving data and 'mNextData' is null. 
				// Check whether there are no more data.
				if (mNoMoreData)
				{
					mStatus = AosDataProcStatus::eNoMoreData;
					mCondVar->signal();
					mLock->unlock();
					return AosDataProcStatus::eNoMoreData;
				}

				// It needs to retrieve data.
				mStatus = AosDataProcStatus::eNeedRetrieveData;
				mCondVar->signal();
				mLock->unlock();
				return AosDataProcStatus::eRetrievingData;
			}
		}
	
		// When it comes to this point, there are data in mCrtData;
		// Need to retrieve the matched value.
		bool rslt = true;
		data = "";
		mCrtIdx++;
		switch (mMatchType)
		{
		case AosIILMatchType::eKey:
			 // The next value is 'mCrtStrValue'.
			 data = mCrtKeys[mCrtIdx]; 
			 break;

		case AosIILMatchType::eValue:
			 // The next value is 'mCrtValues[mCrtIdx]'. 
			 data << mCrtValues[mCrtIdx];
			 break;
				 
		case AosIILMatchType::ePrefix:
			 // It retrieves a specific field in the key column
			 {
			 	OmnString vv = mCrtKeys[mCrtIdx];
			 	const char * value;
			 	int len;
			 	rslt = AosGetField(value, len, vv.data(), vv.length(),
					mMatchedFieldIdx, mFieldSep, rdata.getPtr());
				if (!rslt) break;
				data.assign(value, len);
			 	break;
			 }
		
		case AosIILMatchType::eSubstr:
			 data = mCrtKeys[mCrtIdx];
			 if (mEndPos >= data.length()) 
			 {
				rslt = false;
				break;
			 }
		 	 data.setLength(mEndPos - mStartPos + 1);
			 break;

		default:
			 rslt = false;
			 break;
		}

		if (rslt)
		{
			rslt = getCrtEndIdx(rdata);
			if (rslt)
			{
				if (mCrtMatchKeys.size() == 0) break;
				has_more = ((u64)mCrtEndIdx == mNumDocs - 1) ? true : false;
				mLock->unlock();
				return AosDataProcStatus::eOk;
			}
		}
			
		if (!mIgnoreMatchError) break;
		mIgnoreMatchErrorNum++;
		if (mIgnoreMatchErrorMaxNum < mIgnoreMatchErrorNum) break;
	}	
	
	mLock->unlock();
	AosSetErrorU(rdata, "internal_error");
	OmnAlarm << rdata->getErrmsg() << enderr;
	return AosDataProcStatus::eError;
}


bool
AosIILScanner::getCrtEndIdx(const AosRundataPtr &rdata)
{
	aos_assert_r(mCrtIdx >= 0 && (u64)mCrtIdx < mNumDocs, false);
	int end = mCrtIdx + 1;
	bool found_end = false;
	bool rslt;
	while ((u64)end < mNumDocs)
	{
		switch (mMatchType)
		{
		case AosIILMatchType::eKey:
			 if (mCrtKeys[mCrtIdx] != mCrtKeys[end])
			 {
				found_end = true;
			 }
			 break;

		case AosIILMatchType::eValue:
			 if (mCrtValues[mCrtIdx] != mCrtValues[end])
			 {
				found_end = true;
			 }
			 break;
				 
		case AosIILMatchType::ePrefix:
			 // It retrieves a specific field in the key column
			 {
				OmnString d1, d2;
			 	OmnString vv = mCrtKeys[mCrtIdx];
			 	const char * value;
			 	int len;
			 	rslt = AosGetField(value, len, vv.data(), vv.length(),
					mMatchedFieldIdx, mFieldSep, rdata.getPtr());
				if (!rslt)
				{
					found_end = true;
					break;
				}
				d1.assign(value, len);
			 	
				vv = mCrtKeys[end];
				rslt = AosGetField(value, len, vv.data(), vv.length(),
					mMatchedFieldIdx, mFieldSep, rdata.getPtr());
				if (!rslt)
				{
					found_end = true;
					break;
				}
				d2.assign(value, len);

				if (d1 != d2)
				{
					found_end = true;
				}
			 	break;
			 }
		
		case AosIILMatchType::eSubstr:
			 {
				OmnString d1 = mCrtKeys[mCrtIdx];
				OmnString d2 = mCrtKeys[end];
			 	if (mEndPos >= d1.length() || mEndPos >= d2.length())
			 	{
					found_end = true;
					break;
				}
		 		d1.setLength(mEndPos - mStartPos + 1);
		 		d2.setLength(mEndPos - mStartPos + 1);
				if (d1 != d2)
				{
					found_end = true;
					break;
				}
				break;
			 }

		default:
			 found_end = true;
			 break;
		}
		if (found_end) break;
		end++;
	}
	mCrtEndIdx = end - 1;
	
	mCrtMatchIdx = -1;
	mCrtMatchKeys.clear();
	mCrtMatchValues.clear();

	vector<OmnString> keys;
	vector<u64> values;

	for (int i=0; i<=mCrtEndIdx - mCrtIdx; i++)
	{
		keys.push_back(mCrtKeys[mCrtIdx + i]);
		values.push_back(mCrtValues[mCrtIdx + i]);
	}
	
	if (mScanValue)
	{
		rslt = mScanValue->scanValue(
			keys, values, mCrtMatchKeys, mCrtMatchValues, rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
		mCrtMatchKeys = keys;
		mCrtMatchValues = values;
	}
	return true;
}
	
	
u32
AosIILScanner::getMatchedSize()
{
	return mCrtMatchKeys.size();
}


void
AosIILScanner::resetMatchedIdx()
{
	mCrtMatchIdx = -1;
}


bool
AosIILScanner::setMatchedValue(
		const AosDataRecordObjPtr &record, 
		bool &has_more,
		const AosRundataPtr &rdata)
{
	// This function does two things: 
	// 1. If 'need_set_value' is true, it sets the matched value.
	// 2. Moves to the next value.
	//
	// eError: 		if any error occurs;
	// eOk:			if everything is ok;
	
	// Need to move to the next. Check whether it is waiting for new data.
	// The loop below ensures that it will not stop until it can successfully
	// find the next matched value (the loop is needed only when 'mIgnoreMatchedError'
	// is true.
	aos_assert_r(mCaller, false);
	aos_assert_r(!mMatchReject, false);
	aos_assert_r(record, false);

	has_more = false;
	mCrtMatchIdx++;
	if ((u32)mCrtMatchIdx >= mCrtMatchKeys.size())
	{
		AosSetErrorU(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt vv;
	bool rslt = false;
	u64 value;
	AosIILScannerObjPtr thisptr(this, false);
	for (u32 i=0; i<mSelectors.size(); i++)
	{
		rslt = AosIILValueType::getValue(mSelectors[i],
			vv, mCrtMatchKeys[mCrtMatchIdx].data(),
			mCrtMatchKeys[mCrtMatchIdx].length(),
			mCrtMatchValues[mCrtMatchIdx], thisptr, rdata);
		aos_assert_r(rslt, false);

		if (mSelectors[i].mSetDocid)
		{
			rslt = vv.getU64();
			aos_assert_r(rslt, false);
			record->setDocid(value);
		}

		if (mSelectors[i].mFieldIdx >= 0)
		{
			bool outofmem = false;
			rslt = record->setFieldValue(mSelectors[i].mFieldIdx, vv, outofmem, rdata.getPtr());
			aos_assert_r(rslt, false);
		}
		
		if (mSelectors[i].mSetRundata)
		{
			if (mSelectors[i].mValueType == AosIILValueType::eValue)
			{
				rslt = vv.getU64();
				aos_assert_r(rslt, false);
				rdata->setU64Value(mSeqno, value);	
			}
			else
			{
				OmnString ss = vv.getStr();
				rdata->setCharPtr(mSeqno, ss.data(), ss.length());
			}
		}
	}
	
	rslt = record->setDocidByIdx(mSeqno, mCrtMatchValues[mCrtMatchIdx]);
	aos_assert_r(rslt, false);

	if ((u32)(mCrtMatchIdx + 1) < mCrtMatchKeys.size())
	{
		has_more = true;
	}
	return true;
}


bool
AosIILScanner::setNotMatchedValue(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	// A not-matched case is found. It checks whether it is allowed. 
	// If not, it returns eRejected. Otherwise, it sets the
	// default values.
	aos_assert_r(mCaller, false);
	aos_assert_r(!mNoMatchReject, false);
	aos_assert_r(record, false);
	
	bool rslt = true;
	u64 value;
	for (u32 i=0; i<mSelectors.size(); i++)
	{
		if (!mSelectors[i].mCreateFlag) continue;

		if (mSelectors[i].mSetDocid)
		{
			rslt = !mSelectors[i].mDftValue.getU64();
			aos_assert_r(rslt, false);
			record->setDocid(value);
		}

		if (mSelectors[i].mFieldIdx >= 0)
		{
			bool outofmem = false;
			rslt = record->setFieldValue(mSelectors[i].mFieldIdx, mSelectors[i].mDftValue, outofmem, rdata.getPtr());
			aos_assert_r(rslt, false);
		}
	}	
	
	return true;
}


bool 
AosIILScanner::isFinished() const
{
	if (!mNoMoreData) return false;
	if (mStatus == AosDataProcStatus::eRetrievingData) return false;
	aos_assert_r(!mNextData, true);
	if (!mCrtData) return true;
	if (mCrtData->isEmpty()) return true;
	return false;
}


bool
AosIILScanner::parseValCond(
		const AosXmlTagPtr &val_cond,
		const AosRundataPtr &rdata)
{
	if (!val_cond) return true;

	mValueCond.mValueType = val_cond->getAttrStr(AOSTAG_VALUE_TYPE);
	aos_assert_r(mValueCond.mValueType == "str" || mValueCond.mValueType == "u64", false);
	
	mValueCond.mOpr = AosOpr_toEnum(val_cond->getAttrStr(AOSTAG_OPR));
	aos_assert_r(AosOpr_valid(mValueCond.mOpr), false);

	AosXmlTagPtr vv1 = val_cond->getFirstChild(AOSTAG_VALUE1);
	if (vv1) mValueCond.mValue1 = vv1->getNodeText();

	AosXmlTagPtr vv2 = val_cond->getFirstChild(AOSTAG_VALUE2);
	if (vv2)
	{
		aos_assert_r(vv1, false);
		mValueCond.mValue2 = vv2->getNodeText();
	}

	mValueCond.mIsGroup = val_cond->getAttrBool(AOSTAG_ISGROUP, false);
	if (!mValueCond.mIsGroup) return true;
	
	AosXmlTagPtr group = val_cond->getFirstChild(AOSTAG_GROUP);
	aos_assert_r(group, false);

	mValueCond.mKeyGroupType = AosQueryContextObj::QueryKeyGroupingType_toEnum(group->getAttrStr(AOSTAG_QUERYKEYGROUPINGTYPE));
	mValueCond.mValueGroupType = AosQueryContextObj::QueryValueGroupingType_toEnum(group->getAttrStr(AOSTAG_QUERYVALUEGROUPINGTYPE));
		
	mValueCond.mNeedFilter = group->getAttrBool(AOSTAG_NEEDFILTER, false);
	if (!mValueCond.mNeedFilter) return true;
		
	AosXmlTagPtr filter = group->getFirstChild(AOSTAG_FILTER);
	aos_assert_r(filter, false);

	mValueCond.mFilterValueType = filter->getAttrStr(AOSTAG_VALUE_TYPE);
	aos_assert_r(mValueCond.mFilterValueType == "str" || mValueCond.mFilterValueType == "u64", false);

	mValueCond.mFilterValue1 = filter->getNodeText();
	mValueCond.mFilterOpr = AosOpr_toEnum(filter->getAttrStr(AOSTAG_OPR));
	aos_assert_r(AosOpr_valid(mValueCond.mFilterOpr), false);

	mValueCond.mFilterType = AosQueryContextObj::GroupFilterType_toEnum(filter->getAttrStr(AOSTAG_FILTERTYPE));
	return true;
}


void
AosIILScanner::setListener(const AosIILScannerListenerPtr &listener)
{
	mCaller = listener;
}


AosQueryContextObjPtr
AosIILScanner::getQueryContext() const
{
	return mQueryContext;
}


bool
AosIILScanner::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	if (!mCouldSetQueryContext) return true;
	aos_assert_r(context, false);
	mQueryContext = context->clone();
	
	if (!mValueCond.mIsGroup) return true;
	mQueryContext->setGrouping(true);
	mQueryContext->setKeyGroupingType(mValueCond.mKeyGroupType);
	mQueryContext->setValueGroupingType(mValueCond.mValueGroupType);
	mQueryContext->setSaperator(mFieldSep);

	if (!mValueCond.mNeedFilter) return true;
	if (mValueCond.mFilterValueType == "u64")
	{
		mQueryContext->addGroupFilter(mValueCond.mFilterType,
			mValueCond.mFilterOpr, 
			atoll(mValueCond.mFilterValue1.data()), 
			atoll(mValueCond.mFilterValue2.data()), false);
	}
	else
	{
		mQueryContext->addGroupFilter(mValueCond.mFilterType,
			mValueCond.mFilterOpr, 
			mValueCond.mFilterValue1, mValueCond.mFilterValue2, false);
	}
	return true;
}


int
AosIILScanner::getProgress()
{
	//OmnScreen << "iilscanner:[" << this << "], proc_num:" << mProcRecordNum
	//	<< ", total_num:" << mTotalRecordNum << endl;
	if (mTotalRecordNum == 0)
	{
		if (mStatus == AosDataProcStatus::eNoMoreData) 
		{
			return 100;
		}
		if (mStatus == AosDataProcStatus::eRetrievingData
			|| mStatus == AosDataProcStatus::eNeedRetrieveData)
		{
			return 0;
		}
		OmnAlarm << "TotalRecordNum error" << enderr;
		return 0;
	}
	mProgress = mProcRecordNum * 100 / mTotalRecordNum;
	if (mProgress > 100) mProgress = 100;
	return mProgress;
}

	
bool
AosIILScanner::isReject(const bool is_match) const
{
	if (is_match) return mMatchReject;
	return mNoMatchReject;
}

	
u64
AosIILScanner::getCrtValue() const
{
	aos_assert_r(mCrtIdx >= 0 && (u64)mCrtIdx + 1 <= mNumDocs, false);
	return mCrtValues[mCrtIdx];
}


bool
AosIILScanner::reloadData(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	while (mStatus == AosDataProcStatus::eRetrievingData)
	{
		mLock->unlock();
		OmnSleep(1);
		mLock->lock();
	}
	
	mQueryContext = context;
	mStatus = AosDataProcStatus::eNeedRetrieveData;
	mNoMoreData = false;
	mCrtData = 0;
	mNextData = 0;
	mNumDocs = 0;
	mCrtKeys.clear();
	mCrtValues.clear();
	mCrtValueStr = "";
	mCrtIdx = -1;
	mCrtEndIdx = -1;

	mCondVar->signal();
	mLock->unlock();
	return true;
}


void
AosIILScanner::clear()
{
	mLock->lock();
	mThread = 0;
	mStatus = AosDataProcStatus::eExit;
	mCondVar->signal();
	mLock->unlock();
}

