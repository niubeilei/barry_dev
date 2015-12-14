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
// Docs may have different versions. To retrieve the current version of
// a doc, use docid or objid. To retrieve a version of a doc, it needs
// to use [objid, version]. There is an IIL that maps [objid, version] 
// to docid. 
//
// In an application, it is possible to lock to a specific version of 
// a VPD. When loading a vpd for an application, it checks whether 
// there is an entry:
// 		[vpd, version]
// If yes, it means the vpd is not the current vpd but a specific version.
// It should retrieve that version instead. 
//
// Each container has a version IIL, which is a string IIL. The value 
// portion is objid + version, and the docid portion is the docid. 
//   
//
// Modification History:
// 01/30/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VersionServer/VersionServer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "SEInterfaces/DocClientObj.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Rundata/Rundata.h"
#include "SEUtil/IILName.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"
#include "API/AosApi.h"

OmnSingletonImpl(AosVersionServerSingleton,
	AosVersionServer,
	AosVersionServerSelf,
	"AosVersionServer");


static OmnString AOS_INITVERSION = "1.0";

AosVersionServer::AosVersionServer()
:
mLock(OmnNew OmnMutex())
{
}


AosVersionServer::~AosVersionServer()
{
}


bool
AosVersionServer::start()
{
	return true;
}


bool
AosVersionServer::config(const AosXmlTagPtr &config)
{
	/*
	aos_assert_r(config, false);

	AosXmlTagPtr def = config->getFirstChild("versionmgr");
	aos_assert_r(def, false);
	int maxfilesize = def->getAttrInt("maxfsize", -1);
	int maxdocfiles = def->getAttrInt("maxdocfiles", -1);
	mDirname = config->getAttrStr(AOSCONFIG_DIRNAME);
	mFilename = def->getAttrStr("filename");
	aos_assert_r(maxfilesize > 0, false);
	aos_assert_r(maxdocfiles > 0, false);
	aos_assert_r(mDirname != "", false);
	aos_assert_r(mFilename != "", false);
*/
	return true;
}


OmnString 
AosVersionServer::incrementVersion(const OmnString &version)
{
	// 'version' is in the form:
	// 	<xxx>.<xxx>...<xxx>
	// This function increments the last segment by one.
	if (version == "") return "1.0";
	OmnString parts[eMaxVerLen];
	bool finished;
	AosStrSplit split(version.data(), ".", parts, eMaxVerLen, finished);
	OmnString lastone = parts[split.entries()-1];
	int vv = atoi(lastone.data());
	if (vv < 0) vv = 0;
	vv++;

	OmnString ss;
	for (int i=0; i<split.entries()-1; i++)
	{
		if (i != 0) ss << ".";
		ss << parts[i];
	}
	if (ss != "") ss << ".";
	ss << vv;
	return ss;
}


bool
AosVersionServer::addVersionObj(
		const AosXmlTagPtr &newdoc,
		const AosRundataPtr &rdata)
{
	// Ken Lee, 2013/05/07
	return true;
	// This function stores the doc into the logs and
	// add an IIL entry. If needed, it may remove the older
	// versions and IIL entries. 
	mLock->lock();
	OmnString version;
	if(newdoc)
	{
		OmnString objid = newdoc->getAttrStr(AOSTAG_OBJID);
		aos_assert_rl(objid != "", mLock, false);
		
		AosXmlTagPtr olddoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
		
		if (olddoc)
		{
			version = newdoc->getAttrStr(AOSTAG_VERSION);
		}
		else
		{
			u64 logid;
			bool isunique;
			OmnString version;
			OmnString iilname = AosIILName::composeDeletedObjidIILName();
			aos_assert_rl(iilname!="", mLock, false);
			AosIILClientObj::getIILClient()->getDocid(iilname,
					objid, eAosOpr_eq, true, logid, isunique, rdata);
			if (logid != 0)
			{
				// Chen Ding, 2013/06/17
				OmnNotImplementedYet;
				return false;
				/*
				AosXmlTagPtr log = AosSeLogClientObj::getSelf()->retrieveLog(logid, rdata);
				if (!log)
				{
					AosSetError(rdata, AosErrmsgId::eFailedRetrieveLog);
					OmnAlarm << rdata->getErrmsg() << enderr;
					mLock->unlock();
					return false;
				}
				version = log->getAttrStr(AOSTAG_VERSION);
				aos_assert_rl(version != "", mLock, false);
				if (version.indexOf(AOSTAG_VERDEL_FLAG, 0) == -1)
				{
					// version: zky_ver__n="1.7_delete_yet"
					OmnAlarm << "Invalid version" << enderr;
					mLock->unlock();
					return false;
				}
				int idx = version.indexOf("_", 0);
				aos_assert_rl(idx>0, mLock, false);
				version = version.subString(0, idx);
				aos_assert_rl(version != "", mLock, false);
				*/
			}
		}
	}
	else
	{
		OmnAlarm << "No doc" << enderr;
		mLock->unlock();
		return false;
	}
	
	OmnString newver = incrementVersion(version);
	newdoc->setAttr(AOSTAG_VERSION, newver);
	AosSeLogClientObj::getSelf()->addVersion(newdoc, rdata);
	mLock->unlock();
		
	return true;
}


AosXmlTagPtr
AosVersionServer::getVersionObj(
		const OmnString &objid,
		const OmnString &pctr_objid,
		const OmnString &version, 
		const AosRundataPtr &rdata)
{
	// This function retrieves a specific version of the doc 'objid'. 
	// If not found, it returns null. If 'version' is '-' or empty, it means 
	// the last version. 
	// 
	// Versions are stored in the IIL:
	// 	(AOSIILTAG_VERSION + siteid, version + ":" + docid)
	// 
	if (version == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingVersion);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Sitid: " << rdata->getSiteid() << enderr;
		return 0;
	}
	
	if (objid == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingVersion);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Sitid: " << rdata->getSiteid() << enderr;
		return 0;
	}
	if (pctr_objid == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingVersion);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Sitid: " << rdata->getSiteid() << enderr;
		return 0;
	}

	mLock->lock();
	u64 logid;
	OmnString iilname = AosIILName::composeVersionIILName(pctr_objid);
	aos_assert_rl(iilname != "", mLock, 0);

	OmnString key = composeVersionIILKey(objid, version);
	aos_assert_rl(key != "", mLock, 0);
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, key, logid, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eMissingLogid);
		OmnAlarm << rdata->getErrmsg()
			<< ". IILName: " << iilname
			<< ". Key: " << key << enderr;
		mLock->unlock();
		return 0;
	}
	aos_assert_rl(logid > 0, mLock, 0);
	
	/*
	// Chen Ding, 08/24/2011
	OmnString nn;
	nn << docid << "_" << version << AosGetAttrPostfix(eAosAttrType_NumAlpha);

	//Zky2248,Linda, 01/13/2011
	aos_assert_r(version!="",0); 
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, nn, did, isunique, rdata);
	if (!rslt || did == AOS_INVDID) 
	{
		mLock->unlock();
		rdata->setError() << "Version not found: " << version << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	if (!isunique)
	{
		rdata->setError() << "Found a duplicated version: " 
			<< ":" << docid << ":" << version;
		OmnAlarm << rdata->getErrmsg() << enderr;
	}

	// The higher four bytes are the seqno and the lower four bytes 
	// are the offset. 
	u32 seqno = (did >> 32);
	u32 offset = (u32)did;

	AosXmlTagPtr xml = mVersionDocMgr->readDoc(seqno, offset);
	*/
	AosXmlTagPtr verDoc = getVersionObj(logid, rdata);
	if (!verDoc)
	{
		AosSetError(rdata, AosErrmsgId::eFailedToRetrieveVersionDoc);
		OmnAlarm << rdata->getErrmsg()
			<< ". Logid: " << logid
			<< enderr;
		mLock->unlock();
		return 0;
	}
	mLock->unlock();
	return verDoc;
}


AosXmlTagPtr
AosVersionServer::getVersionObj(const u64 &docid, const AosRundataPtr &rdata)
{
	// Docid is considered as logid
	// Chen Ding, 2013/06/17
	OmnNotImplementedYet;
	return 0;
	/*
	AosXmlTagPtr xml = AosSeLogClientObj::getSelf()->retrieveLog(docid, rdata);
	if (!xml)
	{
		AosSetError(rdata, AosErrmsgId::eFailedToRetrieveVersionDoc);
		OmnAlarm << rdata->getErrmsg() << ". Logid: " << docid << enderr;
		return 0;
	}
	return xml;
	*/
}


/*
AosXmlTagPtr
AosVersionServer::getLastVersionObj(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnString iilname = AosIILName::composeVersionIILName();

	// Chen Ding, 08/24/2011
	OmnString nn;
	nn << docid << "_" << version << AosGetAttrPostfix(eAosAttrType_NumAlpha);
	if(!AosIILClientObj::getIILClient()->iilExist(iilname, rdata)) return 0;
	u64 vid = AOS_INVDID;
	int idx = -10;
	int iilidx = -10;
	bool isunique = false;
	bool rslt = AosIILClientObj::getIILClient()->nextDocidSafe(iilname, idx, iilidx, 
			true, eAosOpr_eq, nn, vid, isunique, rdata);
	aos_assert_r(rslt, 0);
	AosXmlTagPtr vDoc = getVersionObj(vid);
	return vDoc;
}
*/


bool
AosVersionServer::stop()
{
	return true;
}


bool
AosVersionServer::rebuildVersion()
{
	// This function is used to rebuild the entire version.
	// Versions are stored through 'mVersionDocMgr', in multiple
	// files. Each version object is stored in the following
	// format:
	// 	<docsize>	4  bytes
	// 	<docid>		8  bytes
	// 	<reserved>	20 bytes
	// 	<doc body>	the remaining
	// 
	// It reads the records one by one. For each record, it adds
	// an entry to the following iil:
	// 			[AOSIILTAG_VERSION + docid, version, position]
	// where 'docid' is the docid of the document, 'version'
	// is the doc's version, and 'position' is a u64 with the
	// high 4-bytes for seqno and low 4-bytes the offset:
	//		(((u64)seqno) << 32) + offset;
	//
	OmnNotImplementedYet;
	return false;
}


AosXmlTagPtr
AosVersionServer::getVersionObj(
		const u64 &docid,
		const OmnString &objid,
		const OmnString &version,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	if (!doc)
	{
		AosSetError(rdata, AosErrmsgId::eFailedRetrieveDoc);
		OmnAlarm << rdata->getErrmsg()
			<< ". Docid: " << docid << enderr;
		return 0;
	}
	
	OmnString ctnr_objid = doc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(ctnr_objid != "", 0);

	OmnString iilname = AosIILName::composeVersionIILName(ctnr_objid);
	aos_assert_r(iilname != "", 0);
	
	OmnString key = composeVersionIILKey(objid, key);
	aos_assert_r(key != "", 0);
	
	u64 logid;
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, key, logid, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eMissingLogid);
		OmnAlarm << rdata->getErrmsg() 
			<< ". IILName: " << iilname
			<< ". Key: " << key << enderr;
		return 0;
	}
	aos_assert_r(logid > 0, 0);
	
	AosXmlTagPtr verDoc = getVersionObj(logid, rdata);
	if (!verDoc)
	{
		AosSetError(rdata, AosErrmsgId::eFailedToRetrieveVersionDoc);
		OmnAlarm << rdata->getErrmsg()
			<< ". Docid: " << docid
			<< ". Logid: " << logid
			<< enderr;
		return 0;
	}
	return verDoc;
}


AosXmlTagPtr
AosVersionServer::getVersionObj(
		const OmnString &objid,
		const OmnString &version,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 02/17/2017
	// Brian Zhang 30/09/2011
	// AOSMONITORLOG_ENTER(rdata);
	u32 siteid = rdata->getSiteid();
	
	if (objid == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingObjid);
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	if (version == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingVersion);
		OmnAlarm << rdata->getErrmsg() 
		 	<< ". Siteid: " << siteid 
			<< ". Ojbid: " << objid << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	u64 docid;
	bool isunique;
	// AOSMONITORLOG_LINE(rdata);
	
	// Zky2782, Ketty, 2011/02/14
	bool rslt = AosIILClientObj::getIILClient()->getDocidByObjid(siteid, objid, docid, isunique, rdata);
	if(!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eMissingObjid);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Siteid: " << siteid 
			<< ". Objid: " << objid << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	if (docid == AOS_INVDID)
	{
		// If did not find the docid. Check whether it is a deleted doc.
		// AOSMONITORLOG_LINE(rdata);
		OmnString iilname = AosIILName::composeDeletedObjidIILName();
		rslt = AosIILClientObj::getIILClient()->getDocid(iilname, objid, docid, isunique, rdata);

		if (!rslt || docid == AOS_INVDID)
		{
			AosSetError(rdata, AosErrmsgId::eDocidIsInvalid);
			OmnAlarm << rdata->getErrmsg()
				<< ". Siteid: " << siteid
				<< ". Objid: " << objid << enderr;
			// AOSMONITORLOG_FINISH(rdata);
			return 0;
		}
	}

	if (!isunique)
	{
		AosSetError(rdata, AosErrmsgId::eEntryNotUnique);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Siteid: " << siteid
			<< ". Objid: " << objid << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	AosXmlTagPtr vObj = getVersionObj(docid, objid, version, rdata);
	if(!vObj)
	{
		AosSetError(rdata, AosErrmsgId::eFailedToRetrieveVersionDoc);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Siteid: " << siteid
			<< ". Docid: " << docid
			<< ". Objid: " << objid 
			<< ". Version: " << version << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	rdata->setOk();
	// AOSMONITORLOG_FINISH(rdata);
	return vObj;
}


bool	
AosVersionServer::addVersionDoc(
		const u64 &docid,
		const OmnString &doc, 
		const OmnString &version,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool		
AosVersionServer::addVersionObj(
		const u64 &docid,
		const AosXmlTagPtr &olddoc,
		const AosXmlTagPtr &newdoc,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


u64
AosVersionServer::addDeletedVerObj(
		const AosXmlTagPtr &deldoc, 
		const AosRundataPtr &rdata)
{
	// This function add a deleted doc to logserver for backup.
	// Add a atribute zky_deleted="true" for confirm.
	// 1. Some doc maybe has not the attribute "zky_ver__n", we
	// 	  set version as "1.0".
	// 2. Normally, we set version as 
	aos_assert_r(deldoc, false);
	mLock->lock();

	// Chen Ding, 01/29/2012
	// TSK001NOTE
	// This will cause IIL Server problem
	// deldoc->setAttr(AOSTAG_VERDEL_FLAG, "true");
	OmnString version = deldoc->getAttrStr(AOSTAG_VERSION);
	if (version == "") version = "1.0";
	version << "_" << AOSTAG_VERDEL_FLAG;

	// Chen Ding, 01/29/2012
	// TSK001NOTE
	// This will cause IIL Server problem
	// deldoc->setAttr(AOSTAG_VERSION, version);
	AosXmlTagPtr dd = deldoc->clone(AosMemoryCheckerArgsBegin);
	dd->setAttr(AOSTAG_VERSION, version);
	u64 logid = AosSeLogClientObj::getSelf()->addVersion(dd, rdata);
	mLock->unlock();
	aos_assert_r(logid>0, false);	
	return logid;
}
