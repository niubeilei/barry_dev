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
// 2011/12/22	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_HtmlServer_HtmlConfigMgr_h
#define Omn_HtmlServer_HtmlConfigMgr_h

#include "HtmlServer/HtmlUtil.h"
#include "HtmlServer/Ptrs.h"
#include "Thread/RwLock.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

#include <map>
#include <vector>

using namespace std;

typedef map<OmnString, int> AosHtmlFileMap;
typedef map<OmnString, int>::iterator AosHtmlFileMapItr;
typedef map<OmnString, AosHtmlGicStructPtr> AosHtmlGicMap;
typedef map<OmnString, AosHtmlGicStructPtr>::iterator AosHtmlGicMapItr;
typedef vector<AosHtmlFileStructPtr> AosHtmlFileVector;
typedef vector<AosHtmlFileStructPtr>::iterator AosHtmlFileVectorItr;


class AosHtmlConfigMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	
	OmnRwLockPtr		mLock;
	bool				mConfiged;

	OmnString			mJsPath;
	OmnString			mCssPath;
	OmnString			mFilePath;
	OmnString			mImagePath;
	
	AosHtmlFileMap		mFileMap;
	AosHtmlFileVector	mFileVector;
	AosHtmlGicMap		mGicMap;
	int					mTotalFiles;

public:
	AosHtmlConfigMgr();
	~AosHtmlConfigMgr();

	bool			config(const AosXmlTagPtr &config);

	bool			initConfig(const AosXmlTagPtr &config);
	
	vector<int>		getFileIdVector(const OmnString &name);

	OmnString		getGicsLoadJs(
						const bool * flags,
						int &filenum,
						const bool isFullVpd);
	
	OmnString		getJsPath() {return mJsPath;} 
	OmnString		getCssPath() {return mCssPath;} 
	OmnString		getImagePath() {return mImagePath;} 
	OmnString		getFilePath() {return mFilePath;} 
	
	OmnString		createLoadJsFun(int loadFileNum);
	
	OmnString		createLoadJsFun_vpd();

	OmnString		createDocDtd(const int version);

	OmnString		createScriptHeadStr(const OmnString &fname);

	OmnString		createCssHeadStr(const OmnString &fname, const OmnString &theme);
	
	OmnString		createPngfixStr();

	OmnString		createHtmlHead(const AosXmlTagPtr &vpd);
	
	OmnString		createExtReady();

	OmnString		createPerformanceStart();
};

#endif

