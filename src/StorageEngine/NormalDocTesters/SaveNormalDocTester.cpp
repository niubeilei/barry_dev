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
//
// Modification History:
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/NormalDocTesters/SaveNormalDocTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/ActImportDoc.h"
#include "DocClient/DocidMgr.h"
#include "DocClient/DocClient.h"
#include "Debug/Debug.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DocFileMgr/FileGroupMgr.h"
#include "DocFileMgr/StorageApp.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/TaskData.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageEngine/NormalDocTesters/Ptrs.h"
#include "StorageEngine/NormalDocTesters/SengTesterXmlDoc.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/VirtualFileObj.h"
#include "Thread/ThreadPool.h"
#include "VirtualFile/VirtualFile.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "Util1/Time.h"
#include "Util/ReadFile.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

OmnThreadPool sgThreadPool1("test");

map<u64, bool>   AosSaveNormalDocTester::smFlag;

AosSaveNormalDocTester::AosSaveNormalDocTester()
:
mStartDocid(0)
{
}


AosSaveNormalDocTester::~AosSaveNormalDocTester()
{
}


bool
AosSaveNormalDocTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		basicTest(mRundata);
	}
}

bool 
AosSaveNormalDocTester::signal(const int threadLogicId)
{
	return true;
}

bool
AosSaveNormalDocTester::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool 
AosSaveNormalDocTester::start()
{
	mRundata = OmnApp::getRundata()->clone(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);

	OmnThreadedObjPtr thisptr(this, false); 
	mThread = OmnNew OmnThread(thisptr, "tester", 0, false, true, __FILE__, __LINE__);
	mThread->start(); 
	return true;
}


bool
AosSaveNormalDocTester::basicTest(const AosRundataPtr &rdata)
{
	int num = 2;
	for (int i = 0; i < num; i++)
	{
		mFileId.push_back(0);
		mCreateFlag.push_back(false);
		mStartDocid = 0;
	}
	int cout = 0;
	while(1)
	{
		OmnScreen << "ZZZZZZZZZZZZZZZZZZZ: " << cout ++  << endl;
		for (int i = 0; i < num; i++)
		{
			OmnScreen << "===============: " << i << endl;
			mFileId[i] = 0;
			mCreateFlag[i] = false;
			//smFlag[i] = false;
			mStartDocid = 0;
			AosSaveNormalDocTesterPtr thisptr(this, true);
			OmnThrdShellProcPtr req = OmnNew ReadData(rdata, i, thisptr);
			sgThreadPool1.proc(req);
		}

		bool f;
		while(1)
		{
			f = true;
			for (u32 i = 0; i<mCreateFlag.size(); i++)
			{
				if (mCreateFlag[i] == false)  f = false;
			}
			if (f) break;
		}

		while(1)
		{
			f = true;
			map<u64, bool>::iterator iter;
			for (iter = smFlag.begin(); iter != smFlag.end(); iter ++)
			{
				bool flag = (*iter).second;
				if ( flag == false) f = false;
			}
			if (f) break;
		}
		//checkDoc(rdata);
	}
	return true;
}



void
AosSaveNormalDocTester::addReq(const int idx, const AosRundataPtr &rdata)
{
	u64 file_id = mFileId[idx];
	smFlag.insert(make_pair(file_id, false));
	mCreateFlag[idx] = true;

	OmnString records = "<records> <files>";
	int serverid = AosGetSelfServerId();
	//for (int idx = 0; idx < mFileId.size(); idx ++)
	//{
		records << "<" << AOSTAG_TASKDATA_TAGNAME << " "
			<< AOSTAG_PHYSICALID << "=\"" << serverid << "\" " 
			<< AOSTAG_SERVERID << "=\"" << serverid << "\" " 
			<< AOSTAG_SIZEID << "=\"" << -1 << "\" "
			<< AOSTAG_RECORD_LEN << "=\"" << -1 << "\" "
			<< AOSTAG_KEY << "=\"" << "docvariable" << "\" "
			<< AOSTAG_STORAGE_FILEID << "=\"" << mFileId[idx] << "\" "
			<< "/>";
	//}
	records << "</files></records>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(records, "" AosMemoryCheckerArgs);
	AosXmlTagPtr sdoc = root->getFirstChild();
	aos_assert(sdoc);

	AosActImportDocPtr ipdoc = OmnNew AosActImportDoc(false);
	aos_assert(ipdoc);
	AosTaskDataObjPtr pp;
	aos_assert(ipdoc->run(0, sdoc, pp, rdata));
}


void
AosSaveNormalDocTester::createData(const int idx, const AosRundataPtr &rdata)
{
	OmnLocalFilePtr file = 0;
	int64_t crt_last_offset = 0;
	while(1)
	{
		int size = rand() % eMaxReadSize;
		if (crt_last_offset + size > eMaxFileSize) break;
		AosBuffPtr buff = readyData(idx, size, rdata);
		//if (crt_last_offset + buff->dataLen() > eMaxFileSize) break;
		if (!file || crt_last_offset + buff->dataLen() > eMaxFileSize)
		{
			file = createFileLocked(idx, rdata);      
			aos_assert(file && file->isGood());
		}

OmnScreen << "write write write crt_last_offset: " << crt_last_offset << " , " << buff->dataLen() << endl;
		file->writelock();
		file->write(crt_last_offset, buff->data(), buff->dataLen());
		file->unlock();

		crt_last_offset += buff->dataLen();
	}
	file = 0;
	crt_last_offset = 0;
}


OmnLocalFilePtr
AosSaveNormalDocTester::createFileLocked(const int idx, const AosRundataPtr &rdata) 
{
	OmnString fname_prefix = "temp_";

	u64 file_id;
	u32 virtual_id = 0;
	OmnLocalFilePtr file = AosVfsMgrObj::getVfsMgr()->createRaidFile(rdata, virtual_id, file_id, fname_prefix, eMaxFileSize, false);
	//OmnLocalFilePtr file = AosVfsMgrObj::getVfsMgr()->createNewFile(rdata, virtual_id, file_id, fname_prefix, eMaxFileSize, false);
	mFileId[idx] = file_id;
	OmnScreen << "file_id: " << file_id << endl;
	return file;
}

AosBuffPtr
AosSaveNormalDocTester::readyData(const int idx, const int size, const AosRundataPtr &rdata)
{
	u32 bytes_to_read = 1000000;
	AosBuffPtr buff = OmnNew AosBuff(eMaxReadSize, 0 AosMemoryCheckerArgs);
	int record_len  = 0;
	while(1)
	{
		if (buff->dataLen() + record_len + sizeof(int) > size) break;
		//if (buff->dataLen() + record_len + sizeof(int) > eMaxReadSize) break;
		OmnString objid = "";
		u64 docid = AosDocidMgr::getSelf()->nextDocid(objid, rdata);
		if (mStartDocid == 0) mStartDocid = docid;
		//mDocids[idx].push_back(docid);
	//	OmnString str;
	//	str <<"<doc zky_docid=\"" << docid << "\" zky_a = 'kkksk' zky_siteid='100' zky_b= 'adfsdf' zky_c='kkjkeeluyledjkjsdkfjsldkjflsdjflskdjflsdjfls' zky_bse= 'ksjdfkllk' zky_d='dksio' zky_e='iojupo' zky_f='kioheohlslvkglslkidfosdf' zky_g='isdoohiwphjsllsiisopj' zky_h='908899' zky_i='sldjflsiejrlndflksdjflejrfwdjfsdjflsdjflsdjf'>slkjdssfsldfjowiefowpejskdhflsdjflwekjfsldjflsdjkfl;wejlsdjflsdfjlskdkfjsldfjwerwpojhefcoksndflsjdkflsjdflsjdflwejowjropwdsflkksdjflweirpooooopwerkjflsjkflskjvlsjkjljsdlfjsldjflsdjfplowkpnllkwlwwlkmwwlwpwokbnaaalsnmlvkpep</doc>";

		int depth = rand() % 4;
		AosTestXmlDoc xml("", depth);          

		OmnString str;
		str = xml.createNewDoc(docid);
		record_len = str.length();

//char *doc1 = (char *)str.data();
//AosXmlParser parser1;
//AosXmlTagPtr xml1 = parser1.parse(doc1, record_len, "" AosMemoryCheckerArgs);
//aos_assert_r(xml1, 0);

		buff->setU64(docid);
		buff->setInt(record_len);
		buff->setBuff(str.data(), record_len);
	}
	return buff;
}


bool
AosSaveNormalDocTester::checkDoc(const AosRundataPtr &rdata)
{
	int num = 0;
//	for (u32 i = 0; i< 10; i++)
//	{
//		if (mDocids[i].size() <= 0) continue;
//		
//		for (u64 j = 0; j< mDocids[i].size(); j++)
//		{
//			u64 docid = mDocids[i][j];
//			AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
//			if (doc)
//			{
//				aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, false);
//			}
//			else
//			{
//				OmnScreen << " docid: " << docid << endl;
//				aos_assert_r(doc, false);
//			}
//
//			if (num ++ %100 == 0) OmnScreen << "check Doc :" << docid << endl;
//		}
//	}
//	return true;

	OmnString objid = "";
	u64 docid = AosDocidMgr::getSelf()->nextDocid(objid, rdata);
	for (u64 i = mStartDocid; i< docid; i++)
	{
		AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(i, rdata);
		if (doc)
		{
			aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == i, false);
		}
		else
		{
			OmnScreen << " docid: " << i << endl;
			OmnAlarm << " kkljlsdjflksd : " << i << enderr;
		}
		if (num ++ %1000 == 0) OmnScreen << "check Doc :" << i << endl;
	}

	OmnScreen << "total: " << num << endl;
	return true;
}

