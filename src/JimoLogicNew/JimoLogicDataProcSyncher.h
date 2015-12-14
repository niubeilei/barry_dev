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
// 2015/05/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicDataProcSyncher_h
#define AOS_JimoLogicNew_JimoLogicDataProcSyncher_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcSyncher : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString 					mDPName;
	OmnString	 				mTableName;
	OmnString					mDBName;
	OmnString					mOperator;
	OmnString					mInputs;
	OmnString					mInputName;

public:
	AosJimoLogicDataProcSyncher(const int version);
	~AosJimoLogicDataProcSyncher();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	//JimoLogicObjNew interface
	bool compileJQL(AosRundata *rdata, AosJimoProgObj *prog);

	//arvin 2015.08.21
	virtual OmnString getInput()const;
	virtual OmnString getTableName() const;
	virtual AosJimoLogicType::E getJimoLogicType()const {return AosJimoLogicType::eDataProcSyncher;}
	
	// AosJqlStatement interface
	virtual bool run(AosRundata* rdata, AosJimoProgObj *prog, OmnString &statement_str, bool inparser);

private:
	bool createJSON(
				AosRundata *rdata,
				AosJimoProgObj *jimo_prog,
				OmnString &statement_str);

	bool createTask(
        		AosRundata *rdata,
        		AosJimoProgObj *jimo_prog);

};
#endif

