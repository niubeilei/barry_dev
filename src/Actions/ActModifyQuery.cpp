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
// This action sets a value to rundata:
//
// Modification History:
// 11/27/2012 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActModifyQuery.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "QueryClient/QueryClient.h"
#include <string>
#include <vector>
using namespace std;

AosActModifyQuery::AosActModifyQuery(const bool flag)
:
AosSdocAction(AOSACTTYPE_MODIFYQUERY, AosActionType::eModifyQuery, flag)
{
}


AosActModifyQuery::~AosActModifyQuery()
{
}


bool	
AosActModifyQuery::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//format <action zky_type="modifyquery" arith="+|-|*|/" valuefrom="valuesel|sourcequery" source_attrname="" dest_attrname="">
	//			<destquery>
	//				<cmd pagesize="20">
	//					<conds>
	//						<cond .../>
	//						<cond .../>
	//						...
	//					</conds>
	//					<fnames>
	//						<fname>
	//							<oname>xxxx</oname>
	//							<cname>xxxx</cname>
	//							...
	//						</fname>
	//					</fnames>
	//				</cmd>
	//			</destquery>
	//
	//			<valuesel/>
	//
	//			<sourcequery>
	//				<cmd pagesize="20">
	//					<conds>
	//						<cond .../>
	//						<cond .../>
	//						...
	//					</conds>
	//					<fnames>
	//						<fname>
	//							<oname>xxxx</oname>
	//							<cname>xxxx</cname>
	//							...
	//						</fname>
	//					</fnames>
	//				</cmd>
	//			</sourcequery>
	//		 </action>
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	struct modifyquery mq; 
	bool gRSLT = parseConfig(sdoc, mq, rdata);
	aos_assert_r(gRSLT, false);

	int value = getValue(mq, rdata);
	AosXmlTagPtr destdoc = getQueryRslt(mq.dest_query_config, rdata);
	aos_assert_r(destdoc, false);

	gRSLT = modifyDoc(mq, destdoc, value, rdata);
	aos_assert_r(gRSLT, false);

	rdata->setResults(destdoc->toString());
	rdata->setOk();
	return true;
}

bool
AosActModifyQuery::suntractFunc(AosXmlTagPtr &dest, const OmnString &attrname, const int &value, const AosRundataPtr &rdata)
{
	AosXmlTagPtr tmp = dest;
	if (dest->isRootTag()) tmp = dest->getFirstChild();
	AosXmlTagPtr child = tmp->getFirstChild();	
	while(child)
	{
		//attribute maybe none
		OmnString s = child->getAttrStr(attrname, "");
		aos_assert_r(s != "", false);
		int v = atoi(s.data()) - value;
		s = "";
		s << v;
		child->setAttr(attrname, s);
		child = tmp->getNextChild();
	}
	return true;
}

bool
AosActModifyQuery::modifyDoc(const struct modifyquery &mq, AosXmlTagPtr &dest, const int &value, const AosRundataPtr &rdata)
{

	OmnString opr = mq.arith;
	switch (toEnum(opr))
	{
	case eSubtract :	return suntractFunc(dest, mq.dest_attrname, value, rdata);
	default : return false;
	}

	return true;
}

AosXmlTagPtr
AosActModifyQuery::getQueryRslt(const AosXmlTagPtr &xml, const AosRundataPtr &rdata)
{
	int total = 1;
	int start_idx = 0;
	OmnString str = "<Contents/>";
	AosXmlParser parser1;
	AosXmlTagPtr doc = parser1.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, NULL);
	AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(xml, rdata);
	aos_assert_rr(query && query->isGood(), rdata, NULL);
	while(start_idx < total)
	{
		//query start

		OmnString contents;
		bool rslt = query->procPublic(xml, contents, rdata);
		aos_assert_rr(rslt, rdata , NULL);
		AosXmlParser pageparse;
		AosXmlTagPtr page = pageparse.parse(contents, "" AosMemoryCheckerArgs);
		aos_assert_rr(page, rdata, NULL);
		if (page->isRootTag()) page= page->getFirstChild();
		AosXmlTagPtr pagechild = page->getFirstChild();	
		total = page->getAttrInt("total", 0);	
		while(pagechild)
		{
			doc->addNode(pagechild);
			pagechild = page->getNextChild();
		}
		start_idx += xml->getAttrInt("psize", 0);
		query->setStartIdx(start_idx);
		//query end 
	}
	return doc;
}

int
AosActModifyQuery::getValue(const struct modifyquery &mq, const AosRundataPtr &rdata)
{
	if(mq.valuefrom == "sourcequery")
	{
		int sum = 0;
		int num = 0;
		OmnString attrname = mq.source_attrname;
		aos_assert_rr(attrname != "", rdata, false);
		AosXmlTagPtr xml = mq.config->getFirstChild("sourcequery");
		aos_assert_rr(xml, rdata, 0);

		AosXmlTagPtr doc = getQueryRslt(xml, rdata);
		aos_assert_rr(doc, rdata, 0);
		if (doc->isRootTag()) doc = doc->getFirstChild();
		AosXmlTagPtr child = doc->getFirstChild();	
		while(child)
		{
			sum += child->getAttrInt(attrname, 0);
			child = doc->getNextChild();
			num++;
		}
		aos_assert_rr(num > 0, rdata, 0);
		return sum/num;
	}
	aos_assert_rr(mq.valuesel, rdata, 0);
	AosValueRslt valueRslt;
	AosValueSelObj::getValueStatic(valueRslt, mq.valuesel, rdata);
	if (!valueRslt.isNull())
	{
		AosSetError(rdata, AosErrmsgId::eFailedGetValue);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return valueRslt.getI64();

}

bool	
AosActModifyQuery::parseConfig(const AosXmlTagPtr &sdoc, struct modifyquery &mq, const AosRundataPtr &rdata)
{
	OmnString arith  = sdoc->getAttrStr("arith", "-");
	aos_assert_rr(arith != "", rdata, false);
	OmnString valuefrom = sdoc->getAttrStr("valuefrom", "sourcequery");
	aos_assert_rr(valuefrom != "", rdata, false);
	OmnString source_attrname = sdoc->getAttrStr("source_attrname", "");
	AosXmlTagPtr dest_query_config = sdoc->getFirstChild("destquery");
	aos_assert_rr(dest_query_config, rdata, false);
	OmnString dest_attrname = sdoc->getAttrStr("dest_attrname", "");
	aos_assert_rr(dest_attrname != "", rdata, false);
	AosXmlTagPtr valuesel = sdoc->getFirstChild("valuesel");

	mq.config = sdoc;
	mq.valuesel = valuesel;
	mq.arith = arith;
	mq.valuefrom = valuefrom;
	mq.dest_attrname = dest_attrname;
	mq.source_attrname = source_attrname;
	mq.dest_query_config = dest_query_config;
	return true;
}

AosActionObjPtr
AosActModifyQuery::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActModifyQuery(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
