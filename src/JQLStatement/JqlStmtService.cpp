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
//
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtService.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "JQLStatement/JqlStmtDataset.h"
#include "JQLStatement/JqlStmtDataRecord.h"
#include "JQLStatement/JqlStmtDataProc.h"
#include "JQLStatement/JqlStmtDataField.h"
#include "JQLStatement/JqlStmtDatascanner.h"
#include "JQLStatement/JqlStmtSchema.h"
#include "JQLStatement/JqlStmtJob.h"
#include "JQLStatement/JqlStmtDataConnector.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLExpr/ExprString.h"
#include "JQLExpr/ExprNameValue.h"
#include "StreamEngine/JqlService.h"

#include "Debug/Debug.h"

AosJqlStmtService::AosJqlStmtService()
{
	mOp = JQLTypes::eOpInvalid;
	mServiceName = "";
	mJobName = "";
	mStatus = AosJqlStmtService::eStopped;
	mMaxTask = 40;
}

AosJqlStmtService::~AosJqlStmtService()
{
}

bool
AosJqlStmtService::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createService(rdata);
	if (mOp == JQLTypes::eShow) return showServices(rdata);
	if (mOp == JQLTypes::eDrop) return dropService(rdata);
	if (mOp == JQLTypes::eDescribe) return describeService(rdata);
	if (mOp == JQLTypes::eStart) return startService(rdata);
	if (mOp == JQLTypes::eStop) return stopService(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Service", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtService::createService(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString msg;
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eServiceDoc, mServiceName);
	if (doc)
	{
		//already exists
		msg << "Service " << mServiceName << " already exists. "
			    << " Use \"drop\" command to delete firstly.";
		rdata->setJqlMsg(msg);
		return true;
	}

	//generate the xml data firstly
	doc = convertToXml(rdata);
	if (!doc) return false;

	doc->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	doc->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_SERVICE_CONTAINER);

	if ( !createDoc(rdata, JQLTypes::eServiceDoc, mServiceName, doc) )
	{
		msg << "Failed to create Service doc.";
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "Service '" << mServiceName << "'created successfully"; 
	rdata->setJqlMsg(msg);

	return true;
}

AosXmlTagPtr
AosJqlStmtService::convertToXml(const AosRundataPtr &rdata)
{
	OmnString doc;
	doc << "<service zky_service_name=\"" << mServiceName 
		<< "\" zky_job_name =\"" << mJobName
		<< "\" zky_sdoc_objid=\"" << getObjid(rdata, JQLTypes::eServiceDoc, mServiceName) 
		<< "\" zky_otype=\"service\" zky_pctrs=\"jobctnr\" zky_version_version=\"1\">"
		<<		"<scheduler zky_numslots=\"" << mMaxTask << "\" zky_serviceschedulerid=\"norm\"/>" 
		<< "</service>";    

	return AosXmlParser::parse(doc AosMemoryCheckerArgs); 
}

bool
AosJqlStmtService::showServices(const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> Services;
	if ( !getDocs(rdata, JQLTypes::eServiceDoc, Services) )
	{
		OmnString msg;
		msg << "No Service Found. "; 
		rdata->setJqlMsg(msg);
		return true;
	}
	
	int total = Services.size() ; 
	OmnString content;
	content << "<content total=\"" << total << "\"></content>";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	for(u32 i=0; i<Services.size(); i++)
	{
		AosXmlTagPtr Service = Services[i];
		doc->addNode(Service);
	}
	rdata->setResults(content);
	if (mContentFormat == "xml")
	{
		rdata->setResults(doc->toString());
		return true;
	}

	vector<OmnString> fields;
	fields.push_back("zky_service_name");
	fields.push_back("zky_job_name");
	map<OmnString, OmnString> alias_name;
	alias_name["zky_service_name"] = "ServiceName";
	alias_name["zky_job_name"] = "JobName";

	OmnString msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}

bool
AosJqlStmtService::startService(const AosRundataPtr &rdata)
{
	OmnString msg;
	std::vector<std::string>  str_values;    
	OmnTagFuncInfo << endl;

	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eServiceDoc, mServiceName);
	if (!doc)
	{
		msg << "Service " << mServiceName << " doesn't exist. ";
		rdata->setJqlMsg(msg);
		return true;
	}

	OmnString conf = "";
	for(u32 i=0; i<mConfParms->size(); i++)
	{
		AosExprNameValue *parm = dynamic_cast<AosExprNameValue*>((*mConfParms)[i].getPtr());
		//AosExprNameValue *parm = mConfParms[i];
		aos_assert_r(parm, false);
		
		AosExprObjPtr expr = parm->getValueAsExpr();
		if(i > 0)
			conf << "," <<  parm->getName() << " : " 
				<< expr->dump();
		else
			conf <<  parm->getName() << " : " 
				<< expr->dump();

	}
	//new service
	AosJqlService *service = OmnNew AosJqlService(0);
	service->runByJql(rdata.getPtr(), mServiceName, conf);
/*
	mJobName = doc->getAttrStr("zky_job_name"); 
	OmnString jobObjid = getObjid(rdata, JQLTypes::eJobDoc, mJobName);
	//str_values.push_back(mServiceName);
	//mHandler.str_values = str_values;
	if ( !startJob(rdata, jobObjid) )
	{
		OmnCout << "Failed to start service " << mServiceName << endl;
		return false;
	}
*/
	OmnCout << "Service " << mServiceName << " started successfully." << endl; 
	return true;
}

bool
AosJqlStmtService::stopService(const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;

	OmnString msg;
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eServiceDoc, mServiceName);
	if ( !doc )
	{
		//already exists
		msg << "Service " << mServiceName << " doesn't exist. ";
		rdata->setJqlMsg(msg);
		return true;
	}

	mJobName = doc->getAttrStr("zky_job_name"); 
	OmnString jobObjid = getObjid(rdata, JQLTypes::eJobDoc, mJobName);
	OmnString arguments = "";

	if ( !stopJob(jobObjid, arguments) )
	{
		OmnCout << "Failed to stop service ." << mServiceName << endl;
		return false;
	}
	OmnCout << "Service " << mServiceName << " stopped successfully." << endl; 
	return true;
}

bool 
AosJqlStmtService::dropService(const AosRundataPtr &rdata)
{
	if (mServiceName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eServiceDoc, mServiceName);

	if ( !deleteDocByObjid(rdata, objid) )
	{                                                        
		msg << "Failed to delete the service" << mServiceName;
		rdata->setJqlMsg(msg);
		return false;                                        
	}                                                        

	msg << "Service " << mServiceName << " dropped successfully.";
	rdata->setJqlMsg(msg);
	return true;
}


bool
AosJqlStmtService::describeService(const AosRundataPtr &rdata)
{
	return true;
}


AosJqlStatement *
AosJqlStmtService::clone()
{
	return OmnNew AosJqlStmtService(*this);
}


void 
AosJqlStmtService::dump()
{
}

//
//add a system table to monitor the service running status
//
bool 
AosJqlStmtService::createServiceSysTable()
{
	//check if the db sysdb exists, if not, create it firstly
	

	//check if the service sys table exists or not. If not, create it firstly
	
	return true;
}

//
//insert an entry into the service system monitoring table
//
bool 
AosJqlStmtService::addServiceData(OmnString serviceName, AosXmlTagPtr data)
{

	return true;
}
