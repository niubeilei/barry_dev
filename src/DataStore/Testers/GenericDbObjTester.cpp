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
// 	Created: 12/22/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStore/Testers/GenericDbObjTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "DataStore/GenericDbObj.h"
#include "DataStore/GenericField.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "TinyXml/tinyxml.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool 
AosGenericDbObjTester::start()
{
	cout << "    Start GenericDbObj Tester ...\n";
	u32 tries = 1000;

	// Drop the table
	//
	basicTest(tries);
	return true;
}


bool 
AosGenericDbObjTester::basicTest(u32 tries)
{
	// 
	// Test OmnString(const char *d);
	//
	OmnBeginTest << "Test OmnString(const char *d)";
	mTcNameRoot = "DataStore-Basic-Test";
	
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore(OmnStoreId::eGeneral);
	OmnCreateTc << (store) << endtc;

	OmnString def = "<tabledef tablename=\"mytesttable\">"
		"<field name=\"seqno\" datatype=\"int\"/>"
		"<field name=\"status\" datatype=\"char\"/>"
		"<field name=\"xml\" datatype=\"str\"/>"
		"</tabledef>";
	AosGenericDbObj obj;
	AosXmlRc errcode; 
	OmnString errmsg;
	bool tt = obj.parseDef(def, errcode, errmsg);

	OmnCreateTc << (tt) << endtc;

	// Retrieve 'seqno' field
	AosGenericFieldPtr seqno = obj.getField("seqno");
	OmnCreateTc << (seqno) << endtc;
	OmnCreateTc << (seqno->getFieldName() == "seqno") << endtc;
	OmnCreateTc << (seqno->getDataType() == AosGenericField::eType_Int) << endtc;

	// Retrieve 'status' field
	AosGenericFieldPtr status = obj.getField("status");
	OmnCreateTc << (status) << endtc;
	OmnCreateTc << (status->getFieldName() == "status") << endtc;
	OmnCreateTc << (status->getDataType() == AosGenericField::eType_Char) << endtc;

	// Retrieve 'xml' field
	AosGenericFieldPtr xml = obj.getField("xml");
	OmnCreateTc << (xml) << endtc;
	OmnCreateTc << (xml->getFieldName() == "xml") << endtc;
	OmnCreateTc << (xml->getDataType() == AosGenericField::eType_Str) << endtc;

	// Test 'setObjAttrs(...)'
	OmnString values = "<values>"
		"<field name=\"status\">c</field>"
		"<field name=\"xml\"><![CDATA[<the_tag xyz=\"123\"/>]]></field>"
		"</values>";
	TiXmlDocument thedoc;
	thedoc.Parse(values);
	TiXmlNode *root = thedoc.FirstChild();
	TiXmlElement *xmlobj = root->ToElement();
	OmnCreateTc << (xmlobj) << endtc;
	tt = obj.setObjAttrs(xmlobj, errcode, errmsg);
	OmnCreateTc << (tt) << endtc;

	// Retrieve 'status' field
	status = obj.getField("status");
	OmnCreateTc << (status) << endtc;
	OmnCreateTc << (status->getFieldName() == "status") << endtc;
	OmnCreateTc << (status->getDataType() == AosGenericField::eType_Char) << endtc;
	OmnCreateTc << (status->getValueStr() == "c") << endtc;

	// Retrieve 'xml' field
	xml = obj.getField("xml");
	OmnCreateTc << (xml) << endtc;
	OmnCreateTc << (xml->getFieldName() == "xml") << endtc;
	OmnCreateTc << (xml->getDataType() == AosGenericField::eType_Str) << endtc;
	OmnCreateTc << (xml->getValueStr() == "<the_tag xyz=\"123\"/>") << endtc;

	OmnRslt rslt = obj.addToDb();
	OmnCreateTc << (rslt) << endtc;

	// To retrieve the instance based on 'seqno'
	OmnString query_stmt = "<retrieve>"
		"<queried_fields>"
			"<field name=\"seqno\">1</field>"
		"</queried_fields>"
		"</retrieve>";

	TiXmlDocument thedoc1;
	thedoc1.Parse(query_stmt);
	thedoc1.Print();
	root = thedoc1.FirstChild();
	xmlobj = root->ToElement();
	OmnCreateTc << (xmlobj) << endtc;
	bool retrieved;
	int num_matched;
	AosGenericDbObj obj1;
	tt = obj1.parseDef(def, errcode, errmsg);
	OmnCreateTc << (tt) << endtc;

	// Retrieve 'seqno' field
	seqno = obj1.getField("seqno");
	OmnCreateTc << (seqno) << endtc;
	OmnCreateTc << (seqno->getFieldName() == "seqno") << endtc;
	OmnCreateTc << (seqno->getDataType() == AosGenericField::eType_Int) << endtc;

	// Retrieve 'status' field
	status = obj1.getField("status");
	OmnCreateTc << (status) << endtc;
	OmnCreateTc << (status->getFieldName() == "status") << endtc;
	OmnCreateTc << (status->getDataType() == AosGenericField::eType_Char) << endtc;
	OmnCreateTc << (status->getValueStr() != "c") << endtc;

	// Retrieve 'xml' field
	xml = obj1.getField("xml");
	OmnCreateTc << (xml) << endtc;
	OmnCreateTc << (xml->getFieldName() == "xml") << endtc;
	OmnCreateTc << (xml->getDataType() == AosGenericField::eType_Str) << endtc;
	OmnCreateTc << (xml->getValueStr() != "<the_tag xyz=\"123\"/>") << endtc;

	// To retrieve
	tt = obj1.retrieveFromDb(xmlobj, retrieved, num_matched, errcode, errmsg);
	OmnCreateTc << (tt) << endtc;

	// Retrieve 'seqno' field
	seqno = obj1.getField("seqno");
	OmnCreateTc << (seqno) << endtc;
	OmnCreateTc << (seqno->getFieldName() == "seqno") << endtc;
	OmnCreateTc << (seqno->getDataType() == AosGenericField::eType_Int) << endtc;
	OmnCreateTc << (seqno->getValueStr() == "1") << endtc;

	// Retrieve 'status' field
	status = obj1.getField("status");
	OmnCreateTc << (status) << endtc;
	OmnCreateTc << (status->getFieldName() == "status") << endtc;
	OmnCreateTc << (status->getDataType() == AosGenericField::eType_Char) << endtc;
	OmnCreateTc << (status->getValueStr() == "c") << endtc;

	// Retrieve 'xml' field
	xml = obj1.getField("xml");
	OmnCreateTc << (xml) << endtc;
	OmnCreateTc << (xml->getFieldName() == "xml") << endtc;
	OmnCreateTc << (xml->getDataType() == AosGenericField::eType_Str) << endtc;
	OmnCreateTc << (xml->getValueStr() == "<the_tag xyz=\"123\"/>") << endtc;

	return true;
}
