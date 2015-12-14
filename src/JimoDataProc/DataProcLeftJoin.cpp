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
// This data proc filters a record.
//
//
// Modification History:
// 08/12/2015 Created by Jackie
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcLeftJoin.h"

#include "API/AosApi.h"
#include "JQLExpr/ExprBinary.h"


////////////////////////////////////////////////////////////////
// Constructors/Destructors/Config
////////////////////////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcLeftJoin_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcLeftJoin(version);
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


AosDataProcLeftJoin::AosDataProcLeftJoin(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcLeftJoin),
mPreEQ(false),
mLeftCounter(0),
mRightCounter(0),
mTotal(0),
mIsRightEnd(false),
mReverse(false)
{
}


AosDataProcLeftJoin::AosDataProcLeftJoin(const AosDataProcLeftJoin &proc)
:
AosJimoDataProc(proc),
mPreEQ(false),
mLeftCounter(0),
mRightCounter(0),
mTotal(0),
mIsRightEnd(false),
mReverse(false)
{
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	}
	mInputRecords = proc.mInputRecords;
}


AosDataProcLeftJoin::~AosDataProcLeftJoin()
{
}


AosJimoPtr
AosDataProcLeftJoin::cloneJimo() const
{
	return OmnNew AosDataProcLeftJoin(*this);
}


AosDataProcObjPtr
AosDataProcLeftJoin::cloneProc() 
{
	return OmnNew AosDataProcLeftJoin(*this);
}


bool
AosDataProcLeftJoin::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc><![CDATA[
	//	{
	//		"type": "leftjoin",
	//		"reverse":false,
	//		"left_condition_fields":["input1.key_field2", "input1.key_field3"],
	//		"right_condition_fields":["input2.key_field5", "input2.key_field6"],
	//		"conditions": "<expr>"
	//	}
	//]]></dataproc>
	try
	{
		aos_assert_r(def, false);
		aos_assert_r(isVersion1(def), false);

		//create output record templates
		OmnString dpname = def->getAttrStr(AOSTAG_NAME, "");
		mName = dpname;
		aos_assert_r(mName != "", false);
		string jsonstr = def->getNodeText();

		JSONReader reader;
		JSONValue jsonObj;
		bool rslt = reader.parse(jsonstr, jsonObj);
		aos_assert_r(rslt, false);

		OmnString type_str = jsonObj["record_type"].asString(AOSRECORDTYPE_FIXBIN);
		AosDataRecordType::E record_type = AosDataRecordType::toEnum(type_str);
		aos_assert_r(record_type != AosDataRecordType::eInvalid, false);

		//bryant 2015/08/15
		JSONValue alias = jsonObj["alias"];
			for (size_t i=0;i<alias.size();i++)
			{
				OmnString aliasField = alias[i].asString();
				mAlias.push_back(aliasField);
			}

		JSONValue left_fields = jsonObj["left_condition_fields"];
		JSONValue right_fields = jsonObj["right_condition_fields"];
		OmnString errmsg;
		if(left_fields.size()<=0 || right_fields.size()<=0 || left_fields.size()!=right_fields.size())
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "Errmsg: " << errmsg
				<< "CondExpr AosParseJQL ERROR:: left and right must exist, left must equal to right " << enderr;
			return false;
		}

		for(size_t i = 0; i < left_fields.size(); i++)
		{
			OmnString field_name = left_fields[i].asString();
			field_name << ";";
			AosExprObjPtr expr = AosParseExpr(field_name, errmsg, rdata.getPtr()); 
			aos_assert_r(expr, false);
			mLeftCondExprs.push_back(expr);
		}

		for(size_t i = 0; i < right_fields.size(); i++)
		{
			OmnString field_name = right_fields[i].asString();
			field_name << ";";
			AosExprObjPtr expr = AosParseExpr(field_name, errmsg, rdata.getPtr()); 
			aos_assert_r(expr, false);
			mRightCondExprs.push_back(expr);
		}

		//condition by bryant
		OmnString cond_str = jsonObj["condition"].asString();
		if (cond_str != "")
		{
			cond_str << ";";
			mCondition = AosParseExpr(cond_str, errmsg, rdata.getPtr());
			if(!mCondition)
			{
				AosSetErrorUser(rdata,"syntax_error")
					<< "CondExpr AosParseJQL ERROR:: "<< "" <<enderr;
				OmnAlarm << cond_str << enderr;
				return false;
			}

		}

		rslt = createOutput(rdata, dpname, record_type);
		aos_assert_r(rslt, false);

		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
}


void
AosDataProcLeftJoin::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool 
AosDataProcLeftJoin::createOutput(
		const AosRundataPtr &rdata,
		const OmnString &dpname,
		const AosDataRecordType::E record_type)
{
	AosDataRecordObjPtr inputrcd = mInputRecords[0];
	aos_assert_r(inputrcd, false); 

	OmnString name;
	name << dpname << "_output";
	mOutputName = name;
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, record_type);

	OmnString fieldName;
	int fieldLen;
	AosDataFieldType::E fieldType;
	int record_size = mInputRecords.size();
	aos_assert_r(record_size == 2, false);
	vector<AosDataFieldObjPtr> l_fields = mInputRecords[0]->getFields();
	vector<AosDataFieldObjPtr> r_fields = mInputRecords[1]->getFields();

	//bryant 2015/8/10
	OmnString l_field;
	OmnString r_field;
	OmnString l_name;
	OmnString r_name;
	vector<OmnString> aliasFields;
	for (size_t i = 0; i < mAlias.size();i++)
	{
		OmnString aliasField = mAlias[i];
		const char * sep = " ";
		const char *sep1 = ".";
		vector<OmnString> entry;
		int num1 = AosSplitStr(aliasField,sep,entry,3);
		if(num1 == 2)
		{
			vector<OmnString> oldfield;
			int num2 = AosSplitStr(entry[0],sep1,oldfield,2);
			if(num2 == 2)
			{
				aliasFields.push_back(oldfield[0]);
				aliasFields.push_back(oldfield[1]);
				aliasFields.push_back(entry[1]);
			}
			else
			{
				OmnString msg = "[ERR] : DataProc Join \"" ;
				msg << entry[0] << "\" is false \"!";
				rdata->setJqlMsg(msg);
				return false;
			}

		}
		else if(num1 == 3)
		{
			vector<OmnString> oldfield;
			int num2 = AosSplitStr(entry[0],sep1,oldfield,2);
			if(num2 == 2)
			{
				aliasFields.push_back(oldfield[0]);
				aliasFields.push_back(oldfield[1]);
				aliasFields.push_back(entry[2]);
			}
			else
			{
				OmnString msg = "[ERR] : DataProc Join \"" ;
				msg << entry[0] << "\" is false \"!";
				rdata->setJqlMsg(msg);
				return false;
			}
		}
		else
		{
			OmnString msg = "[ERR] : DataProc Join \"" ;
			msg << aliasField << "\" is false \"!";
			rdata->setJqlMsg(msg);
			return false;
		}
		
	}
	for (size_t i = 0;i< aliasFields.size();i+=3)
	{
		aliasFields[i].trimSpaces();
		if(aliasFields[i] == "left_input")
		{
			for(size_t j = 0;j < l_fields.size(); j++)
			{
				l_field = l_fields[j]->getName();
				if(l_field == aliasFields[i+1])
				{
					l_name = aliasFields[i+2];
					l_fields[j]->setName(l_name);
				}
			}
		}
		if(aliasFields[i] == "right_input")
		{
			for(size_t k = 0; k < r_fields.size(); k++)
			{
				r_field = r_fields[k]->getName();
				if(r_field == aliasFields[i+1])
				{
					r_name = aliasFields[i+1];
					r_fields[k]->setName(r_name);
				}
			}
		}
	}

	for(size_t i = 0;i < l_fields.size();i++)
	{
		OmnString left_field = l_fields[i]->getName();
		for(size_t j = 0;j < r_fields.size();j++ )
		{
			OmnString right_field = r_fields[j]->getName();
			if(left_field == right_field)
			{
				OmnString msg = "[ERR] : DataProc Join \"" ;
				msg << left_field << "\" is same to  \"" << right_field <<"!";
				rdata->setJqlMsg(msg);
				return false;
			}
		}
	}


	vector<AosDataFieldObjPtr> input_fields = r_fields;
	input_fields.insert(input_fields.begin(), l_fields.begin(), l_fields.end());
	AosExprObjPtr expr;
	
	for (size_t i = 0; i < input_fields.size(); i++)
	{
		fieldName = input_fields[i]->getName();

		//bryant 2015/08/13
		if(aliasFields.size()>0)
		{
			for(size_t j =0;j<aliasFields.size();j+=3)
			{
				OmnString asName = aliasFields[j+2];
				if (asName == fieldName)
				{
					OmnString real_fname = aliasFields[j+1];
					expr = convertToExpr(real_fname, rdata);
					break;
				}
				else
				{
					expr = convertToExpr(fieldName, rdata);
				}
			}

		}
		else
		{
			expr = convertToExpr(fieldName, rdata);
		}

		fieldLen = input_fields[i]->mFieldInfo.field_data_len;
		if (!expr) return false;

		if (i < l_fields.size())
		{
			fieldType = convertToDataFieldType(rdata.getPtr(), expr, mInputRecords[0]);
		}
		else
		{
			fieldType = convertToDataFieldType(rdata.getPtr(), expr, mInputRecords[1]);
		}
		aos_assert_r(fieldType != AosDataFieldType::eInvalid, false);
		resetDataFieldLen(fieldType, fieldLen);

		if(fieldType == AosDataFieldType::eStr)
		{
			fieldLen = 100;
		}
		output->setField(fieldName, fieldType, fieldLen);
		output->setCmpField(fieldName);
	}

	mFieldSize = input_fields.size();
	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mOutputs.push_back(output);
	return true;
}


bool
AosDataProcLeftJoin::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//json:
	//
	//create DataProcLeftJoin dp_stat
	//{
	//}
	//};
	//<jimodataproc >
	//  <dataproc zky_name="dp_stat" jimo_objid="DataProcLeftJoin_jimodoc_v0">
	//    <![CDATA[{key_fields:["k1", "k2", "k3"], time_field:"xxx", measure_fields:["m1", "m2"], cube_id:"xxx"}]]>
	//  </dataproc>
	//
	//  <datasets>
	//    <dataset  zky_type="datacol">
	//      <datacollector zky_type="iil">
	//        <asm type="stradd">
	//          <datarecord type="ctnr" zky_name="dp_stat">
	//            <datarecord type="fixbin" zky_length="17" zky_name="dp_stat">
	//              <datafields>
	//                <datafield type="str" zky_datatooshortplc="cstr" zky_length="25" zky_name="statkey" zky_offset="0"></datafield>
	//                <datafield type="bin_u64" zky_length="8" zky_name="statid" zky_offset="9"></datafield>
	//              </datafields>
	//            </datarecord>
	//          </datarecord>
	//        </asm>
	//      </datacollector>
	//    </dataset>
	//
	//  </datasets>
	//
	//</jimodataproc>
	
	//generate xml configuration string for jimo dataproc
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\">";

	//add json string into the dataproc node text
	dp_str << "<dataproc zky_name=\"" << dpname << "\"";
	dp_str << "jimo_objid=\"dataprocleftjoin_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]>";
	dp_str << "</dataproc>";
	dp_str << "</jimodataproc>";

	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str);
	//aos_assert_r(dp_xml, false);
	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname));
	//return AosCreateDoc(dp_xml->toString(), true, rdata);
	
	prog->saveLogicDoc(rdata, objid, dp_str);	
	return true;
}

int AosDataProcLeftJoin::compare(const OmnString &left, const OmnString &right)
{
	int found = -1;
	string delim ="|SPLIT|";
    size_t delim_len = delim.length();  

	string leftstr(left.data());
	vector<string> left_tokens;  
    size_t pos = 0;  
    size_t len = leftstr.length();  
    while (pos < len)  
    {  
        int find_pos = leftstr.find(delim, pos);  
        if (find_pos < 0)  
        {  
            left_tokens.push_back(leftstr.substr(pos, len - pos));  
            break;  
        }  
        left_tokens.push_back(leftstr.substr(pos, find_pos - pos));  
        pos = find_pos + delim_len;  
    }  

	string rightstr(right.data());
    vector<string> right_tokens;
    pos = 0;  
    len = leftstr.length();  
    while (pos < len)  
    {  
        int find_pos = rightstr.find(delim, pos);  
        if (find_pos < 0)  
        {  
            right_tokens.push_back(rightstr.substr(pos, len - pos));  
            break;  
        }  
        right_tokens.push_back(rightstr.substr(pos, find_pos - pos));  
        pos = find_pos + delim_len;  
    }  

    for (size_t i = 0; i < left_tokens.size(); i++)
    {
		string left_key = left_tokens[i];
		string right_key = right_tokens[i];
		if(left_key<right_key)
		{
			found = -1;
			break;
		}
		else if(left_key==right_key)
		{
			if(i == left_tokens.size()-1)
			{
				found = 0;
			}
			else
			{
				continue;
			}
		}
		else
		{
			found = 1;
			break;
		}
    }	
	return found;
}

AosDataProcStatus::E
AosDataProcLeftJoin::procData(
		AosRundata *rdata,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	AosDataRecordObj *lhs_record = input_records[0];
	AosDataRecordObj *rhs_record = input_records[1];
	AosValueRslt lhs_value, rhs_value;
	vector<AosDataFieldObjPtr> lhs_fields, rhs_fields;
	AosDataProcStatus::E status;

//OmnScreen << "previous left key : " << mPreLeftKey << ", previous right key : " << mPreRightKey << endl;

	if (!lhs_record)
	{
		return AosDataProcStatus::eExit;
	}

	if(!rhs_record && !mIsRightEnd)
	{
		mIsRightEnd =true;
		mLeftCounter++;
		if(!mPreEQ)
		{
			appendRecord(output_records, lhs_record, 0, rdata);
		}
		return AosDataProcStatus::eLT;
	}

	OmnString left_combine, right_combine;
	for(unsigned int i=0; i<mLeftCondExprs.size(); i++)
	{
		AosExprObjPtr left_expr = mLeftCondExprs[i];
		left_expr->getValue(rdata, lhs_record, lhs_value);
		left_combine << lhs_value.getStr() << "|SPLIT|";	
		
	}

	if(mIsRightEnd)
	{
		//append
		appendRecord(output_records, lhs_record, 0, rdata);
		mLeftCounter++;
		return AosDataProcStatus::eLT;
	}

	for(unsigned int i=0; i<mRightCondExprs.size(); i++)
	{
		AosExprObjPtr right_expr = mRightCondExprs[i];
		right_expr->getValue(rdata, rhs_record, rhs_value);
		right_combine << rhs_value.getStr() << "|SPLIT|";	
		
	}

	int found = compare(left_combine, right_combine);
	//compare between left record and right record
	if (found<0)
	{
		if(!mReverse)
		{
			if(!mPreEQ)
			{
				appendRecord(output_records, lhs_record, 0, rdata);
			}
			status = AosDataProcStatus::eLT;
			mLeftCounter++;
		}
		else
		{
			status = AosDataProcStatus::eGT;
			mRightCounter++;
		}
		mPreEQ = false;
	}
	else if (found>0)
	{
		if(!mReverse)
		{
			status = AosDataProcStatus::eGT;
			mRightCounter++;
		}
		else
		{
			if(!mPreEQ)
			{
				appendRecord(output_records, lhs_record, 0, rdata);
			}
			status = AosDataProcStatus::eLT;
			mLeftCounter++;
		}
		mPreEQ = false;
	}
	else
	{
		appendRecord(output_records, lhs_record, rhs_record, rdata);
		status = AosDataProcStatus::eGT;
		mRightCounter++;
		mPreEQ = true;
	}
	mPreLeftKey = left_combine;
	mPreRightKey = right_combine;
	return status;
}


AosDataProcStatus::E
AosDataProcLeftJoin::procData(
		AosRundata *rdata,
		const AosRecordsetObjPtr &lhs_recordset,
		const AosRecordsetObjPtr &rhs_recordset,
		AosDataRecordObj **output_records)
{
	OmnNotImplementedYet;
	return AosDataProcStatus::eExit;
}

bool
AosDataProcLeftJoin::appendRecord(
		AosDataRecordObj **output_records,
		AosDataRecordObj *l_record,
		AosDataRecordObj *r_record,
		AosRundata *rdata)
{

	AosValueRslt value_rslt;
	bool rslt = false, outofmem = false;
	AosDataRecordObj *output_record;
	int l_fields_size = l_record->getFields().size();
	
	vector<AosDataRecordObj* > v_records;
	v_records.push_back(l_record);
	v_records.push_back(r_record);
	//condition by bryant
	if (mCondition)
	{
		AosValueRslt value;
		mCondition->getValue(rdata, v_records, value);
		rslt = value.getBool();
		if (!rslt)
		{
OmnScreen << "filter record, the condtion is : " << mCondition->dump() << endl;
			return true;
		}
	}

	mOutputRecord->clear();
	output_record = mOutputRecord.getPtr();
	aos_assert_r(output_record, AosDataProcStatus::eError);	

	for (int i = 0; i < mFieldSize; i++)
	{
		if (i < l_fields_size)
		{
			rslt = l_record->getFieldValue(i, value_rslt, outofmem, rdata);
		}
		else
		{
			if(!r_record)
			{
				value_rslt.setNull();
			}
			else
			{
				rslt = r_record->getFieldValue(i-l_fields_size, value_rslt, outofmem, rdata);
			}
		}
		if (!rslt) return AosDataProcStatus::eError;

		rslt = output_record->setFieldValue(i, value_rslt, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;
	}

	//mOutputRecord->flushRecord(rdata);
	rslt = flushRecord(output_records, mOutputRecord.getPtr(), rdata);
	aos_assert_r(rslt, false);

	mTotal++;

	return true;
}
	
