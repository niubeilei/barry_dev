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
// 2015/07/09	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicUserVar.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicUserVar_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicUserVar(version);
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


AosJimoLogicUserVar::AosJimoLogicUserVar(const int version)
:
AosJimoLogicNew(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicUserVar::~AosJimoLogicUserVar()
{
}


bool 
AosJimoLogicUserVar::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	uservar <data_type> <access_mode> <name>;
	//
	
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 1, rdata, false);
	aos_assert_rr(mKeywords[0] == "uservar", rdata, false);

	// parse data type
	OmnString data_type = jimo_parser->nextObjName(rdata);
	if (data_type == "")
	{
		// This is not what it expects.
		return true;
	}

	// parse access mode
	OmnString access_mode = jimo_parser->nextObjName(rdata);
	if (access_mode == "")
	{
		return false;	
	}

	// parse var name
	OmnString var_name = jimo_parser->nextObjName(rdata);
	if (var_name == "")
	{
		return false;
	}

	mDataType = data_type;
	mAccessMode = access_mode;
	mVarName = var_name;
	
	parsed = true;
	return true;
}


bool 
AosJimoLogicUserVar::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	//prog->appendStatement(rdata, "uservar", mVarName);
	AosValueRslt value;
	prog->addUserVar(rdata, mVarName, this);
	return true;
}


bool
AosJimoLogicUserVar::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicUserVar*>(this);
	prog->addJimoLogicNew(rdata, mVarName, jimologic);
	
	OmnNotImplementedYet;
	return true;
}


AosJimoPtr 
AosJimoLogicUserVar::cloneJimo() const
{
	return OmnNew AosJimoLogicUserVar(*this);
}


bool 
AosJimoLogicUserVar::setUserVarValue(
		AosRundata *rdata, 
		const AosValueRslt &value)
{
	mValue = value;
	return true;
}


AosValueRslt 
AosJimoLogicUserVar::getUserVarValue(AosRundata *rdata)
{
	return mValue;
}
