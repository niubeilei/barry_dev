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
// 05/15/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/GetAccessRecord.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "ErrorMgr/ErrmsgId.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "SEServer/SeReqProc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/AccessRcd.h"


AosGetAccessRecord::AosGetAccessRecord(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GET_ACCRCD, 
		AosSeReqid::eGetAccessRecord, rflag)
{
}


bool 
AosGetAccessRecord::proc(const AosRundataPtr &rdata)
{
	// This function retrieves the access record. If the record does not
	// exist, it will create one. The docid whose access record to be
	// retrieved is defined in the attribute: 
	//
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");

	OmnString docidstr, create_flag, parent, datasync;
	AosParseArgs(args, "docid", docidstr, "create", create_flag, "parent", parent, "datasync", datasync);
	if (docidstr == "") docidstr = args;
	if (docidstr == "")
	{
		rdata->setError() << "Missing docid!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	//felicia, 2012/04/12 for datasync
	bool datasync_flag = (datasync=="true");
	bool parent_flag = (parent!="false");
	if (datasync_flag)
	{
		OmnString docids[50];
		AosStrSplit split;
		bool finished;
		int num = split.splitStr(docidstr.data(), "/", docids, 50, finished);
		AosAccessRcdPtr arcd;
		AosXmlTagPtr arcd_doc;
		OmnString contents = "<Contents>";
		for (int i=0; i<num; i++)
		{
			u64 docid = atoll(docids[i].data());
			arcd = AosGetAccessRcd(rdata, 0, docid, "", false); 
			if (arcd)
			{
				arcd_doc = arcd->getDoc();
				if (arcd_doc)
				{
					contents << (char *)arcd_doc->getData();	
				}
			}

		}
		contents << "</Contents>";
		rdata->setResults(contents);
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;	
	}
	
	u64 userid = rdata->getUserid();
	u64 docid = atoll(docidstr.data());
	AosAccessRcdPtr arcd = AosGetAccessRcd(rdata, 0, docid, "", parent_flag); 

	AosXmlTagPtr arcd_doc;
	if (arcd)
	{
		arcd_doc = arcd->getDoc();
		if (arcd_doc)
		{
			// Check whether the access record is for the doc or for its parent. 
			u64 ddd = arcd->getOwnerDocid();
			if (ddd != docid)
			{
				// it is its parent's access record. Need to reset it.
				//Ice AdmDoc
				//OmnString oid = AosObjid::createArdObjid(docid);
				arcd_doc = arcd_doc->clone(AosMemoryCheckerArgsBegin);
				//arcd_doc->setAttr(AOSTAG_OBJID, oid);
				arcd_doc->removeAttr(AOSTAG_DOCID);
				arcd_doc->removeAttr(AOSTAG_CREATOR);
				arcd_doc->removeAttr(AOSTAG_CTIME);
				arcd_doc->removeAttr(AOSTAG_CT_EPOCH);
				arcd_doc->removeAttr(AOSTAG_MTIME);
				arcd_doc->removeAttr(AOSTAG_MT_EPOCH);
				arcd_doc->removeAttr(AOSTAG_MODUSER);
				arcd_doc->removeAttr(AOSTAG_CT_EPOCH);
				arcd_doc->setAttr(AOSTAG_OWNER_DOCID, docidstr);
			
				OmnString cid = AosDocClientObj::getDocClient()->getCloudid(userid, rdata);
				if (cid != "")
				{
					OmnString parent_objid = AosObjid::composeUserArcdHomeContainerObjid(cid);
					arcd_doc->setAttr(AOSTAG_PARENTC, parent_objid);
				}
			}
		}
	}
			
	// Check whether it is allowed
	OmnString contents = "<Contents>";
	if (arcd_doc)
	{
		if (!AosSecurityMgrObj::getSecurityMgr()->checkReadAcd(arcd_doc, rdata))
		{
			contents << "</Contents>";
			rdata->setResults(contents);
			AOSLOG_LEAVE(rdata);
			return false;
		}
		contents << (char *)arcd_doc->getData() << "</Contents>";
	}
	else
	{
		AosXmlTagPtr dd = AosRetrieveDocByDocid(docid, false, rdata);
		if (!dd)
		{
			AosSetError(rdata, "eDocidHasNoDoc") << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		if (!AosSecurityMgrObj::getSecurityMgr()->checkCreateAcd(dd, rdata))
		{
			contents << "</Contents>";
			rdata->setResults(contents);
			AOSLOG_LEAVE(rdata);
			return false;
		}

		if ((create_flag != "" && create_flag == "false"))
		{
			contents << "</Contents>";
			rdata->setResults(contents);
			rdata->setErrmsg("Access Record not found");
			AOSLOG_LEAVE(rdata);
			return false;
		}

		//Linda,AccessRecord 2011/01/05 start
		contents = "<Contents> ";
		OmnString docstr;
		//Ice AdmDoc
		//OmnString objid = AosObjid::createArdObjid(docid);
		//docstr<< "<arcd " << AOSTAG_OBJID << "=\"" << objid
		docstr<<"<arcd "<< AOSTAG_OWNER_DOCID << "=\"" << docidstr 
			<< "\" " << AOSTAG_OTYPE << "=\"" << AOSOTYPE_ACCESS_RCD 
			<< "\" " << AOSTAG_SITEID << "=\"" << rdata->getSiteid() 
			<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSDOCSTYPE_AUTO
			<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" << AOSACTP_PUBLIC
			<< "\" " << AOSTAG_DELMEM_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_ADDMEM_ACSTYPE << "=\"" << AOSACTP_PRIVATE 
			<< "\" " << AOSTAG_DELETE_ACSTYPE << "=\"" << AOSACTP_PRIVATE 
			<< "\" " << AOSTAG_COPY_ACSTYPE << "=\"" << AOSACTP_PRIVATE 
			<< "\" " << AOSTAG_CREATE_ACSTYPE << "=\"" << AOSACTP_PRIVATE 
			<< "\" " << AOSTAG_WRITE_ACSTYPE << "=\"" << AOSACTP_PRIVATE 
			<< "\"/>";

		//Ice AdmDoc
		//AosXmlTagPtr doc = AosDocServerSelf->createDocSafe1(rdata, docstr,
		//		 "", "", true, true, false, false, true, true, ttl);
		//aos_assert_r(doc, false);
		//contents <<doc->toString();
		AosXmlParser parser;
		AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
		aos_assert_r(doc, false);	
		bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(docid, doc, rdata); 
		aos_assert_r(rslt, false);
		AosAccessRcdPtr acdoc = AosGetAccessRcd(rdata, 0, docid, "", false);
		aos_assert_r(acdoc, false);
		aos_assert_r(acdoc->getAttrStr(AOSTAG_DOCID, "")!= "", false);
		AosXmlTagPtr ddd = acdoc->getDoc();
		aos_assert_r(ddd, false);
		contents << ddd->toString();
		contents << "</Contents>";
	}

	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

