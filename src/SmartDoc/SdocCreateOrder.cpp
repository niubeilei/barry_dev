////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 11/17/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocCreateOrder.h"

#include "SEInterfaces/DocClientObj.h"
#include "UtilComm/TcpClient.h"
#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Actions/ActSeqno.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"

const OmnString sgDftSeparator = "_";

AosSdocCreateOrder::AosSdocCreateOrder(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_CREATE_ORDER, AosSdocId::eCreateOrder, flag)
{
}


AosSdocCreateOrder::~AosSdocCreateOrder()
{
}

bool
AosSdocCreateOrder::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function creates an order based on the input. The input
	// should be in the form:
	// 	<order ...>
	// 		<item .../>
	// 		<item .../>
	// 			...
	// 	</order>
	// It retrieves the items from the input, creates an object for
	// each of the items. It then creates an order based on the smartdoc
	// and the items. 
	//
	// 	<sdoc AOSOBJID_TYPE="xxx"
	// 		  AOSTAG_ITEM_OTYPE="xxx"
	// 		  AOSTAG_AMT_ANAME="xxx"
	// 		  AOSTAG_QUANTITY_ANAME="xxx"
	// 		  AOSTAG_OBJID_ANAME="xxx"
	// 		  AOSTAG_DOCID_ANAME="xxx"
	// 		  AOSTAG_ITEM_STYPE="xxx">
	// 		<AOSTAG_ITEM_CT_ACTIONS>
	// 			...
	// 		</AOSTAG_ITEM_CT_ACTIONS>
	// 		<AOSTAG_ORDER_DOC_ACTIONS>
	// 			...
	// 		</AOSTAG_ORDER_DOC_ACTIONS>
	// 		<order ...>
	// 			..template.
	// 		</order>
	// 	</sdoc>
	
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr root = rdata->getReceivedDoc();
	if (!root)
	{
		rdata->setError() << "Can't Retrieve Accessed Doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the items
	AosXmlTagPtr child = root->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Request incorrect!";
		return false;
	}

	//AosXmlTagPtr actiondefs = sdoc->getFirstChild(AOSTAG_ITEM_CT_ACTIONS);
	/*
	AosSdocActionPtr itmctac = AosSdocAction::createAction(actiondefs, errcode, errmsg);
	if(!itmctac)
	{
		errmsg << "Failed to create the action: " << actiondefs->toString();
		OmnAlarm << errmsg <<enderr;
		return false;
	}
	*/


	OmnString objid_type = sdoc->getAttrStr(AOSTAG_OBJID_TYPE);
	//zky_itmotp default  zky_odrtm
	OmnString otype = sdoc->getAttrStr(AOSTAG_ITEM_OTYPE, AOSOTYPE_ORDER_ITEM); 
	//zky_itmostp zky_orderitem
	OmnString stype = sdoc->getAttrStr(AOSTAG_ITEM_STYPE, AOSSTYPE_ORDER_ITEM); 
	OmnString amtaname = sdoc->getAttrStr(AOSTAG_AMT_ANAME, "totalmoney");
	OmnString qtyaname = sdoc->getAttrStr(AOSTAG_QUANTITY_ANAME, "totalnum");
	int total = 0;
	int quantity = 0;
	//u64 userid = rdata->getUserid();
	AosXmlTagPtr objdef = child->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Request incorrect!";
		return false;
	}

	AosXmlTagPtr records = objdef->getFirstChild();
	if (!records)
	{
		rdata->setError() << "Missing records!";
		return false;
	}

	AosXmlTagPtr record = records->getFirstChild();
	// Ketty for Alipay
	if(!record) record = records;

	while (record)
	{
		record->setAttr(AOSTAG_OTYPE, otype);
		record->setAttr(AOSTAG_SUBTYPE, stype);

		if (amtaname.length() > 0)
		{
			total += record->getAttrInt("totalprice", 0);
		}

		if (qtyaname.length() > 0)
		{
			quantity += record->getAttrInt("num", 0);
		}
		record = records->getNextChild();
	}

	// Time to create the order
	AosXmlTagPtr itemdocs;
	AosXmlTagPtr docheader;
	OmnString index;
	OmnString docstr;
	OmnString recObjid;
	recObjid = records->getAttrStr(AOSTAG_OBJID);
	//int ttl = 0;
	OmnString cid = rdata->getCid();
	records->setAttr(amtaname, total);
	records->setAttr(qtyaname, quantity);

	if (recObjid != "")
	{
		OmnString renameflag= "create";
		// Chen Ding, 2011/01/27
		// AosDocServerSelf->modifyObj(
		// 		      siteid, AOSTAG_APPNAME, userid, records, records, 0,
		// 			       renameflag, false, errcode, errmsg, ttl);
		//--Ketty
		//AosDocServerSelf->modifyObj(
		 //		      siteid, AOSTAG_APPNAME, userid, records, records,
		 //			       renameflag, false, errcode, errmsg, ttl);
		AosDocClientObj::getDocClient()->modifyObj(rdata, records, renameflag, false);
		OmnString contents = "<Contents>";
		contents << records->toString() << "</Contents>";
		rdata->setResults(contents);
		return true;	
	}
	
	// get req at first time, create obj
	rdata->setReceivedDoc(records, false);
	bool rslt = createOrderNum(sdoc, rdata);	// Ketty 2012/02/24
	aos_assert_r(rslt, false);
	OmnString userdata = root->getChildTextByAttr("name", "userdata");
	records->setAttr(AOSTAG_PARENTC, userdata);
	records->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	records->setAttr(AOSTAG_JXC_TAG, cid);
	
	/*docstr = records->toString();
	//--Ketty
	//AosXmlTagPtr order = AosDocServer::getSelf()->createDocSafe1(docstr, siteid, userid, cid, AOSAPPNAME_SYSTEM, "", 
	//								true, false, errcode, errmsg, false, false, false, true, ttl);
	order = AosDocServer::getSelf()->createDocSafe1(rdata, docstr, cid, "", 
									true, false, false, false, false, true, ttl);
	if (!order)
	{
		return false;
	}
	contents = "<Contents>";
	contents << order->toString() << "</Contents>";*/
	return true;
}


OmnString
AosSdocCreateOrder::createObjid(
		const OmnString &objid_type, 
		const AosXmlTagPtr &order,
		const AosXmlTagPtr &sdoc, 
		const AosXmlTagPtr &item,
		AosRundataPtr &rdata)
{
	// This function creates an objid for an order item based on 
	// 'objid_type', which supports the following:
	//		'a': 	the objid will be determined by the system.
	//		'b': 	The objid is the docid of the order + Separator + seqno
	//      'c':    The objid is the objid of the order + Separator + seqno
	//      'd':    The objid is the user cid + Separator + time + Separator + seqno
	//
	// 'Separator' is defined by the smartdoc:
	// 	<sdoc ...
	// 		AOSTAG_OBJID_SEP="xxx"
	// 		...
	// 	</sdoc>
	OmnString sep = sdoc->getAttrStr(AOSTAG_OBJID_SEP, sgDftSeparator);

	//default
	if (objid_type == "") return "";

	const char *data = objid_type.data();
	u64 seqno = 0;
	u64 docid = 0;
	bool rslt;
	OmnString cid = "";
	OmnString objid="";
	OmnString time = "";
	OmnString userObjid = "";
	AosSdocCreateOrder::E eObjid_type = AosSdocCreateOrder::toEnum(data[0]);
	switch (eObjid_type)
	{
	case eDocidPlusSeqno:
		 rslt = getSeqno(sdoc, seqno, rdata);
		 aos_assert_r(rslt, "");
		 docid = order->getAttrInt(AOSTAG_DOCID, 0);
		 userObjid << docid << sep << seqno;
		 return userObjid;

	case eObjidPlusSeqno:
		 rslt = getSeqno(sdoc, seqno, rdata);
		 aos_assert_r(rslt, "");
		 objid = order->getAttrStr(AOSTAG_OBJID);
		 userObjid << objid << sep << seqno;
		 return userObjid;

	case eCidPlusTimePlusSeqno:
		//OmnGetTime(session->getLocale());
		rslt = getSeqno(sdoc, seqno, rdata);
		aos_assert_r(rslt, "");
		userObjid << cid << sep << time << sep << seqno;
		return userObjid;

	default:
		return "";
	}
	return "";
}


bool
AosSdocCreateOrder::createOrderNum(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// Ketty 2012/02/24
	
	// There shall be a subtag:
	// 	<sdoc ...>
	// 		<AOSTAG_ORDERNUM_SEQNO_ACT .../>
	// 		...
	// 	</sdoc>
	
	AosXmlTagPtr actdef = sdoc->getFirstChild("seqno_act");
	if (!actdef)
	{
		OmnAlarm << "Missing action definition for generating order number!" << enderr;
		return false;
	}
	
	bool rslt = AosSdocAction::runAction(actdef, rdata);
	return rslt;
}


bool
AosSdocCreateOrder::getSeqno(
		const AosXmlTagPtr &sdoc,
		u64 &seqno,
		const AosRundataPtr &rdata)
{
	// There shall be a subtag:
	// 	<sdoc ...>
	// 		<AOSTAG_OBJID_SEQNO_ACT .../>
	// 		...
	// 	</sdoc>
	/*
	AosXmlTagPtr actdef = sdoc->getFirstChild(AOSTAG_OBJID_SEQNO_ACT);
	if (!actdef)
	{
		OmnAlarm << "Missing action definition for generating objid!" << enderr;
		return false;
	}
	*/
	
	// AosActSeqno seqno_action;
	// seqno = 0;
	OmnNotImplementedYet;
	//bool rslt = seqno_action.getSeqno(seqno, sdoc, rdata);
	//aos_assert_r(rslt, false);
	return false;
}

