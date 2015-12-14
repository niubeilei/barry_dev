////////////////////////////////////////////////////////////////////////////

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
// 05/18/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlServer_HtmlReqProc_h
#define Aos_HtmlServer_HtmlReqProc_h

#include "HtmlServer/HtmlCode.h"
#include "HtmlServer/HtmlConfigMgr.h"
#include "HtmlServer/HtmlOpr.h"
#include "HtmlServer/HtmlUtil.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/File.h"
#include "Ptrs.h"
#include <map> 
#include <iterator> 
#include <vector> 

using namespace std;

typedef vector<AosHtmlVpdStructPtr>  AosHtmlVpdStructVector;
typedef OmnDynArray<AosXmlTagPtr, 10, 10, 50> AosHtmlDynArray;

class AosHtmlReqProc : public AosNetReqProc
{
	OmnDefineRCObject;

private:
	enum
	{
		eCompIdBlockSize = 1000,
		eMaxFiles = 1000
	};
	
	bool			mIsStopping;
	u32 			mCurrentId;
	u32 			mEndId;
	AosRundataPtr 	mRundata;
	
	//Dyn load JS member Data 
	static OmnString			mJsPath;
	static OmnString 			mImagePath;
	static OmnString 			mSimulatePath;
	static OmnString 			mPrintPath;
	static AosHtmlConfigMgrPtr	mHtmlConfigMgr;
	static OmnString			mSEServerName;
	static OmnString			mHtmlServerName;
	static OmnString			mAccessServerName;
	
	static int						mSeqNum;
	static map<int, AosXmlTagPtr>	mSimVpds;
	static map<int, AosXmlTagPtr>	mPrintVpds;
	static OmnString 				mDftGicVpd;
	static OmnString 				mAccessDftGicVpd;
	
	//ken 2011/5/24
	double 			SETime;
	double 			GetIDTime;
	
	bool		mFileLoadFlags[eMaxFiles];
	int 		mFileLoadNum;

	OmnString		mUsername;
	OmnString		mAppname;
	u32				mSiteid;
	OmnString		mSessionId;
	OmnString		mLanguageType;
	AosXmlTagPtr	mLoginObj;
	AosXmlTagPtr	mReqInfo;
	AosXmlTagPtr	mCookies;
	u64				mUrlDocDocid;
	
	AosHtmlOpr::E		mOperation;
	OmnString			mSubOpr;
	AosWebRequestPtr	mReq;
	
	bool		mIsHomePage;
	bool		mNeedLoginObj;
	bool		mIsInEditor;
	int 		mRetrieveType;
	
	bool 	    mPerformance;
	bool 	    mIsGicTester;
	OmnString	mTestVpdStr;
	bool		mVpdRootHLevel;
	bool		mVpdRootVLevel;
	
	AosHtmlDynArray			mXmlRoots;

	int 					mVpdNum;
	OmnString				mVpdPath;
	int 					mVpdArrayLength;
	AosHtmlVpdStructVector	mVpdStructArray;
	
	bool 			mVpdChanged;
	bool      		mIsTablet;

	u32				mParentWidth;	//Ketty 2011/09/21
	u32				mParentHeight;	//Ketty 2011/09/21

public:
	AosHtmlReqProc();
	~AosHtmlReqProc();

	static bool	config(const AosXmlTagPtr &config);

	bool 		stop();
	
	AosNetReqProcPtr	clone();

	bool		procRequest(const OmnConnBuffPtr &buff);

	bool		resetRequest(
					const OmnConnBuffPtr &buff,
					AosXmlRc &errcode,
					OmnString &errmsg);

	bool 		resetFileLoadFlags();

	void 		sendResp(
					const AosXmlRc errcode,
					const OmnString &errmsg,
					const OmnString &contents);

	void 		sendHtml(
					const int type,
					const AosHtmlCode &code,
					const u64 startTime);

	bool		addGic(const OmnString &name);

	bool		addGicAction(AosXmlTagPtr &vpd, const bool &isConvert = false);

	bool		createFullVpd(AosHtmlCode &code, OmnString &errmsg);

	bool		createSimulateVpd(AosHtmlCode &code, OmnString &errmsg);
	
	bool		getVpdFromMap(OmnString url, AosXmlTagPtr &vpd);

	bool		createPrintVpd(AosHtmlCode &code, OmnString &errmsg);
	
	bool		getVpdFromPrintMap(OmnString url, AosXmlTagPtr &vpd, AosXmlTagPtr &obj);

	bool 		createGicTester(AosHtmlCode &code, OmnString &errmsg);
	
	bool 		procServerError();

	bool 		performance(AosHtmlCode &code, OmnString &errmsg);
	
	bool 		getVpd(OmnString &vpdname, AosXmlTagPtr &vpd, AosXmlTagPtr &root);
	
	bool 		convertVpdForPerformance(AosXmlTagPtr &vpd);

	bool		retrieveGicVpd(AosHtmlCode &code, OmnString &errmsg);

	bool		createPartVpd(AosHtmlCode &code, OmnString &errmsg);

	OmnString 	getInsertFlag();

	bool		checkVpdPath(OmnString &vpdname);
	
	OmnString	createVpdPath(OmnString &vpdname);

	bool		appendVpdArray(
					AosXmlTagPtr &vpd,
					AosXmlTagPtr &obj,
					const OmnString &parentId,
					const OmnString &flag,
					const OmnString &vpdPath,
					const int pWidth,
					const int pHeight);

	bool		createCreator(AosHtmlCode &code, OmnString &errmsg);

	bool        createXmlCreator(AosHtmlCode &code, OmnString &errmsg);

	int 		createVpd(
					AosHtmlDynArray &vpds,
					AosHtmlDynArray &objs,
					AosHtmlCode &code);

	bool		simulate(AosHtmlCode &code, OmnString &errmsg);
	
	bool		simulateVpd(
					AosXmlTagPtr &vpd,
					AosXmlTagPtr &obj,
					OmnString &url,
					OmnString local);
	
	bool		print(AosHtmlCode &code, OmnString &errmsg);
	
	bool		printVpd(
					AosXmlTagPtr &vpd,
					AosXmlTagPtr &obj,
					OmnString &url,
					OmnString local);

	int 		createVpd(
					AosXmlTagPtr &vpd,
					AosXmlTagPtr &obj,
					AosHtmlCode &code,
					const OmnString &parentId,
					const int pWidth,
					const int pHeight);

	int			createAllVpd(AosHtmlCode &code);

	OmnString	getInstanceId(AosXmlTagPtr &vpd);

	OmnString   getInstanceId();

	int			getComponentId();

	int 		createPane(
					AosHtmlCode &code,
					AosXmlTagPtr &vpd,
					AosXmlTagPtr &obj,
					const OmnString &parentId,
					const int parentWidth,
					const int parentHeight);

	bool    	createDefaultPanel(
					AosHtmlCode &code,
					const OmnString &parentId);

	bool 		createDefaultPane(
					AosHtmlCode &code,
					OmnString &paneId,
					AosXmlTagPtr &vpd);
	
	bool		createWindow(
					AosXmlTagPtr &vpd,
					AosXmlTagPtr &obj,
					AosHtmlCode &code,
					int &status);	//Ketty 2011/10/27

	bool 		createDefaultWindow(
					AosHtmlCode &code,
					AosXmlTagPtr &vpd);

	bool		createViewPort(
					AosXmlTagPtr &vpd,
					AosHtmlCode &code,
					const int status);

	bool    	createGic(
					AosHtmlCode &code,
					AosXmlTagPtr &vpd,
					AosXmlTagPtr &obj,
					const OmnString &parentId,
					const int parentWidth,
					const int parentHeight);
	
	bool 		createHtml(
					AosXmlTagPtr &vpd,
					AosXmlTagPtr &obj,
					AosHtmlCode &code,
					OmnString &errmsg);
	
	bool    	createEditorPane(
					AosHtmlCode &code,
					AosXmlTagPtr &vpd,
					AosXmlTagPtr &obj,
					const OmnString &parentId,
					const int parentWidth,
					const int parentHeight);
	
	// Ketty 2011/10/08
	bool		addBodyStyle(
					AosXmlTagPtr &vpd,
					AosHtmlCode &code);

	bool 		retrieveAllSites(
					AosHtmlCode &code,
					OmnString &errmsg);

	bool		setVpdRootLevel(const AosXmlTagPtr &vpd);
	
	bool		needReturnHVpd() { return mVpdRootHLevel && mIsHomePage; }

	bool		needReturnVVpd() { return mVpdRootVLevel && mIsHomePage; }
	
//	bool 		needShapDraw() 
//				{ 
//					return (mRetrieveType == AOSHTML_RETRIEVEFULL || 
//							mRetrieveType == AOSHTML_SIMULATE);
//				}

	OmnString	getImagePath() const { return mImagePath; }

 	bool        isHomePage() const { return mIsHomePage; }
	
	bool		isInEditor() const { return mIsInEditor; }

	OmnString	getSsid() const { return mSessionId; }
	u64			getUrldocid() const { return mUrlDocDocid; }

	u32 getSiteid() const { return mSiteid; }

	AosRundataPtr	getRundata() { return mRundata; }

	AosXmlTagPtr	getReqInfo() { return mReqInfo; }

	// have ssid,
	// get mLoginObj from ses
	// save into mLoginObj
	AosXmlTagPtr	getLoginInfo() { return mLoginObj; }

	OmnString	getLanguageType() const { return mLanguageType; }
	
	void		setVpdNum() { mVpdNum++;}

	int			getVpdNum() const { return mVpdNum; }

	int         getVpdArrayLength() { return mVpdArrayLength; }

	int 		getRetrieveType() { return mRetrieveType; }
	
	//ken 2012/11/16
	OmnString	createEditorPaneJson(const AosXmlTagPtr &vpd);



/*
	 //felicia, 2011/08/03
	//AosWebAppPtr	getApp() { return mApp; }
	// The function create document footer content
	bool 		createFooter(AosHtmlCode &code);
	bool    	editVpd( const OmnString &parentId, AosXmlTagPtr &vpd);
	bool convertAction(AosXmlTagPtr &action);
	bool setActionParm(AosXmlTagPtr &parm);
	*/
};
#endif

