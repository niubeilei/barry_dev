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
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "Actions/ActAddAttr.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/QueryMgr.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/XmlDoc.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SearchEngine/IIL.h"
#include "SearchEngine/IILMgr.h"
#include "SearchEngine/WordMgr.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "SearchEngine/DocServer.h"
#include "SeAdmin/SeAdmin.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "SEServer/ImgProc.h"



#include <stdlib.h>

int gAosLogLevel = 1;
#include <dirent.h>

int 
main(int argc, char **argv)
{
	//aos_global_data_init();
	int index = 1;
	OmnApp theApp(argc, argv);
	u64 startdocid, enddocid;
	OmnString siteid;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}
		if (strcmp(argv[index], "-addCloudid") == 0)
		{
			siteid = argv[index +1];
			startdocid = atoll(argv[index+2]);
			enddocid = atoll(argv[index+3]);
		}

		index++;

	}


	bool isRepairing = false;
	AosIIL::staticInit(OmnApp::getAppConfig());
	AosXmlDoc::staticInit(OmnApp::getAppConfig());
	AosWordParser::init(OmnApp::getAppConfig());
	try
	{
		theApp.startSingleton(OmnNew AosWordMgrSingleton());
		theApp.startSingleton(OmnNew AosDocServerSingleton());
		theApp.startSingleton(OmnNew AosIILMgrSingleton());
		AosIILMgrSelf->start(OmnApp::getAppConfig());

		OmnString dirname = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_DIRNAME);
		OmnString logfname = OmnApp::getAppConfig()->getAttrStr("logfilename");
		OmnString wordhashName = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORDID_HASHNAME);
		u32 wordidTablesize = OmnApp::getAppConfig()->getAttrU64(AOSCONFIG_WORDID_TABLESIZE, 0);
		    AosWordMgrSelf->start(dirname, wordhashName, wordidTablesize);

		AosDocServerSelf->setRepairing(isRepairing);
		AosDocServerSelf->start(OmnApp::getAppConfig());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}


	for (u64 docid=startdocid; docid<enddocid; docid++)
	{
OmnScreen <<"####### docid:" << docid <<endl;
if (docid == 58245)
	OmnMark;
		AosXmlTagPtr doc = AosDocServerSelf->getDoc(docid);
		if (!doc) continue;

		u64 creator = doc->getAttrU64(AOSTAG_CREATOR,AOS_INVDID);
		if (creator == AOS_INVDID)
		{
			OmnAlarm << "Doc missing creator " << creator << " docid:"<< docid << enderr;
			continue;
		}

		AosXmlTagPtr udoc = AosDocServerSelf->getDoc(creator);
		if (udoc)
		{
			//creator 是userid 并是一条userAccount
			if (udoc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT)
			{
				// The creator is the docid of the user's account doc. Need
				// to change to cloud id.
				OmnString cid = udoc->getAttrStr(AOSTAG_CLOUDID);
				if (cid == "")
				{
					// The user account did not have a cloud id yet. Need to 
					// add one.
					cid = AosCloudidSvr::getCloudid(creator);
					aos_assert_r(cid!="", false);
					AosDocServerSelf->modifyDoc(udoc, AOSTAG_CLOUDID, cid, false, true, true);
					AosXmlTagPtr aa = AosDocServerSelf->getDoc(creator);
				}

				// Need to modify the creator
				AosDocServerSelf->modifyDoc(doc, AOSTAG_CREATOR, cid, false, true, true);

			}
			else
			{
				// The retrieve is not a user account. This is an error.
				OmnString type = udoc->getAttrStr(AOSTAG_OTYPE);
				OmnAlarm << "Missing user Account!"<< " creator: "<< creator 
					<<" otype: "<< type << enderr;
				continue;
				
			}
		}
		else
		{
			// Did not retrieve a doc. It could be the case that 'creator' 
			// is a cloud id, not docid.
			OmnString cid;
			cid << creator;
			udoc = AosDocServerSelf->getDocByCloudid(siteid, cid);
			if (!udoc)
			{
				OmnAlarm <<"Missing cid!"<< enderr;
			}
			else
			{
				// 'creator' is a cloud id. Do nothing
				if (udoc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT)
				{
					OmnString crtor ;
					crtor << creator;
					aos_assert_r(udoc->getAttrStr(AOSTAG_CLOUDID) == crtor, false);
				}
				else
				{
					OmnAlarm << "Missing user Account!"<< enderr;
					return false;
				}
			}
		}
	}

	return true;
}
