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
// 09/21/2015 Created by Bryant 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcCheckPoint.h"

#include "API/AosApi.h"
#include "JQLExpr/ExprBinary.h"


////////////////////////////////////////////////////////////////
// Constructors/Destructors/Config
////////////////////////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcCheckPoint_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcCheckPoint(version);
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

AosDataProcCheckPoint::AosDataProcCheckPoint(const int ver)
:
AosJimoDataProc(ver,AosJimoType::eDataProcJoin)
{
}

AosDataProcCheckPoint::AosDataProcCheckPoint(const AosDataProcCheckPoint &proc)
:
AosJimoDataProc(proc)
{
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	}

	mData = OmnNew char[1000];
	mSpeed = proc.mSpeed;
	mFields = proc.mFields;
	mRecords = proc.mRecords;
	mInputRecords = proc.mInputRecords;
	mSetHoldor = proc.mSetHoldor;
	mIndex = proc.mIndex;
}

AosDataProcCheckPoint::~AosDataProcCheckPoint()
{
	OmnDelete [] mData;
	mData = 0;
}


AosJimoPtr
AosDataProcCheckPoint::cloneJimo() const
{
	return OmnNew AosDataProcCheckPoint(*this);
}


AosDataProcObjPtr
AosDataProcCheckPoint::cloneProc() 
{
	return OmnNew AosDataProcCheckPoint(*this);
}


bool
AosDataProcCheckPoint::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc><![CDATA[
	//	{
	//		"type": "checkpoint",
	//		"fields": [(alias:ExprString("f1"), fname:"key_field1", type:"u64", max_length:8),  
	//		          (alias:"f1", fname:"key_field1", type:"u64", max_length:8),
	//		           ...],
	//		"speed": 100
	//	}
	//]]></dataproc>
	try
	{
		aos_assert_r(def, false);
		aos_assert_r(isVersion1(def), false);

		//create output record templates
		OmnString dpname = def->getAttrStr(AOSTAG_NAME, "");
		string jsonstr = def->getNodeText();

		JSONReader reader;
		JSONValue jsonObj;
		bool rslt = reader.parse(jsonstr, jsonObj);
		aos_assert_r(rslt, false);

		OmnString type_str = jsonObj["record_type"].asString(AOSRECORDTYPE_FIXBIN);
		AosDataRecordType::E record_type = AosDataRecordType::toEnum(type_str);
		aos_assert_r(record_type != AosDataRecordType::eInvalid,false);

		//fields
		rslt = configFields(jsonObj,rdata);
		aos_assert_r(rslt,false);

		//speed	
		OmnString speed_str = jsonObj["speed"].asString();
		if (speed_str != "")
		{
			mSpeed = speed_str.toDouble(); 
		}

		OmnString str = "<recordset />";
		AosXmlParser xmlparser;
		AosXmlTagPtr recordset_conf = xmlparser.parse(str,"" AosMemoryCheckerArgs);
		mSetHoldor = AosRecordsetObj::createRecordsetStatic(rdata.getPtr(),recordset_conf);

		mBuff = OmnNew AosBuff(10000 AosMemoryCheckerArgs);
		mSetHoldor->holdRecord(rdata.getPtr(),mInputRecords[0]);
		mSetHoldor->setData(rdata.getPtr(), mBuff);

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

bool
AosDataProcCheckPoint::configFields(
			const JSONValue &json,
			const AosRundataPtr &rdata)
{
	// Fields are specified in the form:
	// 	fields: 
	// 	[
	// 		{fname:xxx, type:xxx, max_length:xxx, alias:xxx},
	// 		{fname:xxx, type:xxx, max_length:xxx, alias:xxx},
	// 		...
	// 		{fname:xxx, type:xxx, max_length:xxx, alias:xxx}
	// 	]
	// This function will construct mCheckPointNameMap. Its key
	// is the alias or fname (if alias is not specified).
	// The key must be unique. Otherwise, it is an error.
	JSONValue fields = json["fields"];
//map<OmnString, int>::iterator itr_selectName;
//map<OmnString, AosDataFieldObjPtr>::iterator itr;
	for(size_t i = 0; i < fields.size(); i++)
	{
//bool isAgrFunc = false;
//OmnString funcName = "";

		//fname(value)
		OmnString value_str = fields[i]["fname"].asString();
		aos_assert_r(value_str != "", false);

		//alias
		OmnString name = fields[i]["alias"].asString();
		if (name == "") name = value_str;
		aos_assert_r(name != "", false);

		//check duplicate field name
//		itr_selectName = mCheckPointNameMap.find(name);
//		if (itr_selectName != mCheckPointNameMap.end())
//		{
//			AosLogError(rdata, true, "already_have_the_field")
//				<< AosFN("Field Name") << name << enderr;
//			return false;
//		}
//		mCheckPointNameMap.insert(make_pair(name, 1));
		
		//convert value to expr
		AosExprObjPtr expr = convertToExpr(name, rdata);
		if (!expr) return false;
		AosExprObjPtr value_expr = expr;
//		if(AosExpr::checkIsAgrFunc(expr))
//		{
			// It is an aggregation function
//			isAgrFunc = true;
//			AosExprGenFunc* funcExpr = dynamic_cast<AosExprGenFunc*>(expr.getPtr());       
//			aos_assert_r(funcExpr, false);
//			aos_assert_r(funcExpr->getParmList()->size() > 0, false);                      

//			value_expr = (*(funcExpr->getParmList()))[0];                     
//			aos_assert_r(value_expr, false);
			
//			funcName = funcExpr->getFuctName();
//			aos_assert_r(funcName != "", false);
//		}

		//get len 
//		AosDataFieldObjPtr field;
//		int len = fields[i]["max_length"].asInt();
//		if (len == 0)
//		{
//			len = 50;
//			itr = mInputRecordsMap.find(value_expr->dumpByNoEscape());
//			if (itr != mInputRecordsMap.end())
//			{
//				field = itr->second;
//				aos_assert_r(field, false);
//				len = field->getFieldLen();
//			}
//			else len = -1;
//		}

		//get type
//		AosDataFieldType::E type;
//		OmnString str_type = fields[i]["type"].asString();
//		if (str_type != "")
//		{
//			type = convertToDataFieldType(rdata.getPtr(), str_type);
//			aos_assert_r(type != AosDataFieldType::eInvalid, false);
//		}
//		else
//		{
//			type = convertToDataFieldType(rdata.getPtr(), value_expr, mInputRecords[0]);
//			aos_assert_r(type != AosDataFieldType::eInvalid, false);
//		}
		
//		resetDataFieldLen(type, len);

		mFields.push_back(value_expr);
	}
	return true;
}


void
AosDataProcCheckPoint::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool
AosDataProcCheckPoint::createInputRecordsMap()
{
	aos_assert_r(mInputRecords.size() > 0, false);

	AosDataRecordObjPtr inputrcd = mInputRecords[0];
	aos_assert_r(inputrcd,false);

	vector<AosDataFieldObjPtr> input_fields = inputrcd->getFields();
	for(size_t i = 0; i < input_fields.size(); i++)
	{
		OmnString name = input_fields[i]->getName();
		mInputRecordsMap.insert(make_pair(name,input_fields[i]));
	}
	return true;
}


bool 
AosDataProcCheckPoint::createOutput(
		const AosRundataPtr &rdata,
		const OmnString &dpname,
		const AosDataRecordType::E record_type)
{
	AosDataRecordObjPtr inputrcd = mInputRecords[0];
	aos_assert_r(inputrcd,false);
	
	OmnString name;
	name << dpname << "_output";
	mOutputName = name;
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name,record_type);
	OmnString str_type = "str";
	AosDataFieldType::E	type = convertToDataFieldType(rdata.getPtr(),str_type);
	int fieldLen = 50;
	output->setField("KKID1",type,fieldLen);
	output->setField("KKID2",type,fieldLen);
	output->setField("distance",AosDataFieldType::eBinDouble,sizeof(double));
	output->setField("short_time",AosDataFieldType::eBinDouble,sizeof(double));
	output->setField("speed",AosDataFieldType::eBinDouble,sizeof(double));
	output->init(mTask,rdata);
	mOutputRecord = output->getRecord();
	mOutputs.push_back(output);
	return true;
}


bool
AosDataProcCheckPoint::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//json:
	//
	//create DataProcCheckPoint dp_stat
	//{
	//}
	//};
	//<jimodataproc >
	//</jimodataproc>
	
	//generate xml configuration string for jimo dataproc
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\">";

	//add json string into the dataproc node text
	dp_str << "<dataproc zky_name=\"" << dpname << "\" ";
	dp_str << "jimo_objid=\"dataproccheckpoint_jimodoc_v0\">";
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
AosDataProcCheckPoint::procData(
		AosRundata *rdata,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	//1.check vector size 
	//2.proc data
	//3.append record to vector
	//4.append record to output
	mProcDataCount++;

	//input record
	AosDataRecordObj *input_record = input_records[0];
	aos_assert_r(input_record,AosDataProcStatus::eError);

	int len = 0;
	char *data = 0;
	//AosDataRecordObjPtr dataRecord = input_record->clone(0 AosMemoryCheckerArgs);
	data = input_record->getData(rdata);
	len = input_record->getMemoryLen();
	if (mProcDataCount == 1)
	{
		mBuff = OmnNew AosBuff(10000 AosMemoryCheckerArgs);
		mSetHoldor->setData(rdata,mBuff);
		int idx = mBuff->getCrtIdx();
		mIndex = idx;
		mBuff->setBuff(data,len);
		mSetHoldor->appendRecord(rdata,0,mIndex,len,0);
		mIndex += len;
		//memcpy(mData,data,len);
		//char * b_data = buff->data();
		//int b_len = buff->dataLen();
		//dataRecord->setMemory(b_data,b_len);
		//mRecords.push_back(dataRecord);
		return AosDataProcStatus::eContinue;
	}
	else
	{
		//create output record
		mOutputRecord->clear();
		AosDataRecordObj *output_record = mOutputRecord.getPtr();
		aos_assert_r(output_record,AosDataProcStatus::eError);
		
		AosValueRslt valuerslt2;
		OmnString name2;
		double R2x,R2y;
		bool rslt;
		for (size_t i = 0; i < mFields.size(); i++)
		{
			rslt = mFields[i]->getValue(rdata, input_record, valuerslt2);
			aos_assert_r(rslt, AosDataProcStatus::eError);
			if (i == 0)
			{
				//name2 = valuerslt2.getValueStr1();
				name2 = valuerslt2.getStr();
			}
			else if (i == 1)
			{
				//R2x = valuerslt2.getDoubleValue(rdata);
				R2x = valuerslt2.getDouble();
			}
			else
			{
				//R2y = valuerslt2.getDoubleValue(rdata);
				R2y = valuerslt2.getDouble();
			}
		}
		//if fields size is not 4,will be error.
		mSetHoldor->resetReadIdx();
		for (unsigned int i = 0; i < mSetHoldor->size();i++)
		{
			AosValueRslt valuerslt1;
			OmnString name1;
			double R1x,R1y;
			AosDataRecordObj *record;
			mSetHoldor->nextRecord(rdata,record);
			for (size_t j = 0; j < mFields.size();j++)
			{
				rslt = mFields[j]->getValue(rdata, record, valuerslt1);
				aos_assert_r(rslt, AosDataProcStatus::eError);
				if(j==0)
				{
					name1 = valuerslt1.getStr();
				}
				else if(j==1)
				{
					R1x = valuerslt1.getDouble();
				}
				else
				{
					R1y = valuerslt1.getDouble();
				}
			}

			AosValueRslt v01(name1);
			bool outofmen = false;
			output_record->setFieldValue(0,v01,outofmen,rdata);


			AosValueRslt v02(name2);
			output_record->setFieldValue(1,v02,outofmen,rdata);

			//calculate distance
			double radLat1 = R1x * PI / 180.0;
			double radLat2 = R2x * PI / 180.0;
			double radLng1 = R1y * PI / 180.0;
			double radLng2 = R2y * PI / 180.0;
			double a = radLat1 - radLat2;
			double b = radLng1 - radLng2;
			
			double dst = 2 * asin((sqrt(pow(sin(a / 2),2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2),2))));

			dst = dst * EARTH_RADIUS;
			dst = round(dst * 10000) / 10000;

			AosValueRslt v1(dst);
			output_record->setFieldValue(2,v1,outofmen,rdata);
			//calculate smallTime
			double smallTime = dst / mSpeed;
			
			AosValueRslt v2(smallTime);
			output_record->setFieldValue(3,v2,outofmen,rdata);
			//speed
			AosValueRslt v3(mSpeed);
			output_record->setFieldValue(4,v3,outofmen,rdata);

			mOutputRecord->flushRecord(rdata);
			output_record->clear();
		}

	//	mBuff = OmnNew AosBuff(2000);
	//	mSetHoldor->setData(rdata,mBuff);
		mBuff->setBuff(data,len);
		mSetHoldor->appendRecord(rdata,0,mIndex,len,0);
		mIndex += len;
		//char * b_data = buff->data();
		//int b_len = buff->dataLen();
		//memcpy(mData,data,len);
		//dataRecord->setMemory(b_data,b_len);
		//mRecords.push_back(dataRecord);
		//char * data = output_record->getData();
		//i64 len = output_record->getRecordLen();
		//file.append(data,len);
		return AosDataProcStatus::eContinue;
	}
}


AosDataProcStatus::E
AosDataProcCheckPoint::procData(
		AosRundata *rdata,
		const AosRecordsetObjPtr &lhs_recordset,
		const AosRecordsetObjPtr &rhs_recordset,
		AosDataRecordObj **output_records)
{
	OmnNotImplementedYet;
	return AosDataProcStatus::eExit;
}
