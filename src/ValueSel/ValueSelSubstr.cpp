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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelSubstr.h"

#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocSelObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0

AosValueSelSubstr::AosValueSelSubstr(const bool reg)
:
AosValueSel(AOSACTOPRID_SUBSTR, AosValueSelType::eSubstr, reg)
{
}


AosValueSelSubstr::AosValueSelSubstr(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_SUBSTR, AosValueSelType::eSubstr, false)
{
	if (!config(sdoc, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosValueSelSubstr::~AosValueSelSubstr()
{
}


bool
AosValueSelSubstr::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	mType = toEnum(def->getAttrStr(AOSTAG_SUBSTR_TYPE));
	if (isValidType(mType))
	{
		AosSetErrorU(rdata, "invalid_substr_type") << ": " << def->toString() << enderr;
		return false;
	}

	switch (mType)
	{
	case eSubstr:
		 mStartPos = def->getAttrInt(AOSTAG_START_POS, -1);
		 if (mStartPos < 0)
		 {
			 AosSetErrorU(rdata, "invalid_startpos") << ": " << def->toString() << enderr;
			 return false;
		 }

		 mLength = def->getAttrInt(AOSTAG_LENGTH, -1);
		 if (mLength <= 0)
		 {
			 AosSetErrorU(rdata, "invalid_length") << ": " << def->toString() << enderr;
			 return false;
		 }
		 break;

	case ePick:
		 if (!configPick(def, rdata)) return false;
		 break;

	case eCompose:
		 if (!configCompose(def, rdata)) return false;
		 break;

	default:
		 AosSetErrorU(rdata, "invalid_substr_type") << ": " << def->toString() << enderr;
		 return false;
	}

	mCopyFlag = def->getAttrBool(AOSTAG_COPY_FLAG, true);
	return true;
}


bool
AosValueSelSubstr::configPick(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosValueSelSubstr::configCompose(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueSelSubstr::run(
		AosValueRslt &value,
		const char *record, 
		const int record_len, 
		const AosRundataPtr &rdata)
{
	// This value selector selects a substring from 'record'.
	if (mStartPos < 0 || mLength <= 0)
	{
		AosSetErrorU(rdata, "not_configured") << enderr;
		return false;
	}

	if (!record)
	{
		AosSetErrorU(rdata, "record_is_null") << enderr;
		return false;
	}

	if (mStartPos + mLength >= record_len)
	{
		AosSetErrorU(rdata, "data_too_short") << ": " << record_len
			<< ":" << mStartPos << ":" << mLength << enderr;
		return false;
	}

	// Chen Ding, 07/08/2012
	// if (mCopyFlag)
	// {
	// 	value.setValue(&record[mStartPos], mLength);
	// 	return true;
	// }

	//value.setCharStr1(&record[mStartPos], mLength, mCopyFlag);
	OmnString str(&record[mStartPos], mLength, mCopyFlag);
	value.setStr(str);
	return true;
}


bool
AosValueSelSubstr::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This selector selects a substr from a given string. The string
	// may be separated by a separator. 
	// 	<sdoc AOSTAG_VALUE_TYPE=AOSACTOPRID_SUBSTR
	// 		AOSTAG_SEPARATOR="xxx"
	// 		AOSTAG_DATA_TYPE="xxx"
	// 		AOSTAG_FIELD_INDEX="xxx">
	// 		<AOSTAG_SOURCESTR .../> 	// This is a value selector
	aos_assert_r(rdata, false);
	aos_assert_rr(sdoc, rdata, false);
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorU(rdata, "invalid_data_type") << sdoc->toString() << enderr;
		return false;
	}

	// Retrieve copy_memory_flag
	bool copy_memory_flag = sdoc->getAttrBool(AOSTAG_COPY_MEMORY, true);

	// Retrieve the value selector
	AosXmlTagPtr selector = sdoc->getFirstChild(AOSTAG_SOURCESTR);
	if (!selector)
	{
		AosSetErrorU(rdata, "missing_source_string") << sdoc->toString() << enderr;
		return false;
	}

	AosValueRslt source_value;
	if (!getValueStatic(source_value, selector, rdata)) return false;
	OmnString source_str = source_value.getStr();

	// Retrieve the field index
	int field_idx = sdoc->getAttrInt(AOSTAG_FIELD_IDX, -1);
	if (field_idx >= 0)
	{
		// Retrieve the separator
		OmnString separator = sdoc->getAttrStr(AOSTAG_SEPARATOR);
		if (separator == "")
		{
			AosSetErrorU(rdata, "missing_separator") << sdoc->toString() << enderr;
			return false;
		}

		// It is to retrieve a substring by separator:
		return getValuePriv(source_str.data(), source_str.length(), 
				separator, field_idx, valueRslt, copy_memory_flag, rdata);
	}

	// It is to retrieve a substring by position
	int start_pos = sdoc->getAttrInt(AOSTAG_START_POS, -1);
	int len = sdoc->getAttrInt(AOSTAG_LENGTH, -1);
	if (start_pos < 0)
	{
		AosSetErrorU(rdata, "missing_start_pos") << sdoc->toString() << enderr;
		return false;
	}

	if (len <= 0)
	{
		AosSetErrorU(rdata, "missing_start_pos") << sdoc->toString() << enderr;
		return false;
	}

	return getValuePriv(source_str.data(), source_str.length(), 
				start_pos, len, copy_memory_flag, valueRslt, rdata);
}


bool
AosValueSelSubstr::getValuePriv(
		const char *data, 
		const int data_len,
		const OmnString &separator, 
		const int field_idx, 
		AosValueRslt &value, 
		const bool copy_memory_flag,
		const AosRundataPtr &rdata)
{
	const char *vv = 0;
	int len = 0;
	bool rslt = AosGetField(vv, len, data, data_len, field_idx, separator, rdata.getPtrNoLock());
	if (!rslt) return false;

	// Chen Ding, 07/08/2012
	// if (!copy_memory_flag)
	// {
		OmnString str(vv, len);
		value.setStr(str);
		return true;
	// }

	// value.setValue(vv, len);
	return true;
}


bool
AosValueSelSubstr::getValuePriv(
		const char *data, 
		const int data_len,
		const int start_pos, 
		const int len, 
		const bool copy_memory_flag,
		AosValueRslt &value, 
		const AosRundataPtr &rdata)
{
	if (start_pos + len > data_len)
	{
		AosSetErrorU(rdata, "data_too_short") << ". Data Length: " << data_len
			<< ", Start Position: " << start_pos
			<< ", Substring Length: " << len << enderr;
		return false;
	}

	if (start_pos < 0)
	{
		AosSetErrorU(rdata, "invalid_start_pos") << start_pos << enderr;
		return false;
	}

	if (len < 0)
	{
		AosSetErrorU(rdata, "invalid_length") << len << enderr;
		return false;
	}

	// Chen Ding, 07/08/2012
	// if (!copy_memory_flag)
	// {
		// It does not copy values, just set the pointer.
		// return value.setCharStr(&data[start_pos], len, true);
		//return value.setCharStr1(&data[start_pos], len, copy_memory_flag);
		OmnString str(&data[start_pos], len);
		value.setStr(str);
		return true;
	// }

	// value.setValue(&data[start_pos], len);
	return true;
}


AosValueSelObjPtr
AosValueSelSubstr::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelSubstr(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif
