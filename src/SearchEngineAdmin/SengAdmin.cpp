////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngineAdmin/SengAdmin.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/Siteid.h"
#include "SEUtil/VersionDocMgr.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/Passwords.h"
#include "SearchEngineAdmin/XmlDoc2.h"
#include "SEClient/SEClient.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "StorageMgr/StorageMgr.h"
#include "StorageMgr/FileKeys.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/StrParser.h"
#include "Util/StrSplit.h"
#include "Util/Buff.h"
#include "Util/Opr.h"
#include "Util/HashUtil.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/XmlDoc.h"   
#include "XmlUtil/SeXmlUtil.h"
#include "SEUtil/Docid.h"

static OmnString sgCidCtnr;
static OmnString sgTempLostFoundLogin;
static OmnString sgTempLogCtnr;
static OmnString sgTempLogCtnrLogin;
static OmnString sgTempLogCtnrInvRead;
static OmnString sgTempRootUser;
static OmnString sgTempSysUserCtnr;
static OmnString sgTempRootCtnr;
static OmnString sgTempRoot;
static OmnString sgTempSysRoot;
static OmnString sgTempGuest;
static OmnString sgTempPubNameSpace;
static OmnString sgTempUserDomainCtnr;
static OmnString sgTempSysCtnrObjidPrefix;
static OmnString sgTempUserObjidPrefix;
static int sgInvalidObjid = 0;


OmnSingletonImpl(AosSengAdminSingleton,
                 AosSengAdmin,
                 AosSengAdminSelf,
                "AosSengAdmin");

OmnFilePtr	AosSengAdmin::mIdxFiles[eMaxIdxFiles];
OmnFilePtr	AosSengAdmin::mDocFiles[eMaxIdxFiles];
static AosRundataPtr	 sgRundata;
static u32 sgNumVirtuals;
//static OmnString sgAppFileKey = AOSFILEKEY_DOC_STORAGE_APP;

// will no use	Ketty 2011/11/28
//OmnString			AosSengAdmin::mDirname;
//OmnString			AosSengAdmin::mIdxFname;
//OmnString			AosSengAdmin::mDocFname;

static OmnString sgImagePath;
static int sgFlag = 0;
const int sgGroupSize = 1000;
const int sgSleepLength = 0;

AosSengAdmin::AosSengAdmin()
:
mLock(OmnNew OmnMutex()),
mRemotePort(-1),
mTransId(eInitTransId),
mCondVar(OmnNew OmnCondVar())
{
}


AosSengAdmin::~AosSengAdmin()
{
}


bool      	
AosSengAdmin::start()
{
	// Chen Ding, 09/30/2011
	OmnThreadedObjPtr thisPtr(this, false);
	for (int i=0; i<eMaxThreads; i++)
	{
		mThreads[i] = OmnNew OmnThread(thisPtr, "SengAdminThrd", 0, true, true, __FILE__, __LINE__);
		mThreads[i]->start();
	}
	return true;
}


bool        
AosSengAdmin::stop()
{
	return true;
}


bool
AosSengAdmin::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	mRemoteAddr = config->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	mRemotePort = config->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);

	mConn = OmnNew AosSEClient(mRemoteAddr, mRemotePort);
	//u32 maxdocfiles = config->getAttrU64(AOSCONFIG_XMLDOC_MAX_DOCFILES, 0);

	//AosXmlTagPtr def = config->getFirstChild("convert_info");
	//if (def)
	//{
		//mDocFname = def->getAttrStr(AOSCONFIG_DOC_FILENAME);
		//mIdxFname = def->getAttrStr(AOSCONFIG_IDX_FILENAME);
		//mDirname = def->getAttrStr(AOSCONFIG_DIRNAME);
		//mTargetDirname = def->getAttrStr(AOSCONFIG_TARGETDIRNAME);

		//mOrigDocReader = OmnNew AosDocFileMgr(maxdocfiles, mDirname, mDocFname);
	//}
	//AosXmlTagPtr svrconfig = config->getFirstChild("docserver");
	//if (svrconfig)
	//{
	//	sgNumVirtuals = svrconfig->getAttrInt(AOSTAG_NUM_VIRTUALS, 0);
	//}
	

	// Ketty 2013/01/24 temp.
	//sgNumVirtuals = AosGetNumCubes();
	//aos_assert_r(sgNumVirtuals > 0, false);
	sgRundata = OmnApp::getRundata();

	
	return true;
}


bool    
AosSengAdmin::signal(const int threadLogicId)
{
	return true;
}


bool    
AosSengAdmin::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosSengAdmin::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	static int lsTotal = 0;

	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mRequests.size() <= 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		ReqStruct req = mRequests.front();
		mRequests.pop();
	//	OmnScreen << "To Send: " << req.mDoc->toString() << endl;
		OmnScreen << "To process: " << lsTotal++ << ":" << mRequests.size() << endl;
		mLock->unlock();

		switch (req.mType)
		{
		case eCreateDoc:
			 sendCreateObjReq(req.mSiteid, (char *)req.mDoc->getData());
			 break;
		
		case eCreateArcd:
			 {
				AosXmlTagPtr doc = req.mDoc;
				aos_assert_r(doc, false);
				u64 owndocid = doc->getAttrU64("zky_owndocid", 0);
				aos_assert_r(owndocid>0, false);
				ModifyAccessRcd(req.mSiteid, doc, owndocid, "", 0); 
	    		break;
			 }

		default:
			 OmnAlarm << "Unrecognized request: " << req.mType << enderr;
		}
	}

	return true;
}


bool
AosSengAdmin::rebuildVersion(const u32 seqno, OmnFile &file)
{
	// Versions are stored in files "version_xxx", where 
	// "xxx" are a seqno. Version files contain sequence of 
	// records. Each record is in the format:
	// 	Byte 0-3:	Doc size
	// 	Byte 4-: 	The doc itself
	// The first four bytes are encoded directly as binary.
    u32 offset = AosDocFileMgrObj::getReservedSize();
	AosXmlParser parser;
	int group = 0;
	while(1)
	{
		if (group >= sgGroupSize)
		{
			OmnSleep(sgSleepLength);
			group = 0;
		}

		int docsize = file.readBinaryInt(offset, -1);
		if (docsize < 0 || docsize > eMaxDocSize) 
		{
			if (docsize == -1 ) return true;
			OmnAlarm << "Invalid docsize:" << docsize << enderr;
			return false;
		}
		char *buff = OmnNew char[docsize+10];
		int bytesread = file.readToBuff(offset + 4, docsize, buff);
		aos_assert_r(bytesread == docsize, false);
		buff[docsize] = 0;
		AosXmlTagPtr doc = parser.parse(buff, "" AosMemoryCheckerArgs);
		if (!doc)
		{
			OmnAlarm << "Failed to parse the doc: " << docsize << ":" 
				<< buff << enderr;
		}
		else
		{
			u64 docid = doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
			if (docid == AOS_INVDID)
			{
				OmnAlarm << "Invalid docid: " << docid 
					<< ". The doc: " << buff << enderr;
			}
			else
			{
				OmnString version = doc->getAttrStr(AOSTAG_VERSION);
				if (version != "")
				{
					OmnString args;
					//args << docid << ":" << version << ":" << seqno << ":" << offset;
					args << docid << ":" << version ;
					bool rslt = sendServerReq(mSiteid, "rebuildver", args, doc);
					group ++;
					aos_assert_r(rslt, false);
					OmnScreen << "version: " << version << ":" << docid << endl;
				}
				else
				{
					OmnScreen << "version empty: " << docid << endl;
				}
			}
		}

		OmnDelete [] buff;
		offset += 4 + docsize;
	}

	return true;
}


bool
AosSengAdmin::rebuildVersions(const int version_start, const int version_end)
{
	// Ketty 2011/11/28
	/*
	if (version_start < 0) return true;
	for (int i=version_start; i<=version_end; i++)
	{
		OmnString fname = mDirname;
		fname << "/version_" << i;
		if (!OmnFile::fileExist(fname)) return false;
		
		OmnFile file(fname, OmnFile::eReadOnly);
		if (!file.isGood())
		{
			OmnAlarm << "File not good: " << fname << enderr;
			continue;
		}
		if(!rebuildVersion(i, file)) return false;
	}
	*/
	return true;
}


bool
AosSengAdmin::showOne(
		const u64 &docid,
		const u32 siteid,
		const OmnString &dirname,
		const OmnString &idxFname,
		const OmnString &docFname)
{
	// It erases everything from the data directory except the docfile,
	// reads all the docs in the docfile, and re-create the doc.
	//
	// 'idxFname' identifies the doc index file name, which is in the
	// form:
	// 		<idxFname>_nnn
	// where 'nnn' is a sequence number, starting from 0.
	//
	// Similarly, 'docFname' identifies the doc files:
	// 		<docFname>_nnn
	//
	// for (u64 docid=1; docid<1010; docid++)

	// Ketty 2011/11/28
	/*
	OmnConnBuffPtr docbuff;
	mIdxFname = idxFname;
	mDocFname = docFname;
	AosXmlParser parser;
	mSiteid = siteid;

	OmnScreen << "Read doc: " << docid << endl;
	DocStatus status;
	if (!readDoc(docid, docbuff, false, status)) return false;
	AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
	if (!doc)
	{
		OmnAlarm << "Failed to parse the doc: "
			<< docid << ":" << docbuff->getData() << enderr;
	}
	else
	{
		cout << "-------------------------------" << endl;
		cout << doc->toString()<< endl;
		cout << "-------------------------------" << endl;
	}
	*/
	return true;
}

bool
AosSengAdmin::addOne(
		const u64 &docid,
		const u32 siteid,
		const OmnString &dirname,
		const OmnString &idxFname,
		const OmnString &docFname)
{
	// It erases everything from the data directory except the docfile,
	// reads all the docs in the docfile, and re-create the doc.
	//
	// 'idxFname' identifies the doc index file name, which is in the
	// form:
	// 		<idxFname>_nnn
	// where 'nnn' is a sequence number, starting from 0.
	//
	// Similarly, 'docFname' identifies the doc files:
	// 		<docFname>_nnn
	//
	// for (u64 docid=1; docid<1010; docid++)

	// Ketty 2011/11/28
	/*
	OmnConnBuffPtr docbuff;
	mIdxFname = idxFname;
	mDocFname = docFname;
	AosXmlParser parser;
	mSiteid = siteid;

	OmnScreen << "Read doc: " << docid << endl;
	DocStatus status;
	if (!readDoc(docid, docbuff, false, status)) return false;
	AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
	if (!doc)
	{
		OmnAlarm << "Failed to parse the doc: "
			<< docid << ":" << docbuff->getData() << enderr;
	}
	else
	{
		convertDataForRebuild(doc);
		sendCreateObjReq(siteid, (char *)doc->getData());
	}
	*/
//	rebuildVersions(eMaxVersionSeqno);
//	rebuildLogs(eMaxVersionSeqno);
	return true;
}


bool
AosSengAdmin::incrementalRebuild(
		const u32 siteid, 
		const u64 start_docid,
		const u64 max_docid,
		const int version_start, 
		const int version_end, 
		const int log_start,
		const int log_end)
{
	bool rslt = rebuildDb(start_docid, max_docid, version_start, 
			version_end, log_start, log_end, siteid);
	aos_assert_r(rslt, false);
	OmnScreen << "Build finished: "
		<< "\n    Last Docid: " << mLastDocid << endl;
	return true;
}


bool
AosSengAdmin::sendDocMultiTimes(
		const u64 &docid,
		const u64 &times,
		const int version_start,
		const int version_end,
		const int log_start,
		const int log_end,
		const u32 siteid)
{
	aos_assert_r(siteid > 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	mSiteid = siteid;
	int group = 0;
	mLastDocid = 0;

	OmnString ssid;                                                                                                            
	u64 userid;
	AosXmlTagPtr userdoc;
	u64 urldocid;
	bool rslt = login("chending", 
					  "chen0318", 
					  "yunyuyan_account", 
					  ssid, 
					  urldocid,
					  userid, 
					  userdoc, 
					  siteid, 
					  "");
	aos_assert_r(rslt, false);

	OmnScreen << "Read doc: " << docid << endl;
	DocStatus status;
	if (!readDoc(docid, docbuff, false, status, rdata)) return false;
	AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
	if (!doc)
	{
		OmnAlarm << "Failed to parse the doc: " 
			<< docid << ":" << docbuff->getData() << enderr;
		return false;
	}
	
	if (doc->getAttrStr(AOSTAG_OBJID) == "")
	{
		OmnAlarm << "Doc missing objid: " << doc->toString() << enderr;
		return false;
	}

	if (doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == AOS_INVDID)
	{
		OmnAlarm << "Doc missing docid: " << doc->toString() << enderr;
		return false;
	}

	doc->removeAttr(AOSTAG_DOCID, 1, true);
	doc->removeAttr(AOSTAG_OBJID, 1, true);

	if (doc->getAttrStr(AOSTAG_OTYPE) == "")
	{
		if (doc->toString() == "")
		{
			return false;
		}

		// Check whether it is a VPD. We assume that if the doc contains
		// the word 'gic_creators' and 'gic_type', it is a VPD.
		if (doc->hasChild("panel", true) ||
			doc->hasChild("gic_creators", true))
		{
			// It is a vpd
			doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_VPD);
		}
		else if ((doc->hasChild("vpd", true) && doc->hasAttribute("gic_type", true)) ||
		         (doc->hasAttribute("zky_pctrs", "giccreators", true) && 
				  doc->hasAttribute("gic_type", true)))
		{
			doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_VPD);
		}
		else
		{
			doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_UNKNOWN);
		}
	}

	convertDataForRebuild(doc);

	// Chen Ding, 10/11/2010
	// For all docs whose otype is 'zky_ssion', we will not convert it anymore.
	// In the future, there shall be no such docs.
	if (doc->getAttrStr(AOSTAG_OTYPE) == "zky_ssion")
	{
		OmnScreen << "Session Doc found: " << doc->toString() << endl;
		return false;
	}
	
	for (int i=0; i<(int)times; i++)
	{
		OmnScreen << "Add Doc Num:      " << i << endl;
		if (group >= sgGroupSize)
		{
			if (docid > 5000)
			{
				OmnSleep(sgSleepLength);
			}
			group = 0;
		}
//cout << (char *)doc->getData() << endl;
		sendCreateObjReq2(siteid, 
						  (char *)doc->getData(),
						  ssid,
						  urldocid);
		OmnScreen << "Add Doc Num Succ: " << i << endl;
		group++;
	}

	rebuildVersions(version_start, version_end);
	rebuildLogs(log_start, log_end);
	return true;
}


bool
AosSengAdmin::rebuildDb(
		const u64 &startDocid,
		const u64 &maxDocid,
		const int version_start,
		const int version_end,
		const int log_start,
		const int log_end,
		const u32 siteid)
{
	// It erases everything from the data directory except the docfile, 
	// reads all the docs in the docfile, and re-create the doc.
	// 
	// 'idxFname' identifies the doc index file name, which is in the
	// form:
	// 		<idxFname>_nnn
	// where 'nnn' is a sequence number, starting from 0. 
	//
	// Similarly, 'docFname' identifies the doc files:
	// 		<docFname>_nnn
	//
	// for (u64 docid=1; docid<1010; docid++)
	//u64 maxDocidd = maxDocid;
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	aos_assert_r(siteid > 0, false);
	rdata->setSiteid(siteid);

	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	// Chen Ding, 08/12/2011
	// Will handle the version rebuild later
	// AosVersionMgrPtr versionMgr = OmnNew AosVersionMgr();
	mSiteid = siteid;
	int group = 0;
	u64 docid = 0;
	mLastDocid = 0;
	for (docid=startDocid; docid<=maxDocid; docid++)
	{
		rdata->resetError();
		if (group >= sgGroupSize)
		{
			if (docid > 5000)
			{
		//		OmnSleep(sgSleepLength);
			}
			group = 0;
		}
		
		AosXmlTagPtr doc;
		DocStatus status = eGood;
		if (!(doc=readDoc(docid, rdata))) 
		{
			//OmnScreen << "-------: Failed reading: " << docid << endl;
			continue;
		}
		
		//Zky2782 Ketty 2011/02/14
		if(status == eBad || status == eDeleted)
		{
			// Chen Ding, 08/12/2011
			// retrieve doc from version iil, get the the doc from the fisrt entry.
			// if doc retrieved, get the doc's objid, send a request to server. the 
			//server add the doc to the deleted iil.

			// AosXmlTagPtr vDoc = versionMgr->getLastVersionObj(docid, rdata);
			//	
			// OmnString objid;
			// if(vDoc)
			// {
			// 	// get the objid
			// 	objid = vDoc->getAttrStr(AOSTAG_OBJID, "");
			// 	// 4. send a request to server, (objid, docid)
			// 	sendDeleteObjReq(siteid, objid, docid);
			// 	status = eDeleted;
			// }
			if (status == eBad)
			{
				OmnScreen << "-------: Bad" << endl;
			}
			else 
			{
				OmnScreen << "-------: Deleted" << endl;
			}
			continue;
		}
		//-----------------------Ketty End---------------

		//AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
		if (!doc)
		{
			OmnAlarm << "Failed to parse the doc: " 
				<< docid << ":" << docbuff->getData() << enderr;
			continue;
		}
		
		if (doc->getAttrStr(AOSTAG_OBJID) == "")
		{
			OmnAlarm << "Doc missing objid: " 
				<< doc->toString() << enderr;
			continue;
		}

		if (doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == AOS_INVDID)
		{
			OmnAlarm << "Doc missing docid: " << doc->toString() << enderr;
			continue;
		}

		doc->normalize();
		if (doc->getAttrStr(AOSTAG_OTYPE) == "")
		{
			if (doc->toString() == "")
			{
				OmnAlarm << "Empty doc: " << docid << enderr;
				continue;
			}

			// Check whether it is a VPD. We assume that if the doc contains
			// the word 'gic_creators' and 'gic_type', it is a VPD.
			if (doc->hasChild("panel", true) ||
				doc->hasChild("gic_creators", true))
			{
				// It is a vpd
				doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_VPD);
			}
			else if ((doc->hasChild("vpd", true) && doc->hasAttribute("gic_type", true)) ||
			         (doc->hasAttribute("zky_pctrs", "giccreators", true) && 
					  doc->hasAttribute("gic_type", true)))
			{
				doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_VPD);
			}
			else
			{
				doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_UNKNOWN);
			}
		}

		/*
		if (doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD)
		{
			OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
			u64 docid = doc->getAttrU64(AOSTAG_OWNER_DOCID, 0);
			OmnString did;
			did << docid;
			if (objid.findSubString(did, 0) == -1)
			{
				OmnScreen << objid << "," << docid << endl;
			}
			ModifyAccessRcd(siteid, doc, doc->getAttrU64(AOSTAG_OWNER_DOCID, 0), "");
			OmnScreen << "-------: Modify access record" << endl;
			continue;
		}
		*/
		convertDataForRebuild(doc);

		AosXmlTagPtr arcd;

doc->removeAttr("zky_ver__a");
doc->removeAttr("zky_ver__n");
doc->removeAttr("zky_ver");

		// Chen Ding, 10/11/2010
		// For all docs whose otype is 'zky_ssion', we will not convert it anymore.
		// In the future, there shall be no such docs.
		if (doc->getAttrStr(AOSTAG_OTYPE) == "zky_ssion")
		{
			OmnScreen << "------- Ignored (session): " << doc->toString() << endl;
			continue;
		}
		
		addReq(eCreateDoc, siteid, doc);

		if ((arcd=readArcd(docid, rdata)))
		{
arcd->removeAttr("zky_ver__a");
arcd->removeAttr("zky_ver__n");
arcd->removeAttr("zky_ver");
		//	ModifyAccessRcd(siteid, arcd, arcd->getAttrU64(AOSTAG_OWNER_DOCID, 0), "");
			addReq(eCreateArcd, siteid, arcd);
		}

		mLastDocid = doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(mLastDocid, false);
		group++;
	}
	
	while (true)
	{
		if (!mRequests.empty()) 
		{
			OmnSleep(1);
			continue;
		}
		while (!mRequests1.empty())
		{
			mLock->lock();
			mRequests.push(mRequests1.front());
			mRequests1.pop();
			mCondVar->signal();	
			mLock->unlock();
		}
		break;
	}	
	
	while (true)
	{
		if (!mRequests.empty()) 
		{
			OmnSleep(1);
			continue;
		}
		break;
	}

	char c = '0';
	cout << "Do you want rebuild verion or log?[Y/N]" << endl;
	while(cin >> c)
	{
		if (c == 'Y' || c == 'y')
			break;
		if (c == 'N' || c == 'n')
			return true;
		cout << "Pleas Input Y or N !" << endl;
	}
	rebuildVersions(version_start, version_end);
	rebuildLogs(log_start, log_end);
	
	return true;
}


//Zky2782 Ketty 2011/02/14
bool
AosSengAdmin::sendDeleteObjReq(
		const u32 siteid,
		const OmnString &objid,
		const u64 &docid, 
		const u64 &urldocid)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">rebuildDelObj</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"" << AOSTAG_OBJID << "\">" << objid << "</item>"
		<< "<item name=\"" << AOSTAG_DOCID << "\">" << docid << "</item>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}

bool
AosSengAdmin::sendCreateObjReq(
		const u32 siteid, 
		const char *docstr)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << docstr << "</objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	OmnString docid = child->xpathQuery("Contents/zky_docid", exist, "");
	if (docid == "")
	{
		OmnAlarm << "Failed processing: " << resp << enderr;
		return false;
	}

	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		OmnScreen <<  child->toString() << endl;
	}
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


u64
AosSengAdmin::sendCreateObjReq2(
		const u32 siteid, 
		const AosXmlTagPtr &doc,
		const OmnString &ssid,
		const u64 &urldocid)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>";
//		<< "<item name=\"zky_ssid_" << urldocid << "\">" << ssid << "</item>"
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}

	req << "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << doc->toString() << "</objdef></request>";
//cout << req << endl;	
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	OmnString respcode = child->xpathQuery("status/code", exist, "");
	if(respcode != "200")
	{
		OmnScreen << "Fail to create obj:  " << respcode << endl;
	}
	OmnString docid = child->xpathQuery("Contents/zky_docid", exist, "");
	u64 did = (u64)atoll(docid.data());
	aos_assert_r(did > 0, false);
	return did;
}


bool
AosSengAdmin::sendCreateObjReq2(
		const u32 siteid, 
		const char *docstr,
		const OmnString &ssid,
		const u64 &urldocid)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	//<< "<item name=\"zky_ssid_" << urldocid << "\">" << ssid << "</item>"
	req << "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << docstr << "</objdef></request>";
//cout << req << endl;	
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	OmnString docid = child->xpathQuery("Contents/zky_docid", exist, "");
	//aos_assert_r(docid != "", false);
	OmnString respcode = child->xpathQuery("status/code", exist, "");
	//aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	if(respcode != "200")
	{
		OmnScreen << "Fail to create obj:  " << respcode << endl;
	}
	return true;
}


bool
AosSengAdmin::sendModifyReq(
		const u32 siteid, 
		const char *docstr)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	OmnString req = "<request>";
	req << "<item name=\"operation\">modifyObj</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<xmlobj>" << docstr << "</xmlobj>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosSengAdmin::sendModifyReq(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &docstr, 
		const AosRundataPtr &rdata,
		const bool check_error)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">modifyObj</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<xmlobj>" << docstr << "</xmlobj>";

	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
			<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
			<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
			<< "</request>";
	}
			
	OmnString errmsg;
	OmnString resp;
	rdata->setOk();
	if (!mConn)
	{
		rdata->setError() << "No connection to server!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		rdata->setError() << "Failed processing the request. Errmsg: "
			<< errmsg;
		OmnAlarm << rdata->getErrmsg();
		return false;
	}

	if (resp == "")
	{
		rdata->setError() << "Failed receiving response!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Invalid response: " << resp;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool exist;
	if (!check_error)
	{
		if (child->xpathQuery("status/code", exist, "") != "200")
		{
			return false;
		}
	}

	if (child->xpathQuery("status/code", exist, "") != "200") 
	{
		if (child->xpathQuery("status/code", exist, "") != "200") 
		{
			rdata->setError() << "Failed processing request. Error code: "
				<< child->xpathQuery("status/code", exist, "");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	rdata->setOk();
	return true;
}


bool
AosSengAdmin::rereadDocs(
		const u32 siteid, 
		const u64 &max_docid,
		const OmnString &idxfname, 
		const OmnString &docfname)
{
	// This function re-reads all the docs and sends a modify request
	// to the server. This can be useful if the stored doc has some
	// syntax errors that can be fixed by the xml parser. 
	
	// Ketty 2011/11/28
	/*
	mIdxFname = idxfname;
	mDocFname = docfname;
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	AosXmlTagPtr doc;
	DocStatus status;
	for (u64 docid=1001; docid<max_docid; docid++)
	{
		OmnScreen << "Read doc: " << docid << endl;
		if (!readDoc(docid, docbuff, false, status)) continue;
		doc = parser.parse(docbuff->getData(), "" AosMemoryCheckerArgs);
		if (!doc) continue;
		sendModifyReq(siteid, (char *)doc->getData());
	}
	*/
	return true;
} 

AosXmlTagPtr
AosSengAdmin::readArcd(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
	
	/*
	OmnConnBuffPtr docbuff = 0;
	u32 arcd_seqno, u1;
	u64 arcd_offset, arcd_size, u2, u3, compressed_size;
	DocStatus status;
	bool rslt = readHeaderVersion201102(docid, u1, 
			u2, u3, compressed_size, arcd_seqno, arcd_offset, arcd_size, status, rdata);

	if (!rslt || arcd_size==0)
	{
		return NULL;
	}

	//zky2782 Ketty 2011/02/16
	if (status == eBad || status == eDeleted)
	{
		// The doc is deleted. 
		return NULL;
	}

	// Ready to read the doc itself
	docbuff = OmnNew OmnConnBuff(arcd_size);
	aos_assert_r(docbuff->determineMemory(arcd_size), 0);
	char *data = docbuff->getBuffer();
	//rslt = mOrigDocReader->readDoc(arcd_seqno, arcd_offset, data, arcd_size);
	//AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrNew(docid, rdata);
	aos_assert_r(docfilemgr, NULL);
	compressed_size = 0;
	rslt = docfilemgr->readDoc(arcd_seqno, arcd_offset, data, arcd_size, compressed_size, sgRundata);
	if (!rslt)
	{
		OmnAlarm << "Failed to read doc: " << docid << enderr;
		return NULL;
	}
	docbuff->setDataLength(arcd_size);
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(docbuff, "" AosMemoryCheckerArgs);	
	return xml;
	//OmnNotImplementedYet;
	return 0;
	*/
}


bool
AosSengAdmin::readDoc(
		const u64 &docid,
		OmnConnBuffPtr &docbuff, 
		const bool readDeleted, 
		DocStatus &status, 
		const AosRundataPtr &rdata)
{
	docbuff = 0;
	u32 docseqno, u1;
	u64 docoffset, docsize, u2, u3, compressed_size;
	bool rslt = readHeaderVersion201102(docid, docseqno, 
			docoffset, docsize, compressed_size, u1, u2, u3, status, rdata);
	if (!rslt)
	{
		return false;
	}

	//zky2782 Ketty 2011/02/16
	if (status == eBad || status == eDeleted)
	{
		// The doc is deleted. 
		if (!readDeleted) return false;
	}

	// Ready to read the doc itself
	docbuff = OmnNew OmnConnBuff(docsize);
	aos_assert_r(docbuff->determineMemory(docsize), 0);
	// char *data = docbuff->getBuffer();
	//rslt = mOrigDocReader->readDoc(docseqno, docoffset, data, docsize);
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	aos_assert_r(docfilemgr, false);
	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
	//rslt = docfilemgr->readDoc(docseqno, docoffset, data, docsize, compressed_size, sgRundata);
	if (!rslt)
	{
		//OmnAlarm << "Failed to read doc: " << docid << enderr;
		return false;
	}
	docbuff->setDataLength((u32)docsize);
	return true;

	/*
	int bytesread = docfile->readToBuff(
			docoffset + AosDocFileMgr::eDocOffset, len, data);
	if (bytesread != len)
	{
		OmnAlarm << "Data incorrect: " << bytesread << ":" << len
			<< ":" << docseqno << ":" << docoffset << ":" << docid << enderr;
		return false;
	}
	docbuff->setDataLength(docsize);
	return true;
	*/
}


OmnFilePtr
AosSengAdmin::openIdxFile(const u32 seqno)
{
	// Ketty 2011/11/28
	/*
	aos_assert_r(seqno < eMaxIdxFiles, 0);
	if (mIdxFiles[seqno]) return mIdxFiles[seqno];

	OmnString idxfn = mDirname;
	idxfn << "/" << mIdxFname << "_" << seqno;
	OmnFilePtr ff = OmnNew OmnFile(idxfn, OmnFile::eReadWrite);
	aos_assert_r(ff, 0);
	if(!ff->isGood())
	{
		return NULL;
	}
	mIdxFiles[seqno] = ff;
	return ff;
	*/
	return 0;
}


OmnFilePtr
AosSengAdmin::openDocFile(const u32 seqno)
{
	// Ketty 2011/11/28
	/*
	aos_assert_r(seqno < eMaxDocFiles, 0);
	if (mDocFiles[seqno]) return mDocFiles[seqno];

	OmnString docfn = mDirname;
	docfn << "/" << mDocFname << "_" << seqno;
	OmnFilePtr ff = OmnNew OmnFile(docfn, OmnFile::eReadWrite);
	aos_assert_r(ff && ff->isGood(), 0);
	mDocFiles[seqno] = ff;
	return ff;
	*/
	return 0;
}


AosXmlTagPtr
AosSengAdmin::readDocByDocid(
		const u64 &docid, 
		const u32 siteid) 
{
	aos_assert_r(siteid != 0, 0);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);
	AosXmlTagPtr doc = AosSengAdmin::getSelf()->readDoc(docid, rdata);
	if (!doc)
	{
		OmnAlarm << "Failed to read doc: " << docid << enderr;
		return 0;
	}
OmnScreen  << "Doc: " << doc->toString() << endl;
	return doc;
}


bool
AosSengAdmin::rebuildDoc(
		const u64 &docid,
		const u32 siteid,
		const OmnString &idxFname, 
		const OmnString &docFname)
{
	// It erases everything from the data directory except the docfile, 
	// reads all the docs in the docfile, and re-create the doc.
	// 
	// 'idxFname' identifies the doc index file name, which is in the
	// form:
	// 		<idxFname>_nnn // where 'nnn' is a sequence number, starting from 0. 
	//
	// Similarly, 'docFname' identifies the doc files:
	// 		<docFname>_nnn
	//
	// for (u64 docid=1; docid<1010; docid++)
	
	// Ketty 2011/11/28
	//OmnConnBuffPtr docbuff;
	//mIdxFname = idxFname;
	//mDocFname = docFname;
	//DocStatus status;
	//if (!readDoc(docid, docbuff, false, status)) return false;
	//sendCreateObjReq(siteid, docbuff->getData());
	return true;
}


AosXmlTagPtr
AosSengAdmin::retrieveDoc(
		const u32 siteid, 
		const OmnString &objid,
		const OmnString &ssid,
		const u64 &urldocid, 
		const OmnString &isInEditor,
		const AosXmlTagPtr &cookies)
{
	OmnString req = "<request><item name='zky_siteid'><![CDATA[";
	req << siteid << "]]></item>"
		<< "<item name='operation'><![CDATA[retrieve]]></item>"
		<< "<item name='username'><![CDATA[nonameyet]]></item>";
	if (urldocid!= 0)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	//<< "<item name=\"zky_ssid\">" << sessionId << "</item>"
	req	<< "<item name=\"zky_isineditor\">" << isInEditor << "</item>"
		<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
		<< "<item name='objid'><![CDATA[" 
		<< objid << "]]></item>"
		<< "<item name='trans_id'><![CDATA[53]]></item>";
	//--Ketty
	if(cookies)
	{
	    req << cookies->toString();
	}
	req << "</request>";
	
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(resp, "" AosMemoryCheckerArgs);
	return doc;
}


bool
AosSengAdmin::readDocByObjid(
		const OmnString &objid,
		const u64 &maxDocid,
		const u32 siteid,
		const bool readDeleted)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnConnBuffPtr docbuff;
	OmnString oid;
	int num_reads = 0;
	AosXmlTagPtr doc;
	OmnScreen << "Reading doc by objid: " << objid << ":" << maxDocid << ":" << siteid << endl;
	for (u64 docid=1; docid<maxDocid; docid++)
	{
		// OmnScreen << "Read doc: " << docid << endl;
		if (!(doc=AosSengAdmin::getSelf()->readDoc(docid, rdata)))
		{
			continue;
		}
	
		oid = doc->getAttrStr(AOSTAG_OBJID);
		if (oid == objid)
		{
			num_reads++;
			if (num_reads > 1)
			{
				OmnAlarm << "Duplicated docs are read: " << enderr;
				OmnScreen << "Doc read: " << doc->toString() << endl;
			}
			OmnScreen << "Doc read: " << doc->toString() << endl;
		}
	}
	return true;
}


bool
AosSengAdmin::convertDataForRebuild(const AosXmlTagPtr &doc)
{
	// doc->removeAttr("zky_csdate", 1, true);
	// doc->removeAttr("zky_msdate", 1, true);
	// doc->removeAttr("zky_cdate", 1, true);
	// doc->removeAttr("zky_mdate", 1, true);

	// Chen Ding, 05/31/2010
	doc->removeAttr("bt_query_value", 50, false);
	doc->removeAttr("bt_text_align_x", 50, false);
	doc->removeAttr("bt_text_align_y", 50, false);

	OmnString creator = doc->getAttrStr(AOSTAG_CREATOR);
	if (creator == "guest" || creator == "system")
	{
		doc->removeAttr(AOSTAG_CREATOR);
	}

	// Chen Ding, 10/13/2010
	// For access records, change all read permits to public
	if (doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD)
	{
		doc->setAttr(AOSTAG_READ_ACSTYPE, AOSACTP_PUBLIC);
		doc->setAttr(AOSTAG_COPY_ACSTYPE, AOSACTP_PUBLIC);
	}
	// Need to review
	// convertImageDoc(doc);
	cleanData(doc);
	return true;
}


bool
AosSengAdmin::convertImageDoc(const AosXmlTagPtr &doc)
{
	// If it is an image doc, which is determined by AOSTAG_OTYPE == AOSOTYPE_IMAGE,
	// check whether AOSTAG_ORIG_FNAME is set. If not, this is an old version. 
	// Need to retrieve AOSTAG_TNAIL, which is in the format:
	// 		<dirname>/<type> + 't' + nnn + "." + Extension
	//
	// 1. Set AOSTAG_IMGDIR to $(OpenLzHome) + <dirname>
	// 2. Set AOSTAG_ORIG_FNAME to <type> + 'o' + nnn + '.' + extension
	if (doc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_IMAGE) return true;
	OmnString ofname = doc->getAttrStr(AOSTAG_ORIG_FNAME);
	if (ofname != "") return true;

	OmnString tnail = doc->getAttrStr(AOSTAG_TNAIL);
	if (tnail == "") return true;

	const char *data = tnail.data();
	const int len = tnail.length();

	// Find the '/'. 
	int slashidx = -1;
	for (int i=1; i<len; i++)
	{
		if (data[i] == '/')
		{
			slashidx = i;
			break;
		}
	}
	aos_assert_r(slashidx > 0, false);

	OmnString dname(data, slashidx);

	char type = data[slashidx+1];
	if (data[len-4] != '.') return true;
	OmnString extension(&data[len-3]);
	OmnString nnn(&data[slashidx + 3], len - 4 - slashidx);

	aos_assert_r(sgImagePath != "", false);
	OmnString dirname = sgImagePath;
	dirname << "/" << dname;
	doc->setAttr(AOSTAG_IMGDIR, dirname);

	ofname = "";
	ofname << type << "o" << nnn << "." << extension;
	doc->setAttr(AOSTAG_ORIG_FNAME, ofname);
	doc->removeAttr("unique", false, true);

	return true;
}


bool
AosSengAdmin::setImagePath(const OmnString &path)
{
	sgImagePath = path;
	return true;
}


bool
AosSengAdmin::convertParentContners(
		const u64 &maxDocid,
		const u32 siteid,
		const OmnString &idxFname, 
		const OmnString &docFname)
{
	/*
	OmnConnBuffPtr docbuff;
	mIdxFname = idxFname;
	mDocFname = docFname;
	AosXmlParser parser;
	for (u64 docid=1; docid<maxDocid; docid++)
	{
		OmnScreen << "Read doc: " << docid << endl;
		if (!readDoc(docid, docbuff)) continue;
		AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
		if (!doc)
		{
			OmnAlarm << "Failed to parse the doc: " 
				<< docid << ":" << docbuff->getData() << enderr;
		}
		else
		{
			OmnString ctnrs = doc->getAttrStr(AOSTAG_PUB_CONTAINER);
			OmnString ctnrs = doc->getContainer();
			if (ctnrs != "")
			{
				OmnStrParser parser(ctnrs, ", " AosMemoryCheckerArgs);
				parser.reset();
				bool isFirst = true;
				OmnString remaining;
				while (parser.hasMore())
				{
					OmnString word = parser.nextWord();
					aos_assert_r(word != "", false);
					if (isFirst)
					{
						isFirst = false;
						doc->setAttr(AOSTAG_PUB_CONTAINER, word);
					}
					else
					{
						if (remaining != "") remaining << ",";
						remaining << word;
					}
				}

				if (remaining != "")
				{
					doc->setAttr(AOSTAG_SPCONTAINER, remaining);
					sendModifyReq(siteid, (char *)doc->getData());
				}
			}
		}
	}
	*/
	OmnNotImplementedYet;
	return true;
}


bool
AosSengAdmin::cleanData(const AosXmlTagPtr &doc)
{
	// 1. Remove all the attributes "zky_imdir"
	doc->removeAttr("zky_imdir", true, false);

	// 2. Remove all the attributes 'zky_ver' that are not at the root level
	doc->removeSubAttr("zky_ver", true, false);
	return true;
}


bool
AosSengAdmin::readDocsByContainer(
		const OmnString &container,
		const u64 &maxDocid,
		const u32 siteid,
		const OmnString &idxFname, 
		const OmnString &docFname, 
		const bool readDeleted)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnString ctnr;
	AosXmlTagPtr doc;	
	for (u64 docid=1; docid<maxDocid; docid++)
	{
		// OmnScreen << "Read doc: " << docid << endl;
		if (!(doc = readDoc(docid, rdata)))
		{
			continue;
		}	

		ctnr = doc->getContainer1();

		if (container == ctnr)
		{
			OmnScreen << "Doc read: " << doc->toString() << endl;
		}
	}
	return true;
}


bool
AosSengAdmin::backup(
		const u32 &siteid,
		const AosXmlTagPtr &data)
{
	// This function copy the req of seserver and send it to backupserver.
	aos_assert_r(data, false);
	aos_assert_r(siteid > 0, false);
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", data->toString(), resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(xmlroot, false);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosSengAdmin::retrieveObj(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &username, 
		const OmnString &passwd,
		const OmnString &objid, 
		AosXmlTagPtr &xmlroot)
{
	// This function retrieves the object '[siteid, objid]'. If found, 
	// the object is return through 'xmlroot'. Otherwise, false is returned.
	// The requester should have logged in and a session ID is passed 
	// on in case the access requires security check. 
	//
	// 	<request>
	// 		<item name='operation'><![CDATA[retrieve]]></item>
	// 		<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	// 		<item name='objid'><![CDATA[xxx]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='trans_id'><![CDATA[xxx]]></item>
	// 		<item name='zky_uname'><![CDATA[xxx]]></item>
	// 		<item name='zky_passwd'><![CDATA[xxx]]></item>
	// 	</request>
	//
	// IMPORTANT: This is a blocking call. It will not return until
	// it receives response. 
	//
	// Future Enhancements: need to improve so that we can set a timer.

	OmnString req = "<request>";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"objid\">" << objid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	//	<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"" << AOSTAG_USERNAME << "\">" << username << "</item>"
		<< "<item name=\"" << AOSTAG_PASSWD << "\">" << passwd << "</item>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosSengAdmin::retrieveObjs(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &username,
		const OmnString &passwd,
		const int start_idx,
		const bool reverse,
		const int pagesize,
		const OmnString &fnames,
		const OmnString &order_fname,
		const OmnString &query, 
		const bool getTotal,
		AosXmlTagPtr &xmlroot)
{
	// This function retrieves a list of objects. The result is in the form:
	// 	<Contents>
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 	</Contents>
	//
	// It is passed back through 'xmlroot'.
	//
	// Returns:
	// Upon success, it returns true and the results are returned through 'xmlroot'. 
	// Otherwise, it returns false.
	//
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverCmd]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_uname'><![CDATA[xxx]]></item>
	// 		<item name='zky_passwd'><![CDATA[xxx]]></item>
	//	    <command>
	//	    	<cmd start_idx="xxx" 
	//	             reverse="true|false" 
	//	             psize="xxx" 
	//	             fnames="xxx" 
	//	             order="xxx" 			// Order field name
	//	             query="xxx" 
	//	             get_total="true|false" 
	//	             opr="retlist"/>
	//	    </command>
	//	</request>
	

	OmnString reverseStr = (reverse)?"true":"false";
	OmnString totalStr = (getTotal)?"true":"false";
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	//	<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"" << AOSTAG_USERNAME << "\">" << username << "</item>"
		<< "<item name=\"" << AOSTAG_PASSWD << "\">" << passwd << "</item>"
		<< "<command>"
		<< 	   "<cmd start_idx=\"" << start_idx << "\""
		<<			"reverse=\"" << reverseStr << "\""
		<<			"psize=\"" << pagesize << "\""
		<<			"fnames=\"" << fnames << "\""
		<<			"order=\"" << order_fname << "\""
		<<			"get_total=\"" << totalStr << "\""
		<<			"opr=\"retlist\"/>"
		<< "</command>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosSengAdmin::retrieveObjs(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &username,
		const OmnString &passwd,
		const OmnString &command,
		AosXmlTagPtr &xmlroot)
{
	// This function retrieves a list of objects. The result is in the form:
	// 	<Contents>
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 	</Contents>
	//
	// It is passed back through 'xmlroot'.
	//
	// Returns:
	// Upon success, it returns true and the results are returned through 'xmlroot'. 
	// Otherwise, it returns false.
	//
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverCmd]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_uname'><![CDATA[xxx]]></item>
	// 		<item name='zky_passwd'><![CDATA[xxx]]></item>
	//	    <command>
	//	    	<cmd start_idx="xxx" 
	//	             reverse="true|false" 
	//	             psize="xxx" 
	//	             fnames="xxx" 
	//	             order="xxx" 			// Order field name
	//	             query="xxx" 
	//	             get_total="true|false" 
	//	             opr="retlist"/>
	//	    </command>
	//	</request>
	

	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	//	<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"" << AOSTAG_USERNAME << "\">" << username << "</item>"
		<< "<item name=\"" << AOSTAG_PASSWD << "\">" << passwd << "</item>"
		<< command 
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(xmlroot, false);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	/*
if (child->xpathQuery("status/code", exist, "") != "200")
	OmnMark;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	*/
	return true;
}

bool
AosSengAdmin::resolveUrl1(
		const u32 siteid, 
		const OmnString &url,
		const OmnString &full_url,
		const OmnString &query_str,
		AosXmlTagPtr &root,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString &ssid,
		u64 &urldocid,
		const AosXmlTagPtr &cookies, 
		const bool isTablet)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverCmd]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_uname'><![CDATA[xxx]]></item>
	// 		<item name='zky_passwd'><![CDATA[xxx]]></item>
	//	    <command>
	//	    	<cmd start_idx="xxx" 
	//	             reverse="true|false" 
	//	             psize="xxx" 
	//	             fnames="xxx" 
	//	             order="xxx" 			// Order field name
	//	             query="xxx" 
	//	             get_total="true|false" 
	//	             opr="retlist"/>
	//	    </command>
	//	</request>
	
	vpd = 0;
	obj = 0;

	aos_assert_r(url != "", false);
	OmnString tablet = isTablet ? "true" : "false";

	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">resolve_url</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"tablet\">" << tablet << "</item>"
		<< "<item name=\"args\">" << url << "</item>"
		<< "<item name=\"" << AOSTAG_FULL_URL << "\">" << full_url << "</item>"
		<< "<item name=\"" << AOSTAG_URL_QUERY_STR << "\">" << query_str << "</item>";
	//--Ketty
	if(cookies)
	{
	    req << cookies->toString();
	}
	req << "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg),false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	// The response should be in the following format:
	// 	<Contents>
	// 		<vpd ...>
	// 		...
	// 		</vpd>
	//		...
	//		</obj>
	//	</Contents>
	child = root->getFirstChild("Contents");
	aos_assert_r(child, false);

	// Chen Ding, 12/28/2011
	urldocid = child->getAttrU64(AOSTAG_URLDOC_DOCID,0);

	vpd = child->getFirstChild();
	obj = child->getNextChild();
	return true;
}


bool
AosSengAdmin::resolveUrl1(
		const u32 siteid, 
		const OmnString &url,
		const OmnString &full_url,
		const OmnString &query_str,
		AosXmlTagPtr &root,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString &ssid,
		u64 &urldocid,
		const AosXmlTagPtr &cookies,
		AosXmlTagPtr &loginobj, 
		const bool isTablet)
{
	vpd = 0;
	obj = 0;

	aos_assert_r(url != "", false);

	OmnString tablet = isTablet ? "true" : "false";

	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">resolve_url</item>"
		<< "<item name=\"loginobj\">true</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << sessionId << "</item>"
	req	<< "<item name=\"tablet\">" << tablet << "</item>"
		<< "<item name=\"args\">" << url << "</item>"
		<< "<item name=\"" << AOSTAG_FULL_URL << "\">" << full_url << "</item>"
		<< "<item name=\"" << AOSTAG_URL_QUERY_STR << "\">" << query_str << "</item>";
	//--Ketty
	if(cookies)
	{
	    req << cookies->toString();
	}
	req << "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	AosXmlParser parser;
	root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	bool loginFlag;
	if (child->xpathQuery("status/login", loginFlag, "") == "true")
	{
		AosXmlTagPtr status = root->getFirstChild("status");
		loginobj = status->getFirstChild("zky_lgnobj");
	}
	
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	// The response should be in the following format:
	// 	<Contents>
	// 		<vpd ...>
	// 		...
	// 		</vpd>
	//		<obj>
	//		...
	//		</obj>
	//	</Contents>
	child = root->getFirstChild("Contents");
	aos_assert_r(child, false);

	// Chen Ding, 12/28/2011
	urldocid = child->getAttrU64(AOSTAG_URLDOC_DOCID,0);

	vpd = child->getFirstChild();
	obj = child->getNextChild();
	return true;
}


// Do not use this function anymore. 
// Chen Ding, 02/10/2012
/*
bool
AosSengAdmin::requestNewId(
		const u32 siteid, 
		AosXmlTagPtr &idInfo)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">get_newid</item>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	idInfo = child;
	return true;

}
*/


bool
AosSengAdmin::rebuildLog(const u32 seqno, OmnFile &file)
{
	// Log files contain sequence of 
	// records. Each record is in the format:
	// 	Byte 0-3:	Doc size
	// 	Byte 4-: 	The doc itself
	// The first four bytes are encoded directly as binary.
	u32 offset = AosDocFileMgrObj::getReservedSize();
	AosXmlParser parser;
	while(1)
	{
		int docsize = file.readBinaryInt(offset, -1);
		if (docsize < 0 || docsize > eMaxDocSize)
		{
			if (docsize == -1 ) return true;
			OmnAlarm << "Invalid docsize:" << docsize << enderr;
			return false;
		}
		char *buff = OmnNew char[docsize+10];
		int bytesread = file.readToBuff(offset + 4, docsize, buff);
		aos_assert_r(bytesread == docsize, false);
		buff[bytesread] = 0;
		AosXmlTagPtr logdoc = parser.parse(buff, "" AosMemoryCheckerArgs);
		if (!logdoc)
		{
			OmnAlarm << "Failed to parse the doc: " << buff << enderr;
			OmnDelete buff;
			// continue;
		}
		else
		{
			OmnString logid = logdoc->getAttrStr(AOSTAG_LOGID);
			if (logid != "")
			{
				OmnString args;
				//args << seqno << ":" << offset;
				bool rslt = sendServerReq(mSiteid, "rebuildlog", args, logdoc);
				OmnDelete [] buff;
				aos_assert_r(rslt, false);
				OmnScreen << "log: " << logid << endl;
			}
			else
			{
				OmnDelete [] buff;
				OmnScreen << "version empty: " << logid << endl;
			}
		}

		offset += 4 + docsize;
	}

	return true;
}


bool
AosSengAdmin::rebuildLogs(const int log_start, const int log_end)
{
	if (log_start < 0) return true;

	// In the current implementations, all logs are in the files:
	// 		zkyielog_xxx
	// where 'xxx' is a sequence number. 

	// Ketty 2011/11/28
	/*
	OmnString fnamebase = "zykielog_";

	for (int i=log_start; i<=log_end; i++)
	{
		OmnString fname = mDirname;
		fname << "/" << fnamebase << i;
		if (OmnFile::fileExist(fname))
		{
			OmnFile file(fname, OmnFile::eReadOnly);
			if (!file.isGood())
			{
				OmnAlarm << "File not good: " << fname << enderr;
				continue;
			}
			if (!rebuildLog(i, file)) return false;
		}
	}
	*/
	return true;
}


AosXmlTagPtr
AosSengAdmin::queryDocsByAttrs(
		const u32 siteid, 
		const OmnString &attr1, 
		const AosOpr opr1,
		const OmnString &value1, 
		const OmnString &attr2, 
		const AosOpr opr2,
		const OmnString &value2, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<cond type=\"";
	query << AosOpr_toStr(opr1) << "\">"
		<< "<lhs><![CDATA[" << attr1 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value1 <<"]]></rhs>"
		<<"</cond>"
		<<"<cond type=\""
		<< AosOpr_toStr(opr2) << "\">"
		<< "<lhs><![CDATA[" << attr2 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value2 <<"]]></rhs>"
		<<"</cond>";

	const int psize = 20;
	const OmnString order = "false";
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query,queryid);
}


AosXmlTagPtr
AosSengAdmin::queryDocsByAttrs(
		const u32 siteid, 
		const OmnString &attr, 
		const AosOpr opr,
		const OmnString &value, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<cond type=\"";
	query << AosOpr_toStr(opr) << "\">"
		<< "<lhs><![CDATA[" << attr <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value <<"]]></rhs>"
		<<"</cond>";

	const int psize = 20;
	const OmnString order = "";
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query,queryid);
}

AosXmlTagPtr
AosSengAdmin::queryDocsByAttrs(
		const u32 siteid, 
		const OmnString &attr1, 
		const AosOpr opr1,
		const OmnString &value1, 
		const OmnString &attr2, 
		const AosOpr opr2,
		const OmnString &value2, 
		const OmnString &attr3, 
		const AosOpr opr3,
		const OmnString &value3, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<cond type=\"";
	query << AosOpr_toStr(opr1) << "\">"
		<< "<lhs><![CDATA[" << attr1 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value1 <<"]]></rhs>"
		<<"</cond>"
		<<"<cond type=\""
		<< AosOpr_toStr(opr2) << "\">"
		<< "<lhs><![CDATA[" << attr2 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value2 <<"]]></rhs>"
		<<"</cond>"
		<<"<cond type=\""
		<< AosOpr_toStr(opr3) << "\">"
		<< "<lhs><![CDATA[" << attr3 <<"]]></lhs>"
		<< "<rhs><![CDATA[" << value3 <<"]]></rhs>"
		<<"</cond>";

	const int psize = 20;
	const OmnString order = "false";
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query, queryid);
}

AosXmlTagPtr
AosSengAdmin::queryDocsByContainer(
		const u32 siteid, 
		const OmnString &container, 
		const int startidx,
		const int queryid)
{
	const int psize = 20;
	const OmnString order = "";
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, container, order, fnames, "",queryid);
}


AosXmlTagPtr
AosSengAdmin::queryDocByObjid(
		const u32 siteid, 
		const OmnString &objid,
		const int startidx,
		const int queryid)
{
	OmnString query = "<cond type=\"eq\">";
	query<<"<lhs>zky_objid</lhs>"
		<<"<rhs><![CDATA["<< objid <<"]]></rhs>"
		<<"</cond>";

	const int psize = 20;
	const OmnString order = AOSTAG_OBJID;
	OmnString fnames; 
	fnames<<AOSTAG_OBJID << "|$$|" << AOSTAG_DOCID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query,queryid);
}


AosXmlTagPtr
AosSengAdmin::queryDocByDocid(
		const u32 siteid, 
		const OmnString &docid,
		const int startidx,
		const int queryid)
{

	OmnString query = "<cond type=\"eq\">";
	query<< "<lhs>zky_docid</lhs>"
		<< "<rhs><![CDATA[" << docid << "]]></rhs>"
		<< "</cond>";

	const int psize = 5;
	const OmnString order = AOSTAG_OBJID;
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize,"", order, fnames, query,queryid);
}


AosXmlTagPtr
AosSengAdmin::queryDocsByWords(
		const u32 siteid, 
		const OmnString &words, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<words type=\"wd\">";
	query << words << "</words>";

	const int psize = 20;
	const OmnString order = AOSTAG_OBJID;
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	AosXmlTagPtr xml = queryDocs(siteid,  startidx, psize, "", order, fnames, query,queryid);
	return xml;
}


AosXmlTagPtr
AosSengAdmin::queryDocsByTags(
		const u32 siteid, 
		const OmnString &tags, 
		const int startidx,
		const int queryid)
{
	OmnString query = "<tags type=\"tg\">";
	query << tags << "</tags>";

	const int psize = 20;
	const OmnString order = AOSTAG_OBJID;
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query,queryid);
}


AosXmlTagPtr
AosSengAdmin::queryDocs(
		const u32 siteid, 
		const int startidx,
		const int psize,
		const OmnString &query,
		const OmnString &fields,
		const int queryid)
{
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name=\"" << AOSTAG_SITEID << "\">" 
		<< "<![CDATA["<< siteid <<"]]>"<< "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<command>"
		<< "<cmd start_idx=\"" << startidx << "\""
		<< " psize=\"" << psize << "\"";

	if(queryid >= 0)
	{
		req << " queryid=\"" << queryid<< "\"";
	}
	req << " opr=\"" << "retlist" << "\">"
		<< "<conds>"
		<< 	  "<cond type=\"AND\">"
		<< query
		<<    "</cond>"
		<< "</conds>"
		<< "<fnames>"
		<< fields
		<< "</fnames>"
		<< "</cmd></command>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	// The response should be in the form:
	// 	<response>
	// 		<status .../>
	// 		<transid .../>
	// 		<Contents total="xxx"
	// 			queryid="xxx"
	// 			num="xxx">
	// 			<record .../>
	// 			<record .../>
	// 			...
	// 		</Contents>
	// 	</response>
	//
	AosXmlParser parser;
	AosXmlTagPtr xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	return xmlroot;
}

AosXmlTagPtr
AosSengAdmin::queryDocs(
		const u32 siteid, 
		const int startidx,
		const int psize,
		const OmnString &ctnrs,
		const OmnString &order,
		const OmnString &fields,
		const OmnString &query,
		const int queryid)
{
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name=\"" << AOSTAG_SITEID << "\">" 
		<< "<![CDATA["<< siteid <<"]]>"<< "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<command>"
		<< "<cmd start_idx=\"" << startidx << "\"";
	if(order != "")
	{
		req << " order=" << "\"" << order << "\"";
	}

	req << " psize=\"" << psize << "\"";

	if(queryid >= 0)
	{
		req << " queryid=\"" << queryid<< "\"";
	}
	req	<< " ctnrs=\"" << ctnrs << "\""
		<< " fnames=\"" << fields << "\""
		<< " opr=\"" << "retlist" << "\">"
		<< "<conds>"
		<< 	  "<cond type=\"AND\">"
		<< query
		<<    "</cond>"
		<< "</conds>"
		<< "</cmd></command>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	// The response should be in the form:
	// 	<response>
	// 		<status .../>
	// 		<transid .../>
	// 		<Contents total="xxx"
	// 			queryid="xxx"
	// 			num="xxx">
	// 			<record .../>
	// 			<record .../>
	// 			...
	// 		</Contents>
	// 	</response>
	//
	AosXmlParser parser;
	AosXmlTagPtr xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	return xmlroot;
}


AosXmlTagPtr
AosSengAdmin::queryDocByWords(
		const u32 siteid, 
		const OmnString &words, 
		const u64 &docid)
{
//cout << "docid is :" << docid << endl;
	int queryid = -1;
	int startidx = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByWords(siteid, words, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr<<docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		int num = contents->getAttrInt("num", -1);
		queryid = contents->getAttrInt("queryid",-1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;

}


AosXmlTagPtr
AosSengAdmin::queryDocByContainer(
		const u32 siteid, 
		const OmnString &container, 
		const u64 &docid)
{
	int startidx = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	int queryid = -1;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByContainer(siteid, container, startidx,queryid);
		if (!rslts) return 0;

		// The results are in the form:
		// 	<response>
		// 		<status .../>
		// 		<transid .../>
		// 		<Contents total="xxx"
		// 			queryid="xxx"
		// 			num="xxx">
		// 			<record .../>
		// 			<record .../>
		// 			...
		// 		</Contents>
		// 	
		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
//cout << child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) << endl;
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr<<docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;
}


AosXmlTagPtr
AosSengAdmin::queryDocByTags(
		const u32 siteid, 
		const OmnString &tags, 
		const u64 &docid)
{
	int startidx = -1;
	int guard = 0;
	int queryid = -1;
	u64 curdocid = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByTags(siteid, tags, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			curdocid = child->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
//cout << "docid: " << curdocid << endl;
			if (curdocid == docid)
			{
				docidstr<<docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1,queryid);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;

}


AosXmlTagPtr
AosSengAdmin::queryDocByAttrs(
		const u32 siteid, 
		const OmnString &attr,
		const AosOpr opr,
		const OmnString &value, 
		const u64 &docid)
{
	int startidx = -1;
	int queryid = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByAttrs(siteid, attr, opr, value, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr << docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;
}

AosXmlTagPtr
AosSengAdmin::queryDocByAttrs(
		const u32 siteid, 
		const OmnString &attr1,
		const AosOpr opr1,
		const OmnString &value1, 
		const OmnString &attr2,
		const AosOpr opr2,
		const OmnString &value2, 
		const u64 &docid)
{
	int startidx = -1;
	int queryid = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByAttrs(siteid, attr1, opr1, value1, attr2, opr2, value2, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr << docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	OmnShouldNeverComeHere;
	return 0;
}

AosXmlTagPtr
AosSengAdmin::queryDocByAttrs(
		const u32 siteid, 
		const OmnString &attr1,
		const AosOpr opr1,
		const OmnString &value1, 
		const OmnString &attr2,
		const AosOpr opr2,
		const OmnString &value2, 
		const OmnString &attr3,
		const AosOpr opr3,
		const OmnString &value3, 
		const u64 &docid)
{
	int startidx = -1;
	int queryid = -1;
	int guard = 0;
	const int maxTries = 10000;
	OmnString docidstr;
	while (guard++ < maxTries)
	{
		AosXmlTagPtr rslts = queryDocsByAttrs(siteid, attr1, opr1, value1, 
					attr2, opr2, value2, attr3, opr3, value3, startidx,queryid);
		if (!rslts) return 0;

		AosXmlTagPtr contents = rslts->getFirstChild("Contents");
		aos_assert_r(contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		while (child)
		{
			if (child->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == docid)
			{
				docidstr << docid;
				AosXmlTagPtr doc = queryDocByDocid(siteid, docidstr, -1);
				return doc;
			}

			child = contents->getNextChild();
		}

		queryid = contents->getAttrInt("queryid", -1);
		int num = contents->getAttrInt("num", -1);
		aos_assert_r(num >= 0, 0);
		if (num == 0) return 0;

		if (startidx == -1) startidx = 0;
		startidx += num;
	}
	return 0;
}


bool
AosSengAdmin::sendServerReq(
		const u32 siteid, 
		const OmnString &reqid,
		const OmnString &args,
		const AosXmlTagPtr &obj)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">" << reqid << "</item>"
		<< "<item name=\"args\">" << args << "</item>";
	if (obj)
		req << "<objdef>" << obj->toString() << "</objdef>";
	req	<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);

	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	return true;
}


AosXmlTagPtr
AosSengAdmin::createDoc(
		const AosXmlTagPtr &doc,
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &owner_cid,
		const OmnString &resolve_objid, 
		const OmnString &saveas,
		const AosRundataPtr &rdata)
{
	return createDoc(doc,
		rdata->getSiteid(),
		ssid, 
		urldocid, 
		owner_cid,
		resolve_objid, 
		saveas,
		rdata);
}

AosXmlTagPtr
AosSengAdmin::createDoc(
		const AosXmlTagPtr &doc,
		const u32 siteid,
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &owner_cid,
		const OmnString &resolve_objid, 
		const OmnString &saveas,
		const AosRundataPtr &rdata)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << 	"<objdef>" <<doc->toString() <<"</objdef>"
		<< 	"<command>"
		<< 		"<cmd "
		<<			"opr=\"createcobj\" "
		<<			"res_objid=\"" << resolve_objid << "\" "
		<<			AOSTAG_SAVEAS << "=\"" << saveas << "\" "
		<<		"/>"
		<<	"</command>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"" << AOSTAG_OWNER_CID << "\">" << owner_cid << "</item>";

	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
			<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
			<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
			<< "</request>";
	}

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(rdata->getSiteid(), "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;

	if (rdata->getArg1(AOSARG_TRACK) == "true")
	{
		rdata->setArg1(AOSARG_REQUEST_STR, req);
		rdata->setArg1(AOSARG_RESPONSE_STR, resp);
	}

	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = resproot->getFirstChild("Contents");
	aos_assert_r(con, 0);
	OmnString data = con->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}


AosXmlTagPtr 
AosSengAdmin::retrieveDocByObjid(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &objid,
		const bool needLock)
{
	OmnString resp;
	return retrieveDocByObjid(siteid,ssid, urldocid,objid,resp, needLock);
}
	
AosXmlTagPtr 
AosSengAdmin::retrieveDocByObjid(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &objid,
		OmnString &resp,
		const bool needLock)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString req = "<request >";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>"
		<< "<item name=\"objid\">" << objid << "</item>"
		<< "<item name=\"zky_editor\">true</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
				<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
				<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
				<< "</request>";
	}

	OmnString errmsg;
	if (!mConn)
	{
		OmnAlarm << "Failed conn!" <<enderr;
		return 0;
	}
	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		OmnAlarm << "Failed conn!" <<enderr;
		return 0;
	}

	if(resp == "")
	{
		OmnAlarm << "Missing Resp!" <<enderr;
		return 0;
	}

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	if(!child)
	{
		OmnAlarm << "Missing Child Tag!" <<enderr;
		return 0;
	}
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") 
	{
		return 0;
	}

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	if(!child1)
	{
		// Ketty temp.
		//OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}
	AosXmlTagPtr child2 = child1->getFirstChild();
	if(!child2)
	{
		OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}
bool
AosSengAdmin::retrieveDocByDocid(
			const u32 siteid,
			const u64 &docid)
{
	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	u64 urldocid;
	aos_assert_r(login("yuhui", "111111", "yunyuyan_account", 
				ssid, urldocid, userid, userdoc, siteid), false);
	AosXmlTagPtr doc = retrieveDocByDocid(siteid, ssid, docid, true);
	aos_assert_r(doc, false);

OmnScreen << "Retrieve doc: " << doc->toString() << endl;
	return true;
}


AosXmlTagPtr 
AosSengAdmin::retrieveDocByDocid(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid,
		const u64 &docid,
		const bool if_error)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString req = "<request >";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>"
		<< "<item name=\"zky_docid\">" << docid << "</item>"
		<< "<item name=\"zky_editor\">true</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
				<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
				<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
				<< "</request>";
	}

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (if_error)
	{
		if(child->xpathQuery("status/code", exist, "") != "200")
		{
			return 0;	
		}
	}
	else
	{
		aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	}

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	if (!child1) 
	{
		return 0;
	}

	AosXmlTagPtr child2 = child1->getFirstChild();
	if (!child2) 
	{
		return 0;
	}

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


AosXmlTagPtr 
AosSengAdmin::retrieveDocByDocid(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid, 
		const u64 &docid,
		OmnString &resp1)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString req = "<request >";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"zky_docid\">" << docid << "</item>"
		<< "<item name=\"zky_editor\">true</item>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);
	resp1 = resp;

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	if (!child1) 
	{
		return 0;
	}

	AosXmlTagPtr child2 = child1->getFirstChild();
	if (!child2) 
	{
		return 0;
	}

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


// This function used to be:
// AosSengAdmin::readDocByDocid(const u32 siteid, const u64 &docid)
// Its name is changed.
AosXmlTagPtr 
AosSengAdmin::queryAndRetrieveByDocid(const u32 siteid, const u64 &docid)
{
	// This function does the following:
	// 1. Using query-by-docid to retrieve the doc from the server.
	// 2. After retrieved the doc, get the objid. 
	// 3. Use the objid to retrieve the doc from the server again.
	const int startidx = -1;
	OmnString doci;
	doci << docid;
	AosXmlTagPtr resproot = queryDocByDocid(siteid, doci, startidx);
	AosXmlTagPtr child = resproot->getFirstChild("Contents");
	AosXmlTagPtr record = child->getFirstChild("record");

	if (!record) return 0;

	OmnString objid = record->getAttrStr(AOSTAG_OBJID);
	AosXmlTagPtr doc = retrieveDoc(siteid, objid, "", 0, "", 0);
	aos_assert_r(doc, 0);
	AosXmlTagPtr child1 = doc ->getFirstChild("Contents");
	AosXmlTagPtr child2 = child1 ->getFirstChild();

	OmnString data = child2->toString();
	AosXmlParser parser;
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


bool 
AosSengAdmin::vote(
		const int votetype,
		const OmnString &objid,
		const OmnString &mode,
		const OmnString &num,
		const OmnString &hour,
		const u32 siteid) 
{
	// This function sends a request to vote the object 'objid'.
	// If 'votetype' == 1, it is voting up; 'votetype' == 2, 
	// it is voding down.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	OmnString args = "type=";
	if (votetype == 1) args << AOSTAG_VOTEUP;
	else if (votetype == 2) args << AOSTAG_VOTEDOWN;
	else
	{
		OmnAlarm << "Unrecognized vote type: " << votetype << enderr;
		return 0;
	}
	args << ",objid="<< objid <<",mode=" <<mode << ",num=" <<num << ",hour=" <<hour;

	req << "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverreq</item>"
		<< "<item name =\"args\">"<< args<< "</item>"
		<< "<item name=\"reqid\">vote</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "</request>";

	//<request>
	//	<item name="zky_siteid"><![CDATA[mSiteid]]></item>
	//	<item name="operation"><![CDATA[serverreq]]></item>
	//	<item name="args"><![CDATA[type=zky_voteup,objid=vpd_vote.100013,
	//	mode =siggle,num=1,hour=0]]></item>
	//	<item name="reqid"><![CDATA[vote]]></item>
	//	<item name="trans_id"><![CDATA[21]]></item>
	//	</request>

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}

AosXmlTagPtr
AosSengAdmin::moveMsg(
		const u32 siteid, 
		const OmnString &objid,
		const OmnString &containerid,
		const OmnString &ssid,
		const u64 &urldocid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverreq]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='reqid'><![CDATA[move_msg]]></item>
	// 		<item name='args'><![CDATA[objid="objid",movetocontainer ="containerid"]]></item>
	//	</request>
	
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"reqid\">" << "move_msg" << "</item>"
		<< "<item name=\"args\">" << "objid=" << objid << ",containerid=" << containerid << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		return 0;
	}
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = child->getFirstChild("Contents");
	aos_assert_r(con, 0);
	AosXmlTagPtr child2 = con->getFirstChild();
	if(!child2)
	{
	    OmnAlarm << "Missing Xml!" <<enderr;
	    return 0;
	}

	OmnString data = child2->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}

AosXmlTagPtr
AosSengAdmin::createInbox(
		const u32 siteid, 
		const AosXmlTagPtr &doc,
		const OmnString &cloudid,
		const OmnString &ssid,
		const u64 &urldocid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverreq]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='reqid'><![CDATA[create_inbox]]></item>
	// 		<item name='args'><![CDATA[cloudid="cloudid"]]></item>
	//	    <objdef>
	//	    	<inbox zky_objid="inbox" AOSTAG_MSGCONTAINER_OBJID="container"/>
	//	    </objdef>
	//	</request>
	
	u32 trans_id = mTransId++;
	OmnString docstr = doc->toString();
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"reqid\">" << "create_inbox" << "</item>"
		<< "<item name=\"args\">" << "cloudid=" << cloudid << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<objdef>" << docstr << "</objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		return 0;
	}
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = child->getFirstChild("Contents");
	aos_assert_r(con, 0);
	AosXmlTagPtr child2 = con->getFirstChild();
	if(!child2)
	{
	    OmnAlarm << "Missing Xml!" <<enderr;
	    return 0;
	}

	OmnString data = child2->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}

AosXmlTagPtr
AosSengAdmin::createMsgCtnr(
		const u32 siteid, 
		const AosXmlTagPtr &doc,
		const OmnString &ssid,
		const u64 &urldocid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverreq]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='reqid'><![CDATA[create_msgcontainer]]></item>
	//	    <objdef>
	//	    	...
	//	    </objdef>
	//	</request>
	
	u32 trans_id = mTransId++;
	OmnString docstr = doc->toString();
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"reqid\">" << "create_msgcontainer" << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<objdef>" << docstr << "</objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		return 0;
	}
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = child->getFirstChild("Contents");
	aos_assert_r(con, 0);
	AosXmlTagPtr child2 = con->getFirstChild();
	if(!child2)
	{
	    OmnAlarm << "Missing Xml!" <<enderr;
	    return 0;
	}

	OmnString data = child2->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}



AosXmlTagPtr
AosSengAdmin::createUserCtnr(
		const u32 siteid, 
		const AosXmlTagPtr &doc,
		const OmnString &ssid,
		const u64 &urldocid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverCmd]]></item>
	//	    <item name='reqid'><![CDATA[crt_usrctnr]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_userid'><![CDATA[xxx]]></item>
	// 		<item name='zky_cloudid'><![CDATA[xxx]]></item>
	//	    <objdef>
	//	    	...
	//	    </objdef>
	//	</request>
	
	u32 trans_id = mTransId++;
	OmnString docstr = doc->toString();
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << docstr << "</objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		return 0;
	}
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}
	AosXmlTagPtr con = root->getFirstChild("Contents");
	aos_assert_r(con, 0);
	OmnString data = con->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}


AosXmlTagPtr
AosSengAdmin::createUserAcct(
		const AosXmlTagPtr &doc,
		const OmnString &ssid,
		const u64 &urldocid,
		const u32 siteid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverCmd]]></item>
	//	    <item name='reqid'><![CDATA[crt_usrctnr]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_id'><![CDATA[xxx]]></item>
	// 		<item name='zky_cloudid'><![CDATA[xxx]]></item>
	//	    <objdef>
	//	    	...
	//	    </objdef>
	//	</request>
	u32 trans_id = mTransId++;
	
	OmnString objdef = doc->toString();
	/*
	OmnString objdef = "<user ";
	objdef << AOSTAG_OTYPE << "=\"" << AOSOTYPE_USERACCT << "\" "
			<< AOSTAG_OBJID << "=\"" << objid << "\" "
			<< AOSTAG_USER_CTNR << "=\"" << user_ctnr << "\" "
			<< AOSTAG_USERNAME << "=\"" << objid << "\" "
			<< AOSTAG_HPCONTAINER <<"=\"" << user_ctnr << "\" "
			<< AOSTAG_SITEID << "=\"" << siteid << "\">"
			<< "</user>";
			*/

	OmnString req = "<request>";
	req << "<objdef>" << objdef <<"</objdef>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"reqid\">create_user</item>"
		<< "<item name=\"args\">null</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
		
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
			<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
			<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
			<< "</request>";
	}

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = root->getFirstChild("Contents");
	aos_assert_r(con, 0);
	OmnString data = con->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}

bool
AosSengAdmin::createSuperUser(
			const OmnString &requester_passwd,
			AosXmlTagPtr &userdoc,
			const u32 siteid)
{
	u32 trans_id = mTransId++;
	OmnString obj;
	obj <<"<embedobj " << AOSTAG_PASSWD "=\""<<requester_passwd <<"\"/>";

	OmnString req = "<request>";
	req	<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"reqid\">create_super_user</item>"
		<< "<item name=\"args\">null</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<objdef>" << obj <<"</objdef>"
		<< "</request>";
	//<request>
	//	<objdef>
	//		<embedobj/> 
	//	</objdef>
	//	<item name="zky_siteid"><![CDATA[mSiteid]]></item>
	//	<item name="operation"><![CDATA[serverreq]]></item>
	//	<item name="reqid"><![CDATA[create_user]]></item>
	//	<item name="args"><![CDATA[null]]></item>
	//	<item name="trans_id"><![CDATA[13]]></item>
	//</request>

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);


	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	userdoc = resproot->getFirstChild("Contents");
	aos_assert_r(userdoc, false);
	userdoc = userdoc->getFirstChild();
	aos_assert_r(userdoc, false);

	OmnString docstr = userdoc->toString();
	userdoc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(userdoc, false);
	return true;
}

bool
AosSengAdmin::ModifyAccessRcd(
		const u32 siteid, 
		const AosXmlTagPtr &doc,
		const u64 &owndocid,
		const OmnString &ssid,
		const u64 &urldocid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverreq]]></item>
	//	    <item name='reqid'><![CDATA[arcd_mod]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_userid'><![CDATA[xxx]]></item>
	// 		<item name='zky_cloudid'><![CDATA[xxx]]></item>
	//	    <objdef>
	//	    	...
	//	    </objdef>
	//	</request>
	
	aos_assert_r(owndocid, false);
	u32 trans_id = mTransId++;
	
	OmnString docstr = doc->toString();

	OmnString req = "<request>";
	req << "<objdef>" << docstr <<"</objdef>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"reqid\">arcd_mod</item>"
		<< "<item name=\"args\"> owndocid="<<owndocid <<" </item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	aos_assert_r(child, false);
	return true;
}


bool
AosSengAdmin::login(
			const OmnString &username,
			const OmnString &passwd,
			const OmnString &ctnr_name,
			OmnString &ssid,
			u64 &urldocid,
			u64 &userid,
			AosXmlTagPtr &userdoc,
			const u32 siteid,
			const OmnString &cid,
			const OmnString &rootssid)	
{
	u32 trans_id = mTransId++;
	OmnString obj;
	obj << "<embedobj "
		<< "getsysurl" << "=\"" << "true"<<"\" "
		<< AOSTAG_USERNAME << "=\""<< username <<"\" "
		<< " opr=\"login\" "
		<< AOSTAG_CLOUDID << "=\"" << cid <<"\" "
		<< AOSTAG_HPVPD << "=\"yyy_room_frame\" "
		<< AOSTAG_LOGIN_VPD << "=\"yyy_login\" "
		<< AOSCONFIG_CTNR << "=\""<< ctnr_name <<"\"><" 
		<< AOSTAG_PASSWD <<">"<< passwd <<"</"<< AOSTAG_PASSWD
		<<"></embedobj>";

	OmnString req = "<request>";
	req << "<command>" << obj <<"</command>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
	 	<< "<item name='loginobj'>true</item>"
		<< "</request>";
	// <request>
	// <item name='zky_siteid'><![CDATA[mSiteid]]></item>
	// 		<item name='operation'><![CDATA[serverCmd]]></item>
	// 		<item name='loginobj'><![CDATA[true]]></item>
	// 		<item name='trans_id'><![CDATA[5]]></item>
	// 		<command>
	// 			<embedobj container="yunyuyan_account" zky_hpvpd="yyy_room_frame" 
	// 				rattrs="zky_category|sep418|zky_hpvpd|sep418|zky_objimg|sep418|zky_fans|sep418|zky_abmctnr|sep418|zky_realnm|sep418|zky_uname"
	// 				zky_lgnvpd="yyy_login" opr="login" zky_uname="yuhui"><zky_passwd>&lt;![CDATA[12345]]&gt;</zky_passwd>
	// 			</embedobj>
	// 		</command>
	// 		<zky_cookies>
	// 			<cookie zky_name="aos_userid_ck"><![CDATA[201074183101135610747320097]]></cookie>
	// 			<cookie zky_name="JSESSIONID"><![CDATA[999454DA3E45597FD08E8C9B0D6482C2]]></cookie>
	// 		</zky_cookies>
	// </request>

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	AosXmlTagPtr contents = resproot ->getFirstChild("Contents");
	AosXmlTagPtr record = contents->getFirstChild();
	ssid = record->getAttrStr(AOSTAG_SESSIONID);
	userid = record->getAttrU64(AOSTAG_DOCID, 0);

	AosXmlTagPtr doc = parser.parse(record->toString(), "" AosMemoryCheckerArgs);
	aos_assert_r(doc, false);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid!="", false);
	OmnString ss = ssid;
	if (rootssid != "") ss = rootssid;
	urldocid = doc->getAttrU64("zky_sysurldocid", 0);
	userdoc = retrieveDocByObjid(siteid, ss, urldocid, objid, false);
	aos_assert_r(userdoc, false);
	userid = userdoc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(userid, false);
	return true;
}


AosXmlTagPtr
AosSengAdmin::getAccessRcd(
		const u32 siteid, 
		const u64 &docid,
		const OmnString &ssid,
		const u64 &urldocid, 
		const bool create_flag, 
		const bool get_parent)
{
	u32 trans_id = mTransId++;

	OmnString args = "docid=";
	args << docid << ",create=";
	if (create_flag) args << "true";
	else args << "false";
	args << ",parent=";
	if (get_parent) args << "true";
	else args << "false";

	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='args'>"<< args <<"</item>"
		<< "<item name='reqid'>arcd_get</item>"
		<< "<item name='trans_id'>"<< trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
			<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
			<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
			<< "</request>";
	}

			
	OmnString errmsg;
	OmnString resp;

	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);


	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(resp, "" AosMemoryCheckerArgs);
	if (!doc) 
	{
		return 0;
	}

	AosXmlTagPtr child1 = doc ->getFirstChild("Contents");
	AosXmlTagPtr child2 = child1 ->getFirstChild();
	
	if (!child2) 
	{
		return 0;
	}
	OmnString data = child2->toString();
	AosXmlParser parser1;
	AosXmlTagPtr redoc = parser1.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


bool
AosSengAdmin::logout(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const AosRundataPtr &rdata)
{
	u32 trans_id = mTransId++;

	rdata->setOk();
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>logout</item>"
		<< "<item name='trans_id'>"<< trans_id << "</item>";

	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
			<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
			<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
			<< "</request>";
	}
			
	OmnString errmsg;
	OmnString resp;
	if (!mConn)
	{
		rdata->setError() << "Missing connection to server!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		rdata->setError() << "Failed sending request to server: " << errmsg;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (resp == "")
	{
		rdata->setError() << "Failed receiving response";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// if (rdata->withLog()) rdata->appendLog(resp);

	AosXmlParser parser;
	AosXmlTagPtr resproot =  parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Failed parsing response!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool exist;
	rslt = (child->xpathQuery("status/code", exist, "") == "200");
	if (!rslt)
	{
		rdata->setError() << "Failed logging out: " 
			<< child->xpathQuery("status/code", exist, "");
		return false;
	}
	rdata->setOk();
	return true;
}


bool
AosSengAdmin::addFollower(
		const OmnString &ssid,
		const u64 &urldocid,
		const u32 siteid,
		const OmnString &byfollower_cid)
{
	u32 trans_id = mTransId++;
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>" << siteid << "</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>addfollower</item>"
		<< "<item name='trans_id'>" << trans_id << "</item>"
		<< "<objdef>"
		<< "<Contents>"
		<< "<byfollower_cid>" << byfollower_cid << "</byfollower_cid>"
		<< "</Contents>"
		<< "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
OmnScreen << "\r\n==========================================\r\n" << endl;
OmnScreen << resp  << endl;
OmnScreen << "\r\n==========================================\r\n" << endl;
	aos_assert_r(resp != "", false);
	return true;
}

bool
AosSengAdmin::removeFollower(
		const OmnString &ssid,
		const u64 &urldocid,
		const u32 siteid,
		const OmnString &byfollower_cid)
{
	u32 trans_id = mTransId++;
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>" << siteid << "</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>removefollower</item>"
		<< "<item name='trans_id'>" << trans_id << "</item>"
		<< "<objdef>"
		<< "<Contents>"
		<< "<byfollower_cid>" << byfollower_cid << "</byfollower_cid>"
		<< "</Contents>"
		<< "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
OmnScreen << "\r\n==========================================\r\n" << endl;
OmnScreen << resp  << endl;
OmnScreen << "\r\n==========================================\r\n" << endl;
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSengAdmin::addMicroBlogMsg(
		const OmnString &ssid,
		const u64 &urldocid,
		const u32 siteid,
		const OmnString &msg)
{
	u32 trans_id = mTransId++;
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>" << siteid << "</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>addmicroblogmsg</item>"
		<< "<item name='trans_id'>" << trans_id << "</item>"
		<< "<objdef>"
		<< "<Contents>"
		<< "<msg>" << msg << "</msg>"
		<< "</Contents>"
		<< "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
OmnScreen << "\r\n==========================================\r\n" << endl;
OmnScreen << resp  << endl;
OmnScreen << "\r\n==========================================\r\n" << endl;
	aos_assert_r(resp != "", false);
	return true;
}


AosXmlTagPtr
AosSengAdmin::createLog(
		const u32 siteid,
		const OmnString &logname,
		const OmnString &ctnr_objid,
		const OmnString &contents)
{
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='logname'>" << logname << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>crtlog</item>"
		<< "<objdef>"
		<< "<doc zky_log_ctnr_objid=\"" << ctnr_objid << "\" "
		<< " zky_needresp=\"true\" >"
		<< "<contents>" << contents << "</contents>"
		<< "</doc>"
		<< "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(resproot, 0);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr respcontents = resproot->getFirstChild("Contents");
	aos_assert_r(respcontents, 0);
	return respcontents;
}


bool
AosSengAdmin::addVersion(
		const AosXmlTagPtr &doc,
		const u32 siteid)
{
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>addver</item>"
		<< "<objdef>" << doc->toString() << "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(resproot, false);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	AosXmlTagPtr respcontents = resproot->getFirstChild("Contents");
	aos_assert_r(respcontents, false);
	OmnString logid = respcontents->getNodeText();
	aos_assert_r(logid != "", false);
	return true;
}


AosXmlTagPtr
AosSengAdmin::retrieveLog(
		const u32 siteid,
		const u64 &logid)
{
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>reclog</item>"
		<< "<objdef>"
		<< "<loginfo>" << logid << "</loginfo>"
		<< "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(resproot, 0);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr respcontents = resproot->getFirstChild("Contents");
	aos_assert_r(respcontents, 0);
	AosXmlTagPtr rcvLog = respcontents->getFirstChild();
	aos_assert_r(rcvLog, 0);
	return rcvLog;
}


bool
AosSengAdmin::sendInMsg(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &friend_cid, 
			const OmnString &msg)
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>sendinmsg</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<msg>"<< msg <<"</msg>"
		<< "</Contents></objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSengAdmin::addFriend(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &ctn_name, 
			const OmnString &friend_cid) 
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>addfriend</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<ctn_name>"<< ctn_name <<"</ctn_name>"
		<< "</Contents></objdef></request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}

bool
AosSengAdmin::inviteFriend(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &ctn_name, 
			const OmnString &friend_cid) 
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>invitefriend</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<ctn_name>"<< ctn_name <<"</ctn_name>"
		<< "</Contents></objdef></request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSengAdmin::denyFriend(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &ctn_name, 
			const OmnString &friend_cid) 
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>denyfriend</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<ctn_name>"<< ctn_name <<"</ctn_name>"
		<< "</Contents></objdef></request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSengAdmin::removeFriend(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &ctn_name, 
			const OmnString &friend_cid) 
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>removefriend</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<ctn_name>"<< ctn_name <<"</ctn_name>"
		<< "</Contents></objdef></request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSengAdmin::addFriendResp(
	const u32 siteid, 
	const OmnString &requester, 
	const OmnString &friendid)
{
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>addFriendResp</item>"
		<< "<item name='requester'>"<< requester <<"</item>"
		<< "<item name='friend'>"<< friendid <<"</item>"
		<< "<item name='trans_id'>"<< trans_id << "</item></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSengAdmin::buildDocByObjid(
		const OmnString &objid,
		const u64 &maxDocid,
		const u32 siteid,
		const OmnString &idxFname, 
		const OmnString &docFname, 
		const bool readDeleted)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnConnBuffPtr docbuff;
	// Ketty 2011/11/28
	//mIdxFname = idxFname;
	//mDocFname = docFname;
	OmnString oid;
	DocStatus status;
	for (u64 docid=1; docid<maxDocid; docid++)
	{
		OmnScreen << "Read doc: " << docid << endl;
		if (!readDoc(docid, docbuff, readDeleted, status, rdata)) continue;
	
		AosXmlParser parser;
		AosXmlTagPtr doc = parser.parse(docbuff->getData(), "" AosMemoryCheckerArgs);
		if (doc)
		{
			oid = doc->getAttrStr(AOSTAG_OBJID);
			if (oid == objid)
			{
				OmnScreen << "To build doc: " << doc->getData() << endl;
				sendCreateObjReq(siteid, (char *)doc->getData());
				return true;
			}
		}
	}
	return true;
}

/*
bool
AosSengAdmin::checkRebuild(
		const u32 siteid, 
		const u64 &startdocid, 
		const u64 &enddocid)
{

	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	OmnString resp;

	u64 docid, docid1, docid2;
	OmnString objid, objid1, objid2, otype, otype1, otype2, crttime, crttime1, crttime2;
	
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	mSiteid = siteid;

	int num_errors = 0;
	login("yuhui", "12345",	"yunyuyan_account", ssid, userid, userdoc, siteid);
	
	int group = 0;
	int docids[] = {35483,35484,35501,35502,109774,109775,144850,144851,144858,144859,144897,144898};
	for (int i =0;i<12;i++)
	{
		docids[i] = 144898 ;
		if (group >= sgGroupSize)
		{
			OmnSleep(sgSleepLength);
			group = 0;
		}

		if (!readDoc(docids[i], docbuff))
		{
			cout << "missing doc" << docids[i] << endl ;
			continue;
		}
		AosXmlTagPtr local_doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
		if (!local_doc)
		{
			OmnAlarm << "Failed to parse the doc: " 
				<< docids[i] << ":" << docbuff->getData() << enderr;
			continue;
		}
		
		// normalize the objid
		local_doc->normalize();
		if ((objid = local_doc->getAttrStr(AOSTAG_OBJID)) == "")
		{
			OmnAlarm << "Doc missing objid: " << local_doc->toString() << enderr;
			continue;
		}

		if (docids[i] != local_doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID)  && docids[i] == AOS_INVDID)
		{
			OmnAlarm << "Doc missing docid: " << local_doc->toString() << enderr;
			continue;
		}
		
		if (local_doc->getAttrStr(AOSTAG_OTYPE) == "zky_ssion")
		{
			// Do not check the session obj
			continue;
		}

		otype = local_doc->getAttrStr(AOSTAG_OTYPE, "");
		crttime = local_doc->getAttrStr(AOSTAG_CTIME, "");

		//objid.normalizeWhiteSpace(true, true);
		//otype.normalizeWhiteSpace(true, true);
		//crttime.normalizeWhiteSpace(true, true);

//		if (docid == 5391 || docid == 5392 || docid == 5634) continue;
//		if (docid == 5635) continue;		// Failed retrieving: gicdesc_input
//		if (docid == 5642) continue;		// Failed retrieving: 
//		if (docid == 8341) continue;		// Objid with space 
//		if (docid == 8408) continue;		// Server Error (bug1)
//		if (docid == 34690) continue;		// Objid with Chinese (bug3)
//		if (docid == 34694) continue;		// Objid with Chinese (bug3)

		AosXmlTagPtr byObjid = retrieveDocByObjid(siteid,  ssid, objid,resp);
		if (!byObjid)
		{
			OmnAlarm << "Failed to retrieve object: " << objid << enderr;
			OmnScreen << local_doc->toString() << num_errors++ << endl;
			OmnScreen << "RESP:" << resp << endl;
			continue;
		}

		objid1 = byObjid->getAttrStr(AOSTAG_OBJID);
		if (objid != objid1)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byObjid->toString() << 
				num_errors++ << endl;
			continue;
		}

		docid1= byObjid->getAttrU64(AOSTAG_DOCID,0);
		if (docids[i] != docid1)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byObjid->toString() << num_errors++ << endl;
			continue;
		}

		otype1 = byObjid->getAttrStr(AOSTAG_OTYPE);
		if (otype != otype1)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byObjid->toString() << num_errors++ << endl;
			continue;
		}

		crttime1 = byObjid->getAttrStr(AOSTAG_CTIME);
		if (crttime!= crttime1)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byObjid->toString() << num_errors++ << endl;
			continue;
		}

		AosXmlTagPtr byDocid = retrieveDocByDocid(siteid, ssid, docids[i]);
		if (!byDocid)
		{
			OmnAlarm << "Failed to retrieve: " << docids[i] << enderr;
			OmnScreen << local_doc->toString() << num_errors++ << endl;
			OmnScreen << "resp msg:" << resp<< endl;
			continue;
		}

		objid2 = byDocid->getAttrStr(AOSTAG_OBJID);
		if (objid != objid2)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byDocid->toString() << num_errors++ << endl;
			continue;
		}
		docid2 = byDocid->getAttrU64(AOSTAG_DOCID,0);
		if ((unsigned int)docids[i] != docid2)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byDocid->toString() << num_errors++ << endl;
			continue;
		}

		otype2 = byDocid->getAttrStr(AOSTAG_OTYPE);
		if (otype != otype2)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byDocid->toString() << num_errors++ << endl;
			continue;
		}

		crttime2 = byDocid->getAttrStr(AOSTAG_CTIME);
		if (crttime!= crttime2)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byDocid->toString() << num_errors++ << endl;
			continue;
		}

		group++;
		cout << "Checked: " << docids[i] << endl;
	}

	return true;
}
*/

bool
AosSengAdmin::checkRebuild(
		const u32 siteid, 
		const u64 &startdocid, 
		const u64 &enddocid)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	OmnString resp;

	u64 docid, docid1, docid2;
	OmnString objid, objid1, objid2, otype, otype1, otype2, crttime, crttime1, crttime2, pctnr, pctnr2;
	
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	mSiteid = siteid;

	int num_errors = 0;
	u64 urldocid;
	login("yuhui", "111111", "yunyuyan_account", ssid, urldocid, userid, userdoc, siteid);
	
	DocStatus status;
	for (docid=startdocid; docid<enddocid; docid++)
	{
		// Read the doc
		if (!readDoc(docid, docbuff, false, status, rdata)) continue;
		if (!docbuff) continue;
		AosXmlTagPtr local_doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
		if (!local_doc)
		{
			OmnAlarm << "Failed to parse the doc: " 
				<< docid << ":" << docbuff->getData() << enderr;
			continue;
		}
		
		// Check objid
		local_doc->normalize();
		if ((objid = local_doc->getAttrStr(AOSTAG_OBJID)) == "")
		{
			OmnAlarm << "Doc missing objid: " << local_doc->toString() << enderr;
			continue;
		}

		// Check docid
		if (docid != local_doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID)  && docid == AOS_INVDID)
		{
			OmnAlarm << "Doc missing docid: " << local_doc->toString() << enderr;
			continue;
		}
		
		// Skip the session doc. 
		if (local_doc->getAttrStr(AOSTAG_OTYPE) == "zky_ssion")
		{
			// Do not check the session obj
			continue;
		}

		pctnr = local_doc->getAttrStr(AOSTAG_PARENTC ,"");
		otype = local_doc->getAttrStr(AOSTAG_OTYPE, "");
		crttime = local_doc->getAttrStr(AOSTAG_CTIME, "");
		if (otype == AOSOTYPE_ACCESS_RCD)
		{
			continue;
		}

		AosXmlTagPtr byDocid = retrieveDocByDocid(siteid, ssid, urldocid, docid, resp);
		if (!byDocid)
		{
			OmnAlarm << "Failed to retrieve: " << docid << enderr;
			OmnScreen << local_doc->toString() << num_errors++ << endl;
			OmnScreen << "resp msg:" << resp<< endl;
			continue;
		}

		objid2 = byDocid->getAttrStr(AOSTAG_OBJID);
		if (objid != objid2)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byDocid->toString() << num_errors++ << endl;
			continue;
		}
		docid2 = byDocid->getAttrU64(AOSTAG_DOCID,0);
		if (docid != docid2)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byDocid->toString() << num_errors++ << endl;
			continue;
		}

		otype2 = byDocid->getAttrStr(AOSTAG_OTYPE);
		if (otype2 != "zky_unknown" && otype != otype2 )
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byDocid->toString() << num_errors++ << endl;
			continue;
		}

		crttime2 = byDocid->getAttrStr(AOSTAG_CTIME);
		if (crttime!= crttime2)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byDocid->toString() << num_errors++ << endl;
			continue;
		}

		pctnr2 = byDocid->getAttrStr(AOSTAG_PARENTC, "");
		if (pctnr != "" && pctnr2 != pctnr )
		{
			if (pctnr != objid)
				OmnScreen << "Failed the check: " << local_doc->toString()
					<< "\n\n" << byDocid->toString() << num_errors++ << endl;
			continue;
		}

		AosXmlTagPtr byObjid = retrieveDocByObjid(siteid,  ssid, urldocid, objid,resp);
		if (!byObjid)
		{
			OmnAlarm << "Failed to retrieve object: " << objid << enderr;
			//OmnScreen << local_doc->toString() << num_errors++ << endl;
			OmnScreen << "RESP:" << resp << endl;
			continue;
		}

		objid1 = byObjid->getAttrStr(AOSTAG_OBJID);
		if (objid != objid1)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byObjid->toString() << 
				num_errors++ << endl;
			continue;
		}

		docid1= byObjid->getAttrU64(AOSTAG_DOCID,0);
		if (docid != docid1)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byObjid->toString() << num_errors++ << endl;
			continue;
		}

		otype1 = byObjid->getAttrStr(AOSTAG_OTYPE);
		if (otype1 != "zky_unknown" && otype != otype1 )
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byObjid->toString() << num_errors++ << endl;
			continue;
		}

		crttime1 = byObjid->getAttrStr(AOSTAG_CTIME);
		if (crttime!= crttime1)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << byObjid->toString() << num_errors++ << endl;
			continue;
		}

		cout << __FILE__ << ":" << __LINE__ << ": Checked: " << docid << endl;
	}

	return true;
}


bool
AosSengAdmin::showDocId(
		const u64 &did, 
		const AosRundataPtr &rdata)
{
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	
	DocStatus status;
	if (!readDoc(did, docbuff, false, status, rdata))
	{
		OmnScreen << "Fail to read doc by did:" << did <<endl;
	}	

	AosXmlTagPtr doc =  parser.parse(docbuff, "" AosMemoryCheckerArgs);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	OmnScreen <<  "doc id is: " << docid <<endl;
	return true;
}


bool
AosSengAdmin::tortureGetObjid(
		const u32 siteid, 
		const OmnString &tested_objid,
		const u64 &startdocid, 
		const u64 &enddocid)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	OmnString resp;

	OmnString objid, objid1, objid2, otype, otype1, otype2, crttime, crttime1, crttime2;
	
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	mSiteid = siteid;

	int num_errors = 0;
	u64 urldocid;
	login("yuhui", "12345",	"yunyuyan_account", ssid, urldocid, userid, userdoc, siteid);
	
	int group = 0;
	while (1)
	{
		if (group >= sgGroupSize)
		{
			OmnSleep(sgSleepLength);
			group = 0;
		}

		int num = (rand() % 10) + 1;
		DocStatus status;
		for (int i=0; i<num; i++)
		{
			u64 docid = (u64)rand() % enddocid + 5000;
			if (!readDoc(docid, docbuff, false, status, rdata)) continue;
			AosXmlTagPtr local_doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
			if (!local_doc)
			{
				OmnAlarm << "Failed to parse the doc: " 
					<< docid << ":" << docbuff->getData() << enderr;
				continue;
			}
		
			OmnString objid = local_doc->getAttrStr(AOSTAG_OBJID);
			AosXmlTagPtr byObjid = retrieveDocByObjid(siteid, ssid, urldocid, objid,resp);
			if (!byObjid)
			{
				OmnAlarm << "Failed to retrieve object: " << objid << enderr;
				OmnScreen << local_doc->toString() << num_errors++ << endl;
				OmnScreen << "RESP:" << resp << endl;
				continue;
			}

			if (objid == "yyy_login") continue;

			if (objid != byObjid->getAttrStr(AOSTAG_OBJID))
			{
				OmnAlarm << "Retrieved incorrect: " << objid << ":" 
					<< byObjid->getAttrStr(AOSTAG_OBJID) << enderr;
				OmnScreen << "Correct Obj: " << local_doc->toString() << endl;
				OmnScreen << "Incorrect Obj: " << byObjid->toString() << endl;
				exit(0);
			}
		}

		AosXmlTagPtr byObjid = retrieveDocByObjid(siteid, ssid, urldocid, tested_objid, resp);
		if (!byObjid)
		{
			OmnAlarm << "Failed to retrieve object: " << tested_objid << enderr;
			OmnScreen << "RESP:" << resp << endl;
			continue;
		}

		if (tested_objid != byObjid->getAttrStr(AOSTAG_OBJID))
		{
			OmnAlarm << "Retrieved incorrect: " << tested_objid << ":" 
				<< byObjid->getAttrStr(AOSTAG_OBJID) << enderr;
			OmnScreen << "Incorrect Obj: " << byObjid->toString() << endl;
			exit(0);
		}
		group ++;
	}

	return true;
}

bool
AosSengAdmin::checkQuery(
		const u32 siteid, 
		const u64 &startdocid, 
		const u64 &enddocid,
		const u64 &max_num)
{
	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	u64 urldocid;
	login("yuhui", "12345", "yunyuyan_account", ssid, urldocid, userid, userdoc,  siteid);
	u64 docid;
	int group = 0;
	srand(99999);
	for (u64 j = 0; j< max_num; j++)
	{
		docid  = (rand()%(enddocid - startdocid))+startdocid;
		OmnScreen << "-------num     j: "<< j << " docid:" << docid << endl;
        OmnScreen <<"-----------------------------------------------------------"<< endl;
		if (group >= sgGroupSize)
		{
			OmnSleep(sgSleepLength);
			group = 0;
		}

		AosXmlTagPtr doc = retrieveDocByDocid(siteid,  ssid, docid, true);
		if (!doc) continue;

		aos_assert_r(docid == doc->getAttrU64(AOSTAG_DOCID,0), false);
		bool rslt = query(doc, siteid, ssid, urldocid, docid);
		if (!rslt)
		{
			OmnAlarm << "Failed query !!!!" << enderr;
		}
	
	}
	return true;
}

bool
AosSengAdmin::query(
		const AosXmlTagPtr &doc,
		const u32 siteid,
		const OmnString &ssid,
		const u64 &urldocid,
		const u64 &docid)
{
	OmnString otype = doc->getAttrStr(AOSTAG_OTYPE);
	if (otype == "")
	{
		OmnAlarm << "Doc missing otype " << enderr;
		return false;
	}
	OmnString ctime = doc->getAttrStr(AOSTAG_CTIME);
	if (ctime == "")
	{
		OmnAlarm << "Doc missing ctime " << enderr;
		return false;
	}

    OmnString creator = doc->getAttrStr(AOSTAG_CREATOR);
	if (creator == "")
	{
		OmnAlarm << "Doc missing creator " << enderr;
		return false;
	}

	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	if (objid == "")
	{
		OmnAlarm << "Doc missing objid " << enderr;
		return false;
	}
	if (docid == 90059)
		OmnMark;
		
	AosXmlTagPtr xml;
	//int i = rand()%4;
	int i = rand()%4;
	OmnScreen << "-------        i: " << i << endl;
	if(i == 3)
	{
	   sgFlag ++ ;
	   if(sgFlag == 10)
	   {
	       xml = QueryNe(otype, ctime, docid);
		   sgFlag = 0 ;
	   }
	   else
	   {
	       return true;
	   }
	}
	switch (i)
	{
	case 0://==
			xml = QueryEq(otype, ctime, creator, docid);
			break;
	case 1: //<=
		    xml = QueryLe(otype, ctime, docid);
			break;
	case 2: //>=
			xml = QueryGe(otype, ctime, docid);
			break;
	/*
	case 3: //!=
			//xml = QueryNe(otype, ctime, docid);
			break;
			*/
	}
		
	OmnScreen <<"-------Success : " <<" docid: "<< docid <<" objid: "<< objid <<"  i:" << i << endl ;
	if (i== 3)
	{
		if (xml) 
		{
			OmnAlarm << "Doc missing docid/i " << docid << ":" << i << enderr;
			return false;
		}
		//aos_assert_r(!xml, false);
	}
	if (i != 3)
	{
		if (!xml)
		{
			OmnAlarm << "Doc missing docid/i " << docid << ":" << i << enderr;
			return false;
		}
		aos_assert_r (xml, false);
		AosXmlTagPtr contents = xml->getFirstChild("Contents");
		aos_assert_r (contents, 0);
		AosXmlTagPtr child = contents->getFirstChild();
		aos_assert_r (child,  false);
		aos_assert_r (child->getAttrStr(AOSTAG_OBJID) == objid, false);
		aos_assert_r (child->getAttrU64(AOSTAG_DOCID,0) == docid, false);
		AosXmlTagPtr xmldoc = retrieveDocByObjid(siteid, ssid, urldocid, objid);
		aos_assert_r (xmldoc->getAttrStr(AOSTAG_OTYPE) == otype, false);
		aos_assert_r (xmldoc->getAttrStr(AOSTAG_CTIME) == ctime, false);
		aos_assert_r (xmldoc->getAttrStr(AOSTAG_CREATOR) == creator, false);

	}
	return true;
}

AosXmlTagPtr
AosSengAdmin::QueryEq(
			const OmnString &otype,
			const OmnString &ctime,
			const OmnString &creator,
			const u64 &docid)
{
	//==
	u32 siteid = mSiteid;
	aos_assert_r(otype != "" && ctime != "" && creator != "", 0); 
	AosOpr opr = AosOpr_toEnum("==");
	AosXmlTagPtr xmldoc = queryDocByAttrs(siteid, AOSTAG_OTYPE, opr, otype,
			AOSTAG_CTIME, opr, ctime, AOSTAG_CREATOR, opr, creator, docid);
	return xmldoc;
}

AosXmlTagPtr
AosSengAdmin::QueryLe(
			const OmnString &otype,
			const OmnString &ctime,
			const u64 &docid)
{
	//<=
	aos_assert_r(otype != "" && ctime != "", 0); 
	u32 siteid = mSiteid;
	AosOpr opr = AosOpr_toEnum("<=");
	AosOpr opr1 = AosOpr_toEnum("==");
	AosXmlTagPtr xmldoc = queryDocByAttrs(siteid, AOSTAG_OTYPE, opr1, otype,
			AOSTAG_CTIME, opr, ctime, docid);
	return xmldoc;
}

AosXmlTagPtr
AosSengAdmin::QueryGe(
			const OmnString &otype,
			const OmnString &ctime,
			const u64 &docid)
{
	//>=
	aos_assert_r(otype != "" && ctime != "", 0); 
	u32 siteid = mSiteid;
	AosOpr opr = AosOpr_toEnum(">=");
	AosOpr opr1 = AosOpr_toEnum("==");
	AosXmlTagPtr xmldoc = queryDocByAttrs(siteid, AOSTAG_OTYPE, opr1, otype,
				AOSTAG_CTIME, opr, ctime, docid);
	return xmldoc;
}

AosXmlTagPtr
AosSengAdmin::QueryNe(
			const OmnString &otype,
			const OmnString &ctime,
			const u64 &docid)
{
	//!=
	aos_assert_r(otype != "" && ctime != "" , 0); 
	u32 siteid = mSiteid;
	AosOpr opr = AosOpr_toEnum("!=");
	AosXmlTagPtr xmldoc = queryDocByAttrs(siteid, AOSTAG_OTYPE, opr, otype,
				AOSTAG_CTIME, opr, ctime, docid);
	return xmldoc;
}

AosXmlTagPtr 
AosSengAdmin::retrieveVersionByObjid(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const OmnString &objid, 
		const OmnString &version)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="subopr">verobj</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="objid">xxx</item>
	//  </request>
	u32 trans_id = mTransId++;
	OmnString req = "<request >";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"subopr\">verobj</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"zky_objid\">" << objid <<"</item>"
		<< "</request>";
		//<< "<item name=\"zky_objid\">" << objid << ":" << version << "</item>"

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	aos_assert_r(child1, 0);
	AosXmlTagPtr child2 = child1->getFirstChild();
	aos_assert_r(child2, 0);

	OmnString data = child2->toString();
    AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
    return redoc;
}


bool
AosSengAdmin::checkVersion(
		const u32 siteid, 
		const u64 &startdocid,
		const u64 &enddocid, 
		const u32 num_tries)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnScreen <<"~~~~~~~~~~~~~~~~TTTT~~~~~~~~~~~~~" << endl;
	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;

	OmnConnBuffPtr docbuff;
	AosXmlParser parser;

	int num_errors = 0;
	u64 urldocid;
	login("yuhui", "12345",	"yunyuyan_account", ssid, urldocid, userid, userdoc, siteid);
	
	int group = 0;
	for (u32 i=0; i<num_tries; i++)
	{
		if (group >= sgGroupSize)
		{
			OmnSleep(sgSleepLength);
			group = 0;
		}

		DocStatus status;
		u64 docid = startdocid + rand() % (enddocid - startdocid);
		if (!readDoc(docid, docbuff, false, status, rdata)) continue;
		AosXmlTagPtr local_doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);


		if (!local_doc)
		{
			OmnAlarm << "Failed to parse the doc: " 
				<< docid << ":" << docbuff->getData() << enderr;
			continue;
		}
		
		OmnString version = local_doc->getAttrStr(AOSTAG_VERSION);
		if (version == "") continue;

		OmnString objid = local_doc->getAttrStr(AOSTAG_OBJID);
		AosXmlTagPtr server_doc = retrieveVersionByObjid(siteid, ssid, urldocid, objid, version);
		
		
		
		if (!server_doc)
		{
			OmnAlarm << "Failed to retrieve object: " << objid << enderr;
			OmnScreen << local_doc->toString() << num_errors++ << endl;
			continue;
		}

		OmnString server_version = server_doc->getAttrStr(AOSTAG_VERSION);
		OmnString server_objid = server_doc->getAttrStr(AOSTAG_OBJID);
		u64 server_docid = server_doc->getAttrU32(AOSTAG_DOCID, 0);
		objid = local_doc->getAttrStr(AOSTAG_OBJID);
		docid = local_doc->getAttrU32(AOSTAG_DOCID, 0);

		if (version != server_version)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << server_doc->toString() << 
				num_errors++ << endl;
			continue;
		}
		if (docid != server_docid)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << server_doc->toString() << endl;
			continue;
		}
		if (objid != server_objid)
		{
			OmnScreen << "Failed the check: " << local_doc->toString()
				<< "\n\n" << server_doc->toString() << endl;
			continue;
		}
	}
	return true;
}


bool
AosSengAdmin::readTargetDoc(
		const u64 &docid,
		OmnConnBuffPtr &docbuff, 
		const bool readDeleted)
{
	// int maxHeaderPerFile = AosXmlDoc::getMaxHeaderPerFile();
	int maxHeaderPerFile = eAosMaxFileSize/AosXmlDoc::eDocHeaderSize;

	// Create the buff for header
	char bb[AosXmlDoc::eDocHeaderSize];

	// OmnScreen << "Read doc: " << docid << endl;
	u32 seqno = docid / maxHeaderPerFile;
	u32 offset = (docid % maxHeaderPerFile) * AosXmlDoc::eDocHeaderSize;

	OmnFilePtr idxfile = mTargetIdxFiles[seqno];
	if (!idxfile) idxfile = openTargetIdxFile(seqno);
	aos_assert_r(idxfile, false);

	int len = idxfile->readBinaryInt(offset, -1);
	if (len <= 0 || (u32)len > AosXmlDoc::eDocHeaderSize)
	{
		return false;
	}

	len = idxfile->readToBuff(offset+4, len, bb);
	aos_assert_r(len >= 0, false);
	AosBuff headerBuff(bb, len, len AosMemoryCheckerArgs);
		
	headerBuff.getU32(AosXmlDoc::eInvVersion);			// version
	headerBuff.getU32(AOS_INVSID);						// siteid
	headerBuff.getU64(AOS_INVDID);						// docid
	u32 docseqno = headerBuff.getU32(eAosInvFseqno);	// doc seqno
	u64 docoffset = headerBuff.getU64(eAosInvFoffset);	// doc offset
	u64 docsize = headerBuff.getU64(eAosInvFoffset);	// docsize

	/*
	char status = headerBuff.getChar(eAosIndStatus);
	if (status == AosXmlDoc::eDocStatus_Delete && !readDeleted)
	{
		// The doc was deleted
		return false;
	}
	*/

	if (docseqno == eAosInvFseqno && docoffset == eAosInvFoffset && docsize == eAosInvFoffset)
	{
		// The doc was deleted
		return false;
	}

	if (docseqno == eAosInvFseqno) 
	{
		OmnAlarm << "Doc seqno invalid: " << docid << enderr;
		return false;
	}

	if (docoffset == eAosInvFoffset) 
	{
		OmnAlarm << "Doc offset invalid: " << docid << enderr;
		return false;
	}

	if (docsize == eAosInvFoffset)
	{
		OmnAlarm << "Doc size invalid: " << docid << enderr;
		return false;
	}

	// Ready to read the doc itself
	docbuff = OmnNew OmnConnBuff(docsize);
	aos_assert_r(docbuff->determineMemory(docsize), 0);
	//char *data = docbuff->getBuffer();
	//bool rslt = mTargetDocReader->readDoc(docseqno, docoffset, data, docsize);
	//aos_assert_r(rslt, false);
	docbuff->setDataLength((u32)docsize);
	return true;
}


OmnFilePtr
AosSengAdmin::openTargetIdxFile(const u32 seqno)
{
	// Ketty 2011/11/28
	/*
	aos_assert_r(seqno < eMaxIdxFiles, 0);
	if (mTargetIdxFiles[seqno]) return mTargetIdxFiles[seqno];

	OmnString idxfn = mTargetDirname;  
	idxfn << "/" << mIdxFname << "_" << seqno;
	OmnFilePtr ff = OmnNew OmnFile(idxfn, OmnFile::eReadOnly);
	aos_assert_r(ff && ff->isGood(), 0);
	mIdxFiles[seqno] = ff;
	return ff;
	*/
	return 0;
}


bool
AosSengAdmin::verifyParentCtnr(
			const u32 siteid,
			const u64 &docid,
			const AosXmlTagPtr &doc,
			bool &delFlag,
			const OmnString &ssid,
			const u64 &urldocid)
{
	// This function is not used anymore
	/*
	// This function check doc's zky_pactrs 
	// if not empty, retrieve it,
	// if empty , alarm.
	if (delFlag) return true;
	OmnString ctnr_objid = doc->getAttrStr("zky_pctrs");
	aos_assert_r(ctnr_objid != "", false);

	AosXmlTagPtr container = retrieveDocByObjid(
								siteid, ssid, ctnr_objid, false);
	if (!container)
	{
		doc->setAttr("zky_pctrs", AOSOBJIDPRE_LOSTaFOUND);
		saveFileVersion201012(doc);
		return true;
	}
	OmnString objid = doc->getAttrStr("zky_objid");
	aos_assert_r(objid != "", false);
	
	if (objid == ctnr_objid)
	{
		doc->setAttr("zky_pctrs", AOSOBJIDPRE_LOSTaFOUND);
		saveFileVersion201012(doc);
		return true;
	}
	
	return true;
	*/
	OmnShouldNeverComeHere;
	return false;
}



bool 
AosSengAdmin::checkDoc(
		const u32 siteid,
		const u64 &startdocid,
		const u64 &enddocid
		)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	u64 docid;
	OmnConnBuffPtr docbuff1;
	OmnConnBuffPtr docbuff2;
	OmnScreen << "To verify docs ..." << endl;
	DocStatus status;
	for (docid=startdocid; docid<enddocid; docid++)
	{
		if (!readDoc(docid, docbuff1, false, status, rdata)) 
		{
			OmnScreen << "Can't read Origial Doc, try another" <<endl;
			continue;
		}
		if (!readTargetDoc(docid, docbuff2))
		{
			OmnScreen << "Read TargetDoc Failed!" <<endl;
			return false;
		}

		if (docbuff1->getDataLength() == docbuff2->getDataLength() &&
			strncmp(docbuff1->getData(), docbuff2->getData(), docbuff1->getDataLength() == 0))
		{
			OmnScreen << "TargetDoc matches OrigialDoc" <<endl;
		}
		else
		{
			OmnAlarm << "TargetDoc doesn't match OrigialDoc!" <<enderr;
		}
	}
	OmnScreen << "To verify docs ... Done!" << endl;
	return true;
}

bool
AosSengAdmin::sendDeleteMsgContainerReq(
			const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &cname,
			const OmnString &container)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"reqid\">" << "delete_msgcontainer" << "</item>"
		<< "<item name=\"args\">" << "inbox=" << container << ",cname=" << cname << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
	<< "</request>";
	
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if(child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	return true;
}

bool
AosSengAdmin::sendDeleteMsgReq(
			const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &objid,
			const OmnString &container)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"reqid\">" << "delete_msg" << "</item>"
		<< "<item name=\"args\">" << "objid=" << objid << ",container=" << container << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
	<< "</request>";
	
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if(child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	return true;
}


bool
AosSengAdmin::removeDocFromServer(
			const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &objid,
		    const u64 &docid)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">delObject</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (objid != "")
	{
		req << "<item name=\"objid\"><![CDATA[" << objid << "]]></item>";
	}
	if (docid)
	{
		req << "<item name=\"" << AOSTAG_DOCID << "\">" << docid <<"</item>";
	}
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
			<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
			<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
			<< "</request>";
	}
	
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if(child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	return true;
}


bool
AosSengAdmin::getDomain(OmnString &domain) 
{

	OmnString req = "<request>";
	req	<< "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"zky_siteid\">100</item>"
		<< "<item name=\"reqid\">getdomain</item>"
		<< "<item name=\"args\">dftdomain</item>"
		<< "</request>"; 
			
	OmnString errmsg;
	OmnString resp;
	u32 siteid = 100; 
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	// The response should be in the following format:
	// 	<Contents>
	// 		<vpd ...>
	// 		...
	// 		</vpd>
	//		...
	//		</obj>
	//	</Contents>
	child = root->getFirstChild("Contents");
	aos_assert_r(child, false);
	domain = child->getNodeText();
	return true;
}


bool 
AosSengAdmin::checkHtmlGenerate(
		const u32 siteid, 
		const u64 &startdocid, 
		const u64 &enddocid)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnString ssid;
	u64 urldocid = 0;
	//u64 userid;
	AosXmlTagPtr userdoc;

	OmnConnBuffPtr docbuff;
	AosXmlParser parser;

	//login("yuhui", "12345",	"yunyuyan_account", ssid, userid, userdoc, siteid);
	
	int group = 0;
	for (u64 docid=startdocid; docid<enddocid; docid++)
	{
		if (group >= sgGroupSize)
		{
			OmnSleep(sgSleepLength);
			group = 0;
		}

		DocStatus status;
		if (!readDoc(docid, docbuff, false, status, rdata)) 
		{
			OmnScreen << "Skip (failed reading): " << docid << endl;
			continue;
		}

		if (!docbuff)
		{
			OmnAlarm << "Buffer is null: " << docid << enderr;
			continue;
		}

		AosXmlTagPtr local_doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
		if (!local_doc)
		{
			OmnAlarm << "Failed to parse the doc: " 
				<< docid << ":" << docbuff->getData() << enderr;
			continue;
		}
		
		// normalize the objid
		local_doc->normalize();
		OmnString objid;
		if ((objid = local_doc->getAttrStr(AOSTAG_OBJID)) == "")
		{
			OmnAlarm << "Doc missing objid: " << local_doc->toString() << enderr;
			continue;
		}

		//if (local_doc->getAttrStr(AOSTAG_OTYPE) == "vpd")
		//{
			//OmnScreen << local_doc->toString() << endl;
			generateHtml(objid, ssid, urldocid);
		//}

		group++;
		cout << __FILE__ << ":" << __LINE__ << " Checked: " << docid << endl;
	}

	return true;
}


OmnString 
AosSengAdmin::generateHtml(
		const OmnString &vpdname,
		const OmnString &ssid, 
		const u64 &urldocid,
		bool fullPage) 
{
	OmnString req;
	if (!fullPage)
	{
		// The resquest should be in the following format:
		//<request>
		//	<item name='operation'><![CDATA[retrieveVpd]]></item>
		//	<item name='objid'><![CDATA[meiluo_image_mouser_big]]></item>
		//	<zky_cookies>
		//		<cookie zky_name="zky_ssid"><![CDATA[PFQTAZzUWAQQdksWgAWVAHhLc]]></cookie>
		//	</zky_cookies>
		//</request>

		req = "<request>";
		req	<< "<item name=\"operation\"><![CDATA[retrieveVpd]]></item>"
			<< "<item name='objid'><![CDATA["<< vpdname << "]]></item>"
			<< "<item name=\"zky_siteid\">100</item>"
			<< "<zky_cookies>"
			<< "<cookie zky_name=\"zky_ssid\"><![CDATA[" << ssid << "]]></cookie>"
			<< "</zky_cookies>"
			<< "</request>"; 

	}
	else 
	{
		// The resquest should be in the following format:
		//<request>
		//	<item name='operation'><![CDATA[retrieveFullVpd]]></item>
		//	<item name='url'><![CDATA[http://192.168.99.95:8080/lps-4.7.2/publish/meiluo.html]]></item>
		//	<zky_cookies>
		//		<cookie zky_name="zky_ssid"><![CDATA[BGAsAEIBJATl9SSUyAK1AcvUK]]></cookie>
		//	</zky_cookies>
		//</request>
		req = "<request>";
		req	<< "<item name=\"operation\"><![CDATA[]]></item>"
			<< "<item name='objid'><![CDATA["<< vpdname<< "]]></item>"
			<< "<item name=\"zky_siteid\">100</item>"
			<< "<zky_cookies>"
			<< "<cookie zky_name=\"zky_ssid\"><![CDATA[" << ssid << "]]></cookie>"
			<< "</zky_cookies>"
			<< "</request>"; 
	}

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, "");
	aos_assert_r(mConn->procRequest(100, "", "", req, resp, errmsg), "");
	aos_assert_r(resp != "", "");	

	return resp;
}


bool
AosSengAdmin::readHeaderVersion201012(
		const u64 &docid,
		u32 &seqno, 
		u64 &offset, 
		u64 &docsize, 
		AosSengAdmin::DocStatus &status) 
{
	// This is the version that reads docs for 2010/12 releases. 
	// The header format is:
	// 		version		u32
	// 		siteid		u32
	// 		docid		u64
	// 		seqno		u32
	// 		offset		u64
	// 		docsize		u64
	// 		status		char
	// 		ctime		u32
	// 		mtime		u32
	//
	//  below are optional attributes, which are all string type:
	// 		vvpd
	// 		evpd
	// 		doctype
	// 		subtype
	// 		objid
	// 		thmnail
	// 		appname
	// 		containers
	// 		creator
	// 		modifier
	//
	int maxHeaderPerFile = eAosMaxFileSize/AosXmlDoc::eDocHeaderSize;

	status = eBad;

	// Create the buff for header
	char bb[AosXmlDoc::eDocHeaderSize];

	seqno = docid / maxHeaderPerFile;
	offset = (docid % maxHeaderPerFile) * AosXmlDoc::eDocHeaderSize;

	OmnFilePtr idxfile = mIdxFiles[seqno];
	if (!idxfile) idxfile = openIdxFile(seqno);
	aos_assert_r(idxfile, false);

	int len = idxfile->readBinaryInt(offset, -1);
	if (len <= 0 || (u32)len > AosXmlDoc::eDocHeaderSize)
	{
		// Failed reading the header. Consider this as not a valid header
		status = eInvalid;
		return false;
	}

	status = eBad;
	len = idxfile->readToBuff(offset+4, len, bb);
	aos_assert_r(len >= 0, false);
	AosBuff headerBuff(bb, len, len AosMemoryCheckerArgs);

	u32 version = headerBuff.getU32(AosXmlDoc::eInvVersion);
	u32 siteid  = headerBuff.getU32(AOS_INVSID);
	u64 did 	= headerBuff.getU64(AOS_INVDID);
	seqno		= headerBuff.getU32(eAosInvFseqno);
	offset		= headerBuff.getU64(eAosInvFoffset);
	docsize		= headerBuff.getU64(eAosInvDocSize);
	char st     = headerBuff.getChar(eAosIndStatus);
	u32 ctime   = headerBuff.getU32(eAosInvTime);
	u32 mtime	= headerBuff.getU32(eAosInvTime);
	char flag   = headerBuff.getChar(AosXmlDoc::eInvStorageFlag);

	status = eGood;
	if (st != AosXmlDoc::eDocStatus_Active)
	{
		status = eDeleted;
		return false;
	}

	if (version == AosXmlDoc::eInvVersion)
	{
		OmnAlarm << "Version invalid: " << docid << enderr;
		status = eBad;
	}

	if (siteid == AOS_INVSID)
	{
		OmnAlarm << "Siteid invalid: " << docid << enderr;
		status = eBad;
	}

	if (ctime == eAosInvTime)
	{
		// Some docs' ctime was not set correctly. We will 
		// ignore them for now. 
		// OmnAlarm << "Invalid ctime: " << docid << enderr;
		// is_bad = true;
	}

	if (mtime == eAosInvTime)
	{
		// OmnAlarm << "Invalid mtime: " << docid << enderr;
		// is_bad = true;
	}

	if (flag == AosXmlDoc::eInvStorageFlag)
	{
		OmnAlarm << "Invalid flag: " << docid << enderr;
		status = eBad;
	}

	if (did != docid)
	{
		OmnAlarm << "Docid mismatch: " << did << ":" << docid << enderr;
		status = eBad;
	}
	if (seqno == eAosInvFseqno)
	{
		OmnAlarm << "Invalid seqno: " << docid << enderr;
		status = eBad;
	}
	if (offset == eAosInvFoffset)
	{
		OmnAlarm << "Invalid offset: " << docid << enderr;
		status = eBad;
	}

	if (docsize == eAosInvFoffset)
	{
		OmnAlarm << "Invalid docsize: " << docid << enderr;
		status = eBad;
	}

	if (status == eBad) return false;
	status = eGood;
	return true;
}


bool 
AosSengAdmin::scanDocs1(
		const u64 &startDocid,
		const u64 &maxDocid,
		const u32 siteid)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	u64 docid;
	OmnConnBuffPtr docbuff;
	DocStatus status;
	AosXmlParser parser;
	for (docid=startDocid; docid<=maxDocid; docid++)
	{
		OmnScreen << "Read doc: " << docid << endl;
		readDoc(docid, docbuff, false, status, rdata);
		if (!docbuff) continue;
		OmnString str(docbuff->getData(), docbuff->getDataLength());
		modifyIILTypeAttr(str);
		AosXmlTagPtr doc = parser.parse(str, "" AosMemoryCheckerArgs);
		if (!doc)
		{
			OmnAlarm << "doc parser error" << enderr;
			continue;
		}
		saveFileVersion201012(doc, rdata);
	}
	return true;
}


bool
AosSengAdmin::scanDocs(
		const u64 &startDocid,
		const u64 &maxDocid, 
		const u32 siteid)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	// Login to remote server
	OmnString ssid;
	u64 urldocid = 0;
	AosXmlTagPtr userdoc;

	OmnConnBuffPtr docbuff;
	OmnString oid;
	CheckRslts rslts;
	u64 last_docid = 0;
	DocStatus status;
	AosXmlParser parser;
	AosXmlTagPtr doc;
	bool delFlag;
	u64 docid;
	for (docid=startDocid; docid<=maxDocid; docid++)
	{
		delFlag = false;
		OmnScreen << "Read doc: " << docid << endl;
		readDoc(docid, docbuff, false, status, rdata);
		switch (status)
		{
		case eInvalid:
			 rslts.num_invalid++;
			 break;

		case eDeleted:
			 rslts.num_deleted++;
			 break;

		case eBad:
			 rslts.num_bad++;
			 break;

		case eGood:
			 checkDoc(siteid, docid, docbuff, delFlag, rslts, ssid, urldocid);
			 break;

		default:
			 OmnAlarm << "Unrecognized status: " << status << enderr;
			 rslts.num_errors++;
			 break;
		}
	}

	int num_non_exist = docid - last_docid;
	rslts.num_invalid -= num_non_exist;
	OmnScreen << rslts.toString() << endl;
	return true;
}


bool
AosSengAdmin::removeEmbeddedImg(
		AosXmlTagPtr &doc, 
		bool &delFlag, 
		const AosRundataPtr &rdata)
{
	// This function checks whether 'doc' is too big. If yes, it checks
	// whether it contains an embedded image, which is in the form:
	// 	<img src=|&&|data:image/png;base64.../>
	OmnString str = doc->toString();
	int len = str.length();
	char *data = (char *)str.data();
	int start_idx = 0;
	bool modified = false;
	while (1)
	{
	    char *ptr = strstr((char*)&data[start_idx], "<img src=|&&|data:image/png;base64");
		if (!ptr) 
		{
			if (modified)
			{
				// save the doc.
				str.setLength(len);
				doc = AosXmlParser::parse(str AosMemoryCheckerArgs);
				if (!doc)
				{
					OmnAlarm << "Failed to parse xml";
					return false;
				}
				saveFileVersion201012(doc, rdata);
			}
			return true;
		}

		modified = true;
		// Found one. Scan all the way to the next '>'
		int found_pos = ptr - data;
		int idx = found_pos;
		while (idx < len && data[idx] != '>') idx++;
		// Found the image block. Remove it.
		aos_assert_r(idx < len, false);
	
		int moved_len = len - idx;
//		if (moved_len < 100) continue;
		
		memmove((void*)&data[found_pos], &data[idx], moved_len);
		
		start_idx += found_pos;
		len -= (idx - found_pos);
		data[len] = 0;
		/*
		int moved_len = idx - found_pos;
		memmove((void*)&data[found_pos], &data[idx], moved_len);
		
		start_idx += idx;
		len -= idx;
		*/
	}
	return true;
}


bool
AosSengAdmin::deleteDocVersion201012(const u64 &docid)
{
	OmnNotImplementedYet;
	return false;
	/*
	int maxHeaderPerFile = eAosMaxFileSize/AosXmlDoc::eDocHeaderSize;
	char bb[AosXmlDoc::eDocHeaderSize];
	u32 header_seqno = docid / maxHeaderPerFile;
	u64 header_offset = (docid % maxHeaderPerFile) * AosXmlDoc::eDocHeaderSize;

	OmnFilePtr idxfile = mIdxFiles[header_seqno];
	if (!idxfile) idxfile = openIdxFile(header_seqno);
	aos_assert_r(idxfile, false);

	int len = idxfile->readBinaryInt(header_offset, -1);
	aos_assert_r(len > 0 && (u32)len <= AosXmlDoc::eDocHeaderSize, false);
	len = idxfile->readToBuff(header_offset+4, len, bb);
	aos_assert_r(len >= 0, false);
	AosBuff headerBuff(bb, len, len AosMemoryCheckerArgs);

	headerBuff.getU32(AosXmlDoc::eInvVersion);
	headerBuff.getU32(AOS_INVSID);
	headerBuff.getU64(AOS_INVDID);
	headerBuff.getU32(eAosInvFseqno);
	headerBuff.getU64(eAosInvFoffset);
	headerBuff.getU64(eAosInvDocSize);
	headerBuff.getChar(eAosIndStatus);
	headerBuff.getU32(eAosInvTime);
	headerBuff.getU32(eAosInvTime);
	headerBuff.getChar(AosXmlDoc::eInvStorageFlag);

	u32 sec = OmnGetSecond();
	AosBuff buff(bb, AosXmlDoc::eDocHeaderSize, 0 AosMemoryCheckerArgs);
	buff.setU32(AosXmlDoc::eInvVersion);
	buff.setU32(AOS_INVSID);
	buff.setU64(AOS_INVDID);
	buff.setU32(eAosInvFseqno);
	buff.setU64(eAosInvFoffset);
	buff.setChar(AosXmlDoc::eDocStatus_Deleted);
	buff.setU32(sec);
	buff.setU32(sec);
	buff.setChar(AosXmlDoc::eInvStorageFlag);

	aos_assert_r(idxfile->put(header_offset, buff.data(), buff.dataLen(), true), false);
	// The caller should log the deletion. 
	return true;
	*/
}


bool
AosSengAdmin::checkDoc(
		const u32 siteid,
		const u64 &docid,
		const OmnConnBuffPtr &docbuff, 
		bool &delFlag,
		AosSengAdmin::CheckRslts &rslts,
		const OmnString &ssid,
		const u64 &urldocid)
{
	// This function assumes a doc has been retrieved (in 'docbuff'). 
	// It parses the doc and does the following check.
	// 1. If failed parsing, set rslts.num_syntax;
	// 2. Whether docid in doc == 'docid'
	// 3. Whether it has objid
	// 4. Whether its objid is duplicated
	// 5. If it is a user account, whether its cloudid unique
	// 6. AOSTAG_OTYPE is set
	// 7. AOSTAG_CREATOR is a valid cloudid
	// 8. AOSTAG_MODIFIER is a valid cloudid
	// 9. AOSTAG_STYPE is set
	
	//OmnString str(docbuff->getData(), docbuff->getDataLength());
	//modifyIILTypeAttr(str);

	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
	if (!doc)
	{
		rslts.num_syntax++;
		OmnAlarm << "Failed parse doc: " << docid << enderr;
		return false;
	}

	verifyDocid(siteid, docid, doc, delFlag, rslts);
	verifyObjid(siteid, docid, doc, delFlag, rslts);
	verifyCloudid(siteid, docid, doc, delFlag, rslts);
	verifyCreator(siteid, docid, doc, delFlag, rslts, ssid, urldocid);
	removeEmbeddedImg(doc, delFlag, rdata);
	verifyMutiCtnr(siteid, docid, doc, delFlag);
	// verifyParentCtnr(siteid, docid, doc, delFlag, ssid);
	verifyArcd(siteid, docid, doc, delFlag);
	verifyUserAct(siteid, docid, doc, delFlag);
	
	//OmnString str(docbuff->getData(), docbuff->getDataLength());
	//modifyIILTypeAttr(str);

	//saveFileVersion201012(doc);

	rslts.num_docs++;
	return true;
}


bool
AosSengAdmin::verifyUserAct(
		const u32 siteid,
		const u64 &docid, 
		const AosXmlTagPtr &doc,
		bool &delFlag)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	if (delFlag) return true;
	
	aos_assert_r(doc, false);
	OmnString otype = doc->getAttrStr("zky_otype");
	if (otype != "zky_uact") return true;
	OmnString heditor = doc->getAttrStr("zky_heditor");
	if (heditor == "")
	{
		doc->setAttr("zky_heditor", "1");
		saveFileVersion201012(doc, rdata);
		return true;
	}

	return true;
}


bool
AosSengAdmin::modifyIILTypeAttr(OmnString &str)
{
	static OmnString replacesrc[5] = {
		"zky_cloudid", 
		"zky_ctmepo", 
		"zky_mtmepo", 
		"zky_ver",
		"zky_passwd"};
	static OmnString replacedsc[5] = {
		"zky_cloudid__a", 
		"zky_ctmepo__d", 
		"zky_mtmepo__d",
		"zky_ver__a", 
		"zky_passwd__n"};
	static OmnString replaceinsert[5] = {
		"__a", 
		"__d", 
		"__d", 
		"__a", 
		"__n"};

	for (int i=0; i<5 ;i++)
	{
		int pos = 0;
		while(1)
		{
			pos = str.findSubString(replacesrc[i], pos);
			if (pos == -1 && pos < str.length())
			{
				break;
			}
			if (strncmp(replacedsc[i].data(), str.data()+pos, replacedsc[i].length()))
			{
				str.insert(replaceinsert[i],pos+replacesrc[i].length());
			}
			pos++;
		}
	}
	return true;

}


bool
AosSengAdmin::verifyArcd(
		const u32 siteid,
		const u64 &docid,
		const AosXmlTagPtr &doc, 
		bool &delFlag)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	if (delFlag) return true;
	
	aos_assert_r(doc, false);

	OmnString docstr = doc->toString();
	if (docstr.length() < 10) return true;
	OmnString tagname = docstr.substr(1, 9);
	if (tagname != "container") return true;
	OmnString otype = doc->getAttrStr(AOSTAG_OTYPE);
	if (otype != "zky_arcd") return true;
	doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_CONTAINER);
	saveFileVersion201012(doc, rdata);
	return true;
}


bool
AosSengAdmin::verifyDocid(
		const u32 siteid,
		const u64 &docid,
		const AosXmlTagPtr &doc, 
		bool &delFlag,
		AosSengAdmin::CheckRslts &rslts)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	// Verify docid: the docid retrieved from the doc must
	// be the same as 'docid'.
	if (delFlag) return true;

	u64 did = doc->getAttrU64(AOSTAG_DOCID, 0);
	if (docid != 0 && did == docid) 
	{
		if (rslts.docidExists(docid))
		{
			OmnAlarm << "Docid already exists: " << docid << enderr;
			return false;
		}
		rslts.docids[docid] = 'a';
		return true;
	}

	OmnAlarm << "Docid invalid. The real docid is: " << docid 
		<< ", but the docid in the doc is: "
		<< did << enderr;
	OmnScreen << "The doc in the real location (bad one: "
		<< docid << "):\n"
		<< doc->toString() << endl;

	AosXmlTagPtr dd = readDocByDocid(did, siteid);
	if (!dd)
	{
		OmnAlarm << "Failed to read the doc: " << did << enderr;
		rslts.num_errors++;
		return false;
	}
		
	OmnScreen << "The doc (correct one: " << did << "): \n"
		<< dd->toString() << endl;

	// deleteDocVersion201012(docid);
	deleteDocVersion201102(docid, rdata);
	delFlag = true;
	rslts.num_docs_deleted++;
	return false;
}

	
bool
AosSengAdmin::verifyCreator(
		const u32 siteid,
		const u64 &docid,
		const AosXmlTagPtr &doc,
		bool &delFlag,
		AosSengAdmin::CheckRslts &rslts,
		const OmnString &ssid,
		const u64 &urldocid)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	// Get creator form doc, if empty, set AOSCLOUDID_UNKNOWN.
	if (delFlag) return true;
	OmnString creator = doc->getAttrStr(AOSTAG_CREATOR);
	if (creator == "")
	{
		rslts.num_no_creators++;
		doc->setAttr(AOSTAG_CREATOR, AOSCLOUDID_UNKNOWN);
		saveFileVersion201012(doc, rdata);
		return true;
	}

	if (rslts.cloudidExists(creator))
	{
		return true;
	}

	if (rslts.docidExists(atoll(creator.data())))
	{
		// The creator is a docid. Need to be cloudid. 
		OmnString cid = rslts.getCidByDocid(atoll(creator.data()));
		if (cid != "")
		{
			doc->setAttr(AOSTAG_CREATOR, cid);
			saveFileVersion201012(doc, rdata);
			return true;
		}
	}

	doc->setAttr(AOSTAG_CREATOR, AOSCLOUDID_UNKNOWN);
	saveFileVersion201012(doc, rdata);
	rslts.num_no_creators++;
	return true;

	/*
	// If not empty, assume it is cloud id , retrieve doc by cloud id,
	// if exist ,do nothing, else assume it is a docid, retrieve doc
	// by docid, if exist, do nothing, if not exist, set AOSCLOUDID_UNKNOWN.
	AosXmlTagPtr userdoc = retrieveDocByCloudid(siteid, creator);
	if (!userdoc)
	{
		u64 did = atoll(creator);
		userdoc = retrieveDocByDocid(siteid, ssid, did, true);
		if (!userdoc)
		{
			rslts.num_no_creators++;
			doc->setAttr(AOSTAG_CREATOR, AOSCLOUDID_UNKNOWN);
			saveFileVersion201012(doc);
			return true;
		}
		else
		{
			// This means the creator was a docid. Need to retrieve the Cloudid
			// and change the creator.
			OmnString cid = userdoc->getAttrStr(AOSTAG_CLOUDID);
			if (cid == "")
			{
				OmnAlarm << "Doc missing cloudid: " << userdoc->toString() << enderr;
				return false;
			}
		
			doc->setAttr(AOSTAG_CREATOR, cid);
			saveFileVersion201012(doc);
		}
	}
	*/

	return true;
}


bool
AosSengAdmin::verifyObjid(
		const u32 siteid,
		const u64 &docid,
		const AosXmlTagPtr &doc,
		bool &delFlag,
		AosSengAdmin::CheckRslts &rslts)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	// Check whether objid is duplicated.
	if (delFlag) return true;
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	if (objid == "")
	{
		rslts.num_no_objids++;
		return true;
	}

	AosStr2U64Itr_t itr = rslts.objids.find(objid);
	if (itr == rslts.objids.end())
	{
		rslts.objids[objid] = docid;
		return true;
	}

	rslts.num_dup_objids++;
	OmnAlarm << "Objid duplicated: " << objid
		<< ". The first docid: " << itr->second
		<< ". The current docid: " << docid << enderr;

	AosXmlTagPtr dd = readDocByDocid(itr->second, siteid);
	if (!dd)
	{
		OmnAlarm << "Failed to read the doc: " << itr->second << enderr;
		rslts.num_errors++;
		return false;
	}

	// Special Case 1: 
	// 1. both are access records
	// 2. zky_owndocid are the same
	// 3. stype is auto
	// we will remove the older one automatically.
	if (dd->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD &&
		doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD &&
		dd->getAttrStr(AOSTAG_OWNER_DOCID) == doc->getAttrStr(AOSTAG_OWNER_DOCID))
	{
		OmnScreen << "Both are access records and both for the same doc. Delete the"
			" older one!" << endl;
		// deleteDocVersion201012(itr->second);
		deleteDocVersion201102(itr->second, rdata);
		delFlag = true;
		rslts.num_docs_deleted++;
		return false;
	}

	// Special Case 2:
	// 1. otype identical
	// 2. stype identical
	// 3. siteid identifical
	// 4. AOSTAG_OWNER_DOCID identical
	// 5. AOSTAG_PARENTC identical
	// 6. otype is AOSOTYPE_CONTAINER
	// Remove the older one
	if (doc->getAttrStr(AOSTAG_OTYPE) == dd->getAttrStr(AOSTAG_OTYPE) &&
		doc->getAttrStr(AOSTAG_STYPE) == dd->getAttrStr(AOSTAG_STYPE) &&
		doc->getAttrStr(AOSTAG_SITEID) == dd->getAttrStr(AOSTAG_SITEID) &&
		doc->getAttrStr(AOSTAG_OWNER_DOCID) == dd->getAttrStr(AOSTAG_OWNER_DOCID) &&
		doc->getAttrStr(AOSTAG_PARENTC) == dd->getAttrStr(AOSTAG_PARENTC) &&
		doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER)
	{
		OmnScreen << "Both are containers and both for the same doc. Delete the"
			" older one!" << endl;
		// deleteDocVersion201012(itr->second);
		deleteDocVersion201102(itr->second, rdata);
		delFlag = true;
		rslts.num_docs_deleted++;
		return false;
	}

	// Special Case 3:
	// 1. otype is vpd
	if (doc->getAttrStr(AOSTAG_OTYPE) == "vpd")
	{
		OmnScreen << "The first doc: " << dd->toString() << endl;
		OmnScreen << "\nThe current doc: " << doc->toString() << endl;
		OmnScreen << "\nBoth are vpd. Do you want to rename the previous one (1),"
			" or the current one (2), or do nothing (3)?" << endl;
		char selection[100];
		cin >> selection;
		AosXmlTagPtr selected_doc;
		if (selection[0] == '1')
		{
			selected_doc = dd;
		}
		else if (selection[0] == '2')
		{
			selected_doc = doc;
		}
		else
		{
			return false;
		}
		
		// handler objid by this
		objid << sgInvalidObjid++; 
		selected_doc->setAttr(AOSTAG_OBJID, objid);
		saveFileVersion201012(selected_doc, rdata);
		return true;
	}

	// Special Case 4: 
	// 1. stype is auto
	// 2. has owndocid
	// 3. has addmem
	// 4. has delmem
	// 5. has create
	// 6. has vote
	if (dd->getAttrStr(AOSTAG_STYPE) == doc->getAttrStr(AOSTAG_STYPE) &&
		dd->getAttrStr(AOSTAG_STYPE) == AOSSTYPE_AUTO &&
		dd->getAttrStr(AOSTAG_OWNER_DOCID) == doc->getAttrStr(AOSTAG_OWNER_DOCID) &&
		dd->getAttrStr(AOSTAG_OWNER_DOCID) != "" &&
		dd->getAttrStr(AOSTAG_ADDMEM_ACSTYPE) != "" &&
		dd->getAttrStr(AOSTAG_DELMEM_ACSTYPE) != "" &&
		dd->getAttrStr(AOSTAG_CREATE_ACSTYPE) != "" &&
		dd->getAttrStr(AOSTAG_VOTE_ACCESSES) != "")
	{
		// deleteDocVersion201012(itr->second);
		deleteDocVersion201102(itr->second, rdata);
		delFlag = true;
		rslts.num_docs_deleted++;
		return false;
	}

	// Special Case 5:
	// 1. One is container and the other access record
	// 2. Access record is auto
	// 3. Both have owndocid
	// 4. container stype is "zky_usrpf"
	// Remove the access record
	AosXmlTagPtr ctnrdoc;
	AosXmlTagPtr arddoc;
	if (dd->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER &&
		doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD)
	{
		ctnrdoc = dd;
		arddoc = doc;
	}
	else if (doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER &&
			 dd->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD)
	{
		ctnrdoc = doc;
		arddoc = dd;
	}

	if (ctnrdoc && arddoc)
	{
		if (arddoc->getAttrStr(AOSTAG_STYPE) == AOSSTYPE_AUTO &&
			arddoc->getAttrStr(AOSTAG_OWNER_DOCID) != "" &&
			ctnrdoc->getAttrStr(AOSTAG_OWNER_DOCID) != "" &&
			ctnrdoc->getAttrStr(AOSTAG_STYPE) != AOSSTYPE_USER_PFOLDER)
		{
			// deleteDocVersion201012(arddoc->getAttrU64(AOSTAG_DOCID, 0));
			deleteDocVersion201102(arddoc->getAttrU64(AOSTAG_DOCID, 0), rdata);
			delFlag = true;
			rslts.num_docs_deleted++;
			return false;
		}
	}

	OmnScreen << "The first doc: " << dd->toString() << endl;
	OmnScreen << "\nThe current doc: " << doc->toString() << endl;
	OmnScreen << "Do you want to remove the current one (1);\n"
		<< "The previous one (2)\n" 
		<< "Do nothing (3)" << endl;
	char selection[100];
	cin >> selection;
	if (strcmp(selection, "1") == 0)
	{
		// deleteDocVersion201012(docid);
		deleteDocVersion201102(docid, rdata);
		delFlag = true;
		rslts.num_docs_deleted++;
		return false;
	}

	if (strcmp(selection, "2") == 0)
	{
		// deleteDocVersion201012(itr->second);
		deleteDocVersion201102(itr->second, rdata);
		delFlag = true;
		rslts.num_docs_deleted++;
		return false;
	}

	return false;
}


bool
AosSengAdmin::saveFileVersion201012(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	//int maxHeaderPerFile = eAosMaxFileSize/AosXmlDoc::eDocHeaderSize;
	//int maxHeaderPerFile = AosXmlDoc::getMaxHeaderPerFile();
	char bb[AosXmlDoc::eDocHeaderSize];
	//u32 header_seqno = docid / maxHeaderPerFile;
	//u64 header_offset = (docid % maxHeaderPerFile) * AosXmlDoc::eDocHeaderSize;
	
	int len;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
	//docfilemgr->readHeader(header_seqno, header_offset, bb, AosXmlDoc::eDocHeaderSize, len, sgRundata);
	AosBuff headerBuff(bb, len, len AosMemoryCheckerArgs);
	
	headerBuff.getU32(AosXmlDoc::eInvVersion);
	headerBuff.getU32(AOS_INVSID);
	headerBuff.getU64(AOS_INVDID);
	u32 doc_seqno = headerBuff.getU32(eAosInvFseqno);
	u64 doc_offset = headerBuff.getU64(eAosInvFoffset);
	headerBuff.getU64(eAosInvDocSize);

	OmnString str = doc->toString();
	bool brslt;// = mOrigDocReader->saveDoc(doc_seqno, doc_offset, str.length(), str.data());
	//u32 compressed_size = 0;
	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
	//bool brslt = docfilemgr->saveDoc(doc_seqno, doc_offset, str.length(), str.data(), false, compressed_size, sgRundata);
	headerBuff.setCrtIdx(16);
	headerBuff.setU32(doc_seqno);
	headerBuff.setU64(doc_offset);
	headerBuff.setU64(str.length());

	//bool hrslt = mOrigDocReader->saveHeader(header_seqno, header_offset, headerBuff.data(), headerBuff.dataLen(), true);
	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
	bool hrslt;// = docfilemgr->saveHeader(header_seqno, header_offset, headerBuff.data(), headerBuff.dataLen(), true, sgRundata);
	aos_assert_r(brslt && hrslt, false);

	/*
	// Sanity Check
	char *buff = OmnNew char[str.length() + 100];
	bool rslt = mOrigDocReader->readDoc(doc_seqno, doc_offset, buff, str.length());
	aos_assert_r(rslt, false);
	AosXmlParser parser;
	AosXmlTagPtr dd = parser.parse(buff, "" AosMemoryCheckerArgs);
	aos_assert_r(dd, false);
	delete buff;
	*/
	return true;
}


bool
AosSengAdmin::verifyMutiCtnr(
		const u32 siteid,
		const u64 &docid,
		const AosXmlTagPtr &doc,
		bool &delFlag)
{
	// Some doc may have many containers,eg: zky_pctrs="xxx,xxx,...",
	// Now we split the attribute, and set the first as the zky_pctrs,
	// and the others set as AOSTAG_MEMBEROF
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	if (delFlag) return true;
	aos_assert_r(docid != 0, false);
	aos_assert_r(doc, false);

	OmnString ctnr_objid = doc->getAttrStr(AOSTAG_PARENTC);
	if (ctnr_objid == "")
	{
		doc->setAttr(AOSTAG_PARENTC, AOSOBJIDPRE_LOSTaFOUND);
		saveFileVersion201012(doc, rdata);
		return true;
	}

	// First ,we split str by ",", then split str by " "
	OmnString part;
	OmnStrParser1 parser(ctnr_objid, ", ", false, false);
	OmnString parent_objid;
	int num_parents = 0;
	map<OmnString, OmnString> themap;
	while ((part = parser.nextWord()) != "")
	{
		num_parents++;
		if (num_parents == 1)
		{
			parent_objid = part;
		}
		
		themap[part] = part;
	}

	aos_assert_r(parent_objid != "", false);
	if (num_parents == 1)
	{
		if (parent_objid != ctnr_objid)
		{
			doc->setAttr(AOSTAG_PARENTC, parent_objid);
			saveFileVersion201012(doc, rdata);
			return true;
		}
	}

	doc->setAttr(AOSTAG_PARENTC, parent_objid);
	saveFileVersion201012(doc, rdata);

	OmnString mm = doc->getAttrStr(AOSTAG_MEMBEROF);
	if (mm != "")
	{
		OmnStrParser1 parser1(mm, ", ", false, false);
		while ((part = parser1.nextWord()) != "")
		{
			themap[part] = part;
		}
	}

	map<OmnString, OmnString>::iterator itr;
	int idx = 0;
	OmnString memberofs;
	themap.erase(parent_objid);
	for (itr = themap.begin(); itr != themap.end(); itr++)
	{
		if (idx > 0) memberofs << ",";
		memberofs << itr->second;
		idx++;
	}
	doc->setAttr(AOSTAG_MEMBEROF, memberofs);
	saveFileVersion201012(doc, rdata);

	return true;
}


bool
AosSengAdmin::verifyCloudid(
		const u32 siteid,
		const u64 &docid,
		const AosXmlTagPtr &doc,
		bool &delFlag,
		AosSengAdmin::CheckRslts &rslts)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	if (delFlag) return true;
	if (doc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_USERACCT) return true;
	OmnString cid = doc->getAttrStr(AOSTAG_CLOUDID);
	if (cid == "")
	{
		OmnScreen << "User account missing cloudid: "
			<< doc->toString() << endl;
		OmnScreen << "Do you want to delete the doc(yes)?" << endl;

		char selection[100];
		cin >> selection;
		if (strcmp(selection, "yes") == 0)
		{
			// deleteDocVersion201012(docid);
			deleteDocVersion201102(docid, rdata);
			delFlag = true;
			rslts.num_docs_deleted++;
		}
		else
		{
			rslts.num_errors++;
		}
		return false;
	}

	aos_assert_r(cid != "", false);
	AosStr2U64Itr_t itr = rslts.cloudids.find(cid);
	if (itr == rslts.cloudids.end())
	{
		rslts.cloudids[cid] = docid;
		rslts.docid2Cid[docid] = cid;
		return true;
	}

	rslts.num_dup_cids++;
//	OmnAlarm << "Cloudid duplicated: " << cid
//		<< ". The first docid: " << itr->second
//		<< ". The current docid: " << docid << enderr;

	AosXmlTagPtr dd = readDocByDocid(itr->second, siteid);
	if (!dd)
	{
		OmnAlarm << "Failed to read the doc: " << itr->second << enderr;
		rslts.num_errors++;
		return false;
	}

	OmnScreen << "The first doc: " << dd->toString() << endl;
	OmnScreen << "\nThe current doc: " << doc->toString() << endl;
	OmnScreen << "Do you want to remove the current one (1);\n"
		<< "The previous one (2)\n" 
		<< "Do nothing (3)" << endl;
	char selection[100];
	cin >> selection;
	if (strcmp(selection, "1") == 0)
	{
		// deleteDocVersion201012(docid);
		deleteDocVersion201102(docid, rdata);
		delFlag = true;
		rslts.num_docs_deleted++;
		return false;
	}

	if (strcmp(selection, "2") == 0)
	{
		// deleteDocVersion201012(itr->second);
		deleteDocVersion201102(itr->second, rdata);
		delFlag = true;
		rslts.num_docs_deleted++;
		return false;
	}

	return false;
}


bool
AosSengAdmin::fixCreatorProblem(
		const u32 siteid, 
		const u64 &start_docid, 
		const u64 &end_docid)
{
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	u64 urldocid;
	bool rslt = login("chending", "chen0318", "yunyuyan_account", ssid, urldocid, 
			userid, userdoc, siteid, "");
	aos_assert_r(rslt, false);

	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	int group = 0;
	DocStatus status;
	for (u64 docid=start_docid; docid <= end_docid; docid++)
	{
		if (group++ >= sgGroupSize)
		{
			OmnSleep(sgSleepLength);
			group = 0;
		}

		if (!readDoc(docid, docbuff, false, status, rdata)) continue;
		AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
		if (!doc)
		{
			OmnAlarm << "Failed to parse the doc: " 
				<< docid << ":" << docbuff->getData() << enderr;
			continue;
		}
		
		// Retrieve the doc to see whether they are the same
		OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
		AosXmlTagPtr orig_doc = retrieveDocByObjid(siteid, ssid, urldocid, objid);
		if (!orig_doc)
		{
			OmnAlarm << "Failed to retrieve the doc: " 
				<< doc->getAttrStr(AOSTAG_OBJID) << enderr;
			continue;
		}

		OmnString creator = doc->getAttrStr(AOSTAG_CREATOR);
		//if (creator != "" && creator.isDigitStr() && creator != AOSCLOUDID_UNKNOWN &&
		//	creator != AOSCLOUDID_GUEST && creator != AOSCLOUDID_SYSTEM)
		if (creator != "" && creator.isDigitStr() && creator != AOSCLOUDID_UNKNOWN)
		{
			// This is a valid creator. But need to check whether it is
			// indeed a cloud id. 
			OmnString cid = retrieveCloudid(siteid, ssid, urldocid, creator);
			if (cid != "")
			{
				// The creator is not a cloudid. Change it.
				creator = cid;
			}
			else
			{
				if (!isValidCloudid(siteid, ssid, urldocid, creator))
				{
					OmnScreen << "(1)The creator is not a valid cloudid: "
						<< creator << ". Do you want to change to unknown(1) "
						<< " or ignore it (2), use the creator (3)?" 
						<< " The original creator: " 
						<< orig_doc->getAttrStr(AOSTAG_CREATOR) << endl;
					char buff[100];
					cin >> buff;
					if (buff[0] == '1')
					{
						creator = AOSCLOUDID_UNKNOWN;
					}
					else if (buff[0] == '2')
					{
						continue;
					}
				}
			}

			if (orig_doc->getAttrStr(AOSTAG_CREATOR) != creator)
			{
				// Need to modify the creator. 
				if (!modifyAttr(siteid, ssid, urldocid, 
					doc->getAttrStr(AOSTAG_OBJID), 
					AOSTAG_CREATOR, creator, "", "false", "true"))
				{
					OmnAlarm << "Failed to modify: " << creator << enderr;
					OmnScreen << doc->toString() << endl;
					continue;
				}
				OmnScreen << "Modified: " << docid << ":"
					<< orig_doc->getAttrStr(AOSTAG_CREATOR)
					<< ":" << creator << endl;
			}
			continue;
		}

		// Check whether the objid is in the form:
		// 		<name>.<cloudid>
		// If yes, and it is indeed a valid cloudid, use it as its creator.
		OmnString prefix, cid;
		AosObjid::decomposeObjid(objid, prefix, cid);
		if (cid != "")
		{
			// Check whether it is a valid cloudid
			if (isValidCloudid(siteid, ssid, urldocid, cid))
			{
				// It is a valid cloudid
				if (orig_doc->getAttrStr(AOSTAG_CREATOR) != creator)
				{
					// Need to modify the creator. 
					OmnScreen << "(2)The creator is not a valid cloudid: "
						<< creator << ". Do you want to change to unknown(1) "
						<< " or ignore it (2), use the creator (3)?" 
						<< " The original creator: " 
						<< orig_doc->getAttrStr(AOSTAG_CREATOR) << endl;
					char buff[100];
					cin >> buff;
					if (buff[0] == '1')
					{
						creator = AOSCLOUDID_UNKNOWN;
					}
					else if (buff[0] == '2')
					{
						continue;
					}

					if (!modifyAttr(siteid, ssid, urldocid, 
						doc->getAttrStr(AOSTAG_OBJID), 
						AOSTAG_CREATOR, creator, "", "false", "true"))
					{
						OmnAlarm << "Failed to modify: " << creator << enderr;
						OmnScreen << doc->toString() << endl;
						continue;
					}
					OmnScreen << "Modified: " << docid << ":"
						<< orig_doc->getAttrStr(AOSTAG_CREATOR)
						<< ":" << creator << endl;
				}
				continue;
			}
		}

		//if (creator == "" || creator == AOSCLOUDID_UNKNOWN || 
		//	creator == AOSCLOUDID_GUEST ||
		//	creator == AOSCLOUDID_SYSTEM)
		if (creator == "" || creator == AOSCLOUDID_UNKNOWN )
		{
			creator = doc->getAttrStr(AOSTAG_MODUSER);
		}

		//if (creator == "" || creator == AOSCLOUDID_UNKNOWN ||
		//	creator == AOSCLOUDID_GUEST)
		if (creator == "" || creator == AOSCLOUDID_UNKNOWN)
		{
			creator = doc->getAttrStr("zky_modby");
		}

		//if (creator == "" || creator == AOSCLOUDID_UNKNOWN ||
		//	creator == AOSCLOUDID_GUEST)
		if (creator == "" || creator == AOSCLOUDID_UNKNOWN)
		{
			continue;
		}

		// Check whether it is a docid
		cid = retrieveCloudid(siteid, ssid, urldocid, creator);
		if (cid != "")
		{
			// The creator is a docid. Need to modify it to cid.
			creator = cid;
		}

		if (creator == "96881" || creator == "34087")
		{
			OmnString oid;
			// These are damaged user accounts. Need to repair them.
			if (creator == "96881") oid = "zkyoidur_100046";
			else if (creator == "34087") oid = "zkyoidur_20080743";

			// Special case
			if (!modifyAttr(siteid, ssid, urldocid, oid, 
				AOSTAG_OTYPE, AOSOTYPE_USERACCT, "", "false", "true"))
			{
				OmnAlarm << "Failed to modify: " << creator << enderr;
				OmnScreen << doc->toString() << endl;
				continue;
			}
			cid = retrieveCloudid(siteid, ssid, urldocid, creator);
			if (cid != "") 
			{
				creator = cid;
			}
		}

		if (creator != "" && orig_doc->getAttrStr(AOSTAG_CREATOR) != creator)
		{
			u64 dd = atoll(creator.data());
			if (dd < 20000000)
			{
				if (!creator.isDigitStr() || !isValidCloudid(siteid, ssid, urldocid, creator))
				{
					OmnScreen << "(3)The creator is not a valid cloudid: "
						<< creator << ". Do you want to change to unknown(1) "
						<< " or ignore it (2), use the creator (3)?" 
						<< " The original creator: " 
						<< orig_doc->getAttrStr(AOSTAG_CREATOR) << endl;
					char buff[100];
					cin >> buff;
					if (buff[0] == '1')
					{
						creator = AOSCLOUDID_UNKNOWN;
					}
					else if (buff[0] == '2')
					{
						continue;
					}
				}
			}

			if (!modifyAttr(siteid, ssid, urldocid, 
				doc->getAttrStr(AOSTAG_OBJID), 
				AOSTAG_CREATOR, creator, "", "false", "true"))
			{
				OmnAlarm << "Failed to modify: " << creator << enderr;
				OmnScreen << doc->toString() << endl;
				continue;
			}
			OmnScreen << "Modified: " << docid << ":"
				<< orig_doc->getAttrStr(AOSTAG_CREATOR)
				<< ":" << creator << endl;
		}
	}

	return true;
}


OmnString
AosSengAdmin::retrieveCloudid(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &userid)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">getcid</item>"
		<< "<item name=\"args\">userid=" << userid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, "");
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), "");
	aos_assert_r(resp != "", "");

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, "");
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") return "";

	child = resproot->getNextChild();
	if (!child) return "";
	OmnString cid = child->getAttrStr(AOSTAG_CLOUDID);
	return cid;
}


bool
AosSengAdmin::modifyAttr(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &objid, 
		const OmnString &aname,
		const OmnString &value,
		const OmnString &dft,
		const OmnString &value_unique, 
		const OmnString &docid_unique)
{
	OmnString oid = objid;
	oid.replace("=", "#61", true);

	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">modattr</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"args\">objid=" << oid 
		<< ",attrname=" << aname 
		<< ",value_unique=" << value_unique
		<< ",docid_unique=" << docid_unique
		<< "</item>"
		<< "<item name=\"value\"><![CDATA[" << value<< "]]></item>"
		<< "<item name=\"default\"><![CDATA[" << dft<< "]]></item>";
		
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
			<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
			<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
			<< "</request>";
	}	

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") return false;
	return true;
}


bool
AosSengAdmin::isValidCloudid(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &cid)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">checkcid</item>"
		<< "<item name=\"args\">cloudid=" << cid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") return "";

	child = resproot->getNextChild();
	if (!child) return false;
	OmnString rslt = child->getAttrStr("rslt");
	return (rslt == "true");
}


bool
AosSengAdmin::readHeaderVersion201102(
		const u64 &docid,
		u32 &seqno, 
		u64 &offset, 
		u64 &docsize, 
		u64 &compressed_size,
		u32 &arcd_seqno,
		u64 &arcd_offset,
		u64 &arcd_docsize,
		AosSengAdmin::DocStatus &status, 
		const AosRundataPtr &rdata) 
{
	// This is the version that reads docs for 2010/12 releases. 
	// The header format is:
	// 		version		u32
	// 		siteid		u32
	// 		docid		u64
	// 		seqno		u32
	// 		offset		u64
	// 		docsize		u64
	const int maxHeaderPerFile = AosXmlDoc::getMaxHeaderPerFile();
	status = eBad;

	// Create the buff for header
	char bb[AosXmlDoc::eDocHeaderSize];

	seqno = docid / maxHeaderPerFile;
	offset = (docid % maxHeaderPerFile) * AosXmlDoc::eDocHeaderSize;

	int len;
	//AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrNew(docid, rdata);
	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
	//docfilemgr->readHeader(seqno, offset, bb, AosXmlDoc::eDocHeaderSize, len, sgRundata);
	AosBuff headerBuff(bb, len, len AosMemoryCheckerArgs);

	u32 version = headerBuff.getU32(AosXmlDoc::eInvVersion);
	u32 siteid  = headerBuff.getU32(AOS_INVSID);
	u64 did 	= headerBuff.getU64(AOS_INVDID);
	seqno		= headerBuff.getU32(eAosInvFseqno);
	offset		= headerBuff.getU64(eAosInvFoffset);
	docsize		= headerBuff.getU64(eAosInvDocSize);
	compressed_size = headerBuff.getU64(0);
	arcd_seqno  = headerBuff.getU32(eAosInvFseqno);
	arcd_offset = headerBuff.getU64(eAosInvDocSize);
	arcd_docsize = headerBuff.getU64(eAosInvDocSize);

	status = eGood;

	if (version == AosXmlDoc::eInvVersion)
	{
		//OmnAlarm << "Version invalid: " << docid << enderr;
		status = eBad;
	}

	if (siteid == AOS_INVSID)
	{
		//OmnAlarm << "Siteid invalid: " << docid << enderr;
		status = eBad;
	}

	if (did != docid)
	{
		//OmnAlarm << "Docid mismatch: " << did << ":" << docid << enderr;
		status = eBad;
	}

	if (seqno == eAosInvFseqno)
	{
		//OmnAlarm << "Invalid seqno: " << docid << enderr;
		status = eBad;
	}

	if (offset == eAosInvFoffset)
	{
		//OmnAlarm << "Invalid offset: " << docid << enderr;
		status = eBad;
	}

	if (docsize == eAosInvFoffset || docsize == 0)
	{
		//OmnAlarm << "Invalid docsize: " << docid << enderr;
		status = eBad;
	}

	//zky2782 Ketty 2011/02/16
	if (status == eBad) return true;
	status = eGood;
	return true;
}


bool
AosSengAdmin::deleteDocVersion201102(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	//const int maxHeaderPerFile = AosXmlDoc::getMaxHeaderPerFile();
	char bb[AosXmlDoc::eDocHeaderSize];
	//u32 header_seqno = docid / maxHeaderPerFile;
	//u64 header_offset = (docid % maxHeaderPerFile) * AosXmlDoc::eDocHeaderSize;

	int len;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrNew(docid, rdata);
	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
	//docfilemgr->readHeader(header_seqno, header_offset, bb, AosXmlDoc::eDocHeaderSize, len, sgRundata);
	AosBuff headerBuff(bb, len, len AosMemoryCheckerArgs);

	headerBuff.getU32(AosXmlDoc::eInvVersion);		// Version
	headerBuff.getU32(AOS_INVSID);					// Siteid
	headerBuff.getU64(AOS_INVDID);					// Docid
	headerBuff.getU32(eAosInvFseqno);				// Seqno
	headerBuff.getU64(eAosInvFoffset);				// Offset
	headerBuff.getU64(eAosInvDocSize);				// Docsize

	AosBuff buff(bb, AosXmlDoc::eDocHeaderSize, 0 AosMemoryCheckerArgs);
	buff.setU32(AosXmlDoc::eInvVersion);			// Version
	buff.setU32(AOS_INVSID);						// Siteid
	buff.setU64(AOS_INVDID);						// Docid
	buff.setU32(eAosInvFseqno);						// Seqno
	buff.setU64(eAosInvFoffset);					// Offset
	buff.setU64(0);									// Docsize

	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
	bool rslt ;//= docfilemgr->saveHeader(header_seqno, header_offset, buff.data(), buff.dataLen(), true, sgRundata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosSengAdmin::getLoginObj(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		AosXmlTagPtr &loginobj)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">resolve_url</item>"
		<< "<item name=\"loginobj\">true</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << sessionId << "</item>";
	req << "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	AosXmlTagPtr status = child->getFirstChild("status");
	aos_assert_r(status, false);
	loginobj = status->getFirstChild("zky_lgnobj");
	return true;
}


// Ketty 05/25/2011
bool
AosSengAdmin::checkCloudid(
		const u32 siteid,
		const OmnString &cloudid,
		bool	&exist) 
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">checkcid</item>"
		<< "<item name=\"args\">cloudid::" << cloudid << "</item>";
	req << "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	
	AosXmlTagPtr contents = child->getFirstChild("Contents");
	aos_assert_r(contents, false);
	
	OmnString rslt = contents->getAttrStr("rslt", "");
	if(rslt == "true")	exist = true;
	if(rslt == "false")	exist = false;
	return true;
}


// Tank 05/28/2011
bool
AosSengAdmin::manualOrder(
		const u32 siteid,
		const OmnString &reqid,
		const OmnString &args,
		OmnString &rslt) 
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">" << reqid << "</item>"
		<< "<item name=\"args\">" << args << "</item>";
	req << "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	bool exist;
OmnScreen << "++++++++++++++" << child->toString() << endl;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	if (!exist)
	{
		OmnMark;
	}
	
	AosXmlTagPtr contents = child->getFirstChild("Contents");
	aos_assert_r(contents, false);

	OmnScreen << "Contents:" << contents->toString() << endl;
	rslt = contents->toString();	
	return true;
}

bool
AosSengAdmin::convertSystemDocs(
		const u64 &startdocid,
		const u64 &enddocid,
		const char *fname,
		const u32 siteid, 
		const AosRundataPtr &rdata)
{
	u64 userid ;
	OmnString ssid ;
	AosXmlTagPtr userdoc ;
	AosXmlParser parser;
	AosXmlTagPtr doc ;
    DocStatus status;
	OmnConnBuffPtr docbuff;
	map<OmnString, OmnString>::iterator  iter ;
	u64 urldocid;
	login("yuhui", "12345",	"yunyuyan_account", ssid, urldocid,  
			userid, userdoc, siteid);

	if (!fname)
	{
		OmnAlarm << "Missing file name!" << enderr;
		return false;
	}

	OmnFile f(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!f.isGood())
	{
		OmnAlarm << "Failed to open file: " << fname << enderr;
	    return false;
	}

	OmnString contents;
	if (!f.readToString(contents))
	{
		OmnAlarm << "Failed to read file: " << fname << enderr;
		return false;
	}
	AosXmlTagPtr tag  = parser.parse(contents, "" AosMemoryCheckerArgs);
	if (!tag)
	{
		OmnAlarm << "parse file error!: " << fname << enderr;
		return false;
	}

	//the file should be :
	//<Contents>
	//    <record>
	//        <item>objid</item>
	//        <item>ctnr</item>
	//    </record>
	//    ......
	//</Contents>
    AosXmlTagPtr cont_tag = tag->getFirstChild();
	aos_assert_r(cont_tag,false);
	AosXmlTagPtr rec_tag = cont_tag->getFirstChild();
	aos_assert_r(rec_tag,false);
	AosXmlTagPtr objid_tag ;
	AosXmlTagPtr ctnr_tag ;

	//get all objid and ctnr into ctnr
	map<OmnString,OmnString>lCtnr;
	lCtnr.clear();
	while(rec_tag)
	{
        objid_tag = rec_tag->getFirstChild();
		ctnr_tag = rec_tag->getNextChild();
		if(objid_tag && ctnr_tag)
		{
		    OmnString objid = objid_tag->getNodeText();
			OmnString ctnr = ctnr_tag->getNodeText();
			lCtnr.insert(pair<OmnString, OmnString>(objid ,ctnr));
		}
		rec_tag = cont_tag->getNextChild();
	}
	
	//read doc form startdocid to enddocid 
	for(u64 docid=startdocid; docid<enddocid; docid++)
	{
	    if (!readDoc(docid, docbuff, false, status, rdata)) continue;
		if(!docbuff) continue;
		doc = parser.parse(docbuff,"" AosMemoryCheckerArgs);
		if(doc)
		{
		    OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
			if((iter = lCtnr.find(objid)) != lCtnr.end())
			{
	            doc->removeAttr(AOSTAG_DOCID, 1, true);
		        doc->setAttr("zky_public_doc","true");
		        doc->setAttr("zky_public_ctnr","true");
				doc->setAttr("zky_pctrs", iter->second);
			    bool rslt = sendModifyReq(siteid, ssid, urldocid, doc->toString(), rdata);
			    if(!rslt)
			    {
			        OmnAlarm << "Modify doc failed : " << objid << enderr ;
				    continue ;
			    }
			}
		}
	}
	return true ;
}


bool
AosSengAdmin::changeCreator(
		const u64 &start_docid, 
		const u64 &end_docid,
		const OmnString &value,
		const u32 siteid, 
		const AosRundataPtr &rdata)
{
	u64 userid ;
	OmnString ssid ;
	AosXmlTagPtr userdoc ;
	u64 urldocid;
	login("100018", "12345",	"yunyuyan_account", ssid, urldocid, 
			userid, userdoc, siteid);
	for (u64 docid=start_docid; docid<end_docid; docid++)
	{
		OmnConnBuffPtr buff;
		DocStatus status;
		if (readDoc(docid, buff, false, status, rdata))
		{
			AosXmlParser parser;
			AosXmlTagPtr doc = parser.parse(buff->getData(), "" AosMemoryCheckerArgs);
			if (doc && doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT) 
			{
				OmnString cid = doc->getAttrStr(AOSTAG_CLOUDID);
				if (cid == "")
				{
					OmnAlarm << "Cloud id is empty: " << doc->toString() << enderr;
				}
				else
				{
					if (doc->getAttrStr(AOSTAG_CREATOR) != cid)
					{
						doc->setAttr(AOSTAG_CREATOR, cid);
						if (!sendModifyReq(siteid, ssid,urldocid, doc->toString(), rdata))
						{
							OmnAlarm << "Failed modifying the doc: " 
								<< doc->toString() << enderr;
						}
					}
				}
			}
		}
	}
	return true;
}


AosXmlTagPtr 
AosSengAdmin::retrieveDocByCloudid(
		 const u32 siteid, 
		 const OmnString &cid, 
		 const u64 &urldocid) 
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString req = "<request >";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>"
		<< "<item name=\"reqid\">getuserdocbycloudid</item>"
		<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
		<< "<item name=\"cid\">" << cid << "</item>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	if (!mConn)
	{
		OmnAlarm << "Failed conn!" <<enderr;
		return 0;
	}
	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		OmnAlarm << "Failed conn!" <<enderr;
		return 0;
	}

	if (resp == "")
	{
		OmnAlarm << "Missing Resp!" <<enderr;
		return 0;
	}

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	if(!child)
	{
		OmnAlarm << "Missing Child Tag!" <<enderr;
		return 0;
	}
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") 
	{
		return 0;
	}

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	if(!child1)
	{
		OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}
	AosXmlTagPtr child2 = child1->getFirstChild();
	if(!child2)
	{
		OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


AosXmlTagPtr 
AosSengAdmin::createUserDomain(
		const AosXmlTagPtr &doc, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const u32 siteid, 
		const AosRundataPtr &rdata)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">createuserdomain</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< 	"<objdef>" << doc->toString() <<"</objdef>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;

	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = resproot->getFirstChild("Contents");
	aos_assert_r(con, 0);
	OmnString data = con->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}

bool
AosSengAdmin::checkLogin(
		const u32 siteid,
		const OmnString &cid,
		const OmnString &ssid,
		const u64 &urldocid)
{
	u32 trans_id = mTransId++;
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>" << siteid << "</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>checklogin</item>"
		<< "<item name='trans_id'>" << trans_id << "</item>"
		<< "<objdef>"
		<< "<Contents>"
		<< "<ssid>" << ssid << "</ssid>"
		<< "<cid>" << cid << "</cid>"
		<< "</Contents>"
		<< "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return exist;
}


/*
bool
AosSengAdmin::convertDatacolQuery(
		const AosXmlTagPtr &datacol_tag,
		const AosRundataPtr &rdata)
{
	// 'datacol_tag' is in the form:
	// 	<datacol 
	// 		cmp_query="the query"
	// 		cmp_order="fieldname"
	// 		cmp_order2="sort-name"
	// 		cmp_sortflag="sort-flag"
	// 		cmp_increase="increase"
	// 		cmp_fnames="field names"
	// 		cmp_joins="xxx"
	// 		cmp_psize="xxx"
	// 		cmp_reverse="xxx"/>
	// 'query' is a string of the following format:
	// 		condition|$$|condition|$$|...
	// where each 'condition' is in the folowing format:
	// 		xxx|$|xxx|$|xxx|$|xxx|$|xxx
	// Each field is interpreted depending on the values. 
	

	// 1. Add the conditions tag
	AosXmlTagPtr conds_tag = datacol_tag->addNode1("conds");
	aos_assert_r(conds_tag, false);
	AosXmlTagPtr and_tag = conds_tag->addNode1("cond");
	aos_assert_r(and_tag, false);
	and_tag->setAttr("type", "AND");

	bool hasConds = false;

	// 2. Convert 'query' into the new format 
	OmnString query = datacol_tag->getAttrStr("cmp_query");
	if (query != "")
	{
		OmnString conditions[30];
		bool finished;
		AosStrSplit split;
		int num_conds = split.splitStr(query.data(), "|$$|", conditions, 30, finished);
		aos_assert_rr(num_conds > 0, rdata, false);
		for (int i=0; i<num_conds; i++)
		{
			if (conditions[i] != "")
			{
				bool rslt = convertOneCond(conditions[i], and_tag, rdata);
				aos_assert_r(rslt, false);
			}
		}

		datacol_tag->removeAttr("cmp_query");
		hasConds = true;
	}

	// 3. Handle 'containers'
	// <cmd ...
	// 		ctnrs="xxx"
	// 		tname="xxx"
	
	OmnString ctnrs = datacol_tag->getAttrStr("cmp_tname");
	if (ctnrs != "")
	{
		bool rslt = AosTermContainer::convertToXml(and_tag, ctnrs, rdata);
		aos_assert_r(rslt, false);
		datacol_tag->removeAttr("cmp_tname");
		hasConds = true;
	}
	
	// 4. Handle 'tags'
	OmnString tags = datacol_tag->getAttrStr("cmp_tags");
    if(tags != "")
	{
		bool rslt = AosTermTag::convertToXml(and_tag, tags, rdata);
		aos_assert_r(rslt, false);
		datacol_tag->removeAttr("cmp_tags");
		hasConds = true;
	}
	// 5. Handle 'keywords'
	OmnString words = datacol_tag->getAttrStr("cmp_words");
	if(words != "")
	{
		bool rslt = AosTermKeywords::convertToXml(and_tag, words, rdata);
		aos_assert_r(rslt, false);
		datacol_tag->removeAttr("cmp_words");
		hasConds = true;
	}

	if (!hasConds)
	{
		OmnAlarm << "The query is empty!" << enderr;
		return false;
	}
	return true;
}
*/

// 1. If a query has mSubopr == "ver_deleted", it should convert to AosTermVersionDocs. 
// 2. If a query has mSubopr == "version", it should convert to AosTermVersionDocs.
// 3. If a query has mSubopr == "delobjs", it should convert to AosTermDeletedDocs.
// 4. Convert AOSQUERYTYPE_WHOREADME   
// 5. Convert AOSQUERYTYPE_FRIEND
// 6. Convert AOSQUERYTYPE_COMMENT
// 7. Convert AOSQUERYTYPE_MONITORLOG
//
// 8. "container|$|=|$|<container_name>" (this is for containers)
// 9. "zky_tag|$|*|$|ddd,ddd,..." (this is for tags)
// 10. <container>|$|lg|$|logtime|$|reverse|$|opr
//     This is for login logs:
// 11. <smartdoc_objid>|$|AOSQUERYTYPE_ALLOWANCE|$|*[|$|reverse]
//     This is for allowance
// 12. AOSQUERYTYPE_WHOREADME|$|=|$|objid|$|reverse|$|filter
//     This is for who-read-me
// 13. buddylist_type|$|AOSQUERYTYPE_FRIEND|$|*[|$|reverse]
//     This is for friends.
// 14. AOSQUERYTYPE_COMMENT|$|cmtd_docid|$|cmtd_objid|$|*|$|reverse
//     This is for comments
// 15. AOSQUERYTYPE_MONITORLOG|$|=|$|problem_docid|$|1
//     This is for monitor
// 16. AOSQUERY_USERCUSTOM|$|*|$|*|$|reverse|$|filter
//     This is for user custom
// 17. *|$|wd|$|keyword
//     This is for keywords
// 18. attrname|$|opr|$|value
//     where:
//     	opr: =, >, <, >=, <=, ==, !=, 
//     		01 (for objid)
//     		lk (for like)
//     		an (for any)
/*
bool
AosSengAdmin::convertOneCond(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const AosRundataPtr &rdata)
{
	// Each condition is in the following format:
	// 		xxx|$|xxx|$|xxx|$|xxx|$|xxx
	// Each field is interpreted depending on the values. 
	
	aos_assert_r(conds_tag, false);
	OmnString fields[5];

	AosStrSplit split;
	bool finished;
	int nn = split.splitStr(cond_str.data(), "|$|", fields, 5, finished);

	OmnString opr = AosOpr_toStrCode(fields[1]);
	if (fields[0] == "container")
	{
		bool rslt = AosTermContainer::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (fields[0] == AOSTAG_TAG)
	{
		bool rslt = AosTermTag::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (fields[1] == "lg")
	{
		//bool rslt = AosTermLog::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		//aos_assert_r(rslt, false);
		return true;
	}

	if (fields[1] == AOSQUERYTYPE_ALLOWANCE)
	{
		bool rslt = AosTermAllowance::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (fields[0] == AOSQUERYTYPE_WHOREADME)
    {
		bool rslt = AosTermAllowance::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (fields[1] == AOSQUERYTYPE_FRIEND)
    {
		bool rslt = AosTermFriend::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (fields[0] == AOSQUERYTYPE_COMMENT)
    {
		bool rslt = AosTermFriend::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (fields[0] == AOSQUERYTYPE_MONITORLOG)
	{
		bool rslt = AosTermMonitorLog::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (fields[0] == AOSQUERYTYPE_USERCUSTOM)
	{
		bool rslt = AosTermMonitorLog::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (fields[1] == "wd")
	{
		// setWords(fields[2]);
		// return true;
		bool rslt = AosTermKeywords::convertToXml(cond_str, conds_tag, fields, nn, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (nn < 3 || opr == "")
	{
		OmnAlarm << "Condition incorrect: " << cond_str << enderr;
		return false;
	}

	bool rslt = AosTermArith::convertToXml(cond_str, conds_tag, fields, nn, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosSengAdmin::convertQuery(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	OmnString type = term->getAttrStr("type");
	if (type == "") 
	{
		OmnAlarm << "Found a term that has no type: " << term->toString() << enderr;
		return false;
	}

	if (type == "lt" || 
		type == "le" ||
		type == "eq" ||
		type == "ne" ||
		type == "gt" ||
		type == "ge" ||
		type == "an" ||
		type == "lk")
	{
		bool rslt = AosTermArith::convertQuery(term, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (type == "rg")
	{
		bool rslt = AosTermInrange::convertQuery(term, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (type == "te")
	{
		bool rslt = AosTermDate::convertQuery(term, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (type == "wa")
	{
		bool rslt = AosTermKeywords::convertQuery(term, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (type == "wo")
	{
		OmnAlarm << "Not supported yet!" << enderr;
		// bool rslt = AosTermCtnWordOr::convertQuery(term, rdata);
		// aos_assert_r(rslt, false);
		return true;
	}

	if (type == "wd")
	{
		// Refer to AosTermAnd::procWordsForm1(...)
		bool rslt = AosTermKeywords::convertQuery(term, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (type == "tg")
	{
		bool rslt = AosTermTag::convertQuery(term, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (type == "ep")
	{
		// For AosTermAnd::parseEpochCond(...)
		bool rslt = AosTermArith::convertEpochCond(term, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (type == "lg")
	{
		//bool rslt = AosTermLoginLog::convertQuery(term, rdata);
		//aos_assert_r(rslt, false);
		bool rslt = false;//AosTermLoginLog::convertQuery(term, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (type == "nx")
	{
		bool rslt = AosTermNotExist::convertQuery(term, rdata);
		aos_assert_r(rslt, false);
		return true;
	}

	if (type == "container")
	{
		// Most likely this is a new term. Ignore it.
		return true;
	}

	if (type == "arith" || type == "container")
	{
		// Most likely this is a new term. Ignore it.
		// felicia, 2011/09/20, for var
		AosXmlTagPtr cond_tag = term->getFirstChild("cond");
		if (cond_tag)
		{
			AosXmlTagPtr parm_tag = cond_tag->getFirstChild();
			if (parm_tag)
			{
				term->setAttr("zky_ctobjid", "giccreators_querynew_term_var_h");
				cond_tag->setAttr("ctype", "var");
			}
		}

		return true;
	}

	OmnAlarm << "Unrecognized query type: " << type << enderr;
	return false;
}


bool
AosSengAdmin::convertVersionIILs()
{
	// In the current implementations, given a docid, its versions are stored
	// in:
	// 	AOSIILTAG_VERSION + docid
	//
	// In the new implementations, versions are stored in an IIL whose IILID
	// is stored in the doci's parent container's access record:
	// 		AOSTAG_VERSIONIILID
	// The IIL is for all the docs in a container. 
	//
	// It is a string IIL. Its values are in the form:
	// 		docid + "_" + version_number
	// The IIL should be sorted numerical alphabetic.
	OmnNotImplementedYet;
	return false;
}

bool
AosSengAdmin::convertFnames(
    const AosXmlTagPtr &datacol_tag,
    const AosRundataPtr &rdata)
{
	OmnString fnames = datacol_tag->getAttrStr("cmp_fnames");
	aos_assert_rr(fnames != "", rdata, false);
	OmnString pp[5];

	OmnString type;
	OmnString oname;
	OmnString cname;
	OmnString exist;
	OmnString join_idx;

	OmnString rslt = "<fnames>";
	AosStrSplit split(fnames, "|$$|");
	OmnString str;
	while(split.hasMore() && ((str = split.nextWord()) != ""))
	{
	    if (str != "")
	    {
	        AosStrSplit split1;
	        int nn = split1.splitStr(str, "|$|", pp, 5);
	        aos_assert_r(nn == 1 || nn >=3, false);
	        aos_assert_r(pp[2]!="", false);
            type = pp[2];
	        oname = pp[0];
	        cname = (nn >=3)?pp[1]:pp[0];
	        exist = pp[3];
	        join_idx = pp[4];
	        rslt << "<fname type=\"" << type << "\" exist=\"" << exist << "\""
				//Johzi, 2011/08/26
				 << " zky_ctobjid=\"giccreators_query_fname_h\"" 

			     << " join=\"" << join_idx << "\">";
	        rslt << "<oname><![CDATA[" << oname << "]]></oname>";
	        rslt << "<cname><![CDATA[" << cname << "]]></cname></fname>";
	    }
	}
	rslt << "</fnames>";
	AosXmlParser parser;
	AosXmlTagPtr fname_tag = parser.parse(rslt, "" AosMemoryCheckerArgs);
	aos_assert_r(fname_tag, false);
	fname_tag = datacol_tag->addNode(fname_tag);
	aos_assert_r(fname_tag, false);
	datacol_tag->removeAttr("cmp_fnames");
	return true;
}
*/


bool
AosSengAdmin::sendDocByContainerToRemServer(
				const OmnString &ctnr,
				const u64 &startdocid,
				const u64 & enddocid,
				const u32 siteid)
{
	// this function send all obj in a container
	// to remote server
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	u64 userid;
	OmnString ssid;
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	DocStatus status;
	AosXmlTagPtr userdoc;
	// This is user in zhiliantianxia.com
	u64 urldocid;
	login("hubin", "12345","yunyuyan_account", ssid, urldocid, userid, userdoc, siteid);

	for(u64 docid=startdocid;docid<enddocid;docid++)
	{
		if (!readDoc(docid, docbuff, false, status, rdata)) continue;
		if(!docbuff) continue;
		AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
		if(doc)
		{
			OmnString container = doc->getAttrStr("zky_pctrs");
			if(container == ctnr)
			{
OmnScreen << "Read doc: " << docid << endl;
				doc->removeAttr(AOSTAG_DOCID, 1, true);
				doc->setAttr("zky_public_ctnr", "true");
				doc->setAttr("zky_public_doc", "true");
				bool rslt = sendModifyReq(siteid, ssid, urldocid, doc->toString(), rdata, false);
				if(!rslt)
				{
					OmnAlarm << " Failed : " << doc->getAttrStr("zky_objid") << enderr;
					continue;
				}
			}
		}
	}
	return true;
}

bool 
AosSengAdmin::getLanguageType(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const OmnString &sendltype, 
		OmnString &receiveltype)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[100]]></item>
	//	    <item name='operation'><![CDATA[serverreq]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='reqid'><![CDATA[get_language]]></item>
	// 		<item name='args'><![CDATA[languagetype="Chinese" ]]></item>
	//	</request>
	
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid<< "</item>"
	req	<< "<item name=\"reqid\">get_language</item>";
			
	if(sendltype == "")
	{
		req << "</request>";
	}
	else
	{
		req	<< "<item name=\"args\"><![CDATA[languagetype=" << sendltype << "]]></item>"
			<< "</request>";
	}
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg),false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	// The response should be in the following format:
	// 	<Contents>
	// 		<language>Chinese</language>
	//	</Contents>
	receiveltype = root->getNodeText("language");
	return true;
}

void
AosSengAdmin::convertCtnrNamesInit()
{
	// Remove the siteid from following container names:
	
	sgCidCtnr = AosObjid::composeCidCtnrObjid(mSiteid);
	sgTempLostFoundLogin = AosObjid::composeLostFoundCtnrObjid(mSiteid);
	sgTempLogCtnrLogin = AosObjid::composeLoginLogCtnrObjid(mSiteid);
	sgTempLogCtnrInvRead = AosObjid::composeInvReadLogCtnr(mSiteid);
	sgTempRootUser = AosObjid::composeSuperUserObjid(mSiteid);
	sgTempLogCtnr = AosObjid::composeLogCtnr(mSiteid);
	// sgTempSysUserCtnr = AosObjid::getSysUserCtnrObjid(mSiteid);
	sgTempRootCtnr = AosObjid::getRootCtnrObjid(mSiteid);
	sgTempRoot = AosObjid::composeRootAcctObjid(mSiteid);
	// sgTempSysCtnrObjidPrefix = AosObjid::composeSysCtnrObjid(mSiteid);
	sgTempSysRoot = AosObjid::composeSysRootAcctObjid(mSiteid);
	sgTempGuest = AosObjid::composeGuestObjid(mSiteid);
	sgTempPubNameSpace = AosObjid::composePubNamespaceObjid(mSiteid);
	sgTempUserDomainCtnr = AosObjid::composeUserDomainCtnrObjid(mSiteid);
	// sgTempUserObjidPrefix = AosObjid::composeUserCtnrObjid(mSiteid);
}


bool
AosSengAdmin::convertCtnrNames(const AosXmlTagPtr &doc)
{
	OmnString ctnr_objid = doc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(ctnr_objid != "", false);

	if (ctnr_objid == sgCidCtnr)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeCidCtnrObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempLostFoundLogin)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeLostFoundCtnrObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempLogCtnrLogin)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeLoginLogCtnrObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempLogCtnrInvRead)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeInvReadLogCtnr(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempRootUser)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeSuperUserObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempLogCtnr)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeLogCtnr(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempSysUserCtnr)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::getSysUserCtnrObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempRootCtnr)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::getRootCtnrObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempRoot)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeRootAcctObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempSysRoot)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeSysRootAcctObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempGuest)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeGuestObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempPubNameSpace)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composePubNamespaceObjid(mSiteid));
		return true;
	}

	if (ctnr_objid == sgTempUserDomainCtnr)
	{
		doc->setAttr(AOSTAG_PARENTC, AosObjid::composeUserDomainCtnrObjid(mSiteid));
		return true;
	}

	// Split the string by "_". If it has three parts, check whether
	// the third part is otype. If yes, check the first two parts. 
	OmnString parts[3];
	AosStrSplit split;
	int nn = split.splitStr(ctnr_objid, "_", parts, 3);
	if (nn != 3) return false;
	if (parts[3] != doc->getAttrStr(AOSTAG_OTYPE)) return false;
	if (parts[1] != "100") return false;

	OmnString prefix = parts[0];
	prefix << "_" << parts[1];
	
	if (prefix == sgTempSysCtnrObjidPrefix || prefix == sgTempUserObjidPrefix)
	{
		// It should be changed to:
		// 	parts[0] + "_" + otype;
		ctnr_objid = parts[0];
		ctnr_objid << "_" << parts[2];
		doc->setAttr(AOSTAG_PARENTC, ctnr_objid);
		return true;
	}

	return false;
}

bool
AosSengAdmin::sendShortMsg(
		const OmnString &msg,
		const OmnString &receiver,
	    const u32 siteid)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
	    << "<item name=\"reqid\">sdshm</item>"
		<< "<item name=\"needresp\">noresp</item>"
		<< "<objdef>"
		<< "<shm " << "shm_receivers=\"" << receiver << "\" >"
		<< "<contents>" << msg << "</contents>"
		<< "</shm>"
		<< "</objdef>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(resproot, false);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	return true;
}


bool
AosSengAdmin::addReq(
		const ReqType type,
		const u32 siteid, 
		const AosXmlTagPtr &doc)
{
	mLock->lock();
	ReqStruct req;
	req.mType = type;
	req.mSiteid = siteid;
	req.mDoc = doc;
	if (type == eCreateDoc)
	{
		mRequests.push(req);
	}
	else if (type == eCreateArcd)
	{
		mRequests1.push(req);
	}
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosSengAdmin::scanParents(
		const u64 &startDocid,
		const u64 &maxDocid, 
		const u32 siteid)
{

	// Login to remote server
	aos_assert_r(siteid != 0, false);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(siteid);

	OmnString ssid;
	u64 urldocid = 0;
	AosXmlTagPtr userdoc;

	OmnConnBuffPtr docbuff;
	OmnString oid;
	CheckRslts rslts;
	DocStatus status;
	AosXmlParser parser;
	AosXmlTagPtr doc;
	bool delFlag;
	u64 docid;
	for (docid=startDocid; docid<=maxDocid; docid++)
	{
		delFlag = false;
		OmnScreen << "Read doc: " << docid << endl;
		readDoc(docid, docbuff, false, status, rdata);
		switch (status)
		{
		case eInvalid:
			 rslts.num_invalid++;
			 break;

		case eDeleted:
			 rslts.num_deleted++;
			 break;

		case eBad:
			 rslts.num_bad++;
			 break;

		case eGood:
			 checkParent(siteid, docid, docbuff, delFlag, rslts, ssid, urldocid);
			 break;

		default:
			 OmnAlarm << "Unrecognized status: " << status << enderr;
			 rslts.num_errors++;
			 break;
		}
	}

	return true;
}


bool
AosSengAdmin::checkParent(
		const u32 siteid,
		const u64 &docid,
		const OmnConnBuffPtr &docbuff, 
		bool &delFlag,
		AosSengAdmin::CheckRslts &rslts,
		const OmnString &ssid,
		const u64 &urldocid)
{
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docbuff, "" AosMemoryCheckerArgs);
	if (!doc)
	{
		OmnAlarm << "Failed parse doc: " << docid << enderr;
		return false;
	}

	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	if (doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER)
	{
		aos_assert_r(rslts.objidExists(objid), false);
		rslts.objids[objid] = doc->getAttrU64(AOSTAG_DOCID, 0);
	}

	return true;
}


AosDocFileMgrObjPtr
AosSengAdmin::getDocFileMgr(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	//int vid = 0;
	// Ketty 2012/07/30
	//if (!sgDocStorageApp) sgDocStorageApp = OmnNew AosVirtualFileSys(sgAppFileKey, AOSPASSWD_SYSTEM_CALL, sgRundata);
	// if (!sgDocDevLocation) sgDocDevLocation = OmnNew AosDevLocation1();
	aos_assert_r(sgRundata, 0);

	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);
	// Ketty 2012/07/30
	//AosDocFileMgrObjPtr docfilemgr = sgDocStorageApp->retrieveDocFileMgr(
	//		vid, siteid, sgRundata);
	// Chen Ding, 2013/02/11
	// AosStorageMgrObjPtr storagemgr = AosStorageMgrObj::getStorageMgr();
	AosVfsMgrObjPtr storagemgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_rr(storagemgr, rdata, 0);
	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
	AosDocFileMgrObjPtr docfilemgr;// = 
	//	storagemgr->retrieveDocFileMgr(vid, AosTransModule::eDoc, rdata);
	aos_assert_r(docfilemgr, 0);
	return docfilemgr;
}


bool
AosSengAdmin::modifyDocTest(
		const int &tries,
		const OmnString &objid,
		const u32 siteid)
{
	// This funciton modify the doc named objid.
	// It tested for checking the version, when the 
	// nuber of version is more than the configuration 
	// of the doc, it will delete the version.
	aos_assert_r(objid != "", false);
	aos_assert_r(siteid != 0, false);
	
	u64 userid;
	OmnString ssid;
	AosXmlTagPtr userdoc;
	u64 urldocid;
	bool rslt = login("linhaiming", "12345", 
			"yunyuyan_account", ssid, urldocid, userid, userdoc, siteid, "");
	aos_assert_r(rslt, false);

	AosRundataPtr rdata = OmnApp::getRundata();
	for (int i=0; i<tries; i++)
	{
		OmnString resp;
		AosXmlTagPtr doc = retrieveDocByObjid(siteid, ssid, urldocid, objid, resp, true); 
		aos_assert_r(doc, false);
		doc->setAttr("zky_tries", i);
		aos_assert_r(sendModifyReq(siteid, ssid, urldocid, 
					doc->toString(), rdata, true), false);
	}
	return true;
}


AosXmlTagPtr
AosSengAdmin::sendDocLockReq(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const OmnString &docstr)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<command><cmd opr='doclock' res_objid='true'/></command>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<objdef>" << docstr << "</objdef>";

	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
			<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
			<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
			<< "</request>";
	}
			
	OmnString errmsg;
	OmnString resp;
	if (!mConn)
	{
		OmnAlarm << "No connection to server!" << enderr;;
		return 0;
	}

	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		OmnAlarm <<"Failed processing the request. Errmsg: " << enderr;
		return 0;
	}

	if (resp == "")
	{
		OmnAlarm  << "Failed receiving response!" << enderr;
		return 0;
	}

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr contents = resproot->getFirstChild("Contents");
	aos_assert_r(contents, 0);
	AosXmlTagPtr record = contents->getFirstChild();
	aos_assert_r(record, 0);
	OmnString data = record->toString();
	contents = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(contents, 0);
	return contents;
}
bool
AosSengAdmin::sendDeleteReq(
			const u32 &siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &docid)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">delObject</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req << "<item name=\"zky_docid\">" << docid <<"</item>"
	<< "</request>";
	
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if(child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	return true;
}

AosXmlTagPtr
AosSengAdmin::queryDocsByAttrs(
		const u32 siteid,
		const int &numconds, 
		const AosSengAdmin::QueryType *types,
		const OmnString *attrs, 
		const AosOpr *oprs,
		const OmnString *values,
		const bool *orders,
		const bool *reverses,
		const int psize,
		const bool reverse,
		const int startidx,
		const int queryid)
{
	aos_assert_r(numconds>0 && numconds <= eMaxQueryCond,0);
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name=\"" << AOSTAG_SITEID << "\">" 
		<< "<![CDATA["<< siteid <<"]]>"<< "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<command>"
		<< "<cmd "
		<< "psize='" << psize << "' ";
	 
	if(reverse)
	{
		req << "reverse='false' ";
	}
	else
	{
		req << "reverse='true' ";
	}
	if(queryid)
	{
		req << "queryid='" << queryid << "' ";
	}
	req << "sortflag='false' increase='false' opr='retlist' ";
	req << "start_idx='" << startidx << "'> ";
	
	req << "<conds><cond type=\"AND\">";
	for(int i= 0;i < numconds;i++)
	{
		switch(types[i])
		{
			case eQueryHit:
				req << "<term type=\"wordand\" reserse =\"";
				if(reverses[i]){
					req << "true";
				}else{
					req << "false";
				}
				req << "\" order=\"";
				if(orders[i]){
					req << "true";
				}else{
					req << "false";
				}
//				req << "\" zky_ctobjid=\"giccreators_querynew_term_h\">";
//				req << "<selector  aname=\"" << attrs[i] << "\"/>";
//				req << "<cond type=\"arith\" zky_opr=\"eq\"><![CDATA[";
//				req << attrs[i] << "]]>";
//				req << "</cond></term>";

				req << "\" >";
				req << "<selector type=\"keywords\" aname=\"" << attrs[i] << "\"/>";
				req << "<cond type=\"arith\" zky_opr=\"eq\"><![CDATA[";
				req << attrs[i] << "]]>";
				req << "</cond></term>";
				break;
			case eQueryStr:
			case eQueryU64:
				req << "<term type=\"arith\" reserse =\"";
				if(reverses[i]){
					req << "true";
				}else{
					req << "false";
				}
				req << "\" order=\"";
				if(orders[i]){
					req << "true";
				}else{
					req << "false";
				}
				req << "\" zky_ctobjid=\"giccreators_querynew_term_h\">";
				req << "<selector type=\"attr\" aname=\"";
				req << attrs[i];
				req << "\"/>";
				req << "<cond type=\"arith\" zky_opr=\"";
				req << AosOpr_toStr(oprs[i]);
				req << "\"><![CDATA[";
				req << values[i] << "]]>";
				req << "</cond></term>";
				break;
			default:
				aos_assert_r(0,0);
		}
	}
	req << "</cond></conds>";
//	req << "<fnames>"
//	req << "	<fname type="1" zky_ctobjid="giccreators_query_fname_h">
//	req << "		<oname><![CDATA[realname]]></oname>
//	req << "		<cname><![CDATA[realname]]></cname>
//	req << "	</fname>
//	req << "
//	req << "	<fname type="1" zky_ctobjid="giccreators_query_fname_h">
//	req << "		<oname><![CDATA[stime]]></oname>
//	req << "		<cname><![CDATA[stime]]></cname>
//	req << "	</fname>
//	req << "	
//	req << "	<fname type="1" zky_ctobjid="giccreators_query_fname_h">
//	req << "		<oname><![CDATA[checked]]></oname>
//	req << "		<cname><![CDATA[checked]]></cname>
//	req << "	</fname>
//	req << "</fnames>
	req << "<fnames>"
		<< "<fname type=\"1\" zky_ctobjid=\"zky_docid\">"
		<< "<oname><![CDATA[realname]]></oname>"
		<< "<cname><![CDATA[realname]]></cname>"
		<< "</fname>"
		<< "</fnames>";

	req << "</cmd>";
	req << "</command>" << "</request>";
	
	//send request
	OmnString errmsg;
	OmnString resp;


	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	//receive response
	AosXmlParser parser;
	AosXmlTagPtr xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	return xmlroot;
}		 


AosXmlTagPtr
AosSengAdmin::readDoc(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid>0, 0);

	// Ketty 2012/01/22 temp
	OmnNotImplementedYet;
	u64 id;// = AosXmlDoc::getOwnDocid(docid);

	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrNew(id, rdata);
	//u64 did = parseDocid(rdata, docid);
	if (!docfilemgr)
	{
   		rdata->setError() << "File does not exist!";
    	return 0;
	}
	
	// Ketty 2012/01/22 temp
	AosXmlTagPtr doc;// = AosXmlDoc::readFromFile(did, docfilemgr, rdata AosMemoryCheckerArgs);
	if (doc) 
	{
   		// aos_assert_rr(docid == doc->getAttrU64(AOSTAG_DOCID, 0), rdata, 0);
    	if (docid != doc->getAttrU64(AOSTAG_DOCID, 0))
    	{
       		OmnAlarm << "Docid mismatch: " << docid << ":"
           		<< doc->toString() << enderr;
        	return 0;
    	}
		return doc;
	}
	return 0;
}


AosDocFileMgrObjPtr
AosSengAdmin::getDocFileMgrNew(
        const u64 &docid,
        const AosRundataPtr &rdata)
{

    // Chen Ding, 03/24/2012
	int vid;
     if (sgNumVirtuals > 0) vid = docid % sgNumVirtuals;
    //int vid = AosGetVirtualIDByDocid(docid);
    return getDocFileMgrByVid(vid, rdata);
}



AosDocFileMgrObjPtr                                                               
AosSengAdmin::getDocFileMgrByVid(const int vid, const AosRundataPtr &rdata)
{
	// Ketty 2012/07/30
	//if (!sgDocStorageApp)
	//{
	//	sgDocStorageApp = AosVirtualFileSys::getDocServerVirtualFileSys(
	//	        AOSPASSWD_SYSTEM_CALL, rdata);
	//}
	// if (!sgDocDevLocation)
	// {
	// 	sgDocDevLocation = OmnNew AosDevLocation1();
	// }
    //AosDocFileMgrObjPtr docfilemgr = sgDocStorageApp->retrieveDocFileMgr(
    //        vid, siteid, rdata);
	// Chen Ding, 2013/02/11
	// AosStorageMgrObjPtr storagemgr = AosStorageMgrObj::getStorageMgr();
	AosVfsMgrObjPtr storagemgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_rr(storagemgr, rdata, 0);
	// Ketty 2013/01/22
	OmnNotImplementedYet;
	return 0;
    AosDocFileMgrObjPtr docfilemgr;// = storagemgr->retrieveDocFileMgr(
    //        vid, AosTransModule::eDoc, rdata);
    if (docfilemgr)
    {
        OmnString prefix = "doc_";
        prefix << vid;
        //docfilemgr->setFileNamePrefix(prefix);
        return docfilemgr;
    }
	else
	{
		OmnAlarm << "No File" << enderr;
		return 0;
	}

	/*	
	OmnString prefix = "doc_";
    prefix << vid;
    docfilemgr = sgDocVirtualFileSys->createDocFileMgr(
                vid, siteid, prefix, sgDocDevLocation, rdata);
    if (!docfilemgr)
    {
        OmnAlarm << "Failed creating docfilemgr. This is a serious problem!"
           << enderr;
        return 0;
    }
*/
  
    return docfilemgr;
}


u64
AosSengAdmin::parseDocid(
        const AosRundataPtr &rdata,
        const u64 &docid)
{
    //mVirtualId = docid % mNumVirtuals;
    //localid = (docid - mVirtualId) / mNumVirtuals;
    
    if (sgNumVirtuals == 0) return docid;

    //1. process access docid
    u64 id = docid;
	AosDocType::E type = AosDocType::getDocType(docid);
    if (type != AosDocType::eNormalDoc)
    {
		// Ketty 2012/01/22 temp
		OmnNotImplementedYet;
        //id = AosXmlDoc::getOwnDocid(docid);
    }

    //2. process system  docid
    if (id <= AOSDOCID_MAX_SYS_DOCID) return docid;

    u64 did  = id /sgNumVirtuals;
    if (type != AosDocType::eNormalDoc)
    {
        //did = AosXmlUtil::setDocidType(type, did);
        AosXmlUtil::setDocidType(type, did);
    }
    return did;
}
