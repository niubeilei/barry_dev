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
// 04/09/2015 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcJoin.h"

#include "API/AosApi.h"
#include "JQLExpr/ExprBinary.h"


////////////////////////////////////////////////////////////////
// Constructors/Destructors/Config
////////////////////////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcJoin_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcJoin(version);
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


AosDataProcJoin::AosDataProcJoin(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcJoin),
mRawOutputRecord(0),
mRawCondition(0),
mIsLeftMove(false),
mIsRightEnd(false),
mLeftCounter(0),
mRightCounter(0),
mTotal(0),
mJoinFields(0),
mCmpRslt(-1)
{
}


AosDataProcJoin::AosDataProcJoin(const AosDataProcJoin &proc)
:
AosJimoDataProc(proc),
mRawOutputRecord(0),
mRawCondition(0),
mIsLeftMove(false),
mIsRightEnd(false),
mLeftCounter(0),
mRightCounter(0),
mTotal(0),
mJoinFields(0),
mCmpRslt(-1)
{
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
		mRawOutputRecord = mOutputRecord.getPtr();
	}
	mInputRecords = proc.mInputRecords;
	mJoinFields = proc.mJoinFields;
	mCmpRslt = proc.mCmpRslt;

	mName = proc.mName;
}


AosDataProcJoin::~AosDataProcJoin()
{
	mRawCondition = NULL;
}


AosJimoPtr
AosDataProcJoin::cloneJimo() const
{
	return OmnNew AosDataProcJoin(*this);
}


AosDataProcObjPtr
AosDataProcJoin::cloneProc() 
{
	return OmnNew AosDataProcJoin(*this);
}


bool
AosDataProcJoin::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc><![CDATA[
	//	{
	//		"type": "join",
	//		"alias":["input1.f1 as f2","input2.f3 as f4"...],
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

/*		//byrant 2015/08/11
		JSONValue inputAlias = jsonObj["input_alias"];
		if(inputAlias.size() == 2)
		{
			mLeftInputAlias = inputAlias[0u].asString();
			mRightInputAlias = inputAlias[1u].asString();
		}
*/
		//bryant 2015/08/13
		JSONValue alias = jsonObj["alias"];
			for (size_t i=0;i<alias.size();i++)
			{
				OmnString aliasField = alias[i].asString();
				mAlias.push_back(aliasField);
			}

			
	

		JSONValue left_fields = jsonObj["left_condition_fields"];
		JSONValue right_fields = jsonObj["right_condition_fields"];
		// aos_assert_r(left_fields.size()>0, false);
		// aos_assert_r(right_fields.size()>0, false);
		// aos_assert_r(left_fields.size()==right_fields.size(), false);
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
			mRawLeftCondExprs.push_back(expr.getPtr());
		}

		for(size_t i = 0; i < right_fields.size(); i++)
		{
			OmnString field_name = right_fields[i].asString();
			field_name << ";";
			AosExprObjPtr expr = AosParseExpr(field_name, errmsg, rdata.getPtr()); 
			aos_assert_r(expr, false);
			mRightCondExprs.push_back(expr);
			mRawRightCondExprs.push_back(expr.getPtr());
		}

		mJoinFields = right_fields.size();
		OmnString str = "<recordset />";
		AosXmlParser xmlparser;
		AosXmlTagPtr recordset_conf = xmlparser.parse(str, "" AosMemoryCheckerArgs);
		mRightSetHoldor = AosRecordsetObj::createRecordsetStatic(rdata.getPtr(), recordset_conf);
		mRawRightSetHoldor = mRightSetHoldor.getPtr();
		
		mBuff = OmnNew AosBuff(100000 AosMemoryCheckerArgs);
   	 	mRawRightSetHoldor->holdRecord(rdata.getPtr(), mInputRecords[1]);
		mRawRightSetHoldor->setData(rdata.getPtr(), mBuff);
		
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
			mRawCondition = mCondition.getPtr();
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
AosDataProcJoin::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool 
AosDataProcJoin::createOutput(
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
		//bool rslt = aliasFields[i].trimSpaces();
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

		if (!expr) return false;
		int fieldLen = input_fields[i]->mFieldInfo.field_data_len;

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
		//output->setCmpField(fieldName);
	}

	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mRawOutputRecord = mOutputRecord.getPtr();
	mOutputs.push_back(output);
	return true;
}


bool
AosDataProcJoin::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//json:
	//
	//create DataProcJoin dp_stat
	//{
	//}
	//};
	//<jimodataproc >
	//  <dataproc zky_name="dp_stat" jimo_objid="DataProcJoin_jimodoc_v0">
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
	dp_str << "jimo_objid=\"dataprocjoin_jimodoc_v0\">";
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

int 
AosDataProcJoin::compare(
		vector<AosValueRslt> &lhs,
		vector<AosValueRslt> &rhs)
{
	if (rhs.size() == 0) return 1;
	int found = -1;
	for (u32 i = 0; i < mJoinFields; i++)
	{
		if (lhs[i].isNull()) return -1;
		if (rhs[i].isNull()) return 1;

		AosDataType::E type = AosDataType::autoTypeConvert(lhs[i].getType(), rhs[i].getType());
		if (lhs[i] < rhs[i])
			found = -1;
		else if (AosValueRslt::doComparison(eAosOpr_eq, type, lhs[i], rhs[i]))
			found = 0;
		else 
			found = 1;
		if (found)
		{
			return found;
		}
    }	
	return found;
}


AosDataProcStatus::E
AosDataProcJoin::procData(
		AosRundata *rdata,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;
	AosDataRecordObj *lhs_record = input_records[0];
	AosDataRecordObj *rhs_record = input_records[1];
	AosValueRslt lhs_value, rhs_value;
	AosDataProcStatus::E status;

	int rslt;
	if (!lhs_record || (!rhs_record && mCmpRslt < 0))
	{
		return AosDataProcStatus::eExit;
	}

	if(!rhs_record && !mIsRightEnd)
	{
		mIsRightEnd =true;
		mIsLeftMove = true;	
		mLeftCounter++;
		return AosDataProcStatus::eLT;
	}

	// 'lhs_record' is not null. 
	vector<AosValueRslt> left_values;
	for(unsigned int i=0; i<mRawLeftCondExprs.size(); i++)
	{
		mRawLeftCondExprs[i]->getValue(rdata, lhs_record, mValue);
		left_values.push_back(mValue);
	}

	if(mIsRightEnd)
	{
		rslt = compare(left_values, mRightValues);
		if(rslt == 0)
		{
			mRawRightSetHoldor->resetReadIdx();
			AosDataRecordObj *holdrecord;
			for(unsigned int i=0; i<mRawRightSetHoldor->size(); i++)
			{
				mRawRightSetHoldor->nextRecord(rdata, holdrecord);
				appendRecord(output_records, lhs_record, holdrecord, rdata);
	//OmnScreen << "append 1 m*n record, left : " << left_combine << ", right : " << right_combine << endl;
			}
		}
		else if(rslt == -1)
		{
			return AosDataProcStatus::eExit;
		}
		mIsLeftMove = true;	
		mLeftCounter++;
		return AosDataProcStatus::eLT;
	}

	vector<AosValueRslt> right_values;
	for(unsigned int i=0; i<mRawRightCondExprs.size(); i++)
	{
		mRawRightCondExprs[i]->getValue(rdata, rhs_record, mValue);
		right_values.push_back(mValue);
	}

	//append right holdor to output, otherwise clean right holdor
	rslt = compare(left_values, mLeftValues); 
	if(mIsLeftMove && rslt == 0)
	{
		mRawRightSetHoldor->resetReadIdx();
		for(unsigned int i=0; i<mRawRightSetHoldor->size(); i++)
		{
			AosDataRecordObj *holdrecord;
			mRawRightSetHoldor->nextRecord(rdata, holdrecord);
			appendRecord(output_records, lhs_record, holdrecord, rdata);
//OmnScreen << "append 2 m*n record, left : " << left_combine << ", right : " << right_combine << endl;
		}
	}

	if(mIsLeftMove && rslt)	
	{
		mRightSetHoldor->reset();
		mBuff = OmnNew AosBuff(100000 AosMemoryCheckerArgs);
		mRightSetHoldor->setData(rdata, mBuff);
	}

	mCmpRslt = compare(left_values, right_values);
	//compare between left record and right record
	if (mCmpRslt <0)
	{
		mIsLeftMove = true;	
		status = AosDataProcStatus::eLT;
		mLeftCounter++;
	}
	else if (mCmpRslt > 0)
	{
		mIsLeftMove = false;	
		status = AosDataProcStatus::eGT;
		mRightCounter++;
	}
	else
	{
		mIsLeftMove = false;	
		char * inputrecord_data = rhs_record->getData(rdata);        
		int rcd_len = rhs_record->getRecordLen();
		int idx = mBuff->getCrtIdx();
		mBuff->setBuff(inputrecord_data, rcd_len);
		mRightSetHoldor->appendRecord(rdata, 0, idx, rcd_len, 0);

		appendRecord(output_records, lhs_record, rhs_record, rdata);
//OmnScreen << "append m*n record, the key is : " << left_combine << endl;
		status = AosDataProcStatus::eGT;
		mRightCounter++;
	}
	mLeftValues = left_values;
	mRightValues = right_values;
	return status;
}


AosDataProcStatus::E
AosDataProcJoin::procData(
		AosRundata *rdata,
		const AosRecordsetObjPtr &lhs_recordset,
		const AosRecordsetObjPtr &rhs_recordset,
		AosDataRecordObj **output_records)
{
	OmnNotImplementedYet;
	return AosDataProcStatus::eExit;
}

bool
AosDataProcJoin::appendRecord(
		AosDataRecordObj **output_records,
		AosDataRecordObj *l_record,
		AosDataRecordObj *r_record,
		AosRundata *rdata)
{

	bool rslt = false, outofmem = false;
	int l_fields_size = l_record->getFields().size();
	int r_fields_size = r_record->getFields().size();
	int total_fields_size = l_fields_size + r_fields_size;
	
	vector<AosDataRecordObj* > v_records;
	v_records.push_back(l_record);
	v_records.push_back(r_record);
	//condition by bryant
	if (mRawCondition)
	{
		mRawCondition->getValue(rdata, v_records, mValue);
		rslt = mValue.getBool();
		if (!rslt)
		{
			return true;
		}
	}

	mRawOutputRecord->clear();
	for (int i = 0; i < total_fields_size; i++)
	{
		if (i < l_fields_size)
		{
			rslt = l_record->getFieldValue(i, mValue, outofmem, rdata);
		}
		else
		{
			rslt = r_record->getFieldValue(i-l_fields_size, mValue, outofmem, rdata);
		}
		if (!rslt) return AosDataProcStatus::eError;

		rslt = mRawOutputRecord->setFieldValue(i, mValue, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;
	}

	rslt = flushRecord(output_records, mOutputRecord.getPtr(), rdata);
	aos_assert_r(rslt, false);

	mTotal++;

	return true;
}
	
