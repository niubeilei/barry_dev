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
// 2014/11/19 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CSVAnalyzer_BasicDataAnalyzer_h
#define Aos_CSVaAnalyzer_BasicDataAnalyzer_h

#include "DataAnalyzer/DataAnalyzer.h"


class AosBasicDataAnalyzer : public AosDataAnalyzer
{
	OmnDefineRCObject;

protected:

public:
	AosBasicDataAnalyzer(const int version);
	~AosBasicDataAnalyzer();

	virtual bool	config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool	analyze(
						AosRundata *rdata,
						AosBuff *data, 
						vector<Aos> &parms);
};
#endif

