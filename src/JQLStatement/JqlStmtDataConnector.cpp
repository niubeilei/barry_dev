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
#include "JQLStatement/JqlStmtDataConnector.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"

#include "AosConf/DataConnector.h"
#include "AosConf/DataSplit.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using namespace AosConf;
using boost::shared_ptr;


AosJqlStmtDataConnector::AosJqlStmtDataConnector(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mConnectorName = "";
	mConnectorType = "";
	mSvrId = "";
	mFileName = "";
	mCoding = "";
	mSplitSize = "";
	mDirList = 0;
	mOp = JQLTypes::eOpInvalid;
}


AosJqlStmtDataConnector::AosJqlStmtDataConnector()
{
	mConnectorName = "";
	mConnectorType = "";
	mSvrId = "";
	mFileName = "";
	mCoding = "";
	mSplitSize = "";
	mDirList = 0;
	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtDataConnector::~AosJqlStmtDataConnector()
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
AosJqlStmtDataConnector::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if(mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;

		return false;
	}

	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createDataConnector(rdata);
	if (mOp == JQLTypes::eShow) return showDataConnectors(rdata);
	if (mOp == JQLTypes::eDrop) return dropDataConnectors(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDataConnector(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL DataConnector", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}

bool
AosJqlStmtDataConnector::createDataConnector(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDataConnectorDoc, mConnectorName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);

	if (doc)
	{
		msg << "DataConnector " << mConnectorName << " already exists. "
			    << " Use \"drop\" command to delete firstly.";
		rdata->setJqlMsg(msg);
		OmnCout << msg << endl;
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

	if ( !createDoc(rdata, JQLTypes::eDataConnectorDoc, mConnectorName, doc) )
	{
		msg << "Failed to create DataConnector doc." ;
		rdata->setJqlMsg(msg);
		OmnCout << msg << endl;
		return true;
	}

	msg << "DataConnector " << mConnectorName << " created successfully.";
	rdata->setJqlMsg(msg);
	OmnCout << msg << endl;
	return true;
}


AosXmlTagPtr
AosJqlStmtDataConnector::convertToXml(const AosRundataPtr &rdata)
{
	OmnString doc;
	boost::shared_ptr<DataConnector> dcon = boost::make_shared<DataConnector>();
	if (mConfParms)
	{
		return getConf(rdata, dcon, mConfParms);
	}

	dcon->setAttribute("zky_name", mConnectorName);
	dcon->setAttribute("zky_otype", "dataconnector");

	boost::shared_ptr<DataSplit> split = boost::make_shared<DataSplit>();
	split->setAttribute("group_size", mSplitSize);

	if (mConnectorType == "FILE" || mConnectorType == "file")
	{
		//dcon->setAttribute(AOSTAG_PHYSICALID, mSvrId);
		//dcon->setAttribute("file_name", mFileName);
		dcon->setAttribute("type", "file");
		dcon->setAttribute("jimo_objid", "dataconnector_file_jimodoc_v0");
		OmnString file_conf = "";
		file_conf << "<file "
			<< AOSTAG_PHYSICALID << "=\"" << mSvrId << "\" "
			<< AOSTAG_CHARACTER << "=\"" << mCoding << "\">"
			<< "<![CDATA[" << mFileName << "]]>"
			<< "</file>";
		dcon->setFile(file_conf.getBuffer());
		split->setAttribute("jimo_objid", "dataspliter_file_jimodoc_v0");
	}
	else if (mConnectorType == "DIRECTORY" || mConnectorType == "directory")
	{
		dcon->setAttribute("type", "directory");
		dcon->setAttribute("jimo_objid", "dataconnector_dir_jimodoc_v0");
		dcon->setAttribute(AOSTAG_CHARACTER, mCoding);
			
		OmnString difs_conf = "";
		difs_conf << "<dirs>";
		for (u32 i = 0; i < mDirList->size(); i++)
		{
			AosDirList *dirl = (*mDirList)[i];
			if (dirl)
			{
				difs_conf << "<dir dir_name=\"" << dirl->mFileName << "\" "
					<< AOSTAG_PHYSICALID << "=\"" << dirl->mSvrId << "\"/>";
			}
		}
		difs_conf << "</dirs>";
		dcon->setDirs(difs_conf.getBuffer());
		split->setAttribute("jimo_objid", "dataspliter_dirfiles_jimodoc_v0");
	}

	dcon->setSplit(split);
	doc = dcon->getConfig();
	AosXmlTagPtr conf = AosXmlParser::parse(doc AosMemoryCheckerArgs); 
	if (!conf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	conf->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	conf->setAttr(AOSTAG_CTNR_PUBLIC, "true");  
	conf->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_CONNECTOR_CONTAINER);   
	return conf;
}

bool
AosJqlStmtDataConnector::showDataConnectors(const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> connectors;
	if( !getDocs(rdata, JQLTypes::eDataConnectorDoc, connectors) )
	{
		OmnString error;
		OmnCout << "No DataConnector found! " << endl;
		rdata->setJqlMsg(error);
		OmnCout << error << endl;
		return true;
	}

	int total = connectors.size() ; 
	OmnString content;
	content << "<content total=\"" << total << "\">";
	for(u32 i=0; i<connectors.size(); i++)
	{
		OmnString vv = connectors[i]->getAttrStr("zky_name", "");
		if (vv != "")
		{
			content << "<record name=\"" << vv << "\"/>";
		}
	}
	content << "</content>";

	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
	}

	vector<OmnString> fields;
	fields.push_back("name");
	map<OmnString, OmnString> alias_name;
	alias_name["name"] = "DataConnectorName";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	OmnString msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}

bool 
AosJqlStmtDataConnector::dropDataConnectors(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDataConnectorDoc, mConnectorName);                  
	bool rslt = deleteDocByObjid(rdata, objid); 
	if (!rslt)         
	{                                                        
		msg << "Failed to drop data connectors doc.";  
		rdata->setJqlMsg(msg);
		OmnCout <<  msg << endl;
		return false;                                        
	}                                                        

	msg << "Data Connector " << mConnectorName << " dropped successfully."; 
	rdata->setJqlMsg(msg);
	OmnCout << msg << endl;
	return true;                                             
}


bool
AosJqlStmtDataConnector::describeDataConnector(const AosRundataPtr &rdata)
{
	if (mConnectorName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 
	string objid = getObjid(rdata, JQLTypes::eDataConnectorDoc, mConnectorName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		OmnString error;
		error << "Failed to display dataconnector " << mConnectorName << " : not found." ;
		rdata->setJqlMsg(error);
		OmnCout << error << endl;
		return false;
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;

	fields.push_back("zky_name");
	fields.push_back("type");
	fields.push_back(AOSTAG_PHYSICALID);
	fields.push_back("file_name");
	fields.push_back(AOSTAG_CHARACTER);
	fields.push_back("group_size");
	alias_name["zky_name"] = "Name";
	alias_name["type"] = "Type";
	alias_name[AOSTAG_PHYSICALID] = "ServerId";
	alias_name["file_name"] = "FileName";
	alias_name[AOSTAG_CHARACTER] = "Character";
	alias_name["group_size"] = "GroupSize";

	OmnString content = "<content>";
	content << doc->toString() << "</content>";
	AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	OmnString msg = printTableInXml(fields, alias_name, def);
	if (mContentFormat == "xml")
	{
		rdata->setResults(def->toString());
		return true;
	}
	setPrintData(rdata, msg);
	return true;
}


AosJqlStatement *
AosJqlStmtDataConnector::clone()
{
	return OmnNew AosJqlStmtDataConnector(*this);
}


void 
AosJqlStmtDataConnector::dump()
{
	//OmnScreen << "JQL Check Doc Exist Statement: " << "" << endl;
}


/***************************
 * Getter/setter
 * **************************/
void
AosJqlStmtDataConnector::setName(OmnString name)
{
	mConnectorName = name;
}


void
AosJqlStmtDataConnector::setType(OmnString type)
{
	mConnectorType = type;
}


void 
AosJqlStmtDataConnector::setSvrId(int64_t svr_id)
{
	mSvrId << svr_id;
}


void
AosJqlStmtDataConnector::setFileName(OmnString file_name)
{
	mFileName = file_name;
}


void
AosJqlStmtDataConnector::setCoding(OmnString coding)
{
	mCoding	= coding;
}


void 
AosJqlStmtDataConnector::setSplitSize(OmnString split_size)
{
	mSplitSize = split_size;
}


void 
AosJqlStmtDataConnector::setDirList(vector<AosDirList*> *dirl)
{
	mDirList = dirl;
}
