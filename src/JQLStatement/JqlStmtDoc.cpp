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
#include "JQLStatement/JqlStmtDoc.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"

#include "JQLParser/JQLParser.h"                     
using namespace std;                                 
extern int AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;

AosJqlStmtDoc::AosJqlStmtDoc(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
}

AosJqlStmtDoc::AosJqlStmtDoc()
{
}

AosJqlStmtDoc::~AosJqlStmtDoc()
{
}


bool
AosJqlStmtDoc::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if(mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	if (mOp == JQLTypes::eCreate) return createDoc(rdata);
	//if (mOp == JQLTypes::eShow) return showDocs(rdata);
	//if (mOp == JQLTypes::eDrop) return dropDocs(rdata);
	//if (mOp == JQLTypes::eDescribe) return describeDoc(rdata);
	//if (mOp == JQLTypes::eRun) return runDoc(rdata);
	if (mOp == JQLTypes::eShow) return showDoc(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Doc", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtDoc::createDoc(const AosRundataPtr &rdata)
{
	// This function will save the doc
	if (mDocConf == "") return true;	
	AosXmlTagPtr doc = AosXmlParser::parse(mDocConf AosMemoryCheckerArgs);
	if (!doc)
	{
		rdata->setJqlMsg("ERROR: doc's format is not XML");
		return false;
	}

	doc = createDoc1(rdata, mDocConf);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	u64 docid = doc->getAttrU64("zky_docid", 0);
	OmnString msg = "";
	msg << "Create doc sucessful, docid " << docid;
	rdata->setJqlMsg(msg);

	return true;
}


bool
AosJqlStmtDoc::showDoc(const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc;
	if (mShowType == "docid")
	{
		if (0 == mDocDocid)
		{
			rdata->setJqlMsg("ERROR: docid not equie 0");
			return false;
		}

		doc = getDocByDocid(rdata, mDocDocid);
	}
	else if (mShowType == "objid")
	{
		if (mDocObjid == "")
		{
			rdata->setJqlMsg("ERROR: objid is null");
			return false;
		}

		doc = getDocByObjid(rdata, mDocObjid);
	}
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	OmnString tmp = "<content>";
	tmp << doc->toString() << "</content>";
	//rdata->setResults(tmp);	
	
	if (mContentFormat == "xml")  	
	{
		//Lina JOBFLOW-49 2015/11/02

		OmnString content = "<content>";
		content << doc->toString() << "</content>";
		AosXmlParser parser;
		AosXmlTagPtr doc2 = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);

		rdata->setResults(doc2->toString());
		return true;
	}
	setPrintData(rdata, doc->toString());
	return true;
}


AosJqlStatement *
AosJqlStmtDoc::clone()
{
	return OmnNew AosJqlStmtDoc(*this);
}


void 
AosJqlStmtDoc::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtDoc::setDocObjid(OmnString objid)
{
	mShowType = "objid";
	mDocObjid = objid;
}

void 
AosJqlStmtDoc::setDocDocid(unsigned long docid)
{
	mShowType = "docid";
	mDocDocid = (u64)docid;
}


void 
AosJqlStmtDoc::setDocConf(OmnString doc_conf)
{
	mDocConf = doc_conf;
}
