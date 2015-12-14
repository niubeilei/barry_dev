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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtDatabase.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEUtil/Containers.h"

AosJqlStmtDatabase::AosJqlStmtDatabase(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
}


AosJqlStmtDatabase::AosJqlStmtDatabase()
{
	mName = "";
	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtDatabase::~AosJqlStmtDatabase()
{
}

bool
AosJqlStmtDatabase::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mOp == JQLTypes::eCreate) return createDatabase(rdata);
	if (mOp == JQLTypes::eShow) return showDatabases(rdata);
	if (mOp == JQLTypes::eDrop) return dropDatabase(rdata);
	if (mOp == JQLTypes::eUse) return useDatabase(rdata);

	AosSetEntityError(rdata, "JQL_Database_run_err", "JQL Database", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtDatabase::createDatabase(const AosRundataPtr &rdata)
{
	if(mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString msg = "";
	OmnString objid = getObjid(rdata, JQLTypes::eDatabaseDoc, mName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (doc) {
		msg << "Database " << mName << " already exists. "
			    << " Use \"drop\" command to delete firstly.";
		rdata->setJqlMsg(msg);
		return true;
	}
	doc = convertToXml(rdata);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	doc->setAttr(AOSTAG_OBJID, objid);

	if ( !createDoc(rdata, JQLTypes::eDatabaseDoc, mName, doc))
	{
		msg << "Failed to create Database doc.";
		return false;
	}

	msg << "Database '" << mName << "' created successfully."; 
	rdata->setJqlMsg(msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtDatabase::convertToXml(const AosRundataPtr &rdata)
{
	OmnString doc = "";
	doc << "<database zky_database_name=\"" << mName << "\" zky_otype=\"database\" >";
	doc << "</database>";
	AosXmlTagPtr conf = AosXmlParser::parse(doc AosMemoryCheckerArgs); 
	if (!conf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	conf->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	conf->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	conf->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_DATABASE_CONTAINER);
	return conf;
}

bool
AosJqlStmtDatabase::showDatabases(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> Databases;
	if( !getDocs(rdata, JQLTypes::eDatabaseDoc, Databases) )
	{
		msg << "No Database Found. "; 
		rdata->setJqlMsg(msg);
		return true;
	}
	
	int total = Databases.size() ; 
	OmnString content;
	content << "<content total=\"" << total << "\"></content>";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	for(u32 i=0; i<Databases.size(); i++)
	{
		AosXmlTagPtr Database = Databases[i];
		doc->addNode(Database);
	}

	rdata->setResults(content);
	if (mContentFormat == "xml")
	{
		rdata->setResults(doc->toString());
		return true;
	}
	vector<OmnString> fields;
	fields.push_back("zky_database_name");
	map<OmnString, OmnString> alias_name;
	alias_name["zky_database_name"] = "DatabaseName";
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}

bool 
AosJqlStmtDatabase::dropDatabase(const AosRundataPtr &rdata)
{
	OmnString msg;
	if (mName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	OmnString objid = getObjid(rdata, JQLTypes::eDatabaseDoc, mName);
	bool rslt = deleteDocByObjid(rdata, objid);

	if (!rslt) {                                                        
		msg << "Failed to delete the Database " << mName;
		rdata->setJqlMsg(msg);
		return false;                                        
	}                                                        
	msg << "Database " << mName << " dropped successfully.";
	rdata->setJqlMsg(msg);
	return true;
}

bool
AosJqlStmtDatabase::useDatabase(const AosRundataPtr &rdata)
{
	if (mName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDatabaseDoc, mName);
	if (!(getDocByObjid(rdata, objid)))
	{
		msg << "Unknown database '" << mName << "'"; 
		rdata->setJqlMsg(msg);
		return true;
	}

	//need to set the current database to be the new one if changed
	//Current database is set in some session data structure. It could be
	//the session object in Rundata object
	
	setCurDatabase(rdata, mName);
	msg = "Database change to ";
	msg << mName;
	rdata->setJqlMsg(msg);

	return true;
}


AosJqlStatement *
AosJqlStmtDatabase::clone()
{
	return OmnNew AosJqlStmtDatabase(*this);
}


void 
AosJqlStmtDatabase::dump()
{
	OmnScreen << "JQL Check Doc Exist Statement: " << mDoc << endl;
}


bool
AosJqlStmtDatabase::checkDatabase(
		const AosRundataPtr &rdata,
		const OmnString &database_name)
{
	if (database_name == "") 
		return false;
	if ( getDoc(rdata, JQLTypes::eDatabaseDoc, database_name) )
	{
		return true;
	}

	return false;
}


/***************************
 * Getter/setter
 * **************************/

void 
AosJqlStmtDatabase::setName(OmnString name)
{
	mName = name;
}

