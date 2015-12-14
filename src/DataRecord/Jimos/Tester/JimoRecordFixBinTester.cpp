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
// 2013/05/29: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/Jimos/Tester/JimoRecordFixBinTester.h"
#include "DataRecord/DataRecord.h"

#include "API/AosApi.h"
#include "DataRecord/Jimos/JimoRecordFixBin.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"

bool mValue1, mValue2, mValue3;
bool setValues(char *data, int len);

bool AosJimoRecordFixBinTester()
{
	// Run the DLL
	// construct worker_doc and jimo_doc
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);

	OmnString ss = "<record type=\"fixbin\" zky_name=\"doc\" record_len=\"20\" >"
		 "<element type=\"str\" zky_name=\"system_type\" zky_offset=\"0\" zky_length=\"2\"/>"
		 "<element type=\"str\" zky_name=\"seqno\" zky_offset=\"4\" zky_length=\"3\"/>"
		 "<element type=\"str\" zky_name=\"calling_number\" zky_offset=\"11\" zky_length=\"5\"/>"
		 "<element type=\"bool\" zky_name=\"test\" zky_value_from_field=\"calling_number\" zky_offset=\"11\" zky_length=\"5\" zky_ignore_serialize=\"true\">"
			 "<default>true</default>"
	 	 "</element>"
		 "</record>";


	AosXmlTagPtr worker_doc = AosStr2Xml(rdata, ss AosMemoryCheckerArgs);
	aos_assert_r(worker_doc, false);

	ss = "<jimo zky_libname=\"libJimoDataRecord.so\""
		"zky_objid=\"test\""
		"zky_classname=\"AosJimoRecordFixBin\"" 
		"current_version=\"1.0\">"

		"<versions current_version=\"1.0\">"
			"<ver_0 version=\"1.0\">libJimoDataRecord.so</ver_0>"
		"</versions>"

	"</jimo>";	
		
	AosXmlTagPtr jimo_doc = AosStr2Xml(rdata, ss AosMemoryCheckerArgs);
	aos_assert_r(jimo_doc, false);

	// 1. create jimo
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_doc);
	aos_assert_r(jimo, false);
	OmnScreen << "Jimo created" << endl;

	AosJimoType::E jimo_type = jimo->getJimoType();
	aos_assert_r(jimo_type == AosJimoType::eDataRecord, false);
	AosDataRecord *data_record= dynamic_cast<AosDataRecord*>(jimo.getPtr());
	aos_assert_r(data_record, false);
	bool rslt = data_record->config(worker_doc, rdata);
	aos_assert_r(rslt, false);
	int numFields = data_record->getNumFields();

	int recordlen = data_record->getRecordLen();
	OmnScreen << "=========================numfields: " << numFields << endl;
	OmnScreen << "=========================recordlen: " << recordlen << endl; 


	for (int kk=0; kk<1000; kk++)
	{
		OmnScreen << "start ======== " << kk << endl;
		AosValueRslt value;
		int tmpopr = rand() % 5;
		switch(tmpopr)
		{
		case 0:
			 value.setValue("dddddddddddTrue  dddd");
			 break;
		case 1:
			 value.setValue("dddddddddddtrue  dddd");
			 break;
		case 2:
			 value.setValue("ddddddddddd true dddd");
			 break;
		case 3:
			 value.setValue("ddddddddddd True dddd");
			 break;
		case 4:
			 value.setValue("ddddddddddd false dddd");
			 break;
		default:
			 OmnAlarm << "internal error " << enderr;
			 return false;
		}

		vector<bool> fieldrslt;
		for (int i=0; i<numFields; i++)
		{
			bool rslt = data_record->setFieldValue(i, value, rdata);
			fieldrslt.push_back(rslt);
		}
		

		for (int j=0; j<numFields; j++)
		{
			bool rslt = data_record->getFieldValue(j, value, rdata);	
			if (rslt != fieldrslt[j]) OmnAlarm << "reslut is not match the set value!!!" << enderr;
		}
		OmnScreen << "end ======== " << kk << endl;
	}




//	// case 1. generate value, manual to set value, the check the value
//	// case 2. call dataField's func to set value, then check the value
//	char data[1000];
//
//	// Run1: use 'jimo->run(...)'
//	int tries = 1000000;
//	u64 time1 = OmnGetTimestamp();
//	u64 last_time = time1;
//	for (int i=0; i<tries; i++)
//	{
//		// Field1: 	[5, 10], Bool
//		// Field2: 	[25, 33], Bool
//		// Field3: 	[51, 60], Bool
//		setValues(data, 1000);
//		
//		if (i % 100000 == 0 && i)  
//		{
//			u64 time2 = OmnGetTimestamp();
//			cout << "Finished: " << i << ". Time: " << time2 - last_time << endl;
//			last_time = time2;
//		}
//		jimo->run(rdata, worker_doc);
//	}
//
//	u64 time3 = OmnGetTimestamp();
//	cout << "Total finished: " << time3 - time1 << endl;

//	for (int ...)
//	{
//		mValue1 = rand() % 2;
//		field1->setFieldToRecord(..., mValue1);
//		field2->setFieldToRecord(..., mValue2);
//		field3->setFieldToRecord(..., mValue3);
//	}

	return true;
}


OmnString 
generateValue(int num)
{
	OmnString vv = "";
	if (num >= 4)
	{
		vv << "true";
	}
	for (int i=0; i< num-4; i++)
	{
		vv << " ";
	}

	return vv;
}


bool
setValues(char *data, int len)
{
	// Field1: 	[5,  15], Bool
	// Field2: 	[25, 35], Bool
	// Field3: 	[35, 45], Bool
	// 	'data': dddddxxxxxxddddddddddddddddddddddddddddddddd
	OmnString vv = generateValue(11);
	aos_assert_r(vv.length() == 11, false);
	strncpy(&data[5], vv.data(), 11);
	vv.toLower();
	mValue1 = (vv == "true");

	vv = generateValue(11);
	aos_assert_r(vv.length() == 11, false);
	strncpy(&data[25], vv.data(), 11);
	vv.toLower();
	mValue2 = (vv == "true");

	vv = generateValue(11);
	aos_assert_r(vv.length() == 11, false);
	strncpy(&data[35], vv.data(), 11);
	vv.toLower();
	mValue3 = (vv == "true");

	return true;
}


