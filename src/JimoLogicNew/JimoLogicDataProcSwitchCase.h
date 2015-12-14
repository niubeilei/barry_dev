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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcSwitchCase_h
#define AOS_JimoLogicNew_JimoLogicDataProcSwitchCase_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcSwitchCase : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString						mSwitchCaseName;
	AosExprObjPtr					mSwitchValue;
	vector<AosExprObjPtr >			mSwitchCase;
	vector<vector<AosExprObjPtr> >	mDataProcs;
	vector<vector<AosExprObjPtr> > 	mCase;
public:

	AosJimoLogicDataProcSwitchCase(const int version);
	~AosJimoLogicDataProcSwitchCase();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface

	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft);

	// AosJqlStatement interface
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool isparse);
	
	bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog);

private:

	bool createSwitchCaseDataProc(
				AosRundata *rdata,
				AosJimoProgObj *prog,
				OmnString &statement_str);
	
	bool addSwitchCaseTask(
				AosRundata *rdata, 
				AosJimoProgObj *prog);

	bool collectDataProcs(
			 	AosRundata *rdata,
 	  			const vector<AosExprObjPtr> &switchCase,
				vector<vector<AosExprObjPtr> > &dataprocs,
				vector<vector<AosExprObjPtr> > &cases);
	
	bool getDataprocNames(
			AosRundata *rdata,
			vector<vector<AosExprObjPtr> > &dataprocs,
			vector<OmnString> &dataproc_names);


};
#endif

