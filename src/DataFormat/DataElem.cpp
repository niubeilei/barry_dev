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
// 03/01/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DataField. Chen Ding, 07/08/2012
#include "DataFormat/DataElem.h"

#include "Actions/SdocAction.h"
#include "DataFormat/DataElem.h"
#include "DataFormat/DataElemStr.h"
#include "DataFormat/DataElemStrRandom.h"
#include "DataFormat/DataElemTime.h"
#include "DataFormat/DataElemU64.h"
#include "IILAssembler/IILAssembler.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosDataElem::AosDataElem(const AosDataElemType::E type)
:
mRawFormat(eRawFormat_Invalid),
mType(type),
mName(""),
mDataLen(-1),
mOffset(-1),
mIsConst(false),
mIsFixedPosition(true),
mHasValidValue(false)
{
}


AosDataElem::~AosDataElem()
{
}


bool
AosDataElem::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	//  <dataelem
	//      AOSTAG_TYPE="xxx"
	//      AOSTAG_NAME="xxx"					The field's name
	//      AOSTAG_OFFSET="xxx"					The field's position
	//      AOSTAG_ISCONST="xxx"				Whether this is a constant value.
	//      AOSTAG_FIXED_POSITION="true|false"	Whether the position is fixed or not.
	//      AOSTAG_RAW_FORMAT="u64|str|..."		How original data is expressed
	//      AOSTAG_VALUE="xxx" 					Needed only when AOSTAG_ISCONST is true
	//      AOSTAG_FIELD_OPR="xxx" 				
	//      AOSTAG_LENGTH="xxx"> 				Data Length
	//      <data_filter .../>					An action used to filter data.
	//  </dataelem>
	aos_assert_rr(def, rdata, false);
	
	mName = def->getAttrStr(AOSTAG_NAME);
	mOffset = def->getAttrInt(AOSTAG_OFFSET, -1);
	mIsConst = def->getAttrBool(AOSTAG_ISCONST, false);
	mIsFixedPosition = def->getAttrBool(AOSTAG_FIXED_POSITION, true);
	mFieldOpr = AosFieldOpr::toEnum(def->getAttrStr(AOSTAG_FIELD_OPR));
	mFromDocidIdx = def->getAttrInt(AOSTAG_FROM_DOCID_IDX, -1);
	mSetRecordDocid = def->getAttrBool(AOSTAG_SET_RECORD_DOCID, false);

	// Parse the raw format
	OmnString ss = def->getAttrStr(AOSTAG_RAW_FORMAT);
	if (ss == "u64")
	{
		mRawFormat = eRawFormat_U64;
	}
	else if(ss == "str")
	{
		mRawFormat = eRawFormat_Str;
	}
	else
	{
		AosSetErrorU(rdata, "invalid_raw_format") << ": " << ss << ":" << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	// Configure the condition.
	AosXmlTagPtr tag = def->getFirstChild(AOSTAG_CONDITION);
	if (tag)
	{
		// Chen Ding, 05/31/2012
		// Conditions are optional. If not specified, do nothing.
		AosCondType::E type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
		if (AosCondType::isValid(type))
		{
			mFilter = AosCondition::getCondition(tag, rdata);
		}
	}
	return true;
}


bool 	
AosDataElem::setRecordByPrevValue(const AosDataRecord *record)
{
	aos_assert_r(record, false);
	return setRecordByPrevValue(record.getData(), record.getRecordLen());
}


AosDataElemPtr 
AosDataElem::createElem(
		const AosXmlTagPtr &element, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(element, 0);

	OmnString typeStr = element->getAttrStr(AOSTAG_TYPE);
	AosDataElemType::E type = AosDataElemType::toEnum(typeStr);
	aos_assert_r(AosDataElemType::isValid(type), 0);
	
	AosDataElemPtr elem;
	switch(type)
	{
	case AosDataElemType::eStr :
		 elem = OmnNew AosDataElemStr(element, rdata);
		 break;
	
	case AosDataElemType::eStrRandom :
		 elem = OmnNew AosDataElemStrRandom(element, rdata);
		 break;
	
	case AosDataElemType::eTime :
		 elem = OmnNew AosDataElemTime(element, rdata);
		 break;
	
	case AosDataElemType::eU64 :
		 elem = OmnNew AosDataElemU64(element, rdata);
		 break;

	default :
		 break;
	}
	
	return elem;
}
#endif
	
