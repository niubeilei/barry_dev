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
// The torturer is in SengTorturer/TesterCreateObjNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CreateObj.h"

#include "Actions/ActSeqno.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SeReqProc/ResumeReq.h"
#include "SEModules/ImgProc.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/Docid.h"
#include "SEUtil/UrlMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosCreateObj::AosCreateObj(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATEOBJ, 
		AosSeReqid::eCreateObj, rflag)
{
}


bool 
AosCreateObj::proc(const AosRundataPtr &rdata)
{
	// It creates a new object. Note that in our system, there 
	// is no issue of re-creating an object because whenever
	// an object is created, a new object id (i.e., docid)
	// is assigned, regardless of whether the object is indeed
	// a new object.
	//
	// This function is the generic function that creates objects
	// of all kinds. If the creation of a specific type of objects
	// requires specific processing, it should use a different
	// function.
	//
	// 'root' should be in the form:
	// 	<request ...>
	// 		<command>
	// 			<cmd zky_saveas="true" 
	// 				res_objid="true|false"
	// 				owner="xxx"
	// 				opr="xxx" 
	// 				.../>
	// 		</command>
	// 		<objdef zky_type="xxx"
	// 				zky_name="xxx"
	// 				zky_tnail="xxx">
	// 			<zky_tag>xxx</zky_tag>
	// 			<zky_tagc>xxx</zky_tagc>
	// 			<fname>xxx</fname>
	// 		</objdef>
	// 	</request>
	//
	// If success, it returns the following:
	// 	<Contents objid="xxx"/>
	//
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();
	AosXmlTagPtr cmd = rootchild->getFirstChild("command");
	if (cmd)
	{
		cmd = cmd->getFirstChild();
	}

	OmnString errmsg;
	OmnString contents;

	if (AosSeReqProc::getIsStopping())
	{
		rdata->setError() << "Server is stopping";
		AOSLOG_LEAVE(rdata);
		return true;
	}

	OmnString cid = AosCloudidSvr::getCloudid(rdata->getUserid(), rdata);

	OmnString owner_cid = root->getChildTextByAttr("name", AOSTAG_OWNER_CID);
	if (owner_cid != "") cid = owner_cid;

	OmnString subopr = cmd->getAttrStr("subopr");
	AosXmlTagPtr xmldoc = rootchild->getFirstChild("objdef");
	if (!xmldoc)
	{
		errmsg = "Missing the object to be created";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr doc = xmldoc->getFirstChild();
	if (!doc)
	{
		errmsg = "Missing the object to be created(1)";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	if (subopr == "uploadImg")
	{
		AosImgProcPtr mImgProc = AosSeReqProc::getImgProc();
		if (!mImgProc) mImgProc = OmnNew AosImgProc();
		bool rslt = mImgProc->uploadImage(rdata, doc, cmd, contents);
		if(!rslt) 
		{
			AOSLOG_LEAVE(rdata);
			return false;
		}else
		{
			rdata->setResults(contents);
			rdata->setOk();
			AOSLOG_LEAVE(rdata);
			return true;
		}
	}

	//james 2011/01/21
	u64 origdid = 0;
	if (subopr == "createcomment")
	{
		//1. set comment doc.
		doc->setAttr(AOSTAG_HPCONTAINER, AOSTAG_CTNR_CMT);
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_CMT);
		//2. To obtain the comment doc.
		OmnString cmtdobjid = doc->getAttrStr(AOSTAG_CMTED_DOC, "");
		if (cmtdobjid == "")
		{
			errmsg = "Invalid commented object!";
			OmnAlarm << errmsg << enderr;
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		//3. get original doc, the AOSTAG_CMTED_DOC is the first doc.
		AosXmlTagPtr cmtdoc = AosDocClientObj::getDocClient()->getDocByObjid(cmtdobjid, rdata);
		if (!cmtdoc)
		{
			errmsg = "Invalid orignal comment";
			OmnAlarm << errmsg << enderr;
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		origdid = cmtdoc->getAttrU64(AOSTAG_ORIG_CMTED, 0);
		if (!origdid)
		{
			origdid = cmtdoc->getAttrU64(AOSTAG_DOCID, 0);
		}

		doc->setAttr(AOSTAG_ORIG_CMTED, origdid);
	}

	if (subopr == "creategic")
	{
		OmnString gic_type = doc->getAttrStr("gic_type","");
		if (gic_type == "")
		{
			errmsg = "Invalid Gic Type!";
			OmnAlarm << errmsg << enderr;
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		OmnString tagStr = doc->getAttrStr(AOSTAG_TAG, "");
		if (tagStr == "" )
			tagStr << "ztg_gic,ztg_" << gic_type;
		else
			tagStr << ",ztg_gic,ztg_" << gic_type;
		doc->setAttr(AOSTAG_TAG,tagStr);
		OmnString ctrn = doc->getAttrStr(AOSTAG_PARENTC, "");
		if (ctrn == "")
		{
			OmnString ctStr;
			ctStr << "zky_gics." << cid;
			doc->setAttr(AOSTAG_PARENTC, ctStr);
		}
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_GIC);
	
		AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDocByObjid("gic_seqno_default", rdata);
		if(!sdoc)
		{
			errmsg = "missing smartdoc!";
			OmnAlarm << errmsg << enderr;
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		OmnString seqno;
		bool rslt = AosActSeqno::getSeqno(seqno, sdoc, rdata);
		if(rslt)
		{
			doc->setAttr("gic_seqno", seqno);
		}
	}
	
	//Ketty server monitor 2011/02/25
	if(subopr == "reportProblem")
	{
		OmnString browserId_str;

		map<OmnString, OmnString> mRecvCookie;
		rdata->getCookies(mRecvCookie);
		map<OmnString, OmnString>::iterator iter = mRecvCookie.find(AOSTAG_BROWSERID);
		if(iter != mRecvCookie.end())
		{
			browserId_str = iter->second;
		}

		if(browserId_str == "")
		{
			errmsg = "miss BrowserId!";
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		doc->setAttr(AOSTAG_BROWSERID, browserId_str);
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_PROBLEMREPORT);
		doc->setAttr(AOSTAG_PARENTC, AOSCTNR_PROBLEMREPORTS);
		doc->setAttr(AOSTAG_PUBLIC_DOC, true);
		doc->setAttr(AOSTAG_CTNR_PUBLIC, true);
		doc->removeAttr(AOSTAG_OBJID);
		doc->removeAttr(AOSTAG_DOCID);
	}

	//felicia, createapp 2011/08/17
	if(subopr == "createapp")
	{
//		bool rt = AosWebAppMgr::createApp(doc, rdata);
//		aos_assert_r(rt, false);
	}

	// Check whether it is "save as"
	OmnString saveas = cmd->getAttrStr(AOSTAG_SAVEAS);
	if (saveas == "true")
	{
		// Chen Ding, 2011/02/10
		// There is a problem in OpenLaszlo implementations. 
		// It removed the docid before sending the doc here. 
		// This makes it impossible to check the copy right.
		// Will fix the problem. 
		//
		// Tmp: do not check
		// Check whether the user has the right to copy the object.
		// AosXmlTagPtr copied_doc = AosDocClient::getSemf()->getDoc(
		// 	doc->getAttrU64(AOSTAG_DOCID, 0), "");
		// if (!copied_doc)
		// {
		// 	// Somebody wanted to create a copy of a doc but the doc 
		// 	// was not there. This is not allowed because if someone wants 
		// 	// to copy a doc but the doc does not allow copying, if 
		// 	// the user removes the docid, there is no way for us to 
		// 	// check the access. 
		// 	errcode = eAosXmlInt_Denied;
		// 	errmsg = "Original doc not found!";
		// 	sendResp(req, errcode, errmsg, "");
		// 	return true;
		// }

		// Chen Ding, 2011/02/09
		// if (!AosSecurityMgrSelf->checkCopy(mSiteid, mAppid, mUserid, doc, errcode, errmsg))
		// if (!AosSecurityMgrSelf->checkCopy(mSiteid, 
		// 		mAppid, mUserid, copied_doc, errcode, errmsg))
		// {
		// 	sendResp(req, errcode, errmsg, "");
		// 	return true;
		// }

		doc->removeAttr(AOSTAG_DOCID);
		AosSeReqProc::prepareSaveas(doc);
	}

	
	//Linda Zky2750
	//Control save Number
	OmnString success;
	OmnString savenum = cmd->getAttrStr("ctrl_savenum");
	if (savenum == "true")
	{
		OmnString sobjid = cmd->getAttrStr("sdocobjid");
		if (sobjid != "")
		{
			OmnString ctnrobjid = cmd->getAttrStr(AOSTAG_CONTAINER);
			if (ctnrobjid == "")
			{
				//--Ketty
				OmnString errmsg = "Missing Container!";
				OmnAlarm << errmsg << enderr;
				rdata->setError() << errmsg;
				AOSLOG_LEAVE(rdata);
				return false;	
			}

			// Chen Ding, 06/05/2011
			rdata->setRetrievedDoc(root, true);
			rdata->setArg1(AOSARG_OBJID, ctnrobjid);
			AosSmartDocObj::procSmartdocsStatic(sobjid, rdata);
			if (!rdata->isOk())
			{
				AOSLOG_LEAVE(rdata);
				return false;
			}

			success <<"\" smsg =\"" << rdata->getResults();	
		}
	}
	//Zky3293, Linda, 2011/04/25
	//OmnString sobjids = cmd->getAttrStr(AOSTAG_SDOCOBJID);
	OmnString sobjids = cmd->getAttrStr(AOSTAG_PREV_SDOCOBJID);
	if (sobjids != "")
	{
		OmnStrParser1 parser(sobjids, ", ");
		OmnString sdocid;
		while((sdocid = parser.nextWord()) != "")
		{
			rdata->setReceivedDoc(root, true);
			AosSmartDocObj::procSmartdocsStatic(sdocid, rdata);
		}
		AosXmlTagPtr doc1 = rdata->getCreatedDoc();
		if (doc1)	doc = doc1;
	}

	bool resolve_objid = cmd->getAttrStr("res_objid") == "true";
	// bool resolve_objid = true;
	
	if (AosDocClientObj::getDocClient()->createDoc1(rdata, cmd, cid, doc, 
			resolve_objid, 0, 0, false))
	{
		OmnString sobjids = cmd->getAttrStr(AOSTAG_SDOCOBJID);
		if (sobjids != "")
		{
			OmnStrParser1 parser(sobjids, ", ");
			OmnString sdocid;

			OmnString zky_objid = doc->getAttrStr(AOSTAG_OBJID);
			OmnString zky_docid = doc->getAttrStr(AOSTAG_DOCID);
			rdata->setResults("");
			OmnString results;
			results << "<Contents " << AOSTAG_OBJID
				<< "=\"" << zky_objid
				<< "\" " << AOSTAG_DOCID << "=\"" << zky_docid << "\"/>";
			rdata->setResults(results);

			while((sdocid = parser.nextWord()) != "")
			{
				rdata->setReceivedDoc(doc, false);
				AosSmartDocObj::procSmartdocsStatic(sdocid, rdata);
			}
			rdata->setReceivedDoc(root, true);
		}
		else
		{
			contents = "<Contents ";
			contents << AOSTAG_DOCID << "=\"" << doc->getAttrStr(AOSTAG_DOCID)
					<< "\" " << AOSTAG_OBJID << "=\"" << doc->getAttrStr(AOSTAG_OBJID)
					<< success
					<< "\"/>";
			rdata->setOk();
			rdata->setResults(contents);
		}
		rdata->setOk();
	}
	AOSLOG_LEAVE(rdata);
	return true;
}

