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
// 06/14/2013 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/Testers4/DirScannerTester.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/QueryReqObj.h"
#include "QueryClient/QueryClient.h"
#include "XmlUtil/SeXmlParser.h"
#include "Porting/Sleep.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("dirscanner_tester", __FILE__, __LINE__);

AosDirScannerTester::AosDirScannerTester()
{
	mName = "DirScannerTester";
	mIsFinished = false;
}


AosDirScannerTester::~AosDirScannerTester()
{
}


bool 
AosDirScannerTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosDirScannerTester::basicTest()
{
	AosRundataPtr rdata = OmnApp::getRundata();
	bool rslt = false;
	rslt = config(rdata);
	rslt = createDirScanner(rdata);
	rslt = test(rdata);
	return true;
}

bool
AosDirScannerTester::test(const AosRundataPtr &rdata)
{
	OmnThrdShellProcPtr runner;                     
	vector<OmnThrdShellProcPtr> test_runners; 
	for (u32 i=0; i<10; i++)
	{
		runner = OmnNew createBinaryDocThrd(this, rdata);
		test_runners.push_back(runner);
	}
	bool rslt = sgThreadPool->procSync(test_runners);
	aos_assert_r(rslt, false);
OmnScreen << "========================start check=====================" << endl;
	rslt = check(rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDirScannerTester::config(const AosRundataPtr &rdata)
{
	//<scanner zky_physicalid="0" zky_recursion="true">
	//	<dirs>
	//		<dir>xxxx</dir>
	//	</dirs>
	//	<binarydoc/>
	//</scanner>
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);
	AosXmlTagPtr scanner_conf = app_conf->getFirstChild("scanner");
	aos_assert_r(scanner_conf, false);
	AosXmlTagPtr dirs_tag = scanner_conf->getFirstChild("dirs");
	aos_assert_r(dirs_tag, false);
	AosXmlTagPtr dir_tag = dirs_tag->getFirstChild(true);
	OmnString dir_name;
	while(dir_tag)
	{
		dir_name = dir_tag->getNodeText();
		mDirNames.push_back(dir_name);
		dir_tag = dirs_tag->getNextChild();
	}
	aos_assert_r(mDirNames.size() > 0, false);
	mSep = scanner_conf->getAttrStr("zky_sep", "\n");
	mRecursion = scanner_conf->getAttrBool("zky_recursion", false);
	mPhysicalId = scanner_conf->getAttrInt("zky_physicalid", -1);
	mTemplateDoc = scanner_conf->getFirstChild(AOSTAG_BINARYDOC);
	if (!mTemplateDoc)
	{
		mTemplateDoc = AosXmlParser::parse("<binarydoc/>" AosMemoryCheckerArgs);
	}
	aos_assert_r(mTemplateDoc, false);
	mContainer = mTemplateDoc->getAttrStr("zky_pctrs");
	aos_assert_r(mContainer != "", false);
	OmnString ctnr_str;
	ctnr_str << "<container zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_objid=\"" << mContainer << "\" zky_siteid=\"100\" zky_otype=\"zky_ctnr\" zky_pctrs=\"_zt3e\" zky_crtor=\"50003\" zky_stype=\"zky_lostfnd\"/>";
	AosXmlTagPtr doc = AosXmlParser::parse(ctnr_str AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	bool rslt = AosModifyDoc(doc, rdata);
	aos_assert_r(rslt, false);
	mIsFinished = false;
	return true;
}

bool
AosDirScannerTester::createDirScanner(const AosRundataPtr &rdata)
{
	mScanner = AosDataScannerObj::createDirScannerStatic(mPhysicalId, mDirNames, mSep, mRecursion, rdata);
	return true;
}

bool
AosDirScannerTester::createBinaryDoc(const AosRundataPtr &rdata)
{
	AosBuffMetaDataPtr metaData = OmnNew AosBuffMetaData();
	bool rslt = mScanner->getNextBlockAndMetaData(metaData, rdata);
	aos_assert_r(rslt, false);
	if (!metaData || !metaData->getBuff() || metaData->getBuff()->dataLen() <= 0)
	{
		setFinished();
		return true;
	}
	AosXmlTagPtr doc = mTemplateDoc->clone(AosMemoryCheckerArgsBegin);
	doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	AosFileInfo info = metaData->getFileInfo();
	doc->setAttr("source_fname", info.mFileName);
	doc->setAttr(AOSTAG_BLOCKIDX, metaData->getCrtBlockIdx());
	AosXmlTagPtr binarydoc = AosCreateBinaryDoc(-1, 
			doc->toString(), true, metaData->getBuff(), rdata); 
	aos_assert_r(binarydoc, false);
OmnScreen << "====create binarydoc: " << doc->toString() << endl;
	return true;
}

bool
AosDirScannerTester::isFinished()
{
	mLock->lock();
	bool finished = mIsFinished;
	mLock->unlock();
	return finished;
}

bool
AosDirScannerTester::setFinished()
{
	mLock->lock();
	mIsFinished = true;
	mLock->unlock();
	return true;
}

bool
AosDirScannerTester::createBinaryDocThrd::run()
{
	bool rslt = false;
	while(!mTester->isFinished())
	{
		rslt = mTester->createBinaryDoc(mRundata);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosDirScannerTester::createBinaryDocThrd::procFinished()
{
	return true;
}

bool
AosDirScannerTester::check(const AosRundataPtr &rdata)
{
	vector<AosFileInfo> file_list;
	OmnString dir_name;
	for(u32 i=0; i<mDirNames.size(); i++)
	{
		dir_name = mDirNames[i];
		AosNetFileMgrObj::getFileListStatic(file_list, dir_name, mPhysicalId, -1, rdata);
	}

	OmnString fileName;
	int64_t	fileLen;
	bool rslt = false;
	OmnString doc_md5;
	OmnString file_md5;
	for (u32 i=0; i<file_list.size(); i++)
	{
		fileName = file_list[i].mFileName;
		fileLen = file_list[i].mFileLen;
OmnScreen << "=============check file fileName: " << fileName << " fileLength: " << fileLen << endl;

		map<OmnString, u64> docids = getBinaryDocids(fileName, rdata);
		aos_assert_r(docids.size() > 0, false);
		AosBuffPtr doc_binary;
		rslt = puttingTogether(doc_binary, docids, rdata);
OmnScreen << "============block size: " << docids.size() << endl;
		AosBuffPtr file_binary;
		AosNetFileMgrObj::readFileToBuffStatic(file_binary, fileName, mPhysicalId, 0, (u32)fileLen, rdata);
		aos_assert_r(doc_binary->dataLen() == fileLen, false);
		aos_assert_r(file_binary->dataLen() == fileLen, false);
		aos_assert_r(doc_binary->dataLen() == file_binary->dataLen(), false);
		OmnString docstr(doc_binary->data(), doc_binary->dataLen());
		doc_md5 = AosMD5Encrypt(docstr);

		OmnString filestr(file_binary->data(), file_binary->dataLen());
		file_md5 = AosMD5Encrypt(filestr);
OmnScreen << "=======doc_fd5: " << doc_md5 << " file_md5: " << file_md5 << endl;
		aos_assert_r(doc_md5 == file_md5, false);
	}
	return true;
}

bool
AosDirScannerTester::puttingTogether(
		AosBuffPtr &buff,
		map<OmnString, u64> &docids,
		const AosRundataPtr &rdata)
{
	buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	for (u32 i=0; i<docids.size(); i++)
	{
		OmnString idx_str;
		idx_str << i;
		AosBuffPtr binary;
		AosXmlTagPtr tmpdoc = AosRetrieveBinaryDoc(docids[idx_str], binary, rdata);
		aos_assert_r(tmpdoc && binary, false);
		buff->appendBuff(binary);
	}
	aos_assert_r(buff->dataLen() > 0, false);
	return true;
}

map<OmnString, u64>
AosDirScannerTester::getBinaryDocids(
					const OmnString &source_name,
					const AosRundataPtr &rdata)
{
	map<OmnString, u64> docids;
	bool finished = true;
	int startIdx = 0;
	while(finished)
	{
		AosXmlTagPtr contents = queryBinaryDoc(startIdx, source_name, rdata);
		aos_assert_r(contents, docids);
		int total = contents->getAttrInt("total", 0);     
		int crt_idx = contents->getAttrInt("crt_idx", 0); 
		int num = contents->getAttrInt("num", 0);         
		if (num > 0)                                      
		{                                                 
			AosXmlTagPtr record = contents->getFirstChild(true);
			while(record)
			{
				docids[record->getAttrStr("zky_block_idx")] = record->getAttrU64(AOSTAG_DOCID, 0);
				record = contents->getNextChild();
			}
		}                                                 
		if (crt_idx + num >= total)                       
		{                                                 
			finished = false;                             
		}                                                 
		startIdx = crt_idx + num;                         
	}
	return docids;
}

AosXmlTagPtr
AosDirScannerTester::queryBinaryDoc(
				const int startIdx,
				const OmnString &source_name, 
				const AosRundataPtr &rdata)
{
	OmnString queryStr = "";
	queryStr << "<cmd psize=\"100\" order=\"zky_block_idx\" reverse=\"false\" opr=\"retlist\" start_idx=\"" << startIdx << "\"><conds><cond type=\"AND\"><cond type=\"arith\"><selector type=\"attr\" aname=\"zky_pctrs\"/><cond type=\"arith\" zky_opr=\"eq\"><![CDATA[" << mContainer << "]]></cond></cond><cond type=\"arith\"><selector type=\"attr\" aname=\"source_fname\"/><cond type=\"arith\" zky_opr=\"eq\"><![CDATA[" << source_name << "]]></cond></cond></cond></conds><fnames><fname type=\"x\"><cname><![CDATA[xml]]></cname><oname><![CDATA[xml]]></oname></fname></fnames></cmd>";
	//queryStr << "<cmd psize=\"50\" order=\"zky_block_idx\" reverse=\"true\" opr=\"retlist\" start_idx=\"" << startIdx << "\"><conds><cond type=\"AND\"><cond type=\"arith\"><selector type=\"attr\" aname=\"zky_pctrs\"/><cond type=\"arith\" zky_opr=\"eq\"><![CDATA[" << mContainer << "]]></cond></cond></cond></conds><fnames><fname type=\"x\"><cname><![CDATA[xml]]></cname><oname><![CDATA[xml]]></oname></fname></fnames></cmd>";
	AosXmlTagPtr cmd = AosXmlParser::parse(queryStr AosMemoryCheckerArgs);
	aos_assert_r(cmd, 0);

	AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(cmd, rdata);
	aos_assert_r(query && query->isGood(), 0);

	OmnString strData;
	bool rslt = query->procPublic(cmd, strData, rdata);
	aos_assert_r(rslt, 0);

	AosXmlTagPtr contents = AosXmlParser::parse(strData AosMemoryCheckerArgs);
	aos_assert_r(contents, 0);

	return contents;
}
