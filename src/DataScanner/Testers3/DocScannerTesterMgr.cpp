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
// How to torture: 
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 09/26/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/Testers3/DocScannerTesterMgr.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DataScanner/Testers3/DocScannerTester.h"
#include "DataScanner/Testers3/FixedLengthDoc.h"
#include "DataScanner/Testers3/NormalDoc.h"
#include "DataScanner/Testers3/Ptrs.h"
#include "DataScanner/DataScanner.h"
#include "DataScanner/DocScanner.h"
#include "DocClient/DocidShufflerMgr.h"
#include "DocClient/DocClient.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
static int smStartTime = 0;

AosDocScannerTesterMgr::AosDocScannerTesterMgr(const AosXmlTagPtr &def)
{
	mName = "DocScannerTesterMgr";
	aos_assert(def);
	mCreated = def->getAttrBool("create", false);
	mCreateDocType = def->getAttrInt("create_doc_type", 0);
	mScannerNum  = def->getAttrU64("num_scanner", 0);
	mObjid = def->getAttrStr(AOSTAG_OBJID, 0);
	mIsRetrieve = def->getAttrBool("is_retrieve", true);
	mCreateConf = def->getAttrBool("is_create_conf", false);
}


AosDocScannerTesterMgr::~AosDocScannerTesterMgr()
{
}


bool 
AosDocScannerTesterMgr::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosDocScannerTesterMgr::basicTest()
{
	AosRundataPtr rdata = OmnApp::getRundata()->clone(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	rdata->setUserid(307);
	mNum = 10000000;
	if (mCreateConf)
	{
		AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByObjid("vpd_data", rdata);	
OmnScreen << "doc : " << doc->toString() << endl;
	}
	if (mCreated) 
	{
		config();
		u64 startTime = OmnGetSecond();
		aos_assert_r(createData(rdata), false);
		u32 delta = OmnGetSecond() - startTime;
		OmnScreen << "num docs :"<< mNum * mConfig.size() << " , Speed: " << delta << endl;
		return true;
	}

	for (u64 i = 0; i < mScannerNum; i++)
	{
		//u64 scanner_id = AosDocidShufflerMgr::getSelf()->createScannerId(rdata);
		OmnString scanner_id; 
		int id = 1;
		scanner_id << id;
		AosDocScannerTesterPtr obj = OmnNew AosDocScannerTester(
				scanner_id, mObjid, mIsRetrieve, rdata);
		id++;
		scanner_id = "";
		mVector.push_back(obj);

	}
	for (u64 i = 0; i < mVector.size(); i++)
	{
		mVector[i]->startScanner(true);
	}
	return true;
}


bool
AosDocScannerTesterMgr::createData(const AosRundataPtr &rdata)
{
	if (mCreateDocType == 0)
	{
		return createFixedLengthDoc1(rdata);
	}
	else
	{
		return createNormal(rdata);
	}
	return true;
}


bool
AosDocScannerTesterMgr::createNormal(const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByObjid(mObjid, rdata);	
	if (!doc)
	{
		OmnString docstr = "<records ";
		docstr << "zky_objid =\"" <<mObjid << "\" zky_public_doc=\"true\" "
			<< "zky_public_ctnr=\"true\"> </records>";
		AosXmlTagPtr dd = AosDocClient::getSelf()->createDoc(docstr, true, rdata);
		aos_assert_r(dd, false);
		doc = AosDocClient::getSelf()->getDocByObjid(mObjid, rdata);	
		aos_assert_r(dd->getAttrStr(AOSTAG_OBJID, "") == mObjid, false);
	}
	aos_assert_r(doc, false);
	AosXmlTagPtr records = doc->getFirstChild();
	aos_assert_r(records, false);
	u64 start_docid;
	AosNormalDoc obj;
	for (u32 j = 1; j <= mNum; j++)
	{
		rdata->resetTTL();
		obj.createData(rdata);
	}
	start_docid = obj.getStartDocid();
	OmnString docstr;
	docstr << "<record start_docid=\"" << start_docid << "\" "
		<< " num_docs=\"" << mNum << "\" />";

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	records->addNode(xml);
	bool rslt = AosModifyDoc(doc, rdata);
OmnScreen << "doc : " << doc->toString() << endl;
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDocScannerTesterMgr::createFixedLengthDoc(const AosRundataPtr &rdata)
{
	u64 start_docid = 0;
	u64 sizeid = 0;
	bool finish = false;
	OmnString docstr = "<records ";
	docstr << "zky_objid =\"" << mObjid << "\" zky_public_doc=\"true\" "
		<< "zky_public_ctnr=\"true\">";

	for (u32 i = 0; i < mConfig.size(); i++)
	{
		finish = false;
		AosFixedLengthDoc   obj(mConfig[i], rdata);
		for (u32 j = 1; j <= mNum; j++)
		{
			if (j == mNum) finish = true;
			obj.createData(rdata, finish, mNum);
		}
		start_docid = obj.getStartDocid();
		sizeid = obj.getCrtSizeid();
		docstr << "<record start_docid=\"" << start_docid << "\" "
		 	<< " num_docs=\"" << mNum << "\" "
			<< "sizeid =\"" << sizeid << "\" />";
		OmnSleep(10);
	}
	docstr <<"</records>";
	AosXmlTagPtr dd = AosDocClient::getSelf()->createDoc(docstr, true, rdata);
	aos_assert_r(dd, false);
OmnScreen << "doc : " << dd->toString() << endl;
	aos_assert_r(dd->getAttrStr(AOSTAG_OBJID, "") == mObjid, false);
	return true;
}

bool
AosDocScannerTesterMgr::createFixedLengthDoc1(const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByObjid(mObjid, rdata);	
	if (!doc)
	{
		OmnString docstr = "<records ";
		docstr << "zky_objid =\"" << mObjid << "\" zky_public_doc=\"true\" "
			<< "zky_public_ctnr=\"true\">";
		docstr <<"</records>";
		doc = AosDocClient::getSelf()->createDoc(docstr, true, rdata);
		aos_assert_r(doc->getAttrStr(AOSTAG_OBJID, "") == mObjid, false);
	}
	aos_assert_r(doc, false);
	AosXmlTagPtr records = doc->getFirstChild();
	aos_assert_r(records, false);

	u64 start_docid = 0;
	u64 sizeid = 0;
	bool finish = false;
	u64 count = 0;
	for (u32 i = 0; i < mConfig.size(); i++)
	{
		finish = false;
		AosFixedLengthDoc   obj(mConfig[i], rdata);
		obj.setCheck(false);
		for (u32 j = 1; j <= mNum; j++)
		{
			//if (j == mNum) finish = true;
			if (smStartTime == 0) smStartTime = OmnGetSecond();
			if (OmnGetSecond() - smStartTime >= 900) finish = true;
			count ++;
			obj.createData(rdata, finish, mNum);
			if (finish) break;
		}
		start_docid = obj.getStartDocid();
		sizeid = obj.getCrtSizeid();

		OmnString docstr ;
		docstr<< "<record start_docid=\"" << start_docid << "\" "
		 	<< " num_docs=\"" << mNum << "\" "
			<< "sizeid =\"" << sizeid << "\" />";
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		records->addNode(xml);
if (OmnGetSecond() - smStartTime >= 900 && finish) break;
OmnSleep(10);
	}
	OmnScreen <<"============== count:" << count << " , " << smStartTime << " , " << OmnGetSecond() - smStartTime << "s, "  << (OmnGetSecond() - smStartTime) / 60 << "m"<< endl;

	bool rslt = AosModifyDoc(doc, rdata);
OmnScreen << "doc : " << doc->toString() << endl;
	aos_assert_r(rslt, false);
	return true;
}



void
AosDocScannerTesterMgr::config()
{
	//int num_size = 100;
	int num_size = 3;
	for (int i = 0; i < num_size; i++)
	{
		int record_len = rand() %1991 + 9;
OmnScreen << " record_len :" << record_len << endl;
		OmnString docstr="<record type=\"fixbin\" ";
		docstr << " zky_name=\"doc\" record_len=\" " << record_len << "\" " 
			<< " zky_objid=\"unicom_ac_config_" << record_len << "\" "
			<< " zky_pctrs=\"sdoc_unicom_doc\" zky_public_doc=\"true\" zky_public_ctnr=\"true\">"
			//<< " <records> "
			//<< " <element type=\"str\" zky_name=\"num\" zky_rawformat=\"str\" zky_length=\"8\" zky_offset=\"0\"/> "
			//<< " <element type=\"str\" zky_name=\"str\" zky_rawformat=\"str\" "
			//<< " zky_length=\"" << record_len - 8<< "\" "
			//<< " zky_offset=\"8\"/>"
			<< " <element type=\"str\" zky_name=\"str\" zky_rawformat=\"str\" "
			<< " zky_length=\"" << record_len << "\" "
			<< " zky_offset=\"0\"/>"
			//<< " </records> "
			<< " </record> ";

		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
		mConfig.push_back(xml);
	}
}

/*
bool
AosDocScannerTesterMgr::create(const AosRundataPtr &rdata)
{
	AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByObjid("vpd_data11", rdata);	
	if (dd)
	{
		bool rslt = AosDocClient::getSelf()->deleteDoc(dd, rdata);
		aos_assert_r(rslt, false);
	}

	OmnString docstr;
	docstr <<"<records zky_objid =\"vpd_data11\" zky_public_doc=\"true\" zky_public_ctnr=\"true\">"
		<< "<record start_docid=\"432345564395339786\" num_docs=\"10000\" sizeid=\"10\"/>"
		<<" <record start_docid=\"432345564412117002\" num_docs=\"10000\" sizeid=\"11\"/>"
		<<" <record start_docid=\"432345564428894218\" num_docs=\"10000\" sizeid=\"12\"/>"
		<<" <record start_docid=\"432345564445671434\" num_docs=\"10000\" sizeid=\"13\"/>"
		<<" <record start_docid=\"432345564462448650\" num_docs=\"10000\" sizeid=\"14\"/>"
		<<" <record start_docid=\"432345564479225866\" num_docs=\"10000\" sizeid=\"15\"/>"
		<<" <record start_docid=\"432345564496003082\" num_docs=\"10000\" sizeid=\"16\"/>"
		<<" <record start_docid=\"432345564512780298\" num_docs=\"10000\" sizeid=\"17\"/>"
		<<" <record start_docid=\"432345564529557514\" num_docs=\"10000\" sizeid=\"18\"/>"
		<<" <record start_docid=\"432345564546334730\" num_docs=\"10000\" sizeid=\"19\"/>"
		<<" <record start_docid=\"432345564563111946\" num_docs=\"100000\" sizeid=\"20\"/>"
		<<" <record start_docid=\"432345564579889162\" num_docs=\"100000\" sizeid=\"21\"/>"
		<<" <record start_docid=\"432345564596666378\" num_docs=\"100000\" sizeid=\"22\"/>"
		<<" <record start_docid=\"432345564613443594\" num_docs=\"100000\" sizeid=\"23\"/>"
		<<" <record start_docid=\"432345564630220810\" num_docs=\"100000\" sizeid=\"24\"/>"
		<<" <record start_docid=\"432345564646998026\" num_docs=\"100000\" sizeid=\"25\"/>"
		<<" <record start_docid=\"432345564663775242\" num_docs=\"100000\" sizeid=\"26\"/>"
		<<" <record start_docid=\"432345564680552458\" num_docs=\"100000\" sizeid=\"27\"/>"
		<<" <record start_docid=\"432345564697329674\" num_docs=\"100000\" sizeid=\"28\"/>"
		<<" <record start_docid=\"432345564714106890\" num_docs=\"100000\" sizeid=\"29\"/>"
		<<" <record start_docid=\"432345564730884106\" num_docs=\"100000\" sizeid=\"30\"/>"
		<<" <record start_docid=\"432345564747661322\" num_docs=\"100000\" sizeid=\"31\"/>"
		<<" <record start_docid=\"432345564764438538\" num_docs=\"100000\" sizeid=\"32\"/>"
		<<" <record start_docid=\"432345564781215754\" num_docs=\"100000\" sizeid=\"33\"/>"
		<<" <record start_docid=\"432345564797992970\" num_docs=\"100000\" sizeid=\"34\"/>"
		<<" <record start_docid=\"432345564814770186\" num_docs=\"100000\" sizeid=\"35\"/>"
		<<" <record start_docid=\"432345564831547402\" num_docs=\"100000\" sizeid=\"36\"/>"
		<<" <record start_docid=\"432345564848324618\" num_docs=\"100000\" sizeid=\"37\"/>"
		<<" <record start_docid=\"432345564865101834\" num_docs=\"100000\" sizeid=\"38\"/>"
		<<" <record start_docid=\"432345564881879050\" num_docs=\"100000\" sizeid=\"39\"/>"
		<<" <record start_docid=\"432345564898656266\" num_docs=\"100000\" sizeid=\"40\"/>"
		<<" <record start_docid=\"432345564915433482\" num_docs=\"100000\" sizeid=\"41\"/>"
		<<" <record start_docid=\"432345566240833546\" num_docs=\"1000000\" sizeid=\"120\"/>"
		<<" <record start_docid=\"432345566257610762\" num_docs=\"1000000\" sizeid=\"121\"/>"
		<<" <record start_docid=\"432345566274387978\" num_docs=\"1000000\" sizeid=\"122\"/>"
		<<" <record start_docid=\"432345566291165194\" num_docs=\"1000000\" sizeid=\"123\"/>"
		<<" <record start_docid=\"432345566307942410\" num_docs=\"1000000\" sizeid=\"124\"/>"
		<< "</records>";
	dd = AosDocClient::getSelf()->createDoc(docstr, true, rdata);
	aos_assert_r(dd->getAttrStr(AOSTAG_OBJID, "") == "vpd_data11", false);
	return true;
}


bool
AosDocScannerTesterMgr::create1(const AosRundataPtr &rdata)
{
	AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByObjid("vpd_data12", rdata);	
	if (dd)
	{
		bool rslt = AosDocClient::getSelf()->deleteDoc(dd, rdata);
		aos_assert_r(rslt, false);
	}

	OmnString docstr;
	docstr <<"<records zky_objid =\"vpd_data12\" zky_public_doc=\"true\" zky_public_ctnr=\"true\">"
		<<" <record start_docid=\"432345565066428426\" num_docs=\"100000\" sizeid=\"50\"/>"
		<<" <record start_docid=\"432345565083205642\" num_docs=\"100000\" sizeid=\"51\"/>"
		<<" <record start_docid=\"432345565099982858\" num_docs=\"100000\" sizeid=\"52\"/>"
		<<" <record start_docid=\"432345565116760074\" num_docs=\"100000\" sizeid=\"53\"/>"
		<<" <record start_docid=\"432345565133537290\" num_docs=\"100000\" sizeid=\"54\"/>"
		<<" <record start_docid=\"432345565234200586\" num_docs=\"100000\" sizeid=\"60\"/>"
		<<" <record start_docid=\"432345565250977802\" num_docs=\"100000\" sizeid=\"61\"/>"
		<<" <record start_docid=\"432345565267755018\" num_docs=\"100000\" sizeid=\"62\"/>"
		<<" <record start_docid=\"432345565284532234\" num_docs=\"100000\" sizeid=\"63\"/>"
		<<" <record start_docid=\"432345565301309450\" num_docs=\"100000\" sizeid=\"64\"/>"
		<<" <record start_docid=\"432345565318086666\" num_docs=\"100000\" sizeid=\"65\"/>"
		<<" <record start_docid=\"432345565334863882\" num_docs=\"100000\" sizeid=\"66\"/>"
		<<" <record start_docid=\"432345565351641098\" num_docs=\"100000\" sizeid=\"67\"/>"
		<<" <record start_docid=\"432345565368418314\" num_docs=\"100000\" sizeid=\"68\"/>"
		<<" <record start_docid=\"432345565385195530\" num_docs=\"100000\" sizeid=\"69\"/>"
		<<" <record start_docid=\"432345565401972746\" num_docs=\"100000\" sizeid=\"70\"/>"
		<<" <record start_docid=\"432345565418749962\" num_docs=\"100000\" sizeid=\"71\"/>"
		<<" <record start_docid=\"5070\" num_docs=\"100000\"/>"
		<<" <record start_docid=\"432345565569744906\" num_docs=\"100000\" sizeid=\"80\"/>"
		<<" <record start_docid=\"432345565586522122\" num_docs=\"100000\" sizeid=\"81\"/>"
		<<" <record start_docid=\"432345565603299338\" num_docs=\"100000\" sizeid=\"82\"/>"
		<<" <record start_docid=\"432345565620076554\" num_docs=\"100000\" sizeid=\"83\"/>"
		<<" <record start_docid=\"432345565636853770\" num_docs=\"100000\" sizeid=\"84\"/>"
		<<" <record start_docid=\"432345565653630986\" num_docs=\"100000\" sizeid=\"85\"/>"
		<<" <record start_docid=\"432345565670408202\" num_docs=\"100000\" sizeid=\"86\"/>"
		<<" <record start_docid=\"432345565687185418\" num_docs=\"100000\" sizeid=\"87\"/>"
		<<" <record start_docid=\"432345565703962634\" num_docs=\"100000\" sizeid=\"88\"/>"
		<<" <record start_docid=\"432345565720739850\" num_docs=\"100000\" sizeid=\"89\"/>"
		<<" <record start_docid=\"432345565737517066\" num_docs=\"100000\" sizeid=\"90\"/>"
		<<" <record start_docid=\"432345565754294282\" num_docs=\"100000\" sizeid=\"91\"/>"
		<<" <record start_docid=\"432345566073061386\" num_docs=\"100000\" sizeid=\"110\"/>"
		<<" <record start_docid=\"432345566324719626\" num_docs=\"1000000\" sizeid=\"125\"/>"
		<<" <record start_docid=\"432345566341496842\" num_docs=\"1000000\" sizeid=\"126\"/>"
		<<" <record start_docid=\"432345566358274058\" num_docs=\"1000000\" sizeid=\"127\"/>"
		<<" <record start_docid=\"432345566375051274\" num_docs=\"1000000\" sizeid=\"128\"/>"
		<<" <record start_docid=\"432345566391828490\" num_docs=\"1000000\" sizeid=\"129\"/>"
		<< "</records>";
	dd = AosDocClient::getSelf()->createDoc(docstr, true, rdata);
	aos_assert_r(dd->getAttrStr(AOSTAG_OBJID, "") == "vpd_data12", false);
	return true;
}
*/
