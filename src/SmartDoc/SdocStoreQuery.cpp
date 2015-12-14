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
#include "SmartDoc/SdocStoreQuery.h"

#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "Util/String.h"
#include "SEUtilServer/UserDocMgr.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "StoreQuery/StoreQueryMgr.h"


AosSdocStoreQuery::AosSdocStoreQuery(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_STOREQUERY, AosSdocId::eStoreQuery, flag)
{
}


AosSdocStoreQuery::~AosSdocStoreQuery()
{
}


bool
AosSdocStoreQuery::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//<smartdoc zky_type="query|retrieve|export" zky_need_export="true" zky_sdoctp="storequery"  zky_rsltdoc_ctnr=”xxx” zky_max_records=”total records of blocks ” zky_blocksize=”xxx”>   //zky_blocksize is total records of one block
	//		<query>
	//			<conds>
	//				<cond  type="AND">
	//					<term/>
	//				</cond>
	//
	//				...
	//				<cond  type="AND">
	//					<term/>
	//				</cond>
	//			</conds>
	//			<fnames>
	//				<fname type="1">
	//					<oname><![CDATA[realname]]></oname>
	//					<cname><![CDATA[realname]]></cname>
	//				</fname>
	//				...
	//				<fname type="1">
	//					<oname><![CDATA[checked]]></oname>
	//					<cname><![CDATA[checked]]></cname>
	//				</fname>
	//			</fnames>
	//		</query>
	//		<schedule zky_scheduletype=”time_int|timeofday|dayofweek|dayofmonth” zky_queryapp_id=”xxx” zky_priority=”xxx” time="xxx"/>  //ignore this node
	//		<results .../>   //attributes for stored query result doc
	//		<retrieve zky_query_result_doc_id="xxx" zky_start_index="xxx" zky_page_size="xxx"/>
	//
	//		<export zky_query_result_doc_id="xxx">
	//			<cells whereisvalue="attribute|node">
	//				<cellname>xxx</cellname>
	//				<cellname>xxx</cellname>
	//				<cellname>xxx</cellname>
	//			</cells>
	//			<downloaddoc>
	//				<attribute name="xxx"></attribute>
	//				...
	//				<attribute name="xxx"></attribute>
	//			</downloaddoc>
	//		</export>
	//		
	// </smartdoc>

	if (!sdoc)
	{
		rdata->setError() << "Missing sdoc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString zky_type = sdoc->getAttrStr("zky_type", "");
	aos_assert_r(zky_type != "", false);
	if(zky_type == "query")
	{
		AosXmlTagPtr query = sdoc->getFirstChild("query");
		aos_assert_r(query, false);
		return addQuery(sdoc, rdata);
	}

	return true;
}

bool
AosSdocStoreQuery::addQuery(const AosXmlTagPtr &querynode, const AosRundataPtr &rdata)
{
	return AosStoreQueryMgr::getSelf()->addQuery(querynode, rdata); 
}

