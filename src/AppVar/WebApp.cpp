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
// 07/15/2011	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "AppVar/WebApp.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Objid.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"
#include <map>
#include <set>
#include <vector>
using namespace std;

OmnMutexPtr		AosWebApp::smLock = OmnNew OmnMutex();
map<OmnString, AosWebAppPtr> 	AosWebApp::smApps;

static bool sgSanityCheck = true;


AosWebApp::AosWebApp(
		const OmnString &url, 
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mUrl(url)
{
	// 1. Retrieve the URL Doc.
	OmnString newurl = url;
	OmnString url_objid = AosObjid::createUrlObjid(newurl);
	u32 siteid = rdata->getSiteid();
	OmnString ssid = rdata->getSsid();
	u64 urldocid = rdata->getUrldocDocid();
	AosXmlTagPtr urldoc = AosSengAdmin::getSelf()->retrieveDocByObjid(siteid, ssid, urldocid,url_objid); 
	OmnString errormsg;
	if (!urldoc)
	{
		errormsg = "Missing urldoc ! url :  ";
		errormsg << url;
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
		throw e;
		/*rdata->setError() << "Missing urldoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return;*/
	}
	
	OmnString delapp = urldoc->getAttrStr("zky_delapp");
	if(delapp == "true")
	{
		errormsg = "The Application instance ";
		errormsg << url << " have been temporary deleted !";
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
		throw e;
	}

	mLanguage = urldoc->getAttrStr("languge", "zh_cn");  
	mDefualtDataFormat  = urldoc->getAttrStr("dataFormat", "yyy_mm_dd:hh:mm:ss");

	OmnString appid = urldoc->getNodeText("zky_appid");
	if (appid != "")
	{
		AosXmlTagPtr app_doc = AosSengAdmin::getSelf()->retrieveDocByObjid(siteid, ssid, urldocid, appid);
		if (!app_doc)
		{
			errormsg = "Failed to get the application through the objid : ";
			errormsg << appid;
			OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
			throw e;
		}
		delapp = app_doc->getAttrStr("zky_delapp");
		if (delapp == "true")
		{
			errormsg = "The Application ";
			errormsg << appid << " have been temporary deleted !";
			OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
			throw e;
		}
	}

	mAppid = urldoc->getNodeText("zky_instid");//get the appid
	if(mAppid == "")
	{
		mAppid = "notapp";
		/*OmnString errormsg = "Missing Appid ! url : ";
		errormsg << url;
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
		throw e;*/
	}
	//aos_assert(mAppid != "");
	
	/*OmnString appbaseid = urldoc->getNodeText("zky_appid");//the base's url like ctnr
	if (appbaseid != "")
	{
		// Check whether it is already in the map.
		map<OmnString, AosWebAppPtr>::iterator itr;
		smLock->lock();
		itr = smApps.find(appbaseid);
		if (itr != smApps.end())
		{
			mBaseApp = itr->second;
			smLock->unlock();
		}
		else
		{
			smLock->unlock();
			try
			{
				mBaseApp = OmnNew AosWebApp(appbaseid, rdata);
			}
			catch (const OmnExcept &e)
			{
				errormsg = "Failed to get the app through the url : ";
				errormsg << url;
				OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
				throw e;
			}
		}
	}*/

	if (!init(mAppid, rdata))
	{
		errormsg = "Failed to get the app through the url : ";
		errormsg << url;
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
		throw e;
	}
	else
	{
		AosWebAppPtr thisptr(this, false);
		smLock->lock();
		bool rslt = sanityCheckBeforeAdding();
		if(!rslt)
		{
			smLock->unlock();
			errormsg = "The url : ";
			errormsg << url << "has in the map ! ";
			OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errormsg);
			throw e;
		}
		smApps.insert(make_pair(mUrl, thisptr));
		smLock->unlock();
	}
}


AosWebApp::~AosWebApp()
{
}


bool
AosWebApp::init(
		const OmnString &appid,
		const AosRundataPtr &rdata)
{
	// 3. Build the common map
	OmnString commonMap = AOSTAG_APPINST;
	commonMap << "_" << appid;
	u32 siteid = rdata->getSiteid();
	OmnString ssid = rdata->getSsid();
	u64 urldocid = rdata->getUrldocDocid();
	AosXmlTagPtr map = AosSengAdmin::getSelf()->retrieveDocByObjid(siteid, ssid, urldocid, commonMap); 
	if (map)
	{
		bool rslt = buildMap(map, "", rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	rdata->setOk();
	return true;
}


bool
AosWebApp::sanityCheckBeforeAdding()
{
	// This function assumes the entry [url, AosWebAppPtr] is not in 
	// the map yet. 
	if (!sgSanityCheck) return true;
	map<OmnString, AosWebAppPtr>::iterator itr;
	itr = smApps.find(mUrl);
	aos_assert_r(itr == smApps.end(), false);
	return true;
}


bool
AosWebApp::buildMap(
		const AosXmlTagPtr &maps, 
		const OmnString &vpd_objid, 
		const AosRundataPtr &rdata)
{
	// This function builds
	// 	<records type = "common|objid|ctnr">
	// 		<gicinsts>	
	//	  		<item id="21333" xpath="action/xxx" value"">description</item>
	//	  		<item id="xxx" xpath="@zky_valuebd" value"">description</item>
	//	  		...
	//	  	</gicinsts>
	//	  	<gictypes>
	//	  		<item id="gic_input" xpath="action/xxx" value"">description</item>
	//	  		<item id="xxx" xpath="@zky_valuebd" value"">description</item>
	//	  		...
	//	  	</gictypes>
	// 	</reconds>
	//
	// build map
	// the map's struct:
	// 1:
	// map< id|type, map<xpath, xmltag>>
	// Process GIC Instances
	aos_assert_rr(maps, rdata, false);
	AosXmlTagPtr records, record;
	OmnString key, xpath, id;
	if (vpd_objid != "")
	{
		// vpd_objid should not be in mSet. 
		if(sgSanityCheck)
		{
			set<OmnString>::iterator it;
			it = mSet.find(vpd_objid);
			aos_assert_rr(it == mSet.end(), rdata, false);
			mSet.insert(vpd_objid);
		}
		
		records = maps->getFirstChild("gicinsts");
		if (records)
		{
			record = records->getFirstChild();
			while (record)
			{
				// 'key' is:
				// 	AOSZTG_APPINST + "_" + gicinst_id + "_" + attrname
				id = record->getAttrStr("id", "");
				xpath = record->getAttrStr("xpath", "");
				if (id == "")
				{
					OmnAlarm << "id is empty" << enderr;
				}
				else
				{
					key = createKey(AOSZTG_GICID, vpd_objid, id);
					map<OmnString, map<OmnString, AosXmlTagPtr> >::iterator itr;
					itr = mMap.find(key);
					if(itr != mMap.end())
					{
						itr->second.insert(make_pair(xpath, record));	
					}
					else
					{
						map<OmnString, AosXmlTagPtr> cmap;
						cmap.insert(make_pair(xpath, record));
						mMap.insert(make_pair(key, cmap));
					}
				}
				record = records->getNextChild();
			}
		}
	}

	// Process GIC Type based entries
	records = maps->getFirstChild("gictype");
	if(records)
	{
		record = records->getFirstChild();
		while (record)
		{
			// 'key' is:
			// 	AOSZTG_APPINST + "_" + gicinst_id + "_" + attrname
			id = record->getAttrStr("id","");
			xpath = record->getAttrStr("xpath","");
			if (id == "")
			{
				OmnAlarm << "id is empty" << enderr;
			}
			else
			{
				key = createKey(AOSZTG_GICTYPE, vpd_objid, id);
				map<OmnString, map<OmnString, AosXmlTagPtr> >::iterator itr;
				itr = mMap.find(key);
				if(itr != mMap.end())
				{
					itr->second.insert(make_pair(xpath, record));	
				}
				else
				{
					map<OmnString, AosXmlTagPtr> cmap;
					cmap.insert(make_pair(xpath, record));
					mMap.insert(make_pair(key, cmap));
				}
			}
			record = records->getNextChild();
		}
	}

	rdata->setOk();
	return true;
}


AosWebAppPtr 
AosWebApp::getApp(const OmnString &url, const AosRundataPtr &rdata)
{
	// This function retrieves the instance of AosWebAppPtr based on a URL.
	// If the object is not there, it will create it and insert it into smApps.
	
	//smLock->lock();
	map<OmnString, AosWebAppPtr>::iterator itr;
	itr = smApps.find(url);
	AosWebAppPtr app;
	if (itr != smApps.end())
	{
		smApps.erase(itr);
		itr = smApps.find(url);
		if (itr != smApps.end())
		{
			OmnScreen << "none!" << endl;
		}

		app = OmnNew AosWebApp(url, rdata);
		return app;
	}
	
	//smLock->unlock();
	try
	{
		app = OmnNew AosWebApp(url, rdata);
		return app;
	}

	catch(const OmnExcept &e)
	{
		rdata->setError() << e.getErrmsg(); 
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
}


bool 
AosWebApp::resetAppData(
		const OmnString &url,
		const AosRundataPtr &rdata)
{
	smLock->lock();
	map<OmnString, AosWebAppPtr>::iterator itr;
	itr = smApps.find(url);  
	AosWebAppPtr app;
	if (itr != smApps.end())
	{
		app = itr->second;
		smLock->unlock();
		app->mMap.clear();
		bool rslt = app->init(app->mAppid, rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	smLock->unlock();
	return true;
}

bool 
AosWebApp::deleteApp(
		const OmnString &url,
		const OmnString &delapp,
		const AosRundataPtr &rdata)
{
	smLock->lock();
	map<OmnString, AosWebAppPtr>::iterator itr;
	itr = smApps.find(url);  
	AosWebAppPtr app;
	if (itr != smApps.end())
	{
		app = itr->second;
		if(delapp == "false")
		{
			app->mIsdel = "true";
		}
		else
		{
			smApps.erase(itr);
		}
	}
	smLock->unlock();
	return true;
}

bool 
AosWebApp::resetApp(
		const OmnString &url,
		const AosRundataPtr &rdata)
{
	smLock->lock();
	map<OmnString, AosWebAppPtr>::iterator itr;
	itr = smApps.find(url);  
	AosWebAppPtr app;
	if (itr != smApps.end())
	{
		app = itr->second;
		app->mIsdel = "false";
	}
	smLock->unlock();
	return true;
}


void
AosWebApp::comreplace(
		map<OmnString, AosXmlTagPtr> &map1,
		AosXmlTagPtr &vpd)
{
	map<OmnString, AosXmlTagPtr>::iterator itr1;
	for (itr1 = map1.begin(); itr1 != map1.end(); ++itr1)
	{
		OmnString xpath = itr1->first;
		AosXmlTagPtr value = itr1->second;
		//OmnString val = value->getNodeText();
		OmnString val = value->getAttrStr("value");
		vpd->xpathSetAttr(xpath, val);
	}
}

bool
AosWebApp::appConvert(
		AosXmlTagPtr &gic, 
		const AosXmlTagPtr &obj, 
		const AosRundataPtr &rdata)
{
	// This function replaces the values of 'gic' attributes that
	// have corresponding entries in mMap. 
	if (mBaseApp)
	{
		mBaseApp->appConvert(gic, obj, rdata);
	}

	AosXmlTagPtr vpdroot = gic->getRoot();
	aos_assert_rr(vpdroot, rdata, false);
	vpdroot = vpdroot->getFirstChild("Contents");
	aos_assert_rr(vpdroot, rdata, false);
	vpdroot = vpdroot->getFirstChild();
	aos_assert_rr(vpdroot, rdata, false);
	OmnString objid = vpdroot->getAttrStr(AOSTAG_OBJID, "");
	OmnString id = gic->getAttrStr(AOSTAG_COMPID);
	OmnString type  = gic->getAttrStr("gic_type");
	map<OmnString, map<OmnString, AosXmlTagPtr> >::iterator itr1;
	//aos_assert_rr(id != "", rdata, false);
	aos_assert_rr(objid != "", rdata, false);
	aos_assert_rr(type != "", rdata, false);
	set<OmnString>::iterator setitr;

	//common type
	itr1 = mMap.find(createKey(AOSZTG_GICID, "", type));
	if(itr1 != mMap.end())
	{
		comreplace(itr1->second, gic);
	}

	u32 siteid = rdata->getSiteid();
	OmnString ssid= rdata->getSsid();
	u64 urldocid = rdata->getUrldocDocid();

	//specific type
	setitr = mSet.find(objid);
	if (setitr == mSet.end())
	{
		OmnString vpd_objid = AOSTAG_APPINST;
		vpd_objid << "_" << mAppid << "_" << objid;
		AosXmlTagPtr map = AosSengAdmin::getSelf()->retrieveDocByObjid(siteid, ssid, urldocid, vpd_objid);
		if(map)
		{
			buildMap(map, objid, rdata);
		}
		else
		{
			mSet.insert(objid);
		}
	}
	itr1 = mMap.find(createKey(AOSZTG_GICTYPE, objid, type));
	if (itr1 != mMap.end())
	{
		comreplace(itr1->second, gic);
	}

	// specific id
	itr1 = mMap.find(createKey(AOSZTG_GICID, objid, id));
	if(itr1 != mMap.end())
	{
		comreplace(itr1->second, gic);
	}
	rdata->setOk();
	return true;
}

