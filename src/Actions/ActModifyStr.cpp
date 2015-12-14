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
// 05/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActModifyStr.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ValueSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include <string>
#include <vector>
using namespace std;
#if 0

AosActModifyStr::AosActModifyStr(const bool flag)
:
AosSdocAction(AOSACTTYPE_MODIFYSTR, AosActionType::eModifyStr, flag)
{
}


AosActModifyStr::AosActModifyStr(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_MODIFYSTR, AosActionType::eModifyStr, false)
{
	if (!config(sdoc, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosActModifyStr::~AosActModifyStr()
{
}


bool	
AosActModifyStr::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(def, rdata, false);
	mStartPos = def->getAttrInt(AOSTAG_START_POS, -1);
	if (mStartPos < 0)
	{
		AosSetErrorU(rdata, "invalid_start_pos:") << mStartPos;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mMaxLength = def->getAttrInt(AOSTAG_MAX_LENGTH, -1);
	if (mMaxLength < 0)
	{
		AosSetErrorU(rdata, "invalid_max_length:") << mMaxLength;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr tag = def->getFirstChild(AOSTAG_VALUE_SELECTOR);
	if (!tag)
	{
		AosSetErrorU(rdata, "missing_value_selector:") << mMaxLength;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueSelObjPtr mValueSelector = AosValueSelObj::getValueSelStatic(tag, rdata);
	if (!mValueSelector)
	{
		AosSetErrorU(rdata, "failed_creating_selector:") << mMaxLength;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mValueSelector = mValueSelector->clone(tag, rdata);
	mLeftAlign = def->getAttrBool(AOSTAG_LEFT_ALIGN, true);
	mPadding = def->getAttrChar(AOSTAG_PADDING, ' ');
	return true;
}


bool 
AosActModifyStr::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool	
AosActModifyStr::run(
		const char *&record, 
		int &len, 
		const AosRundataPtr &rdata)
{
	// This action modifies the string 'record'. 
	if (mStartPos <= 0 || mStartPos + mMaxLength > len)
	{
		AosSetErrorU(rdata, "invalid_operation") << ":" 
			<< mStartPos << ":" << mMaxLength << enderr;
		return false;
	}

	if (!mValueSelector)
	{
		AosSetErrorU(rdata, "missing_value_selector") << enderr;
		return false;
	}

	AosValueRslt value;
	bool rslt = mValueSelector->run(value, record, len, rdata);
	aos_assert_rr(rslt, rdata, false);
	
	switch (value.getType())
	{
	case AosDataType::eString:
		 {
			 OmnString str = value.getStr();
			 int str_len = str.length();
			 if (str_len > mMaxLength)
			 {
				 AosSetErrorU(rdata, "string_too_long");
				 OmnAlarm << rdata->getErrmsg() << enderr;
				 return false;
			 }

			 int delta = mMaxLength - str_len;
			 if (mLeftAlign)
			 {
				 strncpy((char*)&record[mStartPos], str.data(), str_len);
				 if (mPadding && delta > 0)
				 {
					 memset((void *)&record[mStartPos+str_len], (int)mPadding, delta);
				 }
			 }
			 else
			 {
				 strncpy((char*)&record[mStartPos+delta], str.data(), str_len);
				 if (mPadding && delta > 0)
				 {
					 memset((void*)&record[mStartPos], mPadding, delta);
				 }
			 }
		 }
		 return true;

	case AosDataType::eCharStr:
		 {
			 int str_len;
			 const char *str = value.getCharStr(str_len);
			 if (!str)
			 {
				 AosSetErrorU(rdata, "internal_error") << enderr;
				 return false;
			 }

			 if (str_len > mMaxLength)
			 {
				 AosSetErrorU(rdata, "string_too_long") << ": " << str_len
					 << ":" << mMaxLength << enderr;
				 return false;
			 }

			 int delta = mMaxLength - str_len;
			 if (mLeftAlign)
			 {
				 strncpy((char*)&record[mStartPos], str, str_len);
				 if (mPadding && delta > 0)
				 {
					 memset((void *)&record[mStartPos+str_len], (int)mPadding, delta);
				 }
			 }
			 else
			 {
				 strncpy((char*)&record[mStartPos+delta], str, str_len);
				 if (mPadding && delta > 0)
				 {
					 memset((void*)&record[mStartPos], mPadding, delta);
				 }
			 }
		 }
		 return true;

	default:
		 break;
	}

	AosSetErrorU(rdata, "internal_error") << ": " << value.getType() << enderr;
	return false;
}


AosActionObjPtr
AosActModifyStr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActModifyStr(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


#endif
