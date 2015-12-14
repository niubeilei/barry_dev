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
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////

#include "AosConf/Output.h"

#include "AosConf/DataField.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordBuff.h"
#include "AosConf/DataRecordCSV.h"
#include "XmlUtil/SeXmlParser.h"
#include "Job/JobMgr.h"
#include "Util/CompareFun.h"
#include <boost/make_shared.hpp> 


AosConf::Output::Output(const string &name, const AosDataRecordType::E type)
:
mName(name),
mRecordType(type),
mDataAssembler(boost::make_shared<AosConf::DataAssembler>()),
mDataCollectorType("")
{
	mDataAssembler->setAttribute("zky_name", mName);
	//mDataAssembler->setAttribute("zky_type", "iil");
	boost::shared_ptr<AosConf::DataRecordCtnr> recordCtnr = boost::make_shared<AosConf::DataRecordCtnr>();
	recordCtnr->setAttribute("zky_name", name);
	boost::shared_ptr<DataRecord> dr;
	string type_str = AosDataRecordType::toStr(mRecordType).data();
	if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		dr = boost::make_shared<AosConf::DataRecordFixbin>();
	}
	else if (mRecordType == AosDataRecordType::eBuff)
	{
		dr = boost::make_shared<AosConf::DataRecordBuff>();
		mCmpFun.record_type = "buff"; 
	}
	else if (mRecordType == AosDataRecordType::eCSV)
	{
		dr = boost::make_shared<AosConf::DataRecordCSV>();
	}

	//mDataColAttrs.insert(std::make_pair<string, string>("zky_type", "iil"));
	mDataColAttrs.insert(std::make_pair("type", "reduce"));
	mDataColAttrs.insert(std::make_pair("zky_name", mName));
	mDataColAttrs.insert(std::make_pair("record_type", type_str));
	dr->setAttribute("type", type_str);
	dr->setAttribute("zky_name", mName);
	recordCtnr->setRecord(dr);
	mDataRecord = recordCtnr;
	mDataAssembler->setDataRecord(mDataRecord);
};

void
AosConf::Output::setRecordConfig(const AosXmlTagPtr &xml)
{
	mDataRecord->setConfig(xml);
}


void 
AosConf::Output::setField(
		const string &name, 
		const AosDataFieldType::E &field_type, 
		const int length)
{
	string type = AosDataFieldType::toString(field_type).data();
	mDataRecord->setField(name, type, length);
}


void  
AosConf::Output::setDataColAttr(
		const string &name,
		const string &value)
{
	mDataColAttrs[name] = value;
}


void
AosConf::Output::setAssemblerAttr(
		const string &name,
		const string &value)
{
	mDataAssembler->setAttribute(name, value);
}

void
AosConf::Output::setCmpFun(
		const string &fun_type,
		const string &cmpfun_reserve)
{
	mCmpFun.type = fun_type;
	mCmpFun.reserve = cmpfun_reserve;
}

string
AosConf::Output::convert(const string &type)
{
	AosDataFieldType::E field_type = AosDataFieldType::toEnum(type);
	AosCompareFun::DataType data_type;
	if (mRecordType == AosDataRecordType::eBuff)
	{
		switch(field_type)
		{
			case AosDataFieldType::eBinU64:
			case AosDataFieldType::eU64:
				data_type = AosCompareFun::eU64;
				break;
			case AosDataFieldType::eBinDateTime:
			case AosDataFieldType::eBinInt64:
			case AosDataFieldType::eInt64:
				data_type = AosCompareFun::eI64;
				break;
			case AosDataFieldType::eStr:
			case AosDataFieldType::eBinChar:
				data_type = AosCompareFun::eBuffStr;
				break;
			case AosDataFieldType::eBinDouble:
				data_type = AosCompareFun::eDouble;
				break;
			default:
				OmnAlarm << "not handle this data type: " << type << enderr;
				return "";
		}
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		switch(field_type)
		{
			case AosDataFieldType::eBinU64:
			case AosDataFieldType::eU64:
				data_type = AosCompareFun::eU64;
				break;
			case AosDataFieldType::eBinDouble:
				data_type = AosCompareFun::eDouble;
				break;
			case AosDataFieldType::eBinDateTime:
			case AosDataFieldType::eBinInt64:
			case AosDataFieldType::eInt64:
				data_type = AosCompareFun::eI64;
				break;
			case AosDataFieldType::eStr:
			case AosDataFieldType::eBinChar:
				data_type = AosCompareFun::eStr;
				break;
			default:
				OmnAlarm << "not handle this data type: " << type << enderr;
				return "";
		}

	}
	return AosCompareFun::getDataTypeStr(data_type).data();
}

void
AosConf::Output::setCmpField(
		const string &name,
		const string &cmp_reserve)
{
	CmpField field = {name, cmp_reserve};
	mCmpFields.push_back(field);
}


void
AosConf::Output::setAggrField(
		const string &name,
		const string &aggrtype)
{
	AggrField field = {name, aggrtype};
	mAggrFields.push_back(field);
}


void 
AosConf::Output::setShuffAttr(
			const string &name, 
			const string &value)
{
	mShuffAttrs[name] = value;
	mDataAssembler->setAttribute(name, value);
}


string	
AosConf::Output::getConfig()
{
	if (mCmpFields.empty())
	{
		mDataCollectorType = "file";
		mDataAssembler->setUseCmp(false);
		mDataAssembler->setAttribute("type", "file");
	}
	else
	{
		mDataCollectorType = "iil";
		mDataAssembler->setUseCmp(true);
		if (mDataAssembler->getAttribute("type") == "")
			mDataAssembler->setAttribute("type", "sort");
	}

	mDataColAttrs.insert(std::make_pair("zky_type", mDataCollectorType));

	string config = "<dataset zky_type=\"datacol\" zky_name=\"" + mName + "\">" +
	                "<datacollector ";
	map<string, string>::iterator itr;

	for (itr = mDataColAttrs.begin(); itr != mDataColAttrs.end(); itr++)
	{
		config += itr->first + "=\"" + itr->second + "\" " ;
		
	}
	for (itr = mShuffAttrs.begin(); itr != mShuffAttrs.end(); itr++)
	{
		config += itr->first + "=\"" + itr->second + "\" ";
	}
	config += " >";
	config += mDataAssembler->getConfig() + "</datacollector>" + "</dataset>";
	return config;
}

void
AosConf::Output::getField(
		const string &name,
		u32 &i,
		boost::shared_ptr<DataField> &field)
{
	vector<boost::shared_ptr<DataField> > fields = mDataRecord->getFields();
	i = 0;
	for (; i<fields.size(); i++)
	{
		if (name == fields[i]->getAttribute("zky_name"))
		{
			field = fields[i];
			break;
		}
	}
	aos_assert(field);
}

void 
AosConf::Output::init(
			const u64 task_docid,
			const AosRundataPtr &rdata)
{
	vector<boost::shared_ptr<DataField> > fields = mDataRecord->getFields();
	if (mRecordType == AosDataRecordType::eBuff)
	{
		for (u32 i=0; i<fields.size(); i++)
		{
			string type = fields[i]->getAttribute("type");
			mDataAssembler->setFieldType(type);
		}
		mDataAssembler->setCmpFun(mCmpFun.type, -1, mCmpFun.record_type, mCmpFun.reserve);
		for (u32 i=0; i<mCmpFields.size(); i++)
		{
			u32 idx = 0;
			boost::shared_ptr<DataField> field;
			getField(mCmpFields[i].name, idx, field);
			aos_assert(field);
			string type = field->getAttribute("type");
			string cmp_type = convert(type);
			int pos = idx, size = -1;
			mDataAssembler->setCmpField("record", cmp_type, pos, size, mCmpFields[i].reserve);
		}

		for (u32 i=0; i<mAggrFields.size(); i++)
		{
			u32 idx = 0;
			boost::shared_ptr<DataField> field;
			getField(mAggrFields[i].name, idx, field);
			aos_assert(field);
			string type = field->getAttribute("type");
			string cmp_type = convert(type);
			int pos = idx;
			mDataAssembler->setAggrField("record", cmp_type, pos, mAggrFields[i].opr);
		}
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		int length = mDataRecord->getLength();
		mDataAssembler->setCmpFun(mCmpFun.type, length, mCmpFun.record_type, mCmpFun.reserve);
		for (u32 i=0; i<mCmpFields.size(); i++)
		{
			u32 idx = 0;
			boost::shared_ptr<DataField> field;
			getField(mCmpFields[i].name, idx, field);
			aos_assert(field);
			string type = field->getAttribute("type");
			string cmp_type = convert(type);
			int pos = -1, size = -1;
			pos = atoi(field->getAttribute("zky_offset").data());
			size = atoi(field->getAttribute("zky_length").data());
			mDataAssembler->setCmpField(cmp_type, pos, size, mCmpFields[i].reserve);
		}

		for (u32 i=0; i<mAggrFields.size(); i++)
		{
			u32 idx = 0;
			boost::shared_ptr<DataField> field;
			getField(mAggrFields[i].name, idx, field);
			aos_assert(field);
			string type = field->getAttribute("type");
			string cmp_type = convert(type);
			int pos = -1;
			pos = atoi(field->getAttribute("zky_offset").data());
			mDataAssembler->setAggrField(cmp_type, pos, mAggrFields[i].opr);
		}
	}

	AosXmlTagPtr output_xml = AosXmlParser::parse(getConfig() AosMemoryCheckerArgs);
	aos_assert(output_xml);

OmnCout << "================config: " << output_xml->toString() << endl;


	AosXmlTagPtr datacol_xml = output_xml->getFirstChild("datacollector");
	aos_assert(datacol_xml);

	AosXmlTagPtr asm_xml = datacol_xml->getFirstChild("asm");
	aos_assert(asm_xml);

	OmnString name = asm_xml->getAttrStr("zky_name");
	aos_assert(name != "");

	mAssembler = AosDataAssembler::createAssembler(
			name, task_docid, asm_xml, rdata.getPtr());
	aos_assert(mAssembler);
	
	AosXmlTagPtr rcd_xml = asm_xml->getFirstChild("datarecord");
	aos_assert(rcd_xml);

	mRecord = AosDataRecordObj::createDataRecordStatic(rcd_xml, task_docid, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert(mRecord);

	mRecord->setDataAssembler(mAssembler.getPtr());
}


AosXmlTagPtr
AosConf::Output::init2(
			const AosTaskObjPtr &task,
			const AosRundataPtr &rdata)
{
	vector<boost::shared_ptr<DataField> > fields = mDataRecord->getFields();
	if (mRecordType == AosDataRecordType::eBuff)
	{
		for (u32 i=0; i<fields.size(); i++)
		{
			string type = fields[i]->getAttribute("type");
			mDataAssembler->setFieldType(type);
		}
		mDataAssembler->setCmpFun(mCmpFun.type, -1, mCmpFun.record_type, mCmpFun.reserve);
		for (u32 i=0; i<mCmpFields.size(); i++)
		{
			u32 idx = 0;
			boost::shared_ptr<DataField> field;
			getField(mCmpFields[i].name, idx, field);
			string type = field->getAttribute("type");
			string cmp_type = convert(type);
			int pos = idx, size = -1;
			mDataAssembler->setCmpField("record", cmp_type, pos, size, mCmpFields[i].reserve);
		}

		for (u32 i=0; i<mAggrFields.size(); i++)
		{
			u32 idx = 0;
			boost::shared_ptr<DataField> field;
			getField(mAggrFields[i].name, idx, field);
			string type = field->getAttribute("type");
			string cmp_type = convert(type);
			int pos = idx;
			mDataAssembler->setAggrField("record", cmp_type, pos, mAggrFields[i].opr);
		}
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		int length = mDataRecord->getLength();
		mDataAssembler->setCmpFun(mCmpFun.type, length, mCmpFun.record_type, mCmpFun.reserve);
		for (u32 i=0; i<mCmpFields.size(); i++)
		{
			u32 idx = 0;
			boost::shared_ptr<DataField> field;
			getField(mCmpFields[i].name, idx, field);
			string type = field->getAttribute("type");
			string cmp_type = convert(type);
			int pos = -1, size = -1;
			pos = atoi(field->getAttribute("zky_offset").data());
			size = atoi(field->getAttribute("zky_length").data());
			mDataAssembler->setCmpField(cmp_type, pos, size, mCmpFields[i].reserve);
		}

		for (u32 i=0; i<mAggrFields.size(); i++)
		{
			u32 idx = 0;
			boost::shared_ptr<DataField> field;
			getField(mAggrFields[i].name, idx, field);
			string type = field->getAttribute("type");
			string cmp_type = convert(type);
			int pos = -1;
			pos = atoi(field->getAttribute("zky_offset").data());
			mDataAssembler->setAggrField(cmp_type, pos, mAggrFields[i].opr);
		}
	}

	AosXmlTagPtr output_xml = AosXmlParser::parse(getConfig() AosMemoryCheckerArgs);

OmnCout << "================config: " << output_xml->toString() << endl;

	// Add by Young, 2015/10/13
	AosXmlTagPtr datacol_xml = output_xml->getFirstChild("datacollector");
	aos_assert_r(datacol_xml, 0);

	AosXmlTagPtr asm_xml = datacol_xml->getFirstChild("asm");
	aos_assert_r(asm_xml, 0);

	AosXmlTagPtr rcd_xml = asm_xml->getFirstChild("datarecord");
	aos_assert_r(rcd_xml, 0);

	mRecord = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(mRecord, 0);
	//mRecord->setDataAssembler(mAssembler);


	return output_xml;
}
