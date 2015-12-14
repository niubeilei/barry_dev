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
// Modification History:
// 08/03/2011 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "RemoteConvertData/RemoteConvertData.h"

#include "SEUtil/IILName.h"
#include "MsgClient/MsgClient.h"
#include "SEModules/ObjMgr.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/AccessRcd.h"



OmnSingletonImpl(AosRemoteConvertDataSingleton, 
				 AosRemoteConvertData, 
				 AosRemoteConvertDataSelf, 
				 "AosRemoteConvertData");

AosDocClientObjPtr AosRemoteConvertData::smDocClient;
AosIILClientObjPtr AosRemoteConvertData::smIILClient;

AosRemoteConvertData::AosRemoteConvertData()
:
mLock(OmnNew OmnMutex()),
mRemotePort(-1),
mTransId(eInitTransId),
mCondVar(OmnNew OmnCondVar())
{
}


AosRemoteConvertData::~AosRemoteConvertData()
{
}


bool 
AosRemoteConvertData::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	for (int i=0; i<eMaxThreads; i++)
	{
		mThreads[i] = OmnNew OmnThread(thisPtr, "RemoteConvertDataThrd", 0, true, true, __FILE__, __LINE__);
		mThreads[i]->start();
	}
	return true;
}


bool 
AosRemoteConvertData::stop()
{
	return true;
}


bool
AosRemoteConvertData::config(const AosXmlTagPtr &config)
{
	//<servers>
	//	<server .../>
	//	<buildserver .../>
	//</servers>
	aos_assert_r(config, false);
	mRemoteAddr = config->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	mRemotePort = config->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
	mConn = OmnNew AosSEClient(mRemoteAddr, mRemotePort);
	return true;
}

bool
AosRemoteConvertData::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
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
		//OmnScreen << "To Send: " << req.mDoc->toString() << endl;
		OmnScreen << "To process: " << lsTotal++ << ":" << mRequests.size() << endl;
		mLock->unlock();

		switch(req.mType)
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
AosRemoteConvertData::signal(const int threadLogicId)
{
	return true;
}


bool
AosRemoteConvertData::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosRemoteConvertData::addReq(
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
AosRemoteConvertData::sendServerReq(
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
	req << "</request>";

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

bool
AosRemoteConvertData::sendCreateObjReq(
		const u32 siteid,
		const char *docstr)
{
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

bool
AosRemoteConvertData::ModifyAccessRcd(
		const u32 siteid,
		const AosXmlTagPtr &doc,
		const u64 &owndocid,
		const OmnString &ssid,
		const u64 &urldocid)
{
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
	req << "<item name=\"trans_id\">" << trans_id << "</item>"
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
AosRemoteConvertData::rebuildDb(
		const u64 &startDocid,
		const u64 &maxDocid,
		const AosRundataPtr &rdata)
{
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid!=0, rdata, false);

	u64 docid = 0;
	for(docid=startDocid; docid<=maxDocid; docid++)
	{
		AosXmlTagPtr doc = smDocClient->getDocByDocid(docid, rdata);
		if (!doc)
		{
			continue;
		}

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

		doc->normalize();

		convertDataForRebuild(doc);
		if (doc->getAttrStr(AOSTAG_OTYPE) == "zky_ssion")
		{
			OmnScreen << "------- Ignored (session): " << doc->toString() << endl;
			continue;
		}

		addReq(eCreateDoc, siteid, doc);

		AosAccessRcdPtr  arcd = smDocClient->getAccessRecord(docid, false, rdata);
		if (!arcd)
		{
			continue;
		}
		AosXmlTagPtr arcd_doc = arcd->getDoc();
		if (arcd_doc)
		{
			arcd_doc->removeAttr("zky_ver__a");
			arcd_doc->removeAttr("zky_ver__n");
			arcd_doc->removeAttr("zky_ver");
			addReq(eCreateArcd, siteid, arcd_doc);
		}
	}

	while (true)
	{
		if (!mRequests.empty()) continue;
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
		if (!mRequests.empty()) continue;
		break;
	}
	return true;
}


bool
AosRemoteConvertData::convertDataForRebuild(const AosXmlTagPtr &doc)
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
AosRemoteConvertData::cleanData(const AosXmlTagPtr &doc)
{
	// 1. Remove all the attributes "zky_imdir"
	doc->removeAttr("zky_imdir", true, false);

	// 2. Remove all the attributes 'zky_ver' that are not at the root level
	doc->removeSubAttr("zky_ver", true, false);
	return true;
}


