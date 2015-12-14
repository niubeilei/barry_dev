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
// 08/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEModules/SiteMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Rundata/Rundata.h"
#include "SEModules/Site.h"
#include "SEModules/LoginMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosSiteMgrSingleton,
                 AosSiteMgr,
                 AosSiteMgrSelf,
                "AosSiteMgr");

u64 AosSiteMgr::smDefaultSiteId = 100;

AosSiteMgr::AosSiteMgr()
:
mLock(OmnNew OmnMutex())
{
	AosSiteMgrObj::setSiteMgr(this);
}


AosSiteMgr::~AosSiteMgr()
{
}


bool
AosSiteMgr::init(const AosRundataPtr &rdata)
{
	// 	<config ...>
	// 		<AOSCONFIG_SITEMGR>
	// 			<sites>
	// 				<site .../>
	// 				...
	// 			</sites>
	// 		</AOSCONFIG_SITEMGR>
	// 		...
	// 	</config>
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_rr(conf, rdata, false);
	AosXmlTagPtr sitemgrconf = conf->getFirstChild("sitemgr");
	if (!sitemgrconf) return true;

	AosXmlTagPtr sitestag = sitemgrconf->getFirstChild();
	aos_assert_rr(sitestag, rdata, false);

	AosXmlTagPtr sitetag = sitestag->getFirstChild();
	while (sitetag)
	{
		u32 siteid = sitetag->getAttrU32(AOSTAG_SITEID,0);
		if (siteid == 0) 
		{
			rdata->setError() << "siteid is empty: " << sitetag->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
		else
		{
			try 
			{
				// Chen Ding, 2013/02/11
				// AosSitePtr site = OmnNew AosSite(AOS_DEFAULT_SITE, rdata);
				AosSitePtr site = OmnNew AosSite(smDefaultSiteId, rdata);
				mSites[siteid] = site;

				if (sitetag->getAttrBool("create", true))
				{
					site->init(rdata);
				}

				if (sitetag->getAttrBool(AOSTAG_DEFAULT_SITE, false))
				{
					mDftSite = site;
				}
			}

			catch (...)
			{
				OmnAlarm << "Failed creating the default site: " << enderr;
				return false;
			}
		}
		sitetag = sitestag->getNextChild();
	}


	mLanguageCode = sitemgrconf->getAttrStr("lang_code");
	return true;
}


bool      	
AosSiteMgr::start()
{
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->reset();
	aos_assert_r(init(rdata), false);
	return true;
}


bool        
AosSiteMgr::stop()
{
	return true;
}


bool
AosSiteMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


// Chen Ding, 07/29/2011
OmnString
AosSiteMgr::getUnknownCloudid(const AosRundataPtr &rdata)
{
	AosSitePtr site = getSite(rdata);
	if (!site)
	{
		rdata->setError() << "Failed retrieving the site: " << rdata->getSiteid();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return "";
	}

	return site->getUnknownCloudid(rdata);
}


OmnString
AosSiteMgr::getGuestCloudid(const AosRundataPtr &rdata)
{
	AosSitePtr site = getSite(rdata);
	if (!site)
	{
		rdata->setError() << "Failed retrieving the site: " << rdata->getSiteid();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return "";
	}

	return site->getGuestCloudid(rdata);
}


AosSitePtr
AosSiteMgr::getSite(const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	if (siteid == 0) 
	{
		rdata->setError() << "To retrieve a site but the siteid is null";
		return 0;
	}

	mLock->lock();
	SiteItr itr = mSites.find(siteid);
	if (itr == mSites.end())
	{
		mLock->unlock();
		rdata->setError() << "Site not found: " << siteid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	AosSitePtr site = itr->second;
	mLock->unlock();
	rdata->setOk();
	return site;
}


OmnString 
AosSiteMgr::getLanguageCode(const AosRundataPtr &rdata)
{
	AosSitePtr site = getSite(rdata);
	if (!site)
	{
		return mLanguageCode;
	}
	return site->getLanguageCode(rdata);
}


AosLocale::E
AosSiteMgr::getLocale(const AosRundataPtr &rdata)
{
	AosSitePtr site = getSite(rdata);
	if (!site)
	{
		// AosSetError(rdata, AosErrmsgId::eFailedRetrieveSite) << rdata->getSiteid();
		// OmnAlarm << rdata->getErrmsg() << enderr;
		return AosLocale::getDftLocale();
	}

	return site->getDftLocale();
}

