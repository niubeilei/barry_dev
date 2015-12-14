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
#include "JimoLogicNew/JimoLogicSetUserVar.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicSetUserVar_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicSetUserVar(version);
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


AosJimoLogicSetUserVar::AosJimoLogicSetUserVar(const int version)
:
AosJimoLogicNew(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicSetUserVar::~AosJimoLogicSetUserVar()
{
}


bool 
AosJimoLogicSetUserVar::parseJQL(
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
	//aos_assert_rr(mKeywords[0] == "uservar", rdata, false);

	// 1. var name 
	// 2. var value 
	mVarName = jimo_parser->nextKeyword(rdata);
	OmnString value =  jimo_parser->nextObjName(rdata);

	//jimo_parser->reset();
	//OmnString value = jimo_parser->nextKeyword(rdata);	
	//// value to -> type
	mValue = AosValueRslt(value);

	mValue = mValue;
	mVarName = mVarName;
	
	parsed = true;
	return true;
}


bool 
AosJimoLogicSetUserVar::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	prog->setUserVarValue(rdata, mVarName, mValue);
	return true;
}


bool
AosJimoLogicSetUserVar::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicSetUserVar*>(this);
	prog->addJimoLogicNew(rdata, mVarName, jimologic);
	
	OmnNotImplementedYet;
	return true;
}


AosJimoPtr 
AosJimoLogicSetUserVar::cloneJimo() const
{
	return OmnNew AosJimoLogicSetUserVar(*this);
}
