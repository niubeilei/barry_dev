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
// 01/30/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEModules/VersionMgr.h"

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Rundata/Rundata.h"
#include "SEUtil/IILName.h"
#include "SEUtil/VersionDocMgr.h"
#include "SEUtil/DocTags.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"


static OmnString AOS_INITVERSION = "1.0";

AosVersionMgr::AosVersionMgr()
:
mLock(OmnNew OmnMutex())
{
	aos_assert(OmnApp::getAppConfig());
	mVersionDocMgr = OmnNew AosVersionDocMgr();

	AosXmlTagPtr def = OmnApp::getAppConfig()->getFirstChild("versionmgr");
	aos_assert(def);
	u64 maxfilesize = def->getAttrU64("maxfsize", -1);
	/*u64 largefilesize = def->getAttrU64(AOSCONFIG_LARGE_FILESIZE, 0);
	aos_assert(largefilesize > 0);*/
	//int headerperfile = def->getAttrInt("maxheaderperfile", -1);
	u64 maxdocfiles = def->getAttrU64("maxdocfiles", -1);
	// int maxheaderfiles = def->getAttrInt("maxheaderfiles", -1);
	mDirname = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_DIRNAME);
	mFilename = def->getAttrStr("filename");
	aos_assert(maxfilesize > 0);
	//aos_assert(headerperfile > 0);
	aos_assert(maxdocfiles > 0);
	//aos_assert(maxheaderfiles > 0);
	aos_assert(mDirname != "");
	aos_assert(mFilename != "");

	mVersionDocMgr->init(mDirname, mFilename);	
}


AosVersionMgr::~AosVersionMgr()
{
}


OmnString 
AosVersionMgr::incrementVersion(const OmnString &version)
{
	// 'version' is in the form:
	// 	<xxx>.<xxx>...<xxx>
	// This function increments the last segment by one.
	OmnString parts[eMaxVerLen];
	bool finished;
	AosStrSplit split(version, ".", parts, eMaxVerLen, finished);
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
AosVersionMgr::addVersionIdx(
		const u64 &docid,
		const u32 seqno, 
		const u32 offset, 
		const OmnString &version,
		const AosRundataPtr &rdata)
{
	// This function is used to rebuild the version index. 
	OmnString iilname = AosIILName::composeVersionIILName(docid);
	u64 ptr = (((u64)seqno) << 32) + offset;
	AosIILClientObj::getIILClient()->addValueDoc(iilname, version, ptr, false, true, rdata);
	return true;
}


bool
AosVersionMgr::addVersionDoc(
		const u64 &docid,
		const OmnString& doc, 
		const OmnString &version,
		const AosRundataPtr &rdata)
{
	// This function is used to rebuild the version index. 
	u32 seqno;
	u64 offset;
	bool rslt = mVersionDocMgr->saveDoc(seqno, offset, doc.length(), doc.data());
	aos_assert_r(rslt, false);

	OmnString iilname = AosIILName::composeVersionIILName(docid);
	u64 ptr = (((u64)seqno) << 32) + offset;
	AosIILClientObj::getIILClient()->addValueDoc(iilname, version, ptr, false, true, rdata);
	return true;
}
	

bool
AosVersionMgr::addVersionObj(
		const u64 &docid,
		const AosXmlTagPtr &olddoc, 
		const AosXmlTagPtr &newdoc,
		const AosRundataPtr &rdata)
{
	// This function stores the doc into the version file and
	// add an IIL entry. If needed, it may remove the older
	// versions and IIL entries. The IIL entry is in the form:
	// 	[AOSIILTAG_VERSION + docid, version, docid]
	// where 'docid' is an u64. Its high four bytes are the seqno
	// and low four bytes offset. If we want to retrieve a 
	// specific version of a given doc, find the entry:
	// 	[AOSIILTAG_VERSION + docid, version]
	// then use the docid to retrieve the object. 
	mLock->lock();
	bool exist;
	OmnString version = olddoc->getAttrStr1(AOSTAG_VERSION, exist);
	//Linda,SengTorturer
	// the old doc and iil must synchronism,  but the old doc will be 
	// replace by new doc , it will be save to other palce, and replace iil
	// from new doc . so we keep the iil status , save the change to file.
	AosXmlTagPtr oldvdoc = olddoc->clone();

	//Ketty 2011/02/17
	if (version == "")
	{
		version = AOS_INITVERSION;
		oldvdoc->setAttr(AOSTAG_VERSION, version);
	}
	
	const char *data = (const char *)oldvdoc->getData();
	int len = oldvdoc->getDataLength();
	aos_assert_rl(len > 0, mLock, false);

	// Save the doc
	u32 seqno;
	u64 offset;
	aos_assert_rl(mVersionDocMgr->saveDoc(seqno, offset, len, data), mLock, false);

	// Add the IIL entry
	OmnString iilname = AosIILName::composeVersionIILName(docid);
	u64 ptr = (((u64)seqno) << 32) + offset;

	//if (version == "")
	//{
	//	AosIILClientSelf->addValueDoc(iilname, AOS_INITVERSION, ptr, false, true);
	//}
	//else
	//{
	//AosIILClientSelf->addValueDoc(iilname, version, ptr, false, true, rdata);
	AosIILClientObj::getIILClient()->addValueDoc(iilname, version, ptr, false, true, rdata);
	//}

	if(newdoc)
	{
		OmnString newver = incrementVersion(version);
		newdoc->setAttr(AOSTAG_VERSION, newver);
	}
	// Update the version
	/*
	if (newdoc)
	{
		//OmnString newver = incrementVersion((version == "")?AOS_INITVERSION:version);


		OmnString vv = newdoc->getAttrStr(AOSTAG_VERSION);
		if (vv == "")
		{
			// The document has no version attribute yet. 
			// Need to add the following words:
			//AosIILClientSelf->addDoc(AOSTAG_VERSION, strlen(AOSTAG_VERSION), docid);
			OmnString iilname = composeAttrIILName(AOSTAG_VERSION);
			AosIILClientObj::getIILClient()->addValueDoc2(iilname, newver, docid, false, true);
		}
		else
		{
			if (version != "")
			{
				// The document already had version attribute. 
				// Need to remove the old version and add the new version
				//AosIILClientSelf->removeDoc(version.data(), version.length(), docid);
			}

			OmnString nn = AosIILName::composeAttrIILName(AOSTAG_VERSION);
			AosIILClientObj::getIILClient()->modifyValueDoc1(nn, vv, newver, false, true, docid);
		}
		//AosIILClientSelf->addDoc(newver.data(), newver.length(), docid);
	}*/
	
	mLock->unlock();
	return true;
}


AosXmlTagPtr
AosVersionMgr::getVersionObj(
		const u64 &docid,
		const OmnString &version, 
		OmnString &errmsg,
		const AosRundataPtr &rdata)
{
	// This function retrieves a specific version of the doc 'docid'. 
	// If not found, it returns null. If 'version' is '-', it means 
	// the last version. 
	// 
	// Versions are stored in the IIL:
	// 	(AOSIILTAG_VERSION + siteid, version + ":" + docid)
	// 
	if (version == "")
	{
		errmsg = "Version is empty: ";
		errmsg << docid;
		OmnAlarm << errmsg << enderr;
		return 0;
	}

	mLock->lock();
	OmnString iilname = AosIILName::composeVersionIILName(docid);
	u64 did;
	bool isunique;
	//Zky2248,Linda, 01/13/2011
	aos_assert_r(version!="",0); 
	bool rslt = AosIILClientSelf->getDocid(iilname, version, did, isunique, rdata);
	//bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, version, did, isunique, rdata);
	if (!rslt || did == AOS_INVDID) 
	{
		mLock->unlock();
		errmsg = "Version not found: ";
		errmsg << version << ":" << docid;
		return 0;
	}

	if (!isunique)
	{
		OmnAlarm << "Found a duplicated version: " 
			<< ":" << docid << ":" << version << enderr;
	}

	// The higher four bytes are the seqno and the lower four bytes 
	// are the offset. 
	u32 seqno = (did >> 32);
	u32 offset = (u32)did;

	AosXmlTagPtr xml = mVersionDocMgr->readDoc(seqno, offset);
	mLock->unlock();
	return xml;
	return 0;
}


AosXmlTagPtr
AosVersionMgr::getVersionObj(const u64 &docid)
{
	mLock->lock();
	u32 seqno = (docid >> 32);
	u32 offset = (u32)docid;
	AosXmlTagPtr xml = mVersionDocMgr->readDoc(seqno, offset);
	mLock->unlock();
	return xml;
	return 0;
}


AosXmlTagPtr
AosVersionMgr::getLastVersionObj(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnString iilname = AosIILName::composeVersionIILName(docid);

	//AosIILPtr iil = AosIILClientObj::getIILClient()->getIILPublic(iilname, rdata);
	//if(!AosIILClientObj::getIILClient()->iilExist(iilname, rdata)) return 0;
/*
	if(iil->getIILType() != eAosIILType_Str)
	{
		OmnAlarm << "Version type is wrong! " << enderr;
		return 0;
	}
			
	// 2.get the newest version doc from the iil	
	AosIILStrPtr striil = (AosIILStr *)iil.getPtr();
	int idx = -10;
	int iilidx = -10;
	OmnString value;
	u64 vid;
	bool rslt = striil->nextDocidSafe(idx, iilidx, true, value, vid, rdata);
	aos_assert_r(rslt, 0);
	
	u64 vid = AOS_INVDID;
	int idx = -10;
	int iilidx = -10;
	bool isunique = false;
	bool rslt = AosIILClientObj::getIILClient()->nextDocidSafe(iilname, idx, iilidx, true, eAosOpr_an, "", vid, isunique, rdata);
*/
	
	u64 vid = AOS_INVDID;
	bool isunique = false;
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, "", eAosOpr_an, true, vid, isunique, rdata);
	aos_assert_r(rslt, 0);

	AosXmlTagPtr vDoc = getVersionObj(vid);
	return vDoc;
}


bool
AosVersionMgr::stop()
{
	mVersionDocMgr->stop();
	return true;
}


bool
AosVersionMgr::rebuildVersion()
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
#endif
