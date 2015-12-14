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
#include "JQLStatement/JqlStmtDatascanner.h"
#include "JQLExpr/Expr.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"

#include "AosConf/DataScanner.h"
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;
using namespace AosConf;

AosJqlStmtDatascanner::AosJqlStmtDatascanner()
{
	mName = "";
	mConnectorName = "";
	mDirList = NULL;
	mNeedToOrder = false;
	mOp = JQLTypes::eOpInvalid;
	mErrmsg = "";
}


AosJqlStmtDatascanner::AosJqlStmtDatascanner(const OmnString &errmsg)
{
	mName = "";
	mConnectorName = "";
	mDirList = NULL;
	mNeedToOrder = false;
	mOp = JQLTypes::eOpInvalid;
	mErrmsg = errmsg;
}


AosJqlStmtDatascanner::~AosJqlStmtDatascanner()
{
	if (mDirList)
	{
		for (u32 i = 0; i < mDirList->size(); i++)
		{
			if ((*mDirList)[i])
				delete (*mDirList)[i];
		}
	}
	delete mDirList;
	mDirList = 0;
}

bool
AosJqlStmtDatascanner::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createDatascanner(rdata);
	if (mOp == JQLTypes::eShow) return showDatascanners(rdata);
	if (mOp == JQLTypes::eDrop) return dropDatascanner(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDatascanner(rdata);

	AosSetEntityError(rdata, "JQL_datascanner_run_err", "JQL Datascanner", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}

bool
AosJqlStmtDatascanner::createDatascanner(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDataScannerDoc, mName);
	boost::shared_ptr<DataScanner> dscan = boost::make_shared<DataScanner>();

	// for new config
	if (mConfParms)
	{
		return getConf(rdata, dscan, mConfParms);
	}

	// 1. create connector
	if ( !createConn(rdata) )
	{
		rdata->setJqlMsg("ERROR: create scanner failed (when create connect)");
		return false;
	}
	// 2. create scanner
	OmnString jimo_objid = "datascanner_parallel_jimodoc_v0";
	if (mNeedToOrder) jimo_objid = "datascanner_cube_jimodoc_v0";

	dscan->setAttribute("jimo_objid", jimo_objid);
	dscan->setAttribute("zky_name", mName);
	//dscan->setAttribute("zky_otype", "datascanner");
	AosXmlTagPtr connectorDoc = getDoc(rdata, JQLTypes::eDataConnectorDoc, mConnectorName);
	if (!connectorDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	boost::shared_ptr<DataConnector> connectorObj = boost::make_shared<DataConnector>(connectorDoc);
	dscan->setConnector(connectorObj);

	// 3. save datascanner doc
	AosXmlTagPtr doc = AosXmlParser::parse(dscan->getConfig() AosMemoryCheckerArgs);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if ( !createDoc(rdata, JQLTypes::eDataScannerDoc, mName, doc) )
	{
		msg << "Failed to create Task doc.";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg << "Data Scanner " << mName << " created successfully.";
	rdata->setJqlMsg(msg);
	return true;
}

bool
AosJqlStmtDatascanner::createConn(const AosRundataPtr &rdata)
{
	// If connector is exist, do nothing.
	// otherwise, generator connector configure
	if (mConnectorType == "")
	{
		AosXmlTagPtr connectorDoc = getDoc(rdata, JQLTypes::eDataConnectorDoc, mConnectorName);
		if (connectorDoc) return true;

		return false;
	}

	AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
	data_connector->setName(mConnectorName);
	if (mDirList)
	{
		data_connector->setType("DIRECTORY");
		data_connector->setDirList(mDirList);
	}
	else
	{
		data_connector->setType(mConnectorType);
		data_connector->setSvrId(mSvrId.toInt());
		data_connector->setFileName(mFileName);
	}
	data_connector->setCoding(mEncoding);
	data_connector->setSplitSize(mSplitSize);
	data_connector->setOp(JQLTypes::eCreate);
	return data_connector->run(rdata, 0);
}

bool
AosJqlStmtDatascanner::showDatascanners(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> scanners;
	if( !getDocs(rdata, JQLTypes::eDataScannerDoc, scanners) )
	{
		msg << "No Data Scanner Found. ";
		rdata->setJqlMsg(msg);
		return true;
	}

	int total = scanners.size() ; 
	OmnString content;
	content << "<content total=\"" << total << "\">";
	for(u32 i=0; i<scanners.size(); i++)
	{
		OmnString vv = scanners[i]->getAttrStr("zky_name", "");
		if (vv != "") {
			content << "<record scannername=\"" << vv << "\"/>";
		}
	}

	content << "</content>";
	rdata->setResults(content);
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	vector<OmnString> fields;
	fields.push_back("scannername");
	map<OmnString, OmnString> alias_name;
	alias_name["scannername"] = "DataScannerName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}

bool 
AosJqlStmtDatascanner::dropDatascanner(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDataScannerDoc, mName);                  
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                        
		msg << "Failed to delete data scanner doc.";  
		rdata->setJqlMsg(msg);
		return false;                                        
	}                                                        

	msg << "Data Scanner " << mName << " dropped successfully."; 
	rdata->setJqlMsg(msg);
	return true;
}


bool
AosJqlStmtDatascanner::describeDatascanner(const AosRundataPtr &rdata)
{
	if (mName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eDataScannerDoc, mName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (!doc)
	{
		msg << "Failed to display data scanner " << mName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;

	fields.push_back("zky_name");
	fields.push_back("zky_dataconnector_name");
	alias_name["zky_name"] = "Name";
	alias_name["zky_dataconnector_name"] = "ConnectorName";

	OmnString content = "<content>";
	content << doc->toString() << "</content>";
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, def);
	setPrintData(rdata, msg);
	return true;
}


AosJqlStatement *
AosJqlStmtDatascanner::clone()
{
	return OmnNew AosJqlStmtDatascanner(*this);
}


void 
AosJqlStmtDatascanner::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtDatascanner::setName(OmnString name)
{
	mName = name;
}

void
AosJqlStmtDatascanner::setOrder()
{
	mNeedToOrder = true;
}



void 
AosJqlStmtDatascanner::setDirList(vector<AosJqlStmtDataConnector::AosDirList*> *dirl)
{
	mDirList = dirl;
}
