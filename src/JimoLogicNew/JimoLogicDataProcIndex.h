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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcIndex_h
#define AOS_JimoLogicNew_JimoLogicDataProcIndex_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcIndex : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	
	OmnString 										mType;
	OmnString										mParser;
	OmnString										mRecordName;
	OmnString										mInput;
	OmnString										mOpr;
	OmnString										mDocid;
	OmnString										mDocIndexName;
	AosExprObjPtr       							mCond;
	OmnString 										mShuffleType;
	OmnString 										mShuffleField;
	OmnString										mRecordType;
	OmnString	 									mTable;
	//OmnString 										mKeysep;	
	OmnString										mDataProcIndexName;
	OmnString										mDataProcIILName;
	vector<OmnString>								mIILName;
	vector<pair<OmnString, vector<OmnString> > >	mKeysType;
	vector<OmnString> 								mFields;
	OmnString										mInputName;
	bool											mHaveColumn;
	vector<OmnString>								mIndexes;
	vector<OmnString>								mIndexList;
	OmnString										mSelectName;
	OmnString										mSchemaName;

public:

	AosJimoLogicDataProcIndex(const int version);
	~AosJimoLogicDataProcIndex();
	
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
	bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog);

	//arvin 2015.08.21
	virtual OmnString getInput()const;
	
	virtual OmnString getTableName() const;

	virtual bool isExist(const OmnString &name,const OmnString &parm);

	virtual AosJimoLogicType::E getJimoLogicType()const {return AosJimoLogicType::eDataProcIndex;}
	
	// AosJqlStatement interface
	virtual bool run(
			AosRundata* rdata,
			AosJimoProgObj *prog,
			OmnString &statement_str,
			bool isparse);


private:

	bool createIndexOnTable(
			AosRundata *rdata, 
			AosJimoProgObj *prog,
			OmnString &statement_str,
			pair<OmnString,vector<OmnString> >  &keys_type);

    bool createIndexDataproc(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog,
			OmnString &statement_str,
			pair<OmnString,vector<OmnString> > &keys_type);

	bool createDocBatachoprDataproc(
			AosRundata *rdata, 
			AosJimoProgObj *prog, 
			OmnString &statement_str);
	
	bool createSelectDataproc(
			AosRundata *rdata, 
			AosJimoProgObj *jimo_prog, 
			OmnString &statement_str,
			int idx);

    bool createIILBatchoprDataproc(
			AosRundata *rdata, 
			AosJimoProgObj *jimo_prog,
			OmnString &type,
			OmnString &iilname,
			OmnString &statement_str);

 
	bool addIndexTask(
			AosRundata *rdata, 
			AosJimoProgObj *jimo_prog);
    
	bool addIILTask(
			AosRundata *rdata, 
			AosJimoProgObj *jimo_prog);
	

	bool configKeysType(
			AosRundata* rdata,
			AosXmlTagPtr &table_doc,
			const OmnString &name);


	bool getKeys(
			AosRundata *rdata, 
			OmnString &iil_name,
			AosXmlTagPtr &table_doc,
			vector<OmnString> &keys,
			const OmnString &name);
	
	OmnString getTypeFromKey(
			AosRundata *rdata,
			AosXmlTagPtr &table_doc,
			OmnString &key);

	bool addDocTask(
			AosRundata *rdata, 
			AosJimoProgObj *prog);

	bool getDataFields(
		vector<OmnString> &fields,
		const AosXmlTagPtr &tag,
		AosRundata * rdata);

	OmnString getHead(AosRundata *rdata,OmnString &type);

	OmnString checkInexNames(const vector<OmnString> &index_names);
};
#endif

