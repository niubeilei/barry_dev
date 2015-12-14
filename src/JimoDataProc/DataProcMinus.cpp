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
// 10/03/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcMinus.h"

#include "API/AosApi.h"


////////////////////////////////////////////////////////////////
// Constructors/Destructors/Config
////////////////////////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcMinus_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcMinus(version);
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


AosDataProcMinus::AosDataProcMinus(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcMinus)
{
}


AosDataProcMinus::AosDataProcMinus(const AosDataProcMinus &proc)
:
AosJimoDataProc(proc)
{
	if (proc.mOutputRecord)
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	mInputRecords = proc.mInputRecords;
}


AosDataProcMinus::~AosDataProcMinus()
{
}


AosJimoPtr
AosDataProcMinus::cloneJimo() const
{
	return OmnNew AosDataProcMinus(*this);
}


AosDataProcObjPtr
AosDataProcMinus::cloneProc() 
{
	return OmnNew AosDataProcMinus(*this);
}


bool
AosDataProcMinus::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc><![CDATA[
	//	{
	//		"type": "minus",
	//		"record_type":"fixbin|buff"
	//	}
	//]]></dataproc>
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);
	OmnString dp_jsonstr = def->getNodeText();
	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_jsonstr, json);
	aos_assert_r(rslt, false);

	//create output record templates
	mName = def->getAttrStr(AOSTAG_NAME, "");
	aos_assert_r(mName != "", false);
	rslt = createOutput(mName, json, rdata);
	aos_assert_r(rslt, false);

	return true;
}


	void
AosDataProcMinus::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


	bool 
AosDataProcMinus::createOutput(
		const OmnString &dpname,
		const JSONValue &json,
		const AosRundataPtr &rdata)
{
	AosDataRecordObjPtr inputrcd = mInputRecords[0];
	aos_assert_r(inputrcd, false); 

	OmnString name;
	name << dpname << "_output";

	OmnString type_str = json["record_type"].asString();
	if (type_str == "")
	{
		type_str = AOSRECORDTYPE_FIXBIN;
	}
	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	//AosDataRecordType::E type = inputrcd->getType();
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);

	OmnString fieldName;
	int fieldLen;
	AosDataFieldType::E fieldType;
	vector<OmnString> fields;
	vector<AosDataFieldObjPtr> input_fields = inputrcd->getFields();
	for (size_t i = 0; i < input_fields.size(); i++)
	{
		fieldName = input_fields[i]->getName();
		fieldLen = input_fields[i]->mFieldInfo.field_data_len;
		fieldType = input_fields[i]->getType();
		output->setField(fieldName, fieldType, fieldLen);
		fields.push_back(fieldName);
	}
	for (size_t i=0; i < fields.size(); i++)
	{
		output->setCmpField(fields[i]);
	}

	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mOutputs.push_back(output);
	return true;
}


bool
AosDataProcMinus::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//json:
	//
	//create DataProcMinus dp_stat
	//{
	//}
	//};
	//<jimodataproc >
	//  <dataproc zky_name="dp_stat" jimo_objid="DataProcMinus_jimodoc_v0">
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
	dp_str << "jimo_objid=\"dataprocminus_jimodoc_v0\">";
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
AosDataProcMinus::procData(
		AosRundata *rdata,
		const AosRecordsetObjPtr &lhs_recordset,
		const AosRecordsetObjPtr &rhs_recordset,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;

	bool rslt = false;
	AosDataRecordObj *lhs_record = NULL;
	AosDataRecordObj *rhs_record = NULL;
	AosValueRslt lhs_value, rhs_value;
	vector<AosDataFieldObjPtr> lhs_fields, rhs_fields;

	rslt = lhs_recordset->nextRecord(rdata, lhs_record);
	rslt = rhs_recordset->nextRecord(rdata, rhs_record);

	aos_assert_r(lhs_record || rhs_record,  AosDataProcStatus::eError);

	if (!rhs_record)
	{
		appendRecord(lhs_record, rdata);
		return AosDataProcStatus::eLT;
	}

	if (!lhs_record)
	{
		return AosDataProcStatus::eExit;
	}

	lhs_fields = lhs_record->getFields();
	rhs_fields = rhs_record->getFields();
	aos_assert_r(lhs_fields.size() == rhs_fields.size(), AosDataProcStatus::eError);

	for(size_t i = 0; i < lhs_fields.size(); i++)
	{
		rslt = lhs_record->getFieldValue(i, lhs_value, false, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);  

		rslt = rhs_record->getFieldValue(i, rhs_value, false, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);  

		if (lhs_value.isNull()) return AosDataProcStatus::eLT; 
		if (rhs_value.isNull()) return AosDataProcStatus::eGT; 

		if (lhs_value < rhs_value) 
		{
			appendRecord(lhs_record, rdata);
			return AosDataProcStatus::eLT;
		}

		// by andy 
		AosDataType::E type = AosDataType::autoTypeConvert(lhs_value.getType(), rhs_value.getType());
		if (AosValueRslt::doComparison(eAosOpr_gt, type, lhs_value, rhs_value))
		{
			return AosDataProcStatus::eGT;
		}
	}
	return AosDataProcStatus::eEQ;
}


bool
AosDataProcMinus::appendRecord(
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	AosValueRslt value_rslt;
	bool rslt = false, outofmem = false;
	AosDataRecordObj *output_record;
	vector<AosDataFieldObjPtr> fields = record->getFields();
	mOutputRecord->clear();
	output_record = mOutputRecord.getPtr();
	aos_assert_r(output_record, AosDataProcStatus::eError);	

	for (size_t i = 0; i < fields.size(); i++)
	{
		rslt = record->getFieldValue(i, value_rslt, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;

		rslt = output_record->setFieldValue(i, value_rslt, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;
	}

	mOutputRecord->flushRecord(rdata);
	mOutputCount++;

	return true;
}


bool
AosDataProcMinus::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	i64 procDataCount = 0;
	i64 procOutputCount = 0;
	for (size_t i = 0; i < procs.size(); i++)
	{
		procDataCount += procs[i]->getProcDataCount();
		procOutputCount += procs[i]->getProcOutputCount();
	}
	OmnScreen << "DataProcMinus" << "(" << mName << ")" << " finished:" 
				<< ", procDataCount:" << procDataCount
				<< ", OutputCount:" << procOutputCount << endl;
	return true;
}
