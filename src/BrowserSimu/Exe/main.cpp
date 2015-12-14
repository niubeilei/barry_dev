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
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/QueryMgr.h"
#include "SEUtil/DocFileMgr.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlDoc.h"
#include "IILMgr/IILDocid.h"
#include "SEUtil/XmlTag.h"
#include "IdGen/IdGenMgr.h"
#include "SEModules/LogMgr.h"
#include "BrowserSimu/BrowserSimu.h"
#include "BrowserSimu/Ptrs.h"
#include "BrowserSimu/BrowserThrd.h"
#include "SEClient/SqlClient.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "WordMgr/WordMgr1.h"
#include "SearchEngine/DocServer.h"
#include "IILMgr/IILMgr.h"
#include "SearchEngine/DataReader/action_conv.h"
#include "SEServer/Ptrs.h"
#include "WebServerTorturer/WebClient.h"
#include "UtilComm/Ptrs.h"

#include <stdlib.h>

int gAosLogLevel = 1;
int num_threads = 0;
static OmnString sgSiteid = "100";
static bool setConfig(const char *fname);

#include <dirent.h>

AosXmlTagPtr gAosAppConfig;


// extern bool AosDataReader_convertData();
extern bool AosConvertXmlDoc(OmnString &xml);
extern bool AosDataReader_sendReq(const OmnString &contents);
/*
extern bool AosDataReader_ReplaceText(
				const OmnString &pattern, 
				const OmnString &value);
				*/

int 
main(int argc, char **argv)
{
	int index = 1;
	aos_global_data_init();
	int num_thrds = 10;

	OmnApp theApp(argc, argv);

	OmnString pattern, value;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			setConfig(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-reread") == 0 && index + 2 < argc)
		{
			// It is to re-read all the docs 
			// 	-reread <max_docid> <siteid> <doc_idx_fname> <doc_fname>
			u64 max_docid = atoi(argv[index+1]);
			OmnString siteid = argv[index+2];
			OmnString idxfname = argv[index+3];
			OmnString docfname = argv[index+4];
			AosBrowserSimu::getSelf()->rereadDocs(siteid, max_docid, idxfname, docfname);
			break;
		}

		if (strcmp(argv[index], "-readdocid") == 0)
		{
			// Read a doc by docid
			u64 docid = (u64)atoi(argv[index+1]);
			OmnString siteid = argv[index+2];
			OmnString idxfname = argv[index+3];
			OmnString docfname = argv[index+4];
			AosXmlTagPtr doc = AosBrowserSimu::getSelf()->readDocByDocid(docid, siteid, 
					idxfname, docfname);
			if (!doc)
			{
				cout << "Failed to read the doc: " << docid << endl;
				exit(0);
			}
			cout << (char *)doc->getData() << endl;
			exit(0);
			break;
		}

		if (strcmp(argv[index], "-buildbyobjid") == 0)
		{
			// Read a doc by objid 
			// 	admin -readobjid objid maxdocid siteid idxfname docfname
			OmnString objid = argv[index+1];
			u64 maxdocid = atoi(argv[index+2]);
			OmnString siteid = argv[index+3];
			OmnString idxfname = argv[index+4];
			OmnString docfname = argv[index+5];
			bool readDeleted = (strcmp(argv[index+6], "true") == 0);
			AosBrowserSimu::getSelf()->buildDocByObjid(objid, maxdocid, siteid, 
					idxfname, docfname, readDeleted);
			exit(0);
			break;
		}

		if (strcmp(argv[index], "-readobjid") == 0)
		{
			// Read a doc by objid 
			// 	admin -readobjid objid maxdocid siteid idxfname docfname readdelete
			OmnString objid = argv[index+1];
			u64 maxdocid = atoi(argv[index+2]);
			OmnString siteid = argv[index+3];
			OmnString idxfname = argv[index+4];
			OmnString docfname = argv[index+5];
			bool readDeleted = (strcmp(argv[index+6], "true") == 0);
			AosBrowserSimu::getSelf()->start(gAosAppConfig);
			AosBrowserSimu::getSelf()->readDocByObjid(objid, maxdocid, siteid, readDeleted);
			exit(0);
			break;
		}

		if (strcmp(argv[index], "-readctnr") == 0)
		{
			// Read docs by containers 
			// 	admin -readctnr container maxdocid siteid idxfname docfname deleted
			OmnString container = argv[index+1];
			u64 maxdocid = atoi(argv[index+2]);
			OmnString siteid = argv[index+3];
			OmnString idxfname = argv[index+4];
			OmnString docfname = argv[index+5];
			bool readDeleted = (strcmp(argv[index+6], "true") == 0);
			AosBrowserSimu::getSelf()->start(gAosAppConfig);
			AosBrowserSimu::getSelf()->readDocsByContainer(container, maxdocid, siteid, 
					idxfname, docfname, readDeleted);
			exit(0);
			break;
		}

		if (strcmp(argv[index], "-rebuilddoc") == 0)
		{
			// Rebuild doc:
			// 	admin -s -rebuilddoc <docid> <siteid> <idxfname> <docfname>
			//
			// Example:
			// 	admin -s -rebuilddoc 12345 100 /home1/AOS/Data/doc_idx 
			// 		/home1/AOS/Data/doc
			u64 docid = (u64)atoi(argv[index+1]);
			OmnString siteid = argv[index+2];
			OmnString idxfname = argv[index+3];
			OmnString docfname = argv[index+4];
			bool rslt = AosBrowserSimu::getSelf()->rebuildDoc(docid, siteid, 
					idxfname, docfname);
			cout << "Rebuild doc: " << docid  << ":" << rslt << endl;
			exit(0);
			break;
		}


		if (strcmp(argv[index], "-imagepath") == 0 && index + 1 < argc)
		{
			// It is to replace texts
			OmnString path = argv[index+1];
			index += 2;
			AosBrowserSimu::getSelf()->setImagePath(path);
			continue;
		}

		if (strcmp(argv[index], "-rebuilddb") == 0)
		{
			u64 maxDocid = atoi(argv[index+1]);
			OmnString siteid(argv[index+2]);
			AosBrowserSimu::getSelf()->start(gAosAppConfig);
			AosBrowserSimu::getSelf()->rebuildDb(maxDocid, siteid);
			break;
		}

		if (strcmp(argv[index], "-num_thrds") == 0)
		{
			num_thrds = atoi(argv[index+1]);
			AosBrowserSimu::getSelf()->start(gAosAppConfig);
			index++;
			break;
		}

		if (strcmp(argv[index], "-retrievedocss") == 0)
		{
			OmnString siteid(argv[index+1]);
			OmnString ssid(argv[index+2]);
			OmnString objid(argv[index+3]);
			AosBrowserSimu::getSelf()->start(gAosAppConfig);
			AosBrowserSimu::getSelf()->retrieveDocss(siteid, ssid, objid);
			break;
		}
		
		if (strcmp(argv[index], "-test") == 0)
		{
			AosBrowserSimu::getSelf()->start(gAosAppConfig);
			AosBrowserSimu::getSelf()->test();
			break;
		}


		if (strcmp(argv[index], "-addone") == 0)
		{
			u64 docid = atoi(argv[index+1]);
			OmnString siteid(argv[index+2]);
			OmnString dirname(argv[index+3]);
			OmnString idxfname(argv[index+4]);
			OmnString docfname(argv[index+5]);

			AosBrowserSimu::getSelf()->start(gAosAppConfig);
			AosBrowserSimu::getSelf()->addOne(docid, siteid, dirname, idxfname, docfname);
			break;
		}

		index++;
	}

	AosBrowserThrdPtr threads[1000];
	for (int i=0; i<num_thrds; i++)
	{
		OmnScreen << "~~~thread start~~~~~~~" << i << endl;
		threads[i] = OmnNew AosBrowserThrd(i);
	}

	while (1)
	{
		OmnSleep(1);
	}
	theApp.exitApp();
	return 0;
} 




bool AosConvertXmlDoc(OmnString &xml)
{
	// Change the following:
	// 	"__tags"	to	AOSTAG_TAG
	// 	"__tags1"	to 	AOSTAG_TAGC
	// 	"__otype"	to  AOSTAG_OTYPE
	// 	"__stype"	to  AOSTAG_STYPE
	// 	"__name"	to	AOSTAG_OBJNAME
	// 	"@objid"	to	@ + AOSTAG_OBJID
	// 	"dataid"	to	AOSTAG_OBJID
	// 	"@name"		to	@ + AOSTAG_OBJID
	// 	"@username"	to	@ + AOSTAG_USERNAME
	// 	"@password"	to	@ + AOSTAG_PASSWD
	xml.replace("__tags1", AOSTAG_TAGC, true);
	xml.replace("__tags", AOSTAG_TAG, true);
	xml.replace("__otype", AOSTAG_OTYPE, true);
	xml.replace("__stype", AOSTAG_SUBTYPE, true);
	xml.replace("__name", AOSTAG_OBJNAME, true);
	xml.replace("__tnail", AOSTAG_TNAIL, true);
	xml.replace("__desc", AOSTAG_DESCRIPTION, true);
	xml.replace("winobj/", "selfobj/", true);

	OmnString ss = "@";
	ss << AOSTAG_OBJID;
	xml.replace("@objid", ss, true);
	xml.replace("@name", ss, true);
	xml.replace("dataid", "zky_objid", true);

	ss = "@";
	ss << AOSTAG_USERNAME;
	xml.replace("@username", ss, true);

	ss = "@";
	ss << AOSTAG_PASSWD;
	xml.replace("@password", ss, true);

	ss = "@";
	ss << AOSTAG_TNAIL;
	xml.replace("@thumbnail", ss, true);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(xml, "");
	if (!root) return 0;
	AosXmlTagPtr tag = root->getFirstChild();
	aos_assert_r(tag, false);

	OmnString objid = tag->getAttrStr("objid");
	OmnString sss;
	tag->replaceAttrValue("cmp_fnames", true, "name|$|name|$|1", "zky_objid|$|zky_objid|$|1");
	tag->removeAttr("objid", 20, false);
	tag->removeAttr("gic_sep", 20, false);
	tag->setAttr(AOSTAG_OBJID, objid, false);
	tag->removeAttr("zky_vers", 20, false);
	tag->removeAttr("objid_fname", 20, false);
	tag->removeAttr("cmp_objid_fname", 20, false);
	tag->removeAttr("bt_text_align_x", 3, false);
	tag->removeAttr("bt_text_align_y", 3, false);
	tag->removeAttr("zky_cntrd", 3, false);
	tag->removeAttr("zky_cntrm", 3, false);
	tag->removeAttr("zky_cntup", 3, false);
	tag->removeAttr("zky_cntdn", 3, false);
	tag->removeAttr("zky_cntcm", 3, false);
	tag->removeAttr("zky_cntcl", 3, false);
	tag->removeAttr("zky_cntmd", 3, false);

	// If it does not have a container, create a container by extracting
	// the first word (separated by the underscore) of its objid.
	if (tag->getAttrStr(AOSTAG_LOCALE) == "")
	{
		tag->setAttr(AOSTAG_LOCALE, "china");
	}

	// Name conversions
	tag->replaceAttrName("bt_query", "bt_valbd", false);
	tag->replaceAttrName("bt_query_value", "bt_value", false);
	tag->replaceAttrName("username", "zky_uname", true);
	tag->replaceAttrName("password", "zky_passwd", true);
	tag->replaceAttrName("acnt_status", "zky_usrst", true);
	tag->replaceAttrValueSubstr("", true, "@name", "@zky_objid");
	
	OmnString docstr((char *)tag->getData(), tag->getDataLength());

	OmnString req = "<request>";
	req << "<item name=\"operation\">modifyObj</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << sgSiteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<xmlobj>" << docstr << "</xmlobj>"
		<< "</request>";
				
	// OmnScreen << "Converting: " << objid << endl;
	OmnString remoteAddr = gAosAppConfig->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	int remotePort = gAosAppConfig->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
	AosSqlClient conn(remoteAddr, remotePort);
	OmnString errmsg;
	OmnString resp;
	// OmnScreen << "To create: " << req << endl;
	aos_assert_r(conn.procRequest(sgSiteid, "", "", req, resp, errmsg), false);
				// OmnScreen << "Response: " << resp << endl;
	aos_assert_r(resp != "", false);

	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	OmnString docid = child->xpathQuery("Contents/docid", exist, "");
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);

	return true;
}




bool
AosDataReader_sendReq(const OmnString &contents)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">modifyObj</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << sgSiteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<xmlobj>" << contents << "</xmlobj>"
		<< "</request>";
				
	OmnString remoteAddr = gAosAppConfig->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	int remotePort = gAosAppConfig->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
	AosSqlClient conn(remoteAddr, remotePort);
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(conn.procRequest(sgSiteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	OmnString docid = child->xpathQuery("Contents/docid", exist, "");
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);

	return true;
}


static bool setConfig(const char *fname)
{
	if (!fname)
	{
		OmnAlarm << "Missing file name!" << enderr;
		exit(-1);
	}

	OmnFile f(fname, OmnFile::eReadOnly);
	if (!f.isGood())
	{
		OmnAlarm << "Failed to open the config file: " << fname << enderr;
		exit(-1);
	}

	OmnString contents;
	if (!f.readToString(contents))
	{
		OmnAlarm << "Failed to read config: " << fname << enderr;
		exit(-1);
	}

	AosXmlParser parser;
	gAosAppConfig = parser.parse(contents, "");
	if (!gAosAppConfig)
	{
		OmnAlarm << "Configuration file incorrect: " << fname << enderr;
		exit(-1);
	}

	gAosAppConfig->setAttr(AOSTAG_ALLOW_CRTSPUSR, "false");
	//AosXmlDoc::staticInit(gAosAppConfig);
	return true;
}


