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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/SelMySQLRecord.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "QueryCond/CondNames.h"
#include "QueryUtil/QrUtil.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "API/AosApi.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "Thread/Mutex.h"
#include "Util/UtUtil.h"
#include "Util/CodeConvertion.h"
#include "MySQL/DataStoreMySQL.h"

AosSelMySQLRecord::AosSelMySQLRecord(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_BY_MYSQL, AosDocSelectorType::eMysql, reg)
{
}


AosSelMySQLRecord::~AosSelMySQLRecord()
{
}


AosXmlTagPtr 
AosSelMySQLRecord::selectDoc(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	// def looks like :
	//  <term zky_type="query||insert||delete"                           
	//       zky_user="xxxx" 
	//       zky_passwd="xxx"
	//       zky_database_ip="xxx"
	//       zky_database_prot="xxxx"
	//       zky_num_of_field="xx"
	//       zky_database_name="xxxx" >
	//      <head>xxxx,xxxx,xxxxx,xxxx,xxx,xxx</head>
	//      <sql><![CDATA[ select * from xxxxx]]></sql>
	//  </term>
	OmnString user= def->getAttrStr("zky_user");
	aos_assert_r(user != "", 0);

	OmnString passwd = def->getAttrStr("zky_passwd");
	aos_assert_r(passwd != "", 0);

	OmnString database = def->getAttrStr("zky_database_name");
	aos_assert_r(database != "", 0);

	OmnString ip = def->getAttrStr("zky_database_ip", "127.0.0.1");
	aos_assert_r( ip!= "", 0);

	u32 port = def->getAttrU32("zky_database_prot", 3306);

	u32 fileds = def->getAttrInt("zky_num_of_field", 0);

	AosXmlTagPtr headnode = def->getFirstChild("head");
	aos_assert_r(headnode, 0);

	OmnString head = headnode->getNodeText();
	AosStrSplit split(head, ",");
	vector<OmnString> heads = split.entriesV();
	AosXmlTagPtr sqlnode = def->getFirstChild("sql");
	aos_assert_r(sqlnode, 0);

	OmnString SQL = sqlnode->getNodeText();
	aos_assert_r(SQL != "", 0);

	OmnDataStoreMySQLPtr mySql = OmnNew OmnDataStoreMySQL(user, passwd, ip, database, port, "");
	if (!mySql)
	{
		rdata->setError() << "Failed to retrieve the store!";
		return 0;
	}

	OmnRslt rslt = mySql->openDb();
	if (!rslt)
	{
		rdata->setError()<< "Failed to retrieve the store!";
		return 0;
	}

	OmnDbTablePtr raw;
	if ( !(mySql->query(SQL, raw)))
	{
		rdata->setError()<< "Failed To Run SQL: " << SQL << enderr;
		return 0;
	}
	OmnDbRecordPtr record;

	while(raw->hasMore())
	{
		record = raw->next();
	}

	rslt = mySql->closeDb();
	if (!rslt)
	{
		rdata->setError()<< "Failed to retrieve the store!";
		return 0;
	}

	AosXmlTagPtr doc = AosXmlParser::parse("<record></record>" AosMemoryCheckerArgs);
	for(u32 j = 0; j < fileds; j++)
	{
		OmnString value = record->getStr(j, "", rslt);
		if (j < heads.size())
		{
			doc->setAttr(heads[j],value);
		}
		else
		{
			doc->setAttr("zky_null", value);
		}
	}
	return doc; 
}


OmnString
AosSelMySQLRecord::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	// 	<selector AOSTAG_ZKY_TYPE=AOSDOCSELTYPE_BY_OBJID
	// 		AOSTAG_DOC_OBJID="xxx">
	// 		<AOSTAG_VALUEDEF .../>
	// 	</selector>
	return "";
}

