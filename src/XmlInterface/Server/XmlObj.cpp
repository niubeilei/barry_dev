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
#include "XmlInterface/Server/XmlObj.h"

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


// XML Escapes:
// &lt;		"<"
// &gt;		">"
// &amp;	"&"
// &quot;	double quote
// &apos;	single quote (apostrophe)

AosXmlObj::AosXmlObj()
{
}


AosXmlObj::~AosXmlObj()
{
}


bool AosXmlObj::getXmlObj(
		TiXmlElement *cmd,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// This function retrieves the objects from the specified
	// table. If the object contains the specified substring,
	// the xml object is returned.
	//
	// The request should be in the form:
	// 	<cmd	tname="xxx"
	// 			query="xxx"
	// 			idfname="xxx"	optional, default based on tname
	// 			xmlfname="xxx"	optional, default to 'xml'
	// 			pattern="xxx"
	// 			startidx="xxx"
	// 			psize="xxx"/>
	//
	// There is an issue about the total. We use the db command
	// to get the total records, but that is not the total of 
	// the records to be returned by this command. For the time
	// being, we will actually calculate the
	// real total. 
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	errcode = eAosXmlInt_General;

	OmnString tname = cmd->Attribute("tname");
	OmnString query = cmd->Attribute("query");
	OmnString idfname = cmd->Attribute("idfname");
	OmnString xmlfname = cmd->Attribute("xmlfname");
	OmnString pattern = cmd->Attribute("pattern");
	OmnString startidxStr = cmd->Attribute("start_idx");
	OmnString psizeStr = cmd->Attribute("psize");

	if (tname == "")
	{
		errmsg = "Missing tablename!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (idfname == "")
	{
		if (tname == "objtable" || tname == "tag_table") idfname = "dataid";
		else idfname = "name";
	}
	if (xmlfname == "") xmlfname = "xml";

	if (pattern == "")
	{
		errmsg = "Missing Pattern!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Need to replace the single quotes and double quotes in 
	// 'pattern'.
cout << __FILE__ << ":" << __LINE__ << ": before pattern: " << pattern << endl;
	pattern.replace("&apos;", "'", true);
	pattern.replace("&quot;", "\"", true);
cout << __FILE__ << ":" << __LINE__ << ": pattern: " << pattern << endl;

	int startIdx = 0;
	if (startidxStr != "") startIdx = atoi(startidxStr.data());
	if (startIdx < 0) startIdx = 0;

	int psize = 10;
	if (psizeStr != "") psize = atoi(psizeStr.data());
	if (psize < 0) psize = 10;

	// If 'start_idx' == 0, we need to count how many records the query
	// will have.
	OmnRslt rslt;
	OmnString origStmt = "select ";
	origStmt << idfname << ", " << xmlfname << " from " << tname;
	if (query != "") origStmt << " where " << query;

	int totalFound = 0;
	OmnString conts;
	int crtIdx = startIdx;
	int queryIdx = 0;
	bool needTotal = (startIdx == 0);
	int lastIdx = startIdx;
	int totalCollected = 0;
	while (totalCollected <= psize || needTotal)
	{
		OmnString stmt = origStmt;
		stmt << " limit " << queryIdx << ", " << psize;
		queryIdx += psize;

		OmnDbTablePtr table;
		rslt = store->query(stmt, table);
		aos_assert_r(rslt, false);
		if (table->entries() == 0) break;

		table->reset();
		OmnDbRecordPtr record;

		AosXmlTagPtr xml;
		while (table->hasMore())
		{
			crtIdx++;
			record = table->next();
			OmnString value = record->getStr(1, "", rslt);
			if (strstr(value.data(), pattern.data()))
			{
				totalFound++;
				if (totalFound >= startIdx)
				{
					totalCollected++;
					if (totalCollected <= psize)
					{
						lastIdx++;
						conts << "<record name=\"" << 
							record->getStr(0, "", rslt) << "\"/>";
					}
					if (!needTotal) break;
				}
			}
		}
	}

	contents = "<Contents tname=\"";
	contents << tname << "\" start_idx=\"" << lastIdx << "\"";
	if (needTotal)
	{
		contents << " total=\"" << totalFound << "\"";
	}
	contents << " num=\"" << totalCollected << "\">" << conts << "</Contents>";
	errcode = eAosXmlInt_Ok;
	return true;
}
