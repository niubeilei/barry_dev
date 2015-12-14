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
#include "HtmlServer/HtmlConfigMgr.h"

#include "Util/OmnNew.h"
#include "Util/File.h"


AosHtmlConfigMgr::AosHtmlConfigMgr()
:
mLock(OmnNew OmnRwLock()),
mConfiged(false)
{
}


AosHtmlConfigMgr::~AosHtmlConfigMgr()
{
	mConfiged = false;
	mFileMap.clear();
	mFileVector.clear();
	mGicMap.clear();
}


bool
AosHtmlConfigMgr::initConfig(const AosXmlTagPtr &cfg)
{
	mLock->writelock();
	bool rslt = config(cfg);
	mLock->unlock();
	return rslt;
}

bool
AosHtmlConfigMgr::config(const AosXmlTagPtr &config)
{
    aos_assert_r(config, false);
	
	mJsPath = config->getAttrStr(AOSHTML_JSPATH);
	mCssPath = config->getAttrStr(AOSHTML_CSSPATH);
   	mImagePath = config->getAttrStr(AOSHTML_IMAGEPATH);
	mFilePath = config->getAttrStr(AOSHTML_FILEPATH);
   	aos_assert_r(mFilePath != "", false);

	// Ken Lee, 2013/04/11
	bool check_file_exist = config->getAttrBool(AOSHTML_CHECKFILEEXIST, true);

	AosXmlTagPtr def = config->getFirstChild("files");
    aos_assert_r(def, false);
    
    mTotalFiles = 0;
	mFileVector.clear();
	mFileMap.clear();
	mGicMap.clear();

	AosXmlTagPtr filedef = def->getFirstChild();
    while (filedef)
    {
		OmnString name = filedef->getAttrStr("name");
		AosHtmlFileMapItr itr = mFileMap.find(name);
		if(itr != mFileMap.end())
		{
			OmnAlarm << "the file name is exist, file name : " << name << enderr;
			return false;
		}
		
		OmnString src = filedef->getAttrStr("src");
		OmnString path = mFilePath;
		path << src;
		u32 lastMdfTime = OmnFile::getLastModifyTime(path);
		if(lastMdfTime == 0 && check_file_exist)
		{
			OmnAlarm << "failed to get the file's last modify time, filepath : " << path << enderr;
			return false;
		}
		
		AosHtmlFileStructPtr file = OmnNew AosHtmlFileStruct();
		file->mName = name;
		file->mSrc = src;
		file->mFname = filedef->getAttrStr("fname");
      	file->mFileId = mTotalFiles++;
		file->mLastMdfTime = lastMdfTime;

		mFileVector.push_back(file);
		mFileMap.insert(make_pair(file->mName, file->mFileId));
        
		filedef = def->getNextChild();
    }

	/*
    for(AosHtmlFileMapItr itr = mFileMap.begin(); itr != mFileMap.end(); itr++)
    {
		OmnScreen << itr->first << "----------- " << itr->second << endl;
    }
	*/

    AosXmlTagPtr gicsdef = config->getFirstChild("gics");
    aos_assert_r(gicsdef, false);
    
	AosXmlTagPtr gicdef = gicsdef->getFirstChild();
    while (gicdef)
    {
		// Chen Ding, 2011/01/13
		OmnString type = gicdef->getAttrStr("type");
		AosHtmlGicMapItr itr = mGicMap.find(type);
		if(itr != mGicMap.end())
		{
			OmnAlarm << "the gic is exist, gic type : " << type << enderr;
			return false;
		}
		
		AosHtmlGicStructPtr gic = OmnNew AosHtmlGicStruct();
		gic->mGicType = type;

		AosXmlTagPtr dd = gicdef->getFirstChild();
		while (dd)
		{
			OmnString name = dd->getAttrStr("name");
			AosHtmlFileMapItr itr = mFileMap.find(name);
			if(itr == mFileMap.end())
			{
			    OmnAlarm << "can not find the file, gictype:" << type << ", file name : " << name << enderr;
			    return false;
			}
			int fileid = itr->second;
			gic->mFileIds.push_back(fileid);
			
			dd = gicdef->getNextChild();
		}
		mGicMap.insert(make_pair(type, gic));

		gicdef = gicsdef->getNextChild();
    }
	mConfiged = true;
    return true;
}


vector<int>
AosHtmlConfigMgr::getFileIdVector(const OmnString &name)
{
	vector<int> vector;
	aos_assert_r(mConfiged, vector);
	
	if(name == "") return vector;
    
	mLock->readlock();
	AosHtmlGicMapItr itr = mGicMap.find(name);
	if(itr != mGicMap.end())
	{
		AosHtmlGicStructPtr gic = itr->second;
		vector = gic->mFileIds;
	}
	mLock->unlock();
	return vector;
}


OmnString
AosHtmlConfigMgr::getGicsLoadJs(
		const bool * flags,
		int &filenum,
		const bool isFullVpd)
{
	filenum = 0;
	OmnString str;
	mLock->readlock();
	for (int i=0; i<mTotalFiles; i++)
	{
		if(!flags[i]) continue;

		AosHtmlFileStructPtr file = mFileVector[i];
		str << (isFullVpd ? "AosLoadJs" : "AosLoadJs_vpd");
		str << "('" << mJsPath << file->mSrc << "?lmt=" << file->mLastMdfTime << "', '"
		    << file->mFname <<"', " << file->mFileId << "); ";
		filenum++;
	}
	mLock->unlock();
	return str;
}


OmnString
AosHtmlConfigMgr::createLoadJsFun(int loadFileNum)
{
    OmnString str;
	str <<
	"var gAosDwdFiles = new Array();" << 
	"var gAosOrderedFiles = new Array();" <<
	"var gAosVpdJSFiles = new Array();" <<
	"var gAosLastStartedIdx = new Array();" <<
	"var gAosTotalJSToLoad = new Array();" <<
	"var gAosTotalDwds = " << loadFileNum << ";" <<
	"var gAosLastFileToStart = 0;" <<
	"function AosDwdFileInfo(src, fname, elem, fileid){" <<
		"this.src = src;" <<
		"this.fname = fname;" <<
		"this.loaded = false;" <<
		"this.started = false;" <<
		"this.elem = elem;" <<
		"this.fileid = fileid;" <<
	"};" <<
	"function AosLoadJs(src, fname, fileid){" <<
		"var reloadJs = false;" <<
		"if (gAosDwdFiles[fileid]){" <<
			"if(gAosDwdFiles[fileid].reload){" <<
				"reloadJs = true;" <<
			"}else{" <<
				"return;" <<
			"}" <<
		"}" <<
		"var scriptElem = document.createElement('script');" <<
		"scriptElem.src  = src;" <<
		"if(reloadJs){" <<
			"var ff = gAosDwdFiles[fileid];" <<
			"ff.order = gAosDwdFiles[fileid].reloadOrder;" <<
			"ff.elem = scriptElem;" <<
			"gAosOrderedFiles[ff.order] = ff;" <<
		"}else{" <<
			"var ff = new AosDwdFileInfo(src, fname, scriptElem, fileid);" <<
			"gAosDwdFiles[fileid] = ff;" <<
			"ff.order = gAosOrderedFiles.length;" <<
			"gAosOrderedFiles.push(ff);" <<
		"}" <<
		"document.getElementsByTagName('head')[0].appendChild(scriptElem);" <<
		"if(document.all){" <<
			"scriptElem.onreadystatechange = function(){" <<
				"if(this.readyState == 4 ||" <<
				"this.readyState == 'complete' || " <<
				"this.readyState == 'loaded'){" <<
					"AosCallback(fileid, fname);" <<
				"}" <<
			"};" <<
		"}else{" <<
			"scriptElem.onload = function(){" <<
				"AosCallback(fileid, fname);" <<
			"};" <<
		"}" <<
	"};" <<
	"function AosCallback(fileid, fname){" <<
		"var fileinfo = gAosDwdFiles[fileid];" <<
		"if (!fileinfo){" <<
			"return;" <<
		"}" <<
		"fileinfo.loaded = true;" <<
		"var idx = fileinfo.order;" <<
		"if(!window[fname]) {" <<
			"fileinfo.loaded = false;" <<
			"gAosDwdFiles[fileid].reload = true;" <<
			"gAosDwdFiles[fileid].reloadOrder = idx;" <<
			"gAosDwdFiles[fileid].reloadCount = gAosDwdFiles[fileid].reloadCount||0;" <<
			"if(gAosDwdFiles[fileid].reloadCount++ > 3){" <<
				"if(window.console)console.log('********************'+fname+'Js file download error!!!*****************');" <<
				"return;" <<
			"}" <<
			"AosLoadJs(fileinfo.src ,fileinfo.fname, fileid);" <<
			"return;" <<
		"}" <<
		"while (gAosLastFileToStart<idx){" <<
			"var ff = gAosOrderedFiles[gAosLastFileToStart];" <<
			"if (!ff || !ff.loaded){return;}" <<
			"if (!ff.started){" <<
				"window[ff.fname].call();" <<
				"ff.started = true;" <<
			"}" <<
			"gAosLastFileToStart++;" <<
		"}" <<
		"window[fname].call();" <<
		"fileinfo.started = true;" <<
		"gAosLastFileToStart = idx+1;" <<
		"while (gAosLastFileToStart < gAosTotalDwds){" <<
			"var ff = gAosOrderedFiles[gAosLastFileToStart];" <<
			"if (!ff || !ff.loaded){return;}" <<
			"if (!ff.started){" <<
				"window[ff.fname].call();" <<
				"ff.started = true;" <<
			"}" <<
			"gAosLastFileToStart++;" <<
		"}" <<
		"if (gAosLastFileToStart >= gAosTotalDwds){" <<
			"AosStartApp();" <<
		"}" <<
		"if (fileinfo.listeners){" <<
			"for (var i=0; i<fileinfo.listeners.length; i++){" <<
				"AosCheckFileLoaded(fileid, fileinfo.fname, " <<
				"fileinfo.listeners[i], " <<
				"fileinfo.orders[i]);" <<
			"}" <<
		"}" <<
	"};" <<
	"function AosAreAllJSLoaded(transid){" <<
		"var files = gAosVpdJSFiles[transid];" <<
		"if (!files){return;}" <<
		"for (var i=0; i<files.length; i++){" <<
			"var ff = gAosDwdFiles[files[i]];" <<
			"if (!ff || !ff.started){return false;} " <<
		"}" <<
		"AosStartApp_vpd(transid);" <<
		"gAosVpdJSFiles[transid] = [];" <<
		"return true;" <<
	"};";
    return str;
 }


OmnString
AosHtmlConfigMgr::createLoadJsFun_vpd()
{
    OmnString str;
	str <<
	"function AosLoadJs_vpd(src, fname, fileid){" <<
		"var transid = aos_vpdtransid;" <<
		"var scriptElem = document.createElement('script');" <<
		"scriptElem.src = src;" <<
		"var ff = gAosDwdFiles[fileid];" <<
		"if(ff && ff[transid] && ff[transid].reload){" <<
			"var order = gAosDwdFiles[fileid][transid].reloadOrder;" <<
			"ff.elem = scriptElem;" <<
			"gAosDwdFiles[fileid] = ff;" <<
			"gAosVpdJSFiles[transid][order] = fileid;" <<
		"}else{" <<
			"var order = gAosVpdJSFiles[transid].length;" <<
			"gAosVpdJSFiles[transid].push(fileid);" <<
			"if(ff){" <<
				"if(!ff.listeners){" <<
					"ff.listeners = new Array();" <<
					"ff.orders = new Array();" <<
				"}" <<
				"ff.listeners.push(transid);" <<
				"ff.orders.push(order);" <<
				"return;" <<
			"}" <<
			"var ff = new AosDwdFileInfo(src, fname, scriptElem);" <<
			"gAosDwdFiles[fileid] = ff;" <<
		"}" <<
		"document.getElementsByTagName('head')[0].appendChild(scriptElem);" <<
		"scriptElem.onreadystatechange = function(){" <<
			"if(this.readyState == 4 || this.readyState == 'complete' || this.readyState == 'loaded'){" <<
				"AosCallback_vpd(fileid, fname, transid, order);" <<
			"}" <<
		"};" <<
		"scriptElem.onload = function(){" <<
			"AosCallback_vpd(fileid, fname, transid, order);" <<
		"};" <<
	"};" <<
	"function AosCallback_vpd(fileid, fname, transid, order){" <<
		"var crt_ff = gAosDwdFiles[fileid];" <<
		"if (!crt_ff){return;}" <<
		"AosCheckFileLoaded(fileid, fname, transid, order);" <<
		"if (crt_ff.listeners){" <<
			"for (var i=0; i<crt_ff.listeners.length; i++){" <<
				"AosCheckFileLoaded(fileid, fname, crt_ff.listeners[i], crt_ff.orders[i]);" <<
			"}" <<
		"}" <<
	"}" <<
	"function AosCheckFileLoaded(fileid, fname, transid, order){" <<
		"var crt_ff = gAosDwdFiles[fileid];" <<
		"crt_ff.loaded = true;" <<
		"var crt_idx = order;" <<
		"var last_idx = gAosLastStartedIdx[transid];" <<
		"if(!window[fname]){" <<
			"crt_ff[transid] = {};" <<
			"gAosDwdFiles[fileid].loaded = false;" <<
			"crt_ff[transid].reload = true;" <<
			"crt_ff[transid].reloadOrder = order;" <<
			"gAosDwdFiles[fileid].reloadCount = gAosDwdFiles[fileid].reloadCount||0;" <<
			"if(gAosDwdFiles[fileid].reloadCount++>3){" <<
				"if(window.console){" <<
					"console.log('***********'+fname+'download error************');" <<
				"}" <<
				"return;" <<
			"}" <<
			"AosLoadJs_vpd(crt_ff.src ,crt_ff.fname, fileid);" <<
			"return;" <<
		"}" <<
		"while (last_idx < crt_idx){" <<
			"var pos = gAosVpdJSFiles[transid][last_idx];" <<
			"var ff = gAosDwdFiles[pos];" <<
			"if (!ff || !ff.loaded){" <<
				"gAosLastStartedIdx[transid] = last_idx;" <<
				"return;" <<
			"}" <<
			"if (!ff.started){" <<
				"window[ff.fname].call();" <<
				"ff.started = true;" <<
			"}" <<
			"last_idx++;" <<
		"}" <<
		"window[fname].call();" <<
		"crt_ff.started = true;" <<
		"last_idx = crt_idx+1;" <<
		"var total = gAosTotalJSToLoad[transid];" <<
		"while (last_idx < total){" <<
			"var pos = gAosVpdJSFiles[transid][last_idx];" <<
			"var ff = gAosDwdFiles[pos];" <<
			"if (!ff || !ff.loaded){" <<
				"gAosLastStartedIdx[transid] = last_idx;" <<
				"return;" <<
			"}" <<
			"if (!ff.started){" <<
				"window[ff.fname].call();" <<
				"ff.started = true;" <<
			"}" <<
			"last_idx++;" <<
		"}" <<
		"gAosLastStartedIdx[transid] = last_idx;" <<
		"if (last_idx >= total){" <<
			"AosStartApp_vpd(transid);" <<
			"gAosVpdJSFiles[transid] = [];" <<
		"}" <<
	"};";
	return str;
}


OmnString
AosHtmlConfigMgr::createDocDtd(const int version)
{
	if(version != 4) return "";
	return "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">";
}


OmnString
AosHtmlConfigMgr::createPngfixStr()
{
	OmnString script;
	script << "<script type=\"text/javascript\">"
		   << "var gAosPngPic = new Array();"
		   << "function pngImageLoad(caller){"
		   << "if(window.$) {$(caller).pngFix();} "
		   << "else {gAosPngPic.push(caller);}"
		   << "}"
		   << "function error(el){"
		   << "var hasload = el.hasload;"
		   << "if(!hasload){"
		   << "var url = \"http://zhiliantianxia.com/images/\";"
		   << "var imgs = el.src.split(\"/\");"
		   << "var len = imgs.length;"
		   << "url += imgs[len-2] + \"/\" + imgs[len-1];"
		   << "el.hasload = true;"
		   << "el.src = url;}"
		   << "}"
		   << "</script>";
	return script;
}

OmnString
AosHtmlConfigMgr::createScriptHeadStr(const OmnString &fname)
{
	mLock->readlock();
	OmnString str = "";
	AosHtmlFileMapItr itr = mFileMap.find(fname);
	if(itr == mFileMap.end()) return str;
	
 	int fileid = itr->second;
	AosHtmlFileStructPtr file = mFileVector[fileid];
	if(!file) return str;
		
	str << "<script type=\"text/javascript\" src=\"" << mJsPath
		<< file->mSrc << "?lmt=" << file->mLastMdfTime << "\"></script>";
	mLock->unlock();
	return str;
}

	
OmnString
AosHtmlConfigMgr::createCssHeadStr(const OmnString &fname, const OmnString &theme)
{
	mLock->readlock();
	OmnString str;
	str << "<link rel=\"stylesheet\" type=\"text/css\" ";
	if(theme != "") str << "title=\"" << theme << "\" ";
	str << "href=\"" << mCssPath << fname << ".css\" />";
	mLock->unlock();
	return str;
}


OmnString
AosHtmlConfigMgr::createHtmlHead(const AosXmlTagPtr &vpd)
{
	OmnString head;
	aos_assert_r(vpd, head);

	mLock->readlock();
		
	OmnString title = vpd->getAttrStr("title", "Zkyie App");
	OmnString keywords = vpd->getAttrStr("keywords", "Zkyie App");
	OmnString theme = vpd->getAttrStr("theme", "Gray");
	OmnString theme_lower = "xtheme-";
	theme_lower << theme;
	theme_lower.toLower();
	OmnString icon;
	OmnString src = vpd->getAttrStr("icon_src", "");
	if(src != "") icon << mImagePath << src;
	if(icon == "") icon << mJsPath << "extjs/resources/images/favicon.ico";
	
	head << "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=EmulateIE8\" />"
		 << "<meta http-equiv=\"pragma\" content=\"no-cache\" />"
	     << "<meta http-equiv=\"Cache-Control\" content=\"no-cache,must-revalidate\" />"
	     << "<meta http-equiv=\"expires\" content=\"Wed, 26 Feb 1997 08:21:57 GMT\" />"
	     << "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />"
	     << "<meta name=\"Keywords\" content=\"" << keywords << "," << title << "\" />"
	     << "<link rel=\"shortcut icon\" href=\"" << icon <<"\" type=\"image/x-icon\"/>"
	     << "<link rel=\"bookmark\" href=\"" << icon << "\" type=\"image/x-icon\"/>"
	     << "<title>"<< title <<"</title>"
	     << createCssHeadStr("ext-all-notheme", "")
		 << createCssHeadStr(theme_lower, theme)
		 << createCssHeadStr("Aos", "")
		 << createCssHeadStr("AosSys", "");
		 
	OmnString app_css = vpd->getAttrStr("app_css", "");
	if(app_css != "") head << createCssHeadStr(app_css, "");

	//head << createScriptHeadStr("jquery");
	mLock->unlock();
	return head;
}


OmnString
AosHtmlConfigMgr::createExtReady()
{
	mLock->readlock();
	OmnString str;
	str << "Ext.isReady=true;"
		<< "Ext.BLANK_IMAGE_URL='" << mJsPath << "extjs/resources/images/default/s.gif';"
		<< "Ext.QuickTips.init();"
		<< "Ext.state.Manager.setProvider(new Ext.state.CookieProvider());";
	mLock->unlock();
	return str;
}


OmnString
AosHtmlConfigMgr::createPerformanceStart()
{
	OmnString str;
	str << "start_time = new Date().getTime(); "
		<< "new Aos.AosWindow(extConfig); "
		<< "end_time = new Date().getTime(); "
		<< "time = end_time-start_time; "
		<< "alert(time + \"ms\");";
	return str;
}
