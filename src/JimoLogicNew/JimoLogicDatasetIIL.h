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
#ifndef AOS_JimoLogicNew_JimoLogicDatasetIIL_h
#define AOS_JimoLogicNew_JimoLogicDatasetIIL_h

#include "JimoLogicNew/JimoLogicDataset.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDatasetIIL : virtual public AosJimoLogicDataset
{
	OmnDefineRCObject;

protected:
	//select type
	OmnString		    			mType;
	vector<AosExprObjPtr> 			mNameValueList;  //save a list of name:value which have been parsed
    OmnString           			mDatasetName;    //save dataSetDir name

	AosExprObjPtr					mDefineSchema;  //use to save schema that user defines
    OmnString           			mSchemaName;      //use to save schema name which have been exist
	vector<AosExprObjPtr>   		mSplitter;  //save a list of name:value which have been parsed
	vector<AosExprObjPtr>   		mNormalSchemaField;  //use to save normal field like "type", "charset"....
	vector<vector<AosExprObjPtr> >  mSchemaFields;  //use to save the field of fields;
	OmnString           			mCondition;
	OmnString 						mStrSchema;		//when table name is exist, wo use mStrSchema to save Schema field;
	OmnString						mIILName;   

    OmnString           			mDataprocIILName ;
	OmnString           			dataprociil_str;  
	OmnString 						mErrmsg;


public:
	AosJimoLogicDatasetIIL(const int version);
	~AosJimoLogicDatasetIIL();

	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	bool compileJQL(AosRundata*, AosJimoProgObj*);

	// AosGenericObj interface
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj* job,
					OmnString &statements_str,
					bool isparser);

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

private:
	bool getSchemaByTableName(
				AosRundata*		rdata, 
				const OmnString &name,
				const AosJimoLogicDatasetIILPtr &stmt);

	bool getSchemaBySchemaName(
				AosRundata*		rdata, 
				const OmnString &name);

	bool createIILDataproc(
			AosRundata *rdata, 
			AosJimoProgObj* jimo_prog,
			OmnString &statements_str);
};
#endif

