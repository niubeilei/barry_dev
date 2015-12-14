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
//
// Modification History:
// 08/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILSelector/IILSelector.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "CounterClt/CounterClt.h"
//#include "DocClient/DocClient.h"
#include "IILSelector/Ptrs.h"
#include "SEUtil/IILName.h"
#include "QueryUtil/QrUtil.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"

static AosIILSelectorPtr	sgIILSelectors[AosIILSelectorId::eMax];
static OmnMutex				sgLock;


bool		 
AosIILSelector::resolveIIL(
		const AosXmlTagPtr &term, 
		OmnString &iilname, 
		const AosRundataPtr &rdata)
{
	// The 'term' is in the following format:
	// 	<term ...>
	// 		<selector type="xxx" .../>
	// 		<cond .../>
	// 	</term>
	iilname = "";
	aos_assert_rr(term, rdata, 0);
	AosXmlTagPtr selector_tag = term->getFirstChild(AOSTAG_SELECTOR);
	if (!selector_tag)
	{
		// There is no IIL selector. 
		rdata->setError() << "Missing selector tag!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString typestr = selector_tag->getAttrStr(AOSTAG_TYPE);
	AosIILSelectorId::E type = AosIILSelectorId::toEnum(typestr);
	OmnString ctnr_objid;
	OmnString aname;
	switch (type)
	{
	case AosIILSelectorId::eMicroblog:
		 // <selector type=AOIILSELTYPE_ATTR/>
		 // 1. Retrieve the user's account doc.
		 // 2. Retrieve the IILId from that doc.
		 // 3. Return that IILId.
		 break;
	
	case AosIILSelectorId::eLog:
		 // The format is:
		 // <term ...>
		 // 	<selector type=AOSIILSELECTYPE_LOG
		 // 		container="xxx"
		 // 		logtype="xxx"/>
		 // 	<cond .../>
		 // </term>
		 return selectLogIIL(iilname, selector_tag, rdata);

	case AosIILSelectorId::eAttr:
	case AosIILSelectorId::eContainer:
		 // The format is:
		 // <term ...>
		 // 	<selector type=AOSIILSELTYPE_ATTR
		 // 		container="xxx"
		 // 		AOSTAG_ANAME="xxx"/>
		 // 	<cond .../>
		 // </term>
		 {
			 OmnString ctnr_objid = selector_tag->getAttrStr(AOSTAG_CONTAINER_NOZK);
			 if (ctnr_objid == "")
			 {
				 ctnr_objid = selector_tag->getNodeText();
			 }
			 // AosQrUtil::procQueryVars(ctnr_objid, rdata);
			 OmnString aname = selector_tag->getAttrStr(AOSTAG_ANAME);
			 // AosQrUtil::procQueryVars(aname, rdata);
			 if (ctnr_objid == "" && aname == "")
			 {
				 //AosSetError(rdata, AosErrmsgId::eQueryIncorrect);
				 return false;
			 }
			 iilname = AosIILName::composeCtnrMemberListing(ctnr_objid, aname);
			 
			 if (ctnr_objid == "")
			 {
				 return true;
			 }
			 AosXmlTagPtr doc = AosGetDocByObjid(ctnr_objid, rdata);
			 if (!doc) return true;
			 bool isTable = (doc->getAttrStr(AOSTAG_OTYPE, "") == AOSTAG_TABLE);
			 if (!isTable) return true;
			 AosXmlTagPtr index_nodes = doc->getFirstChild("indexes"); 
			 if (index_nodes)                                                    
			 {                                                                   
				 AosXmlTagPtr index_node = index_nodes->getFirstChild(true);     
				 while (index_node)                                              
				 {                                                               
					 OmnString objid = index_node->getAttrStr("idx_name"); 
					 index_node = AosGetDocByObjid(objid, rdata);
					 OmnString iilname1;
					 iilname1 << index_node->getAttrStr("zky_iilname");
					 aos_assert_r(iilname1 != "", 0);                             
					 AosXmlTagPtr columns = index_node->getFirstChild("columns");
					 aos_assert_r(columns, 0);                                   
					 AosXmlTagPtr column = columns->getFirstChild(true);         
					 OmnString field = column->getAttrStr("zky_name");                     
					 aos_assert_r(field != "", 0);                               
					 if (aname == AOSTAG_HPCONTAINER)
					 {
						 iilname = iilname1;
					 }
					 if (field == aname)
					 {
						 iilname = iilname1;
						 return true;
					 }
					 //else if (aname == AOSTAG_HPCONTAINER)
					 //{
					 //	 iilname = 
					 //}
					 index_node = index_nodes->getNextChild();     
				}
			 }
			 return true;
		 }
		 break;

	case AosIILSelectorId::eTags:
		 // The format is:
		 // <term ...>
		 // 	<selector type=AOSIILSELTYPE_ATTR
		 // 		container="xxx">tag,tag,...</tag>
		 // 	<cond .../>
		 // </term>
		 {
			 OmnNotImplementedYet;
			 return false;
		 }
		 break;

	case AosIILSelectorId::eIILName:
		 // <term ...>
		 // 	<selector type=AOSIILSELTYPE_IILNAME>xxx</selector>
		 // </term>
		 // or
		 // <term ...>
		 // 	<selector type=AOSIILSELTYPE_IILNAME iilname="xxx"/>
		 // </term>
		 
		 // Ken Lee, 2013/01/21
		 iilname = selector_tag->getNodeText();
		 if (iilname == "")
		 {
		 	iilname = selector_tag->getAttrStr(AOSTAG_IILNAME);
		 }
		 return true;

	default:
		 return false;
	}

	rdata->setError() << "Unrecognized IIL selector: " << typestr;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}


bool
AosIILSelector::resolveIIL(
		const AosXmlTagPtr &term,
		u64 &iilid,
		const AosRundataPtr &rdata)
{
	// The 'term' is in the following format:
	//  <term ...>
	//      <selector type="xxx" .../>
	//      <cond .../>
	//  </term>

	iilid = 0;
	aos_assert_rr(term, rdata, 0);
	AosXmlTagPtr selector_tag = term->getFirstChild(AOSTAG_SELECTOR);
	if (!selector_tag)
	{
		// There is no IIL selector. 
		rdata->setError() << "Missing selector tag!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString typestr = selector_tag->getAttrStr(AOSTAG_TYPE);
	AosIILSelectorId::E type = AosIILSelectorId::toEnum(typestr);
	OmnString ctnr_objid;
	OmnString aname;
	switch (type)
	{
	case AosIILSelectorId::eIILId:
		 // <term ...>
		 //     <selector type=AOSIILSELTYPE_IILID
		 //         iilid="xxx"/>
		 // </term>
		 iilid = selector_tag->getAttrU64(AOSTAG_IILID, 0);
		 return true;
	default:
		 return false;
	}

	rdata->setError() << "Unrecognized IIL selector: " << typestr;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}

bool
AosIILSelector::selectLogIIL(
			OmnString &iilname,
			const AosXmlTagPtr &selector_tag,
			const AosRundataPtr &rdata)
{
	 // The format is:
	 // <term ...>
	 // 	<selector 
	 // 		type=AOSIILSELECTYPE_LOG
	 // 		gbl_iil="true|false"
	 // 		container="xxx"
	 // 		logname="xxx"/>
	 // 	<cond .../>
	 // </term>
	OmnString ctnr_objid = selector_tag->getAttrStr(AOSTAG_CONTAINER_NOZK);
	OmnString logname = selector_tag->getAttrStr("logname", AOSTAG_DFT_LOGNAME);

    if (ctnr_objid == "")
	{
		 AosSetError(rdata, AosErrmsgId::eMissingContainer);
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}
	
	if (logname == "")
	{
		 AosSetError(rdata, AosErrmsgId::eLogNameIsNull);
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}
	
	iilname = AosIILName::composeLogListingIILName(
		 			rdata->getSiteid(),ctnr_objid, logname);
	aos_assert_r(iilname != "", false);
	return true;
}
