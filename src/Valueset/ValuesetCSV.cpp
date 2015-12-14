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
// This valueset assumes the values are defined in the (generic) CSV
// format:
// 		value + separator 
// 		value + separator
// 		...
// 		value
// The string can be from any source.
//
// Modification History:
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Valueset/ValuesetCSV.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "Value/Value.h"
#include "XmlUtil/XmlTag.h"



AosValuesetCSV::AosValuesetCSV(const OmnString &version)
:
AosValueset(AOS_VALUESET_TYPE_XML, version)
{
	init();
}


AosValuesetCSV::~AosValuesetCSV()
{
}


void
AosValuesetCSV::init()
{
	mValue = 0;
	mValueRaw = 0;
	mData = "";
	mRawData = 0;
	mDataLen = 0;
	mReadIdx = 0;
	mWriteIdx = 0;
	return true;
}


bool
AosValuesetCSV::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	init();

	if (!worker_doc)
	{
		AosSetErrorUser(rdata, "valuesetcsv_missing_worker_doc") << enderr;
		return false;
	}

	AosXmlTagPtr tag = worker_doc->getFirstChild("value_def");
	if (!tag)
	{
		AosSetErrorUser(rdata, "valuesetcsv_missing_value_def") << enderr;
		return false;
	}

	mValue = AosCreateValue(rdata, tag);
	if (!mValue)
	{
		AosSetErrorUser(rdata, "valuesetcsv_failed_creating_value") 
			<< rdata->getErrmsg() << enderr;
		return false;
	}
	mValueRaw = mValue.getPtr();

	return true;	
}


AosValue *
AosValuesetCSV::nextValue(const AosRundataPtr &rdata)
{
	// Values are quoted. The quotes can be single quotes, double
	// quotes, or any single character. If quotes are used inside
	// values, they must be escaped. The results may or may not 
	// contain the quotes. If quotes are removed in the results, 
	// leading white spaces and/or trailing white spaces may be
	// removed. 
	//
	// This function retrieves the value as string, and then 
	// sets the value to mValue, which is then returned.
	aos_assert_rr(mValueRaw, rdata, 0);
	if (!mRawData) return 0;

	if (mReadIdx >= mDataLen) return 0;

	// 'mReadIdx' points to the position right after the separator
	// of the last reading or at the beginning of the data. 
	
	// 1. Move to the beginning quote
	while (mReadIdx < mDataLen && mRawData[mReadIdx] != mQuote) mReadIdx++;
	if (mReadIdx >= mDataLen)
	{
		// Run out of the data. 
		return 0;
	}

	int start_idx = mReadIdx++;
	int end_idx = -1;
	// 2. Move to the ending quote. Note that the quote may appear inside
	//    the contents. If so, the quotes must be escaped.
	while (mReadIdx < mDataLen)
	{
		if (mRawData[mReadIdx] != mQuote)
		{
			// It is not the quote.
			mReadIdx++;
			continue;
		}

		// The quote is found. Check whether it is escaped.
		if (mReadIdx-1 >= start+1 && mRawData[mReadIdx-1] == mQuoteEscape)
		{
			// It is escaped. 
			mReadIdx++;
			continue;
		}

		// It is not escaped. 
		end_idx = mReadIdx++;

		// Skip to the separator
		while (mReadIdx < mDataLen && mRawData[mReadIdx] != mSeparator) mReadIdx++;
				
		// This is the end of the contents.
		getValue(start_idx, end_idx);
		mValueRaw->setValue(&mRawData[start_idx], end_idx-start_idx+1);
		return mValueRaw;
	}

	AosSetErrorUser(rdata, "valuesetcsv_incorrect_format") 
		<< "Value:" << mRawData << enderr;
	return 0;
}


bool
AosValuesetCSV::getValue(const int start, const int end)
{
	if (!mRemoveQuotes) return true;

	start++;
	end--;
	if (!mRemoveWhiteSpaces)
	{
		return true;
	}

	// Skip the leading white spaces
	while (start > end)
	{
		if (!isWhiteSpace(mRawData[start])) break;
		start++;
	}

	while (start > end)
	{
		char c = mRawData[end];
		if (!isWhiteSpace(mRawData[end])) return true;
		end--;
	}

	return true;
}


bool
AosValuesetCSV::resetReadLoop()
{
	mReadIdx = 0;
	return true;
}


bool
AosValuesetCSV::setData(
		const AosRundataPtr &rdata, 
		const AosValuePtr &data)
{
	aos_assert_rr(data, rdata, false);
	mData = data->getStr();
	mRawData = mData.data();
	mDataLen = mRawData.length();

	mReadIdx = 0;
	mWriteIdx = 0;
	return true;
}


AosJimoPtr
AosValuesetCSV::cloneJimo() const
{
	try
	{
		return OmnNew AosValuesetCSV(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}

