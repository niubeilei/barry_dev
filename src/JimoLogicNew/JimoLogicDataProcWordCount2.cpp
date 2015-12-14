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
// Statement Syntax:
// 		DATASET mydataset
// 		(
// 		    name: value,
// 		    name: value,
// 		    ...
// 		    name: value,
//		);
//
// Modification History:
// 2015/05/25 Created by Arvin
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcWordCount2.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include "WordParser/WordParser.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcWordCount2_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcWordCount2(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}	

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}

AosJimoLogicDataProcWordCount2::AosJimoLogicDataProcWordCount2(const int version):
AosJimoLogicDataProc(version),
AosJimoDataProc(version, AosJimoType::eDataProc),
AosJimoLogicDataProcNew(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcWordCount2::~AosJimoLogicDataProcWordCount2()
{
}


bool 
AosJimoLogicDataProcWordCount2::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	// The statement is in the form:
	// 	data proc wordcount <name>
	// 	(
	// 		inputs:[name],
	// 		keys:[key_field1,key_field2,...]
	// 	);
	
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "wordcount", rdata, false);
	mErrmsg << "dataproc wordcount ";
	// parse dataset name
	OmnString wordcount_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata, wordcount_name, /*prog,*/ mErrmsg))
	{
		// This is not what it expects.
		return false;
	}

	mDpName << "_dp_" << wordcount_name;

	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic(this, false);
	// bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	bool rslt = prog->addJimoLogicNew(rdata, mDpName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	OmnString output_name;
	mOutputNames.push_back(output_name);

	mErrmsg << wordcount_name;
	// parse name_value_list
	vector<AosExprObjPtr> name_value_list;
	rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	// parse inputs
	rslt = jimo_parser->getParmArrayStr( rdata, "inputs", name_value_list, mInputs);
	if (!rslt || mInputs.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
		return false;
	}

	//parse keys
	rslt = jimo_parser->getParmArrayStr( rdata, "keys", name_value_list, mWordKeys);
	if(!rslt || mWordKeys.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"keys",mErrmsg);
		return false;
	}


	if(!checkNameValueList(rdata,mErrmsg,name_value_list))
	{
		return false;
	}

	parsed = true;

	for (size_t i = 0; i<mWordKeys.size(); i++)
	{
		mWordMap[mWordKeys[i]] = 0;
	}
	
	OmnScreen << "wordcount parseJQL finish" << endl; 
	return true;
}


bool 
AosJimoLogicDataProcWordCount2::generateCode(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str)
{
	bool rslt = generateDataProcCode(rdata, prog, mDpName, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = generateTaskCode(rdata, prog, "wordcount", mDpName, "dataengine_scan2", mInputs);
	aos_assert_rr(rslt, rdata, false);

	OmnScreen << "wordcount generatecode finish" << endl; 
	return true;
}


bool
AosJimoLogicDataProcWordCount2::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcWordCount2::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcWordCount2(*this);
}


AosDataProcObjPtr
AosJimoLogicDataProcWordCount2::cloneProc() 
{
	return dynamic_cast<AosDataProc*>(OmnNew AosJimoLogicDataProcWordCount2(*this));
}


AosDataProcStatus::E
AosJimoLogicDataProcWordCount2::procData(
        AosRundata *rdata_raw,
        AosDataRecordObj **input_records,
        AosDataRecordObj **output_records)
{
    AosValueRslt value_rslt;
    AosWordParser parser;
    OmnString word = "";
    char *doc = NULL;
    int rcd_len = 0;
    int count = 0;
    map<OmnString, int>::iterator itr = mWordMap.end();
    AosDataRecordObj* input_record = input_records[0];

    rcd_len = input_record->getRecordLen();
    doc = input_record->getData(rdata_raw);
    OmnString keywords(doc, rcd_len);

    parser.setSrc(keywords);
    while(parser.nextWord(word))
    {
        itr = mWordMap.find(word);
        if (itr != mWordMap.end())
        {
            count = itr->second;
            mWordMap[word] = count + 1;
        }
    }

    return AosDataProcStatus::eContinue;
}

bool
AosJimoLogicDataProcWordCount2::finish(
        const vector<AosDataProcObjPtr> &procs,
        const AosRundataPtr &rdata)
{
	map<OmnString, int>::iterator itr;
	for (size_t i=0; i<procs.size(); i++)
	{
		map<OmnString, int> mp;
		mp = dynamic_cast<AosJimoLogicDataProcWordCount2*>(procs[i].getPtr())->mWordMap;
		for (itr = mp.begin(); itr != mp.end(); ++itr)
		{
			mWordMap[itr->first] += mp[itr->first];
		}
	}

	for (itr = mWordMap.begin(); itr != mWordMap.end(); ++itr)
	{
		OmnScreen << itr->first << ":" << itr->second<< endl;
	}
    return true;
}


bool 
AosJimoLogicDataProcWordCount2::semanticsCheck(
		AosRundata *rdata,
		AosJimoProgObj *prog)
{
	// This function checks whether this statement is semantically
	// correct. It returns false if it is not. 
	OmnScreen << "Wordcount semantics check: true" << endl;
	return true;
}
