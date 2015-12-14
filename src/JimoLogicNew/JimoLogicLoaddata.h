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
#ifndef AOS_JimoLogicNew_JimoLogicLoaddata_h
#define AOS_JimoLogicNew_JimoLogicLoaddata_h

#include "JimoLogicNew/JimoLogicLoaddata.h"
#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicLoaddata : virtual public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	//select type
	OmnString		    			mType;
    OmnString           			mDatasetName;    
	OmnString           			mTableName;
	OmnString 						mStrSchema;		
	OmnString						mSchemaName;
	vector<OmnString>				mSubTableList;
	OmnString						mThread;


public:
	AosJimoLogicLoaddata(const int version);
	~AosJimoLogicLoaddata();

	virtual bool parseJQL(
			AosRundata *rdata, 
			AosJimoParserObj *jimo_parser, 
			AosJimoProgObj *prog,
			bool &parsed, 
			bool dft = false);

	bool compileJQL(AosRundata *rdata, AosJimoProgObj *prog);

	// AosGenericObj interface
	virtual bool run(AosRundata *rdata, 
					AosJimoProgObj* job,
					//const OmnString &verb_name,
					OmnString &statements_str,
					bool inparser);

	virtual bool getSchemaName(
				AosRundata *rdata, 
				AosJimoProgObj *prog,
				vector<OmnString> &schema_name);

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;
	
	vector<OmnString>	getOutputNames();

	
private:
	bool createLoaddataDataproc(
			AosRundata *rdata, 
			AosJimoProgObj* jimo_prog,
			OmnString &statements_str);

};
#endif

