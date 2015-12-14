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
// 2015/05/25 Created by Arvin
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicDataProcNew_h
#define AOS_JimoLogicNew_JimoLogicDataProcNew_h

#include "Util/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "JimoLogicNew/JimoLogicDataProc.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcNew :  virtual public AosJimoLogicDataProc, virtual public AosJimoDataProc
{
public:

	AosJimoLogicDataProcNew(const int version);
	~AosJimoLogicDataProcNew();

	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool inparser);

	virtual bool generateCode(
					AosRundata *rdata, 
					AosJimoProgObj *prog,
					OmnString &statement_str);

	virtual bool generateDataProcCode(
							AosRundata *rdata,
							AosJimoProgObj *prog,
							OmnString dpname,
							OmnString &statement_str);

	virtual bool generateTaskCode(
					AosRundata *rdata, 
					AosJimoProgObj *prog,
					const OmnString objname,
					OmnString &dpName,
					const OmnString dataEnginType,
					vector<OmnString> inputs);


	

};
#endif

