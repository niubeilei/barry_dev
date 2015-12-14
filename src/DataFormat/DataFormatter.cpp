////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataFormat/DataFormatter.h"

#include "DataFormat/DataFormatterRaw.h"
#include "DataFormat/DataFormatterFixLen.h"
#include "SEInterfaces/DataProcId.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataProcObj.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

const int sgNumTimestamps = 10;
static u64 lsTimestamps[sgNumTimestamps];
static AosDataProcId::E sgDataProcType[sgNumTimestamps];
static bool sgInited = false;

void AosDataFormatter_PrintStat()
{
	/*OmnScreen << "Data Formatter Statistics: " << endl;
	for (int i=0; i<sgNumTimestamps; i++)
	{
		cout << "    DataProcType: " << sgDataProcType[i] << "=" 
			<< lsTimestamps[i]/1000 << endl;
	}
	cout << endl;
	*/
}


AosDataFormatter::AosDataFormatter()
:
mType(eAosDataFormatType_Invalid),
mName(""),
mRecordLength(0),
mFixedLength(true)
{
	if (!sgInited)
	{
		for (int i=0; i<sgNumTimestamps; i++)
		{
			lsTimestamps[i] = 0;
			sgDataProcType[i] = AosDataProcId::eInvalid;
		}
		sgInited = true;
	}

}


AosDataFormatter::AosDataFormatter(const AosDataFormatType type)
:
mType(type),
mName(""),	
mRecordLength(0),
mFixedLength(true)
{
	if (!sgInited)
	{
		for (int i=0; i<sgNumTimestamps; i++)
		{
			lsTimestamps[i] = 0;
			sgDataProcType[i] = AosDataProcId::eInvalid;
		}
		sgInited = true;
	}
}


AosDataFormatter::AosDataFormatter(
		const AosDataFormatType type,
		const AosXmlTagPtr &formatter,
		const AosRundataPtr &rdata)
:
mType(type),
mRecordLength(0),
mFixedLength(true)
{
	if (!sgInited)
	{
		for (int i=0; i<sgNumTimestamps; i++)
		{
			lsTimestamps[i] = 0;
			sgDataProcType[i] = AosDataProcId::eInvalid;
		}
		sgInited = true;
	}
	bool rslt = config(formatter, rdata);
	if (!rslt) OmnThrowException(rdata->getErrmsg());
}


AosDataFormatter::~AosDataFormatter()
{
	mFields.clear();
	mDataProcs.clear();
}


bool
AosDataFormatter::config(
		const AosXmlTagPtr &formatter, 
		const AosRundataPtr &rdata)
{
	// 'formatter' is in the form:
	// 	<format> 
	// 		<data_procs>
	// 			<proc .../>
	// 			...
	// 		</data_procs>
	// 	</format>
	// this function uses this formatter to create this intsance.
	aos_assert_rr(formatter, rdata, false);
	
	AosXmlTagPtr elements = formatter->getFirstChild("elements");
	if (!elements)
	{
		OmnAlarm << "Missing elements: " << formatter->toString() << enderr;
		return false;
	}

	AosXmlTagPtr element = elements->getFirstChild();
	if (!element)
	{
		OmnAlarm << "Empty formatter" << enderr;
		return false;
	}

	AosDataFieldObjPtr data_elem;
	while (element)
	{
		try	
		{
			data_elem = AosDataField::createDataFieldStatic(element, 0, rdata.getPtr());
			if(data_elem) mFields.push_back(data_elem);
		}
		catch (...)
		{
			OmnAlarm << "Failed creating data element: " << element->toString() << enderr;
		}
		element = elements->getNextChild();
	}

	mName = formatter->getAttrStr(AOSTAG_NAME, "doc");
	mFixedLength = formatter->getAttrBool(AOSTAG_FIXED_LENGTH, true);
	
	mRecordLength = formatter->getAttrInt("record_len", 0);
	aos_assert_r(mRecordLength > 0, false);

	// Parse data procs
	AosXmlTagPtr dp_tag = formatter->getFirstChild("data_procs");
	if (!dp_tag) return true;
	
	AosXmlTagPtr tag = dp_tag->getFirstChild();
	while (tag)
	{
		try
		{
			AosDataProcObjPtr proc = AosDataProcObj::createDataProcStatic(tag, rdata);
			if(proc) mDataProcs.push_back(proc);
		}
		catch(...)
		{
			OmnAlarm << "Failed creating data proc: " << tag->toString() << enderr;
		}
		tag = dp_tag->getNextChild();
	}
	return true;
}


AosDataFieldObjPtr
AosDataFormatter::getElement(
		const int idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && idx < (int)mFields.size(), 0);
	AosDataFieldObjPtr elem = mFields[idx];
	aos_assert_r(elem, 0);
	return elem;
}	


AosDataFormatterPtr
AosDataFormatter::getFormatter(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, 0);
	
	// OmnString typeStr = xml->getAttrStr("type");
	OmnString typeStr = xml->getAttrStr(AOSTAG_TYPE);
	AosDataFormatType type = AosDataFormatType_toEnum(typeStr);
	if (!AosDataFormatType_isValid(type)) return 0;

	AosDataFormatterPtr format;
	try
	{
		switch(type)
		{
		case eAosDataFormatType_Raw : 
			 format = OmnNew AosDataFormatterRaw(xml, rdata);
			 break;
		
		case eAosDataFormatType_FixLen :
			 format = OmnNew AosDataFormatterFixLen(xml, rdata);
			 break;

		default :
			 OmnAlarm << "format error" << enderr;
			 return 0;
		}
	}

	catch(...)
	{
		OmnAlarm << "format error" << enderr;
		return 0;
	}
	return format;
}

	
bool
AosDataFormatter::createRandomDoc(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	int len = mFields.size();
	aos_assert_r(len > 0, false);

	bool rslt;
	for(int i=0; i<len; i++)
	{
		rslt = mFields[i]->createRandomValue(buff, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}
	return true;
}

	
AosDataProcStatus::E
AosDataFormatter::convertData(
		const char *record, 
		const int data_len,
		const AosBuffPtr &buff,
		const u64 &docid,
		const AosImportDataThrdPtr &task,
		const AosRundataPtr &rdata)
{
	// This function reads in a record pointed to by 'record', 
	// creates the IILs as needed, and then creates a new record
	// based on 'target_formatter'. The new record is appended 
	// to 'buff'.
	if(mFixedLength && data_len < mRecordLength)
	{
		return AosDataProcStatus::eDataTooShort;
	}
	
	/*
	AosDataProcStatus::E status;
	u64 timestamp = 0;
	for (u32 i=0; i<mDataProcs.size(); i++)
	{
		timestamp = OmnGetTimestamp();
		status = mDataProcs[i]->procData(record, data_len, idx, docid, buff, 
						this, task, rdata);
		if ((int)i < sgNumTimestamps) 
		{
			sgDataProcType[i] = mDataProcs[i]->getType();
			lsTimestamps[i] += OmnGetTimestamp() - timestamp;
		}

		switch (status)
		{
		case AosDataProcStatus::eDataTooShort:
			 // The data is too short. Do nothing. Just return.
			 return status;

		case AosDataProcStatus::eRecordFiltered:
			 // The record is filtered out. 
			 return status;

		case AosDataProcStatus::eContinue:
			 break;

		case AosDataProcStatus::eError:
			 OmnAlarm << rdata->getErrmsg() << enderr;
			 break;

		default:
			 AosSetErrorU(rdata, "unrecog_dataproc_status") << ": " << status;
			 AosLogError(rdata);
			 break;
		}
	}
	*/
	OmnNotImplementedYet;
	return AosDataProcStatus::eContinue;
}


bool
AosDataFormatter::resolveIILAssemblers(
		const vector<AosDataAssemblerObjPtr> &iil_assemblers, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

