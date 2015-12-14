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
// 01/05/2014	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocCompareTwoDocs.h"

#include "Rundata/Rundata.h"
#include "UtilComm/TcpClient.h"
#include "Actions/ActSeqno.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"
#include "API/AosApiG.h"

AosSdocCompareTwoDocs::AosSdocCompareTwoDocs(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_COMPARETWODOCS, AosSdocId::eCompareTwoDocs, flag)
{
}


AosSdocCompareTwoDocs::~AosSdocCompareTwoDocs()
{
}

bool
AosSdocCompareTwoDocs::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// req format is 
	//
	//<sdoc filter="30|90" msgctnr="test1"> 
	//<query>
	//</query>
	//<compare attrname=”bloodpressure”>
	//	<sort>
	//		<entry value=”0” status=”lower”>
	//		</entry>
	//
	//		<entry value="1" status=”normal>
	//		</entry>
	//
	//		<entry value=”2” status=”higher”>
	//		</entry>
	//	<sort>
	//</compare>
	//<createdoc>
	//	<entry key="zky_objid"/>
	//	<entry key="bloodpressure"/>
	//	<entry key="ctime"/>
	//</createdoc>
	//</sdoc>
	
	//old doc:<myrecord date=”2013.12.5” bloodpressure=”lower” heartbeat=”70”/>
	//new doc:<myrecord date=”2013.12.31” bloodpressure=”high” heartbeat=”65”/>

	//result :
	//
	//1.create trend doc:
	//
	//<doc>
	//	<record zky_objid_old=”xxx” zky_objid_new=”xxx”
	//	zky_ctime_old=”2013.12.5” 
	//	zky_ctime_new=”2013.12.31”
	//	trend="up|down|normal">
	//	...
	//	<record zky_objid_old=”xxx” zky_objid_new=”xxx”
	//	zky_ctime_old=”2013.12.5” 
	//	zky_ctime_new=”2013.12.31”
	//	trend="up|down|normal">
	//</doc>
	//
	//2.create message doc:
	//
	//<msgdoc zky_pctrs="test1" item="bloodpressure" old_docid="xxx" new_docid="xxx" user="xxx" sendflag="true|false">
	//</msgdoc>
	
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString oldobjid = sdoc->getAttrStr("oldobjid", "");
	aos_assert_r(oldobjid != "", false);
	OmnString newobjid = sdoc->getAttrStr("newobjid", "");
	aos_assert_r(newobjid != "", false);

	AosXmlTagPtr comparetag = sdoc->getFirstChild("compares");
	aos_assert_r(comparetag, false);

	AosXmlTagPtr olddoc = AosGetDocByObjid(oldobjid, rdata); 
	aos_assert_r(olddoc, false);
	AosXmlTagPtr newdoc = AosGetDocByObjid(newobjid, rdata); 
	aos_assert_r(newdoc, false);

	OmnString results = "<Contents ";
	results << "old_objid=\"" << oldobjid << "\" ";
	results << "new_objid=\"" << newobjid << "\" ";
	results << ">";

	AosXmlTagPtr entry = comparetag->getFirstChild();
	while(entry)
	{
		OmnString attrname = entry->getAttrStr("attrname", "");
		aos_assert_r(attrname!= "", false);
		OmnString datatype = entry->getAttrStr("datatype", "");
		aos_assert_r(datatype!= "", false);
		if(datatype == "int64")
		{
			AosXmlTagPtr larger = entry->getFirstChild("larger");
			aos_assert_r(larger, false);
			AosXmlTagPtr smaller = entry->getFirstChild("smaller");
			aos_assert_r(smaller, false);
			AosXmlTagPtr normal = entry->getFirstChild("normal");
			aos_assert_r(normal, false);
			bool hasattr = larger->hasAttribute("value",  false);
			aos_assert_r(hasattr, false);
			hasattr = smaller->hasAttribute("value" ,false);
			aos_assert_r(hasattr, false);
			int64_t lv = larger->getAttrInt64("value", 0);
			int64_t sv = smaller->getAttrInt64("value", 0);
			aos_assert_r(lv>sv,  false);
			int64_t ov = olddoc->getAttrInt64(attrname, 0);
			int64_t nv = newdoc->getAttrInt64(attrname, 0);
			int64_t diffvalue = nv-ov;

			OmnString status;
			OmnString text;
			if(diffvalue>lv)
			{
				status = larger->getAttrStr("status", "");
				text = larger->getNodeText();
			}
			else if(diffvalue<sv)
			{
				status = smaller->getAttrStr("status", "");
				text = smaller->getNodeText();
			}
			else
			{
				status = normal->getAttrStr("status", "");
			}
			results << "<entry name=\"" << attrname << "\" status=\"" << status << "\" >" << text << "</entry>"; 
		}
		else
		{
			rdata->setError() << "Can't resolve the datatype " << datatype;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		entry = comparetag->getNextChild();
	}
	results << "</Contents>";

	rdata->setResults(results);
	return true;
}

