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
#include "SmartDoc/SdocDataCollect.h"

#include "Debug/Error.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Security/Session.h"
#include "Util/StrSplit.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"


AosSdocDataCollect::AosSdocDataCollect(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_DATACOLLECT, AosSdocId::eDataCollect, flag)
{
}


AosSdocDataCollect::~AosSdocDataCollect()
{
}


bool
AosSdocDataCollect::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// bool
	// AosSdocDataCollect::procDoc(
	//	const OmnString &siteid,
	//	const AosWebRequestPtr &req, 
	//	const AosXmlTagPtr &sdoc,
	//	const AosSessionPtr &session,
	//	OmnString &contenst,
	//	AosXmlRc &errcode,
	//	OmnString &errmsg)
	// {
	// format 
	// <smartdoc 
	// 		zky_sdoctp="dateCollect">
	// 		<query>
	// 			...
	// 		</query>
	// </smartdoc>


	// [{name:'Jul 07', games: 245, movies: 300, music:700},
	// {name:'Aug 07', games: 240, movies: 350, music:550},
	// {name:'Sep 07', games: 355, movies: 400, music:615},
	// {name:'Oct 07', games: 375, movies: 420, music:460},
	// {name:'Nov 07', games: 490, movies: 450, music:625}]
	
	OmnNotImplementedYet;
	return false;
}


/*
bool 
AosSdocDataCollect::runSmartdoc(
			const OmnString &siteid,
			const AosWebRequestPtr &req, 
			const AosXmlTagPtr &root, 
			const AosXmlTagPtr &userobj, 
			const OmnString &sdoc_objid,
			OmnString &contents, 
			const AosRundataPtr &rdata)
{
	// 1. parse the query
	// get sdoc
	// default
	// <doc ...>
	// 	<query>
	// 		...
	// 	</query>
	// </doc>
	// 	
	//AosXmlTagPtr sdoc = AosDocServer::getSelf()->getDoc(siteid, sdoc_objid);
	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDoc(siteid, sdoc_objid, rdata);

	AosStrSplit split;
	OmnString parts[2];
	bool finished;
	int nn = split.splitStr(sdoc_objid, ".", parts, 2, finished);
	if(nn != 2)
	{
		rdata->setError() << "Objid Format is error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString cid = parts[1];
	// AosQueryReqPtr query = parseQuery(siteid, sdoc, cid, errcode, errmsg);
	// if (!query)
	// {
	// 	OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, errmsg);
	// 	throw e;
	// }

	// 2. run the query
	// AosXmlTagPtr childelem = sdoc->getNextChild("query");
	// OmnString query_result;
	// query->procPublic(childelem, contents, cid, errcode, errmsg);
	// query->releaseIILs();
	// in this query, can get games num

	OmnString data ;
	data <<  "{name:'Jul 07', games: 245, movies: 300, music:700}";

	// 3. create the results
	contents << "<Contents>" 
			<< "<record>"
			<<"[" << data << "]"
			<< "</record>"
			<< "</Contents>";

	return true;
}
*/
