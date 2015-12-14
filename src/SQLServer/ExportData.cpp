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
//		<doc stu_id="110" stu_name="lilyxu" stu_fname="lily" stu_lname="xu" />
//		<doc stu_id="111" stu_name="lucyxu" stu_fname="lucy" stu_lname="xu" />
//
//		<sdoc ...>
//			<query>
//				XML query	
//			</query>
//			<actions>
//				<action type="AOSACTION_SETFROMDB" colname= "id" zky_xpath="xxx" datatype="int">
//					<docselector .../>
//				</action>
//				<action type="AOSACTION_SETFROMDB" colname= "fname" zky_xpath="xxx" datatype="str">
//					<docselector .../>
//				</action>
//				<action type="AOSACTION_SETFROMDB" colname= "lname" zky_xpath="xxx" datatype="str">
//					<docselector .../>
//				</action>
//			</actions>
//		</sdoc>
// 	
// Modification History:
// 06/15/2011	Created by Tracy Huang
////////////////////////////////////////////////////////////////////////////
#include "SQLServer/ExportData.h"

#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Rundata/Rundata.h"
#include "XmlParser/XmlParser.h"
#include "XmlUtil/SeXmlParser.h"

#include "TransServer/TransServer.h"
#include "SqlUtil/SqlReqid.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SdocTypes.h"


AosExportData::AosExportData(const bool regflag)
:
AosSqlProc(AOSSQLPROC_EXPORTDATA, AosSqlReqid::eExportData, regflag)
{
}


AosExportData::~AosExportData()
{
}


bool 
AosExportData::proc(const AosTransPtr &trans, const AosRundataPtr &rdata)
{
	// This function converts data from an XML to a database record.
	
	AosXmlTagPtr trans_xml = trans->getTransDoc();
	if (!trans_xml)
	{
		rdata->setError() << "Failed retrieving the transaction object";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr sdoc = rdata->getReceivedDoc();
	if (!sdoc)
	{
		rdata->setError() << "Missing smart doc";
		return false;
	}

	AosXmlTagPtr actions = sdoc->getFirstChild("actions");
	if (!actions)
	{
		rdata->setError() << "Missing smart Actions";
		return false;
	}

	//get data from our database; by conditions
	AosXmlTagPtr data = sdoc->getFirstChild("Contents");
	if (!data)
	{
		rdata->setError() << "Missing Data";
		return false;
	}

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	if (!store)
	{
		rdata->setError() << "Failed to retrieve the store!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr retrievedDoc = data->getFirstChild();
	while(retrievedDoc)
	{
		rdata->setSourceDoc(retrievedDoc);
		AosSdocAction::runActions(actions, rdata);

		OmnString sql_query = rdata->getResults();
		if (sql_query == "")
		{
			rdata->setError() << "Failed constructing the SQL query!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		OmnRslt rslt = store->runSQL(sql_query);
		if (!rslt)
		{
			rdata->setError() << "Failed running the SQL query: "
				<< sql_query << ". Error: " << rslt.getErrmsg();
			return false;
		}

		retrievedDoc = data->getNextChild();
	}

	rdata->setOk();
	return true;

}

