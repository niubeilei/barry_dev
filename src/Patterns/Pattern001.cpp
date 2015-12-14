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
// 2014/08/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoProg/Pattern001.h"

#include "API/AosApi.h"
#include "JimoProg/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosPattern001_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosPattern001(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}



AosPattern001::AosPattern001(const int version)
:
AosPattern(version)
{
}


AosPattern001::~AosPattern001()
{
}


bool
AosPattern001::config(
		Rundata *rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosPattern001::cloneJimo() const
{
	return OmnNew AosPattern001(*this);
}


bool
AosPattern001::run(
		AosRundata *rdata, 
		AosStatRundata *stat_rdata)
{
	// This pattern assumes 'jimo_rdata' 
	// When a one-dimensional vector is modified, this 
	// pattern checks the following:
	// 		elem[i-n+1] >= value and
	// 		elem[i-n+2] >= value and
	// 		elem[i] >= value
	// If this is true, 
	u32 *elems = jimo_rdata->getInputVector();
	int crt_pos = jimo_rdata->getCrtPos();
	aos_assert_rr(elems, rdata, false);

	int num = 0;
	while (1)
	{
		while (crt_pos-num >= 0 && num < mMaxOccurrences)
		{
			switch (mOpr)
			{
			case eLT: if (elems[crt_pos-num++] >= mValue) return true;
			case eLE: if (elems[crt_pos-num++] > mValue) return true;
			case eEQ: if (elems[crt_pos-num++] != mValue) return true;
			case eNE: if (elems[crt_pos-num++] == mValue) return true;
			case eGT: if (elems[crt_pos-num++] <= mValue) return true;
			case eGE: if (elems[crt_pos-num++] < mValue) return true;
			default: 
				 AosLogError(rdata, "data_error") << enderr;
				 return false;
			}
		}
	
		if (num >= mMaxOccurrences)
		{
			// Found it. 
		}
	}
