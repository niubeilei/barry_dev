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
#include "ValueSel/ValueSelQuery.h"

#include "API/AosApiD.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Util/OmnNew.h"
#include "Util/Opr.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

#define eMaxConds		10

#if 0
AosValueSelQuery::AosValueSelQuery(const bool reg)
:
AosValueSel(AOSACTOPRID_QUERY, AosValueSelType::eQuery, reg)
{
}


AosValueSelQuery::AosValueSelQuery(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_QUERY, AosValueSelType::eQuery, false)
{
}


AosValueSelQuery::~AosValueSelQuery()
{
}


bool
AosValueSelQuery::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	// item should be this format:
	// <item type="xxx">
	// 		<cond>
	// 			<ctnr_objid />
	// 			<attr />
	// 			<opr />
	// 			<reverse />
	// 			<order />
	// 		</cond>
	// 		<cond>
	// 			<ctnr_objid />
	// 			<attr />
	// 			<opr />
	// 			<reverse />
	// 			<order />
	// 		</cond>
	//      ......
	// </item>
	/////////////////////////////////////////////////
	OmnString objids[eMaxConds];
	OmnString attrs[eMaxConds];
	OmnString values[eMaxConds];
	AosOpr oprs[eMaxConds];
	bool reverses[eMaxConds];
	bool orders[eMaxConds];
	AosValueRslt valueRslts[eMaxConds];	

	int psize = item->getAttrInt(AOSTAG_PAGESIZE, eDftPageSize);
	if (psize <= 0) psize = eDftPageSize;

	AosQueryCond qcond;
	qcond.reset();
	vector <AosQueryCond> qvect;	
	if (item->getNumSubtags() >= eMaxConds)
	{
		rdata->setError() << "The conditions is more than defined";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosXmlTagPtr cond;
	if (item->isRootTag())
	{
		cond = item->getFirstChild()->getFirstChild();
	}
	else
	{
		cond = item->getFirstChild();
	}
	while (cond)
	{
		// Get container objids
		OmnString coid;
		AosXmlTagPtr coidtag = cond->getFirstChild(AOSTAG_CTNR_OBJID);
		if (coidtag) 
		{
			aos_assert_r(AosValueSel::getValueStatic(valueRslt, coidtag, rdata), false);
			bool rslt;
			coid = valueRslt.getStr();
			if (rslt)
			{
				qcond.mCtnrObjid = coid; 
			}
			else
			{
				qcond.mCtnrObjid = ""; 
			}
		}
		else
		{
			qcond.mCtnrObjid = ""; 
		}

		// Get attributes
		OmnString attr;
		AosXmlTagPtr attrtag = cond->getFirstChild(AOSTAG_ATTR_NAME);
		if (attrtag) 
		{
			aos_assert_r(AosValueSel::getValueStatic(valueRslt, attrtag, rdata), false);
			bool rslt;
			attr = valueRslt.getStr();
			if (rslt)
			{
				if (attr == "") 
				{
					cond = item->getNextChild();
					continue;
				}
				qcond.mAttrName = attr;
			}
			else
			{
				cond = item->getNextChild();
				continue;
			}
		}
		else
		{
			cond = item->getNextChild();
			continue;
		}
		
		// Get the value of attribute
		OmnString value;
		AosXmlTagPtr valuetag = cond->getFirstChild(AOSTAG_VALUE);
		if (valuetag) 
		{
			aos_assert_r(AosValueSel::getValueStatic(valueRslt, valuetag, rdata), false);
			bool rslt;
			value = valueRslt.getStr();
			if (rslt)
			{
				if (value == "") 
				{
					cond = item->getNextChild();
					continue;
				}
				qcond.mValue = value;
			}
			else
			{
				cond = item->getNextChild();
				continue;
			}
		}
		else
		{
			cond = item->getNextChild();
			continue;
		}
		
		// Get All opr
		OmnString opr;
		AosXmlTagPtr oprtag = cond->getFirstChild(AOSTAG_OPR);
		if (oprtag) 
		{
			aos_assert_r(AosValueSel::getValueStatic(valueRslt, oprtag, rdata), false);
			
			bool rslt;
			opr = valueRslt.getStr();
			if (rslt)
			{
				qcond.mOpr = AosOpr_toEnum(opr);
			}
			else
			{
				qcond.mOpr = eAosOpr_eq;
			}
		}
		else
		{
			qcond.mOpr = eAosOpr_eq;
		}
		
		AosXmlTagPtr rvstag = cond->getFirstChild(AOSTAG_REVERSE);
		if (rvstag) 
		{
			qcond.mReverse = (rvstag->getNodeText() == "true" ? true : false);
		}
		else
		{
			qcond.mReverse = false;
		}
		
		AosXmlTagPtr odrtag = cond->getFirstChild(AOSTAG_ORDER);
		if (odrtag) 
		{
			qcond.mOrder = (odrtag->getNodeText() == "true" ? true : false);
		}
		else
		{
			qcond.mOrder = false;
		}
		qvect.push_back(qcond);
		cond = item->getNextChild();
	}

	for (int i=0; i<(int)qvect.size(); i++)
	{
		objids[i] = qvect[i].mCtnrObjid;
		attrs[i] = qvect[i].mAttrName;
		values[i] = qvect[i].mValue;
		valueRslts[i] = valueRslt;          
		valueRslts[i].setStr(values[i]);
		oprs[i] = qvect[i].mOpr;
		reverses[i] = qvect[i].mReverse;
		orders[i] = qvect[i].mOrder;
	}

	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	aos_assert_r(queryRslt, false);

	bool rslt = AosDbRunQuery(
			-10,psize,
			objids,attrs, valueRslts, oprs, reverses, 
			orders, (int)qvect.size(), queryRslt, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eQueryFailed);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the flag to set one doc or all docs.
	bool onedoc = item->getAttrBool("zky_getonedoc", false);
	if (onedoc)
	{
		AosXmlTagPtr doc = 0;
		OmnString xpath = item->getAttrStr(AOSTAG_XPATH, "");
		bool rslt;
		u64 docid = queryRslt->nextDocid(rslt);
		if (docid == 0)
		{
			if (xpath != "")
			{
				valueRslt.setStr("");
				return true;
			}
			else
			{
				valueRslt.setXml(doc);
				valueRslt.setDataType(AOSDATATYPE_XMLDOC);
				return true;
			}
		}
		
		doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (!doc)
		{
			rdata->setError() << "Inter Error";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (xpath != "")
		{
			OmnString value = doc->getAttrStr(xpath);
			valueRslt.setStr(value);
			return true;
		}
		else
		{
			valueRslt.setXml(doc);
			return true;
		}
	}
	valueRslt.setQueryRslt(queryRslt);
	return true;
}


OmnString 
AosValueSelQuery::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelQuery::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelQuery(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}






#endif
