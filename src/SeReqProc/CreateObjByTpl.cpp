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
#include "SeReqProc/CreateObjByTpl.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosCreateObjByTpl::AosCreateObjByTpl(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATEOBJ_BYTPL, 
		AosSeReqid::eCreateObjByTpl, rflag)
{
}


bool 
AosCreateObjByTpl::proc(const AosRundataPtr &rdata)
{
	// This function is created by Linda 2010/11/15
	// This function tree on something. 'args' are in the form:
	//       "tplname= objname, objsync="xxx:xxx|$$|xxx:xxx|$$|xxx:xxx""
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString errmsg;
	OmnString contents;
	
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args =="")
	{
		errmsg = "Missing args!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString tplname, objsync;
	AosParseArgs(args, "tplname", tplname, "objsync", objsync);

	if (tplname == "")
	{
		errmsg = "Missing tpname";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();
	if (!rootchild)
	{
		errmsg = "Request incorrect!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	if (!objdef)
	{
		errmsg = "Missing the object to be created";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;

	}

	AosXmlTagPtr obj1 = objdef->getFirstChild();
	if (!obj1)
	{
		errmsg = "Missing the object to be created";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr tpl = AosDocClientObj::getDocClient()->getDocByObjid(tplname, rdata);
	if (!tpl)
	{
		errmsg = "Invalid commented object!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString aname, avalue;
	bool finished = false;
	OmnString path;
	obj1->resetAttrNameValueLoop();
	while(!finished)
	{
		if (!obj1->nextAttr(path, aname, avalue, finished, false)||finished) break;
		if (aname != AOSTAG_OBJID  && aname!= AOSTAG_DOCID)
		{
			tpl->setAttr(aname, avalue);
		}
	}

	if(objsync!="")
	{
		AosStrSplit parser(objsync, "|$$|");
		OmnString word;
		while ((word = parser.nextWord()) != "")
		{
			AosStrSplit split;
			bool finished;
			OmnString pair[2];
			split.splitStr(word.data(), ":", pair, 2, finished);
			if (pair[0]!="" && pair[1]!="")
			{
				OmnString value = obj1->getAttrStr(pair[0]);
				if (value != "")
				{	
					tpl->setAttr(pair[1], value);
					if (pair[0]!= AOSTAG_OBJID &&pair[0]!= AOSTAG_DOCID) tpl->removeAttr(pair[0]);
				}
			}
		}
	}

	OmnString type = obj1->getAttrStr(AOSTAG_OTYPE);
	if (type != "")  tpl->setAttr(AOSTAG_OTYPE, type);

	OmnString ctnrs = obj1->getAttrStr(AOSTAG_PARENTC);
	if (ctnrs!="") 
	{
		tpl->setAttr(AOSTAG_PARENTC, ctnrs);
		OmnString prefix2, cid2;
		AosObjid::decomposeObjid(ctnrs, prefix2, cid2);
		if (cid2 =="") tpl->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	}

	tpl->removeAttr(AOSTAG_DOCID, 1, true);
	tpl->removeAttr(AOSTAG_OBJID);
	tpl->removeAttr(AOSTAG_CREATOR);

	AosXmlTagPtr cmd;
	OmnString cid = AosCloudidSvr::getCloudid(rdata->getUserid(), rdata);
	bool resolve_objid = false;
	if (AosDocClientObj::getDocClient()->createDoc1(rdata,cmd, cid, tpl, resolve_objid, 0, 0, false))
	{
		/*
		//Linda,header,Error
		if (!header)
		{
			errmsg = "Internal error!";
			OmnAlarm << errmsg << enderr;
			errcode = eAosXmlInt_General;
			return true;
		}
		*/

		contents = "<Contents ";
		contents << AOSTAG_DOCID << "=\"" << tpl->getAttrStr(AOSTAG_DOCID)
			<< "\" " << AOSTAG_OBJID << "=\"" << tpl->getAttrStr(AOSTAG_OBJID)
			<< "\"/>";
		rdata->setOk();
		rdata->setResults(contents);
	}
	AOSLOG_LEAVE(rdata);
	return true;
}
