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
#ifndef AOS_JimoLogicNew_JimoLogicDatasetDir_h
#define AOS_JimoLogicNew_JimoLogicDatasetDir_h

#include "JimoLogicNew/JimoLogicDatasetDir.h"
#include "JimoLogicNew/JimoLogicDataset.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDatasetDir : virtual public AosJimoLogicDataset
{
	OmnDefineRCObject;

protected:
	//select type
	OmnString		    			mType;
	vector<AosExprObjPtr> 			mNameValueList;  //save a list of name:value which have been parsed
    OmnString           			mDatasetName;    //save dataSetDir name
	vector<vector<AosExprObjPtr> >	mDirs; 		 //use to save the data dirs
	AosExprObjPtr					mDefineSchema;  //use to save schema that user defines
    OmnString           			mSchemaName;      //use to save schema name which have been exist
	vector<AosExprObjPtr>   		mSplitter;  //save a list of name:value which have been parsed
	//OmnString			mInput;
	//OmnString	        mCond;
	vector<AosExprObjPtr>   		mNormalSchemaField;  //use to save normal field like "type", "charset"....
	vector<vector<AosExprObjPtr> >  mSchemaFields;  //use to save the field of fields;
	OmnString           			mTableName;
	OmnString 						mStrSchema;		//when table name is exist, wo use mStrSchema to save Schema field;
	vector<OmnString>				mOutputNames;
	OmnString						mCharset;
	//arvin 2015.07.31
	OmnString 						mDirStr; 		
	// 2015.10.12
	OmnString						mIsSkipFirstLine;
public:
	AosJimoLogicDatasetDir(const int version);
	~AosJimoLogicDatasetDir();

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
					bool isparser);

	virtual bool getSchemaName(
				AosRundata *rdata, 
				AosJimoProgObj *prog,
				vector<OmnString> &schema_name);

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;
	
	vector<OmnString>	getOutputNames();

	// jimodb-753
	bool getInputV(vector<OmnString> &inputs); 
	
private:
	bool createDirDataproc(
			AosRundata *rdata, 
			AosJimoProgObj* jimo_prog,
			OmnString &statements_str);

	bool getSchemaByTableName(
			AosRundata*		rdata, 
			const OmnString &name);

	bool getSchemaBySchemaName(
			AosRundata*		rdata, 
			const OmnString &name,
			const OmnString	&charset);

	bool parseDir(AosRundata *rdata);

};
#endif

