//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// HtmlComplier  
//
// Modification History:
// 05/18/2010: Created by Lynch Yang
// 2011/12/21: Modified By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "HtmlServer/HtmlReqProc.h"

#include "HtmlServer/HtmlRetrieveSites.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "AppVar/WebAppMgr.h"
#include "GICs/GICXmlGen.h"
#include "GICs/GIC.h"
#include "GICs/AllGics.h"
#include "HtmlModules/VpdChecker.h"
#include "HtmlModules/ClientPlatform.h"
#include "HtmlLayoutMgr/Layout.h"
#include "HtmlLayoutMgr/AllLayout.h"
#include "HtmlUtil/HtmlUtil.h"
#include "MultiLang/LangTermIds.h"
#include "MultiLang/LangDictMgr.h"
#include "Porting/TimeOfDay.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/XmlRc.h"
#include "Porting/GetTime.h"
#include "Util/Locale.h"

extern int gAosLogLevel;

static OmnMutex sgSimLock;
static OmnMutex sgPrintLock;
map<int, AosXmlTagPtr>	AosHtmlReqProc::mSimVpds;
map<int, AosXmlTagPtr>	AosHtmlReqProc::mPrintVpds;

int				AosHtmlReqProc::mSeqNum = 0;
OmnString		AosHtmlReqProc::mJsPath = "";
OmnString		AosHtmlReqProc::mImagePath = "";
OmnString		AosHtmlReqProc::mPrintPath = "";
OmnString		AosHtmlReqProc::mSimulatePath = "";
OmnString		AosHtmlReqProc::mSEServerName = "";
OmnString		AosHtmlReqProc::mHtmlServerName = "";
OmnString		AosHtmlReqProc::mAccessServerName = "";

AosHtmlConfigMgrPtr AosHtmlReqProc::mHtmlConfigMgr = OmnNew AosHtmlConfigMgr();

//ken 2011-5-17
OmnString 		AosHtmlReqProc::mDftGicVpd = "giccreators_default_html_h";
OmnString 		AosHtmlReqProc::mAccessDftGicVpd = "giccreators_access_denied_html_h";


AosHtmlReqProc::AosHtmlReqProc()
:
mIsStopping(false),
mCurrentId(0),
mEndId(0)
{
	AosNetReqProcPtr thisptr(this, false);
	mRundata = OmnApp::getRundata();
    aos_assert(mRundata);
    
	mRundata = mRundata->clone(AosMemoryCheckerArgsBegin);
	resetFileLoadFlags();
}


AosHtmlReqProc::~AosHtmlReqProc()
{
}


AosNetReqProcPtr
AosHtmlReqProc::clone()
{
	return OmnNew AosHtmlReqProc();
}


bool
AosHtmlReqProc::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	
	bool rslt = mHtmlConfigMgr->initConfig(config);
	aos_assert_r(rslt, false);
	
	mJsPath = mHtmlConfigMgr->getJsPath();
    mImagePath = mHtmlConfigMgr->getImagePath();
	mPrintPath = config->getAttrStr(AOSHTML_PRINTPATH);
	mSimulatePath = config->getAttrStr(AOSHTML_SIMULATEPATH);
	
	mSEServerName = config->getAttrStr(AOSHTML_SESERVERNAME);
	mHtmlServerName = config->getAttrStr(AOSHTML_HTMLSERVERNAME);
	mAccessServerName = config->getAttrStr(AOSHTML_ACCESSSERVERNAME);
	return true;
}


bool
AosHtmlReqProc::resetFileLoadFlags()
{
	memset(mFileLoadFlags, 0, eMaxFiles);
	mFileLoadNum = 0;
	return true;
}


bool
AosHtmlReqProc::stop()
{
	OmnScreen << "AosHtmlServer is stopping!" << endl;
	mIsStopping = false;
	return true;
}

bool			
AosHtmlReqProc::procRequest(const OmnConnBuffPtr &buff)
{
	u64 startSystemTime = OmnGetTimestamp();
	
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;
	bool rslt = resetRequest(buff, errcode, errmsg);
	if (!rslt)
	{
		sendResp(errcode, errmsg, "");
		return false;
	}

	AosHtmlCode code;
	switch (mOperation)
	{
	case AosHtmlOpr::eRetrieveFullVpd:
		 if(mSubOpr == "publish" || mSubOpr == "publish_demo")
		 {
		 	rslt = createFullVpd(code, errmsg);           // create full html page
		 }
		 else if(mSubOpr == "simulate" || mSubOpr == "simulate_demo")
		 {
			 rslt = createSimulateVpd(code, errmsg);       // create simualte page
		 }
		 else if(mSubOpr == "print" || mSubOpr == "print_demo")
		 {
			 rslt = createPrintVpd(code, errmsg);			// create print html page
		 }
		 else if(mSubOpr == "performance" || mSubOpr == "performance_demo")
		 {
			 rslt = performance(code, errmsg);
		 }
		 else if(mSubOpr == "gictester")
		 {
			 rslt = createGicTester(code, errmsg);
		 }
		 else
		 {
			 errmsg << "Unrecognized Operation:" << (int)mOperation;
			 rslt = false;
		 }
		 break;

	case AosHtmlOpr::eRetrieveVpd:
		 rslt = createPartVpd(code, errmsg);          	// create vpd page
		 break;

	case AosHtmlOpr::eRetrieveGic:
		 rslt = retrieveGicVpd(code, errmsg);    			// create gic
		 if (!rslt)
		 {
			 AosSetError(mRundata, "failed_ret_gic");
			 errcode = eAosXmlInt_General;
			 errmsg = mRundata->getErrmsg();
		 }
		 break;

	case AosHtmlOpr::eRetrieveCreator:
		 rslt = createCreator(code, errmsg); 			  	// create query editor page
		 break;

	case AosHtmlOpr::eRetrieveXmlCreator:
		 rslt = createXmlCreator(code, errmsg); 			  	// create query editor page
		 break;

	case AosHtmlOpr::eSimulate:
		 rslt = simulate(code, errmsg);      			  	// simulate editor vpd page
		 break;

	case AosHtmlOpr::ePrint:
		 rslt = print(code, errmsg);         			  	//  print editor vpd page
		 break;

	case AosHtmlOpr::eRetrieveSite:
		 rslt = retrieveAllSites(code, errmsg);		  	// retrieve site
		 break;

	case AosHtmlOpr::eServerError:
		 rslt = procServerError();
		 break;

	default:
		 errmsg << "Unrecognized Operation:" << (int)mOperation;
		 rslt = false;
		 break;
	}

	if (!rslt)
	{
		OmnAlarm << errmsg << enderr;
		sendResp(errcode, errmsg, "");
		return false;
	}
	
	sendHtml(mRetrieveType, code, startSystemTime);
	return true;
}


bool
AosHtmlReqProc::resetRequest(
		const OmnConnBuffPtr &buff,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	mLoginObj = 0;
	// mSsid = "";
	mReqInfo = 0;
	mReq = NULL;
	mRetrieveType = 0;

	//Ken 2011-5-23
	SETime = 0.0;

	//Ken 2011-4-9
	mVpdNum = 0;
	mVpdPath = "";
	mVpdArrayLength = 0;
	mVpdStructArray.clear();

	mXmlRoots.clear();
	resetFileLoadFlags();
	
	mSessionId = "";
	mIsInEditor = false;
	mIsHomePage = false;
	
	mPerformance = false;
	mIsGicTester = false;
	mTestVpdStr = "";
	mVpdRootHLevel = false;
	mVpdRootVLevel = false;

	//felicia, 07/27/2011
	mVpdChanged = true;	

	if (mIsStopping)
	{
		errmsg = "Server is stopping";
		errcode = eAosXmlInt_General;
		return false;
	}
	
	OmnTcpClientPtr conn = buff->getConn();
	if (!conn)
	{
		errmsg =  "Cann't connect to Server";
		errcode = eAosXmlInt_General;
		return false;
	}
	mReq = OmnNew AosWebRequest(conn, buff);
	char *data = mReq->getData();
	
	if (gAosLogLevel >= 2)
	{
		OmnCout << "\nTo process request: (transid: " << mReq->getTransId() << "):"
		   	<< "\n" << data << endl;
	}

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(buff, "" AosMemoryCheckerArgs);

	AosXmlTagPtr child;
	if (!root || !(child = root->getFirstChild()))
	{
		errmsg = "Failed to parse the request!";
		OmnAlarm << errmsg << enderr;
		OmnScreen << data << endl;
		return false;
	}
	
	mReqInfo = child;
	mXmlRoots.append(root);
	
	mUsername = mReqInfo->getChildTextByAttr("name", AOSTAG_USERNAME);
	mAppname = mReqInfo->getChildTextByAttr("name", AOSTAG_APPNAME);
	
	OmnString siteid_str = mReqInfo->getChildTextByAttr("name", AOSTAG_SITEID);

	if(siteid_str == "" || !siteid_str.isDigitStr())
	{
		mSiteid = 0;
	}
	else
	{
		mSiteid = atol(siteid_str.data());
	}
	
	mUrlDocDocid = mReqInfo->getChildU64ByAttr("name", AOSTAG_URL_DOCDID,0);

	OmnString loginInfoFlag = mReqInfo->getChildTextByAttr("name", "loginobj");
	mNeedLoginObj = (loginInfoFlag == "true") ? true : false;
	
	OmnString userAgent = mReqInfo->getChildTextByAttr("name", "userAgent");
	OmnString platform = mReqInfo->getChildTextByAttr("name", "platform");
	AosClientPlatform clientPlt(userAgent, platform);
	mIsTablet = clientPlt.isTablet();
	mIsTablet = false;
	if(mSiteid == 0) 
	{
		// Chen Ding, 06/06/2011
		mSiteid = 100;
		//modify by ken 2011/06/22
		//errmsg = "Missing Siteid!";
		//errcode = eAosXmlInt_General;
		//return false;
	}
	
	OmnString isInEditor = mReqInfo->getChildTextByAttr("name", AOSHTML_ISINEDITOR);
	mIsInEditor = (isInEditor == "true") ? true : false;
	
	OmnString opr = mReqInfo->getChildTextByAttr("name", AOSHTML_OPERATION);
	mOperation = AosHtmlOpr::toEnum(opr);
	mSubOpr = mReqInfo->getChildTextByAttr("name", "subOpr");
	
	mCookies = mReqInfo->getFirstChild("zky_cookies");
	if(mCookies)
	{
		mSessionId = mCookies->getChildTextByAttr("zky_name", "zky_ssid");
		
		// Chen Ding, 12/26/2011
		// AosXmlTagPtr cookie, cookie_save;
		// cookie = mCookies->getFirstChild("cookie");
		// while(cookie)
		// {
		// 	OmnString zky_name = cookie->getAttrStr("zky_name");
		// 	if (zky_name == "zky_ssid")
		// 	{
		// 		mSsid = cookie->getNodeText();
		// 		cookie_save = cookie;
		// 	}
		// 	cookie = mCookies->getNextChild();
		// }
		// if (mSsid != "")
		// {
		// 	AosSengAdmin::getSelf()->getLoginObj(mSiteid, mSsid, cookie_save, mLoginObj);
		// }
		if (mSessionId != "")
		{
		 	AosSengAdmin::getSelf()->getLoginObj(mSiteid, mSessionId, mUrlDocDocid, mLoginObj);
		}
	}
	
	OmnString ltype = mReqInfo->getChildTextByAttr("name", "languagetype");
	AosSengAdmin::getSelf()->getLanguageType(mSiteid, mSessionId, mUrlDocDocid, ltype, mLanguageType);

	//Ketty get brower width and height.
	OmnString p_width_str = mReqInfo->getChildTextByAttr("name","parentWidth");
	OmnString p_height_str = mReqInfo->getChildTextByAttr("name","parentHeight");
	mParentWidth = atoi(p_width_str.data());
	mParentHeight = atoi(p_height_str.data());
	OmnScreen << "mParentWidth:" << mParentWidth << "; mParentHeight:" << mParentHeight << endl;

	mRundata->reset();
	mRundata->setSiteid(mSiteid);
	mRundata->setSsid(mSessionId);
	
	//OmnString url= child->getChildTextByAttr("name", "url");
	//mApp = AosWebApp::getApp(url, mRundata);
	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


void
AosHtmlReqProc::sendResp(
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents)
{
	OmnString resp = "<status error=\"";
	if (errcode == eAosXmlInt_Ok)
	{
		resp << "false\" code=\"200\"/>";
	}
	else
	{
		resp << "true\" code=\"" << errcode << "\"><![CDATA[" << errmsg << "]]></status>";
	}

	if (contents != "")
	{
		resp << contents;
	}
	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send response: (transid: " << mReq->getTransId()
			<< "): \n" << resp << endl;
	}
	mReq->sendResponse(resp);
}


void
AosHtmlReqProc::sendHtml(
		const int type,
		const AosHtmlCode &code,
		const u64 startTime)
{
	u64 endSystemTime = OmnGetTimestamp();
	float spendTime = 0.0;
	if(endSystemTime >= startTime)
	{
		spendTime = (endSystemTime - startTime) / 1000.0;
		if (spendTime > 2000)
		{
			OmnAlarm << "Server proc timeout.! " 
				<< (u32)SETime << "," << (u32)spendTime << enderr;
		}
	}
	OmnString time;
	time << "SETime=\"" << SETime << "ms\" time=\"" << spendTime << "ms\""; 
	
	OmnString contents, loadJsFun, loadJsFun_vpd, loadJs;
	switch(type)
	{
	case AOSHTML_RETRIEVEFULL :
		 loadJs = mHtmlConfigMgr->getGicsLoadJs(mFileLoadFlags, mFileLoadNum, true);
		 loadJsFun = mHtmlConfigMgr->createLoadJsFun(mFileLoadNum);
		 loadJsFun_vpd = mHtmlConfigMgr->createLoadJsFun_vpd();

		 contents << code.mHtml << "<script " << time << " type='text/javascript'>";

		 if(mIsGicTester) contents << mTestVpdStr; 
		 
		 contents << "window.js_path=\"" << mJsPath <<"\";"
	              << "window.image_path=\"" << mImagePath <<"\";"
		 		  << "window.htmlserver_name=\"" << mHtmlServerName <<"\";"
	              << "window.seserver_name=\"" << mSEServerName <<"\";"
	              << "window.accessserver_name=\"" << mAccessServerName <<"\";"
	              << "window.urldoc_docid=\"" << mUrlDocDocid <<"\";"
				  << loadJsFun << loadJsFun_vpd << loadJs 
				  << "function AosStartApp(){" << code.mJs << "};"
                  << "</script>";
		 break;

	case AOSHTML_RETRIEVEVPD :
		 loadJs = mHtmlConfigMgr->getGicsLoadJs(mFileLoadFlags, mFileLoadNum, false);
		 
		 contents << "<status " << time << " error=\"false\" code=\"200\"/>"
		 		  << "$javascript$"
				  << "gAosVpdJSFiles[aos_vpdtransid] = new Array(); "
				  << "gAosLastStartedIdx[aos_vpdtransid] = 0; "
				  << "gAosTotalJSToLoad[aos_vpdtransid] = " << mFileLoadNum << ";"
				  << code.mJs;

		 if (mOperation == AosHtmlOpr::eRetrieveCreator)// || mOperation == AosHtmlOpr::eRetrieveXmlCreator)
		 {
         	contents << "var aoshtmlstr=" << code.mHtml << ";";
		 }
		 else
		 {
         	contents << "var aoshtmlstr='" << code.mHtml << "';";
		 }
			
		 contents << "var aosjson=" << code.mJson << "; "
			 	  << "var aoscssstr='" << code.mCss << "'; "
				  << "gDataService.setVpdData(aos_vpdtransid); "
				  << loadJs
				  << "AosAreAllJSLoaded(aos_vpdtransid); ";
         break;

	case AOSHTML_SIMULATE:
		 contents << "<status " << time << " error=\"false\" code=\"200\"/>"
				  << "<Contents>"
				  << "<url><![CDATA["<< code.mXml << "]]></url>"
				  << "</Contents>";
		 break;
		
	case AOSHTML_PRINT:
		 contents << "<status time=\"" <<spendTime << "ms\" error=\"false\" code=\"200\"/>"
				  << "<Contents>"
				  << "<url><![CDATA[" << code.mXml << "]]></url>"
				  << "</Contents>";
			break;

	case AOSHTML_RETRIEVESITES:
		 contents << "<status " << time << " error=\"false\" code=\"200\"/>"
			 	  << "<Contents>"
				  << code.mXml
				  << "</Contents>";
	default:
		 break;
	}

	//--Ketty 2011/03/21 set Cookie
	AosXmlTagPtr root = mXmlRoots[mXmlRoots.entries()-1];
	if(root)
	{
		AosXmlTagPtr cookies = root->getFirstChild(AOSTAG_COOKIES);
		if(cookies)
		{
			contents << cookies->toString();
		}
	}
	//--Ketty End

	contents.removeNonprintables();

	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send HTML: (transid: " << mReq->getTransId() << "): \n" << contents << endl;
	}
	
	mReq->sendResponse(contents);
}

	
bool
AosHtmlReqProc::addGic(const OmnString &type)
{
	if(type == "") return false;
   
	vector<int> vector = mHtmlConfigMgr->getFileIdVector(type);

	for(int i=0; i<(int)vector.size(); i++)
	{
		int fileid = vector[i];
		mFileLoadFlags[fileid] = true;
	}
	return true;
}

	
bool
AosHtmlReqProc::addGicAction(AosXmlTagPtr &vpd, const bool &isConvert)
{
	aos_assert_r(vpd, false);
	
	AosXmlTagPtr actions_vpd = isConvert ? vpd : vpd->getFirstChild("actions");
    if(actions_vpd)
    {
		AosXmlTagPtr action = actions_vpd->getFirstChild("action");
        while(action)
        {
          	OmnString act_type = action->getAttrStr("gic_type");
			addGic(act_type);
			action = actions_vpd->getNextChild();
        }
    }
    return true;
}

	
bool 
AosHtmlReqProc::createFullVpd(AosHtmlCode &code, OmnString &errmsg)
{
	OmnString url = mReqInfo->getChildTextByAttr("name", "url");
	OmnString full_url = mReqInfo->getChildTextByAttr("name", "userurl");
	OmnString query_str = mReqInfo->getChildTextByAttr("name", "query_str");
	if(url == "")
	{
		errmsg << "Missing url in request infor";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	
	mRetrieveType = AOSHTML_RETRIEVEFULL;
	mIsHomePage = true;
	
	bool rslt = false;
	AosXmlTagPtr vpdroot, vpd, obj;
	u64 startTime = OmnGetTimestamp();
	if(mNeedLoginObj)
	{
		rslt = AosSengAdmin::getSelf()->resolveUrl1(
				mSiteid, url, full_url, query_str, vpdroot, vpd, obj,
				mSessionId, mUrlDocDocid,mCookies, mLoginObj, mIsTablet);
	}
	else
	{
		rslt = AosSengAdmin::getSelf()->resolveUrl1(
				mSiteid, url, full_url, query_str, vpdroot, vpd, obj,
				mSessionId, mUrlDocDocid, mCookies, mIsTablet);
	}
	u64 endTime = OmnGetTimestamp();
	if(endTime >= startTime)
	{
		SETime += ((endTime - startTime)/1000.0);
	}
	
	if(!rslt || !vpd)
	{
		errmsg << "Cann't retrieve vpd by this url:" << url;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	mXmlRoots.append(vpdroot);
	//setVpdRootLevel(vpd);  // 07/29/2011 michael
	
	if(obj)
	{
		mXmlRoots.append(obj);
	}
	else
	{
		obj = vpd->getFirstChild("embedobj");
	}
		
	addGic("aos_base_start");
	return createHtml(vpd, obj, code, errmsg);
}

	
bool 
AosHtmlReqProc::createSimulateVpd(AosHtmlCode &code, OmnString &errmsg)
{
	OmnString url = mReqInfo->getChildTextByAttr("name", "url");
	if(url == "")
	{
		errmsg << "Missing simulate Url";
		return false;
	}
	
	mRetrieveType = AOSHTML_RETRIEVEFULL;
	mIsHomePage = true;

	AosXmlTagPtr vpd;
	bool rslt = getVpdFromMap(url, vpd);
	if(!rslt || !vpd)
	{
		errmsg << "Failed to get vpd by simulate url";
		return false;
	}
	AosXmlTagPtr obj = vpd->getFirstChild("embedobj");

	addGic("aos_base_start");
	return createHtml(vpd, obj, code, errmsg);
}


bool 
AosHtmlReqProc::getVpdFromMap(
	OmnString url,
	AosXmlTagPtr &vpd)
{
	OmnStrParser1 entry_parser(url, "_", false ,false);
	OmnString entry = entry_parser.nextWord();
	OmnString str;
	while (entry != "")
	{
		str = entry;
		entry = entry_parser.nextWord();
    }
	
	int len = str.length();
	OmnString kk = str.substr(0, len - 4);	
	int k = kk.toInt();

	sgSimLock.lock();
	AosXmlTagPtr pp = mSimVpds[k];
    int v = mSimVpds.erase(k);
	sgSimLock.unlock();

	if(v == 1)
		cout << "Delete Successful"<<endl;
	else 
		cout << "Delete Unsuccessful"<<endl;
	
	if(!pp) return false;
	vpd = pp;
	return true;		
}


bool 
AosHtmlReqProc::createPrintVpd(AosHtmlCode &code, OmnString &errmsg)
{
	OmnString url = mReqInfo->getChildTextByAttr("name", "url");
	if(url == "")
	{
		errmsg << "Missing print Url";
		return false;
	}
	
	mRetrieveType = AOSHTML_RETRIEVEFULL;
	mIsHomePage = true;
	
	AosXmlTagPtr vpd, obj;
	bool rslt = getVpdFromPrintMap(url, vpd, obj);
	if(!rslt)
	{
		errmsg << "Failed to get vpd by print url";
		return false;
	}
	addGic("aos_base_start");
	rslt = createHtml(vpd, obj, code, errmsg);
	if(!rslt)
	{
		OmnAlarm << "Failed to create Html!" << enderr;
		return false;
	}
	OmnString pFun = "setTimeout(\"window.print()\",1000);";
	code.mJs << pFun;
	return true;
}


bool 
AosHtmlReqProc::getVpdFromPrintMap(
	OmnString url,
	AosXmlTagPtr &vpd,
	AosXmlTagPtr &obj)
{
	OmnStrParser1 entry_parser(url, "_", false ,false);
	OmnString entry = entry_parser.nextWord();
	OmnString str;
	while (entry != "")
	{
		str = entry;
		entry = entry_parser.nextWord();
    }
	
	int len = str.length();
	OmnString kk = str.substr(0, len - 4);	
	int k = kk.toInt();

	sgPrintLock.lock();
	AosXmlTagPtr pp = mPrintVpds[k];
    int v = mPrintVpds.erase(k);
	sgPrintLock.unlock();

	if(v == 1)
		cout << "Delete Successful" << endl;
	else 
		cout << "Delete Unsuccessful" << endl;
	
	if(!pp) return false;
	vpd = pp;
	return true;		
}

	
bool
AosHtmlReqProc::createGicTester(AosHtmlCode &code, OmnString &errmsg)
{
	OmnString url = mReqInfo->getChildTextByAttr("name", "url");
	if(url == "")
	{
		errmsg << "Missing url in request infor";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	
	mRetrieveType = AOSHTML_RETRIEVEFULL;
	mIsHomePage = true;
	mIsGicTester = true;
	mIsInEditor = true;
	
	int pos = url.indexOf(0, '/', true);
	int pos2 = url.indexOf(0, '.', true);
	OmnString gic_type = url.substr(pos+1, pos2-1);

	if(gic_type == "")
	{
		errmsg << "missing gic_type:" << gic_type;
		return false;
	}
	
	if (gic_type == "random")
	{
		gic_type = AosGicXmlGen::getRandGicType();
	}

	addGic("aos_base_start");

	OmnString gicstr = AosGicXmlGen::getVpdXml(gic_type, mRundata);
	if(gicstr == "")
	{
		errmsg << "generate gicxml error";
		return false;	
	}

	OmnString type = "gictester_";
	type << gic_type;
	addGic(type);

	OmnString vpdstr;
	vpdstr << "<vpd container_width=\"1000\" container_height=\"1000\" container_width_def=\"fixed\" container_height_def=\"fixed\" ><gic_creators>" << gicstr << "</gic_creators><embedobj /></vpd>";

	AosXmlParser parser;
	AosXmlTagPtr vpd = parser.parse(vpdstr.data(), "" AosMemoryCheckerArgs);
	if(!vpd)
	{
		errmsg << "missing vpd:" << vpdstr;
		return false;
	}
	
	AosXmlTagPtr obj = vpd->getFirstChild("embedobj");
	
	mTestVpdStr = "";
	mTestVpdStr << "if(window.console){ console.log('" <<  vpdstr << "'); }"
				<< "window.onerror = function(errMsg, url, line){"
					<< "if(window.gDataService){"
						<< "var xml='" << vpdstr << "'; gDataService.serverError(errMsg, url, line, xml);"
					<< "} }; "
				<< "window.AosTimeOutSet = setTimeout(function(){"
					<< "if(gAosSystem.mWinObj){"
						<< "var panel = gAosSystem.mWinObj.findByType('aos_panel');"
						<< "if(panel && panel[0]){"
							<< "var gic = panel[0].items.items[0];"
							<< "if(gic){ AosCheckGicDomBase(gic); AosCheckGicDom(gic); }"
						<< "}"
		 			<< "}; location.reload();"
				<< "}, 5000);";
	
	return createHtml(vpd, obj, code, errmsg);
}


bool
AosHtmlReqProc::procServerError()
{
	OmnString errMsg = mReqInfo->getChildTextByAttr("name", "errMsg");
	OmnString url = mReqInfo->getChildTextByAttr("name", "url");
	OmnString line = mReqInfo->getChildTextByAttr("name", "line");
	OmnString xml = mReqInfo->getChildTextByAttr("name", "xml");
	OmnString time = mReqInfo->getChildTextByAttr("name", "time");

	OmnString str;
	str << "------------------------------------------------\n"
		<< "url : " << url << "\n"
		<< "line : " << line << "\n"
		<< "errMsg : " << errMsg << "\n"
		<< "time : " << time << "\n" 
		<< "xml : " << xml << "\n"
		<< "------------------------------------------------\n";

	static OmnFile mFile("errMsgFile.txt", OmnFile::eReadWrite);
	if (!mFile.isGood())
	{
		// The file has not been created yet. Create it.
		mFile.openFile(OmnFile::eCreate);
		aos_assert_r(mFile.isGood(), NULL);
	}
	mFile.append(str, true);
	return false;
}


bool
AosHtmlReqProc::performance(AosHtmlCode &code, OmnString &errmsg)
{
	OmnString url = mReqInfo->getChildTextByAttr("name", "url");
	if(url == "")
	{
		errmsg << "Missing url in request infor";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	
	mRetrieveType = AOSHTML_RETRIEVEFULL;
	mIsHomePage = true;
	mPerformance = true;
	mIsInEditor = true;
	
	int pos = url.indexOf(0, '/', true);
	int pos2 = url.indexOf(0, '.', true);
	OmnString vpdname = url.substr(pos+1, pos2-1);
	if(vpdname == "")
	{
		errmsg << "missing vpdname:" << vpdname ;
		return false;
	}
		
	AosXmlTagPtr vpd, vpdroot, obj;
	bool rslt = getVpd(vpdname, vpd, vpdroot);
	if(!rslt)
	{
		errmsg << "Failed to retrieve , parser response error";
		return false;
	}
	
	obj = vpd->getFirstChild("embedobj");

	rslt = convertVpdForPerformance(vpd);
	if(!rslt)
	{
		errmsg << "Cann't retrieve the vpd";
		OmnAlarm << errmsg << enderr;
		return false;
	}
		
	addGic("aos_base_start");
	return createHtml(vpd, obj, code, errmsg);
}

	
bool
AosHtmlReqProc::getVpd(
		OmnString &vpdname,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &root)
{
	if(vpdname == "" || (int)mVpdStructArray.size() > 10)
	{
		return false;
	}

	OmnString oldVpdname = vpdname;
	if (mIsTablet)
	{
		OmnString prefix, cloudid;
        AosObjid::decomposeObjid(vpdname, prefix, cloudid);
        if (cloudid == "")
        {
	        vpdname << "_pad";
	    }
	    else
	    {
            vpdname = prefix;
            vpdname << "_pad." << cloudid;
        }
	}
	
	OmnString isInEditor;
	isInEditor << mIsInEditor;

	u64 startTime = OmnGetTimestamp();
	root = AosSengAdmin::getSelf()->retrieveDoc(mSiteid, vpdname, mSessionId,mUrlDocDocid, 
			isInEditor, mCookies);
	u64 endTime = OmnGetTimestamp();
	if(endTime >= startTime)
	{
		SETime += ((endTime - startTime)/1000.0);
	}
	
	if(!root) return false;

	mXmlRoots.append(root);
	AosXmlTagPtr contents = root->getFirstChild("Contents");
	if(!contents) return false;

	vpd = contents->getFirstChild();
	if(vpd) return true;

	if(!mIsTablet) return false;

	root = AosSengAdmin::getSelf()->retrieveDoc(mSiteid, oldVpdname, mSessionId,mUrlDocDocid, 
			isInEditor, mCookies);
	if(!root) return false;

	mXmlRoots.append(root);	
	contents = root->getFirstChild("Contents");
	if(!contents) return false;
	             
	vpd = contents->getFirstChild();
	if (!vpd) return false;
	return true;
}

	
bool
AosHtmlReqProc::convertVpdForPerformance(AosXmlTagPtr &vpd)
{
	vpd->setAttr("container_width", 1000);
	vpd->setAttr("container_width_def", "fixed");
	vpd->setAttr("container_height", 10000);
	vpd->setAttr("container_height_def", "fixed");
	
	AosXmlTagPtr gic_creators = vpd->getFirstChild("gic_creators");
	aos_assert_r(gic_creators, false);
	
	AosXmlTagPtr gic = gic_creators->getFirstChild();
	aos_assert_r(gic, false);

	int gic_width = gic->getAttrInt("gic_width", 50);
	int gic_height = gic->getAttrInt("gic_height", 30);
	gic->setAttr("gic_x", 0);
	gic->setAttr("gic_y", 0);
	
	int num = vpd->getAttrInt("gic_num", 200);
	int i = 0, x = 0, y = 0;
	OmnString gicStr;
	while(num)
	{
		if(i==10)
		{
			i = 0;
			y += gic_height;
		}
		x = i * gic_width;
		
		AosXmlTagPtr gicClone = gic->clone(AosMemoryCheckerArgsBegin);
		
		gicClone->setAttr("gic_x", x);
		gicClone->setAttr("gic_y", y);

		gicStr << gicClone->toString();
		num--;
		i++;
	}
	string str(vpd->toString().data());
	int p1 = str.find("<gic_creators>");
	int p2 = str.find("</gic_creators>", 0);
	
	OmnString vpdStr;
	vpdStr << str.substr(0, p1+strlen("<gic_creators>")) << gicStr << str.substr(p2);

	AosXmlParser parser;
	vpd = parser.parse(vpdStr, "" AosMemoryCheckerArgs);
	return true;
}


bool
AosHtmlReqProc::retrieveGicVpd(AosHtmlCode &code, OmnString &errmsg)
{
	mRetrieveType = AOSHTML_RETRIEVEVPD;
	
	OmnString vpdid = mReqInfo->getChildTextByAttr("name", "objid");
	if(vpdid == "")
	{
		errmsg << "missing vpdid";
		return false;
	}
	
	AosXmlTagPtr vpd, vpdroot, obj;
	bool rslt = getVpd(vpdid, vpd, vpdroot);
	if(!rslt && vpdroot)
	{
		bool exist = false;
		OmnString errNum = vpdroot->xpathQuery("status/code", exist, "");
		OmnString dftvpdname = (errNum == "460") ? mAccessDftGicVpd : mDftGicVpd;
		rslt = getVpd(dftvpdname, vpd, vpdroot);
	}
	if(!rslt)
	{
		errmsg << "Failed to retrieve , parser response error";
		return false;
	}

	mVpdNum = 0;
	OmnString parentId;
	AosHtmlCode gicCode;
	rslt = createGic(gicCode, vpd, obj, parentId, -1, -1);
	
	OmnString vpdstr = vpd->toString();
	code.mJs << "if(!window.trans_id) window.trans_id=0; var soloid=10000+window.trans_id; window.trans_id++; var aosvpd=[]; var aosobj=[];"
	         << "aosvpd[0] = gXmlParser.String2Xml(\'" << vpdstr << "\');"
	         << "aosobj[0] = gXmlParser.String2Xml(\'<embedobj \\>\');";
	code.append(gicCode);

	if (code.mJson.length() > 0)
	{
		OmnString str = "aosvpd:aosvpd[0], ";
		code.mJson.insert(str, 1);
	}
	return rslt;
}


bool 
AosHtmlReqProc::createPartVpd(AosHtmlCode &code, OmnString &errmsg)
{
	OmnString vpdid = mReqInfo->getChildTextByAttr("name", "objid");
	if(vpdid == "")
	{
		errmsg << "missing vpdid";
		return false;
	}
	
	mRetrieveType = AOSHTML_RETRIEVEVPD;
	OmnString objid = mReqInfo->getChildTextByAttr("name", AOSHTML_OBJNAME);
	
	AosXmlTagPtr vpd, vpdroot, obj, objroot;
	bool rslt = getVpd(vpdid, vpd, vpdroot);
	if(!rslt)
	{
		errmsg << "Failed to retrieve , parser response error";
		return false;
	}
	
	if(objid != "")
	{
		rslt = getVpd(objid, obj, objroot);
		if(!obj)
		{
			OmnAlarm << "Failed to retrieve objname : " << objid << enderr;
		}
	}
	if(!obj)
	{
		obj = vpd->getFirstChild("embedobj");
	}
	
	OmnString parentId;
	OmnString flag = getInsertFlag();
	rslt = appendVpdArray(vpd, obj, parentId, flag, vpdid, mParentWidth, mParentHeight);
	if(!rslt)
	{
		OmnAlarm << "can't append vpd to array!"<< enderr;
		return false;
	}
	return createAllVpd(code);
}


OmnString
AosHtmlReqProc::createVpdPath(OmnString &vpdname)
{
	OmnString path = mVpdPath;
	path <<  "/" << vpdname;
	return path;
}


OmnString 
AosHtmlReqProc::getInsertFlag()
{
	OmnString flag = OmnGetTimebasedId();
	flag << "_" << mVpdArrayLength;
	return flag;
}


bool 
AosHtmlReqProc::appendVpdArray(
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString &parentId,
		const OmnString &flag,
		const OmnString &vpdPath,
		const int pWidth,
		const int pHeight)
{
	if((int)mVpdStructArray.size() > 10)
	{
		return false;
	}

	AosHtmlVpdStructPtr st = OmnNew AosHtmlVpdStruct();
	
	st->mVpd = vpd;
	st->mObj = obj;
	st->mParentId = parentId;
	st->mFlag << flag;
	st->mVpdPath = vpdPath;
	st->pWidth = pWidth;
	st->pHeight = pHeight;

	mVpdStructArray.push_back(st);
	mVpdArrayLength++;
	return true;
}


bool 
AosHtmlReqProc::createCreator(AosHtmlCode &code, OmnString &errmsg)
{
	AosHtmlDynArray vpds;
	AosHtmlDynArray objs;

	mRetrieveType = AOSHTML_RETRIEVEVPD;

	AosXmlTagPtr xmlobj = mReqInfo->getFirstChild(AOSHTML_XMLOBJ);
	if(!xmlobj)
	{
		errmsg << "Cann't parser request conents";
		return false;
	}
	
	AosXmlTagPtr parent = xmlobj->getFirstChild();
	if(!parent)
	{
		errmsg << "Cann't parser request conents";
		return false;
	}

	AosXmlTagPtr child = parent->getFirstChild();
	while(child)
	{
		OmnString ctobjid = child->getAttrStr("zky_ctobjid","");
		if (ctobjid != "")
		{
			AosXmlTagPtr vpd, root;
			bool rslt = getVpd(ctobjid, vpd, root);
			if(rslt)
			{
				vpds.append(vpd);
			}
		}
		child = parent->getNextChild();
	}
	
	if(vpds.entries() <= 0)
	{
		errmsg << "Cann't parser request conents";
		return false;
	}
	
	return createVpd(vpds, objs, code);
}

bool 
AosHtmlReqProc::createXmlCreator(AosHtmlCode &code, OmnString &errmsg)
{
	//AosHtmlDynArray vpds;
	//AosHtmlDynArray objs;

	mRetrieveType = AOSHTML_RETRIEVEVPD;

	AosHtmlReqProcPtr htmlReqPtr(this, false);
	AosXmlTagPtr xmlobj = mReqInfo->getFirstChild(AOSHTML_XMLOBJ);
	if(!xmlobj)
	{
		errmsg << "Cann't parser request conents";
		return false;
	}
	
	AosXmlTagPtr obj = xmlobj->getFirstChild();
	if(!obj)
	{
		errmsg << "Cann't parser request conents";
		return false;
	}

	OmnString ctobjid = obj->getAttrStr("zky_ctobjid__xmleditor", "");
	if (ctobjid == "")
	{
		obj = obj->getFirstChild();
		if (obj)
		{
			ctobjid = obj->getAttrStr("zky_ctobjid__xmleditor", "");
		}
	}

	AosXmlTagPtr vpd, root;
	AosHtmlCode gicCode;
	int num = mVpdNum;
	if (ctobjid != "")
	{
		bool rslt = getVpd(ctobjid, vpd, root);
		if(!rslt)
		{
			errmsg << "Cann't parser request conents";
			return false;
		}
		OmnString gic_type = vpd->getAttrStr("gic_type", "");
		if (gic_type != "gic_container")
		{
			errmsg << "The Gic's xtype is not gic_container!";
			return false;
		}
		
		mRundata->setArg1("createobj", "objgic");
	
		OmnString objx, objy, objw, objh;
		OmnString pos = obj->getAttrStr("zky_gicpos__xmleditor", "");
		if (pos != "")
		{
			AosStrSplit split;
			bool finished;
			OmnString pair[4];
			split.splitStr(pos.data(), "|$|", pair, 4, finished);
			objx = pair[0];
			objy = pair[1];
			objw = pair[2];
			objh = pair[3];
		}
		if (objx == "") objx = vpd->getAttrStrSimp("gic_x", "0");
		if (objy == "") objy = vpd->getAttrStrSimp("gic_y", "0");
		if (objw == "") objw = vpd->getAttrStrSimp("gic_width", "0");
		if (objh == "") objy = vpd->getAttrStrSimp("gic_height", "0");
		vpd->setAttr("gic_x", objx);
		vpd->setAttr("gic_y", objy);
		vpd->setAttr("gic_width", objw);
		vpd->setAttr("gic_height", objh);
		rslt = AosGic::createGic(htmlReqPtr, vpd, obj, "", gicCode, atoi(objw.data()), atoi(objh.data()));
		aos_assert_r(rslt, false);
	}

	OmnString js = "var aosvpd = [];var aosobj = [];";
	if (vpd)
	{
		js << "aosvpd[" << num << "] = gXmlParser.String2Xml(\'" << vpd->toString() << "\');";
	}
	if (obj)
	{
		js << "aosobj[" << num << "] = gXmlParser.String2Xml(\'" << obj->toString() << "\');";
	}

	code.mJs << "if(!window.trans_id) window.trans_id=0; var soloid=10000+window.trans_id; window.trans_id++;" << js;
	code.append(gicCode);
	
	if (code.mJson == "")
	{
		code.mJson = "{}";
	}
	return	true; 
}

int
AosHtmlReqProc::createVpd(
		AosHtmlDynArray &vpds,
		AosHtmlDynArray &objs,
		AosHtmlCode &code)
{
	AosXmlTagPtr vpd, obj = 0;
	int rslt;
	code.mJs << "if(!window.trans_id) window.trans_id=0; var soloid=10000+window.trans_id; window.trans_id++; var aosvpd=[]; var aosobj=[];";
	code.mJson << "[";
	
	int length = vpds.entries();
	for(int i = 0; i < length; i++)
	{
		if(vpds.get(i, vpd))
		{
			AosHtmlCode temCode;
			temCode.mCounter = i;
			
			OmnString parentId = "";
			rslt = createVpd(vpd, obj, temCode, parentId, mParentWidth, mParentHeight);
			
			code.mJs << temCode.mJs;
			code.mJson << "aosjson" << i;
			code.mHtml << "aoshtmlstr" << i;
		}
		if(i < length-1)
		{
			code.mJson << ",";
			code.mHtml<< "+";
		}
	}
	code.mJson << "]";
	return rslt;
}


bool 
AosHtmlReqProc::simulate(AosHtmlCode &code, OmnString &errmsg)
{
	if(!mReqInfo)
	{
		errmsg = "Missing Request Infor";
		return false;
	}
	
	mRetrieveType = AOSHTML_SIMULATE;
	
	AosXmlTagPtr xmlvpd = mReqInfo->getFirstChild(AOSHTML_XMLVPD);
	AosXmlTagPtr xmlobj = mReqInfo->getFirstChild(AOSHTML_XMLOBJ); 
	OmnString local = mReqInfo->getChildTextByAttr("name", "zky_local");
	
	if(!xmlvpd)
	{
		errmsg = "Missing simulate Vpd Info";
	    OmnAlarm << errmsg << enderr;
		return false;
	}

	AosXmlTagPtr vpd, obj;
	if (xmlobj)
	{
		obj = xmlobj->getFirstChild();
		if(obj)
		{
			mXmlRoots.append(xmlobj);
		}
	}
	
	vpd = xmlvpd->getFirstChild();
	if(!vpd)
	{
		errmsg = "Unrecognized vpd, Failed to parse it";
		OmnAlarm << errmsg << enderr;
		return false;
	}
		
	mXmlRoots.append(xmlvpd);
	if (!obj)
	{
		obj = vpd->getFirstChild("embedobj");
	}

	OmnString url;
	bool rslt = simulateVpd(vpd, obj, url, local);
	if(!rslt)
	{
		errmsg << "Simulating, Failed to generate url";
		OmnAlarm << errmsg << enderr;
		return false;
	}
		
	code.mXml = url;
	return true;
}


bool
AosHtmlReqProc::simulateVpd(
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		OmnString &url,
		OmnString local)
{
	aos_assert_r(mSimulatePath != "", false);
	
	OmnString vpdid = vpd->getAttrStr("zky_objid", "");
	url << "http://" << local << mSimulatePath;
	
	sgSimLock.lock();
	if (vpdid != "")
	{
		url << vpdid << "_" << mSeqNum << ".html";	
	}
	else
	{
		url << "temp_" << mSeqNum << ".html";	
	}

	if (mSeqNum > 10000) mSeqNum = 0;
	mSimVpds.insert(pair<int, AosXmlTagPtr>(mSeqNum, vpd));
	mSeqNum++;
	sgSimLock.unlock();
	return true;
}


bool
AosHtmlReqProc::print(AosHtmlCode &code, OmnString &errmsg)
{
	//this function for print a vpd by a printer
	if(!mReqInfo)
	{
		errmsg = "Missing Request Infor";
		return false;
	}
	
	mRetrieveType = AOSHTML_PRINT;
	
	AosXmlTagPtr xmlvpd = mReqInfo->getFirstChild(AOSHTML_XMLVPD);
	AosXmlTagPtr xmlobj = mReqInfo->getFirstChild(AOSHTML_XMLOBJ); 
	OmnString local = mReqInfo->getChildTextByAttr("name", "zky_local");

	if(!xmlvpd)
	{
		errmsg << "Miss vpd";
		OmnAlarm << errmsg << enderr;
		return false;
	}
		
	mXmlRoots.append(mReqInfo);
	AosXmlTagPtr vpd = xmlvpd->getFirstChild();
	if(!vpd)
	{
		OmnString vpdname = xmlvpd->getNodeText();
		aos_assert_r(vpdname != "", false);
		
		AosXmlTagPtr resp = AosSengAdmin::getSelf()->retrieveDoc(mSiteid,
				vpdname, mSessionId, mUrlDocDocid, "false", mCookies);	
		if(!resp)
		{
			errmsg << "Miss vpd";
			OmnAlarm << errmsg << enderr;
			return false;
		}
	
		mXmlRoots.append(resp);
		AosXmlTagPtr content = resp->getFirstChild("Contents");
		if(!content)
		{
			errmsg << "Miss vpd";
			OmnAlarm << errmsg << enderr;
			return false;
		}
			
		vpd = content->getFirstChild();
		if(!vpd)
		{
			errmsg << "Miss vpd";
			OmnAlarm << errmsg << enderr;
			return false;
		}
	}
	
	AosXmlTagPtr obj;
	if(xmlobj)
	{
		obj = xmlobj->getFirstChild();
	}
	if(!obj)
	{
		obj = vpd->getFirstChild("embedobj");
	}

	OmnString url;
	bool rslt = printVpd(vpd, obj, url, local);
	if(!rslt)
	{
		errmsg << "print, Failed to generate url";
		OmnAlarm << errmsg << enderr;
		return false;
	}
		
	code.mXml = url;
	return true;
}


bool
AosHtmlReqProc::printVpd(
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		OmnString &url,
		OmnString local)
{
	aos_assert_r(mPrintPath != "", false);
	
	OmnString vpdid = vpd->getAttrStr("zky_objid","");
	url << "http://" << local << mPrintPath;
	
	sgPrintLock.lock();
	if (vpdid != "")
	{
		url << vpdid << "_" << mSeqNum << ".html";	
	}
	else
	{
		url << "temp_" << mSeqNum << ".html";	
	}
	if (mSeqNum > 10000) mSeqNum = 0;
	mPrintVpds.insert(pair<int, AosXmlTagPtr>(mSeqNum, vpd));
	mSeqNum++;
	sgPrintLock.unlock();
	return true;
}


int
AosHtmlReqProc::createVpd(
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		AosHtmlCode &vpdCode,
		const OmnString &parentId,
		const int pWidth,
		const int pHeight)
{
	int status = createPane(vpdCode, vpd, obj, parentId, pWidth, pHeight);
	aos_assert_r(status, status);


	OmnString vpdstr = vpd ? vpd->toString() : "";
	OmnString objstr = obj ? obj->toString() : "<embedobj/>";
	objstr.replace("'", "\\'", true);

OmnScreen << "vpd string =======  " << vpd->toString() << endl;
	
	vpdCode.mJs << "aosvpd[" << mVpdNum << "]=gXmlParser.String2Xml(\'" << vpdstr << "\');";
	vpdCode.mJs << "aosobj[" << mVpdNum << "]=gXmlParser.String2Xml(\'" << objstr << "\');";
	
	OmnString str;
	str << "mVpdNum:" << mVpdNum << ", aosvpd:aosvpd[" << mVpdNum << "], aosobj:aosobj[" << mVpdNum << "],";
	if (vpdCode.mJson.length() > 0)
	{
		vpdCode.mJson.insert(str, 1); 
	}
		
	if (mOperation == AosHtmlOpr::eRetrieveCreator)// || mOperation == AosHtmlOpr::eRetrieveXmlCreator)
	{
		vpdCode.mJs << "eval(\"var aoshtmlstr" << vpdCode.mCounter << "=''\");";  
		vpdCode.mJs << "aoshtmlstr" << vpdCode.mCounter << "='" << vpdCode.mHtml << "';"; 
		vpdCode.mJs << "eval('var aosjson" << (vpdCode.mCounter) << "={}');";
		vpdCode.mJs << "aosjson" << vpdCode.mCounter << "=" << vpdCode.mJson <<";"; 
	}
	return status;
}


int
AosHtmlReqProc::createAllVpd(AosHtmlCode &code)
{
	code.mJs << "if(!window.trans_id) window.trans_id=0; var soloid=10000+window.trans_id; window.trans_id++; var aosvpd=[]; var aosobj=[];";
	
	int status, rest, pWidth, pHeight, idx;
	OmnString mFlag, parentId;
	AosXmlTagPtr vpd, obj;

	for (int i=0; i<(int)mVpdStructArray.size(); i++)
	{
		mVpdNum = i;
		AosHtmlCode vpdCode;

		vpd = mVpdStructArray[i]->mVpd;
		obj = mVpdStructArray[i]->mObj;
		pWidth = mVpdStructArray[i]->pWidth;
		pHeight = mVpdStructArray[i]->pHeight;
		parentId = mVpdStructArray[i]->mParentId;
		mVpdPath = mVpdStructArray[i]->mVpdPath;

		status = createVpd(vpd, obj, vpdCode, parentId, pWidth, pHeight);
			
		if(status == AOSHTML_GIC_STATUS)
		{
			OmnString pId = getInstanceId();
			OmnString str, str2;
			str << "parentId:" << parentId << "\",";
			
			if(mIsHomePage)
			{
				str2 << "parentId:\"" << pId << "\",";
			}
			else
			{
				str2 << "parentId:" << pId << "\",";
			}
			
			if(parentId != "")
			{
				idx = vpdCode.mJson.replace(str, str2, false);
				if(idx <= 0)
				{
					OmnAlarm << "Failed to find the id: " << parentId << enderr;
				}
			}
			else
			{
				vpdCode.mJson.insert(str2, 1);
			}
				
			createDefaultPanel(vpdCode, pId);
			if(parentId != "")
			{
				vpdCode.mJson.insert(str, 1);
			}
			status = AOSHTML_PANEL_STATUS;
		}
		
		if(i == 0)
		{
			code.append(vpdCode);
			rest = status;
		}
		else
		{
			code.mJs << vpdCode.mJs;
			code.mCss << vpdCode.mCss;
			code.mFlash << vpdCode.mFlash;
			code.mXml << vpdCode.mXml;
			
			mFlag = mVpdStructArray[i]->mFlag;
OmnScreen << "mFlag" << mFlag <<endl;
			idx = code.mHtml.replace(mFlag, vpdCode.mHtml, false);
			if (idx <= 0)
			{
				OmnAlarm << "Failed to find the flag: " << mFlag << enderr;
			}
			idx = code.mJson.replace(mFlag, vpdCode.mJson, false);
			if (idx <= 0)
			{
				OmnAlarm << "Failed to find the flag: " << mFlag << enderr;
			}
		}
	}
	return rest;
}

	
bool
AosHtmlReqProc::checkVpdPath(OmnString &vpdname)
{
	// This function checks whether the vpd 'vpdname' forms a loop. 
	// If 'vpdname' appears in the path, it forms a loop.
	OmnStrParser1 parser(mVpdPath.data(), "/");
	OmnString vname;
	while((vname = parser.nextWord()) != "")
	{
		if(vname == vpdname)
		{
			OmnAlarm << "vpdpath error, vpdname:" << vpdname << ", vpdpath:" << mVpdPath << enderr;
			return false;
		}
	}
	return true;
}

	
int
AosHtmlReqProc::getComponentId()
{
	if (mCurrentId == mEndId)
	{
		mCurrentId = OmnGetSecond();
		mCurrentId = mCurrentId << 16;
		mEndId = mCurrentId + (1<<16);
		if ((mCurrentId % 100) == 0)
			OmnScreen << "Waring :: the compids are not using iil" << endl;
		// Chen Ding, 02/10/2012
		// AosXmlTagPtr idTag;
		// bool flag = AosSengAdmin::getSelf()->requestNewId(mSiteid, idTag);
		// aos_assert_r(flag && idTag, -1);
		// 	
		// idTag = idTag->getFirstChild("Contents");
		// aos_assert_r(idTag, -1);
		// 	
		// AosXmlTagPtr idInfo = idTag->getFirstChild();
		// aos_assert_r(idInfo, -1);
		//
		// mCurrentId = idInfo->getAttrInt("compid", -1);
		// mEndId = idInfo->getAttrInt("size", -1) + mCurrentId;
		// aos_assert_r(!(mCurrentId==mEndId && mEndId==-1) , -1);
		//mCurrentId = AosSeIdGenMgr::getSelf()->nextCompIdBlock(); 
		//mEndId = mCurrentId + eCompIdBlockSize;
	}
	return mCurrentId++;
}


OmnString
AosHtmlReqProc::getInstanceId()
{
	OmnString compid, r;
	compid << getComponentId();
	if(mIsHomePage)
	{
		r << "10000_" << compid;
	}
	else
	{
		r << "soloid+\"_" << compid;
	}
	return r;
}


OmnString
AosHtmlReqProc::getInstanceId(AosXmlTagPtr &vpd)
{
	//ken 2012/03/05
	/*OmnString compid = vpd->getAttrStr(AOSTAG_COMPID, "");
	if (compid == "")
	{
	    compid << getComponentId();
	    vpd->setAttr(AOSTAG_COMPID, compid);
	}*/
	OmnString compid;
	compid << getComponentId();
	vpd->setAttr(AOSTAG_COMPID, compid);
	
	OmnString r;
	if(mIsHomePage)
	{
		r << "\"10000_" << compid;
	}
	else
	{
		r << "soloid+\"_" << compid;
	}
	OmnString userid = vpd->getAttrStr("gic_instid");
	if(userid == "")
	{
		userid = vpd->getAttrStr("ctn_id");
	}
	if(userid != "")
	{
		r << "_" << userid;
	}
	return r;
}


bool
AosHtmlReqProc::createDefaultPanel(
		AosHtmlCode &code,
		const OmnString &panelId)
{
	OmnString json, html;
	
	json << "{";
	if(mIsHomePage)
	{
		html << "<div id=\"" << panelId << "\" ";
		json << "id:\"" << panelId << "\", ";
		json << "applyTo:\"" << panelId << "\", ";
	}
	else
	{
		html << "<div id='+" << panelId << "\"+' ";
		json << "id:" << panelId << "\", ";
		json << "applyTo:" << panelId << "\", ";
	}

	html << "class=\"x-panel x-panel-noborder\" style=\"width:100px;\">"
		 << "<div class=\"x-panel-bwrap\"><div class=\"x-panel-body x-panel-body-noheader x-panel-body-noborder\" "
		 << "style=\"width:100px;height:100px;\" >"
		 << code.mHtml << "</div></div></div>";

	// Ketty 2011/10/19
	json << "isDefault:true,"
		 << "xtype:'aos_panel', "
		 << "width:100, height:100, "
		 << "mlLayoutType:'lm_fixed', "	//Panel Default layoutType
		 << "mlVpdWidth:100, mlVpdHeight:100, "
		 << "mlWdef:'percent', "
		 << "mlHdef:'percent', "
		 << "isInEditor:" << mIsInEditor << ", "
		 << "items:["
		 << code.mJson
		 << "]}";
	
	code.mJson = json;
	code.mHtml = html;
	return true;
}


int
AosHtmlReqProc::createPane(
		AosHtmlCode &code,
		AosXmlTagPtr &vpd, 
		AosXmlTagPtr &obj, 
		const OmnString &parentId,
		const int parentWidth,
		const int parentHeight)
{
	AosHtmlReqProcPtr htmlReqPtr(this, false);
	return AosLayout::createLayoutVpd(htmlReqPtr, vpd, obj, parentId, code, parentWidth, parentHeight);
}


bool 
AosHtmlReqProc::createHtml( 
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		AosHtmlCode &htmlCode,
		OmnString &errmsg)
{
	//html body
	AosHtmlCode dialogCode;
	// Ketty 2011/10/27
	int status = -1;	
	bool rslt = createWindow(vpd, obj, dialogCode, status);
	if(!rslt)
	{
		errmsg << "Parser vpd error.";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	//create Html code
	//1.create doctype
	htmlCode.mHtml << mHtmlConfigMgr->createDocDtd(4);
	htmlCode.mHtml << "<html>";
	
	//2.create html head.
	htmlCode.mHtml << "<head>";
	htmlCode.mHtml << mHtmlConfigMgr->createHtmlHead(vpd);
	htmlCode.mHtml << mHtmlConfigMgr->createPngfixStr();
	//add css.
	htmlCode.mHtml << "<style type=\"text/css\">"
				   << dialogCode.mCss
				   << "</style>";
	htmlCode.mHtml << "</head>";

	//3.add html body.
	htmlCode.mHtml << dialogCode.mHtml;
	
	htmlCode.mHtml << "</html>";

	//4. add json code.
	OmnString loginInfoStr;
	if(mLoginObj && mNeedLoginObj)
	{
		OmnString lgnObjStr = mLoginObj->toString();
		loginInfoStr << "gAosSystem.mLoginObj=gXmlParser.String2Xml(\'" << lgnObjStr << "\');";
	}

	//add languagetype
	OmnString languagetype;
	if(mLanguageType!="")
	{
		languagetype << "gAosSystem.mLanguageType=\'" << mLanguageType << "\';" << webEnd;
	}

	htmlCode.mJs << dialogCode.mJs
				 << loginInfoStr 
				 << languagetype 
				 << "var extConfig=" << dialogCode.mJson << ";"
				 << mHtmlConfigMgr->createExtReady();

	if(mPerformance)
	{
		htmlCode.mJs << mHtmlConfigMgr->createPerformanceStart();
	}
	else if(status == AOSHTML_EDITORPANE_STATUS)
	{
		// Ketty 2011/10/27
		htmlCode.mJs << "new Ext.Viewport(extConfig);";
	}
	else
	{
		htmlCode.mJs << "new Aos.AosWindow(extConfig);";
	}
	return true;
}


bool
AosHtmlReqProc::createWindow(
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		AosHtmlCode &code,
		int &status)
{
	vpd->setAttr("container_x",0);
	vpd->setAttr("container_y",0);
	OmnString vpdname = vpd->getAttrStr("zky_objid");

	// Chen Ding, 09/24/2011
	code.setCrtVpdObjid(vpdname);

	OmnString parentId = "";
	OmnString flag = getInsertFlag();
	bool stat = appendVpdArray(vpd, obj, parentId, flag, vpdname, mParentWidth, mParentHeight);
	if(!stat)
	{
		OmnAlarm << "can't append vpd to array!"<< enderr;
		return false;
	}
	OmnString panelId, paneId, str;
	status = createAllVpd(code);
	switch(status)
	{
	case AOSHTML_GIC_STATUS :
		 panelId = getInstanceId();
		 str = "";
		 str << "parentId:\"" << panelId << "\",";
		 code.mJson.insert(str, 1);
		 createDefaultPanel(code, panelId);
	
	case AOSHTML_PANEL_STATUS :
		 paneId = getInstanceId();
		 str = "";
		 str << "parentId:\"" << paneId << "\",";
		 code.mJson.insert(str, 1);
		 createDefaultPane(code, paneId, vpd);
	
	case AOSHTML_PANE_STATUS :
		 str = "";
		 str << "parentId:\"10000_10000_aoswindow\",";
		 code.mJson.insert(str, 1);
		 createDefaultWindow(code, vpd);

		 str = "";
		 str << ",mVpdNum:"<<mVpdNum<<", aosvpd:aosvpd["<<mVpdNum<<"], aosobj:aosobj["<<mVpdNum<<"]";
		 code.mJson.insert(str, code.mJson.length()-1);
		 break;
	
	// Ketty 2011/10/27
	case AOSHTML_EDITORPANE_STATUS :
		 createViewPort(vpd, code, status);
 		 break;

	default:
		 return false;
	}
	
	// Ketty 2011/10/08
	//createViewPort(vpd, code, status);
	addBodyStyle(vpd, code);
	return true;
}


bool
AosHtmlReqProc::createDefaultPane(
		AosHtmlCode &code,
		OmnString &paneId,
		AosXmlTagPtr &vpd)
{
	OmnString width = vpd->getAttrStr("container_width", "100");
	OmnString height = vpd->getAttrStr("container_height", "100");
	OmnString container_width_def = vpd->getAttrStr("container_width_def", "percent");
	OmnString container_height_def = vpd->getAttrStr("container_width_def", "percent");
	OmnString autoScroll= vpd->getAttrStr("autoScroll", "true");
	OmnString json, html;
	
	if(mIsHomePage)
	{
		html << "<div id=\"" << paneId << "\" ";
	}
	else
	{
		html << "<div id='+" << paneId << "\"+' ";
	}
	
	// Ketty 2011/09/21
	u32 width_u32 = atoi(width.data());
	u32 height_u32 = atoi(height.data());
	if(container_width_def == "percent")
	{
		width_u32 = mParentWidth * width_u32 / 100;		
	}
	if(container_height_def == "percent")
	{
		height_u32 = mParentHeight * height_u32 / 100;		
	}

	html << "class=\"x-panel x-panel-noborder\" style=\"width:" << width_u32 << "px;\">"
		 << "<div class=\"x-panel-bwrap\"><div class=\"x-panel-body x-panel-body-noheader x-panel-body-noborder\" "
		 << "style=\"width:" << width_u32 << "px; height:" << height_u32 << "px;\" >"
		 << code.mHtml << "</div></div></div>";

	json << "{id:\"" << paneId << "\", "
		 << "applyTo:\"" << paneId << "\", "
		 << "isDefault:true,"
		 << "isInEditor:" << mIsInEditor << ", "
		 << "xtype:'aos_npane', "
		 << "autoScroll:" << autoScroll << ", "
		 << "mlLayoutType:'lm_npane', "
		 << "width:" << width_u32 << ", "
		 << "height:" << height_u32 << ", "
		 << "mlVpdWidth:" << width << ", "
		 << "mlVpdHeight:" << height << ", "
		 << "mlWdef:'" << container_width_def << "', "
		 << "mlHdef:'" << container_height_def << "', " 
		 << "items:["
		 << code.mJson
		 << "]}";
	
	code.mJson = json;
	code.mHtml = html;
	return true;
}


bool
AosHtmlReqProc::createDefaultWindow(
		AosHtmlCode &code,
		AosXmlTagPtr &vpd)
{
	OmnString width_policy = vpd->getAttrStr("container_width_def", "percent");
	OmnString height_policy = vpd->getAttrStr("container_height_def", "percent");
	OmnString ctn_winbg = vpd->getAttrStr("container_background", "");
	OmnString norepeat = vpd->getAttrStr("norepeat", "");
	OmnString ctn_bgcolor = vpd->getAttrStr("container_bgcolor", "");
	ctn_bgcolor = AosHtmlUtil::getWebColor(ctn_bgcolor);
	int w = vpd->getAttrInt("container_width", vpd->getAttrInt("ctn_ww", 100));
    int h= vpd->getAttrInt("container_height", vpd->getAttrInt("ctn_hh", 100));
	
	OmnString width, height, json, html, bgimgpath, bkground;
	if(width_policy == "percent")
	{
		width <<  w * mParentWidth / 100;	
	}
	else
	{
		width << w;
	}
	if(height_policy == "percent")
	{
		height << h * mParentHeight / 100;
	}
	else
	{
		height << h;
	}
	
	json << "{id:\"10000_10000_aoswindow\", "
		 << "applyTo:\"10000_10000_aoswindow\", "
		 << "isDefault:true,"
		 << "isInEditor:" << mIsInEditor << ", "
		 << "ctn_winbg:\"" << ctn_winbg << "\", "
		 << "norepeat:\"" << norepeat << "\", "
		 << "ctn_bgcolor:\"" << ctn_bgcolor << "\", "
		 << "layout:'absolute', "
		 << "xtype:'aos_window', "
		 << "mlLayoutType:'lm_window', "
		 << "width:" << width << ", "
		 << "height:" << height << ", "
		 << "mlWdef:'" << width_policy << "', "
		 << "mlHdef:'" << height_policy << "', "
		 << "mlVpdWidth:" << w << ", "
		 << "mlVpdHeight:" << h << ", "
		 << "items:[" 
		 << code.mJson
		 << "]}";
	code.mJson = json;
	
	bkground << "background:" <<ctn_bgcolor;
	if(ctn_winbg != "")
	{
		bgimgpath = ctn_winbg;
		if(ctn_winbg.indexOf("http://",0) == -1)	
		{
			bgimgpath = mImagePath;
			bgimgpath << ctn_winbg;
		}
		bkground << " url(" << bgimgpath << ");";
		if(norepeat != "" && norepeat != "false")
		{
			bkground << "background-repeat:no-repeat;background-position:center top";	
		}
	}

	// Ketty 2011/12/21  set the window in the center
	int left = 0;
	if(mParentWidth != 0)
	{
		left = (mParentWidth - atoi(width.data())) / 2;
		if(left < 0) left = 0;
	}

	html << "<div id=\"10000_10000_aoswindow\" class=\"x-panel x-panel-noborder\" style=\"position:absolute; width:" << width << "px; height:" << height << "px; left:" << left << "px;\">"
		<< "<div class=\"x-panel-bwrap\"><div class=\"x-panel-body x-panel-body-noheader x-panel-body-noborder\" style=\"" << bkground << ";width:" << width << "px; height:" << height << "px;\">"
		<< code.mHtml << "</div></div></div>";
	
	//html << "<div id=\"10000_10000_aoswindow\" style=\"position:absolute; width:" << width << "px; height: " << height << "px; left:" << left <<  "\">"
	//	 << code.mHtml << "</div>";
	code.mHtml = html;
	return true;
}


bool
AosHtmlReqProc::createViewPort(
		AosXmlTagPtr &vpd,
		AosHtmlCode &code,
		const int status)
{
	// this function just for editor_pane. Ketty 2011/10/27
	OmnString json = "{autoScroll:true, id:'aosviewport', ";
	json << "layout:'fit', ";
	json << " items:" << code.mJson << "}";
	code.mJson = json;
	return true;
}


// Ketty 2011/10/08
bool
AosHtmlReqProc::addBodyStyle(
		AosXmlTagPtr &vpd,
		AosHtmlCode &code)
{
	OmnString bdStyle;
	OmnString bodybg = vpd->getAttrStr("body_background","");
	if(bodybg != "")
	{
		OmnString path;
		if(bodybg.indexOf("http://",0) == -1)
		{
			path = mImagePath;
		}
		path << bodybg;
		bdStyle << "background:url(" << path << ");";
	}
	OmnString bodybgcolor = vpd->getAttrStr("ctn_bodybgcolor","");
	bodybgcolor = AosHtmlUtil::getWebColor(bodybgcolor);
	if(bodybgcolor != "")
	{
		bdStyle << "background-color:" << bodybgcolor << ";";
	}
	OmnString norepeat = vpd->getAttrStr("bodynorepeat","");
	OmnString npstyle;
	if(norepeat != "false" && norepeat !="")
	{
		npstyle = "background-repeat:no-repeat;background-position:center top;";
	}
	bdStyle << npstyle;
	OmnString html;
	OmnString bodyscript= vpd->getAttrStr("body_script","");
	bodyscript.replace("&quot;","\"", true);
	bodyscript.replace("&gt;",">", true);
	bodyscript.replace("&lt;","<", true);
	bodyscript.replace("&amp;#39;","'", true);
	html << "<body style=\"" << bdStyle << "\">" << code.mHtml << "<script>" << bodyscript<<"</script></body>";
	code.mHtml = html;
	return true;
}


bool
AosHtmlReqProc::createGic(
		AosHtmlCode &code,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString &parentId,
		const int parentWidth,
		const int parentHeight)
{
	AosHtmlReqProcPtr htmlReqPtr(this, false);

	AosHtmlCode gicCode;
	bool rslt = false;
	OmnString gictype =	vpd->getAttrStr("gic_type");
	if (gictype != "")
	{
		rslt = AosGic::createGic(htmlReqPtr, vpd, obj, parentId, gicCode, parentWidth, parentHeight);
		if(rslt)
		{
			addGic(gictype);
			addGicAction(vpd);
			code.append(gicCode);
		}
		return true;
	}
	
	AosXmlTagPtr gicCreator = vpd->getFirstChild("gic_creators");
	if(!gicCreator)
	{
		return false;
	}
	
	AosXmlTagPtr gicChild = gicCreator->getFirstChild();
	while(gicChild)
	{
		gictype = gicChild -> getAttrStr("gic_type", "");
		rslt = false;
		if(gictype != "")
		{
			addGicAction(gicChild);
			addGic(gictype);
			rslt = AosGic::createGic(htmlReqPtr, gicChild, obj, parentId, gicCode, parentWidth, parentHeight);
		}
		gicChild = gicCreator->getNextChild();
		if(rslt)
		{
			code.append(gicCode);
			if(gicChild)
			{
				OmnString nextGictype = gicChild -> getAttrStr("gic_type", "");
				if(nextGictype != "")
					code.mJson << ",";
			}
		}
		gicCode.reset();
	}
	return true;
}


bool
AosHtmlReqProc::retrieveAllSites(
		AosHtmlCode &code,
		OmnString &errmsg)
{
	mRetrieveType = AOSHTML_RETRIEVESITES;

	OmnString vpdname = mReqInfo->getChildTextByAttr("name", "objid");
	vpdname = vpdname.substr(1);
	//felicia
	OmnString isUrl = mReqInfo->getChildTextByAttr("name", "isUrl");
	OmnString xmlStr;
	bool rslt = false;
	
	OmnString isInEditor;
	isInEditor << mIsInEditor;
	
	AosHtmlRetrieveSites sites;
	if(isUrl == "true")
	{
		rslt = sites.getAllSites(xmlStr, mCookies, mSiteid, mSessionId, mUrlDocDocid, isInEditor, vpdname, errmsg);
	}
	else
	{
		rslt = sites.retrieveAllSites(xmlStr, mCookies, 
				mSiteid, mSessionId, mUrlDocDocid, isInEditor, vpdname, errmsg);
	}

	code.mXml = xmlStr;
	return rslt;
}


bool 
AosHtmlReqProc::setVpdRootLevel(const AosXmlTagPtr &vpd)
{
	aos_assert_r(vpd, false);
	if ((vpd->getAttrStr("container_width_def", "")) != "fixed")
	{
		mVpdRootHLevel = true;
	}
	if ((vpd->getAttrStr("container_height_def", "")) != "fixed")
	{
		mVpdRootVLevel = true;
	}
	return true;
}


bool
AosHtmlReqProc::createEditorPane(
		AosHtmlCode &code,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString &parentId,
		const int parentWidth,
		const int parentHeight)
{
	addGic("editor_pane");
	
	OmnString str;
	str << "{xtype:'editor_pane', ";
	str << createEditorPaneJson(vpd);

	OmnString id  = getInstanceId();
	if(mIsHomePage)
	{
		str << "id:\"" << id << "\", ";
	}
	else
	{
		str << "id:" << id << "\", ";
	}

	str << "layout:'border', "
		<< "width:" << parentWidth << ", "
		<< "height:" << parentHeight << "}";
	code.mJson  = str;

	return true;
}


OmnString
AosHtmlReqProc::createEditorPaneJson(const AosXmlTagPtr &vpd)
{
	//////////////////////////////////
	//ken 2012/11/16
	//config format:
	//
	//<config>
	//<open_control open_restrict="false" open_vpdname="cute_editor_search_new_h"/>
	//<saveas_control saveas_restrict="false" saveas_vpdname="cute_editor_saveas_h"/>
	//<publish_control publish_restrict="false" publish_vpdname="cute_editor_url_h"/>
	//<savegic_control savegic_restrict="false" savegic_vpdname="cute_editor_savegic_h"/>
	//<tab_vpdnames widget_tab_restrict="true" use_self_tab_vpdnames="true">
	//<tab title="简易组件模版" vpdname="cute_editor_tmp_h"/>
	//<tab title="图片选择器" vpdname="cute_editor_image_h"/>
	//<tab title="自定义组件" vpdname="cute_editor4_h"/>
	//</tab_vpdnames>
	//</config>
	//
	//////////////////////////////////
	
	OmnString str = "";
	if(!vpd) return str;
		
	AosXmlTagPtr cfg = vpd->getFirstChild("config");
	if(!cfg) return str;
	
	AosXmlTagPtr open_control = cfg->getFirstChild("open_control");
	if(open_control)
	{
		bool open_restrict = open_control->getAttrBool("open_restrict", false);
		OmnString open_vpdname = open_control->getAttrStr("open_vpdname");		
		str << "open_restrict:" << open_restrict << ", "
			<< "open_vpdname:'" << open_vpdname <<"', ";
	}
	
	AosXmlTagPtr saveas_control = cfg->getFirstChild("saveas_control");
	if(saveas_control)
	{
		bool saveas_restrict = saveas_control->getAttrBool("saveas_restrict", false);
		OmnString saveas_vpdname = saveas_control->getAttrStr("saveas_vpdname");
		str << "saveas_restrict:" << saveas_restrict << ", "
			<< "saveas_vpdname:'" << saveas_vpdname <<"', ";
	}
	
	AosXmlTagPtr publish_control = cfg->getFirstChild("publish_control");
	if(publish_control)
	{
		bool publish_restrict = publish_control->getAttrBool("publish_restrict", false);
		OmnString publish_vpdname = publish_control->getAttrStr("_vpdname");
		str << "publish_restrict:" << publish_restrict << ", "
			<< "publish_vpdname:'" << publish_vpdname <<"', ";
	}
	
	AosXmlTagPtr savegic_control = cfg->getFirstChild("savegic_control");
	if(savegic_control)
	{
		bool savegic_restrict = savegic_control->getAttrBool("savegic_restrict", false);
		OmnString savegic_vpdname = savegic_control->getAttrStr("_vpdname");
		str << "savegic_restrict:" << savegic_restrict << ", "
			<< "savegic_vpdname:'" << savegic_vpdname <<"', ";
	}

	AosXmlTagPtr tab_vpdnames = cfg->getFirstChild("tab_vpdnames");
	if(tab_vpdnames)
	{
		bool widget_tab_restrict = tab_vpdnames->getAttrBool("widget_tab_restrict", false);
		bool use_self_tab_vpdnames = tab_vpdnames->getAttrBool("use_self_tab_vpdnames", false);
		str << "widget_tab_restrict:" << widget_tab_restrict << ", "
			<< "use_self_tab_vpdnames:" << use_self_tab_vpdnames << ", ";
		if(use_self_tab_vpdnames)
		{
			str << "self_tab_vpdnames:[";
			AosXmlTagPtr tab = tab_vpdnames->getFirstChild();
			while(tab)
			{
				OmnString title = tab->getAttrStr("title");
				OmnString vpdname = tab->getAttrStr("vpdname");
				str << "{title:'" << title << "', vpdname:'" << vpdname << "'}";
				tab = tab_vpdnames->getNextChild();
				if(tab) str << ", ";
			}
			str << "], ";
		}
	}
			
	return str;
}


/*
bool
AosHtmlReqProc::setActionParm(
		AosXmlTagPtr &parm)
{
	parm->setAttr("zky_ctobjid", "giccreators_parmviewer_h");
	OmnString parm_id = parm->getAttrStr("id", "");
	if (parm_id == "")
	{
		parm->removeNode("selector", false, true);
		parm->removeNode("value", false, true);
		return true;
	}
	if (parm_id == "compstr")
	{
		AosXmlTagPtr value = parm->getFirstChild("value");
		while(value)
		{
			OmnString value_type = value->getAttrStr("type", "");
			aos_assert_r(value_type != "", false);
			if (value_type == "const")
			{
				value->setAttr("zky_ctobjid", "giccreators_compstr_constviewer_h");
			}
			else if(value_type == "var")
			{
				value->setAttr("zky_ctobjid", "giccreators_compstr_varviewer_h");
				AosXmlTagPtr value_parm = value->getFirstChild("parm");
				if (value_parm)
				{
					setActionParm(value_parm);
				}
			}
			else
			{
				value->setAttr("type", "const");
				value->setAttr("zky_ctobjid", "giccreators_compstr_constviewer_h");
				OmnAlarm << "Unrecognized value type" << enderr;
			}
			value = parm->getNextChild();
		}
	}
	else
	{
		AosXmlTagPtr selector = parm->getFirstChild("selector");
		if (selector)
		{
			OmnString selector_type = selector->getAttrStr("type", "");
			if (selector_type == "attr")
			{
				selector->setAttr("zky_ctobjid", "giccreators_selectorattr_h");
			}
			else if (selector_type == "xmlattr")
			{
				selector->setAttr("zky_ctobjid", "giccreators_selectorxmlattr_h");
			}
			else if (selector_type == "xmltag")
			{
				selector->setAttr("zky_ctobjid", "giccreators_selectorxmltag_h");
			}
			else if (selector_type == "varpath")
			{
				selector->setAttr("zky_ctobjid", "giccreators_selectorvarpath_h");
			}
			else if (selector_type == "func")
			{
				selector->setAttr("zky_ctobjid", "giccreators_selectorfunc_h");
			}
			else
			{
				parm->removeNode("selector", false, false);
				return true;
			}
			selector->setAttr("zky_sort", "1");
			AosXmlTagPtr selector_parm = selector->getFirstChild("parm");
			if (selector_parm)
			{
				setActionParm(selector_parm);
			}
		}
	}
	return true;
}


bool
AosHtmlReqProc::convertAction(
		AosXmlTagPtr &action)
{
	aos_assert_r(action, false);
	if (!action)
	{
		return  false;
	}
	action->setAttr("zky_ctobjid", "giccreators_actionviewer_h");
	AosXmlTagPtr parm = action->getFirstChild("parm");
	while(parm)
	{
		setActionParm(parm);
		parm = action->getNextChild();
	}
	return true;
}
*/

