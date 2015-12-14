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
#include "JimoDataProc/DataProcMerge.h"

#include "API/AosApi.h"
#include "JQLExpr/ExprBinary.h"


////////////////////////////////////////////////////////////////
// Constructors/Destructors/Config
////////////////////////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcMerge_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcMerge(version);
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


AosDataProcMerge::AosDataProcMerge(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcMerge),
mCondExpr(NULL)
{
}


AosDataProcMerge::AosDataProcMerge(const AosDataProcMerge &proc)
:
AosJimoDataProc(proc)
{
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	}
	mInputRecords = proc.mInputRecords;
	if (proc.mCondExpr)
		mCondExpr = (proc.mCondExpr)->cloneExpr();
}


AosDataProcMerge::~AosDataProcMerge()
{
}


AosJimoPtr
AosDataProcMerge::cloneJimo() const
{
	return OmnNew AosDataProcMerge(*this);
}


AosDataProcObjPtr
AosDataProcMerge::cloneProc() 
{
	return OmnNew AosDataProcMerge(*this);
}


bool
AosDataProcMerge::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc><![CDATA[
	//	{
	//	"type":"merage",
	//	"record_type":"fixbin|buff"
	//	}
	//]]></dataproc>
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


	rslt = createOutput(rdata, dpname, record_type);
	aos_assert_r(rslt, false);

	return true;
}


void
AosDataProcMerge::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool 
AosDataProcMerge::createOutput(
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
	vector<AosDataFieldObjPtr> r_fields = mInputRecords[1]->getFields();
	vector<AosDataFieldObjPtr> input_fields = r_fields;
	input_fields.insert(input_fields.begin(), l_fields.begin(), l_fields.end());
	
	for (size_t i = 0; i < input_fields.size(); i++)
	{
		fieldName = input_fields[i]->getName();
		fieldLen = input_fields[i]->mFieldInfo.field_data_len;
		fieldType = input_fields[i]->getType();
		output->setField(fieldName, fieldType, fieldLen);
		output->setCmpField(fieldName);
	}

	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mOutputs.push_back(output);
	return true;
}


bool
AosDataProcMerge::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//generate xml configuration string for jimo dataproc
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\">";

	//add json string into the dataproc node text
	dp_str << "<dataproc zky_name=\"" << dpname << "\"";
	dp_str << "jimo_objid=\"dataprocmerge_jimodoc_v0\">";
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
AosDataProcMerge::procData(
		AosRundata *rdata,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	AosDataRecordObj *lhs_record = input_records[0];
	AosDataRecordObj *rhs_record = input_records[1];
	AosValueRslt lhs_value, rhs_value;
	vector<AosDataFieldObjPtr> lhs_fields, rhs_fields;

	if (!lhs_record && !rhs_record)
	{
		return AosDataProcStatus::eExit;
	}
	aos_assert_r(lhs_record && rhs_record,  AosDataProcStatus::eError);

	//appendRecord(lhs_record, rdata);

	lhs_fields = lhs_record->getFields();
	rhs_fields = rhs_record->getFields();

	bool rslt = false, outofmem = false;
	AosDataRecordObj *output_record;
	mOutputRecord->clear();
	output_record = mOutputRecord.getPtr();
	for(size_t i = 0; i < lhs_fields.size(); i++)
	{
		rslt = lhs_record->getFieldValue(i, lhs_value, false, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);  

		rslt = output_record->setFieldValue(i, lhs_value, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;
	}

	for(size_t i = 0; i < rhs_fields.size(); i++)
	{
		rslt = rhs_record->getFieldValue(i, rhs_value, false, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);  

		rslt = output_record->setFieldValue(i+lhs_fields.size(), rhs_value, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;
	}

	mOutputRecord->flushRecord(rdata);
	return AosDataProcStatus::eEQ;
}
