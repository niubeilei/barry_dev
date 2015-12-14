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
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/DocServer1.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "DocClient/DocidMgr.h"
#include "SEUtil/IILName.h"
#include "Thread/LockMonitor.h"
#include "SEUtil/TagMgr.h"
#include "SEUtil/Objname.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/ObjidSvr.h"
#include "SEUtilServer/SvUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SmartDoc/SmartDoc.h"
#include "Thread/Thread.h"
#include "Thread/LockMonitor.h"
#include "Thread/LockMonitorMgr.h"
#include "Util/StrSplit.h"
#include "VersionServer/VersionServer.h"
#include "XmlUtil/SeXmlParser.h"
#if 0

using namespace std;

static AosVersionDocMgr sgTmpObjMgr;

bool 	AosDocServer::mShowLog = false;

OmnSingletonImpl(AosDocServerSingleton,
                 AosDocServer,
                 AosDocServerSelf,
                "AosDocServer");

AosDocServer::AosDocServer()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
//mIsRepairing(false),
mMaxObjidTries(eDftMaxObjidTries)
{
	mLockMonitor = OmnNew AosLockMonitor(mLock);
	mLock->mMonitor = mLockMonitor;
	AosLockMonitorMgr::getSelf()->addLock(mLockMonitor);
}


AosDocServer::~AosDocServer()
{
}


bool
AosDocServer::start()
{
	return true;
}


bool
AosDocServer::config(const AosXmlTagPtr &config)
{
	if (!config)
	{
		OmnAlarm << "Missing configuration" << enderr;
		exit(-1);
	}

	OmnString dirname = config->getAttrStr(AOSCONFIG_DIRNAME);
	OmnString tempfilename = config->getAttrStr(AOSCONFIG_DOC_FILENAME);
	tempfilename << "_tmp";
	sgTmpObjMgr.init(dirname, tempfilename); 	

	mShowLog = config->getAttrBool(AOSCONFIG_SERVER_SHOWLOG, false);
	mMaxObjidTries = config->getAttrInt(AOSCONFIG_MAX_OBJID_TRIES, eDftMaxObjidTries);
	return true;
}


bool
AosDocServer::stop()
{
	//mIsStopping = true;
	//AosVersionServer::getSelf()->stop();

	if (mThread)
	{
		AOSLMTR_ENTER(mLockMonitor);
		AOSLMTR_LOCK(mLockMonitor);
		mCondVar->signal();
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		
		if (mShowLog) OmnScreen << "DocServer Thread is stopping" << endl;
		mThread->stop();
	}
	mThread = 0;
    return true;
}


/*
void
AosDocServer::setShowLog(const bool b)
{
	mShowLog = b;
}


u64
AosDocServer::doesObjidExist(
		const OmnString &siteid,
		const OmnString &objid, 
		const AosRundataPtr &rdata)
{
	// Objids are listed by the IIL whose name is created by 
	// If the IIL: AosIILName::composeObjidListingName(siteid) contains the entry
	// for 'objid', the objid exists. 
	//
	// IMPORTANT: This function assumes that the class has been locked.
	//
	if (objid == "") return AOS_INVDID;
	OmnString iilname = AosIILName::composeObjidListingName(siteid);
	aos_assert_r(iilname != "", AOS_INVDID);

	u64 docid;
	bool isunique;
	bool rslt = AosIILClient::getSelf()->getDocid(iilname, objid, docid, isunique, rdata);
	if (!rslt) return AOS_INVDID;
	return docid;
}
*/


/*
bool 
AosDocServer::bindObjid(
		OmnString &objid, 
		u64 &docid,
		bool &objid_changed,
		const bool resolve,
		const bool keepDocid,
		const AosRundataPtr &rdata)
{
	// This function binds 'objid' with 'docid'. If the objid is already used by someone
	// else, 'resolve' tells whether to resolve the objid. If 'resolve' is true, it
	// will resolve the objid by appending "(nnn)", where nnn is a number that makes 
	// the objid unique.
	
	OmnString siteid = rdata->getSiteid();
	aos_assert_rr(siteid != "", rdata, false);

	if (!keepDocid) 
	{
		if (docid)
		{
			OmnAlarm << "To bind an objid but docid is not null: " << docid << enderr;
		}
		docid = AosDocidMgr::getSelf()->nextDocid(rdata);
	}

	aos_assert_rr(docid, rdata, false);
	objid_changed = false;
	if (objid == "")
	{
		if (!resolve)
		{
			rdata->setError() << "Objid is empty";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		objid = AosObjid::createDftObjid(siteid, docid);
		objid_changed = true;
	}

	if (!AosObjid::isValidObjid(objid, rdata))
	{
		rdata->setError() << "Objid incorrect: " << objid << ":" << rdata->getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		if (!resolve) return false;
		objid = AosObjid::createDftObjid(siteid, docid);
		objid_changed = true;
	}

	bool changed = false;
	bool rslt = AosIILClient::getSelf()->bindObjid(objid, docid, changed, resolve, rdata);
	if (!rslt) 
	{
		OmnAlarm << "Failed binding objid: " << rdata->getErrmsg() << enderr;
		return false;
	}

	if (mShowLog) OmnScreen << "Bind objid: " << objid << ":" << docid << endl;
	objid_changed |= changed;
	rdata->setOk();
	return true;
}

bool
AosDocServer::bindCloudid(
		const OmnString &cid, 
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	// Bind the objid and docid. 
	aos_assert_rr(cid != "", rdata, false);
	if (mShowLog) OmnScreen << "Bind Cloudid: " << cid << ":" << docid << endl;
	bool rslt = AosIILClient::getSelf()->bindCloudid(cid, docid, rdata);
	aos_assert_r(rslt, false);
	rdata->setOk();
	return true;
}


bool
AosDocServer::determinePublic(
		const AosXmlTagPtr &doc, 
		const AosXmlTagPtr &cmd,
		const OmnString &tcid,
		OmnString &objid)
{
	// This funciton determines whether it is to create a public or
	// private doc. 
	// 1. If AOSTAG_PUBLIC_DOC is "true", it is public.
	OmnString is_pubdata = doc->getAttrStr(AOSTAG_PUBLIC_DOC);

	if (is_pubdata != "")
	{
		doc->removeAttr(AOSTAG_PUBLIC_DOC, false, true);
	}

	if (is_pubdata == "true")
	{
		return true;
	}

	if (cmd && cmd->getAttrStr(AOSTAG_PUBLIC_DOC) == "true")
	{
	 	return true;
	}

	// Chen Ding, 10/26/2010
	// If it is an access record, it is public.
	if (doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD)
	{
		// It is an access record. Should be treated as public.
		return true;
	}

	if (!mIsRepairing)
	{
		OmnString prefix, cc;
		AosObjid::decomposeObjid(objid, prefix, cc);
		if (cc != tcid)
		{
			objid = AosObjid::compose(objid, tcid);
			doc->setAttr(AOSTAG_OBJID, objid);
		}
	}
	return false;
}

bool
AosDocServer::isCloudidBound(
		const OmnString &cid, 
		u64 &userid, 
		const AosRundataPtr &rdata)
{
	// It checks whether the cloudid 'cid' is a "Real Cloudid". 
	// A cloud id is "Real" if it was bound to a user account.
	aos_assert_r(cid != "", false);
	OmnString iilname = AosIILName::composeCloudidListingName(rdata->getSiteid());
	bool isunique = false;
	bool rslt = AosIILClient::getSelf()->getDocid(iilname, 
			cid, userid, isunique, rdata);
	return (rslt && userid != AOS_INVDID && isunique);
}

bool
AosDocServer::checkModifying(
		const AosXmlTagPtr &origdoc, 
		const AosXmlTagPtr &newdoc,
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(origdoc, rdata, false);
	aos_assert_rr(newdoc, rdata, false);
	newdoc->setAttr(AOSTAG_VERSION, origdoc->getAttrStr(AOSTAG_VERSION));
	newdoc->setAttr(AOSTAG_CTIME, origdoc->getAttrStr(AOSTAG_CTIME));
	newdoc->setAttr(AOSTAG_CT_EPOCH, origdoc->getAttrStr(AOSTAG_CT_EPOCH));
	newdoc->setAttr(AOSTAG_MTIME, OmnGetTime(AosLocale::getDftLocale()));
	newdoc->setAttr(AOSTAG_MT_EPOCH, (u32)OmnGetSecond());
	newdoc->setAttr(AOSTAG_MODUSER, cid);
	newdoc->setAttr(AOSTAG_SITEID, origdoc->getAttrStr(AOSTAG_SITEID));

	//Zky3127, Linda, 2011/03/30
	OmnString siteid = origdoc->getAttrStr(AOSTAG_SITEID);
	aos_assert_r(siteid !="", false);
	aos_assert_r(cid!="", false);
	u64 userid = getDocidByCloudid(siteid, cid, rdata);
//u64 userid = rdata->getUserid();
	aos_assert_r(userid, false);

	//super user, it allows changing creator.
	bool rslt = AosSecurityMgrObj::getSecurityMgr()->isRoot(userid, rdata);
	if (!rslt)
	{
		newdoc->setAttr(AOSTAG_CREATOR, origdoc->getAttrStr(AOSTAG_CREATOR));
	}

	// If newdoc does not have otype, use the old one
	if (newdoc->getAttrStr(AOSTAG_OTYPE) == "") 
	{
		newdoc->setAttr(AOSTAG_OTYPE, origdoc->getAttrStr(AOSTAG_OTYPE));
	}

	// If newdoc does not have stype, use the old one
	if (newdoc->getAttrStr(AOSTAG_SUBTYPE) == "") 
	{
		newdoc->setAttr(AOSTAG_SUBTYPE, origdoc->getAttrStr(AOSTAG_SUBTYPE));
	}

	// All counters cannot be modified
	OmnString cc = origdoc->getAttrStr(AOSTAG_COUNTERCM);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERCM, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERDW);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERDW, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERLK);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERLK, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERRC);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERRC, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERUP);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERUP, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERRD);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERRD, cc);

	// All votes cannot be modified
	cc = origdoc->getAttrStr(AOSTAG_VOTEDOWN);
	if (cc != "") newdoc->setAttr(AOSTAG_VOTEDOWN, cc);

	cc = origdoc->getAttrStr(AOSTAG_VOTEUP);
	if (cc != "") newdoc->setAttr(AOSTAG_VOTEUP, cc);

	cc = origdoc->getAttrStr(AOSTAG_VOTETOTAL);
	if (cc != "") newdoc->setAttr(AOSTAG_VOTETOTAL, cc);

	return true;
}
*/


/*
bool
AosDocServer::checkCreatingNew(
		const OmnString &siteid,
		const OmnString &newobjid, 
		const bool rename, 
		const AosRundataPtr &rdata)
{
	// If 'docid' is null, we will check whether it has objid. If not, 
	// it is to create a new doc. If it has objid and the objid identifies
	// a doc, the operation is considered "bad". It is rejected. 
	if (newobjid == "")
	{
		// This means there are no docid, nor objid. Will treat
		// this as creating a new doc. 
		return true;
	}

	// The new doc has no docid but has objid. Check whether 'objid' can 
	// uniquely identify a doc. If not, it is to create a new doc. 
	// Otherwise, it is rejected. 
	OmnString iname = AosIILName::composeObjidListingName(siteid);
	bool isunique;
	u64 docid;
	bool rslt = AosIILClientSelf->getDocidByObjid(iname, newobjid, docid, isunique, rdata);
	if (!rslt || docid == AOS_INVDID) 
	{
		// (siteid, newobjid) does not identify a doc. If 'rename' is
		// 'create', will create the doc. Otherwise, it is an error.
		if (rename)
		{
			return true;
		}
	}

	return false;
}
*/

bool
AosDocServer::checkCtnrMember1(
			const OmnString &event,
			const AosRundataPtr &rdata)
{
	if(AosDocServer::getSelf()->mIsRepairing) return true;
	//TSK0070, Linda, 2011/05/12  Container Member Verifications
	OmnString sobjid;
	OmnString siteid = rdata->getSiteid();
	aos_assert_rr(siteid != "", rdata, false);
	/*
	AosXmlTagPtr cmd = rdata->getCmd();
	if (cmd)
	{
		sobjid = cmd->getAttrStr(AOSTAG_SDOCOBJID);

		if (sobjid != "")
		{
			AosSMDMgr::procSmartdocs(sobjid, rdata, event);
			if (!rdata->isOk()) return false;
		}
	}

	//config_norm.txt <sysEventSmart syseventsobjid = "xxxx"/>
	sobjid = rdata->getSysSdocEventObjid();
	if (sobjid != "")
	{
		AosSMDMgr::procSmartdocs(sobjid, rdata, event);
		if (!rdata->isOk()) return false;	
	}
	*/
	
	AosXmlTagPtr doc = rdata->getWorkingDoc();
	aos_assert_rr(doc, rdata, false);
	
	//<Event>
	//<EVENT_CTNR_ADDMEMBER>
	//<OBJID>XXXX</OBJID>
	//<OBJID>XXXX</OBJID>
	//....
	//</EVENT_CTNR_ADDMEMBER>
	//...
	//</Event>
	OmnString parent_objid = doc->getAttrStr(AOSTAG_PARENTC);

	if (parent_objid == "")
	{
		// There is only one doc that does not have parent container. 
		if (doc->getAttrStr(AOSTAG_OBJID) == AosObjid::composeSysRootAcctObjid(siteid))
		{
			rdata->setOk();
			return true;
		}

		rdata->setError() << "Missing parent container: " << doc->getAttrStr(AOSTAG_OBJID);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnStrParser1 parser(parent_objid, ",");
	OmnString ctnr_objid;
	while((ctnr_objid = parser.nextWord())!= "")
	{	
		AosXmlTagPtr parent_doc = AosDocClient::getSelf()->getDocByObjid(ctnr_objid, rdata); 
		if (!parent_doc) 
		{
			OmnAlarm << "Failed retrieving the parent doc: " 
				<< ctnr_objid << enderr;
			continue;
		}

		AosXmlTagPtr childtag = parent_doc->getFirstChild("Event");
		if (!childtag) 
		{
			continue;
		}

		AosXmlTagPtr tag = childtag->getFirstChild(event);
		if (!tag) 
		{
			continue;
		}
	
		// 'tag' should be in the form:
		// 	<event_id ...>
		// 		<tag ...>sdocobji</tag>
		// 		<tag ...>sdocobji</tag>
		// 		...
		// 	</event_id>
		OmnString sobjids;
		AosXmlTagPtr objidtag = tag->getFirstChild();
		while(objidtag)
		{
			sobjid = objidtag->getNodeText();
			if (sobjid != "")
			{
				if (sobjids == "") sobjids << sobjid;
				else sobjids << "," << sobjid;
			}
			objidtag = tag->getNextChild();
		}

		rdata->setEvent(event);
		bool rslt = AosSMDMgr::procSmartdocs(sobjids, rdata);
		if (!rslt) return false;
	}
	return true;
}


bool	
AosDocServer::saveDocASC(
		u32 &seqno, 
		u64 &offset, 
		u64 &docsize, 
		const OmnString &str)
{
	return sgTmpObjMgr.saveDocASC(seqno, offset, docsize, str);
}


AosXmlTagPtr
AosDocServer::getTempDoc(const u32 seqno, const u64 &offset) 
{
	return sgTmpObjMgr.readDocASC(seqno, offset);
}

#endif 

#if 0
bool
AosDocServer::addTags(
		const OmnString &tags, 
		const OmnString &objid, 
		const u64 &docid)
{
	// PERFORMANCE ISSUE, Chen Ding, 02/28/2010
	if (mIsStopping)
	{
		OmnAlarm << "Server is stopping!" << enderr;
		return false;
	}

	OmnString foundtags[eMaxTags];
	int num = 0;

	if (tags == "") return true;
	
	// Tags are strings of words separated by commas
	OmnStrParser1 parser(tags, ", ", false, false);
	OmnString tag, word;
	while ((tag = parser.nextWord()) != "")		
	{
		// Check whether it is a duplicated tag
		bool found = false;
		for (int i=0; i<num; i++)
		{
			if (foundtags[i] == tag)
			{
				found = true;
				break;
			}
		}
		if (found) continue;

		if (num < eMaxTags) foundtags[num++] = tag;

		word = AosIILName::composeTagIILName(tag);
		if (mShowLog) OmnScreen << "Add value doc: " << word 
			 << ":" << objid << ":" << docid << endl;
		AosIILClientSelf->addValueDoc(word, objid, docid, false, false);
	}
	return true;
}


bool
AosDocServer::removeTags(
		const OmnString &tags, 
		const OmnString &objid, 
		const u64 &docid)
{
	// PERFORMANCE ISSUE, Chen Ding, 02/28/2010
	if (mIsStopping)
	{
		OmnAlarm << "Server is stopping!" << enderr;
		return false;
	}

	OmnString foundtags[eMaxTags];
	int num = 0;

	if (tags == "") return true;
	
	// Tags are strings of words separated by commas
	OmnStrParser1 parser(tags, ", ", false, false);
	OmnString tag, word;
	while ((tag = parser.nextWord()) != "")		
	{
		// Check whether it is a duplicated tag
		bool found = false;
		for (int i=0; i<num; i++)
		{
			if (foundtags[i] == tag)
			{
				found = true;
				break;
			}
		}

		if (found) continue;
		if (num < eMaxTags) foundtags[num++] = tag;

		word = AosIILName::composeTagIILName(tag);
		if (mShowLog) OmnScreen << "Remove value doc: " << word 
			 << ":" << objid << ":" << docid << endl;
		AosIILClientSelf->removeValueDoc(word, objid, docid);
	}
	return true;
}


bool
AosDocServer::cleanValue(
		const AosXmlTagPtr &doc, 
		const OmnString &aname) 
{
	aos_assert_r(doc, 0);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_r(docid != AOS_INVDID, false);

	OmnString initvalue = 0;
	OmnString oldvalue = doc->getAttrStr(aname, initvalue);

	if (oldvalue =="") 
	{
		return true;
	}
	OmnString newvalue;
	newvalue <<0;

	if (oldvalue == newvalue) 
	{
		return true;
	}

	OmnString iilname = AosIILName::composeAttrIILName(aname);

	OmnString errmsg;
	bool isunique;
	u64 valuedoc;
	//Zky2248,Linda, 01/13/2011
	aos_assert_r(aname!="", false);
	AosIILClient::getSelf()->getDocid(sgRdata, iilname, aname, valuedoc, isunique);

	if(valuedoc ==0) 
	{
		return false; 
	}

	if (mShowLog) OmnScreen << "ModifyValue: " << iilname << ":" << oldvalue
		<< ":" << newvalue << ":" << docid << endl;
	AosIILClientSelf->modifyValueDoc1(iilname, oldvalue, newvalue, false, true, docid);
	doc->setAttr(aname, newvalue);

	AosXmlDoc::saveToFile(docid, doc);
	return true;
}



void
AosDocServer::createTempDoc(
		const AosXmlTagPtr &doc,
		const OmnString &log, 
		OmnString &errmsg)
{
	// It saves the temporary doc into a file and creates a new objid
	// for the doc. The new objid is:
	// 	
	aos_assert(doc);
	OmnString tmpObjid = doc->getAttrStr(AOSTAG_OBJID, "");
	u32 seqno;
	u64 offset; 
	OmnString str;
	str << log << "\n" << doc->toString().data()<< "\n";
	u64 docsize = str.length();
	saveDocASC(seqno, offset, docsize, str);
	
	OmnString objid = AosObjid::composeTempObjid(objid, seqno, offset);
	errmsg << ". A temporary doc is created for this doc. The objid is: " << objid;
}
#endif

