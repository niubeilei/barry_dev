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
// Example:
// In this example, we assume there is a db table: "test":
// 		firstname	(string)
// 		lastname	(string)
// 		age 		(int)
// 		sex			(char)
// 		address		(string)
// We want to create an XML for each record:
// 		<doc fname="xxx"
// 			lname="xxx"
// 			age="xxx"
// 			sex="xxx"
// 			zky_otype="doc"
//			zky_pctrs="abc"
//			>the-address
//		</doc>
//
//		<sdoc ...>
//			<query>
//				select * from test
//			</query>
//			<actions>
//				<action type="AOSACTION_SETFROMDB" db_fidx="0" aname="fname" datatype="str">
//					<docselector .../>
//				</action>
//				<action type="AOSACTION_SETFROMDB" db_fidx="1" aname="lname" datatype="str">
//					<docselector .../>
//				</action>
//				<action type="AOSACTION_SETFROMDB" db_fidx="2" aname="age" datatype="int">
//					<docselector .../>
//				</action>
//				<action type="AOSACTION_SETFROMDB" db_fidx="3" aname="sex" datatype="int">
//					<docselector .../>
//				</action>
//				<action type="AOSACTION_ADDATTR" >		// set zky_otype
//					<docselector .../>
//				</action>
//				<action type="AOSACTION_ADDATTR" >		// set zky_pctrs
//					<docselector .../>
//				</action>
//			</actions>
//		</sdoc>
// 	
// Modification History:
// 05/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SQLServer/ImportData.h"

#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Rundata/Rundata.h"
#include "XmlParser/XmlParser.h"
#include "SEUtil/SeXmlParser.h"

#include "TransServer/TransServer.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SqlUtil/SqlReqid.h"

AosImportData::AosImportData(const bool regflag)
:
AosSqlProc(AOSSQLPROC_IMPORTDATA, AosSqlReqid::eImportData, regflag)
{
}


AosImportData::~AosImportData()
{
}


bool 
AosImportData::proc(const AosTransPtr &trans, const AosRundataPtr &rdata)
{
	// This function converts data from a SQL database to 
	// XML through a smart doc:
	// 	<request ...>
	//		<sdoc ...>
	//			<query>
	//			...
	//			</query>
	//			<actions>
	//				<action .../>
	//				<action .../>
	//				...
	//			</actions>
	//		</sdoc>
	//	</request>
	// AosXmlTagPtr sdoc = rdata->getSdoc();
	// AosXmlTagPtr sdoc = rdata->getReceivedDoc();
	AosXmlTagPtr tran_xml = trans->getTransDoc();
	AosXmlTagPtr sdoc = tran_xml->getFirstChild("sdoc");
	if (!sdoc)
	{
		rdata->setError() << "Missing smart doc";
		return false;
	}

	bool exist;
	OmnString query = sdoc->xpathQuery("query/_#text", exist, "");
	if (query == "")
	{
		rdata->setError() << "Missing query";
		return false;
	}

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	if (!store)
	{
		rdata->setError() << "Failed to retrieve the store!";
		return false;
	}

	OmnDbTablePtr table;
	OmnRslt rslt = store->query(query, table);
	if (!rslt)
	{
		rdata->setError() << "Failed running the query: "
			<< query << ". Error: " << rslt.getErrmsg();
		return false;
	}

	if (!table)
	{
		rdata->setError() << "Table is empty!";
		return false;
	}

	table->reset();
	AosXmlTagPtr actions = sdoc->getFirstChild("actions");
	if (!actions)
	{
		rdata->setError() << "Missing actions";
		return false;
	}

	vector<AosXmlTagPtr> docs;
	while (table->hasMore())
	{
		OmnDbRecordPtr record = table->next();
		if (!record)
		{
			rdata->setError() << "Record is null";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		AosXmlTagPtr xml = createXml(record, actions, rdata);

		if (!xml)
		{
			return false;
		}
		docs.push_back(xml);
	}

	// Create all the docs
	vector<AosXmlTagPtr>::iterator start = docs.begin();
	while(start != docs.end())
	{
		createDocOnServer(*start, rdata);
		start++;
	}

	return true;
}


OmnRslt
AosImportData::serializeFromDb()
{
	// It retrieves a record from the database
	OmnNotImplementedYet;
	return "";
}


OmnRslt
AosImportData::serializeFromRecord(const OmnDbRecordPtr &record)
{
	OmnNotImplementedYet;
	return "";
}


OmnString
AosImportData::insertStmt() const
{
	OmnNotImplementedYet;
	return "";
}


OmnString
AosImportData::retrieveStmt() const
{
	OmnNotImplementedYet;
	return "";
}


OmnString
AosImportData::updateStmt() const
{
	OmnNotImplementedYet;
	return "";
}


OmnString
AosImportData::removeStmt() const
{
	OmnNotImplementedYet;
	return "";
}


OmnString
AosImportData::removeAllStmt() const
{
	OmnNotImplementedYet;
	return "";
}


OmnString
AosImportData::existStmt() const
{
	OmnNotImplementedYet;
	return "";
}


AosXmlTagPtr
AosImportData::createXml(
		const OmnDbRecordPtr &rcd, 
		const AosXmlTagPtr &actions, 
		const AosRundataPtr &rdata)
{
	// Now we got a record. Need to create an XML based on the record.
	// Creating XML is handled by actions.
	//	<actions>
	//		<action .../>
	//		<action .../>
	//		...
	//	</actions>

	rdata->setDbRecord(rcd);
	OmnString docstr = "<doc />";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "");
	if (!doc)
	{
		rdata->setError() << "Failed to create doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setCreatedDoc(doc);

	AosSdocAction::runActions(actions, rdata);  
	return rdata->getCreatedDoc();
}


bool
AosImportData::createDocOnServer(
		const AosXmlTagPtr &xml, 
		const AosRundataPtr &rdata)
{

	// SearchEngineAdmin/SengAdmin.h createDoc
	OmnString siteid = rdata->getSiteid();
	AosSengAdmin::getSelf()->createDoc(siteid, xml, rdata->getSsid(), "", "true","false");
	return false;
}

