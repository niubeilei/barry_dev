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
#include "DataSplitter/Jimos/Tester/DataSplitterDirTester.h"

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
#include "SEInterfaces/UserDomainObj.h"
#include <list>
#include "DataSplitter/Jimos/DirSplitFiles.h"
#include "DataCube/Jimos/DataCubeDir.h"
#include "SEInterfaces/DataSplitterObj.h"

AosDataSplitterDirTester::AosDataSplitterDirTester()
:
mReqid(0),
mIsFinished(false)
{
}


AosDataSplitterDirTester::~AosDataSplitterDirTester()
{
}


bool 
AosDataSplitterDirTester::start()
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
AosDataSplitterDirTester::basicTest(const AosRundataPtr &rdata)
{
	// construct worker_doc and jimo_doc
	OmnString ss = "<data_cube read_block_size=\"1000\" zky_character=\"UTF8\">";
	ss << "<split zky_libname=\"libDataSplitter.so\" jimo_type=\"jimo_datasplitter\" jimo_name=\"jimo_datasplitter\" zky_otype=\"zkyotp_jimo\" zky_objid=\"youngtest\" jimo_objid=\"young_test_datespliter\" zky_classname=\"AosDirSplitSubdir\" current_version=\"1.0\">"
			<< "<versions current_version=\"1.0\">"
				<< "<ver_0 version=\"1.0\">libDataSplitter.so</ver_0>"
			<< "</versions>"	
		<< "</split>"
		<< "<dirs>"
			<< "<dir dir_name=\"/home/young/codes\" physical_id=\"0\" />"
			<< "<dir dir_name=\"/home/young/NoteBook\" physical_id=\"0\" />"
		<< "</dirs>"
	<< "</data_cube>";

	AosXmlTagPtr worker_doc = AosStr2Xml(rdata, ss AosMemoryCheckerArgs);
	aos_assert_r(worker_doc, false);

	AosXmlTagPtr split_xml = worker_doc->getFirstChild("split");
	aos_assert_r(split_xml, false);
	AosDataSplitterObjPtr datasplit = AosCreateDataSplitter(rdata, split_xml);
	aos_assert_r(datasplit, false);
	datasplit->config(rdata, worker_doc);

	vector<AosXmlTagPtr> v;
	datasplit->split(rdata, v);
	for (u32 i=0; i<v.size(); i++)
	{
		AosXmlTagPtr conf = v[i];
		dataCubeTest(rdata, conf);
	}

	return true;
}


bool
AosDataSplitterDirTester::dataCubeTest(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	mIsFinished = false;
	aos_assert_r(worker_doc, false);
	// 1. create jimo
	OmnScreen << "\n OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\nJimo created--------------------" << endl;
	mCubeDir = AosCreateDataCube(rdata, worker_doc);
	AosBuffDataPtr buff_data = OmnNew AosBuffData();
	mCubeDir->setCaller(this);

	mCubeDir->readData(mReqid, rdata);
	mReqid++;
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
AosDataSplitterDirTester::callBack(
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
		mCubeDir->readData(mReqid, mRundata);
		mReqid++;
	}
}

bool
AosDataSplitterDirTester::writeDataIntoFile(
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
AosDataSplitterDirTester::createNewFile(
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


