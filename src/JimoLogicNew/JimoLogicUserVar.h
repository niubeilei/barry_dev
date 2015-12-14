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
#ifndef AOS_JimoLogicNew_JimoLogicUserVar_h
#define AOS_JimoLogicNew_JimoLogicUserVar_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicUserVar : virtual public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:
	OmnString			mDataType;
	OmnString			mAccessMode;
	OmnString 			mVarName;

	AosValueRslt 		mValue;
public:
	AosJimoLogicUserVar(const int version);
	~AosJimoLogicUserVar();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;



	// AosGenericObj interface
	
	virtual bool setUserVarValue(AosRundata *rdata, 
									const AosValueRslt &value);
	virtual AosValueRslt getUserVarValue(AosRundata *rdata);

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

