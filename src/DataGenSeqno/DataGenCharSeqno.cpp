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
// This is a utility to select docs.
//
// Modification History:
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenSeqno/DataGenCharSeqno.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"

AosDataGenCharSeqno::AosDataGenCharSeqno(const bool reg)
:
AosDataGenSeqno(AOSDATGGENTYPE_CHAR_SEQNO, AosDataGenSeqnoType::eCharSeqno, reg)
{
}


AosDataGenCharSeqno::~AosDataGenCharSeqno()
{
}


bool 
AosDataGenCharSeqno::createSeqno(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// <xxx zky_prefix = "xxx" zky_postfix="xxx" zky_start="xxx"
	// 		zky_end ="xxx"  zky_step="xxx" zky_reusable="xxx"
	// 		zky_exceptions="xxx" zky_length="xxx" zky_padding="xxx"/>
	
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
	    datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
	    rdata->setError() << "Datatype incorrect: " << datatype;
	    return false;
	}
   	u64 start = getAttrU64Value(sdoc, AOSTAG_START, 1, rdata);
    u64 end = getAttrU64Value(sdoc, AOSTAG_END, eCharMaxNum, rdata);

	if (end > eCharMaxNum)
	{
		rdata->setError() << "Missing end!";
		return false;
	}

	u64 vvv = sdoc->getAttrU64(AOSTAG_SEQNO_VALUE, 0);
	if (vvv == 0) vvv = start;
	else vvv += 1;

	bool reusabel = sdoc->getAttrBool(AOSTAG_SEQNO_REUSABLE, true);
	if (vvv > end && !reusabel)
	{
		rdata->setError() << "value Big!";
		OmnAlarm <<rdata->getErrmsg() << enderr;
		return true;
	}
	if (vvv > end) vvv = start;

	OmnString vv = getNumberChar(vvv);
	aos_assert_r(vv != "", false);
	int len = strlen(vv.data());

	OmnString prefix = getAttrStrValue(sdoc, AOSTAG_SEQNO_PREFIX, "", rdata);
	OmnString postfix = getAttrStrValue(sdoc, AOSTAG_SEQNO_POSTFIX, "", rdata);
	u64 length = getAttrU64Value(sdoc, AOSTAG_SEQNO_LENGTH, 0, rdata);
	OmnString padding = getAttrStrValue(sdoc, AOSTAG_SEQNO_PADDING, "0", rdata);

	for (; len <= (int)length; len++) prefix << padding; 
	prefix << vvv << postfix;
	prefix.removeLeadingWhiteSpace();
	
	value.setStr(prefix);
	sdoc->setAttr(AOSTAG_SEQNO_VALUE, vvv);
	bool rslt = modifyDoc(sdoc, rdata);
	aos_assert_r(rslt, false);
	return true;
}


OmnString
AosDataGenCharSeqno::getNumberChar(u64 &num)
{
	// Arabic numerals into char numerals(1-625)<a-zz> 
	if (num <= 0 || num > eCharMaxNum) return "";
	OmnString value; 
	u64 idx = (num/25);
	if (idx != 0) value << (char)(96+idx);
	idx = num % 25;
	if (idx != 0) value << (char)(96+idx);
	return value;
}

bool
AosDataGenCharSeqno::createSeqno(
			AosValueRslt &value,
			const AosRundataPtr &rdata)

{
	OmnNotImplementedYet;
	return false;
}
