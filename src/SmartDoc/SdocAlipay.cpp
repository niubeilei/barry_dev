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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocAlipay.h"
#include "SmartDoc/SMDMgr.h"

#include "AppMgr/App.h"
#include "Actions/ActSeqno.h"
#include "Rundata/RdataUtil.h"
#include "SEUtil/Ptrs.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/DocTags.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/String.h"
#include "SEUtilServer/UserDocMgr.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEServer/SeReqProc.h"
#include "Rundata/Rundata.h"
#include "SEModules/ObjMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

OmnString AosSdocAlipay::smAlipayUrl = "https://www.alipay.com/cooperate/gateway.do";
OmnString AosSdocAlipay::smAlipayId = "2088601017997217";
OmnString AosSdocAlipay::smAlipayKey = "spachz0gttdmow65gttq33il7kv6hwxh";
OmnString AosSdocAlipay::smSignType = "MD5";
OmnString AosSdocAlipay::smCharset = "UTF-8";
OmnString AosSdocAlipay::smReturnJsp = "http://112.25.131.18:7000/jsp/alipayReturn.jsp";
OmnString AosSdocAlipay::smNotifyJsp = "http://112.25.131.18:7000/jsp/alipayNotify.jsp";

AosSdocAlipay::AosSdocAlipay(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_ALIPAY, AosSdocId::eAlipay, flag)
{
	//config();
}


AosSdocAlipay::~AosSdocAlipay()
{
}


bool
AosSdocAlipay::config()
{
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert_r(config, false);

	AosXmlTagPtr alipayConfig = config->getFirstChild(AOSSDOCTYPE_ALIPAY);
	aos_assert_r(alipayConfig, false);

	smAlipayUrl = alipayConfig->getAttrStr("alipay_url", "https://www.alipay.com/cooperate/gateway.do");
	smAlipayId = alipayConfig->getAttrStr("alipay_pid");
	smAlipayKey = alipayConfig->getAttrStr("alipay_key");
	smSignType = alipayConfig->getAttrStr("sign_type");
	smCharset = alipayConfig->getAttrStr("charset");
	smReturnJsp = alipayConfig->getAttrStr("return_jsp");
	smNotifyJsp = alipayConfig->getAttrStr("notify_jsp");

	if (smAlipayUrl == "" || smAlipayId == "" ||
			smAlipayKey == "" || smSignType == "" || 
			smCharset == "" || smReturnJsp == "" ||
			smNotifyJsp == "")
	{
		OmnAlarm << "missing alipay ID or key!" << enderr;
		return false;
	}
	return true;
}


bool
AosSdocAlipay::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
 	// <smartdoc zky_sdoctp="alipay"> //this is sdoc type important!
	//		<pay trade_id="@trade_id" subjet="@subject" seller_email="@seller_email"
	//		  buyer_email="@buyer_email" ... />
	//		<sendGoods />
	// </smartdoc>
	//
	aos_assert_r(rdata, false);

	if (!sdoc)
	{
		AosSetError(rdata, "missing_sdoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr cmd = AosRdataUtil::getCommand(rdata);
	if(!cmd)
	{
		AosSetError(rdata, "missing_command");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString alipayOpr = cmd->getAttrStr(AOSTAG_ALIPAY_OPR);
	
	AosXmlTagPtr doc;
	if(alipayOpr == "alipaycb")
	{
		OmnString docid = cmd->getAttrStr(AOSTAG_DOCID, "");
		bool duplicated = false;
		doc = AosDocClientObj::getDocClient()->getDoc(rdata, docid, "", duplicated);
	}
	else
	{
		OmnString objid = cmd->getAttrStr("zky_order_objid", "");
		doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	}
	
	if(!doc)
	{
		AosSetError(rdata, "failed_ret_obj");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString serviceType = cmd->getAttrStr("service_type", "");
	if(alipayOpr == "login")	return login(doc);
	if(alipayOpr == "pay")		return pay(doc, serviceType, sdoc, rdata);
	//if(alipayOpr == "sendGoods") return sendGoods(doc);
	if(alipayOpr == "alipaycb")	return alipayCb(doc, sdoc, rdata);
	
	AosSetError(rdata, "unrecog_alipay_opr") << ": " << alipayOpr;
	return false;
}


bool
AosSdocAlipay::login(const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);

	// 1. the follow parms is needed by Alipay
	map<OmnString, OmnString> parm;
	parm.insert(make_pair("service", "user_authentication"));
	parm.insert(make_pair("partner", smAlipayId));
	parm.insert(make_pair("_input_charset", smCharset));
	parm.insert(make_pair("email", ""));
	parm.insert(make_pair("return_url", smReturnJsp));
	
	OmnString url = procParm(parm, 0);
	return true;
}


bool
AosSdocAlipay::pay(
		const AosXmlTagPtr &doc,
		const OmnString &serviceType,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(serviceType != "", false);
	if(!doc)
	{
		AosSetError(rdata, "missing_doc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	u64 docid= doc->getAttrU64(AOSTAG_DOCID, 0);
	OmnString objid= doc->getAttrStr(AOSTAG_OBJID, "");

	AosXmlTagPtr config = sdoc;

	OmnString returnJsp = smReturnJsp;
	returnJsp << "?zky_docid=" << docid;
	OmnString notifyJsp = smNotifyJsp;
	notifyJsp << "?zky_docid=" << docid;
	
	// 1. the follow parms is needed by Alipay
	map<OmnString, OmnString> parm;
	parm.insert(make_pair("partner", smAlipayId));
	parm.insert(make_pair("_input_charset", smCharset));
	parm.insert(make_pair("sign_type", smSignType));
	parm.insert(make_pair("notify_url", notifyJsp));
	//parm.insert(make_pair("notify_url", "http://yunyuyan.com:8080/lps-4.7.2/prod/jsp/testAlipay.jsp"));
	parm.insert(make_pair("return_url", returnJsp));
	
	OmnString tradeId = getValue(config, "tradeId_path", "", doc);
	if(tradeId == "")
	{
		AosSetError(rdata, "missing_trade_id");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("out_trade_no", tradeId));

	OmnString subject = getValue(config, "subject_path", "subject_default", doc);
	if(subject == "")
	{
		AosSetError(rdata, "missing_subject") << ": " << tradeId;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("subject", subject));

	bool rslt;
	if(serviceType == "dualFunction")
	{
		rslt = payDualFuncPriv(parm, config, doc, rdata);	
	}
	if(serviceType == "direct")
	{
		rslt = payDirectPriv(parm, config, doc, rdata);	
	}
	if(!rslt)
	{
		return false;
	}

	OmnString url = procParm(parm, docid);
	
	OmnString contents;
	contents << "<Contents ";
	contents << AOSTAG_DOCID << "=\"" << docid << "\" " 
			<< AOSTAG_OBJID << "=\"" << objid << "\" >"
			<< "<sendUrl><![CDATA[" << url << "]]></sendUrl>"
			<< "</Contents>";
	
	rdata->setResults(contents);
	rdata->setOk();

	// modify the doc, set the trade status	"WAIT_BUYER_PAY"
	OmnString tradeStatus_path = config->getAttrStr("tradeStatus_path", "trade_status");
	doc->setAttr(tradeStatus_path, "0");
	
	u32 siteid = rdata->getSiteid();
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, doc, "false", false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	if(!rslt)
	{
		AosSetError(rdata, "failed_mod_obj") << ": " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool
AosSdocAlipay::payDualFuncPriv(
		map<OmnString, OmnString> &parm,
		const AosXmlTagPtr &config,
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	parm.insert(make_pair("service", "trade_create_by_buyer"));

	OmnString paymentType = getValue(config, "paymentType_path", "paymentType_default", doc);
	if(paymentType == "")
	{
		AosSetError(rdata, "failed_payment_type");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("payment_type", paymentType));

	OmnString logisticsType = getValue(config, "logisticsType_path", "logisticsType_default", doc); 	
	if(logisticsType == "")
	{
		rdata->setError() << "Missing logisticsType(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("logistics_type", logisticsType));
	
	OmnString logisticsFee = getValue(config, "logisticsFee_path", "logisticsFee_default", doc);
	if(logisticsFee == "")
	{
		rdata->setError() << "Missing logisticsFee(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("logistics_fee", logisticsFee));
	
	OmnString logisticsPayMent = getValue(config, "logisticsPayment_path", "logisticsPayment_default", doc);
	if(logisticsPayMent == "")
	{
		rdata->setError() << "Missing logisticsPayMent(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("logistics_payment", logisticsPayMent));

	OmnString sellerEmail = getValue(config, "sellerEmail_path", "sellerEmail_default", doc);
	OmnString sellerId = getValue(config, "sellerId_path", "sellerId_default", doc);
	OmnString sellerAccountName = getValue(config, "sellerAccountName_path", "sellerAccountName_default", doc);
	if(sellerEmail == "" && sellerId == "" && sellerAccountName == "")
	{
		rdata->setError() << "Missing seller(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("seller_email", sellerEmail));
	parm.insert(make_pair("seller_id", sellerId));
	parm.insert(make_pair("seller_account_name", sellerAccountName));
	
	OmnString price = getValue(config, "price_path", "price_default", doc);
	if(price == "")
	{
		rdata->setError() << "Missing price(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("price", price));
	
	OmnString quantity = getValue(config, "quantity_path", "quantity_default", doc);
	if(quantity == "")
	{
		rdata->setError() << "Missing quantity(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("quantity", quantity));

	OmnString body = getValue(config, "body_path", "body_default", doc);
	parm.insert(make_pair("body", body));
	
	OmnString discount = getValue(config, "discount_path", "discount_default", doc);
	parm.insert(make_pair("discount", discount));
	
	OmnString totalFee = getValue(config, "totalFee_path", "totalFee_default", doc);
	parm.insert(make_pair("total_fee", totalFee));
	
	OmnString receiveName = getValue(config, "receiveName_path", "receiveName_default", doc);
	parm.insert(make_pair("receive_name", receiveName));
	
	OmnString receiveAddress = getValue(config, "receiveAddress_path", "receiveAddress_default", doc);
	parm.insert(make_pair("receive_address", receiveAddress));

	OmnString receiveZip = getValue(config, "receiveZip_path", "receiveZip_default", doc);
	parm.insert(make_pair("receive_zip", receiveZip));
	
	OmnString receivePhone = getValue(config, "receivePhone_path", "receivePhone_default", doc);
	parm.insert(make_pair("receive_phone", receivePhone));
	
	OmnString receiveMobile = getValue(config, "receiveMobile_path", "receiveMobile_default", doc);
	parm.insert(make_pair("receive_mobile", receiveMobile));
	
	return true;	
}


bool
AosSdocAlipay::payDirectPriv(
		map<OmnString, OmnString> &parm,
		const AosXmlTagPtr &config,
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	parm.insert(make_pair("service", "create_direct_pay_by_user"));

	OmnString paymentType = getValue(config, "paymentType_path", "paymentType_default", doc);
	if(paymentType == "")
	{
		rdata->setError() << "Missing paymentType(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("payment_type", paymentType));
	
	OmnString sellerEmail = getValue(config, "sellerEmail_path", "sellerEmail_default", doc);
	OmnString sellerId = getValue(config, "sellerId_path", "sellerId_default", doc);
	OmnString sellerAccountName = getValue(config, "sellerAccountName_path", "sellerAccountName_default", doc);
	if(sellerEmail == "" && sellerId == "" && sellerAccountName == "")
	{
		rdata->setError() << "Missing seller(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	parm.insert(make_pair("seller_email", sellerEmail));
	parm.insert(make_pair("seller_id", sellerId));
	parm.insert(make_pair("seller_account_name", sellerAccountName));

	
	OmnString price = getValue(config, "price_path", "price_default", doc);
	parm.insert(make_pair("price", price));
	
	OmnString quantity = getValue(config, "quantity_path", "quantity_default", doc);
	parm.insert(make_pair("quantity", quantity));
	
	OmnString body = getValue(config, "body_path", "body_default", doc);
	parm.insert(make_pair("body", body));

	OmnString totalFee = getValue(config, "totalFee_path", "totalFee_default", doc);
	parm.insert(make_pair("total_fee", totalFee));
	
	return true;
}


OmnString 
AosSdocAlipay::procParm(map<OmnString, OmnString> &parm, u64 docid)
{
	// 1.filter the parm
	parmFilter(parm);
	
	// 2. generate sign
	OmnString sign = buildSign(parm);
	
	// 3.generate the url-Str
	OmnString url = smAlipayUrl;
	url << "?";
	map<OmnString, OmnString>::iterator it;
	for(it=parm.begin(); it!=parm.end(); ++it)
	{
		url << it->first << "=" << it->second << "&";
	}
	url << "sign=" << sign << "&"
		<< "sign_type=" << smSignType; 
	return url;
}


void
AosSdocAlipay::parmFilter(map<OmnString, OmnString> &parm)
{
	// this Function will filter follow entry from the map:
	//     map value is empty,  map key is "sign" or "sign_type"
	//
	map<OmnString, OmnString>::iterator it;
	for(it=parm.begin(); it!=parm.end(); ++it)
	{
		OmnString key = it->first;
		OmnString value = it->second;
		if(value=="" || key=="sign" || key=="sign_type")
		{
			--it;
			parm.erase(key);
		}
	}
}


OmnString
AosSdocAlipay::buildSign(map<OmnString, OmnString> &parm)
{
	// sort and link the parm
	OmnString str = linkParmToStr(parm);
	str << smAlipayKey;
	//OmnString sign="sssss"; // = Md5Encrypt.md5(str);
	OmnString sign = AosMD5Encrypt(str);
	return sign;
}


OmnString
AosSdocAlipay::linkParmToStr(map<OmnString, OmnString> &parm)
{
	// sort the parm (map will sort by key)
	// link the parm to a string, format is
	//     "key=value&key=value"
	OmnString reqStr = "";
	map<OmnString, OmnString>::iterator it;
	for(it=parm.begin(); it!=parm.end(); ++it)
	{
		reqStr << it->first << "=" << it->second << "&";
	}
	reqStr.remove(reqStr.length()-1, 1);

	return reqStr;
}


bool
AosSdocAlipay::alipayCb(
		const AosXmlTagPtr &oldDoc,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr config = sdoc;
	
	// get the object
	AosXmlTagPtr root = rdata->getReceivedDoc();
	AosXmlTagPtr obj, alipayObj;

	if(!root || !(obj=root->getFirstChild("objdef")) || !(alipayObj=obj->getFirstChild()))
	{
		rdata->setError() << "Missing Object(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
		
	//set super user
	u32 siteid = oldDoc->getAttrU32(AOSTAG_SITEID, 0);
	rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));

	OmnString tradeId_alipay =	alipayObj->getAttrStr("out_trade_no", ""); 	 
	OmnString tradeId_path = config->getAttrStr("tradeId_path", "index");
	OmnString tradeId_oldDoc = oldDoc->getAttrStr(tradeId_path, "");
	if(tradeId_alipay != tradeId_oldDoc || tradeId_alipay == "")
	{
		rdata->setError() << "tradeId not match!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString tradeStatus_path = config->getAttrStr("tradeStatus_path", "trade_status");
	OmnString tradeStatus = oldDoc->getAttrStr(tradeStatus_path, "");
	bool rslt;
	if(tradeStatus == "0")	//WAIT_BUYER_PAY
	{
		rslt = payCb(alipayObj, oldDoc, sdoc, rdata);	
	}
	/*{
		rdata->setError() << "Unrecognized tradeStatus:" << tradeStatus;	
		return false;
	}*/
	
	aos_assert_r(rslt, false);
	
	// run the second smartDoc
	rdata->setReceivedDoc(oldDoc, true);
	OmnString post_sobjid = oldDoc->getAttrStr("zky_post_sdobjid", "");	
	if(post_sobjid == "") return true;

	// Chen Ding, 11/28/2012
	// AosSMDMgr::procSmartdocs(post_sobjid, rdata);
	AosSmartDocObj::procSmartdocsStatic(post_sobjid, rdata);
	return true;
}


bool
AosSdocAlipay::payCb(
		const AosXmlTagPtr &alipayObj,
		const AosXmlTagPtr &oldDoc,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(alipayObj, false);
	aos_assert_r(oldDoc, false);

	OmnString docid= oldDoc->getAttrStr(AOSTAG_DOCID, "");

	AosXmlTagPtr config = sdoc;
	

	OmnString tradeStatus_path = config->getAttrStr("tradeStatus_path", "trade_status");
	OmnString tradeStatus = alipayObj->getAttrStr("trade_status", "");
	if(tradeStatus != "" && tradeStatus_path != "") oldDoc->setAttr(tradeStatus_path, tradeStatus);

	u32 siteid = rdata->getSiteid();
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	bool rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, oldDoc, "false", false);
	rdata->setUserid(userid);
	rdata->setAppname(appname);
	if(!rslt)
	{
		rdata->setError() << "Modify obj error! by docid: " << docid << "(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	bool duplicated = false;
	AosXmlTagPtr newDoc = AosDocClientObj::getDocClient()->getDoc(rdata, docid, "", duplicated);
	if(!newDoc)
	{
		rdata->setError() << "Retrieve obj error! by docid: " << docid << "(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString contents = "<Contents>";
	contents << newDoc->toString();	
	contents << "</Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	return true;
}

