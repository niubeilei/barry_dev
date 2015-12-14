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
//  This class is used to create docs. It will:
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
//  Important:
//  	1. collect word ignore attribute and word that is end of __n, 
//  	2. collect attribute ignore metadata in sgMetaAttrs.
//  	3. member of should no have globle index
//
//  Error:
//  	1. the word and attribute exsit in the doc more than twice.
//  	   if somebody modify the attrubite , it's may be remove the 
//  	   word in the attribtue. but the word also in the anther attribut.
//  	   when somebody remove the attribute, it should be remove the word.
//  	   so the error happend.
//  NotImplement:
//      1. detete the container.
//      2. modify control indexing attributes.
//
// Modification History:
// 07/19/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClient/DocProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocUtil/DocUtil.h"
#include "DocUtil/SengUtil.h"
#include "DocUtil/DocProcUtil.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEUtil/IILName.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocReq.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEModules/ObjMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/CondVar.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "VersionServer/VersionServer.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordNorm.h"
#include "WordMgr/WordIdHash.h"
#include "WordParser/WordParser.h"
#include "TransBasic/Trans.h"
#include "XmlUtil/SeXmlUtil.h"
#include <set>



OmnSingletonImpl(AosDocProcSingleton,
                 AosDocProc,
                 AosDocProcSelf,
                "AosDocProc");



bool AosDocProc::smShowLog = true;
AosIILClientObjPtr AosDocProc::smIILClient;

static const int sgMaxMetaAttrs = 50;
// static int sgNumMetaAttrs = 0;
// static AosDocMetaAttr sgMetaAttrs[sgMaxMetaAttrs];

int AosDocProc::smMaxOutstandingMsgs = 500;

AosDocProc::AosDocProc()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mNumReqs(0),
mMaxWordLen(eDftMaxWordLen),
mIsRepairing(false),
mTotalAccepted(0),
mTotalProcessed(0),
mNumThreads(eDftNumThreads)
{
}


AosDocProc::~AosDocProc()
{
}


bool
AosDocProc::start()
{
	// Ketty 2012/01/12
	for (int i=0; i<mNumThreads; i++)
	{
		mStatus[i] = false;
		mThreads[i]->start();
	}
	return true;
}


bool
AosDocProc::config(const AosXmlTagPtr &config)
{
	// Chen Ding, 02/15/2012
	smIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(smIILClient, false);

	aos_assert_r(config, false);
	AosXmlTagPtr conf = config->getFirstChild("DocProc");
	aos_assert_r(conf, false);

	// Start all the processing threads
	mNumThreads = conf->getAttrInt("num_threads", eDftNumThreads);
	smMaxOutstandingMsgs = conf->getAttrInt("num_reqs_buffered", 500);
	if (smMaxOutstandingMsgs < 10) smMaxOutstandingMsgs = 500;
	if (mNumThreads >= eMaxThreads || mNumThreads < 1) mNumThreads = eDftNumThreads;
	OmnThreadedObjPtr thisPtr(this, false);
	for (int i=0; i<mNumThreads; i++)
	{
		OmnString name = "docproc_";
		name << i;
		mThreads[i] = OmnNew OmnThread(thisPtr, name, i, true, true, __FILE__, __LINE__);
		//mThreads[i]->start();
	}

	// Start the health check thread

	OmnString dirname = config->getAttrStr(AOSCONFIG_DIRNAME);
	mWordNormName = conf->getAttrStr(AOSCONFIG_WORDNORM_HASHNAME);

	// Chen Ding, 10/01/2011
	OmnString ignoredname = conf->getAttrStr(AOSCONFIG_IGNORED_HASHNAME);
	// Chen Ding, 01/28/2012
	// for (int i=0; i<eMaxThreads; i++)
	for (int i=0; i<mNumThreads; i++)
	{
		mWordNorm[i] = OmnNew AosWordNorm(dirname, mWordNormName);
		mWordParser[i] = OmnNew AosWordParser();
		// Ketty 2012/11/05
		/*
		// Ketty 2012/08/01
		//mIgnoredWords[i] = OmnNew AosWordIdHash(dirname, eIgnoredWordSize, ignoredname);
		mIgnoredWords[i] = OmnNew AosWordIdHash(eIgnoredWordSize, ignoredname);
		aos_assert_r(mIgnoredWords[i]->createHashtableNew(), false);
		*/
	}
	mIsRepairing = AosDocClientObj::getDocClient()->isRepairing();

	smShowLog = conf->getAttrBool(AOSCONFIG_SHOWLOG, false);
	AosXmlUtil::setMetaAttrs();
	
	//OmnString doc_transidfname = conf->getAttrStr(AOSCONFIG_DOCTRANSID_FNAME);
	//OmnString fname;
	//if(fname.find('/', true) == fname.length()-1 || fname == "") 
	//{ 
	//	    fname << doc_transidfname; 
	//} 
	//else 
	//{ 
	//	    fname << "/" << doc_transidfname;
	//} 
	//bool rslt = openDocTransIdFile(fname);
	//aos_assert_r(rslt, false);
	return true;
}


bool
AosDocProc::stop()
{
	OmnScreen << "To stop WordMgr!" << endl;
	// AosWordTransClient::getSelf()->stop();
    return true;
}

//felicia, 2013/06/19
/*
bool    
AosDocProc::signal(const int threadLogicId)
{
	return true;
}


bool    
AosDocProc::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}
*/

bool    
AosDocProc::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->broadcastSignal();
	mLock->unlock();
	return true;
}


bool    
AosDocProc::checkThread111(OmnString &err, const int thrdLogicId)
{
	aos_assert_r(thrdLogicId < mNumThreads, false);
	bool threadStatus;
	
	mLock->lock();
	threadStatus = mStatus[thrdLogicId];
	mStatus[thrdLogicId] = false;
	mLock->unlock();

	return threadStatus;
}


static u64 lsLastTimestamp = OmnGetTimestamp();
static u64 lsStartTimestamp = lsLastTimestamp;
static u64 lsDurationTimestamp = lsLastTimestamp;
static OmnString lsSpeedHistory;

bool
AosDocProc::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosDocReqPtr request;
	const int procTimer = 3000;
	u32 thread_id = thread->getLogicId();
    while (state == OmnThrdStatus::eActive)
    {
		mLock->lock();
		if (mNumReqs == 0)
		{
			// There is no more cached entries. Start a timer to 
			// wake this thread up later. 
			mCondVar->wait(mLock);
			mStatus[thread_id] = true;
			mLock->unlock();
			continue;
		}

		if (!mTail)
		{
			OmnAlarm << "Tail is null: " << mNumReqs << enderr;
			mCondVar->wait(mLock);
			mStatus[thread_id] = true;
			mLock->unlock();
			continue;
		}

		mStatus[thread_id] = true;

		// There are something to be processed. Get the first entry
		// from the list, and then release the lock.
		request = mTail;
		mTail = request->prev;
		if (mTail) mTail->next = 0;

		// Chen Ding, 10/15/2011
		mNumReqs--;

		// Chen Ding, 10/02/2011
		// Chen Ding, 10/15/2011
		// if (mNumReqs == smMaxOutstandingMsgs - eMaxThreads)
		if (mNumReqs > 0)
		{
			// Need to signal the ones who are waiting for the queue
			// to be 'not empty'.
			mCondVar->signal();
		}

		if (mNumReqs == 0)
		{
			mHead = 0;
			mTail = 0;
		}

		//OmnScreen << "Processing: " << ++mTotalProcessed 
		//	<< ":" << mTotalAccepted << ":" << mNumReqs << endl;
		mTotalProcessed++;
		if (mTotalProcessed && (mTotalProcessed % 100 == 0))
		{

			u64 tt = OmnGetTimestamp();
			OmnString str;
			str << "Processing: " << mTotalProcessed 
				<< ":" << mTotalAccepted << ":" << mNumReqs 
				<< ". Speed: " << (tt - lsLastTimestamp)/1000
				<< ". Time: " << (tt - lsDurationTimestamp)/1000000 
				<< ":" << (tt - lsStartTimestamp)/1000000
				<< ". Rate: " << (tt - lsStartTimestamp)/mTotalProcessed;

			if (lsSpeedHistory != "") str << ". History: " << lsSpeedHistory;
			OmnScreen << str << endl;

			if ((mTotalProcessed % 10000) == 0)
			{
				if (lsSpeedHistory != "") lsSpeedHistory << ",";
				lsSpeedHistory << (tt - lsStartTimestamp)/1000000;
				lsStartTimestamp = tt;
			}
			lsLastTimestamp = tt;
		}
		
		request->next = 0;
		request->prev = 0;
		mLock->unlock();

		switch (request->reqtype)
		{
		case AosDocReq::eCreateDoc:
			 AosThreadRegisterAction(procTimer, "createdoc");
			 createDocPriv(request);
			 AosThreadUnregisterAction("createdoc");
			 break;
		
		case AosDocReq::eCreateLog:
			 AosThreadRegisterAction(procTimer, "createlog");
			 createLogPriv(request);
			 AosThreadUnregisterAction("createlog");
			 break;

		case AosDocReq::eModifyDoc:
			 AosThreadRegisterAction(procTimer, "moddoc");
			 modifyDocPriv(request);
			 AosThreadUnregisterAction("moddoc");
			 break;

		case AosDocReq::eDeleteDoc:
			 AosThreadRegisterAction(procTimer, "deldoc");
			 deleteDocPriv(request);
			 AosThreadUnregisterAction("deldoc");
			 break;

		case AosDocReq::eDeleteLog:
			 AosThreadRegisterAction(procTimer, "dellog");
			 deleteLogPriv(request);
			 AosThreadUnregisterAction("dellog");
			 break;
		
		case AosDocReq::eModifyAttrStr:
			 AosThreadRegisterAction(procTimer, "modattr");
			 modifyAttrStrPriv(request);
			 AosThreadUnregisterAction("modattr");
			 break;

		default:
			 OmnAlarm << "Unrecognized doc request type: " << request->reqtype << enderr;
		}
		
		// Need to finish the transaction                
		//AosBigTransPtr trans = request->getTrans();         
		//if (trans)
		//{
		//	AosDocClientObj::getDocClient()->docTransFinished(trans);
		//}
		AosXmlTagPtr root = request->root;
		request = 0;
		root = 0;
	}

	return true;
}


bool
AosDocProc::createDocPriv(const AosDocReqPtr &request)
{
	aos_assert_r(smIILClient, false);
	// OmnString logs;
	// OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread(logs);
	AosXmlTagPtr doc = request->doc;
	aos_assert_r(doc, false);
	doc->setReadOnly(true);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	AosRundataPtr rdata = request->getRundata();

	if (smShowLog) 
	{
		OmnScreen << "Doc Created: " << doc->getDataLength() << ":" 
			<< doc.getPtr() << ":" << doc->toString() << endl;
	}

	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	OmnString siteid = doc->getAttrStr(AOSTAG_SITEID);
	aos_assert_r(siteid != "", false);
	AosXmlTagPtr parent_doc;

	// Chen Ding, 10/01/2011
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);

	//mDocTransId[tid] = request->getDocTransid();
	//AOSIILTRANSTEST_ADDDOC("create", mDocTransId[tid], doc);	// Ketty 2012/02/01 

	// Ketty 2013/02/26
	// Chen Ding, 10/09/2011
	int num_phs = AosGetNumPhysicals();
	//vector<AosBuffPtr> buffs(num_phs);
	//vector<AosBuffPtr> buffs(AosGetNumCubes());
	//createBuffs(buffs);
	vector<AosTransPtr>  allTrans[num_phs];

	OmnString parent_objid = doc->getAttrStr(AOSTAG_PARENTC);
	if (parent_objid != "")
	{
		parent_doc = AosDocClientObj::getDocClient()->getDocByObjid(parent_objid, rdata);
		if (parent_doc) parent_doc->setRCObjInUse(eAosRCObjClassId_XmlTag);
	}

	u64 parent_docid = 0;
	if (!parent_doc)
	{
		if (!mIsRepairing)
		{
			AosSetError(rdata, AosErrmsgId::eMissingParentContainer);
			OmnAlarm << rdata->getErrmsg() 
				<< ". Doc: " << doc->getAttrStr(AOSTAG_OBJID) << enderr;
			return false;
		}
	}
	else
	{
		parent_docid = parent_doc->getAttrU64(AOSTAG_DOCID, 0);
	}
	
	// Collect words form xml , it has words in tag name ,attrbute name, 
	// attribute value and node text.
	// 1. Add all the words in mWordHash[tid].
	AosEntryMark::E mark, marknull;

	mWordHashNew[tid].reset();
	//bool rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], parent_doc, doc, false);
	bool rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], parent_doc, doc, false);
	if (!rslt)
	{
		OmnAlarm << "Failed collecting words!" << enderr;
		if (parent_doc) parent_doc->setRCObjInUse(false);
		return false;
	}

	OmnString word;
	//mWordHashNew[tid].resetWordLoop();
	int guard = eMaxWordsPerDoc;
	int num_words_proc = 0;
	//int lastWordTimestamp = OmnGetTimestamp();
	while (guard-- && mWordHashNew[tid].nextEntry(word, mark))
	{
		num_words_proc++;
		addWordIIL(allTrans, num_phs, word, docid, parent_objid, mark, rdata, __LINE__);
	}
	if (guard <= 0)
	{
		OmnAlarm << "Serious problem: " << eMaxWordsPerDoc << enderr;
		if (parent_doc) parent_doc->setRCObjInUse(false);
		return false;
	}

	OmnString ssss;
	ssss << num_words_proc;

	// 2. Add attributes: "name=value"
	mAttrHashNew[tid].reset();
	rslt = AosDocProcUtil::collectAttrs(mAttrHashNew[tid], parent_doc, doc, true);
	if (!rslt)
	{
		OmnAlarm << "Failed collecting attributes: " << doc->toString() << enderr;
		if (parent_doc) parent_doc->setRCObjInUse(false);
		return false;
	}
	OmnString name, value;
	guard = eMaxWordsPerDoc;
	num_words_proc = 0;
	// lastWordTimestamp = OmnGetTimestamp();
	mAttrHashNew[tid].resetWordLoop();
	while (guard-- && mAttrHashNew[tid].nextEntry(name, value, mark))
	{
		num_words_proc++;
		if (value.length() <= 0)
		{
			OmnAlarm << "Value is empty: " << name << enderr;
		}
		else
		{
	    	addAttrIIL(allTrans, num_phs, name, value, docid, parent_objid, mark, rdata);
		}

		// Chen Ding, 11/13/2011
		// Next need to process attribute word indexing. Whether it needs to 
		// do attribute word indexing is controlled by 'mark'. 
		// The following is incorrect!!!!!!!!!!!!!
		//if (name == AOSTAG_TAG)
		
		//OmnScreen << "mark:" << mark
		//		<< "; " << endl;
		if (mark & AosEntryMark::eAttrWordIndex)
		{
			mWordHashNew[tid].reset();
			//rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], value);
			rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], value);
			aos_assert_r(rslt, false);
			while(mWordHashNew[tid].nextEntry(word, marknull))
			{
				addAttrWordIIL(allTrans, num_phs, name, word, docid, parent_objid, mark, rdata);
			}
		}
	}
	if (guard <= 0)
	{
		OmnAlarm << "Too many attributes: " << eMaxWordsPerDoc 
			<< ". Doc: " << doc->toString() << enderr;
		if (parent_doc) parent_doc->setRCObjInUse(false);
		return false;
	}

	ssss = "";
	ssss << num_words_proc;

	// 3 Add system infomation to iil
	bool isCtnr = doc->getAttrStr(AOSTAG_OTYPE,"") == AOSOTYPE_CONTAINER;
	u64 epoch = doc->getAttrU64(AOSTAG_CT_EPOCH, 0);
	if (epoch == 0)
	{
		// The doc does not have epoch time. If it is not repairing, 
		// need to raise alarm.
		if (!mIsRepairing)
		{
			OmnAlarm << "Doc missging creation Epoch time: " 
				<< doc->toString() << enderr;
		}
		epoch = OmnGetSecond();
		doc->setReadOnly(false);
		doc->setAttr(AOSTAG_CT_EPOCH, epoch);
		doc->setReadOnly(true);
	}
	addSysInfoIIL(allTrans, num_phs, docid, objid, parent_docid, parent_objid, epoch, rdata, isCtnr);

	// 4 add meta data
	AosDocProcUtil::addMetaAttrs(allTrans, num_phs, doc, docid, rdata);

	// 5 add memberof 
	OmnString memberof = doc->getAttrStr(AOSTAG_MEMBEROF);
	OmnStrParser1 parser(memberof, ", ", false, false);       
	OmnString ctnr_objid;
	while ((ctnr_objid = parser.nextWord()) != "")
	{
		// Chen Ding, 11/13/2011
		// Adding member_of is to add 'objid' to the parent container
		// 'ctnr_objid'. Need to check whether the container allows 
		// its member listing. 
		// AosXmlTagPtr memberof_doc = AosRetrieveDocByObjid(ctnr_objid, rdata);
		// addMemberOf(buffs, doc, memberof_doc, part, rdata);
		if (AosSecurityMgrObj::getSecurityMgr()->checkMemberOfListing(ctnr_objid, parent_objid, rdata))
		{
			// It is allowed.
			OmnString iilname = AosIILName::composeCtnrMemberObjidIILName(ctnr_objid);
			bool rslt = AosDocProcUtil::addValueToIIL(allTrans, num_phs, iilname, objid, docid, false, false, rdata);
			if (!rslt)
			{
				OmnAlarm << "Failed adding member: " << ctnr_objid
					<< ":" << objid << ":" << docid << ". Error: " << rdata->getErrmsg()
					<< enderr;
			}
		}
	}

	// Finished the processing
	if (request->caller)
	{
		request->caller->procFinished(true, docid, "", request->userdata, 0);
	}

	// Ketty 2013/02/22
	//smIILClient->sendTrans(request->task_trans, buffs, docid, rdata);
	sendTrans(rdata, allTrans, num_phs);
	doc->setReadOnly(false);
	if (parent_doc) parent_doc->resetRCObjInUse();
	return true;
}


bool
AosDocProc::createLogPriv(const AosDocReqPtr &request)
{
	// 	<container ...>
	// 		<AOSTAG_LOG_DEFINITION>zky_log_def
	// 			<AOSTAG_LOG_INDEX>zky_log_index
	// 				.....
	// 			</AOSTAG_LOG_INDEX>
	// 			<AOSTAG_LOG_MANAGE>zky_log_manage
	// 				<AOSTAG_DFT_LOGNAME 
	// 					AOSTAG_AGING_TYPE="agecap"
	// 					max_filesize="50000000"
	// 					max_files="6"/>
	//				<test_1 
	// 					AOSTAG_AGING_TYPE="agecap"
	// 					max_filesize="20000000"
	// 					max_files="1"/>
	//				<test_2 
	// 					AOSTAG_AGING_TYPE="agecap"
	// 					max_filesize="600"
	// 					max_files="100"/>
	//			</AOSTAG_LOG_MANAGE>
	// 		</AOSTAG_LOG_DEFINITION>
	// 	</container>
	// OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	aos_assert_r(smIILClient, false);
	AosXmlTagPtr doc = request->doc;
	aos_assert_r(doc, false);
	doc->setReadOnly(true);
	u64 logid = doc->getAttrU64(AOSTAG_LOGID, 0);
	AosRundataPtr rdata = request->getRundata();

	if (smShowLog) 
	{
		OmnScreen << "log Created: " << doc->getDataLength() << ":" 
			<< doc.getPtr() << ":" << doc->toString() << endl;
	}

	AosXmlTagPtr parent_doc;

	// Chen Ding, 10/01/2011
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);

	//mDocTransId[tid] = request->getDocTransid();
	//AOSIILTRANSTEST_ADDDOC("createlog", mDocTransId[tid], doc);	// Ketty 2012/02/01

	// Ketty 2013/02/26
	// Chen Ding, 10/09/2011
	int num_phs = AosGetNumPhysicals();
	//vector<AosBuffPtr> buffs(num_phs);
	//vector<AosBuffPtr> buffs(AosGetNumCubes());
	//createBuffs(buffs);
	vector<AosTransPtr>  allTrans[num_phs];

	OmnString parent_objid = doc->getAttrStr(AOSTAG_LOG_CTNROBJID);

//ken 2011/10/25
if(parent_objid == "")return false;
	
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);
	parent_doc = docclient->getDocByObjid(parent_objid, rdata);
	// u64 parent_docid = 0;
	if (!parent_doc)
	{
		if (!mIsRepairing)
		{
			AosSetError(rdata, AosErrmsgId::eMissingParentContainer);
			OmnAlarm << rdata->getErrmsg() 
				<< ". Doc: " << doc->getAttrStr(AOSTAG_OBJID) << enderr;
			return false;
		}
	}
	else
	{
		//parent_docid = parent_doc->getAttrU64(AOSTAG_DOCID, 0);
	}
	
	// Collect words form xml , it has words in tag name ,attrbute name, 
	// attribute value and node text.
	// 1. Add all the words in mWordHash[tid].
	AosEntryMark::E mark, marknull;

	mWordHashNew[tid].reset();
	
	//Log's configuration is in container.
	///////////////////////////////////////////////////////////////////////
	AosXmlTagPtr logdef = parent_doc->getFirstChild(AOSTAG_LOG_DEFINITION);
	if (!logdef) return true;
	AosXmlTagPtr idxdef = logdef->getFirstChild(AOSTAG_LOG_INDEX);
	if (!idxdef) return true;
	///////////////////////////////////////////////////////////////////////
	
	//bool rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], idxdef, doc, false);
	bool rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], idxdef, doc, false);
	aos_assert_r(rslt, false);
	OmnString word;
	//mWordHashNew[tid].resetWordLoop();
	int guard = eMaxWordsPerDoc;
	int num_words_proc = 0;
	int lastWordTimestamp = OmnGetTimestamp();
	while (guard-- && mWordHashNew[tid].nextEntry(word, mark))
	{
		num_words_proc++;
		addWordIIL(allTrans, num_phs, word, logid, parent_objid, mark, rdata, __LINE__);
	}
	aos_assert_r(guard, false);
	OmnString ssss;
	ssss << num_words_proc;

	// 2. Add attributes: "name=value"
	mAttrHashNew[tid].reset();
	rslt = AosDocProcUtil::collectAttrs(mAttrHashNew[tid], idxdef, doc, true);
	aos_assert_r(rslt, false);
	OmnString name, value;
	guard = eMaxWordsPerDoc;
	num_words_proc = 0;
	lastWordTimestamp = OmnGetTimestamp();
	while (guard-- && mAttrHashNew[tid].nextEntry(name, value, mark))
	{
		num_words_proc++;
		if ((num_words_proc % 20) == 0)
		{
			u64 tt = OmnGetTimestamp();
			OmnString ss;
			ss << tt - lastWordTimestamp << ":" << tt << ":" << lastWordTimestamp;
			lastWordTimestamp = tt;
		}
	    addAttrIIL(allTrans, num_phs, name, value, logid, parent_objid, mark, rdata);

		mWordHashNew[tid].reset();
		//rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], value);
		rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], value);
		aos_assert_r(rslt, false);
		while(mWordHashNew[tid].nextEntry(word, marknull))
		{
	    	addAttrWordIIL(allTrans, num_phs, name, word, logid, parent_objid, mark, rdata);
		}
	}
	aos_assert_r(guard, false);
	ssss = "";
	ssss << num_words_proc;

	//Jozhi 2013/03/25
	//AosDocProcUtil::addMetaAttrs(buffs, doc, logid, rdata);
	AosDocProcUtil::addMetaAttrs(allTrans, num_phs, doc, logid, rdata);

	// Finished the processing
	if (request->caller)
	{
		request->caller->procFinished(true, logid, "", 
			request->userdata, 0);
	}

	// Ketty 2013/02/22
	//smIILClient->sendTrans(request->task_trans, buffs, logid, rdata);
	sendTrans(rdata, allTrans, num_phs);
	doc->setReadOnly(false);
	return true;
}


bool
AosDocProc::modifyDocPriv(const AosDocReqPtr &request)
{
	// It is to modify an existing doc. The existing doc is in 'origdocroot'
	// and the new doc is in 'newxml'. 
	AosXmlTagPtr origdocroot = request->origdocroot;
	AosXmlTagPtr newxml = request->doc;
	aos_assert_r(newxml, false);

	// Ken Lee, 2013/04/25
	newxml = newxml->clone(AosMemoryCheckerArgsBegin);

	AosRundataPtr rdata = request->getRundata();
	if (request->synobj)
	{
		u64 docid = newxml->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
		OmnString objid = newxml->getAttrStr(AOSTAG_OBJID);
		aos_assert_r(docid != AOS_INVDID, false);

		AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
		aos_assert_r(docclient, false);
		origdocroot = docclient->getDocByDocid(docid, rdata);
	}
	AosXmlTagPtr origxml;
	if (origdocroot) origxml = origdocroot->getFirstChild();

	return modifyDocPriv(origxml, newxml, request, rdata);
}


bool
AosDocProc::modifyDocPriv(
		const AosXmlTagPtr &origxml,
		const AosXmlTagPtr &newxml, 
		const AosDocReqPtr &request,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(smIILClient, false);
	aos_assert_r(origxml, false);
	aos_assert_r(newxml, false);
	newxml->setReadOnly(true);

	// Chen Ding, 10/01/2011
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);
	
	//mDocTransId[tid] = request->getDocTransid();
	// AOSIILTRANSTEST_ADDDOC("modify", mDocTransId[tid], newxml);	// Ketty 2012/02/01
	if (smShowLog)
	{
		//OmnScreen << "Modify: " << origxml->toString() << "\n"
		//<< "newDoc: " << newxml->toString() << endl;
		OmnScreen << "Modify: " << origxml->toString() << endl;
		OmnScreen << "newDoc: " << newxml->toString() << endl;
	}

	OmnString newobjid = newxml->getAttrStr(AOSTAG_OBJID);
	OmnString oldobjid = (origxml)?origxml->getAttrStr(AOSTAG_OBJID):"";
	bool objidChanged = (newobjid != oldobjid);
	bool exist;
	u64 docid = newxml->getAttrU64(AOSTAG_DOCID, AOS_INVDID, exist);
	aos_assert_r(docid != AOS_INVDID, false);

	// 1. zky_objid has changed and the zky_otype is container
	bool isCtnr = newxml->getAttrStr(AOSTAG_OTYPE,"") == AOSOTYPE_CONTAINER;
	if (objidChanged && isCtnr) 
	{
		// The doc is container
		// Container name has changed. need remove old one.
		removeContainer(origxml, rdata);
	}

	OmnString oldctnr_objid = origxml->getAttrStr(AOSTAG_PARENTC);
	OmnString newctnr_objid = newxml->getAttrStr(AOSTAG_PARENTC);

	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);
	AosXmlTagPtr parent_doc = docclient->getDocByObjid(newctnr_objid, rdata);
	aos_assert_r(parent_doc, false);
	AosXmlTagPtr oldparent_doc = parent_doc;
	u64 newctnr_docid = parent_doc->getAttrU64(AOSTAG_DOCID, 0);

	// Ketty 2013/02/26
	// Chen Ding, 10/09/2011
	int num_phs = AosGetNumPhysicals();
	//vector<AosBuffPtr> buffs(num_phs);
	//vector<AosBuffPtr> buffs(AosGetNumCubes());
	//createBuffs(buffs);
	vector<AosTransPtr>  allTrans[num_phs];

	// 2 parent container
	bool parentCtnrChanged = false;

	if (oldctnr_objid != newctnr_objid)
	{
		parentCtnrChanged = true;
		oldparent_doc = docclient->getDocByObjid(oldctnr_objid, rdata);
		// Doc's parent container has changed
		// the system info should be update.
		aos_assert_r(oldparent_doc, false);
		u64 oldctnr_docid = oldparent_doc->getAttrU64(AOSTAG_DOCID, 0);
		removeSysInfoIIL(allTrans, num_phs, docid, oldobjid, oldctnr_docid, oldctnr_objid, 
				origxml->getAttrU64(AOSTAG_CT_EPOCH, 0), rdata, isCtnr);
		addSysInfoIIL(allTrans, num_phs, docid, newobjid, newctnr_docid, newctnr_objid,
				newxml->getAttrU64(AOSTAG_CT_EPOCH,0), rdata, isCtnr);
	}

	// 2.1 objid changed
	if (objidChanged)
	{
		OmnString iilname = AosIILName::composeCtnrMemberObjidIILName(oldctnr_objid);
		if (smShowLog)
			OmnScreen << "Remove ctnr objid: " << iilname << ":" 
				<< oldobjid << ":" << docid << endl;
		// Ketty 2012/01/08
		smIILClient->removeStrValueDoc(allTrans, num_phs, 
				iilname, oldobjid, docid, rdata);
		iilname = AosIILName::composeCtnrMemberObjidIILName(newctnr_objid);
		if (smShowLog)
			OmnScreen << "Add ctnr objid: " << iilname << ":" << newobjid
				<< ":" << docid << endl;
		smIILClient->addStrValueDoc(allTrans, num_phs, iilname, newobjid, docid, false, true, rdata);
	}

	
	AosEntryMark::E markOld;
	AosEntryMark::E markNew;
	AosEntryMark::E marknull;
	// 3.1 all words 
	mWordHashNew[tid].reset();
	mWordHashOld[tid].reset();
	//bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], oldparent_doc, origxml, false);
	//rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], parent_doc, newxml, false);
	bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], oldparent_doc, origxml, false);
	rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], parent_doc, newxml, false);
	aos_assert_r(rslt, false);
	
	OmnString word;
	while (mWordHashOld[tid].nextEntry(word, markOld))
	{
		if (!mWordHashNew[tid].get(word, markNew, true))
		{
			removeWordIIL(allTrans, num_phs, word, docid, oldctnr_objid, markOld, rdata);
		}
		else
		{
			if (parentCtnrChanged)
			{
				//only need update contianer idx
				markOld = setMarkMemberOf(markOld);
				markNew = setMarkMemberOf(markNew);
				removeWordIIL(allTrans, num_phs, word, docid, oldctnr_objid, markOld, rdata);
				addWordIIL(allTrans, num_phs, word, docid, newctnr_objid, markNew, rdata, __LINE__);
			}
		}
	}
	while (mWordHashNew[tid].nextEntry(word, markNew))
	{
		addWordIIL(allTrans, num_phs, word, docid, newctnr_objid, markNew, rdata, __LINE__);
	}

	// 3.2 attribute
	// For each word in WordHash:
	// a. If it is in WordHash1, delete the word from WordHash1
	// b. If it is not in WordHash1, delete
	// c. For any word not deleted in WordHash1, add it
	mAttrHashOld[tid].reset();
	mAttrHashNew[tid].reset();
	mAttrWordHash[tid].reset();
	aos_assert_r(AosDocProcUtil::collectAttrs(mAttrHashOld[tid], oldparent_doc, origxml,  true), false);
	aos_assert_r(AosDocProcUtil::collectAttrs(mAttrHashNew[tid], parent_doc, newxml, true), false);

	OmnString name, oldValue, newValue;
	while (mAttrHashOld[tid].nextEntry(name, oldValue, markOld))
	{
		if (!mAttrHashNew[tid].get(name, oldValue, markNew, true))
		{
			// Does not exist in the new doc. Need to remove
	    	removeAttrIIL(allTrans, num_phs, name, oldValue, docid, oldctnr_objid, markOld, rdata);
			//if (name == AOSTAG_TAG)
			if (markOld & AosEntryMark::eAttrWordIndex)
			{
				mAttrWordHash[tid].add(name, oldValue);
			}
		}
		else
		{
			// Does exist in the new doc.
			if (parentCtnrChanged)
			{
				//only need update contianer idx
				markOld = setMarkMemberOf(markOld);
				markNew = setMarkMemberOf(markNew);
	    		removeAttrIIL(allTrans, num_phs, name, oldValue, docid, oldctnr_objid, markOld, rdata);
				if (oldValue.length() <= 0)
				{
					OmnAlarm << "Value is empty: " << name << enderr;
				}
				else
				{
	    			addAttrIIL(allTrans, num_phs, name, oldValue, docid, newctnr_objid, markNew, rdata);
				}

				// Chen Ding, 11/17/2011
				//if (name == AOSTAG_TAG)
				if (markOld & AosEntryMark::eAttrWordIndex)
				{
					mWordHashOld[tid].reset();
					//rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], oldValue);
					rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], oldValue);
					aos_assert_r(rslt, false);
					while(mWordHashOld[tid].nextEntry(word, marknull))
					{
						removeAttrWordIIL(allTrans, num_phs, name, word, docid, oldctnr_objid, markOld,  rdata);
					}
				}
				if (markNew & AosEntryMark::eAttrWordIndex)
				{
					mWordHashOld[tid].resetWordLoop();
					aos_assert_r(rslt, false);
					while(mWordHashNew[tid].nextEntry(word, marknull))
					{
						addAttrWordIIL(allTrans, num_phs, name, word, docid, newctnr_objid, markNew, rdata);
					}
				}
			}
		}
	}

	// All remaining attributes in attrs2 are new attributes.
	while (mAttrHashNew[tid].nextEntry(name, newValue, markNew))
	{
		if (newValue.length() <= 0)
		{
			OmnAlarm << "Value is empty: " << name << enderr;
		}
		else
		{
	    	addAttrIIL(allTrans, num_phs, name, newValue, docid, newctnr_objid, markNew, rdata);
		}

		// Chen Ding, need to re-think
		//if (name == AOSTAG_TAG)
		if (markNew & AosEntryMark::eAttrWordIndex)
		{
			if (mAttrWordHash[tid].get(name, oldValue, true))
			{
				//The attribute words have changed.	
				mWordHashNew[tid].reset();
				mWordHashOld[tid].reset();
				//bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], oldValue);
				bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], oldValue);
				aos_assert_r(rslt, false);
				//rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], newValue);
				rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], newValue);
				aos_assert_r(rslt, false);

				while (mWordHashOld[tid].nextEntry(word, marknull))
				{
					if (!mWordHashNew[tid].get(word, marknull, true))
					{
						removeAttrWordIIL(allTrans, num_phs, name, word, docid, oldctnr_objid, markOld, rdata);
					}
				}
				while (mWordHashNew[tid].nextEntry(word, marknull))
				{
					addAttrWordIIL(allTrans, num_phs, name, word, docid, newctnr_objid, markNew, rdata);
				}
			}
			else
			{
				//Add new attribute
				mWordHashNew[tid].reset();
				//rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], newValue);
				rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], newValue);
				aos_assert_r(rslt, false);
				while(mWordHashNew[tid].nextEntry(word, marknull))
				{
					addAttrWordIIL(allTrans, num_phs, name, word, docid, newctnr_objid, markNew, rdata);
				}
			}
		}
	}

	while (mAttrWordHash[tid].nextEntry(name, oldValue))
	{
		if (mAttrHashOld[tid].get(name, oldValue, markOld, false))
		{
			if (markOld & AosEntryMark::eAttrWordIndex)
			{
				//Delete old attrbute
				mWordHashNew[tid].reset();
				//rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], oldValue);
				rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], oldValue);
				aos_assert_r(rslt, false);
				while(mWordHashNew[tid].nextEntry(word, marknull))
				{
					removeAttrWordIIL(allTrans, num_phs, name, word, docid, oldctnr_objid, markOld, rdata);
				}
			}
		}
	}

	// 4 Modify Meta data
	AosDocProcUtil::modifyMetaAttrs(allTrans, num_phs, origxml, newxml, docid, rdata);

	// 5 AOSTAG_MEMBEROF  changed
	modifyMemberOf(allTrans, num_phs, origxml, newxml, rdata);

	// 6  modify contaienr index attrbute.
	
	// Ketty 2013/02/22
	//smIILClient->sendTrans(request->task_trans, buffs, docid, rdata);
	sendTrans(rdata, allTrans, num_phs);
	newxml->setReadOnly(false);
	return true;
}


bool
AosDocProc::deleteDocPriv(const AosDocReqPtr &request)
{
	aos_assert_r(smIILClient, false);
	AosXmlTagPtr xml = request->doc;
	xml->setReadOnly(true);
	OmnString siteid = xml->getAttrStr(AOSTAG_SITEID);
	OmnString objid = xml->getAttrStr(AOSTAG_OBJID);

	// Ketty 2013/02/26
	// Chen Ding, 10/09/2011
	int num_phs = AosGetNumPhysicals();
	//vector<AosBuffPtr> buffs(num_phs);
	//vector<AosBuffPtr> buffs(AosGetNumCubes());
	//createBuffs(buffs);
	vector<AosTransPtr>  allTrans[num_phs];

	// Chen Ding, 10/01/2011
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);

	//mDocTransId[tid] = request->getDocTransid();
	//AOSIILTRANSTEST_ADDDOC("delete", mDocTransId[tid], xml);	// Ketty 2012/02/01

	if (smShowLog)
		OmnScreen << "Delete Doc: " << xml->toString() << endl;

	AosRundataPtr rdata = request->getRundata();

	//AosXmlDocPtr header = request->header;
	bool exist;
	u64 docid = xml->getAttrU64(AOSTAG_DOCID, AOS_INVDID, exist);

	OmnString parent_objid = xml->getAttrStr(AOSTAG_PARENTC);
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);
	AosXmlTagPtr parent_doc = docclient->getDocByObjid(parent_objid, rdata);
	aos_assert_r(parent_doc, false);
	u64 parent_docid = parent_doc->getAttrU64(AOSTAG_DOCID, 0);
	AosEntryMark::E mark, marknull;

	// 1. Delete all the words for the doc
	mWordHashOld[tid].reset();
	//bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], parent_doc, xml, false);
	bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], parent_doc, xml, false);
	aos_assert_r(rslt, false);
	OmnString word;
	while (mWordHashOld[tid].nextEntry(word, mark))
	{
		removeWordIIL(allTrans, num_phs, word, docid, parent_objid, mark, rdata);
	}

	// 2. Delete attributes: "name=value"
	mAttrHashOld[tid].reset();
	rslt = AosDocProcUtil::collectAttrs(mAttrHashOld[tid], parent_doc, xml, true);
	aos_assert_r(rslt, false);
	OmnString name, value;
	while (mAttrHashOld[tid].nextEntry(name, value, mark))
	{
	    removeAttrIIL(allTrans, num_phs, name, value, docid, parent_objid, mark, rdata);

		// Chen Ding, 11/17/2011
		// We need to re-think about it.
		//if (name == AOSTAG_TAG)
		
		//OmnScreen << "mark:" << mark
		//		<< "; " << endl;
		if (mark & AosEntryMark::eAttrWordIndex)
		{
			mWordHashOld[tid].reset();
			//rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], value);
			rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], value);
			aos_assert_r(rslt, false);
			while(mWordHashOld[tid].nextEntry(word, marknull))
			{
				removeAttrWordIIL(allTrans, num_phs, name, word, docid, parent_objid, mark, rdata);
			}
		}
	}

	bool isCtnr = xml->getAttrStr(AOSTAG_OTYPE,"") == AOSOTYPE_CONTAINER;
	// 3 Remove system infomation to iil
	removeSysInfoIIL(allTrans, num_phs, docid, objid, parent_docid, parent_objid,
			xml->getAttrU64(AOSTAG_CT_EPOCH, 0), rdata, isCtnr);

	// 4 remove meta data
	AosDocProcUtil::removeMetaAttrs(allTrans, num_phs, xml, docid, rdata);

	// 5 remove member of
	// Chen Ding, 11/13/2011
	// Removing member_of is to add 'objid' from the parent container
	// 'ctnr_objid'. Need to check whether the container allows
	// its member listing.
	// AosXmlTagPtr memberof_doc = AosRetrieveDocByObjid(ctnr_objid, rdata);
	// addMemberOf(buffs, doc, memberof_doc, part, rdata);
	OmnString memberof = xml->getAttrStr(AOSTAG_MEMBEROF);
	OmnStrParser1 parser(memberof, ", ", false, false);       
	OmnString ctnr_objid;
	while ((ctnr_objid = parser.nextWord()) != "")
	{
		if (AosSecurityMgrObj::getSecurityMgr()->checkMemberOfListing(ctnr_objid, parent_objid, rdata))
		{
			// It is allowed.
			OmnString iilname = AosIILName::composeCtnrMemberObjidIILName(ctnr_objid);
			bool rslt = AosDocProcUtil::removeValueFromIIL(allTrans, num_phs, iilname, objid, docid, rdata);
			if (!rslt)
			{
				OmnAlarm << "Failed removing member: " << ctnr_objid
					<< ":" << objid << ":" << docid << ". Error: " << rdata->getErrmsg()
					<< enderr;
			}
		}
	}

	// OmnString memberof = xml->getAttrStr(AOSTAG_MEMBEROF, "");
	// OmnStrParser1 parser(memberof, ", ", false, false);       
	// OmnString part;
	// while ((part = parser.nextWord()) != "")
	// {
	// 	AosXmlTagPtr memberof_doc = AosRetrieveDocByObjid(part, rdata);
	// 	aos_assert_r(memberof_doc, false);
	// 	removeMemberOf(buffs, xml, memberof_doc, rdata);
	// }

	// 6 proc delete container. 
	if (isCtnr)
		removeContainer(xml, rdata);

/* Ice Word Marking
// Log [deletetime, docid]
u32 systemsec = OmnGetSecond();
if (smShowLog) OmnScreen << "Add value doc: " << systemsec << ":" << docid << endl;
smIILClient->addU64ValueDoc(AOSZTG_DELTIME, false, systemsec, 
		docid, false, true, rdata);

// Log [uname, docid]
if (request->userid)
{
	if (smShowLog) OmnScreen << "Add value doc: " << request->userid 
		<< ":" << docid << endl;
	smIILClient->addU64ValueDoc(AOSZTG_DELUNAME, false, request->userid, 
			docid, false, false, rdata);
}
*/

	// Ketty 2013/02/22
	//smIILClient->sendTrans(request->task_trans, buffs, docid, rdata);
	sendTrans(rdata, allTrans, num_phs);
	xml->setReadOnly(false);
	return true;
}


bool
AosDocProc::deleteLogPriv(const AosDocReqPtr &request)
{
	aos_assert_r(smIILClient, false);
	AosXmlTagPtr xml = request->doc;
	xml->setReadOnly(true);

	// Ketty 2013/02/26
	// Chen Ding, 10/09/2011
	int num_phs = AosGetNumPhysicals();
	//vector<AosBuffPtr> buffs(num_phs);
	//vector<AosBuffPtr> buffs(AosGetNumCubes());	
	//createBuffs(buffs);
	vector<AosTransPtr>  allTrans[num_phs];

	// Chen Ding, 10/01/2011
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);

	//mDocTransId[tid] = request->getDocTransid();
	//AOSIILTRANSTEST_ADDDOC("deletelog", mDocTransId[tid], xml);	// Ketty 2012/02/01

	if (smShowLog)
		OmnScreen << "Delete Log: " << xml->toString() << endl;

	AosRundataPtr rdata = request->getRundata();

	//AosXmlDocPtr header = request->header;
	bool exist;
	u64 logid = xml->getAttrU64(AOSTAG_LOGID, AOS_INVDID, exist);

	OmnString parent_objid = xml->getAttrStr(AOSTAG_LOG_CTNROBJID);
	aos_assert_r(parent_objid != "", false);
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);
	AosXmlTagPtr parent_doc = docclient->getDocByObjid(parent_objid, rdata);
	aos_assert_r(parent_doc, false);
	AosEntryMark::E mark, marknull;
	
	//Log's configuration is in container.
	///////////////////////////////////////////////////////////////////////
	AosXmlTagPtr logdef = parent_doc->getFirstChild(AOSTAG_LOG_DEFINITION);
	if (!logdef) return true;
	AosXmlTagPtr idxdef = logdef->getFirstChild(AOSTAG_LOG_INDEX);
	if (!idxdef) return true;
	///////////////////////////////////////////////////////////////////////

	// 1. Delete all the words for the doc
	mWordHashOld[tid].reset();
	//bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], idxdef, xml, false);
	bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], idxdef, xml, false);
	aos_assert_r(rslt, false);
	OmnString word;
	while (mWordHashOld[tid].nextEntry(word, mark))
	{
		removeWordIIL(allTrans, num_phs, word, logid, parent_objid, mark, rdata);
	}

	// 2. Delete attributes: "name=value"
	mAttrHashOld[tid].reset();
	rslt = AosDocProcUtil::collectAttrs(mAttrHashOld[tid], idxdef, xml, true);
	aos_assert_r(rslt, false);
	OmnString name, value;
	while (mAttrHashOld[tid].nextEntry(name, value, mark))
	{
	    removeAttrIIL(allTrans, num_phs, name, value, logid, parent_objid, mark, rdata);

		mWordHashOld[tid].reset();
		//rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], value);
		rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], value);
		aos_assert_r(rslt, false);
		while(mWordHashOld[tid].nextEntry(word, marknull))
		{
	    	removeAttrWordIIL(allTrans, num_phs, name, word, logid, parent_objid, mark, rdata);
		}
	}
	//Jozhi 2013/03/25
	//AosDocProcUtil::removeMetaAttrs(buffs, xml, logid, rdata);
	AosDocProcUtil::removeMetaAttrs(allTrans, num_phs, xml, logid, rdata);

	// Ketty 2013/02/22
	//smIILClient->sendTrans(request->task_trans, buffs, logid, rdata);
	sendTrans(rdata, allTrans, num_phs);
	xml->setReadOnly(false);
	return true;
}


bool
AosDocProc::modifyAttrStrPriv(const AosDocReqPtr &request)
{
	AosRundataPtr rdata = request->getRundata();
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);
	AosXmlTagPtr xml = request->doc;
	aos_assert_r(xml, false);

	OmnString ctnr_objid = xml->getAttrStr(AOSTAG_PARENTC);
	AosXmlTagPtr parent_doc = docclient->getDocByObjid(ctnr_objid, rdata);
	AosEntryMark::E mark;
	bool isMeta = AosXmlTag::isMetaAttr(request->aname);
	if (isMeta)
		mark = AosEntryMark::eGlobalIndex;
	else
		mark = xml->markAttr(parent_doc, request->aname);

	if (smShowLog)
		OmnScreen << "Modify Attr: doc:" << request->docid << ": aname:" <<
			request->aname << ": new:" << request->strval2 << ": old:" << 
			request->strval1 << endl;
	
	if (AosXmlTag::isAttrIgnored(mark, request->aname)) return true;

	// Ketty 2013/02/26
	// Chen Ding, 10/09/2011
	int num_phs = AosGetNumPhysicals();
	//vector<AosBuffPtr> buffs(num_phs);
	//vector<AosBuffPtr> buffs(AosGetNumCubes());
	//createBuffs(buffs);
	vector<AosTransPtr>  allTrans[num_phs];

	// Chen Ding, 10/01/2011
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);
	
	if (request->exist)
	{
		// 1. zky_objid has changed and the zky_otype is container
		if (request->aname == AOSTAG_OBJID)	
		{
			if (xml->getAttrStr(AOSTAG_OTYPE,"") == AOSOTYPE_CONTAINER)
			{
				removeContainer(xml, rdata);
			}
			else
			{
				OmnString iilname = AosIILName::composeCtnrMemberObjidIILName(ctnr_objid);
				if (smShowLog)
					OmnScreen << "Remove ctnr objid: " << iilname << ":" 
						<< request->strval1 << ":" << request->docid << endl;
				smIILClient->removeStrValueDoc(allTrans, num_phs, 
						iilname, request->strval1, request->docid, rdata);
				if (smShowLog)
					OmnScreen << "Add ctnr objid: " << iilname << ":" << request->strval2 
						<< ":" << request->docid << endl;
				smIILClient->addStrValueDoc(allTrans, num_phs, 
						iilname, request->strval2, request->docid, false, true, rdata);
			}

		}

		// 2. zky_pctrs or AOSTAG_MEMBEROF has changed 
		if (request->aname == AOSTAG_PARENTC || request->aname == AOSTAG_MEMBEROF)
		{
			AosXmlTagPtr oldxml = xml->clone(AosMemoryCheckerArgsBegin);
			oldxml->setAttr(AOSTAG_PARENTC, request->strval2);
			modifyDocPriv(xml, oldxml, request, rdata);
			return true;
		}

		// 3. normal attr 
		removeAttrIIL(allTrans, num_phs, request->aname, 
					 request->strval1,
					 request->docid,
					 ctnr_objid,
					 mark,
					 rdata);

		if (request->strval2.length() <= 0)
		{
			OmnAlarm << "Value is empty: " << request->aname << enderr;
		}
		else
		{
			addAttrIIL(allTrans, num_phs, request->aname, request->strval2, request->docid,
					 ctnr_objid, mark, rdata);
		}
		bool rslt = false;
		OmnString word;
		AosEntryMark::E mark1; // invalid mark

		// 4. normal word
		//The attribute words have changed.	
		mWordHashNew[tid].reset();
		mWordHashOld[tid].reset();
		//rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], request->strval1);
		rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], request->strval1);
		aos_assert_r(rslt, false);
		//rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], request->strval2);
		rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], request->strval2);
		aos_assert_r(rslt, false);
		while (mWordHashOld[tid].nextEntry(word, mark1))
		{
			if (!mWordHashNew[tid].get(word, mark1, true))
			{
				// if (!isMeta)
				// {
				//if (request->aname == AOSTAG_TAG)
				if (mark & AosEntryMark::eAttrWordIndex)
				{
					removeAttrWordIIL(allTrans, num_phs, request->aname, word, 
						request->docid, ctnr_objid, mark, rdata);
				}
				// }
				removeWordIIL(allTrans, num_phs, word, request->docid, ctnr_objid, mark, rdata);
			}
		}

		// Chen Ding, 11/17/2011
		while (mWordHashNew[tid].nextEntry(word, mark1))
		{
			// if (!isMeta)
			// {
			   //if (request->aname == AOSTAG_TAG) 
				if (mark & AosEntryMark::eAttrWordIndex)
				{
					addAttrWordIIL(allTrans, num_phs, request->aname, word.data(), 
						request->docid, ctnr_objid, mark, rdata);
				}
			// }
			addWordIIL(allTrans, num_phs, word.data(), request->docid, ctnr_objid, mark, rdata, __LINE__);
		}
		
		// 5. index attr
	}
	else
	{
		if (request->strval2.length() <= 0)
		{
			OmnAlarm << "Value is empty: " << request->aname << enderr;
		}
		else
		{
			addAttrIIL(allTrans, num_phs, request->aname, request->strval2, request->docid,
					 ctnr_objid, mark, rdata);
		}

		mWordHashNew[tid].reset();
		//bool rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], request->strval2);
		bool rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], request->strval2);
		aos_assert_r(rslt, false);
		OmnString word;
		AosEntryMark::E mark1; // invalid mark
		while(mWordHashNew[tid].nextEntry(word, mark1))
		{
			// if (!isMeta)
			   //if (request->aname == AOSTAG_TAG)
				if (mark & AosEntryMark::eAttrWordIndex)
				{
			 	   addAttrWordIIL(allTrans, num_phs, request->aname, word, request->docid, ctnr_objid, mark, rdata);
			    }
			// 
			addWordIIL(allTrans, num_phs, word, request->docid, ctnr_objid, mark, rdata, __LINE__);
		}

		// add memberof 
		if (request->aname == AOSTAG_MEMBEROF)
		{
			OmnStrParser1 parser(request->strval2, ", ", false, false);       
			OmnString part;
			AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
			aos_assert_r(docclient, false);
			while ((part = parser.nextWord()) != "")
			{
				AosXmlTagPtr memberof_doc = docclient->getDocByObjid(part, rdata);
				addMemberOf(allTrans, num_phs, xml, memberof_doc, part, rdata);
			}
		}
	}

	u64 docid = xml->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);
	// Ketty 2013/02/22
	//smIILClient->sendTrans(request->task_trans, buffs, docid, rdata);
	sendTrans(rdata, allTrans, num_phs);
	return true;
}


/*
bool
AosDocProc::addMetaAttrs(
		vector<AosBuffPtr> &buffs,
		const AosXmlTagPtr &doc,
		const u64 &docId,
		const AosRundataPtr &rdata) 
{
	OmnString valueStr;

	for (int i=0; i<sgNumMetaAttrs; i++)
	{
		valueStr = doc->getAttrStr(sgMetaAttrs[i].attrname);
		if(valueStr != "")
		{
			if (smShowLog) 
				OmnScreen << "Add meta: " << 
					sgMetaAttrs[i].attrname << ":" << valueStr << ":" << docId << endl;
			addValueToIIL(buffs, sgMetaAttrs[i].iilname, valueStr, docId, 
				sgMetaAttrs[i].value_unique, 
				sgMetaAttrs[i].docid_unique, 
				rdata);
		}
	}
	return true;
}
*/


/*
bool
AosDocProc::removeMetaAttrs(const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);
	for (int i=0; i<sgNumMetaAttrs; i++)
	{
		doc->removeAttr(sgMetaAttrs[i].attrname);
	}
	return true;
}


bool
AosDocProc::removeMetaAttrs(
		vector<AosBuffPtr> &buffs,
		const AosXmlTagPtr &doc,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	OmnString valueStr;
	OmnString word;

	for (int i=0; i<sgNumMetaAttrs; i++)
	{
		valueStr = doc->getAttrStr(sgMetaAttrs[i].attrname);
		if(valueStr != "")
		{
			if (smShowLog) OmnScreen << "Remove meta: " << sgMetaAttrs[i].attrname << ":"
				<< valueStr << ":" << docid << endl;
			// Chen Ding, 08/09/2011
			removeValueFromIIL(buffs, sgMetaAttrs[i].iilname, valueStr, docid, rdata);
		}
	}
	return true;
}


void
AosDocProc::setMetaAttrs()
{
	sgNumMetaAttrs = 0;
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_CREATOR, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_OBJID, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_OTYPE, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_PARENTC, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_MODUSER, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_CTIME, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_CT_EPOCH, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_MTIME, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_MT_EPOCH, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_SUBTYPE, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_COUNTERCM, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_COUNTERDW, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_COUNTERLK, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_COUNTERRC, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_COUNTERWR, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_COUNTERUP, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_COUNTERRD, false, true);
	sgMetaAttrs[sgNumMetaAttrs++].set(AOSTAG_DOCID, false, true);
	if (sgNumMetaAttrs > sgMaxMetaAttrs)
	{
		OmnAlarm << "Too many meta attributes. Please fix the problem!"
			<< enderr;
		exit(-1);
	}
}
*/


AosDocReqPtr
AosDocProc::addDeleteRequest(
		const AosXmlTagPtr &docroot,
		const AosXmlTagPtr &doc,
		const u64 userid,
		const OmnString &appname, 
		const AosRundataPtr &rdata)
		//u64 &doc_transid)
		//const AosTaskTransPtr &task_trans)
{
	AosDocReqPtr qq = OmnNew AosDocReq(AosDocReq::eDeleteDoc, docroot, doc,
		userid, 0, 0, rdata);
	qq->appname = appname;
	//qq->doc_transid = doc_transid;
	//qq->task_trans = task_trans;
	bool rslt = addRequest(qq);
	aos_assert_r(rslt, 0);
	return qq;
}


AosDocReqPtr
AosDocProc::addModifyRequest(
		const AosXmlTagPtr &root,
		const u64 &userid,
		const AosXmlTagPtr &newxml,
		const AosXmlTagPtr &origxml, 
		const bool synobj, 
		const AosRundataPtr &rdata)
		//u64 &doc_transid)
		//const AosTaskTransPtr &task_trans)
{
	AosDocReqPtr qq = OmnNew AosDocReq(AosDocReq::eModifyDoc, root, newxml, userid, 0, 0, rdata);
	qq->origdocroot = origxml;
	qq->synobj = synobj;
	//qq->doc_transid = doc_transid;
	//qq->task_trans = task_trans;
	bool rslt = addRequest(qq);
	aos_assert_r(rslt, 0);
	return qq;
	// mLock->lock();
	// if (mNumReqs > cMaxOutstandingMsgs) OmnSleep(1);
	// mTotalAccepted++;
	// mNumReqs++;
	// if (mHead) mHead->prev = qq;
	// qq->next = mHead;
	// qq->prev = 0;
	// mHead = qq;
	// if (!mTail) mTail = qq;
	// mCondVar->signal();
	// mLock->unlock();
	// return true;
}


AosDocReqPtr
AosDocProc::addCreateRequest(
		const AosXmlTagPtr &root,
		const u64 &userid,
		const AosXmlTagPtr &xml,
		const AosDocSvrCbObjPtr &caller, 
		void *userdata, 
		const AosRundataPtr &rdata)
		//u64 &doc_transid)
		//const AosTaskTransPtr &task_trans)
{
	AosDocReqPtr qq = OmnNew AosDocReq(AosDocReq::eCreateDoc, root, xml,
		userid, caller, userdata, rdata);
	//qq->doc_transid = doc_transid;
	//qq->task_trans = task_trans;
	bool rslt =  addRequest(qq);
	aos_assert_r(rslt, 0);
	return qq;
}


bool
AosDocProc::addCreateLogRequest(
		const u64 &userid,
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
		//const AosTaskTransPtr &task_trans)
{
	AosDocReqPtr qq = OmnNew AosDocReq(AosDocReq::eCreateLog,
		NULL, xml, userid, NULL, NULL, rdata);
	//qq->doc_transid = 0;
	//qq->task_trans = task_trans;
	bool rslt =  addRequest(qq);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDocProc::addDeleteLogRequest(
		const AosXmlTagPtr &doc,
		const u64 userid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false; 
	AosDocReqPtr qq = OmnNew AosDocReq(AosDocReq::eDeleteLog,
		NULL, doc, userid, 0, 0, rdata);
	qq->appname = "";
	//qq->doc_transid = 0;
	bool rslt = addRequest(qq);
	aos_assert_r(rslt, false);
	return true;
}


AosDocReqPtr
AosDocProc::addModifyAttrRequest(
		const OmnString &aname,
		const OmnString &oldvalue,
		const OmnString &newvalue, 
		const bool exist,
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid, 
		const AosRundataPtr &rdata)
		//u64 &doc_transid)
		//const AosTaskTransPtr &task_trans)
{
	AosDocReqPtr qq = OmnNew AosDocReq(AosDocReq::eModifyAttrStr, docid, rdata);
	
	AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata);
	if (!doc)
	{
		AosSetErrorU(rdata, "failed_retrieve_doc") << ": " << docid
			<< ":" << rdata->getErrmsg() << enderr;
		return 0;
	}
	doc = doc->clone(AosMemoryCheckerArgsBegin);
	qq->doc = doc;

	qq->aname = aname;
	qq->strval1 = oldvalue;
	qq->strval2 = newvalue;
	qq->exist = exist;
	qq->value_unique = value_unique;
	qq->docid_unique = docid_unique;
	//qq->doc_transid = doc_transid;
	//qq->task_trans = task_trans;
	bool rslt =  addRequest(qq);
	aos_assert_r(rslt, 0);
	return qq;

	// mLock->lock();
	// if (mNumReqs > cMaxOutstandingMsgs) OmnSleep(1);
	// mTotalAccepted++;
	// mNumReqs++;
	// if (mHead) mHead->prev = qq;
	// qq->next = mHead;
	// qq->prev = 0;
	// mHead = qq;
	// if (!mTail) mTail = qq;
	// mCondVar->signal();
	// mLock->unlock();
	// return true;
}


/*
bool
AosDocProc::modifyMetaAttrs(
		vector<AosBuffPtr> &buffs,
		const AosXmlTagPtr &olddoc, 
		const AosXmlTagPtr &newdoc, 
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	OmnString word;
	OmnString word2;
	OmnString oldValue;
	OmnString newValue;

	for (int i=0; i<sgNumMetaAttrs; i++)
	{
		oldValue = olddoc->getAttrStr(sgMetaAttrs[i].attrname);
		newValue = newdoc->getAttrStr(sgMetaAttrs[i].attrname);
		if(oldValue != newValue)
		{

			if (oldValue != "")
			{
				if (smShowLog) OmnScreen << "Remove meta: " << oldValue << ":" << docid << endl;
				removeValueFromIIL(buffs, sgMetaAttrs[i].iilname, oldValue, docid, rdata);
			}

			// add new
			if(newValue != "")
			{
				if (smShowLog) OmnScreen << "Add  meta: " << newValue << ":" << docid << endl;
				addValueToIIL(buffs, sgMetaAttrs[i].iilname, newValue, docid, 
						sgMetaAttrs[i].value_unique,
						sgMetaAttrs[i].docid_unique, 
						rdata);
			}
		}
	}

	return true;
}
*/


/*
bool
AosDocProc::isWordIgnored(const OmnString &word)
{
	u64 wordId = mIgnoredWords[tid]->getWordId(word.data(), word.length(), false);
	return (wordId != AOS_INVWID);
}
*/


bool 
AosDocProc::addWordIIL(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const OmnString &word, 
		const u64 docid, 
		const OmnString &ctnr_objid, 
		const AosEntryMark::E mark,
		const AosRundataPtr &rdata, 
		const int line)
{
	aos_assert_r(smIILClient, false);
	if (smShowLog) OmnScreen << "Add word: " << word << ":" << docid << ":" << mark << endl;
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);

	OmnString fname = __FILE__;
	fname << ":" << line;
	bool rslt;
	OmnString iilname;
	// AosXmlTag::EntryMark mark1 = (AosXmlTag::EntryMark)(mark & ~AosXmlTag::eAttrWordIndex);
	AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);
	switch(mark1)
	{
	case AosEntryMark::eContainerIndex:
	case AosEntryMark::eBothIndex:
		 iilname= AosIILName::composeContainerWordIILName(ctnr_objid, word);
		 // Chen Ding, 01/28/2012
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname.data(), iilname.length(), false, docid, rdata);
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, false, docid, rdata, fname, __LINE__);
		 rslt = smIILClient->addHitDoc(allTrans, arr_len, iilname, docid, rdata);

	case AosEntryMark::eGlobalIndex:
		 // Chen Ding, 01/28/2012
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 word, word.length(), false, docid, rdata);
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 word, false, docid, rdata, fname, __LINE__);
		 rslt = smIILClient->addHitDoc(allTrans, arr_len, word, docid, rdata);
		 break;

	case AosEntryMark::eCtnrOnly:
		 iilname= AosIILName::composeContainerWordIILName(ctnr_objid, word);
		 // Chen Ding, 01/28/2012
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname.data(), iilname.length(), false, docid, rdata);
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, false, docid, rdata, fname, __LINE__);
		 rslt = smIILClient->addHitDoc(allTrans, arr_len, iilname, docid, rdata);
		 break;
		 
	case AosEntryMark::eNoIndex:
		 break;

	default:
		 break;
	}
	return rslt;
}


bool
AosDocProc::removeWordIIL(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const OmnString &word, 
		const u64 docid, 
		const OmnString &ctnr_objid, 
		const AosEntryMark::E mark,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(smIILClient, false);
	if (smShowLog) OmnScreen << "Remove word: " << word << ":" << docid << ":" << mark << endl;

	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);
	bool rslt;
	OmnString iilname;
	// AosXmlTag::EntryMark mark1 = (AosXmlTag::EntryMark)(mark & ~AosXmlTag::eAttrWordIndex);
	AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);

	OmnString fnamestr = __FILE__;
	switch(mark1)
	{
	case AosEntryMark::eContainerIndex:

	case AosEntryMark::eBothIndex:
		 iilname= AosIILName::composeContainerWordIILName(ctnr_objid, word);
		 // Chen Ding, 01/27/2012
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 // 		 iilname.data(), iilname.length(), docid, rdata);
		 fnamestr << ":" << __FILE__;
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, docid, rdata, fnamestr, __LINE__);
		 rslt = smIILClient->removeHitDoc(allTrans, arr_len, 
				 iilname, docid, rdata);

	case AosEntryMark::eGlobalIndex:
		 // Chen Ding, 01/27/2012
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 // 		 word, word.length(), docid, rdata);
		 fnamestr << ":" << __FILE__;
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 //		 word, docid, rdata, fnamestr, __LINE__);
		 rslt = smIILClient->removeHitDoc(allTrans, arr_len, 
				 word, docid, rdata);
		 break;

	case AosEntryMark::eCtnrOnly:
		 iilname= AosIILName::composeContainerWordIILName(ctnr_objid, word);
		 // Chen Ding, 01/27/2012
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname.data(), iilname.length(), docid, rdata);
		 fnamestr << ":" << __FILE__;
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, docid, rdata, fnamestr, __LINE__);
		 rslt = smIILClient->removeHitDoc(allTrans, arr_len, 
				 iilname, docid, rdata);
		 break;

	case AosEntryMark::eNoIndex:
		 break;

	default:
		 break;
	}
	return rslt;
}


bool 
AosDocProc::addAttrIIL(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const OmnString &name, 
		const OmnString &value, 
		const u64 docid, 
		const OmnString &ctnr_objid, 
		const AosEntryMark::E mark,
		const AosRundataPtr &rdata)
{
	if (smShowLog) 
		OmnScreen << "Add attr: " << name << ":" << value << ":" << 
			docid << ":" << mark << endl;
	bool rslt;
	OmnString iilname;
	// AosXmlTag::EntryMark mark1 = (AosXmlTag::EntryMark)(mark & ~AosXmlTag::eAttrWordIndex);
	AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);
	switch(mark1)
	{
	case AosEntryMark::eBothIndex:

	case AosEntryMark::eContainerIndex:
		 iilname = AosIILName::composeContainerAttrIILName(ctnr_objid, name);
		 rslt = AosDocProcUtil::addValueToIIL(allTrans, arr_len, iilname, value, docid, false, true, rdata);

	case AosEntryMark::eGlobalIndex:
		 iilname = AosIILName::composeAttrIILName(name);
		 rslt = AosDocProcUtil::addValueToIIL(allTrans, arr_len, iilname, value, docid, false, true, rdata);
		 break;

	case AosEntryMark::eCtnrOnly:
		 iilname = AosIILName::composeContainerAttrIILName(ctnr_objid, name);
		 rslt = AosDocProcUtil::addValueToIIL(allTrans, arr_len, iilname, value, docid, false, true, rdata);
		 break;

	case AosEntryMark::eNoIndex:
		 break;

	default:
		 break;
	}
	return rslt;
}


bool 
AosDocProc::removeAttrIIL(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const OmnString &name, 
		const OmnString &value, 
		const u64 docid, 
		const OmnString &ctnr_objid, 
		const AosEntryMark::E mark,
		const AosRundataPtr &rdata)
{
	if (smShowLog) 
		OmnScreen << "Remove attr: " << name << ":" << value << ":" 
			<< docid << ":" << mark << endl;
	bool rslt;
	OmnString iilname;
	// AosXmlTag::EntryMark mark1 = (AosXmlTag::EntryMark)(mark & ~AosXmlTag::eAttrWordIndex);
	AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);
	switch(mark1)
	{
	case AosEntryMark::eBothIndex:

	case AosEntryMark::eContainerIndex:
		 iilname = AosIILName::composeContainerAttrIILName(ctnr_objid, name);
		 rslt = AosDocProcUtil::removeValueFromIIL(allTrans, arr_len, iilname, value, docid,  rdata);

	case AosEntryMark::eGlobalIndex:
		 iilname = AosIILName::composeAttrIILName(name);
		 rslt = AosDocProcUtil::removeValueFromIIL(allTrans, arr_len, iilname, value, docid, rdata);
		 break;

	case AosEntryMark::eCtnrOnly:
		 iilname = AosIILName::composeContainerAttrIILName(ctnr_objid, name);
		 rslt = AosDocProcUtil::removeValueFromIIL(allTrans, arr_len, iilname, value, docid, rdata);
		 break;

	case AosEntryMark::eNoIndex:
		 break;
	default:
		 break;
	}
	return rslt;
}


bool 
AosDocProc::addAttrWordIIL(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const OmnString &attrname, 
		const OmnString &word, 
		const u64 docid, 
		const OmnString &ctnr_objid, 
		const AosEntryMark::E mark, 
		const AosRundataPtr &rdata) 
{
	// It processes the attribute word indexing. 'Attribute Word Indexing'
	// means it will create one IIL for each of words contained in the 
	// attribute named 'attrname'. There are global attribute word indexing
	// and container attribtue word indexing.
	aos_assert_r(smIILClient, false);
	if (smShowLog) 
		OmnScreen << "Add attr word: " << attrname << ":" << word 
			<< ":" << docid << ":" << mark << endl;
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);

	bool rslt;
	OmnString iilname;
	// AosXmlTag::EntryMark mark1 = (AosXmlTag::EntryMark)(mark & ~AosXmlTag::eAttrWordIndex);
	//AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);
	switch(mark)
	{
	case AosEntryMark::eBothIndex|AosEntryMark::eAttrWordIndex:

	case AosEntryMark::eContainerIndex|AosEntryMark::eAttrWordIndex:
		 iilname = AosIILName::composeContainerAttrWordIILName(ctnr_objid, attrname, word);
		 // Chen Ding, 01/28/2012
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, iilname.length(), false, docid, rdata);
		 //rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, false, docid, rdata, __FILE__, __LINE__);
		 rslt = smIILClient->addHitDoc(allTrans, arr_len, 
				 iilname, docid, rdata);

	case AosEntryMark::eGlobalIndex|AosEntryMark::eAttrWordIndex:
		 iilname = AosIILName::composeAttrWordIILName(attrname, word);
		 // Chen Ding, 01/28/2012
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, iilname.length(), false, docid, rdata);
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, false, docid, rdata, __FILE__, __LINE__);
		 rslt = smIILClient->addHitDoc(allTrans, arr_len, 
				 iilname, docid, rdata);
		 break;

	case AosEntryMark::eCtnrOnly|AosEntryMark::eAttrWordIndex:
		 iilname = AosIILName::composeContainerAttrWordIILName(ctnr_objid, attrname, word);
		 // Chen Ding, 01/28/2012
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, iilname.length(), false, docid, rdata);
		 // rslt = smIILClient->addHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, false, docid, rdata, __FILE__, __LINE__);
		 rslt = smIILClient->addHitDoc(allTrans, arr_len,
				 iilname, docid, rdata);
		 break;

	case AosEntryMark::eNoIndex|AosEntryMark::eAttrWordIndex:
		 iilname = AosIILName::composeAttrWordIILName(attrname, word);
		 rslt = smIILClient->addHitDoc(allTrans, arr_len, 
				 iilname, docid, rdata);
		 break;

	default:
		 break;
	}
	return rslt;

}


bool 
AosDocProc::removeAttrWordIIL(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const OmnString &name, 
		const OmnString &word, 
		const u64 docid, 
		const OmnString &ctnr_objid, 
		const AosEntryMark::E mark,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(smIILClient, false);
	if (smShowLog) 
		OmnScreen << "Remove attr word: " << name << ":" << word 
			<< ":" << docid << ":" << mark << endl;
	bool rslt;
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);

	OmnString iilname;
	// AosXmlTag::EntryMark mark1 = (AosXmlTag::EntryMark)(mark & ~AosXmlTag::eAttrWordIndex);
	AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);
	switch(mark1)
	{
	case AosEntryMark::eBothIndex:

	case AosEntryMark::eContainerIndex:
		 iilname = AosIILName::composeContainerAttrWordIILName(ctnr_objid, name, word);
		 // Chen Ding, 01/27/2012
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, iilname.length(), docid, rdata);
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, docid, rdata, __FILE__, __LINE__);
		 rslt = smIILClient->removeHitDoc(allTrans, arr_len, 
				 iilname, docid, rdata);

	case AosEntryMark::eGlobalIndex:
		 iilname = AosIILName::composeAttrWordIILName(name, word);
		 // Chen Ding, 01/27/2012
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 // 	 iilname, iilname.length(), docid, rdata);
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, docid, rdata, __FILE__, __LINE__);
		 rslt = smIILClient->removeHitDoc(allTrans, arr_len, 
				 iilname, docid, rdata);
		 break;

	case AosEntryMark::eCtnrOnly:
		 iilname = AosIILName::composeContainerAttrWordIILName(ctnr_objid, name, word);
		 // Chen Ding, 01/27/2012
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, iilname.length(), docid, rdata);
		 // rslt = smIILClient->removeHitDoc(mDocTransId[tid]++, buffs, 
		 //		 iilname, docid, rdata, __FILE__, __LINE__);
		 rslt = smIILClient->removeHitDoc(allTrans, arr_len, 
				 iilname, docid, rdata);
		 break;

	case AosEntryMark::eNoIndex:
		 break;
	default:
		 break;
	}
	return rslt;

}


bool
AosDocProc::addMemberOf(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const AosXmlTagPtr doc,
		const AosXmlTagPtr &memberof_doc,
		const OmnString &memberof_objid,
		const AosRundataPtr &rdata)
{
	// A doc may have an attribute 'member_of'. Its contents are 
	// the names of those containers that this doc should be a member of.
	//
	// Chen Ding, 10/01/2011
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);
	
	// 1. Add words
	mWordHashNew[tid].reset();
	//bool rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], memberof_doc, doc, false);
	bool rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], mWordParser[tid], mWordNorm[tid], memberof_doc, doc, false);
	aos_assert_r(rslt, false);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, false);
	OmnString word;
	AosEntryMark::E mark;
	while (mWordHashNew[tid].nextEntry(word, mark))
	{
		mark = setMarkMemberOf(mark);
		addWordIIL(allTrans, arr_len, word, docid, memberof_objid, mark, rdata, __LINE__);
	}

	// 2. Add attributes: "name=value"
	mAttrHashNew[tid].reset();
	rslt = AosDocProcUtil::collectAttrs(mAttrHashNew[tid], memberof_doc, doc, true);
	aos_assert_r(rslt, false);
	OmnString name, value;
	while (mAttrHashNew[tid].nextEntry(name, value, mark))
	{
		if (value.length() <= 0)
		{
			OmnAlarm << "Value is empty: " << name << enderr;
		}
		else
		{
	    	addAttrIIL(allTrans, arr_len, name, value, docid, memberof_objid, mark, rdata);
		}

		// Chen Ding, 11/13/2011
		// Next need to process attribute word indexing. Whether it needs to 
		// do attribute word indexing is controlled by 'mark'. 
		// The following is incorrect!!!!!!!!!!!!!
		// mWordHashNew[tid].reset();
		// rslt = AosDocProcUtil::collectWords(mWordHashNew[tid], value);
		// aos_assert_r(rslt, false);
		// while(mWordHashNew[tid].nextEntry(word, marknull))
		// {
		// 	mark = setMarkMemberOf(mark);
	    // 	addAttrWordIIL(buffs, name, word, docid, memberof_objid, mark, rdata);
		// }
	}
	return true;
}


bool
AosDocProc::removeMemberOf(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const AosXmlTagPtr doc,
		const AosXmlTagPtr &memberof_doc,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 10/01/2011
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);
	
	// 1. Remvoe words
	mWordHashOld[tid].reset();
	//bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], memberof_doc, doc, false);
	bool rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], memberof_doc, doc, false);
	aos_assert_r(rslt, false);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	OmnString memberof_objid = memberof_doc->getAttrStr(AOSTAG_OBJID, "");
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, false);
	OmnString word;
	AosEntryMark::E mark, marknull;
	while (mWordHashOld[tid].nextEntry(word, mark))
	{
		mark = setMarkMemberOf(mark);
		removeWordIIL(allTrans, arr_len, word, docid, memberof_objid, mark, rdata);
	}

	// 2. Remove attributes: "name=value"
	mAttrHashOld[tid].reset();
	rslt = AosDocProcUtil::collectAttrs(mAttrHashOld[tid], memberof_doc, doc, true);
	aos_assert_r(rslt, false);
	OmnString name, value;
	while (mAttrHashOld[tid].nextEntry(name, value, mark))
	{
	    removeAttrIIL(allTrans, arr_len, name, value, docid, memberof_objid, mark, rdata);

		mWordHashOld[tid].reset();
		//rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], value);
		rslt = AosDocProcUtil::collectWords(mWordHashOld[tid], mWordParser[tid], mWordNorm[tid], value);
		aos_assert_r(rslt, false);
		while(mWordHashOld[tid].nextEntry(word, marknull))
		{
			mark = setMarkMemberOf(mark);
	    	removeAttrWordIIL(allTrans, arr_len, name, word, docid, memberof_objid, mark, rdata);
		}
	}

	return true;
}


bool 
AosDocProc::addSysInfoIIL(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const u64 docid, 
		const OmnString &objid, 
		const u64 ctnr_docid, 
		const OmnString &ctnr_objid, 
		const u32 createtime, 
		const AosRundataPtr &rdata,
		const bool isCtnr)
{
	aos_assert_r(smIILClient, false);
	if (ctnr_objid == "") return true;

	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);

	OmnString iilname;

	// 1. Container create time IIL(u64) value : ctime 
	iilname = AosIILName::composeCtnrMemberCtimeIILName(ctnr_objid); 
	if (smShowLog)
		OmnScreen << "Add ctnr ctime: " << iilname << ":" << createtime 
			<< ":" << docid << endl;
	//smIILClient->addU64ValueDoc(mDocTransId[tid]++, buffs, 
	//		iilname, false, createtime, docid, false, true, rdata);
	smIILClient->addU64ValueDoc(allTrans, arr_len, 
			iilname, createtime, docid, false, true, rdata);

	// 2. Container objid 
	iilname = AosIILName::composeCtnrMemberObjidIILName(ctnr_objid);
	if (smShowLog)
		OmnScreen << "Add ctnr objid: " << iilname << ":" << createtime 
			<< ":" << docid << endl;
	//smIILClient->addStrValueDoc(mDocTransId[tid]++, buffs, 
	//		iilname, false, objid, docid, false, true, rdata);
	smIILClient->addStrValueDoc(allTrans, arr_len, iilname, objid, docid, false, true, rdata);

	// 3. Container Descendant IIL(u64) value : descentant docid
	/*
	if (ctnr_docid > 0)
	{
		OmnString descIILName = AosIILName::composeDescendantIILName(); 
		OmnString ancesIILName = AosIILName::composeAncestorIILName(); 
		if (isCtnr)
		{
			//Container Ancestor IIL(u64) value : ancestor docid
			smIILClient->addAncestor(mDocTransId[tid]++, buffs, 
				ancesIILName, docid, ctnr_docid, rdata);
		}

		smIILClient->addDescendant(mDocTransId[tid]++, buffs, descIILName, 
			ancesIILName, docid, ctnr_docid, rdata);
	}
	*/
	return true;
}



bool 
AosDocProc::removeSysInfoIIL(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const u64 docid, 
		const OmnString &objid, 
		const u64 ctnr_docid, 
		const OmnString &ctnr_objid, 
		const u32 createtime, 
		const AosRundataPtr &rdata,
		const bool isCtnr)
{
	aos_assert_r(smIILClient, false);
	//1. Container create time IIL(u64) value : ctime 
	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// Chen Ding, 01/28/2012
	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	aos_assert_r(tid >= 0 && tid < mNumThreads, false);

	OmnString iilname;
	iilname = AosIILName::composeCtnrMemberCtimeIILName(ctnr_objid); 
	if (smShowLog)
		OmnScreen << "Remove ctnr ctime: " << iilname << ":" 
			<< createtime << ":" << docid << endl;

	//mDocTransId[tid]++;
	//smIILClient->removeU64ValueDoc(mDocTransId[tid]++, buffs,
	//		iilname, createtime, docid, rdata);
	smIILClient->removeU64ValueDoc(allTrans, arr_len,
			iilname, createtime, docid, rdata);

	iilname = AosIILName::composeCtnrMemberObjidIILName(ctnr_objid);
	if (smShowLog)
		OmnScreen << "Remove ctnr objid: " << iilname << ":" 
			<< objid << ":" << docid << endl;
	//smIILClient->removeStrValueDoc(mDocTransId[tid]++,
	//		buffs, iilname, objid, docid, rdata);
	smIILClient->removeStrValueDoc(allTrans, arr_len, iilname, objid, docid, rdata);

	//Container Descendant IIL(u64) value : descentant docid
	/*
	OmnString descIILName = AosIILName::composeDescendantIILName(); 
	OmnString ancesIILName = AosIILName::composeAncestorIILName(); 
	smIILClient->removeDescendant(mDocTransId[tid]++, buffs, descIILName, 
			ancesIILName, docid, ctnr_docid, rdata);
	if (isCtnr)
	{
		//Container Ancestor IIL(u64) valeu : ancestor docid
		smIILClient->removeAncestor(mDocTransId[tid]++, buffs, 
				ancesIILName, docid, ctnr_docid, rdata);
	}
	*/
	return true;
}


bool
AosDocProc::removeContainer(const AosXmlTagPtr &ctnr, const AosRundataPtr &rdata)
{
	return true;
}


bool
AosDocProc::modifyMemberOf(
		//vector<AosBuffPtr> &buffs,
		vector<AosTransPtr>  *allTrans,
		const u32 arr_len,
		const AosXmlTagPtr &origxml,
		const AosXmlTagPtr &newxml,
		const AosRundataPtr &rdata)
{
	OmnString memberofOld = origxml->getAttrStr(AOSTAG_MEMBEROF, "");
	OmnString memberofNew = newxml->getAttrStr(AOSTAG_MEMBEROF, "");
	if (memberofOld != memberofNew)
	{
		// Chen Ding, 10/01/2011
		int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
		// Chen Ding, 01/28/2012
		// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
		aos_assert_r(tid >= 0 && tid < mNumThreads, false);
	
		AosXmlTagPtr tag;
		OmnString part;
		mMemberOfNew[tid].clear();
		mMemberOfOld[tid].clear();
		OmnStrParser1 parserOld(memberofOld, ", ", false, false);       
		while ((part = parserOld.nextWord()) != "")
		{
			mMemberOfOld[tid].insert(part);
		}
		OmnStrParser1 parserNew(memberofNew, ", ", false, false);       
		while ((part = parserNew.nextWord()) != "")
		{
			mMemberOfNew[tid].insert(part);
		}
		set<OmnString>::iterator itrNew;
		set<OmnString>::iterator itrOld = mMemberOfOld[tid].begin();
		while(itrOld != mMemberOfOld[tid].end())
		{
			itrNew = mMemberOfNew[tid].find(*itrOld);
			if (itrNew != mMemberOfNew[tid].end())
			{
				mMemberOfNew[tid].erase(*itrNew);
				itrNew = itrOld;
				++itrOld;
				mMemberOfOld[tid].erase(itrNew);
			}
			else
			{
				++itrOld;
			}	

		}

		AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
		aos_assert_r(docclient, false);
		for (itrNew = mMemberOfNew[tid].begin(); itrNew != mMemberOfNew[tid].end(); ++itrNew)
		{
			AosXmlTagPtr doc = docclient->getDocByObjid(*itrNew, rdata);
			aos_assert_r(doc, false);
			//addMemberOf(buffs, newxml, doc, *itrNew, rdata);
			if (AosSecurityMgrObj::getSecurityMgr()->checkMemberOfListing(*itrNew, 
						newxml->getAttrStr(AOSTAG_PARENTC), rdata))
			{
				// It is allowed.
				OmnString iilname = AosIILName::composeCtnrMemberObjidIILName(*itrNew);
				AosDocProcUtil::addValueToIIL(allTrans, arr_len, iilname, *itrNew, 
						newxml->getAttrU64(AOSTAG_DOCID, 0), false, false, rdata);
			}
		}
		for (itrOld = mMemberOfOld[tid].begin(); itrOld != mMemberOfOld[tid].end(); ++itrOld)
		{
			AosXmlTagPtr doc = docclient->getDocByObjid(*itrOld, rdata);
			aos_assert_r(doc, false);
			//removeMemberOf(buffs, origxml, doc, rdata);
			if (AosSecurityMgrObj::getSecurityMgr()->checkMemberOfListing(
						*itrOld, newxml->getAttrStr(AOSTAG_PARENTC), rdata))
			{
				// It is allowed.
				OmnString iilname = AosIILName::composeCtnrMemberObjidIILName(*itrOld);
				AosDocProcUtil::removeValueFromIIL(allTrans, arr_len, iilname, *itrOld, newxml->getAttrU64(AOSTAG_DOCID, 0),
						 rdata);
			}
		}

	}
	return true;
}

/*
u64 
AosDocProc::createDocTransid()
{
	// Doc transid consists of three parts:
	// 		ModuleID	(2 bytes)
	// 		DocTransID	(3 bytes)
	// 		Seqno		(3 bytes)
	// 'DocTransID' is managed by this client. 
	mLock->lock();
	u64 transid = mCrtDocTransId++;
	mNumDocTransIds--;
	if (mNumDocTransIds == 0)
	{
		loadNewTransId();
	}
	u64 doc_transid = (AosDocClientObj::getDocClient()->getModuleID()) << 48;
	doc_transid += (transid << 24);
	mLock->unlock();
	return doc_transid;
}

bool
AosDocProc::openDocTransIdFile(
			const OmnString &fname)
{
	mFile = OmnNew OmnFile(fname, OmnFile::eReadWrite);
	aos_assert_r(mFile, false);
	if (mFile->isGood()) return true;
	mFile = OmnNew OmnFile(fname, OmnFile::eCreate);
	aos_assert_r(mFile, false);
	if (!mFile->isGood())
	{
		OmnAlarm << "Failed to open file: " << fname << enderr;
		return false;
	}
	return true;
}

bool
AosDocProc::loadNewTransId()
{
	// Each DocProc keeps its current doc trans ID in file. 
	// Every time it loads in mDocTransidInc number of 
	// doc transid. This function loads in mDocTransidInc
	// number of doc transid, and saves the new value 
	// to the file. 

	u64 offset = 0;
	u64 crt_transid = mFile->readU32(offset, 0);
	if (crt_transid != mCrtDocTransId)
	{
		OmnAlarm << "Doc Trans ID mismatch: " << crt_transid 
			<< ":" << mCrtDocTransId << enderr;
	}

	mCrtDocTransId = crt_transid;
	mNumDocTransIds = mDocTransidInc;
	u32 newid = mCrtDocTransId + mNumDocTransIds;
	if (newid >= eMaxDocTransId)
	{
		newid = 1;
	}
	mFile->setU32(offset, mCrtDocTransId + mNumDocTransIds, false);
	return true;
}
*/

// Ketty 2013/02/26
//bool
//AosDocProc::createBuffs(vector<AosBuffPtr> &buffs)
//{
//	for (u32  i=0; i<buffs.size(); i++)
//	{
//		//buffs[i] = OmnNew AosBuff(eBuffSize, eBuffIncSize AosMemoryCheckerArgs);
//		//felicia, 2012/09/26
//		buffs[i] = OmnNew AosBuff(eBuffSize AosMemoryCheckerArgs);
//	}
//	return true;
//}


bool
AosDocProc::sendTrans(
		const AosRundataPtr &rdata,
		vector<AosTransPtr> *allTrans,
		const u32 arr_len)
{
	bool success = true;

	for(u32 i=0; i<arr_len; i++)
	{
		vector<AosTransPtr> & vt = allTrans[i];
		if(vt.size() < 0)   continue;

		bool rslt = AosSendManyTrans(rdata, vt); 
		if (!rslt)
		{
			OmnAlarm << rdata->getErrmsg() << ". Failed sending: " << enderr;
			success = false;
		}
	}
	return success;	
}

OmnString
AosDocProc::getWordIILName(
		const AosEntryMark::E mark,
		const OmnString &ctnr_objid,
		const OmnString &word)
{
	OmnString iilname = "";
	AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);
	switch(mark1)
	{
	case AosEntryMark::eContainerIndex:
	case AosEntryMark::eBothIndex:
		 iilname= AosIILName::composeContainerWordIILName(ctnr_objid, word);
		 break;

	case AosEntryMark::eGlobalIndex:
		 iilname = word;
		 break;

	case AosEntryMark::eCtnrOnly:
		 iilname= AosIILName::composeContainerWordIILName(ctnr_objid, word);
		 break;
		 
	case AosEntryMark::eNoIndex:
		 break;

	default:
		 break;
	}

	return iilname;
}

OmnString
AosDocProc::getAttrIILName(
		const AosEntryMark::E mark,
		const OmnString &ctnr_objid,
		const OmnString &name)
{
	OmnString iilname = "";
	AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);
	switch(mark1)
	{
	case AosEntryMark::eBothIndex:

	case AosEntryMark::eContainerIndex:
		 iilname = AosIILName::composeContainerAttrIILName(ctnr_objid, name);
		 break;

	case AosEntryMark::eGlobalIndex:
		 iilname = AosIILName::composeAttrIILName(name);
		 break;

	case AosEntryMark::eCtnrOnly:
		 iilname = AosIILName::composeContainerAttrIILName(ctnr_objid, name);
		 break;

	case AosEntryMark::eNoIndex:
		 break;

	default:
		 break;
	}

	return iilname;
}

OmnString
AosDocProc::getAttrWordIILName(
		const AosEntryMark::E mark,
		const OmnString &ctnr_objid,
		const OmnString &attrname,
		const OmnString &word)
{
	OmnString iilname = "";
	AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);
	switch(mark1)
	{
	case AosEntryMark::eBothIndex:

	case AosEntryMark::eContainerIndex:
		 iilname = AosIILName::composeContainerAttrWordIILName(ctnr_objid, attrname, word);
		 break;

	case AosEntryMark::eGlobalIndex:
		 iilname = AosIILName::composeAttrWordIILName(attrname, word);
		 break;

	case AosEntryMark::eCtnrOnly:
		 iilname = AosIILName::composeContainerAttrWordIILName(ctnr_objid, attrname, word);
		 break;

	case AosEntryMark::eNoIndex:
		 break;

	default:
		 break;
	}
	return iilname;
}
