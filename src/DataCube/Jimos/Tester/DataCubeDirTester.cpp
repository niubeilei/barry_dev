////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	   
//
// Modification History:
// 09/18/2013 Create by Young
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/Tester/DataCubeDirTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "SEUtil/IILName.h"
#include "IILClient/IILClient.h"
#include "TransUtil/RoundRobin.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/UtUtil.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/OmnNew.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "DocClient/DocClient.h"
#include "SearchEngine/DocMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SEServer/Local/AllTrans.h"

#include "SEBase/SecReq.h"
#include <list>
#include "DataCube/Jimos/DataCubeDir.h"

AosDataCubeDirTester::AosDataCubeDirTester()
:
mIsFinished(false)
{
}


AosDataCubeDirTester::~AosDataCubeDirTester()
{
}


bool 
AosDataCubeDirTester::start()
{
	OmnScreen << "Start AosSeLogClient Tester ..." << endl;

	// Siteid is random.
	OmnString siteid = "100";//getSiteId(); 
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	rdata->setSsid(mSsid);
	mRundata = rdata;

	aos_assert_r(basicTest(rdata), false);
	return true;
}


bool 
AosDataCubeDirTester::basicTest(const AosRundataPtr &rdata)
{
	readDirTest(rdata);

	return true;
}


bool
AosDataCubeDirTester::readDirTest(const AosRundataPtr &rdata)
{
	// Run the DLL
	// construct worker_doc and jimo_doc
	OmnString ss = "<worker read_block_size=\"1000\" zky_character=\"UTF8\">";
	ss << "<dirs>"
		<< "<dir physical_id=\"0\" dir_name=\"/home/young/test/readtestdir/111\" />"
		<< "<dir physical_id=\"0\" dir_name=\"/home/young/test/readtestdir/222\" />"
		<< "<dir physical_id=\"0\" dir_name=\"/home/young/test/readtestdir/333\" />"
		<< "<dir physical_id=\"0\" dir_name=\"/home/young/test/readtestdir/444\" />"
	   << "</dirs></worker>";
	AosXmlTagPtr worker_doc = AosStr2Xml(rdata, ss AosMemoryCheckerArgs);
	aos_assert_r(worker_doc, false);

	ss = "<jimo zky_libname=\"libDataCubicJimos.so\""
		"zky_objid=\"test\""
		"zky_classname=\"AosDataCubeDir\"" 
		"current_version=\"1.0\">"

		"<versions current_version=\"1.0\">"
			"<ver_0 version=\"1.0\">libDataCubicJimos.so</ver_0>"
		"</versions>"

	"</jimo>";	
		
	AosXmlTagPtr jimo_doc = AosStr2Xml(rdata, ss AosMemoryCheckerArgs);
	aos_assert_r(jimo_doc, false);

	// 1. create jimo
	OmnScreen << "\n Jimo created--------------------" << endl;

	mDataCube = AosCreateDataCube(rdata, worker_doc, jimo_doc);
	mDataCube->setCaller(this);
	mDataCube->readData(0, rdata);
	while(1)
	{
		if (mIsFinished)
		{
OmnScreen << "finsifd --------------------------------========" << endl;
			break;
		}
		sleep(1);
	}


OmnScreen << "\n   ------------------------- \n" 
		<< " directory read finish!...\n"
		<< " --------------------------- " << endl;
	return true;
}


void
AosDataCubeDirTester::callBack(
		const u64 &reqid,
		const AosBuffDataPtr &buff_data,
		bool finished)
{
	u32 len = buff_data->dataLen();
	if (len > 0) writeDataIntoFile(mRundata, buff_data);
	if (finished)
	{
		OmnScreen << "\n \n \n WWWWWWWWWWWWWWWWWWW callback finished!" << endl;
		mIsFinished = true;
	}
	else
	{
		mDataCube->readData(reqid, mRundata);
	}
}


bool
AosDataCubeDirTester::writeDataIntoFile(
		const AosRundataPtr &rdata,
		const AosBuffDataPtr &buff_data)
{
	// 1. check wheather have curFile ,
	// 		if not exist, create current file.
	if (!mCrtFile) 
	{
		createNewFile(rdata, buff_data);
		aos_assert_r(mCrtFile, false);
	}
	
	// 2.append data to file.
	AosXmlTagPtr metadata = buff_data->getMetadata();
	aos_assert_r(metadata, false);
	OmnString name = metadata->getAttrStr(AOSTAG_SOURCE_NAME, "");
	aos_assert_r(name != "", false);
	OmnString filename = "/home/young/test/writetestdir/";
	filename << name;
	aos_assert_r(filename == mCrtFileName, false);

	AosBuffPtr buff = buff_data->getBuff();
	aos_assert_r(buff, false);
	mCrtFile->append(buff->data(), buff->dataLen(), true);

	// 3. check current file isEOF.
	if (buff_data->isEOF())
	{
		mCrtFile = 0;
	}
	return true;
}


bool 
AosDataCubeDirTester::createNewFile(
		const AosRundataPtr &rdata,
		const AosBuffDataPtr &buff_data)
{
	AosXmlTagPtr metadata = buff_data->getMetadata();
	aos_assert_r(metadata, false);
	OmnString name = metadata->getAttrStr(AOSTAG_SOURCE_NAME, "");
	aos_assert_r(name != "", false);
	mCrtFileName = "/home/young/test/writetestdir/";
	mCrtFileName << name;

	//AosNetFileInfo fileinfo = buff_data->getNetFileInfo();
	//mCrtFileName = fileinfo.mFileName;
	//int idx = mCrtFileName.indexOf(0, '/', true);
	//aos_assert_r(idx >= 0, false);
	//OmnString name = mCrtFileName.substr(idx + 1);
	//mCrtFileName = "/home/young/test/writetestdir/";
	//mCrtFileName << name;

	mCrtFile = OmnNew OmnFile(mCrtFileName, OmnFile::eWriteCreate AosMemoryCheckerArgs);
	aos_assert_r(mCrtFile, false);

OmnScreen << "new file created..........filename: " << mCrtFileName << endl;
	return true;
}


