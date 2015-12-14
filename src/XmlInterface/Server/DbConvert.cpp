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
// 11/11/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/Server/DbConvert.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Book/BookMgr.h"
#include "Book/Container.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataService/DataService.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Proggie/TaggedData/TaggedData.h"
#include "Proggie/ProggieUtil/TcpRequest.h"
#include "SearchEngine/SeXmlParser.h"
#include "SearchEngine/XmlTag.h"
#include "SearchEngine/DocServer.h"
#include "TinyXml/TinyXml.h"
#include "util_c/strutil.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include <iostream>
#include <fstream>
#include <sstream>


AosConvertDb::AosConvertDb()
{
}


AosConvertDb::~AosConvertDb()
{
}


bool 
AosConvertDb::changeAttrName(
		TiXmlElement *cmd,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// This function changes the name of the attribute
	// identified by 'aname' to the new name 'newname'.
	// The command should be in the form:
	// 	<cmd	tname="xxx"
	// 			query="xxx"		optional
	// 			xmlfname="xxx"
	// 			idfname="xxx"
	// 			aname="xxx"
	//			sep="xxx"		optional
	// 			newname="xxx"/>
	//
	errcode = eAosXmlInt_General;

	OmnString query = cmd->Attribute("query");
	OmnString sep = cmd->Attribute("sep");
	OmnString tname = cmd->Attribute("tname");
	aos_assert_re(tname != "", false, errmsg, "Missing table name!");

	OmnString xmlfname = cmd->Attribute("xmlfname");
	aos_assert_re(xmlfname != "", false, 
			errmsg, "Missing XML Field Name!");

	OmnString idfname = cmd->Attribute("idfname");
	aos_assert_re(idfname != "", false, 
			errmsg, "Missing ID Field Name!");

	OmnString aname = cmd->Attribute("aname");
	aos_assert_re(aname != "", false, 
			errmsg, "Missing Attribute Name!");

	OmnString newname = cmd->Attribute("newname");
	aos_assert_re(newname != "", false, 
			errmsg, "Missing New Attribute Name!");

	if (changeAttrName(tname, query, xmlfname, 
			idfname, aname, newname, sep, errmsg))
	{
		errcode = eAosXmlInt_Ok;
		return true;
	}
	return false;
}


const int sgNumTables = 19;
static OmnString sgTablenames[sgNumTables] = 
{
	"bugs",
	"chenblg",
	"cvpd",
	"dialog",
	"giccreators",
	"gicdesc",
	"gics",
	"howto",
	"imageEditing",
	"img",
	"lmeditor",
	"mysports",
	"object",
	"p796",
	"school",
	"ustc",
	"vpd",
	"webtemp",
	"window"
};

OmnString
AosConvertDb::getIdfname(const OmnString &tname)
{
	if (tname == "objtable" ||
		tname == "images")
	{
		return "dataid";
	}

	for (int i=0; i<sgNumTables; i++)
	{
		if (tname == sgTablenames[i]) return "name";
	}

	OmnAlarm << "Unrecognized table: " << tname << enderr;
	return "";
}


bool
AosConvertDb::changeAllAttrName(
		const OmnString &tname, 
		const OmnString &query,
		const OmnString &xmlfname,
		const OmnString &idfname, 
		const OmnString &aname, 
		const OmnString &newname, 
		const OmnString &sep,
		OmnString &errmsg)
{
	if (tname != "")
		return changeAttrName(tname, query, xmlfname, idfname, 
			aname, newname, sep, errmsg);

	for (int i=0; i<sgNumTables; i++)
	{
		changeAttrName(sgTablenames[i], query, xmlfname, 
			idfname, aname, newname, sep, errmsg);
	}

	changeAttrName("objtable", query, xmlfname, 
			"dataid", aname, newname, sep, errmsg);
	changeAttrName("images", query, xmlfname, 
			"dataid", aname, newname, sep, errmsg);
	return true;
}


bool
AosConvertDb::changeAttrName(
		const OmnString &tname, 
		const OmnString &query,
		const OmnString &xmlfname,
		const OmnString &idfname, 
		const OmnString &aname, 
		const OmnString &newname, 
		const OmnString &sep,
		OmnString &errmsg)
{
	OmnString stmt = "select ";
	if (tname == "tag_table")
	{
		stmt << "xml,tag,dataid from tag_table";
	}
	else
	{
		stmt << xmlfname << ", " << idfname << " from " << tname;
	}
    if (query != "") stmt << " where " << query;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt;
	OmnDbTablePtr table;
	rslt = store->query(stmt, table);
	aos_assert_re(rslt, false, errmsg, "Internal Error!");
	table->reset();
	while (table->hasMore())
	{
		OmnDbRecordPtr record = table->next();
		OmnString value = record->getStr(0, "", rslt);
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(value, "");
		if (xml)
		{
			xml = xml->getFirstChild();
			if (xml)
			{
				int len;
				u8 *vv = xml->getAttr(newname, len);
				if (len > 0)
				{
					// The new attribute exists. Need to copy
					// the values from 'aname' to the 'newname'
					// and delete 'aname'. 
					OmnString newvalue((char *)vv, len);
					vv = xml->getAttr(aname, len);
					if (len > 0)
					{
						newvalue << sep;
						newvalue.append((char *)vv, len);	
					}
					xml->setAttr(newname, newvalue.data(), newvalue.length());
					xml->removeAttr(aname, true);
				}
				else
				{
					bool found = false;
					aos_assert_re(xml->changeAttrName(aname, newname, found), 
						false, errmsg, "Internal Error!");
					if (!found) continue;
				}
				OmnString value = (char *)xml->getData();
				value.escapeSymbal('\'', '\'');
				stmt = "update ";
				if (tname == "tag_table")
				{
					OmnString tag = record->getStr(1, "", rslt);
					OmnString dataid = record->getStr(2, "", rslt);
					stmt << tname << " set xml='" << value << "'"
						<< " where tag='" << tag << "' AND dataid='"
						<< dataid << "'";
				}
				else
				{
					OmnString id = record->getStr(1, "", rslt);
					stmt << tname << " set " << xmlfname 
						<< "='" << value << "'"
						<< " where " << idfname << "='" << id << "'";
				}
				aos_assert_re(store->runSQL(stmt), 
					false, errmsg, "Internal Error!");
			}
		}
	}

	return true;
}


bool
AosConvertDb::dumpXml(
		const OmnString &tname, 
		const OmnString &idname,
		const OmnString &dataid)
{
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt;
	OmnDbTablePtr table;
	OmnString stmt = "select xml from ";
	stmt << tname << " where " << idname << "='" << dataid << "'";
	OmnDbRecordPtr record;
	rslt = store->query(stmt, record);
	OmnString value = record->getStr(0, "", rslt);
	cout << __FILE__ << ":" << __LINE__ << ". XML Length: " 
		<< value.length() << endl;
	u8 *data = (u8 *)value.data();
	for (int i=0; i<value.length(); i++)
	{
		cout << (u32)data[i] << " ";
	}
	return true;
}


bool
AosConvertDb::changeAllAttrValues(
		const OmnString &tname, 
		const OmnString &query,
		const OmnString &aname, 
		const OmnString &avalue, 
		const OmnString &newvalue, 
		OmnString &errmsg)
{
	if (tname != "")
		return changeAttrValue(tname, query, 
			aname, avalue, newvalue, errmsg);

	for (int i=0; i<sgNumTables; i++)
	{
		changeAttrValue(sgTablenames[i], query,  
			aname, avalue, newvalue, errmsg);
	}

	changeAttrValue("objtable", query, 
			aname, avalue, newvalue, errmsg);
	changeAttrValue("images", query, 
			aname, avalue, newvalue, errmsg);
	return true;
}


bool
AosConvertDb::changeAttrValue(
		const OmnString &tname, 
		const OmnString &query,
		const OmnString &aname, 
		const OmnString &avalue, 
		const OmnString &newvalue, 
		OmnString &errmsg)
{
	// This function the value of the attribute whose name
	// is 'aname' and value is 'avalue' to the new value
	// 'newvalue'.
	OmnString stmt = "select ";
	if (tname == "tag_table")
	{
		stmt << "xml,tag,dataid from tag_table";
	}
	else if (tname == "images" || tname == "objtable")
	{
		stmt << "xml, dataid from " << tname;
	}
	else
	{
		stmt << "xml, name from " << tname;
	}

	OmnString pattern = aname;
	pattern << "=\"" << avalue << "\"";
	OmnString newpattern = aname;
	newpattern << "=\"" << newvalue << "\"";

    if (query != "") stmt << " where " << query;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt;
	OmnDbTablePtr table;
	rslt = store->query(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed querying the database; " << stmt << enderr;
		return false;
	}

	table->reset();
	while (table->hasMore())
	{
		OmnDbRecordPtr record = table->next();
		OmnString value = record->getStr(0, "", rslt);
		cout << __FILE__ << ":" << __LINE__ << ": trying: " << record->getStr(1, "", rslt) << endl;
		if (value.replace(pattern, newpattern, true))
		{
			stmt = "update ";
			stmt << tname << " set xml='" << value << "' where ";
			OmnString idstr;
			if (tname == "tag_table")
			{
				stmt << "tag='" << record->getStr(1, "", rslt)
					<< "' AND dataid='" << record->getStr(2, "", rslt)
					<< "'";
			}
			else if (tname == "objtable" || tname == "images")
			{
				stmt << "dataid='" << record->getStr(1, "", rslt)
					<< "'";
			}
			else
			{
				stmt << "name='" << record->getStr(1, "", rslt) << "'";
			}
		cout << __FILE__ << ":" << __LINE__ << ": Found: " << stmt << endl;
			aos_assert_re(store->runSQL(stmt), 
				false, errmsg, "Internal Error!");
		}
	}

	return true;
}


bool
AosConvertDb::removeAttrs(
		const OmnString &tname, 
		const OmnString &query,
		const OmnString &aname, 
		const OmnString &avalue, 
		OmnString &errmsg)
{
	if (tname != "")
		return removeAttr(tname, query, 
			aname, avalue, errmsg);

	for (int i=0; i<sgNumTables; i++)
	{
		removeAttr(sgTablenames[i], query,  
			aname, avalue, errmsg);
	}

	removeAttr("objtable", query, aname, avalue, errmsg);
	removeAttr("images", query, aname, avalue, errmsg);
	return true;
}


bool
AosConvertDb::removeAttr(
		const OmnString &tname, 
		const OmnString &query,
		const OmnString &aname, 
		const OmnString &avalue, 
		OmnString &errmsg)
{
	// This function removes the specified attribute. 
	// If 'avalue' is not empty, only the ones whose 
	// values equal to 'avalue' will be removed.
	OmnString stmt = "select ";
	if (tname == "tag_table")
	{
		stmt << "xml,tag,dataid from tag_table";
	}
	else if (tname == "images" || tname == "objtable")
	{
		stmt << "xml, dataid from " << tname;
	}
	else
	{
		stmt << "xml, name from " << tname;
	}

    if (query != "") stmt << " where " << query;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt;
	OmnDbTablePtr table;
	rslt = store->query(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed querying the database; " << stmt << enderr;
		return false;
	}

	table->reset();
	while (table->hasMore())
	{
		OmnDbRecordPtr record = table->next();
		OmnString value = record->getStr(0, "", rslt);
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(value, "");
		if (xml)
		{
			xml = xml->getFirstChild();
			if (xml)
			{
				int found = 0;
				if (avalue != "")
				{
					found = xml->removeAttr(aname, true);
				}
				else
				{
					int len;
					u8 *vv = xml->getAttr(aname, len);
					if (len > 0 && strncmp((char *)vv, avalue.data(), len) == 0)
					{
						found = xml->removeAttr(aname, true);
					}
				}

				if (found > 0)
				{
					stmt = "update ";
					stmt << tname << " set xml='" << (char *)xml->getData() 
						<< "' where ";
					OmnString idstr;
					if (tname == "tag_table")
					{
						stmt << "tag='" << record->getStr(1, "", rslt)
							<< "' AND dataid='" << record->getStr(2, "", rslt)
							<< "'";
					}
					else if (tname == "objtable" || tname == "images")
					{
						stmt << "dataid='" << record->getStr(1, "", rslt)
							<< "'";
					}
					else
					{
						stmt << "name='" << record->getStr(1, "", rslt) << "'";
					}
					aos_assert_re(store->runSQL(stmt), 
						false, errmsg, "Internal Error!");
				}
			}
		}
	}

	return true;
}


bool
AosConvertDb::removeDupeAttrs(
		const OmnString &tname, 
		const OmnString &query,
		OmnString &errmsg)
{
	if (tname != "")
		return removeDupeAttr(tname, query, errmsg);

	for (int i=0; i<sgNumTables; i++)
	{
		removeDupeAttr(sgTablenames[i], query,  errmsg);
	}

	removeDupeAttr("objtable", query, errmsg);
	removeDupeAttr("images", query, errmsg);
	return true;
}


bool
AosConvertDb::removeDupeAttr(
		const OmnString &tname, 
		const OmnString &query,
		OmnString &errmsg)
{
	// This function removes all the duplicated attributes
	// for the selected table.
	OmnString stmt = "select ";
	if (tname == "tag_table")
	{
		stmt << "xml,tag,dataid from tag_table";
	}
	else if (tname == "images" || tname == "objtable")
	{
		stmt << "xml, dataid from " << tname;
	}
	else
	{
		stmt << "xml, name from " << tname;
	}

    if (query != "") stmt << " where " << query;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt;
	OmnDbTablePtr table;
	rslt = store->query(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed querying the database; " << stmt << enderr;
		return false;
	}

	table->reset();
	while (table->hasMore())
	{
		OmnDbRecordPtr record = table->next();
		OmnString value = record->getStr(0, "", rslt);
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(value, "");
		if (xml)
		{
			xml = xml->getFirstChild();
			if (xml)
			{
				int found = 0;
				found = xml->removeDupeAttr();
				if (found > 0)
				{
					stmt = "update ";
					stmt << tname << " set xml='" << (char *)xml->getData() 
						<< "' where ";
					OmnString idstr;
					if (tname == "tag_table")
					{
						stmt << "tag='" << record->getStr(1, "", rslt)
							<< "' AND dataid='" << record->getStr(2, "", rslt)
							<< "'";
					}
					else if (tname == "objtable" || tname == "images")
					{
						stmt << "dataid='" << record->getStr(1, "", rslt)
							<< "'";
					}
					else
					{
						stmt << "name='" << record->getStr(1, "", rslt) << "'";
					}
					aos_assert_re(store->runSQL(stmt), 
						false, errmsg, "Internal Error!");
				}
			}
		}
	}

	return true;
}


