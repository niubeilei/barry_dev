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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/Forum.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "SEServer/SeReqProc.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosForum::AosForum(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_FORUM, AosSeReqid::eForum, rflag)
{
}


bool 
AosForum::proc(const AosRundataPtr &rdata)
{
	// This function is created by john 2010/12/16
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString errmsg;

	u64 userid = rdata->getUserid();
	AosXmlTagPtr xmldoc = root->getFirstChild("objdef");
	AosXmlTagPtr doc = xmldoc->getFirstChild();
	//OmnString cid = AosCloudidSvr::getCloudid(userid);
	OmnString opr = root->getChildTextByAttr("name", "args");
	if(opr == "create_forum")
	{
		//If AOSTAG_PUBLISHER is not specified, it defaults to the creator.
		OmnString owner_objid = doc->getAttrStr(AOSTAG_PUBLISHER);
		//check whether the owner is existed
		if(owner_objid	== "")
			doc->setAttr(AOSTAG_PUBLISHER,	userid);
		else
		{
			bool	duplicated = true;
			AosXmlTagPtr owner = AosDocClientObj::getDocClient()->getDoc(rdata, "", owner_objid, duplicated);
			if(!owner)
				doc->setAttr(AOSTAG_PUBLISHER,	userid);
			else
				doc->setAttr(AOSTAG_PUBLISHER,	owner_objid);
		}


		OmnString access = doc->getAttrStr(AOSTAG_ACCESS);
		OmnString ctrn = AOSTAG_CTNR_FORUMS;
		if(access == "private")
		{
			ctrn << "." << AosCloudidSvr::getCloudid(userid, rdata);
		}
		doc->setAttr(AOSTAG_PARENTC, ctrn);
			
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_CONTAINER);
		doc->setAttr(AOSTAG_STYPE, AOSSTYPE_FORUM);
	}

	if(opr == "create_topic")
	{
		// Chen Ding, 01/19/2012
		// Its parent container is its forum.
		// OmnString forum_objid = doc->getAttrStr(AOSTAG_FORUM);
		OmnString forum_objid = doc->getAttrStr(AOSTAG_PARENTC);
		if(forum_objid == "")
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		bool duplicated = true;
		AosXmlTagPtr forum = AosDocClientObj::getDocClient()->getDoc(rdata,"", forum_objid, duplicated);
		//aos_assert_r(forum, false);
		if(!forum)
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		//check the add-member access control of the forum 
		AosXmlTagPtr forum_doc = AosRetrieveDocByObjid(forum_objid, false, rdata);
		if (!forum_doc)
		{
			AosSetError(rdata, "eDocNotFound") << forum_objid;
			AOSLOG_LEAVE(rdata);		
			return false;
		}

		bool rslt = AosCheckCreateDoc(rdata, forum_doc, doc);
		//aos_assert_r(rslt, false);
		if(!rslt)
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		doc->setAttr(AOSTAG_PARENTC, forum_objid);
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_CONTAINER);
		doc->setAttr(AOSTAG_STYPE, AOSSTYPE_FORUM_TOPIC);
	}

	if(opr == "create_thread")
	{
		// Chen Ding, 01/19/2012
		// Its parent must be its topic
		// OmnString topic_objid = doc->getAttrStr(AOSTAG_TOPIC);
		OmnString topic_objid = doc->getAttrStr(AOSTAG_PARENTC);
		if(topic_objid == "")
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		OmnString forum_objid = doc->getAttrStr(AOSTAG_FORUM);
		//aos_assert_r(forum_objid, false);
		if(forum_objid == "")
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		bool duplicated = true;
		AosXmlTagPtr forum = AosDocClientObj::getDocClient()->getDoc(
				rdata, "", forum_objid, duplicated);
		if(!forum)
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		AosXmlTagPtr topic = AosDocClientObj::getDocClient()->getDoc(
				rdata, "", topic_objid, duplicated);
		//aos_assert_r(topic, false);
		if(!topic)
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		bool rslt = AosCheckCreateDoc(rdata, topic, doc);
		if(!rslt)
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		doc->setAttr(AOSTAG_PARENTC, topic_objid);
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_CONTAINER);
		doc->setAttr(AOSTAG_STYPE, AOSSTYPE_TOPIC_THRD);
	}

	if(opr == "create_message")
	{
		// Chen Ding, 01/19/2012
		// Its parent must be the thread
		// OmnString thrd_objid = doc->getAttrStr(AOSTAG_THREAD);
		OmnString thrd_objid = doc->getAttrStr(AOSTAG_PARENTC);
		if(thrd_objid == "")
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		OmnString topic_objid = doc->getAttrStr(AOSTAG_TOPIC);
		//aos_assert_r(topic_objid, false);
		if(topic_objid == "")
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		OmnString forum_objid = doc->getAttrStr(AOSTAG_FORUM);
		//aos_assert_r(forum_objid, false);
		if(forum_objid == "")
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}
		
		bool duplicated = true;
		AosXmlTagPtr forum = AosDocClientObj::getDocClient()->getDoc(
				rdata, "", forum_objid, duplicated);
		//aos_assert_r(forum, false);
		if(!forum)
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		AosXmlTagPtr topic = AosDocClientObj::getDocClient()->getDoc(
				rdata, "", topic_objid, duplicated);
		//aos_assert_r(topic, false);
		if(!topic)
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		AosXmlTagPtr thrd = AosDocClientObj::getDocClient()->getDoc(
				rdata, "", thrd_objid, duplicated);
		aos_assert_r(thrd, false);
		if(!thrd)
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		//check the add-member access control of the topic thread
		bool rslt = AosCheckCreateDoc(rdata, thrd, doc);
		if(!rslt)
		{
			AOSLOG_LEAVE(rdata);		
			return false;	
		}

		doc->setAttr(AOSTAG_PARENTC, thrd_objid);
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_MESSAGE);
		doc->setAttr(AOSTAG_STYPE, AOSSTYPE_FORUMMSG);
	}

	AosXmlDocPtr header;
	OmnString docstr = doc->toString();
	//don't understand
	AosXmlTagPtr newdoc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, docstr, "",
			        "", true, false,false, false, false, false);
	if(newdoc)
	{
		
		OmnString contents = "<Contents ";
		contents << AOSTAG_DOCID << "=\"" << newdoc->getAttrStr(AOSTAG_DOCID)
			<< "\" " << AOSTAG_OBJID << "=\"" << newdoc->getAttrStr(AOSTAG_OBJID)
			<< "\"/>";
		rdata->setResults(contents);
		rdata->setOk();
	}
	else
	{
		errmsg = "Internal error!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
	}
	AOSLOG_LEAVE(rdata);		
	return true;
}
