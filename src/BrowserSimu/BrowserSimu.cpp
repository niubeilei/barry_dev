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
#include "BrowserSimu/BrowserSimu.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Sleep.h"
#include "SearchEngine/SeError.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/SeTypes.h"
#include "LogServer/LogMgr.h"
#include "SEModules/VersionMgr.h"
#include "SEUtil/XmlDoc.h"
#include "SEUtil/XmlTag.h"
#include "WordMgr/WordMgr1.h"
#include "SearchEngine/DocServer.h"
#include "SEClient/SqlClient.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/StrParser.h"
#include "Util/Buff.h"
#include "Util/Opr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "Util/File.h"

OmnSingletonImpl(AosBrowserSimuSingleton,
		AosBrowserSimu,
		AosBrowserSimuSelf,
		"AosBrowserSimu");
static OmnMutex sgLock;

static OmnString sguri = "http://192.168.99.97:8080/lps-4.7.2/dev/jsp/server_proxy.jsp";
static OmnString sgmethod = "POST";
static OmnString sgcookies = "";


static OmnString sgImagePath;
const int sgGroupSize = 1000;
const int sgSleepLength = 5;

AosBrowserSimu::AosBrowserSimu()
:
mLock(OmnNew OmnMutex()),
mRemoteAddr("192.168.99.97"),
mRemotePort(8080),
mTransId(eInitTransId),
mNumConns(0)
{
}


AosBrowserSimu::~AosBrowserSimu()
{
}


bool      	
AosBrowserSimu::start()
{
	return true;
}


bool        
AosBrowserSimu::stop()
{
	return true;
}


OmnRslt     
AosBrowserSimu::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool
AosBrowserSimu::start(const AosXmlTagPtr &config)
{
	mRemoteAddr = config->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	mRemotePort = config->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
	mConn = OmnNew AosSqlClient(mRemoteAddr, mRemotePort);

	//u32 maxdocfiles = config->getAttrU64(AOSCONFIG_XMLDOC_MAX_DOCFILES, 0);
	//u32 maxheaderfiles = config->getAttrU64(AOSCONFIG_XMLDOC_MAX_HDRFILES, 0);

	/*AosXmlTagPtr def = config->getFirstChild("convert_info");
	if (def)
	{
		mDocFname = def->getAttrStr(AOSCONFIG_DOC_FILENAME);
		mIdxFname = def->getAttrStr(AOSCONFIG_IDX_FILENAME);
		mDirname = def->getAttrStr(AOSCONFIG_DIRNAME);
		mTargetDirname = def->getAttrStr(AOSCONFIG_TARGETDIRNAME);
	}
	
	mXmlDocReader = OmnNew AosDocFileMgr(maxdocfiles, 
		maxheaderfiles, mDirname, mDocFname);*/
	return true;
}


bool    
AosBrowserSimu::signal(const int threadLogicId)
{
	return true;
}


bool    
AosBrowserSimu::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosBrowserSimu::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(1000);
	}

	return true;
}


bool
AosBrowserSimu::rebuildVersion(const u32 seqno, OmnFile &file)
{
	// Versions are stored in files "version_xxx", where 
	// "xxx" are a seqno. Version files contain sequence of 
	// records. Each record is in the format:
	// 	Byte 0-3:	Doc size
	// 	Byte 4-: 	The doc itself
	// The first four bytes are encoded directly as binary.
	u32 offset = AosDocFileMgr::getReservedSize();
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
		if (docsize < 0) break;
		char *buff = OmnNew char[docsize+10];
		int bytesread = file.readToBuff(offset + 4, docsize, buff);
		aos_assert_r(bytesread == docsize, false);
		buff[docsize] = 0;
		AosXmlTagPtr doc = parser.parse(buff, "");
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
					args << docid << ":" << version << ":" << seqno << ":" << offset;
					bool rslt = sendServerReq(mSiteid, "rebuildver", args, NULL);
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
AosBrowserSimu::rebuildVersions(const u32 max_seqno)
{
	for (u32 i=0; i<max_seqno; i++)
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
		rebuildVersion(i, file);
	}

	return true;
}


bool
AosBrowserSimu::addOne(
		const u64 &docid,
		const OmnString &siteid,
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
	OmnConnBuffPtr docbuff;
	mIdxFname = idxFname;
	mDocFname = docFname;
	AosXmlParser parser;
	mSiteid = siteid;

	OmnScreen << "Read doc: " << docid << endl;
	if (!readDoc(docid, docbuff)) return false;
	AosXmlTagPtr doc = parser.parse(docbuff, "");
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

//	rebuildVersions(eMaxVersionSeqno);
//	rebuildLogs(eMaxVersionSeqno);
	return true;
}

bool
AosBrowserSimu::rebuildDb(
		const u64 &maxDocid,
		const OmnString &siteid)
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
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	mSiteid = siteid;
	int group = 0;
	for (u64 docid=1; docid<maxDocid; docid++)
	{
		if (group >= sgGroupSize)
		{
			if (docid > 5000)
			{
				OmnSleep(sgSleepLength);
			}
			group = 0;
		}

		OmnScreen << "Read doc: " << docid << endl;
		if (!readDoc(docid, docbuff)) continue;
		AosXmlTagPtr doc = parser.parse(docbuff, "");
		if (!doc)
		{
			OmnAlarm << "Failed to parse the doc: " 
				<< docid << ":" << docbuff->getData() << enderr;
			continue;
		}
		
		if (doc->getAttrStr(AOSTAG_OBJID) == "")
		{
			OmnAlarm << "Doc missing objid: " << doc->toString() << enderr;
			continue;
		}

		if (doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == AOS_INVDID)
		{
			OmnAlarm << "Doc missing docid: " << doc->toString() << enderr;
			continue;
		}

		// Normalize objid
		doc->normalizeObjid();

		if (doc->getAttrStr(AOSTAG_OTYPE) == "")
		{
			if (doc->toString() == "")
			{
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
//				OmnAlarm << "Doc missing otype. Set to unknown: " 
//					<< doc->toString() << enderr;
			}

			doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_UNKNOWN);
		}

		convertDataForRebuild(doc);

		// Chen Ding, 10/11/2010
		// For all docs whose otype is 'zky_ssion', we will not convert it anymore.
		// In the future, there shall be no such docs.
		if (doc->getAttrStr(AOSTAG_OTYPE) == "zky_ssion")
		{
			OmnScreen << "Session Doc found: " << doc->toString() << endl;
		}
		else
		{
			sendCreateObjReq(siteid, (char *)doc->getData());
			group ++;
		}
	}

	rebuildVersions(eMaxVersionSeqno);
	rebuildLogs(eMaxVersionSeqno);


	//Copy version and log files
	OmnString cp =  "cp";
	cp = cp << " " << mDirname <<"/version_*" << " " << mTargetDirname <<"/" ;
	system(cp);
	cp =  "cp";
	cp = cp << " "<< mDirname <<"/zykielog_*" << " " << mTargetDirname <<"/" ;
	system(cp);

	//bool rs = checkRebuild(siteid, 0, maxDocidd);
	//aos_assert_r(rs, false);
	return true;
}


bool
AosBrowserSimu::sendCreateObjReq(
		const OmnString &siteid, 
		const char *docstr)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << docstr << "</objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	OmnString docid = child->xpathQuery("Contents/zky_docid", exist, "");
	aos_assert_r(docid != "", false);
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosBrowserSimu::sendModifyReq(
		const OmnString &siteid, 
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
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	// OmnString docid = child->xpathQuery("Contents/docid", exist, "");
	// aos_assert_r(docid != "", false);
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosBrowserSimu::rereadDocs(
		const OmnString &siteid, 
		const u64 &max_docid,
		const OmnString &idxfname, 
		const OmnString &docfname)
{
	// This function re-reads all the docs and sends a modify request
	// to the server. This can be useful if the stored doc has some
	// syntax errors that can be fixed by the xml parser. 
	mIdxFname = idxfname;
	mDocFname = docfname;
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	AosXmlTagPtr doc;
	for (u64 docid=1001; docid<max_docid; docid++)
	{
		OmnScreen << "Read doc: " << docid << endl;
		if (!readDoc(docid, docbuff)) continue;
		doc = parser.parse(docbuff->getData(), "");
		if (!doc) continue;
		sendModifyReq(siteid, (char *)doc->getData());
	}
	return true;
} 


bool
AosBrowserSimu::readDoc(
		const u64 &docid,
		OmnConnBuffPtr &docbuff, 
		const bool readDeleted)
{
	// int maxHeaderPerFile = AosXmlDoc::getMaxHeaderPerFile();
	int maxHeaderPerFile = 100000000/296;

	// Create the buff for header
	char bb[AosXmlDoc::eDocHeaderSize];

	// OmnScreen << "Read doc: " << docid << endl;
	u32 seqno = docid / maxHeaderPerFile;
	u32 offset = (docid % maxHeaderPerFile) * AosXmlDoc::eDocHeaderSize;

	OmnFilePtr idxfile = mIdxFiles[seqno];
	if (!idxfile) idxfile = openIdxFile(seqno);
	aos_assert_r(idxfile, false);

	int len = idxfile->readBinaryInt(offset, -1);
	if (len <= 0 || (u32)len > AosXmlDoc::eDocHeaderSize)
	{
		return false;
	}

	len = idxfile->readToBuff(offset+4, len, bb);
	aos_assert_r(len >= 0, false);
	AosBuff headerBuff(bb, len, 0);
		
	headerBuff.getU32(AosXmlDoc::eInvVersion);			// version
	headerBuff.getU32(AOS_INVSID);						// siteid
	headerBuff.getU64(AOS_INVDID);						// docid
	u32 docseqno = headerBuff.getU32(eAosInvFseqno);	// doc seqno
	u64 docoffset = headerBuff.getU64(eAosInvFoffset);	// doc offset
	u64 docsize = headerBuff.getU64(eAosInvFoffset);	// docsize

	char status = headerBuff.getChar(eAosIndStatus);
	if (status == AosXmlDoc::eDocStatus_Delete && !readDeleted)
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

	/*
	OmnFilePtr docfile = mDocFiles[docseqno];
	if (!docfile) docfile = openDocFile(docseqno);
	aos_assert_r(docfile, false);

	len = docfile->readBinaryInt(docoffset, -2);
	if (len == -1)
	{
		OmnScreen << "Record was deleted: " << docseqno << ":"
			<< docoffset << ":" << docid << endl;
		return false;
	}

	if (len < -1)
	{
		OmnAlarm << "Read doc failed: " << docseqno << ":"
			<< docoffset << ":" << len << ":" << docsize << ":" << docid << enderr;
		return false;
	}
	*/

	// Ready to read the doc itself
	docbuff = OmnNew OmnConnBuff(docsize);
	aos_assert_r(docbuff->determineMemory(docsize), 0);
	char *data = docbuff->getBuffer();
	bool rslt = mXmlDocReader->readDoc(docseqno, docoffset, data, docsize);
	aos_assert_r(rslt, false);
	docbuff->setDataLength(docsize);
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
AosBrowserSimu::openIdxFile(const u32 seqno)
{
	aos_assert_r(seqno < eMaxIdxFiles, 0);
	if (mIdxFiles[seqno]) return mIdxFiles[seqno];

	OmnString idxfn = mDirname;
	idxfn << "/" << mIdxFname << "_" << seqno;
	OmnFilePtr ff = OmnNew OmnFile(idxfn, OmnFile::eReadOnly);
	aos_assert_r(ff && ff->isGood(), 0);
	mIdxFiles[seqno] = ff;
	return ff;
}


OmnFilePtr
AosBrowserSimu::openDocFile(const u32 seqno)
{
	aos_assert_r(seqno < eMaxDocFiles, 0);
	if (mDocFiles[seqno]) return mDocFiles[seqno];

	OmnString docfn = mDirname;
	docfn << "/" << mDocFname << "_" << seqno;
	OmnFilePtr ff = OmnNew OmnFile(docfn, OmnFile::eReadOnly);
	aos_assert_r(ff && ff->isGood(), 0);
	mDocFiles[seqno] = ff;
	return ff;
}



AosXmlTagPtr
AosBrowserSimu::readDocByDocid(
		const u64 &docid, 
		const OmnString &siteid, 
		const OmnString &idxfname, 
		const OmnString &docfname)
{
	mIdxFname = idxfname;
	mDocFname = docfname;

	OmnConnBuffPtr buff;
	if (!readDoc(docid, buff)) return 0;
	OmnScreen << "doc read: " << buff->getData() << endl;
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(buff->getData(), "");
	if (!doc) return 0;
	return doc;
}


bool
AosBrowserSimu::rebuildDoc(
		const u64 &docid,
		const OmnString &siteid,
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
	OmnConnBuffPtr docbuff;
	mIdxFname = idxFname;
	mDocFname = docFname;
	if (!readDoc(docid, docbuff)) return false;
	sendCreateObjReq(siteid, docbuff->getData());
	return true;
}


// username = "yuhui"
// hpvpd = "yyy_room_frame"
// login_vpdname = "yyy_login"
// ctnr = "yunyuyan_account"
// passwd = "12345"
bool
AosBrowserSimu::login(
		const OmnString &siteid, 
		const OmnString &username,
		const OmnString &hpvpd,
		const OmnString &login_vpdname,
		const OmnString &ctnr,
		const OmnString &passwd, 
		OmnString &ssid)
{
 	OmnConnBuffPtr buff = OmnNew OmnConnBuff();

	mLock->lock();
	u32 trans_id = mTransId++;
	mLock->unlock();
	
	OmnTcpClientPtr conn = getConn();
	aos_assert_r(conn, false);

	//Login
	OmnString obj;
	obj << "<embedobj "<< AOSTAG_USERNAME << "=\""<< username << "\"" 
		<< " opr=\"login\" "
		<< AOSTAG_HPVPD << "=\"" << hpvpd << "\" "
		<< AOSTAG_LOGIN_VPD << "=\"" << login_vpdname << "\" "
		<< AOSCONFIG_CTNR << "="<< "\"" << ctnr << "\"" <<"><" 
		<< AOSTAG_PASSWD <<">"<< passwd <<"</"<< AOSTAG_PASSWD
		<< "></embedobj>";

	OmnString req;
	req << "command=" << obj <<"&"
		<< AOSTAG_SITEID << "=" << siteid << "&"
		<< "operation=serverCmd" << "&"
		<< "trans_id=" << trans_id << "&"
	 	<< "loginobj=true" ;

	mClient->sendRequestPublic(sguri, sgmethod, req, sgcookies, conn, buff);
	returnConn(conn);

	if(!buff)
	{
		aos_assert_r(buff, false);
		return false;
	}

	OmnString cont = buff->getString();
	int idx = cont.indexOf("<response>", 0);
	if (idx > 0)
	{
		const char *data = cont.data();
		OmnString response(&data[idx]);
		response.removeTailWhiteSpace();
		AosXmlParser parser;
		AosXmlTagPtr respose = parser.parse(response, "");
		AosXmlTagPtr status = respose->getFirstChild("status");
		AosXmlTagPtr sessid = status->getFirstChild("zky_lgnobj");
		ssid = sessid->getAttrStr("zky_ssid");
		
		//application
		bool rs = retrieveDocss(siteid, ssid, "objid_37401");
		queryDocByObjid(siteid, "objid_37401", -1, -1);
		aos_assert_r(rs, false);

		return true;	
	}

	return false;
}

	
bool
AosBrowserSimu::retrieveDocss(
		const OmnString &siteid, 
		const OmnString &ssid,
		const OmnString &objid)
{
	mLock->lock();
	u32 trans_id = mTransId++;
	mLock->unlock();
	
 	OmnConnBuffPtr buff = OmnNew OmnConnBuff();

	OmnTcpClientPtr conn = getConn();
	aos_assert_r(conn, false);


	OmnString parms;
	parms << "zky_siteid=" << siteid << "&"
		<< "operation=retrieve" <<"&"
		<< "username=nonameyet" << "$"
		<< "zky_ssid=" << ssid << "&"
		<< "objid="	<< objid << "&"
		<< "trans_id=" << trans_id;
			
	mClient->sendRequestPublic(sguri,sgmethod, parms, sgcookies, conn, buff);
	returnConn(conn);
	
	if (!buff)
	{
//	    OmnScreen << "Failed to read response!" << endl;
		OmnAlarm << "Results incorrect: " << enderr;
	    return false;
	}
//	OmnScreen << "~~~~~~~~~~~~~~success~~retrieve~~~~~~~~~~~~~~~~~~" << endl;
	return true;
}


AosXmlTagPtr
AosBrowserSimu::retrieveDoc(
		        const OmnString &siteid,
				const OmnString &objid,
				const OmnString &sessionId)
{
	    OmnString req = "<request><item name='zky_siteid'><![CDATA[";
		req << siteid << "]]></item>"
		        << "<item name='operation'><![CDATA[retrieve]]></item>"
	            << "<item name='username'><![CDATA[nonameyet]]></item>"
		        << "<item name=\"zky_ssid\">" << sessionId << "</item>"
		        << "<item name='objid'><![CDATA["
		        << objid << "]]></item>"
		        << "<item name='trans_id'><![CDATA[53]]></item></request>";

			    OmnString errmsg;
			    OmnString resp;
			    aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
			    aos_assert_r(resp != "", false);
			    AosXmlParser parser;
			    return parser.parse(resp, "");
}

bool
AosBrowserSimu::readDocByObjid(
		const OmnString &objid,
		const u64 &maxDocid,
		const OmnString &siteid,
		const bool readDeleted)
{
	OmnConnBuffPtr docbuff;
	OmnString oid;
	for (u64 docid=1; docid<maxDocid; docid++)
	{
		OmnScreen << "Read doc: " << docid << endl;
		if (!readDoc(docid, docbuff, readDeleted)) continue;
	
		AosXmlParser parser;
		AosXmlTagPtr doc = parser.parse(docbuff->getData(), "");
		if (doc)
		{
			oid = doc->getAttrStr(AOSTAG_OBJID);
			if (oid == objid)
			{
				OmnScreen << "Doc read: " << doc->getData() << endl;
				return true;
			}
		}
	}
	return true;
}


bool
AosBrowserSimu::convertDataForRebuild(const AosXmlTagPtr &doc)
{

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
	convertImageDoc(doc);
	cleanData06222010(doc);
	return true;
}


bool
AosBrowserSimu::convertImageDoc(const AosXmlTagPtr &doc)
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
AosBrowserSimu::setImagePath(const OmnString &path)
{
	sgImagePath = path;
	return true;
}


bool
AosBrowserSimu::convertParentContners(
		const u64 &maxDocid,
		const OmnString &siteid,
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
		AosXmlTagPtr doc = parser.parse(docbuff, "");
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
				OmnStrParser parser(ctnrs, ", ");
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
AosBrowserSimu::cleanData06222010(const AosXmlTagPtr &doc)
{
	// 1. Remove all the attributes "zky_imdir"
	doc->removeAttr("zky_imdir", true, false);

	// 2. Remove all the attributes 'zky_ver' that are not at the root level
	doc->removeSubAttr("zky_ver", true, false);
	return true;
}


bool
AosBrowserSimu::readDocsByContainer(
		const OmnString &container,
		const u64 &maxDocid,
		const OmnString &siteid,
		const OmnString &idxFname, 
		const OmnString &docFname, 
		const bool readDeleted)
{
	OmnConnBuffPtr docbuff;
	mIdxFname = idxFname;
	mDocFname = docFname;
	OmnString ctnr;
	for (u64 docid=1; docid<maxDocid; docid++)
	{
		// OmnScreen << "Read doc: " << docid << endl;
		if (!readDoc(docid, docbuff, readDeleted)) continue;
	
		AosXmlParser parser;
		AosXmlTagPtr doc = parser.parse(docbuff->getData(), "");
		if (doc)
		{

			ctnr = doc->getAttrStr(AOSTAG_PARENTC);

			ctnr = doc->getContainer1();

			if (container == ctnr)
			{
				OmnScreen << "Doc read: " << doc->toString() << endl;
			}
		}
	}
	return true;
}


bool
AosBrowserSimu::retrieveObj(
		const OmnString &siteid, 
		const OmnString &ssid,
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
	// 		<item name='zky_siteid'><![CDATA[100]]></item>
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
	mLock->lock();
	u32 trans_id = mTransId++;
	mLock->unlock();
	
	OmnString req;
	req << "operation=retrieve" << "&"
		<< AOSTAG_SITEID << "=" << siteid << "&"
		<< "objid=" << objid << "&"
		<< "zky_ssid=" << ssid << "&"
		<< "trans_id=\"" << trans_id << "&"
		<< "<item name=\"" << AOSTAG_USERNAME << "\">" << username << "</item>"
		<< "<item name=\"" << AOSTAG_PASSWD << "\">" << passwd << "</item>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	xmlroot = parser.parse(resp, "");
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosBrowserSimu::retrieveObjs(
		const OmnString &siteid, 
		const OmnString &ssid,
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
	//	   	<item name='zky_siteid'><![CDATA[100]]></item>
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
	
	mLock->lock();
	u32 trans_id = mTransId++;
	mLock->unlock();

	OmnString reverseStr = (reverse)?"true":"false";
	OmnString totalStr = (getTotal)?"true":"false";
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"" << AOSTAG_USERNAME << "\">" << username << "</item>"
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
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	xmlroot = parser.parse(resp, "");
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosBrowserSimu::retrieveObjs(
		const OmnString &siteid, 
		const OmnString &ssid,
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
	//	   	<item name='zky_siteid'><![CDATA[100]]></item>
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
	
	mLock->lock();
	u32 trans_id = mTransId++;
	mLock->unlock();

	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"" << AOSTAG_USERNAME << "\">" << username << "</item>"
		<< "<item name=\"" << AOSTAG_PASSWD << "\">" << passwd << "</item>"
		<< command 
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	xmlroot = parser.parse(resp, "");
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}

bool
AosBrowserSimu::resolveUrl(
		const OmnString &siteid, 
		const OmnString &url,
		AosXmlTagPtr &root,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString &sessionId)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[100]]></item>
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

	aos_assert_r(url, false);
	mLock->lock();
	mLock->unlock();

	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">resolve_url</item>"
		<< "<item name=\"zky_ssid\">" << sessionId << "</item>"
		<< "<item name=\"args\">" << url << "</item>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	root = parser.parse(resp, "");
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

	vpd = child->getFirstChild();
	obj = child->getNextChild();
	return true;
}

bool
AosBrowserSimu::resolveUrl(
		const OmnString &siteid, 
		const OmnString &url,
		AosXmlTagPtr &root,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString &sessionId,
		AosXmlTagPtr &loginobj)
{
	vpd = 0;
	obj = 0;

	aos_assert_r(url, false);
	mLock->lock();
	mLock->unlock();

	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">resolve_url</item>"
		<< "<item name=\"loginobj\">true</item>"
		<< "<item name=\"zky_ssid\">" << sessionId << "</item>"
		<< "<item name=\"args\">" << url << "</item>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	root = parser.parse(resp, "");
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

	vpd = child->getFirstChild();
	obj = child->getNextChild();
	return true;
}

bool
AosBrowserSimu::requestNewId(
		const OmnString &siteid, 
		AosXmlTagPtr &idInfo)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">get_newid</item>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr xmlroot = parser.parse(resp, "");
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	idInfo = child;
	return true;

}


bool
AosBrowserSimu::rebuildLog(const u32 seqno, OmnFile &file)
{
	// Log files contain sequence of 
	// records. Each record is in the format:
	// 	Byte 0-3:	Doc size
	// 	Byte 4-: 	The doc itself
	// The first four bytes are encoded directly as binary.
	u32 offset = AosDocFileMgr::getReservedSize();
	AosXmlParser parser;
	while(1)
	{
		int docsize = file.readBinaryInt(offset, -1);
		if (docsize < 0) break;
		char *buff = OmnNew char[docsize+10];
		int bytesread = file.readToBuff(offset + 4, docsize, buff);
		aos_assert_r(bytesread == docsize, false);
		AosXmlTagPtr logdoc = parser.parse(buff, "");
		if (!logdoc)
		{
			OmnAlarm << "Failed to parse the doc: " << buff << enderr;
			OmnDelete buff;
			continue;
		}
		else
		{
OmnScreen << "offset :" << offset << ":" << logdoc->toString() <<endl;
			OmnString logid = logdoc->getAttrStr(AOSTAG_LOGID);
			if (logid != "")
			{
				OmnString args;
				args << seqno << ":" << offset;
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
AosBrowserSimu::rebuildLogs(const u32 max_seqno)
{
	// In the current implementations, all logs are in the files:
	// 		zkyielog_xxx
	// where 'xxx' is a sequence number. 
	
	OmnString fnamebase = "zykielog_";

	for (u32 i=0; i<max_seqno; i++)
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
			rebuildLog(i, file);
		}
	}

	return true;
}


AosXmlTagPtr
AosBrowserSimu::queryDocsByAttrs(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocsByAttrs(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocsByContainer(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocByObjid(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocByDocid(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocsByWords(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocsByTags(
		const OmnString &siteid, 
		const OmnString &tags, 
		const int startidx,
		const int queryid)
{
	// It queries the server based on the tags 'tags'. Upon success, 
	// the server responds the results:
	// 	<Contents>
	// 		<record .../>
	// 		<record .../>
	//		...
	//	</Contents>
	// The function will return an XML pointing to "<Contents>...</Contents>"
	OmnString query = "<tags type=\"tg\">";
	query << tags << "</tags>";

	const int psize = 20;
	const OmnString order = AOSTAG_OBJID;
	OmnString fnames; 
	fnames << AOSTAG_OBJID;

	return queryDocs(siteid, startidx, psize, "", order, fnames, query,queryid);
}


AosXmlTagPtr
AosBrowserSimu::queryDocs(
		const OmnString &siteid, 
		const int startidx,
		const int psize,
		const OmnString &ctnrs,
		const OmnString &order,
		const OmnString &fields,
		const OmnString &query,
		const int queryid)
{
	mLock->lock();
	u32 trans_id = mTransId++;
	mLock->unlock();

	OmnConnBuffPtr buff = OmnNew OmnConnBuff();

	OmnTcpClientPtr conn = getConn();
	aos_assert_r(conn, false);


	OmnString req ;
	OmnString obj ;
	
	obj	<< "<cmd start_idx=\"" << startidx << "\"";
	if(order != "")
	{
		obj << " order=" << "\"" << order << "\"";
	}
	obj << " psize=\"" << psize << "\"";
	if(queryid >= 0)
	{
		obj << " queryid=\"" << queryid<< "\"";
	}
	obj	<< " ctnrs=\"" << ctnrs << "\""
		<< " fnames=\"" << fields << "\""
		<< " opr=\"" << "retlist" << "\">"
		<< "<conds>"
		<< 	  "<cond type=\"AND\">"
		<< query
		<<    "</cond>"
		<< "</conds>"
		<< "</cmd>";

	req << AOSTAG_SITEID << "=" << siteid << "&" 
		<< "operation=serverCmd" << "&"
		<< "trans_id=" << trans_id << "&"
		<< "command=" << obj;

	 mClient->sendRequestPublic(sguri,sgmethod, req, sgcookies, conn, buff);
	 returnConn(conn);
	
	 
	 if(!buff)
	 {
		 aos_assert_r(buff, false);
	 }

	 OmnString cont = buff->getString();
	 int idx = cont.indexOf("<response>", 0);
	 aos_assert_r(idx, false);

	 const char *data = cont.data();
	 OmnString response(&data[idx]);
	 response.removeTailWhiteSpace();
	 AosXmlParser parser;
	 AosXmlTagPtr respose = parser.parse(response, "");
	 AosXmlTagPtr child = respose->getFirstChild();
	 aos_assert_r(child, 0);
	 bool exist;
	 aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	 return respose;
}


/*
AosXmlTagPtr
AosBrowserSimu::queryDocs(
		const OmnString &siteid, 
		const int startidx,
		const int psize,
		const OmnString &ctnrs,
		const OmnString &order,
		const OmnString &fields,
		const OmnString &query,
		const int queryid)
{
	mLock->lock();
	u32 trans_id = mTransId++;
	mLock->unlock();

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
	AosXmlTagPtr xmlroot = parser.parse(resp, "");
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	return xmlroot;
}


*/

AosXmlTagPtr
AosBrowserSimu::queryDocByWords(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocByContainer(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocByTags(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocByAttrs(
		const OmnString &siteid, 
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
AosBrowserSimu::queryDocByAttrs(
		const OmnString &siteid, 
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


bool
AosBrowserSimu::sendServerReq(
		const OmnString &siteid, 
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
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "");
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
AosBrowserSimu::createDoc(
		const OmnString &siteid, 
		const AosXmlTagPtr &doc,
		const OmnString &ssid, 
		const OmnString &owner_cid,
		const OmnString &resolve_objid, 
		const OmnString &saveas)
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
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name=\"" << AOSTAG_OWNER_CID << "\">" << owner_cid << "</item>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;

	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = resproot->getFirstChild("Contents");
	aos_assert_r(con, false);
	OmnString data = con->toString();
	con = parser.parse(data, "");
	aos_assert_r(con, false);
	return con;
}


AosXmlTagPtr 
AosBrowserSimu::retrieveDocByObjid(
		const OmnString &siteid, 
		const OmnString &ssid, 
		const OmnString &objid)
{
	OmnString resp;
	return retrieveDocByObjid(siteid,ssid,objid,resp);
}
	
AosXmlTagPtr 
AosBrowserSimu::retrieveDocByObjid(
		const OmnString &siteid, 
		const OmnString &ssid, 
		const OmnString &objid,
		OmnString &resp)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString request = "<request >";
	request << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name=\"objid\">" << objid << "</item>"
		<< "</request>";

	OmnString errmsg;
	aos_assert_r(mConn->procRequest(siteid, "", "", request, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") return 0;

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	aos_assert_r(child1, 0);
	AosXmlTagPtr child2 = child1->getFirstChild();
	aos_assert_r(child2, 0);

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "");
	return redoc;
}


AosXmlTagPtr 
AosBrowserSimu::retrieveDocByDocid(
		const OmnString &siteid, 
		const OmnString &ssid, 
		const u64 &docid)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString request = "<request >";
	request << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name=\"zky_docid\">" << docid << "</item>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", request, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	aos_assert_r(child1, 0);
	AosXmlTagPtr child2 = child1->getFirstChild();
	aos_assert_r(child2, 0);

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "");
	return redoc;
}


AosXmlTagPtr 
AosBrowserSimu::readDocByDocid(const OmnString &siteid, const u64 &docid)
{
	// It reads the doc 'docid' direct from the doc file.
	const int startidx = -1;
	OmnString doci;
	doci << docid;
	AosXmlTagPtr resproot = queryDocByDocid(siteid, doci, startidx);
	AosXmlTagPtr child = resproot->getFirstChild("Contents");
	AosXmlTagPtr record = child->getFirstChild("record");

	if (!record) return 0;

	OmnString objid = record->getAttrStr(AOSTAG_OBJID);
	AosXmlTagPtr doc = retrieveDoc(siteid, objid, "");
	aos_assert_r(doc, false);
	AosXmlTagPtr child1 = doc ->getFirstChild("Contents");
	AosXmlTagPtr child2 = child1 ->getFirstChild();

	OmnString data = child2->toString();
	AosXmlParser parser;
	AosXmlTagPtr redoc = parser.parse(data, "");
	return redoc;
}


bool 
AosBrowserSimu::vote(
		const int votetype,
		const OmnString &objid,
		const OmnString &mode,
		const OmnString &num,
		const OmnString &hour,
		const OmnString &siteid) 
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
	//	<item name="zky_siteid"><![CDATA[100]]></item>
	//	<item name="operation"><![CDATA[serverreq]]></item>
	//	<item name="args"><![CDATA[type=zky_voteup,objid=vpd_vote.100013,
	//	mode =siggle,num=1,hour=0]]></item>
	//	<item name="reqid"><![CDATA[vote]]></item>
	//	<item name="trans_id"><![CDATA[21]]></item>
	//	</request>

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosBrowserSimu::createUserCtnr(
		const OmnString &siteid, 
		const AosXmlTagPtr &doc,
		const OmnString &ssid,
		OmnString &new_objid, 
		u64 &server_docid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[100]]></item>
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
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << docstr << "</objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "");
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	child = root->getFirstChild("Contents");
	aos_assert_r(child, false);

	new_objid = child->getAttrStr(AOSTAG_OBJID);
	server_docid = atoll(child->getAttrStr(AOSTAG_DOCID));
	return true;
}


bool
AosBrowserSimu::createUserAcct(
		OmnString &objid,
		const AosXmlTagPtr &doc,
		const OmnString &ssid,
		u64 &server_docid,
		OmnString &passwd,
		OmnString &cid,
		const OmnString &siteid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[100]]></item>
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
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "");
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	AosXmlTagPtr con = root->getFirstChild("Contents");
	aos_assert_r(con, false);

	AosXmlTagPtr acc = con->getFirstChild("accounts");
	aos_assert_r(acc, false);

	AosXmlTagPtr child1 = acc->getFirstChild("account");
	aos_assert_r(child1, false);

	cid = child1->getAttrStr("username");
	passwd = child1->getNodeText();

	AosXmlTagPtr record = acc->getFirstChild("record");

	objid = record->getAttrStr(AOSTAG_OBJID);
	server_docid = atoll(record->getAttrStr(AOSTAG_DOCID));

	return true;
}

bool
AosBrowserSimu::createSuperUser(
			const OmnString &requester_passwd,
			AosXmlTagPtr &userdoc,
			const OmnString &siteid)
{
	u32 trans_id = mTransId++;
	OmnString obj;
	obj <<"<embedobj " << AOSTAG_PASSWD "=\""<<requester_passwd <<"\"/>";

	OmnString req = "<request>";
	req << "<objdef>" << obj <<"</objdef>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"reqid\">create_super_user</item>"
		<< "<item name=\"args\">null</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "</request>";
	//<request>
	//	<objdef>
	//		<embedobj/> 
	//	</objdef>
	//	<item name="zky_siteid"><![CDATA[100]]></item>
	//	<item name="operation"><![CDATA[serverreq]]></item>
	//	<item name="reqid"><![CDATA[create_user]]></item>
	//	<item name="args"><![CDATA[null]]></item>
	//	<item name="trans_id"><![CDATA[13]]></item>
	//</request>

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	userdoc = resproot->getFirstChild("Contents");
	aos_assert_r(userdoc, false);
	userdoc = userdoc->getFirstChild();
	aos_assert_r(userdoc, false);

	OmnString docstr = userdoc->toString();
	userdoc = parser.parse(docstr, "");
	aos_assert_r(userdoc, false);
	return true;
}

bool
AosBrowserSimu::createAccessRcd(
		const OmnString &siteid, 
		const AosXmlTagPtr &doc,
		OmnString &objid,
		const OmnString &ssid,
		u64 &server_docid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[100]]></item>
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
	
	/*
	OmnString docstr = "<accessctnr ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_ACCESS_RCD
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_AUTO 
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOTYPE_ACCESS_RCD
		<< "\" " << AOSTAG_PARENTC << "=\"" << createdoc->getAttrStr(AOSTAG_HOME_CTNR)
		<< "\" " << AOSTAG_OBJID << "=\"" << objid
		//<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" << AOSACTP_LOGIN_PUBLIC
		<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" << "lpublic"
		<< "\" " << AOSTAG_DELMEM_ACSTYPE << "=\"" << AOSACTP_PRIVATE
		<< "\" " << AOSTAG_ADDMEM_ACSTYPE << "=\"" << AOSACTP_PRIVATE
		<< "\" " << AOSTAG_DELETE_ACSTYPE << "=\"" << AOSACTP_PRIVATE
		<< "\" " << AOSTAG_COPY_ACSTYPE << "=\"" << AOSACTP_PRIVATE
		<< "\" " << AOSTAG_CREATE_ACSTYPE << "=\"" << AOSACTP_PRIVATE
		<< "\" " << AOSTAG_WRITE_ACSTYPE << "=\"" << AOSACTP_PRIVATE
		<< "\" " << AOSTAG_OWNER_DOCID << "=\"" << docid
		<< "\"/>";
	*/
	OmnString docstr = doc->toString();

	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << docstr << "</objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "");
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
	//		<obj>
	//		...
	//		</obj>
	//	</Contents>
	child = root->getFirstChild("Contents");
	aos_assert_r(child, false);
	objid = child->getAttrStr(AOSTAG_OBJID);
	server_docid = atoll(child->getAttrStr(AOSTAG_DOCID));
	return true;
}


AosXmlTagPtr
AosBrowserSimu::getAccessRcd(
		const OmnString &siteid, 
		const u64 &docid,
		const OmnString &ssid)
{
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='args'>"<< docid <<"</item>"
		<< "<item name='reqid'>arcd_get</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name='trans_id'>"<< trans_id << "</item></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr doc =  parser.parse(resp, "");

	AosXmlTagPtr child1 = doc ->getFirstChild("Contents");
	AosXmlTagPtr child2 = child1 ->getFirstChild();

	OmnString data = child2->toString();
	AosXmlParser parser1;
	AosXmlTagPtr redoc = parser1.parse(data, "");
	return redoc;
}


bool
AosBrowserSimu::logout(const OmnString &siteid, const OmnString &ssid)
{
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>logout</item>"
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "<item name='trans_id'>"<< trans_id << "</item></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot =  parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	return (child->xpathQuery("status/code", exist, "") == "200");
}


bool
AosBrowserSimu::addFriendResp(const OmnString &siteid, const OmnString &requester, const OmnString &friendid)
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
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosBrowserSimu::buildDocByObjid(
		const OmnString &objid,
		const u64 &maxDocid,
		const OmnString &siteid,
		const OmnString &idxFname, 
		const OmnString &docFname, 
		const bool readDeleted)
{
	OmnConnBuffPtr docbuff;
	mIdxFname = idxFname;
	mDocFname = docFname;
	OmnString oid;
	for (u64 docid=1; docid<maxDocid; docid++)
	{
		OmnScreen << "Read doc: " << docid << endl;
		if (!readDoc(docid, docbuff, readDeleted)) continue;
	
		AosXmlParser parser;
		AosXmlTagPtr doc = parser.parse(docbuff->getData(), "");
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



bool
AosBrowserSimu::showDocId(u64 did)
{
	OmnConnBuffPtr docbuff;
	AosXmlParser parser;
	
	if(!readDoc(did, docbuff))
	{
		OmnScreen << "Fail to read doc by did:" << did <<endl;
	}	

	AosXmlTagPtr doc =  parser.parse(docbuff, "");
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	OmnScreen <<  "doc id is: " << docid <<endl;
	return true;
}


OmnTcpClientPtr
AosBrowserSimu::getConn()
{
	mLock->lock();
	for (int i=0; i<mNumConns; i++)
	{
		if (mConnIdle[i])
		{
			OmnTcpClientPtr conn = mConns[i];
			mConnIdle[i] = false;
			mLock->unlock();
			return conn;
		}
	}

	if (mNumConns < eMaxConns)
	{
		OmnTcpClientPtr conn = OmnNew OmnTcpClient("nn", mRemoteAddr, mRemotePort, 10, eAosTLT_NoLengthIndicator);
		conn->setClientId(mNumConns);
		mConnIdle[mNumConns] = false;
		mConns[mNumConns] = conn;
		mNumConns++;
		mLock->unlock();
		return conn;
	}

	mLock->unlock();
	OmnAlarm << "Too many connections. Max allowed: " << eMaxConns << enderr;
	return OmnNew OmnTcpClient("error", mRemoteAddr, mRemotePort, 1, eAosTLT_NoLengthIndicator);
}


bool
AosBrowserSimu::returnConn(const OmnTcpClientPtr &conn)
{
	conn->closeConn();
	int idx = conn->getClientId();
	aos_assert_r(idx >= 0 && idx < mNumConns, false);
	mLock->lock();
	mConnIdle[idx] = true;
	mLock->unlock();
	return true;
}

bool
AosBrowserSimu::test()
{
//	while(1)
	{
		OmnString siteid = "100";
		OmnString username = "yuhui";
		OmnString hpvpd = "yyy_room_frame";
		OmnString login_vpdname = "yyy_login";
		OmnString ctnr = "yunyuyan_account";
		OmnString passwd = "12345";
		OmnString ssid;
		//OmnString objid = "zky_oidur_yuhui";
		OmnString objid = "objid_37401";
	
		login(siteid, username, hpvpd, login_vpdname, ctnr, passwd, ssid);
		retrieveDocss(siteid, ssid, objid);
	}
		  return true;
}	
