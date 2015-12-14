////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocJoin.h"


#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DbQuery/Query.h"
#include "Debug/ErrId.h"
#include "Debug/Except.h"
#include "SEInterfaces/DocClientObj.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocServerCb.h"
#include "SmartDoc/SmartDoc.h"
#include "SEUtil/Docid.h"
#include "Util/SPtr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"



AosSdocJoin::AosSdocJoin(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_JOIN, AosSdocId::eJoin, flag)
{
}

AosSdocJoin::~AosSdocJoin()
{
}

bool
AosSdocJoin::init(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// The smartdoc format should be in the form:
	//  <doc AOSTAG_OTYPE=AOSOTYPE_SMARTDOC 
	//  	 AOSTAG_SMARTDOC_OPR="query|simulate|run">
	// 		<query ...>
	// 		<join ...>
	// 		<actions ...>
	// 			<action type="xxx">
	// 				<attr namebd="xxx" attrname=" " />
	// 			<action ...>
	// 			...
	// 			<action ...>
	// 		</actions>
	// 	</...>
	bool rslt = parseActions(sdoc, mActions, rdata);
	if (!rslt)
	{
		OmnAlarm  << rdata->getErrmsg() << enderr;
		return false;
	}

	rslt = parseJoin(sdoc, rdata);
	if (!rslt)
	{
		OmnAlarm  << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool
AosSdocJoin::parseJoin(const AosXmlTagPtr &sdoc, 
				const AosRundataPtr &rdata) 
{
	AosXmlTagPtr joinTag = sdoc->getFirstChild(AOSTAG_JOINTAG);
	if (!joinTag)
	{
		rdata->setError() << "Missing join tag";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mJoinAttr = joinTag->getAttrStr(AOSTAG_ATTRNAME);
	if (mJoinAttr == "")
	{
		rdata->setError() << "Missing Join Attribute";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mJoinType = joinTag->getAttrStr(AOSTAG_TYPE);
	if (mJoinType == "")
	{
		rdata->setError() << "Missing Join Type";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


bool 
AosSdocJoin::modifyDoc(const AosRundataPtr &rdata)
{
	AosXmlTagPtr def;
	for (int i=0; i<(int)mActions.size(); i++)
	{
		mActions[i]->run(def, rdata);
	}
	return true;
}


bool
AosSdocJoin::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//rdata->setError() << "Not implemented yet!";
	//OmnNotImplementedYet;
	//return false;
	//                                                          
	// The response should be in the form:                      
	//   <data ...>                                              
	//       <records>                                           
	//        <rcd value="xxx">name</rcd>                     
	//        <rcd value="xxx">name</rcd>                     
	//           ...                                             
	//       </records>                                          
	//   </data>                                                 
	// 
	//  Example:                                                 
	//  <doc zky_otype="zky_smtdoc" zky_sdoc_opr="query|simulate|run">
	//       <query>                                             
	//       ...                                                 
	//       </query>                                            
	//       <join attrname="xxx" type="docid|objid"/>           
	//
	//    <actions>                                           
	//  	  <action type="addattr">                         
	//         <attr name="xxx" value="xxx"/>              
	//         ...                                         
	//       </action>                                       
	//      </actions>                                          
	//  </doc>                                                  
	//                                                              
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = init(sdoc, rdata);
	if (!rslt)
	{
		rdata->setError()<< "Init SmartDoc Join Error";
		return false;
	}

	mOperator = AosSdocOpr::toEnum(sdoc->getAttrStr(AOSTAG_SMARTDOC_OPR));
	OmnString run_doc = sdoc->getAttrStr(AOSTAG_RUN);
	if (run_doc != "no")
	{
		// Chen Ding, 2011/01/18
		//u64 userid = rdata->getUserid();
		AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				sdoc->getAttrU64(AOSTAG_DOCID, 0), 
				sdoc->getAttrStr(AOSTAG_OBJID), 
				AOSTAG_RUN, "no", "no", false, false, true);
	}

	AosQueryReqObjPtr qry = AosQuery::parseQuery(sdoc, rdata);
	if (!qry)
	{
		OmnAlarm  << rdata->getErrmsg() << enderr;
		return false;
	}
	AosXmlTagPtr query_xml = sdoc->getFirstChild(AOSTAG_QUERY);
	if (!query_xml)
	{
		rdata->setError() << "Missing Query Tag";
		return false;
	}
	switch (mOperator)
	{
	case AosSdocOpr::eQueryOnly:
		 return query(qry, query_xml, rdata);

	case AosSdocOpr::eSimulate:
		 query(qry, query_xml, rdata);
		 return runPriv(rdata, true);

	case AosSdocOpr::eRun:
		 query(qry, query_xml, rdata);
		 return runPriv(rdata, false);

	default:
		 rdata->setError() << "Unrecognized operator:";
		 OmnAlarm << rdata->getErrmsg() << mOperator << enderr;
		 return false;
	}
	OmnShouldNeverComeHere;
	return false;
}


bool
AosSdocJoin::query(
		const AosQueryReqObjPtr &query,
		const AosXmlTagPtr &query_xml,
		const AosRundataPtr &rdata)
{
	bool rslt = AosQuery::doQuery(query, query_xml, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSdocJoin::runPriv(const AosRundataPtr &rdata, bool tag)
{
	AosXmlTagPtr sourceXml, targetXml;
	sourceXml = 0;
	targetXml = 0;

	// though this step, can get a OmnString , the query result..
	AosXmlParser parser;
	AosXmlTagPtr queryResultXml = parser.parse(rdata->getResults(), "" AosMemoryCheckerArgs);
	if(!queryResultXml)
	{
		rdata->setError() << "Failed to Retrieve Query Result";
		return false;
	}

	u64 sourceDocid = 0;
	AosXmlTagPtr tmp = queryResultXml->getFirstChild("record");
	if (!tmp)
	{
		rdata->setError() << "No Record in QueryResult";
		return false;
	}

	while(tmp)
	{
		sourceDocid = tmp->getAttrU64(AOSTAG_DOCID, 0);
		sourceXml = AosDocClientObj::getDocClient()->getDocByDocid(sourceDocid, rdata);	//get sourceXml
		if (!sourceXml)
		{
			rdata->setError() << "Failed to retriee source doc!";
			return false;
		}

		if (mJoinAttr == "")
		{
			rdata->setError() << "Join attribute is null";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (mJoinType == "")
		{
			rdata->setError() << "Join attribute is null";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	
		OmnString targetDocid;
		OmnString cid = rdata->getCid();

		targetDocid = sourceXml->getAttrStr(mJoinAttr);		
		//targetDocid << "." << cid;

		if (targetDocid != "")
		{
			if (mJoinType == "docid")
			{
				u64 did = AosDocid::convertToU64(targetDocid);
				targetXml = AosDocClientObj::getDocClient()->getDocByDocid(did, rdata);
			}
			else 
			{
				// joinType == objid
				targetXml = AosDocClientObj::getDocClient()->getDocByObjid(targetDocid, rdata);
			}

			if (!targetXml)
			{
				rdata->setError() << "Failed to Retrieve TargetXml";
				return false;
			}
		}
		else
		{
			rdata->setError() << "Failed to Get TargetDocid";
			return false;
		}
		rdata->setSourceDoc(sourceXml, true);
		rdata->setTargetDoc(targetXml, true);
		modifyDoc(rdata);
		tmp = queryResultXml->getNextChild();
	}

	rdata->setOk();
	return true;
}


// get targetXml by sourceXml
bool
AosSdocJoin::nextDoc(
		const AosXmlTagPtr &queryResultXml,
		AosXmlTagPtr &sourceXml,
		AosXmlTagPtr &targetXml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mJoinAttr != "", false);
	aos_assert_r(mJoinType != "", false);
	
	OmnString targetDocid;
	sourceXml = 0;
	targetXml = 0;
	while (nextDoc(queryResultXml, sourceXml, rdata))	//get next sourceXml
	{
		aos_assert_r(sourceXml, false);

		targetDocid = sourceXml->getAttrStr(mJoinAttr);		
		if (targetDocid != "")
		{
			if (mJoinType == "docid")
			{
				u64 did = AosDocid::convertToU64(targetDocid);
				targetXml = AosDocClientObj::getDocClient()->getDocByDocid(did, rdata);
				//targetXml = AosDocClient::getSelf()->getDocByDocId(targetDocid, "", rdata);
			}
			else 
			{
				// joinType == objid
				targetXml = AosDocClientObj::getDocClient()->getDocByObjid(targetDocid, rdata);
			}
			aos_assert_r(targetXml, false);
			return true;
		}
	}
	return false;
}


// retrieve sourceXml
bool 
AosSdocJoin::nextDoc(
		const AosXmlTagPtr &queryResultXml,
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	u64 sourceDocid = 0;
	AosXmlTagPtr tmp;
	while(tmp = queryResultXml->getNextChild("record"))
	{
		sourceDocid = tmp->getAttrU64(AOSTAG_DOCID, 0);
		xml = AosDocClientObj::getDocClient()->getDocByDocid(sourceDocid, rdata);
		return true;
	}
	return false;
}
