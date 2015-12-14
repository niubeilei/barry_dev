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
// 06/16/2015 Created by YanYan 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcMatch.h"

#include "API/AosApi.h"
#include "JQLExpr/ExprBinary.h"


////////////////////////////////////////////////////////////////
// Constructors/Destructors/Config
////////////////////////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcMatch_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcMatch(version);
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

AosDataProcMatch::AosDataProcMatch(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcMatch),
mLeftPrieKey(""),
mRightPrieKey("")
{
}


AosDataProcMatch::AosDataProcMatch(const AosDataProcMatch &proc)
:
AosJimoDataProc(proc),
mLeftPrieKey(""),
mRightPrieKey("")
{
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	}
	mInputRecords = proc.mInputRecords;
}


AosDataProcMatch::~AosDataProcMatch()
{
}


AosJimoPtr
AosDataProcMatch::cloneJimo() const
{
	return OmnNew AosDataProcMatch(*this);
}


AosDataProcObjPtr
AosDataProcMatch::cloneProc() 
{
	return OmnNew AosDataProcMatch(*this);
}


bool
AosDataProcMatch::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc><![CDATA[
	//	{
	//		"type": "match",
	//		"fieldtype":["str","str","double","str"],
	//		"left_fields":["input1.key_field1", "input1.key_field2"...],
	//		"right_fields":["input2.key_field1", "input2.key_field2"...],
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

		OmnString type_str = jsonObj["record_type"].asString(AOSRECORDTYPE_BUFF);
		AosDataRecordType::E record_type = AosDataRecordType::toEnum(type_str);
		aos_assert_r(record_type != AosDataRecordType::eInvalid, false);

		rslt = createOutput(rdata, dpname, record_type);
		aos_assert_r(rslt, false);

		JSONValue left_fields = jsonObj["left_fields"];
		JSONValue right_fields = jsonObj["right_fields"];
		OmnString errmsg;
		if(	left_fields.size()<=0 || right_fields.size()<=0 || left_fields.size()!=right_fields.size() )
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "Errmsg: " << errmsg
				<< "CondExpr AosParseJQL ERROR:: In DataProcMatch, left and right must exist, left must equal to right " << enderr;
			return false;
		}

		mFieldType = jsonObj["fieldtype"];
		if(mFieldType.size() > 0)
		{
			aos_assert_r(left_fields.size() == mFieldType.size(), false);
		}

		for(size_t i = 0; i < left_fields.size(); i++)
		{
			OmnString field_name = left_fields[i].asString();
			field_name << ";";
			AosExprObjPtr expr = AosParseExpr(field_name, errmsg, rdata.getPtr()); 
			aos_assert_r(expr, false);
			mLeftFieldExprs.push_back(expr);
		}

		for(size_t i = 0; i < right_fields.size(); i++)
		{
			OmnString field_name = right_fields[i].asString();
			field_name << ";";
			AosExprObjPtr expr = AosParseExpr(field_name, errmsg, rdata.getPtr()); 
			aos_assert_r(expr, false);
			mRightFieldExprs.push_back(expr);
		}

		AosXmlTagPtr record_conf = mInputRecords[0]->getRecordDoc();
		record_conf->setAttr("with_field_cache", "false");
		mRemainingRecord = AosDataRecordObj::createDataRecordStatic(record_conf, 0, rdata.getPtr() AosMemoryCheckerArgs);
		aos_assert_r(mRemainingRecord, false);
		mBuff = OmnNew AosBuff(100000 AosMemoryCheckerArgs);
		//Jackie JIMODB-596
		mRemainingRecord->clear();
		mRemainingRecord->setMemory(mBuff->data(), mBuff->dataLen());

		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
}


void
AosDataProcMatch::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool 
AosDataProcMatch::createOutput(
		const AosRundataPtr &rdata,
		const OmnString &dpname,
		const AosDataRecordType::E record_type)
{
	AosDataRecordObjPtr inputrcd = mInputRecords[0];
	aos_assert_r(inputrcd, false); 

	OmnString name;
	name << dpname << "_output";
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, record_type);

	OmnString fieldName;
	int fieldLen;
	AosDataFieldType::E fieldType;
	int record_size = mInputRecords.size();
	aos_assert_r(record_size == 2, false);
	vector<AosDataFieldObjPtr> l_fields = mInputRecords[0]->getFields();
	for (size_t i = 0; i < l_fields.size(); i++)
	{
		fieldName = l_fields[i]->getName();
		fieldLen = l_fields[i]->mFieldInfo.field_data_len;
		fieldType = l_fields[i]->getType();
		output->setField(fieldName, fieldType, fieldLen);
		output->setCmpField(fieldName);
	}

	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mOutputs.push_back(output);
	return true;
}


bool
AosDataProcMatch::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//json:
	//
	//create DataProcMatch dp_stat
	//{
	//}
	//};
	//<jimodataproc >
	//  <dataproc zky_name="dp_stat" jimo_objid="DataProcMatch_jimodoc_v0">
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
	dp_str << "jimo_objid=\"dataprocmatch_jimodoc_v0\">";
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


AosDataProcStatus::E
AosDataProcMatch::procData(
		AosRundata *rdata,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	AosDataRecordObj *lhs_record = input_records[0];
	AosDataRecordObj *rhs_record = input_records[1];
	AosValueRslt lhs_value, rhs_value;
	vector<AosDataFieldObjPtr> lhs_fields, rhs_fields;
	OmnString left, right;
	AosDataProcStatus::E status;
	
	//get left key and right key
	if(lhs_record)
	{
		for(unsigned int i=0; i<mLeftFieldExprs.size(); i++)
		{
			AosExprObjPtr left_expr = mLeftFieldExprs[i];
			left_expr->getValue(rdata, lhs_record, lhs_value);
			left << lhs_value.getStr() << "|SPLIT|";	
			
		}
	}

	if(rhs_record)
	{
		for(unsigned int i=0; i<mRightFieldExprs.size(); i++)
		{
			AosExprObjPtr right_expr = mRightFieldExprs[i];
			right_expr->getValue(rdata, rhs_record, rhs_value);
			right << rhs_value.getStr() << "|SPLIT|";	
			
		}
	}

	//1.case
	if (!lhs_record || (lhs_record && !rhs_record) )
	{
		if(left!=mLeftPrieKey && right!=mRightPrieKey && mLeftPrieKey==mRightPrieKey)
		{
			OmnString key;
			if(mLeftPrieKey!="")
			{
				key = mLeftPrieKey;
			}
			if(mRightPrieKey!="")
			{
				key = mRightPrieKey;
			}
			appendRecord(output_records, mRemainingRecord.getPtr(), mLeftPrieKey, rdata);
		}
		return AosDataProcStatus::eExit;
	}
	
	int found = compare(left, right);

	//2.case
	if (found>0)
	{
		if(left!=mLeftPrieKey && right!=mRightPrieKey && mLeftPrieKey!="" && mRightPrieKey!="" && mLeftPrieKey==mRightPrieKey)
		{
			appendRecord(output_records, mRemainingRecord.getPtr(), mLeftPrieKey, rdata);
		}
		status = AosDataProcStatus::eGT;
		mBuff = OmnNew AosBuff(100000 AosMemoryCheckerArgs);
		//Jackie JIMODB-596
		mRemainingRecord->clear();
		mRemainingRecord->setMemory(mBuff->data(), mBuff->dataLen());
	}
	else if (found<0)
	{
		if(left!=mLeftPrieKey && right!=mRightPrieKey && mLeftPrieKey!="" && mRightPrieKey!="" && mLeftPrieKey==mRightPrieKey)
		{
			appendRecord(output_records, mRemainingRecord.getPtr(), mLeftPrieKey, rdata);
		}
		status = AosDataProcStatus::eLT;
		mBuff = OmnNew AosBuff(100000 AosMemoryCheckerArgs);
		//Jackie JIMODB-596
		mRemainingRecord->clear();
		mRemainingRecord->setMemory(mBuff->data(), mBuff->dataLen());
	}
	else
	{
		if(left!=mLeftPrieKey && right!=mRightPrieKey && mLeftPrieKey!="" && mRightPrieKey!="" && mLeftPrieKey==mRightPrieKey)
		{
			appendRecord(output_records, mRemainingRecord.getPtr(), mLeftPrieKey, rdata);
		}
		status = AosDataProcStatus::eEQ;//two side move together
		char * inputrecord_data = lhs_record->getData(rdata);        
		int rcd_len = lhs_record->getRecordLen();
		mBuff->reset();
		mBuff->setBuff(inputrecord_data, rcd_len);
		//Jackie JIMODB-596
		mRemainingRecord->clear();
		mRemainingRecord->setMemory(mBuff->data(), mBuff->dataLen());
	}

	mLeftPrieKey = left;
	mRightPrieKey = right;
	return status;

}

int AosDataProcMatch::compare(const OmnString &left, const OmnString &right)
{
	if(left == "" && right == "")
	{
		return 0;
	}
	else if(left == "")
	{
		return 1;
	}
	else if(right == "")
	{
		return -1;
	}

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
		if(mFieldType.size()>0)
		{
			OmnString type = mFieldType[i].asString();
			char c = type.data()[0];
			switch(c)
			{
				case 'd' :
				{
					double l = atof(left_key.data()); 
					double r = atof(right_key.data()); 
					if(l<r)
					{
						found = -1;
						return found;
					}
					else if(l==r)
					{
						if(i == left_tokens.size()-1)
						{
							found = 0;
						}
					}
					else
					{
						found = 1;
						return found;
					}
				}
				break;

				case 's' :
				default :
				{
					if(left_key<right_key)
					{
						found = -1;
						return found;
					}
					else if(left_key==right_key)
					{
						if(i == left_tokens.size()-1)
						{
							found = 0;
						}
					}
					else
					{
						found = 1;
						return found;
					}
				}
				break;
			}
		}
		else
		{
			if(left_key<right_key)
			{
				found = -1;
				return found;
			}
			else if(left_key==right_key)
			{
				if(i == left_tokens.size()-1)
				{
					found = 0;
				}
			}
			else
			{
				found = 1;
				return found;
			}
		}
    }	
	return found;
}

AosDataProcStatus::E
AosDataProcMatch::procData(
		AosRundata *rdata,
		const AosRecordsetObjPtr &lhs_recordset,
		const AosRecordsetObjPtr &rhs_recordset,
		AosDataRecordObj **output_records)
{
	OmnNotImplementedYet;
	return AosDataProcStatus::eExit;
}

bool
AosDataProcMatch::appendRecord(
		AosDataRecordObj **output_records,
		AosDataRecordObj *record,
		const OmnString &key,
		AosRundata *rdata)
{

	if(key == "" || mOutCurrKey == key)
	{
		return true;
	}
	mOutCurrKey = key;
	
	AosValueRslt value_rslt;
	bool rslt = false, outofmem = false;
	AosDataRecordObj *output_record;
	int fields_size = record->getFields().size();
	
	mOutputRecord->clear();
	output_record = mOutputRecord.getPtr();
	aos_assert_r(output_record, AosDataProcStatus::eError);	

	for (int i = 0; i < fields_size; i++)
	{
		rslt = record->getFieldValue(i, value_rslt, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;

		rslt = output_record->setFieldValue(i, value_rslt, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;
	}

	rslt = flushRecord(output_records, mOutputRecord.getPtr(), rdata);
	aos_assert_r(rslt, false);

	return true;
}
