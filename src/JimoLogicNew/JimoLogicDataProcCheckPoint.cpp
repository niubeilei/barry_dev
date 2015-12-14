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
// 2015/09/19 Created by Bryant Zhou 
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcCheckPoint.h"

#include "API/AosApi.h"

#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcCheckPoint_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcCheckPoint(version);
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


AosJimoLogicDataProcCheckPoint::AosJimoLogicDataProcCheckPoint(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcCheckPoint::~AosJimoLogicDataProcCheckPoint()
{
}


bool 
AosJimoLogicDataProcCheckPoint::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	//<dataproc><![CDATA[
	//	  dataproc checkpoint dataprocname
	//    {
	//         fields:
	//         (
	//				(fname:f1),
	//				(fname:f2),
	//				...
	//				(fname:fn)
	//         ),
	//         inputs:datasetname,
	//         speed:100
	//     }
	//]]></dataproc>


	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "checkpoint", rdata, false);
	mErrmsg = "dataproc checkpoint ";

	// parse dataproc name
	OmnString dataproc_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dataproc_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dataproc_name;
	
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		//setErrMsg(rdata, 0, "",mErrmsg);
		setErrMsg(rdata, eGenericError, "",mErrmsg);
		return false;
	}

	mDataProcName = dataproc_name;
	mNameValueList = name_value_list;

	// Configure the common attributes
	if (!configCommonAttrs(rdata,jimo_parser, name_value_list,mErrmsg))
	{
	     AosLogError(rdata, true, "missing_common_attributes");
         return false;
	}
	
	//parse inputs
	OmnString attrname = "inputs";
	OmnString input = jimo_parser->getParmStr(rdata, attrname, name_value_list);
	mInput = input;

	//parse fields
	rslt = parseFields(rdata, name_value_list, jimo_parser); 
	if (!rslt) return false;

	//parse speed
	attrname = "speed";
	OmnString speed = jimo_parser->getParmStr(rdata, attrname, name_value_list);
	mSpeed = speed;

	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}

bool
AosJimoLogicDataProcCheckPoint::parseFields(
		AosRundata *rdata,
		vector<AosExprObjPtr> &name_value_list,
		AosJimoParserObj *jimo_parser)
{
	vector<AosExprObjPtr> field_names;
	// Fields must be in the following format:
	// 	fields: [(alias:ExprString("f1"), fname:"key_field1", type:"u64", max_length:8),
	//			 (alias:"f1", fname:"key_field1", type:"u64", max_length:8),
	//			 ...]
	//  'alias' is optional,
	//  'fname': identifies the field name. This is mandatory.
	//  'type': is the field's data type. This is mandatory.
	//  'max_length': is the field's max length. It is optional.
	//if (distincts.size() <= 0 || distincts[0u]->getValue(rdata) == "all")
	//{
		bool rslt = jimo_parser->getParmArray(rdata, "fields", name_value_list, field_names);
		if (rslt)
        {
            if(field_names.size() <= 0)
            {
			    setErrMsg(rdata, eGenericError, "fields",mErrmsg);
			    return false;
            }
		}
		JSONValue fieldJSON;
		for (u32 i=0; i<field_names.size(); i++)
		{
			// {"fname":"xxx", "alias":"xxx","alias":"xxx","max_len":"xxx},
			// {"fname":"xxx", "alias":"xxx","alias":"xxx","max_len":"xxx},
			if(!mIsService)
			{
				if (field_names[i]->getType() != AosExprType::eBrackets)
				{
					AosLogError(rdata, true, "incorrect_field_specs") << enderr;
					return false;
				}
			}

			vector<AosExprObjPtr> field_exprs = field_names[i]->getExprList();
			OmnString alias, fname, type, max_length;
			for (u32 k=0; k<field_exprs.size(); k++)
			{
				if (field_exprs[k]->getType() != AosExprType::eNameValue)
				{
					AosLogError(rdata, true, "expr type error") << enderr;
					return false;
				}

				OmnString nn = field_exprs[k]->getName().toLower();
				if (nn == "alias") { alias = field_exprs[k]->getValue(rdata);}
				else if (nn == "fname") 
				{
					fname = field_exprs[k]->getValue(rdata);
					if(i == 0 && k == 0)
					{
						if(mIsService)
						{
							mIndexKey = fname;
						}
					}
				}
				else if (nn == "type") { type = field_exprs[k]->getValue(rdata);}
				else if (nn == "max_length") { max_length = field_exprs[k]->getValue(rdata);}
			}

			JSONValue fieldJV;
			if (fname == "")
			{
				AosLogError(rdata, true, "missing_fname") << enderr;
				return false;
			}
			else
			{
				fieldJV["fname"] = fname.data();
			}
			if (type != "")
			{
				fieldJV["type"] = type.data();
			}

			if (alias == "")  alias = fname;
			fieldJV["alias"] = alias.data();

			if (max_length != "")
			{
				fieldJV["max_length"] = max_length.toInt();
			}
			mFieldsJV.append(fieldJV);
			mFieldsName.push_back(fname);
			mSubFields.push_back(alias);
		}
		mFields = field_names;
	return true;
}

bool
AosJimoLogicDataProcCheckPoint::compileJQL(
			AosRundata *rdata, 
			AosJimoProgObj *prog)
{
	mDataprocCheckPointName ="";
	mOutputName ="";
	aos_assert_rr(prog, rdata, false);

	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcCheckPoint*>(this);
	//felicia 2015/08/07 
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic); 
	aos_assert_rr(rslt, rdata, false);

	OmnString jobName = prog->getJobname();
	mDataprocCheckPointName << "_dp_" << jobName << "_" << mDataProcName;

	if (mOutput == "")
	{
		mOutputName << mDataprocCheckPointName << "_output";
	}
	else
	{
		mOutputName = mOutput;
	}
	mOutputNames.push_back(mOutputName);
	
	return true;
}


bool 
AosJimoLogicDataProcCheckPoint::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	AosJqlStatementPtr stmt = dynamic_cast<AosJqlStatement*>(this);
	OmnString jql = stmt->getOrigStatement();
	bool rslt = createJimoLogicDoc(rdata, mDataProcName, jql);
	aos_assert_rr(rslt, rdata, false);

	// checkpoint dataproc
	rslt = createCheckPointDataproc(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);

	// task for checkpoint
	rslt = addCheckPointTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcCheckPoint::parseRun(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata, jimo_prog, stmt, statements);
	aos_assert_rr(rslt, rdata, false);
	if (statements.size() <= 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< AosFN("Statement") << stmt << enderr;
		return false;
	}

	OmnString ss;
	for(size_t i = 0;i < statements.size();i++)
	{
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, ss, true);
	}
	return true;
}


bool
AosJimoLogicDataProcCheckPoint::getOutputName(
		AosRundata *rdata, 
		AosJimoProgObj *prog, 
		OmnString &outputName)
{
	outputName = mOutputName;
	return true;
}


bool
AosJimoLogicDataProcCheckPoint::createCheckPointDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	OmnString dataprocCheckPoint_str = "create dataproccheckpoint ";
	dataprocCheckPoint_str << mDataprocCheckPointName << " { "
		<< "\"type\":" << "\"checkpoint\"" << ","
		<< "\"record_type\":" << "\"buff\"" << ","
		<< "\"fields\":" << mFieldsJV.toStyledString() << ",";
	dataprocCheckPoint_str << "\"speed\":" << "\"" <<  mSpeed << "\"";
	dataprocCheckPoint_str << "};";

	OmnScreen <<"zzzzzzzzz" << dataprocCheckPoint_str << endl;
	statements_str << "\n" << dataprocCheckPoint_str;

    bool rslt = parseRun(rdata, dataprocCheckPoint_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}

bool
AosJimoLogicDataProcCheckPoint::addCheckPointTask(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString addCheckPoint_str;
	//OmnString outputname;
 	//getOutputName(rdata, jimo_prog, outputname);
	OmnString output;
	OmnString taskName;
	taskName << "task_index_iil_" << mDataProcName;
	//input = mDataprocOutputs.find(mInput[i])->second;
    output << mDataprocCheckPointName << "_output";
	addCheckPoint_str << " {"
		<< "\"name\":" << "\"" << taskName << "\"" 
		<< ",\"model_type\":" << "\"simple\"" 
		<< ",\"dataengine_type\":" << "\"dataengine_scan2\"" 
		<< ",\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataprocCheckPointName << "\""
		<< ",\"inputs\":" << "[\"" << mInput;

	//for(u32 i=0; i<mInputNames.size();i++)
	//{
	//	if(i>0) addCheckPoint_str << ",";
	//	addCheckPoint_str << "\"" << mInputNames[i] << "\"";
	//}
	
	addCheckPoint_str << "\"]";
    addCheckPoint_str << ",\"outputs\":" << "[\"" << output << "\"]"
        << "}]}";
	if(jimo_prog)
		jimo_prog->appendStatement(rdata, "tasks",addCheckPoint_str);
	OmnScreen << addCheckPoint_str << endl;
	return true;
}

AosJimoPtr 
AosJimoLogicDataProcCheckPoint::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcCheckPoint(*this);
}

