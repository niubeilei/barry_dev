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
// 2014/09/04 Created by Rain
////////////////////////////////////////////////////////////////////////////
#include "BSON/BFDocument.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFDocument_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFDocument(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		//AosLogError(rdata, false, "failed_creating_jimo") << enderr;
		OmnScreen << "failed_creating_jimo" << endl;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosBFDocument::AosBFDocument(const int version)
:
AosBsonField(version, eFieldTypeDocument)
{
}


AosBFDocument::~AosBFDocument()
{
}


AosJimoPtr 
AosBFDocument::cloneJimo() const
{
	return OmnNew AosBFDocument(*this);
}


bool
AosBFDocument::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// eString e_name document
	// document ::= u32 (length) e_list "\x00" 
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);
	OmnString vv = "It is a document name!";
	value.setStr(vv);

	pos += 4;
    return true;
}


//
//This method add length and data of the value
//to 'data' buffer. "pos" will be moved to the 
//end of 'data'. 
//
//'pos' is used for the caller to know the new
//data length
//
//Value is always in string format. It is up to
//the BSON field to parse the data into different
//types
//
bool
AosBFDocument::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff) 
{
	// This function appends 'varunint (length)' + 'string' (contents)
	// to 'buff'. 
	OmnString str = value.getStr();
	int len = str.length();

	//append 'varunint (length)'
	bool rslt = AosVarUnInt::encode((u32)len, buff);
	aos_assert_r(rslt, false);

	//append 'string' (contents)
	buff->gotoEnd();
	buff->setBuff(str.data(), len);
	return true;
}


bool 
AosBFDocument::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	//pos point to the start of 'u32' .
	//0x03 e_name document(u32 e_name ...)
	pos +=4;
	return true;
}


bool 
AosBFDocument::setType(
		AosRundata *rdata,
	    AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeDocument);
	return true;
}

/*
bool 
AosBFDocument::xpathSetAttr(
		AosRundata *rdata, 
		AosBuffPtr buff,
		const vector<OmnString> &fnames, 
		int crt_idx,
		const AosValueRslt &value, 
		const bool create)
{
	// This function sets a new field, if not exist yet. 
	// fnames[crt_idx] is its own name and fnames[crt_idx+1]
	// is the name of the newly added member. 
	
	mBSON = OmnNew AosBSON(buff);
	if ((u64)crt_idx + 1 == fnames.size() - 1) 
	{
		OmnString name = fnames[0];
		for (int i=1; i<=crt_idx; i++)
		{
			name << "." << fnames[i];
		}

		return mBSON->setDocFieldValue(fnames[0], name, fnames[crt_idx+1], value, rdata); 
	}

	return mBSON->xpathSetAttr(rdata, fnames, crt_idx, value, create);

	return true;
}
*/
