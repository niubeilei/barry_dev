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
#include "DataGenSeqno/DataGenArabicSeqno.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"

const u64 sgDefEnd = (((u64)0x00ffffff) << 32) + 0xffffffff;
AosDataGenArabicSeqno::AosDataGenArabicSeqno(const bool reg)
:
AosDataGenSeqno(AOSDATGGENTYPE_ARABIC_SEQNO, AosDataGenSeqnoType::eArabicSeqno, reg),
mStep(0)
{
}

AosDataGenArabicSeqno::AosDataGenArabicSeqno(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
:
AosDataGenSeqno(AOSDATGGENTYPE_ARABIC_SEQNO, AosDataGenSeqnoType::eArabicSeqno, false)
{
	aos_assert(parse(config, rdata));
}

AosDataGenArabicSeqno::~AosDataGenArabicSeqno()
{
}


bool 
AosDataGenArabicSeqno::createSeqno(
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

	u64 vvv = sdoc->getAttrU64(AOSTAG_SEQNO_VALUE, 0);
   	u64 start = getAttrU64Value(sdoc, AOSTAG_START, eDftStart, rdata); 
    u64 end = getAttrU64Value(sdoc, AOSTAG_END, sgDefEnd, rdata);
	u64 step = getAttrU64Value(sdoc, AOSTAG_STEP, eDftStep, rdata);
	if (vvv == 0) vvv = start;
	else vvv += step;
	bool reusabel = sdoc->getAttrBool(AOSTAG_SEQNO_REUSABLE, true);
	if (vvv > end && !reusabel)
	{
		rdata->setError() << "value Big!";
		OmnAlarm <<rdata->getErrmsg() << enderr;
		return true;
	}
	if (vvv > end) vvv = start;
	OmnString vv;
	vv << vvv;
	int len = strlen(vv.data());

	OmnString prefix = getAttrStrValue(sdoc, AOSTAG_SEQNO_PREFIX, "", rdata);
	OmnString postfix = getAttrStrValue(sdoc, AOSTAG_SEQNO_POSTFIX, "", rdata);

	u64 length = getAttrU64Value(sdoc, AOSTAG_SEQNO_LENGTH, 0, rdata);
	OmnString padding = getAttrStrValue(sdoc, AOSTAG_SEQNO_PADDING, "0", rdata);

	for (; len < (int)length; len++) prefix << padding; 
	prefix<< vvv << postfix;
	prefix.removeLeadingWhiteSpace();
	
	value.setStr(prefix);
	sdoc->setAttr(AOSTAG_SEQNO_VALUE, vvv);
	bool rslt = modifyDoc(sdoc, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool 
AosDataGenArabicSeqno::createSeqno(
		 AosValueRslt &value,
		 const AosRundataPtr &rdata)
{
	aos_assert_r(mStep, false);
	if (mSeqnovalue == 0) mSeqnovalue = mStart;
	else mSeqnovalue += mStep;
	OmnString vv;
	vv << mSeqnovalue;
	int len = strlen(vv.data());
	OmnString val = mPrefix;
	for (; len < (int)mLength; len++) val << mPadding; 
	val<< mSeqnovalue << mPostfix;
	val.removeLeadingWhiteSpace();
	value.setStr(val);
	return true;
}


bool
AosDataGenArabicSeqno::parse(
		const AosXmlTagPtr &config,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(config, rdata, false);
	mSeqnovalue = config->getAttrU64(AOSTAG_SEQNO_VALUE, 0);
   	mStart = getAttrU64Value(config, AOSTAG_START, eDftStart, rdata); 
    mEnd = getAttrU64Value(config, AOSTAG_END, sgDefEnd, rdata);
	mStep = getAttrU64Value(config, AOSTAG_STEP, eDftStep, rdata);
	mPrefix = getAttrStrValue(config, AOSTAG_SEQNO_PREFIX, "", rdata);
	mPostfix = getAttrStrValue(config, AOSTAG_SEQNO_POSTFIX, "", rdata);
	mLength = getAttrU64Value(config, AOSTAG_SEQNO_LENGTH, 0, rdata);
	mPadding = getAttrStrValue(config, AOSTAG_SEQNO_PADDING, "0", rdata);
	u64 totaldoc = getAttrU64Value(config, "zky_totaldoc", 1, rdata);
	int vvv = mSeqnovalue + (totaldoc * mStep);
	if (mSeqnovalue == 0) vvv = vvv + (mStart - mStep);
	//int vvv = mSeqnovalue + totaldoc + mStart; 
	aos_assert_rr(vvv >= 0 && (u64)vvv < mEnd, rdata, false);
	config->setAttr(AOSTAG_SEQNO_VALUE, vvv);
	bool rslt = modifyDoc(config, rdata);
	aos_assert_r(rslt, false);
	return true;
}
