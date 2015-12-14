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
// 2015/05/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicSchemaFixlength_h
#define AOS_JimoLogicNew_JimoLogicSchemaFixlength_h

#include "JimoLogicNew/JimoLogicSchema.h"

class AosJimoLogicSchemaFixlength : virtual public AosJimoLogicSchema
{
	OmnDefineRCObject;

private:
	//vector<vector<AosExprObjPtr> >		mSchemaFields;
	vector<Field>			mFieldDefs;
	OmnString				mRecordDelimiter;
	int64_t						mRecordLength;

public:
	AosJimoLogicSchemaFixlength(const int version);
	~AosJimoLogicSchemaFixlength();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool inparser);

private:
	bool createSchemaDataproc(
				AosRundata *rdata,
				AosJimoProgObj* jimo_prog,
				OmnString &statements_str);
};
#endif

