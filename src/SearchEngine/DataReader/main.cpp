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
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/QueryMgr.h"
#include "SEUtil/DocFileMgr.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlDoc.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/IdGenMgr.h"
#include "SEUtil/LogMgr.h"
#include "SearchEngine/IIL.h"
#include "SEClient/SqlClient.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "SearchEngine/WordMgr.h"
#include "SearchEngine/DocServer.h"
#include "SearchEngine/IILMgr.h"
#include "SearchEngine/DataReader/action_conv.h"
#include "SEServer/Ptrs.h"


#include <stdlib.h>

int gAosLogLevel = 1;
static OmnString sgSiteid = "100";
static OmnString sgRemoteAddr="127.0.0.1";
static int sgRemotePort = 5555;

const OmnString sgConfig =
    "<config "
		"dirname=\"/home/AOS/Data\""
		"idgenfname=\"idgen\""
		"serverReadonly=\"true\""
		"wordid_hashname=\"wordid\""
		"wordid_tablesize=\"1000000\""
		"doc_filename=\"doc\""
		"iil_filename=\"iil\""
		"docidgen_name=\"docid\""
		"iilidgen_name=\"iilid\""
		"ignored_hashname=\"ignoredWords\""
		"wordnorm_hashname=\"wordNorm\""
		"max_iils=\"1000\""
		"max_iilsize=\"50000\""
		"max_filesize=\"100000000\""
		"large_filesize=\"10000\""
		"xmldoc_incsize=\"1000\""
		"iil_incsize=\"300\""
		"xmldoc_max_headerfiles=\"1000\""
		"xmldoc_max_docfiles=\"2000\""
		"iil_max_headerfiles=\"1000 \""
		"iil_max_docfiles=\"2000 \""
		"version_filename=\"version\""
		"local_addr=\"loopback\""
   		"local_port=\"5565-5565\""
    	"service_rcd=\"true\""
	 	"service_name=\"ReqDistrTest\""
	  	"support_actions=\"true\""
	   	"req_delinerator=\"first_word_high\""
	    "max_proc_time=\"10000\""
	    "logfilename=\"zykielog\""
		"max_conns=\"400\">"
		"<versionmgr "
			"filename=\"version\""
			"dirname=\"/home/AOS/Data\""
			"maxfsize=\"100000000\""
			"large_filesize=\"10000\""
			"maxdocfiles=\"100\""
			"incsize=\"300\""
			"maxheaderperfile=\"1000000\""
			"maxheaderfiles=\"100\"/>"
		"<IdGens>"
			"<docid crtid=\"1000\""
				"crtbsize=\"5000000\""
				"nextbsize=\"5000000\"/>"
			"<iilid crtid=\"1000\""
				"crtbsize=\"5000000\""
				"nextbsize=\"5000000\"/>"
			"<imageid crtid=\"1000\""
				"crtbsize=\"5000000\""
				"nextbsize=\"5000000\"/>"
		"</IdGens>"
	"</config>";

#include <dirent.h>

static OmnString sgAosOpenLzHome;
AosXmlTagPtr gAosAppConfig;

OmnString AosGetOpenLzHome()
{
	return sgAosOpenLzHome;
}

extern bool AosDataReader_convertAction();
extern bool AosDataReader_convertData();
extern bool AosConvertXmlDoc(OmnString &xml);
extern bool AosDataReader_sendReq(const OmnString &contents);
extern bool AosDataReader_ReplaceText(
				const OmnString &pattern, 
				const OmnString &value);
extern bool AosProcConvertAction(const AosXmlTagPtr &doc);

int 
main(int argc, char **argv)
{
	int index = 1;
	aos_global_data_init();

	OmnString config = sgConfig;

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(config, "");
	gAosAppConfig = root->getFirstChild();
	

	OmnString pattern, value;
	while (index < argc)
	{
		if (strcmp(argv[index], "-dirname") == 0 && index < argc-1)
		{
			OmnString dirname = argv[index+1];
			index += 2;
			gAosAppConfig->setAttr(AOSCONFIG_DIRNAME, dirname);
			continue;
		}

		index++;
	}

	AosIIL::staticInit(gAosAppConfig);
	AosXmlDoc::staticInit(gAosAppConfig);

	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew AosIdGenMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	AosDataReader_convertAction();
	theApp.exitApp();
	return 0;
} 


bool 
AosDataReader_convertAction()
{
	u64 max_docid = 40000;
	AosXmlDoc header;
	AosXmlTagPtr doc;
	// for (u64 i=1000; i<1001; i++)
	for (u64 i=1; i<max_docid; i++)
	{
		// OmnScreen << "Read doc: " << i << endl;
		doc = header.loadXmlFromFile(i);
		if (doc && header.isDocGood())
		{
			AosProcConvertAction(doc);
		}
	}
	return true;
}


bool 
AosDataReader_convertData()
{
	u64 max_docid = AosDocServerSelf->getCrtMaxDocid();
	AosXmlDoc header;
	AosXmlTagPtr doc;
	// for (u64 i=1000; i<1001; i++)
	for (u64 i=0; i<max_docid; i++)
	{
		OmnScreen << "Read doc: " << i << endl;
		doc = header.loadXmlFromFile(i);
		if (doc)
		{
			OmnString xml((char *)doc->getData());
			AosConvertXmlDoc(xml);
		}
	}
	return true;
}


bool AosProcConvertAction(const AosXmlTagPtr &xmldoc)
{
	AosXmlTagPtr doc = xmldoc;
	aos_assert_r(doc->removeEmptyAttr("__actions", true) >= 0, false);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);

	bool exist;
	AosXmlTagPtr node;
	OmnString action = doc->getAttrStr("__actions", true, exist, node);

	// Try "__actions"
	bool changed = false;
	OmnString sss;
	while (exist)
	{
		changed = true;
		aos_assert_r(node, false);
		if (!node->removeAttr("__actions", false, true))
		{
			OmnAlarm << "Failed to remove action: " 
				<< (char *)node->getData() 
				<< ". Action: " << action << enderr;
			return false;
		}

		aos_assert_r(action != "", false);
		
		// Found an action attribute. Split it.
		AosStrSplit split;
		OmnString parts[20];
		bool finished;

		OmnString rslts = "<actions>";
		int nn = split.splitStr(action.data(), "|$$|", parts, 20, finished);
		for (int i=0; i<nn; i+=2)
		{
			if (i+1 >= nn)
			{
				OmnAlarm << "Action not correct: " << objid
					<< ". Action: " << action << enderr;
				OmnScreen << "The doc: " << doc->getData() << endl;
				return false;
			}
			aos_assert_r(i+1 < nn, false);
			AosConvertAction(objid, rslts, parts[i], parts[i+1]);

		}
		rslts << "</actions>";
	
		OmnScreen << "Convert Action: " << action << endl;
		OmnScreen << "Converted: " << rslts << "\n" << endl;

		if (rslts != "<actions></actions>")
		{
			int startpos = node->getTagEnd()+1;
			aos_assert_r(startpos > 0, false);
			char *data = (char *)doc->getData();
			if (data[startpos-2] == '/')
			{
				// The node does not have child nodes. Need to change it.
				aos_assert_r(node->getNumSubtags() == 0, false);
				data[startpos-2] = '>';
				data[startpos-1] = ' ';
				startpos--;
				rslts << "</" << node->getTagname() << ">";
			}
			else
			{
				// Check whether the node is in the form:
				// <tag ...>...</tag>
				OmnString tagname = node->getTagname();
				if (data[startpos-tagname.length()-2] == '/')
				{
					// It is in the form: <tag ...>...</tag>
					startpos = startpos-tagname.length()-3;
				}
			}
		
			OmnString contents((const char *)doc->getData(), doc->getDataLength());
			aos_assert_r(contents.insert(rslts, startpos), false);

			AosXmlParser parser;
			doc = parser.parse(contents, "");
			if (!doc)
			{
				OmnScreen << "Failed processing the action: " 
					<< objid << ". Action: " << action 
					<< ". Contents: " << contents << endl;
				return false;
			}
		}
		action = doc->getAttrStr("__actions", true, exist, node);
	}

	// Try "container_created_actions"
	action = doc->getAttrStr("container_created_actions", true, exist, node);
	while (exist)
	{
		changed = true;
		aos_assert_r(node, false);
		if (!node->removeAttr("container_created_actions", false, true))
		{
			OmnAlarm << "Failed to remove action: " 
				<< (char *)node->getData() 
				<< ". Action: " << action << enderr;
			return false;
		}

		aos_assert_r(action != "", false);
		
		// Found an action attribute. Split it.
		AosStrSplit split;
		OmnString parts[20];
		bool finished;

		OmnString rslts = "<actions>";
		int nn = split.splitStr(action.data(), "|$$|", parts, 20, finished);
		for (int i=0; i<nn; i++)
		{
			AosConvertAction(objid, rslts, "evt_ctn_created", parts[i]);

		}
		rslts << "</actions>";
	
		OmnScreen << "Convert Action: " << action << endl;
		OmnScreen << "Converted: " << rslts << "\n" << endl;

		if (rslts != "<actions></actions>")
		{
			int startpos = node->getTagEnd()+1;
			aos_assert_r(startpos > 0, false);
			char *data = (char *)doc->getData();
			if (data[startpos-2] == '/')
			{
				// The node does not have child nodes. Need to change it.
				aos_assert_r(node->getNumSubtags() == 0, false);
				data[startpos-2] = '>';
				data[startpos-1] = ' ';
				startpos--;
				rslts << "</" << node->getTagname() << ">";
			}
			else
			{
				// Check whether the node is in the form:
				// <tag ...>...</tag>
				OmnString tagname = node->getTagname();
				if (data[startpos-tagname.length()-2] == '/')
				{
					// It is in the form: <tag ...>...</tag>
					startpos = startpos-tagname.length()-3;
				}
			}
		
			OmnString contents((const char *)doc->getData(), doc->getDataLength());
			aos_assert_r(contents.insert(rslts, startpos), false);

			AosXmlParser parser;
			doc = parser.parse(contents, "");
			if (!doc)
			{
				OmnScreen << "Failed processing the action: " 
					<< objid << ". Action: " << action 
					<< ". Contents: " << contents << endl;
				return false;
			}
		}
		action = doc->getAttrStr("container_created_actions", true, exist, node);
	}

	// Try "ctn_actions"
	action = doc->getAttrStr("ctn_actions", true, exist, node);
	while (exist)
	{
		changed = true;
		aos_assert_r(node, false);
		if (!node->removeAttr("ctn_actions", false, true))
		{
			OmnAlarm << "Failed to remove action: " 
				<< (char *)node->getData() 
				<< ". Action: " << action << enderr;
			return false;
		}

		aos_assert_r(action != "", false);
		
		// Found an action attribute. Split it.
		AosStrSplit split;
		OmnString parts[20];
		bool finished;

		OmnString rslts = "<actions>";
		int nn = split.splitStr(action.data(), "|$$|", parts, 20, finished);
		for (int i=0; i<nn; i+=2)
		{
			if (i+1 >= nn)
			{
				OmnScreen << "Action not correct: " << objid
					<< ". Action: " << action << endl;
				return false;
			}
			aos_assert_r(i+1 < nn, false);
			AosConvertAction(objid, rslts, parts[i], parts[i+1]);
		}
		rslts << "</actions>";
	
		OmnScreen << "Convert Action: " << action << endl;
		OmnScreen << "Converted: " << rslts << "\n" << endl;

		if (rslts != "<actions></actions>")
		{
			int startpos = node->getTagEnd()+1;
			aos_assert_r(startpos > 0, false);
			char *data = (char *)doc->getData();
			if (data[startpos-2] == '/')
			{
				// The node does not have child nodes. Need to change it.
				aos_assert_r(node->getNumSubtags() == 0, false);
				data[startpos-2] = '>';
				data[startpos-1] = ' ';
				startpos--;
				rslts << "</" << node->getTagname() << ">";
			}
			else
			{
				// Check whether the node is in the form:
				// <tag ...>...</tag>
				OmnString tagname = node->getTagname();
				if (data[startpos-tagname.length()-2] == '/')
				{
					// It is in the form: <tag ...>...</tag>
					startpos = startpos-tagname.length()-3;
				}
			}
		
			OmnString contents((const char *)doc->getData(), doc->getDataLength());
			aos_assert_r(contents.insert(rslts, startpos), false);

			AosXmlParser parser;
			doc = parser.parse(contents, "");
			if (!doc)
			{
				OmnScreen << "Failed processing the action: " 
					<< objid << ". Action: " << action 
					<< ". Contents: " << contents << endl;
				return false;
			}
		}
		action = doc->getAttrStr("ctn_actions", true, exist, node);
	}

	// Try "gic_created_actions"
	action = doc->getAttrStr("gic_created_actions", true, exist, node);
	while (exist)
	{
		OmnScreen << "giccreatedactions: " << doc->getAttrStr(AOSTAG_OBJID) << endl;
		changed = true;
		aos_assert_r(node, false);
		if (!node->removeAttr("gic_created_actions", false, true))
		{
			OmnAlarm << "Failed to remove action: " 
				<< (char *)node->getData() 
				<< ". Action: " << action << enderr;
			return false;
		}

		aos_assert_r(action != "", false);
		/*
		
		// Found an action attribute. Split it.
		AosStrSplit split;
		OmnString parts[20];
		bool finished;

		OmnString rslts = "<actions>";
		int nn = split.splitStr(action.data(), "|$$|", parts, 20, finished);
		for (int i=0; i<nn; i++)
		{
			AosConvertAction(objid, rslts, "evt_gic_created", parts[i]);

		}
		rslts << "</actions>";
	
		OmnScreen << "Convert Action: " << action << endl;
		OmnScreen << "Converted: " << rslts << "\n" << endl;

		if (rslts != "<actions></actions>")
		{
			int startpos = node->getTagEnd()+1;
			aos_assert_r(startpos > 0, false);
			char *data = (char *)doc->getData();
			if (data[startpos-2] == '/')
			{
				// The node does not have child nodes. Need to change it.
				aos_assert_r(node->getNumSubtags() == 0, false);
				data[startpos-2] = '>';
				data[startpos-1] = ' ';
				startpos--;
				rslts << "</" << node->getTagname() << ">";
			}
			else
			{
				// Check whether the node is in the form:
				// <tag ...>...</tag>
				OmnString tagname = node->getTagname();
				if (data[startpos-tagname.length()-2] == '/')
				{
					// It is in the form: <tag ...>...</tag>
					startpos = startpos-tagname.length()-3;
				}
			}
		
			OmnString contents((const char *)doc->getData(), doc->getDataLength());
			aos_assert_r(contents.insert(rslts, startpos), false);

			AosXmlParser parser;
			doc = parser.parse(contents, "");
			if (!doc)
			{
				OmnScreen << "Failed processing the action: " 
					<< objid << ". Action: " << action 
					<< ". Contents: " << contents << endl;
				return false;
			}
		}
		*/
		action = doc->getAttrStr("gic_created_actions", true, exist, node);
	}

	if (!changed) return true;

	OmnString contents((const char *)doc->getData(), doc->getDataLength());
	aos_assert_r(AosDataReader_sendReq(contents), false);
	return true;
}


bool AosConvertXmlDoc(OmnString &xml)
{
	// Change the following:
	// 	"__tags"	to	AOSTAG_TAG
	// 	"__tags1"	to 	AOSTAG_TAGC
	// 	"__otype"	to  AOSTAG_OTYPE
	// 	"__stype"	to  AOSTAG_STYPE
	// 	"__pctnrs"	to  AOSTAG_HPCONTAINER
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
	xml.replace("__pctnrs", AOSTAG_HPCONTAINER, true);
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
	tag->replaceAttrValue(AOSTAG_HPCONTAINER, true, "img", "image");
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
	if (tag->getAttrStr(AOSTAG_HPCONTAINER) == "" && objid != "")
	{
		AosStrSplit split;
		OmnString parts[10];
		bool finished;
		split.splitStr(objid.data(), "_", parts, 10, finished);
		tag->setAttr(AOSTAG_HPCONTAINER, parts[0]);
	}

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
	AosSqlClient conn(sgRemoteAddr, sgRemotePort);
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
AosDataReader_ReplaceText(
		const OmnString &pattern, 
		const OmnString &value)
{
	// This function searches the entire database to replace 
	// the pattern 'pattern' with the value 'value'.
	u64 max_docid = AosDocServerSelf->getCrtMaxDocid();
	AosXmlDoc header;
	AosXmlTagPtr doc;
	for (u64 i=0; i<max_docid; i++)
	{
		OmnScreen << "Read doc: " << i << endl;
		doc = header.loadXmlFromFile(i);
		if (doc)
		{
			OmnString xml((char *)doc->getData());
			int nn = xml.replace(pattern, value, true);
			if (nn > 0)
			{
				// Contents being modified
				OmnScreen << "Modify doc: " << i << endl;
				AosDataReader_sendReq(xml);
			}
		}
	}
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
				
	AosSqlClient conn(sgRemoteAddr, sgRemotePort);
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


bool 
AosDataReader_rebuildDatabase(
		const OmnString &idxFname, 
		const OmnString &docFname)
{
	// It erases everything from the data directory except the docfile, 
	// reads all the docs in the docfile, and re-create the doc.
	// 
	// 'idxFname' identifies the doc index file name, which is in the
	// form:
	// 		<idxFname>_nnn
	// where 'nnn' is a sequence number, starting from 0. 
	//
	// Similarly, 'docFname' identifies the doc files:
	// 		<docFname>_nnn
	//
	const u32 maxDocFiles = 20;
	OmnFilePtr docfiles[maxDocFiles];
	u64 max_docid = AosDocServerSelf->getCrtMaxDocid();
	AosXmlDoc header;
	AosXmlTagPtr doc;
	AosSqlClient conn(sgRemoteAddr, sgRemotePort);

	// Open the idx file
	OmnString idxfn = idxFname;
	u32 crtIdxSeqno = 0;
	idxfn << "_" << crtIdxSeqno;
	OmnFilePtr idxfile = OmnNew OmnFile(idxfn, OmnFile::eReadOnly);
	aos_assert_r(idxfile && idxfile->isGood(), false);

	// Open the doc file
	u32 crtDocSeqno = 0;
	OmnString docfn = docFname;
	docfn << "_" << crtDocSeqno;
	OmnFilePtr docfile = OmnNew OmnFile(docfn, OmnFile::eReadOnly);
	aos_assert_r(docfile && docfile->isGood(), false);
	int maxHeaderPerFile = AosXmlDoc::getMaxHeaderPerFile();

	// Create the buff for header
	char bb[AosXmlDoc::eDocHeaderSize];

	for (u64 docid=0; docid<max_docid; docid++)
	{
		OmnScreen << "Read doc: " << docid << endl;
		u32 seqno = docid / maxHeaderPerFile;
		u32 offset = (docid % maxHeaderPerFile) * AosXmlDoc::eDocHeaderSize;

		if (seqno != crtIdxSeqno)
		{
			crtIdxSeqno++;
			idxfn = idxFname;
			crtIdxSeqno = 0;
			idxfn << "_" << crtIdxSeqno;
			idxfile = OmnNew OmnFile(idxfn, OmnFile::eReadOnly);
			aos_assert_r(idxfile && idxfile->isGood(), false);
		}

		int len = idxfile->readBinaryInt(offset, -1);
		if (len <= 0 || (u32)len > AosXmlDoc::eDocHeaderSize)
		{
			OmnAlarm << "Invalid length: " << len 
				<< ":" << docid << enderr;
			continue;
		}

		len = idxfile->readToBuff(offset+4, len, bb);
		aos_assert_r(len >= 0, false);
		AosBuff headerBuff(bb, len, 0);
		
		headerBuff.getU32(AosXmlDoc::eInvVersion);	// version
		headerBuff.getU32(AOS_INVSID);	// siteid
		headerBuff.getU64(AOS_INVDID);	// docid
		u32 docseqno = headerBuff.getU32(eAosInvFseqno);	// doc seqno
		u64 docoffset = headerBuff.getU64(eAosInvFoffset);	// doc offset
		u64 docsize = headerBuff.getU64(eAosInvFoffset);	// docsize
		if (docseqno == eAosInvFseqno) 
		{
			OmnAlarm << "Doc seqno invalid: " << docid << enderr;
			continue;
		}

		if (docoffset == eAosInvFoffset) 
		{
			OmnAlarm << "Doc offset invalid: " << docid << enderr;
			continue;
		}

		if (docsize == eAosInvFoffset)
		{
			OmnAlarm << "Doc size invalid: " << docid << enderr;
			continue;
		}

		// Ready to read the doc itself
		OmnConnBuffPtr docbuff = OmnNew OmnConnBuff(docsize);
		aos_assert_r(docbuff->determineMemory(docsize), 0);
		char *data = docbuff->getBuffer();

		if (!docfiles[docseqno])
		{
			OmnString fn = docFname;
			fn << "_" << docseqno;
			docfiles[docseqno] = OmnNew OmnFile(fn, OmnFile::eReadOnly);
			aos_assert_r(docfiles[docseqno] && docfiles[docseqno]->isGood(), false);
		}

		len = docfiles[docseqno]->readBinaryInt(docoffset, -2);
		if (len == -1)
		{
			OmnAlarm << "Record was deleted: " << docseqno << ":"
				<< docoffset << ":" << docid << enderr;
			continue;
		}

		if (len <= 0)
		{
			OmnAlarm << "Failed to read the doc: " 
				<< docseqno << ":" << docoffset << ":" << docid << enderr;
			continue;
		}

		if ((u64)len > docsize)
		{
			OmnAlarm << "Data too big: " << len << ":" << docsize 
				<< ":" << docid << enderr;
			continue;
		}

		int bytesread = docfiles[docseqno]->readToBuff(
				docoffset + AosDocFileMgr::eDocOffset, len, data);
		if (bytesread == len)
		{
			OmnAlarm << "Data incorrect: " << bytesread << ":" << len
				<< ":" << docseqno << ":" << docoffset << ":" << docid << enderr;
			continue;
		}
		docbuff->setDataLength(docsize);

		// Now the doc has been read into 'data'. Need to send a request
		// to the server to create it.
		OmnString req = "<request>";
		req << "<item name=\"operation\">serverCmd</item>"
			<< "<item name=\"" << AOSTAG_SITEID << "\">" << sgSiteid << "</item>"
			<< "<command><cmd opr='createcobj'/></command>"
			<< "<objdef>" << docbuff->getData() << "</objdef></request>";
				
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
		aos_assert_r(docid != "", false);
		aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	}

	return true;
}

