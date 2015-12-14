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
#ifndef AOS_JimoLogicNew_JimoLogicSetUserVar_h
#define AOS_JimoLogicNew_JimoLogicSetUserVar_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicSetUserVar : virtual public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:
	OmnString 			mVarName;
	AosValueRslt		mValue;

public:
	AosJimoLogicSetUserVar(const int version);
	~AosJimoLogicSetUserVar();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;


	// AosGenericObj interface
	virtual bool parseJQL(
			AosRundata *rdata, 
			AosJimoParserObj *jimo_parser, 
			AosJimoProgObj *prog,
			bool &parsed, 
			bool dft = false);
	
	bool compileJQL(AosRundata *rdata, AosJimoProgObj *prog);

	// AosJqlStatement interface
	virtual bool run(AosRundata *rdata, 
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool inparser) ;

};
#endif

