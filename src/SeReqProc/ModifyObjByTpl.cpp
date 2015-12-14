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
#include "SeReqProc/ModifyObjByTpl.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "SEServer/SeReqProc.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosModifyObjByTpl::AosModifyObjByTpl(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_MODIFYOBJ_BYTPL, 
		AosSeReqid::eModifyObjByTpl, rflag)
{
}


bool 
AosModifyObjByTpl::proc(const AosRundataPtr &rdata)
{
	// This function is created by Linda 2010/11/15
	// This function tree on something. 'args' are in the form:
	//      "objidbd= objname, objsync="xxx:xxx|$$|xxx:xxx|$$|xxx:xxx""
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString contents;
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		rdata->setError() << "Missing args!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString objidbd, objsync;
	AosParseArgs(args, "objidbd", objidbd, "objsync", objsync);

	if (objidbd == "")
	{
		rdata->setError() << "Missing objidbd";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();
	if (!rootchild)
	{
		rdata->setError() << "Request incorrect!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Missing the object to be modify";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	AosXmlTagPtr obj1 = objdef->getFirstChild();
	if (!obj1)
	{
		rdata->setError() << "Missing the object";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString moobjid = obj1->getAttrStr(objidbd); 
	if (moobjid =="")
	{
		rdata->setError() << "Missing the objid,  objid: " << moobjid;
		AOSLOG_LEAVE(rdata);
		return false;

	}
	AosXmlTagPtr modoc = AosDocClientObj::getDocClient()->getDocByObjid(moobjid, rdata);
	if (!modoc)
	{
		rdata->setError() << "Failed to retrieve the doc to modify, objid: "<< moobjid;
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
		if (aname != objidbd && aname != AOSTAG_OBJID  && aname!= AOSTAG_DOCID)
		{
			modoc->setAttr(aname, avalue);
		}
	}
	//Linda 2010/11/19
	if (obj1->getAttrStr(AOSTAG_OTYPE)!="")
		modoc->setAttr(AOSTAG_OTYPE, obj1->getAttrStr(AOSTAG_OTYPE));

	OmnString parentc = obj1->getAttrStr(AOSTAG_PARENTC);
	if (parentc != "")
	{
		modoc->setAttr(AOSTAG_PARENTC, parentc);
		OmnString prefix2, cid2;
		AosObjid::decomposeObjid(parentc, prefix2, cid2);
		if (cid2 == "") modoc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	}

	if (objsync != "")
	{
		AosStrSplit parser(objsync, "|$$|");
		OmnString word;
		while ((word = parser.nextWord()) != "")
		{
			AosStrSplit split;
			bool finished;
			OmnString pair[2];
			split.splitStr(word.data(), ":", pair, 2, finished);
			if (pair[0] != "" && pair[1] != "")
			{
				OmnString value = obj1->getAttrStr(pair[0]);
				if (value != "") 
				{	
					modoc->setAttr(pair[1], value);
					if (pair[0] != AOSTAG_OBJID && pair[0]!= AOSTAG_DOCID) 
					{
						modoc->removeAttr(pair[0]);
					}
				}
			}
		}
	}

	//AosXmlDocPtr header;
	AosXmlTagPtr child1;
	OmnString renameflag = "false";
	AosDocClientObj::getDocClient()->modifyObj(rdata, modoc, renameflag, false);

	// Chen Ding, 2013/06/01
	// if (rdata->getErrcode() == eAosXmlInt_ObjidChanged)
	// {
	// 	contents = "<Contents ";
	// 	contents << AOSTAG_OBJID << "='" << child1->getAttrStr(AOSTAG_OBJID)
	// 		    << "'/>";
	// 	rdata->setResults(contents);
	// 	rdata->setOk();
	// }

	AOSLOG_LEAVE(rdata);
	return true;
}
