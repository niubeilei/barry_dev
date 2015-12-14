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
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "MySQL/DataStoreMySQL.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SEClient/SqlClient.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"


#include <stdlib.h>
#include <dirent.h>

static OmnString sgRemoteAddr="127.0.0.1";
static int sgRemotePort = 5555;
static OmnString sgSiteid;

extern bool AosConvertAllTables();
extern bool AosConvertDb(const OmnString &tablename);
extern bool AosConvertStdTable(const OmnString &tname);
extern bool AosConvertObjtable(const OmnString &tname);
extern bool AosConvertFramerTable();
extern AosXmlTagPtr AosRemoveAttrs(OmnString &xml);

int 
main(int argc, char **argv)
{
	aos_global_data_init();


	int index = 1;
	OmnString passwd;
	int port = -1;
	OmnString username;
	OmnString appname;
	OmnString opr;
	OmnString tablename;
	while (index < argc)
	{
		if (strcmp(argv[index], "-passwd") == 0 && index < argc-1)
		{
			passwd = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-siteid") == 0 && index < argc-1)
		{
			sgSiteid = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-dbport") == 0 && index < argc-1)
		{
			port = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-convdb") == 0 && index < argc-1)
		{
			opr = "convdb";
			tablename = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-rtport") == 0 && index < argc-1)
		{
			int pp = atoi(argv[index+1]);
			index += 2;

			if (pp > 0) sgRemotePort = pp;
			continue;
		}

		if (strcmp(argv[index], "-rtaddr") == 0 && index < argc-1)
		{
			sgRemoteAddr = argv[index+1];
			index += 2;
			continue;
		}

		index++;
	}

	if (passwd == "")
	{
		cout << "********** Missing pasword!" << endl;
		return -1;
	}

	if (port == -1)
	{
		cout << "********** Missing port!" << endl;
		return -1;
	}

	OmnApp theApp(argc, argv);
	try
	{
		OmnDataStoreMySQL::startMySQL("root", passwd, 
		 		"mydb", port, "torturer");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	if (opr == "convdb")
	{
		AosConvertDb(tablename);
		exit(0);
	}
	else
	{
		cout << "********** Unrecognized operation: " << opr << endl;
		exit(0);
	}

	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 


bool AosConvertDb(const OmnString &tablename)
{
	// It converts the table 'tablename'. If 'tablename' is "all", 
	// it converts all the known tables.
	if (tablename == "all") return AosConvertAllTables();

	OmnNotImplementedYet;
	return false;
}


bool AosConvertAllTables()
{
	// The following tables are ignored:
	// act
	// atb
	// book
	// bugs
	// chenblg
	// containers
	// cvpd
	// dbtrans
	// edtwk
	// filemgrdata
	// gics
	// globals
	// group_obj_dm
	// hotel
	// howto
	// id_generators
	// ignoredWords
	// jgu
	// mysports
	// object_dictionary
	// pmdef
	// pmvpd
	// pwteditor
	// rule
	// rule_list
	// se_docheader
	// seqno_tablename
	// sequence
	// systemids
	// table_changes
	// testdocid_fm
	// testdocindex
	// testrawdocindex
	// testwordid
	// torturer_attrs
	// torturer_awords
	// torturer_ctnr
	// torturer_docs
	// torturer_dwords
	// torturer_seng_words
	// torturer_tags
	// u64ids
	// user
	// wordhashinfo
	// wintpl
	// window
	// webform
	// vertab
	// webbook
	// image_info
	// image_names
	//
	// The following tables are questionable:
	// imageEditing
	// img
	// framer_ct
	// tag_table
	//
	// The following tables are processed
	// dialog
	// giccreators
	// gicdesc
	// lmeditor
	// object
	// p796
	// school
	// ustc
	// vpd
	// webtemp
	//
	// Following are non-standard tables:
	// objtable
	// images
	// framer
	//

	AosConvertStdTable("art");
	AosConvertStdTable("cvpd");
	AosConvertStdTable("dadesign");
	AosConvertStdTable("dayuan");
	AosConvertStdTable("dialog");
	AosConvertStdTable("diz");
	AosConvertStdTable("giccreators");
	AosConvertStdTable("gicdesc");
	AosConvertStdTable("gics");
	AosConvertStdTable("hotel");
	AosConvertStdTable("hua");
	AosConvertStdTable("hysta");
	AosConvertStdTable("imageEditing");
	AosConvertStdTable("img");
	AosConvertStdTable("imgvpd");
	AosConvertStdTable("ivpd");
	AosConvertStdTable("jane");
	AosConvertStdTable("jealousy");
	AosConvertStdTable("lexy");
	AosConvertStdTable("list");
	AosConvertStdTable("lmeditor");
	AosConvertStdTable("object");
	AosConvertStdTable("p796");
	AosConvertStdTable("school");
	AosConvertStdTable("selector");
	AosConvertStdTable("shoes");
	AosConvertStdTable("sucai");
	AosConvertStdTable("szascent");
	AosConvertStdTable("szrmsc");
	AosConvertStdTable("ustc");
	AosConvertStdTable("vpd");
	AosConvertStdTable("webtemp");
	AosConvertStdTable("yu");
	AosConvertStdTable("zykie");

	AosConvertObjtable("objtable");
	AosConvertObjtable("images");

	AosConvertFramerTable();
	return true;
}


bool AosConvertStdTable(
		const OmnString &tname)
{
	// The table is 'tname' and has the following structure:
	// 		name
	// 		keywords
	// 		description
	// 		xml
	// Only name and xml are converted.
//	OmnScreen << "Converting Table: " << tname << endl;
	int startidx = 0;
	int psize = 100;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnDbRecordPtr record;
	AosSqlClient conn(sgRemoteAddr, sgRemotePort);
	while (1)
	{
		OmnString stmt = "select name, xml from ";
		stmt << tname << " limit " << startidx << ", " << psize;
		startidx += psize;
		OmnDbTablePtr table;
		OmnRslt rslt;
		aos_assert_r(store->query(stmt, table), false);
		aos_assert_r(table, false);

		table->reset();
		if (!table->hasMore()) break;

		while (table->hasMore())
		{
			record = table->next();
			OmnString objid = record->getStr(0, "", rslt);
			OmnString xml = record->getStr(1, "", rslt);
			aos_assert_r(objid != "", false);
			if (xml != "")
			{
				AosXmlParser parser;
				AosXmlTagPtr root = AosRemoveAttrs(xml);
				if (!root) continue;
				AosXmlTagPtr child = root->getFirstChild();
				aos_assert_r(child, false);
				child->setAttr(AOSTAG_OBJID, objid);

				// Add the tablename as the container name
				OmnString ctnrs = child->getAttrStr(AOSTAG_CONTAINERS);
				if (ctnrs != "") ctnrs << ",";
				ctnrs << tname;
				child->setAttr(AOSTAG_CONTAINERS, ctnrs);

				OmnString docstr((char *)child->getData(), child->getDataLength());

				OmnString req = "<request>";
				req << "<item name=\"operation\">serverCmd</item>"
					<< "<item name=\"" << AOSTAG_SITEID << "\">" << sgSiteid << "</item>"
					<< "<command><cmd opr='createcobj'/></command>"
					<< "<objdef>" << docstr << "</objdef></request>";
				
//				OmnScreen << "Converting: " << objid << endl;
				OmnString errmsg;
				OmnString resp;
//				OmnScreen << "To create: " << req << endl;
				aos_assert_r(conn.procRequest(sgSiteid, "", "", req, resp, errmsg), false);
				// OmnScreen << "Response: " << resp << endl;
				aos_assert_r(resp != "", false);

				AosXmlTagPtr resproot = parser.parse(resp, "");
				child = resproot->getFirstChild();
				aos_assert_r(child, false);
				bool exist;
				OmnString docid = child->xpathQuery("Contents/docid", exist, "");
				aos_assert_r(docid != "", false);
				aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
			}
		}
	}

	return true;
}


bool AosConvertObjtable(const OmnString &tname)
{
	// +------------+-------------+------+-----+----------+-------+
	// | Field      | Type        | Null | Key | Default  | Extra |
	// +------------+-------------+------+-----+----------+-------+
	// | dataid     | varchar(32) | NO   | PRI |          |       |
	// | name       | varchar(64) | YES  |     | NULL     |       |
	// | tnail      | varchar(64) | YES  |     | NULL     |       |
	// | vvpdname   | varchar(64) | YES  |     | vpd_home |       |
	// | container  | varchar(32) | YES  |     | NULL     |       |
	// | tags       | text        | YES  |     | NULL     |       |
	// | xml        | text        | YES  |     | NULL     |       |
	// | evpdname   | varchar(64) | YES  |     | NULL     |       |
	// | type       | varchar(64) | YES  |     | NULL     |       |
	// | subtype    | varchar(64) | YES  |     | NULL     |       |
	// | creator    | varchar(64) | YES  |     | NULL     |       |
	// | createtime | datetime    | YES  |     | NULL     |       |
	// | updatetime | datetime    | YES  |     | NULL     |       |
	// +------------+-------------+------+-----+----------+-------+
	// 1. 'dataid' is converted to docname
	// 2. 'type' should be AOSTAG_DOCTYPE
	// 3. 'subtype' should be AOSTAG_SUBTYPE
	//
//	OmnScreen << "Converting table: " << tname << endl;
	int startidx = 0;
	int psize = 100;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnDbRecordPtr record;
	AosSqlClient conn(sgRemoteAddr, sgRemotePort);
	while (1)
	{
		OmnString stmt = "select dataid, name, container, "
			"tags, type, subtype, xml from ";
		stmt << tname << " limit " << startidx << ", " << psize;
		startidx += psize;
		OmnDbTablePtr table;
		OmnRslt rslt;
		aos_assert_r(store->query(stmt, table), false);
		aos_assert_r(table, false);

		table->reset();
		if (!table->hasMore()) break;

		while (table->hasMore())
		{
			record = table->next();
			OmnString objid 	= record->getStr(0, "", rslt);
			OmnString name 		= record->getStr(1, "", rslt);
			OmnString container = record->getStr(2, "", rslt);
			OmnString tags 		= record->getStr(3, "", rslt);
			OmnString otype		= record->getStr(4, "", rslt);
			OmnString subtype 	= record->getStr(5, "", rslt);
			OmnString xml 		= record->getStr(6, "", rslt);

			aos_assert_r(objid != "", false);
			if (xml != "")
			{
				AosXmlParser parser;
				AosXmlTagPtr root = AosRemoveAttrs(xml);
				if (!root) continue;
				AosXmlTagPtr child = root->getFirstChild();
				aos_assert_r(child, false);
				child->setAttr(AOSTAG_OBJID, objid);

				// Standardizing the attributes
				child->setAttr(AOSTAG_OBJID, objid);
				child->setAttr(AOSTAG_DOCNAME1, name);
				child->setAttr(AOSTAG_CONTAINERS, container);
				child->setAttr(AOSTAG_TAG, tags);
				child->setAttr(AOSTAG_DOCTYPE, otype);
				child->setAttr(AOSTAG_SUBTYPE, subtype);

				OmnString docstr((char *)child->getData(), child->getDataLength());

				OmnString req = "<request>";
				req << "<item name=\"operation\">serverCmd</item>"
					<< "<item name=\"" << AOSTAG_SITEID << "\">" << sgSiteid << "</item>"
					<< "<command><cmd opr='createcobj'/></command>"
					<< "<objdef>" << docstr << "</objdef></request>";
				
				// OmnScreen << "Converting: " << objid << endl;
				OmnString errmsg;
				OmnString resp;
				// OmnScreen << "To create: " << req << endl;
				aos_assert_r(conn.procRequest(sgSiteid, "", "", req, resp, errmsg), false);
				// OmnScreen << "Response: " << resp << endl;
				aos_assert_r(resp != "", false);

				AosXmlTagPtr resproot = parser.parse(resp, "");
				child = resproot->getFirstChild();
				aos_assert_r(child, false);
				bool exist;
				OmnString docid = child->xpathQuery("Contents/docid", exist, "");
				aos_assert_r(docid != "", false);
				aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
			}
		}
	}

	return true;
}


bool AosConvertFramerTable()
{
	// The table is 'tname' and has the following structure:
	// 		name
	// 		keywords
	// 		description
	// 		xml
	// 		tnail
	// Name and xml are converted. 'tnail' will be set to 'xml'.
//	OmnScreen << "Converting Table: framer" << endl;
	int startidx = 0;
	int psize = 100;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnDbRecordPtr record;
	AosSqlClient conn(sgRemoteAddr, sgRemotePort);
	while (1)
	{
		OmnString stmt = "select name, thumbnail, xml from framer ";
		stmt << " limit " << startidx << ", " << psize;
		startidx += psize;
		OmnDbTablePtr table;
		OmnRslt rslt;
		aos_assert_r(store->query(stmt, table), false);
		aos_assert_r(table, false);

		table->reset();
		if (!table->hasMore()) break;

		while (table->hasMore())
		{
			record = table->next();
			OmnString objid = record->getStr(0, "", rslt);
			OmnString tnail = record->getStr(1, "", rslt);
			OmnString xml   = record->getStr(2, "", rslt);
			aos_assert_r(objid != "", false);
			if (xml != "")
			{
				AosXmlParser parser;
				AosXmlTagPtr root = AosRemoveAttrs(xml);
				if (!root) continue;
				AosXmlTagPtr child = root->getFirstChild();
				aos_assert_r(child, false);
				child->setAttr(AOSTAG_OBJID, objid);

				child->setAttr(AOSTAG_OBJID, objid);
				child->setAttr(AOSTAG_TNAIL, tnail);
				OmnString docstr((char *)child->getData(), child->getDataLength());

				OmnString req = "<request>";
				req << "<item name=\"operation\">serverCmd</item>"
					<< "<item name=\"" << AOSTAG_SITEID << "\">" << sgSiteid << "</item>"
					<< "<command><cmd opr='createcobj'/></command>"
					<< "<objdef>" << docstr << "</objdef></request>";
				
				// OmnScreen << "Converting: " << objid << endl;
				OmnString errmsg;
				OmnString resp;
				// OmnScreen << "To create: " << req << endl;
				aos_assert_r(conn.procRequest(sgSiteid, "", "", req, resp, errmsg), false);
				// OmnScreen << "Response: " << resp << endl;
				aos_assert_r(resp != "", false);

				AosXmlTagPtr resproot = parser.parse(resp, "");
				child = resproot->getFirstChild();
				aos_assert_r(child, false);
				bool exist;
				OmnString docid = child->xpathQuery("Contents/docid", exist, "");
				aos_assert_r(docid != "", false);
				aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
			}
		}
	}

	return true;
}


AosXmlTagPtr AosRemoveAttrs(OmnString &xml)
{
	// Change the following:
	// 	"__tags"	to	AOSTAG_TAG
	// 	"__tags1"	to 	AOSTAG_TAGC
	// 	"__otype"	to  AOSTAG_DOCTYPE
	// 	"__stype"	to  AOSTAG_STYPE
	// 	"__pctnrs"	to  AOSTAG_CONTAINERS
	// 	"__name"	to	AOSTAG_OBJNAME
	// 	"@objid"	to	@ + AOSTAG_OBJID
	// 	"@dataid"	to	@ + AOSTAG_OBJID
	// 	"@name"		to	@ + AOSTAG_OBJID
	// 	"@username"	to	@ + AOSTAG_USERNAME
	// 	"@password"	to	@ + AOSTAG_PASSWD
	xml.replace("__tags1", AOSTAG_TAGC, true);
	xml.replace("__tags", AOSTAG_TAG, true);
	xml.replace("__otype", AOSTAG_DOCTYPE, true);
	xml.replace("__stype", AOSTAG_SUBTYPE, true);
	xml.replace("__pctnrs", AOSTAG_CONTAINERS, true);
	xml.replace("__name", AOSTAG_OBJNAME, true);
	xml.replace("__tnail", AOSTAG_TNAIL, true);
	xml.replace("__desc", AOSTAG_DESCRIPTION, true);
	xml.replace("winobj/", "selfobj/", true);

	OmnString ss = "@";
	ss << AOSTAG_OBJID;
	xml.replace("@objid", ss, true);
	xml.replace("@dataid", ss, true);
	xml.replace("@name", ss, true);

	ss = "@";
	ss << AOSTAG_USERNAME;
	xml.replace("@username", ss, true);

	ss = "@";
	ss << AOSTAG_PASSWD;
	xml.replace("@password", ss, true);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(xml, "");
	if (!root) return 0;
	AosXmlTagPtr tag = root->getFirstChild();
	aos_assert_r(tag, false);

	OmnString objid = tag->getAttrStr("objid");
	tag->removeAttr("objid", 20);
	tag->removeAttr("gic_sep", 20);
	tag->setAttr(AOSTAG_OBJID, objid);
	tag->removeAttr(AOSTAG_DOCID, 20);
	tag->removeAttr("zky_vers", 20);
	tag->removeAttr("objid_fname", 20);
	tag->removeAttr("cmp_objid_fname", 20);
	tag->removeAttr("bt_text_align_x", 1);
	tag->removeAttr("bt_text_align_y", 1);
	tag->removeAttr("zky_cntrd", 1);
	tag->removeAttr("zky_cntrm", 1);
	tag->removeAttr("zky_cntup", 1);
	tag->removeAttr("zky_cntdn", 1);
	tag->removeAttr("zky_cntcm", 1);
	tag->removeAttr("zky_cntcl", 1);
	tag->removeAttr("zky_cntmd", 1);

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

	return root;
}

