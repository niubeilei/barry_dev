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
#include "SmartDoc/SdocTotal.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosSdocTotal::AosSdocTotal(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_GETTOTAL, AosSdocId::eGetTotal, flag)
{
}


AosSdocTotal::~AosSdocTotal()
{
}


bool 
AosSdocTotal::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// bool
	// AosSdocTotal::procDoc(
	//	const OmnString &siteid,
	//	const AosWebRequestPtr &req,
	//	const AosXmlTagPtr &def,
	//	const AosSessionPtr &session,
	//	OmnString &contenst,
	//	AosXmlRc &errcode,
	//	OmnString &errmsg)
	// {

	// This function retrieves a series of totals based on the doc 
	// 'def', which should be in the form:
	// 	<doc ...>
	// 		<data>
	// 			<rcd .../>
	// 			<rcd .../>
	// 			...
	// 		</data>
	// 	</doc>
	// each <rcd .../> defines one data. It contains a query. It should
	// be in the following format:
	// 	<rcd name="xxx">
	// 		<query>
	// 			...
	// 		</query>
	// 	</rcd>
	// 
	// The response should be in the form:
	// 	<data ...>
	// 		<records>
	// 			<rcd value="xxx">name</rcd>
	// 			<rcd value="xxx">name</rcd>
	// 			...
	// 		</records>
	// 	</data>
	//
	// Example:
	// 	<doc zky_otype="zky_smtdoc"> 
	// 		<data>
	// 			<rcd name="VPD">
	// 				<query type="eq">
	// 					<lhs>zky_otype</hls>
	// 					<rhs>vpd</rhs>
	// 				</query>
	// 			</rcd>
	// 			<rcd name="Image">
	// 				<query type="eq">
	// 					<lhs>zky_otype</hls>
	// 					<rhs>image</rhs>
	// 				</query>
	// 			</rcd>
	// 			<rcd name="Version">
	// 				<query type="eq">
	// 					<lhs>zky_otype</hls>
	// 					<rhs>zky_ver</rhs>
	// 				</query>
	// 			</rcd>
	// 		</data>
	// 	</doc>
	//
	rdata->setError() << "Not implemented yet";
	OmnNotImplementedYet;
	return false;
	/*
	aos_assert_r(def, "");
	AosXmlTagPtr data = def->getFirstChild(AOSTAG_DATA);
	aos_assert_r(data, "");
	AosXmlTagPtr rcd = data->getFirstChild();
	aos_assert_r(rcd, "");
	OmnString contents = "<data><records>";
	int64_t total;
	while (rcd)
	{
		OmnString name = rcd->getAttrStr(AOSTAG_NAME);
		AosXmlTagPtr query = rcd->getFirstChild();
		if (!query)
		{
			OmnAlarm << "Query is empty!" << enderr;
		}
		else 
		{
			if (!AosQueryTerm::getTotal(siteid, query, total, errmsg))
			{
				OmnAlarm << "Failed to run query: " << errmsg 
					<< ". Query: " << rcd->toString() << enderr;
			}
			else
			{
				contents << "<rcd value=\"" << total 
					<< "\"><![CDATA[" << name << "]]></rcd>";
			}
		}
		rcd = data->getNextChild();
		if (!rcd) break;
	}

	contents << "</records></data>";
	return true;
	*/
}
