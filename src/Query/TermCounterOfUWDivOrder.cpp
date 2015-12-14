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
// This is a Term Template. When creating a new term, one should copy
// this file and modify it. 
//
// Modification History:
// 2013/08/12  Andy Zhang     
////////////////////////////////////////////////////////////////////////////
#include "Query/TermCounterOfUWDivOrder.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterClt/CounterClt.h"
#include "CounterUtil/CounterQuery.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "SEUtil/IILName.h"
#include "Query/TermIILType.h"
#include "Query/TermOr.h"
#include "Query/TermAnd.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/BuffArray.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"


AosTermCounterOfUWDivOrder::AosTermCounterOfUWDivOrder(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_COUNTEROFUWDIVORDER, AosQueryType::eCounterOfUWDivOrder, regflag)
{
}


AosTermCounterOfUWDivOrder::AosTermCounterOfUWDivOrder(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_FILTER, AosQueryType::eFilter, false)
{
	mIsGood = parse(def, rdata);
}


AosTermCounterOfUWDivOrder::~AosTermCounterOfUWDivOrder()
{
}


bool 	
AosTermCounterOfUWDivOrder::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the next docid that is selected by this term.
	// What it does is to retrieve the data based on the condition, and then 
	// returns the next docid. Normally you do not need to override this function. 
	// Refer to AosQueryTerm::nextDocid(...) to see whether it is the one you
	// need. If yes, do not override it. Remove this function. 
	
	docid = 0;
	finished = false;
	if (!mDataLoaded) loadData(rdata);

	if (mCrtIdx >= (i64)mDocs.size())
	{
		finished = true;
		return true;
	}
	docid = ++mCrtIdx;
	return true;
}


bool
AosTermCounterOfUWDivOrder::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermCounterOfUWDivOrder::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	if (query_rslt) 
	{
		query_rslt->appendDocid(0);
		query_rslt->reset();
	}
	if(query_context)
	{
		query_context->setFinished(true);
	}
	return true;
}


AosXmlTagPtr
AosTermCounterOfUWDivOrder::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
{
	aos_assert_r(docid > 0, 0);
	u64 tempdocid; 
	if (!mReverseOrder)
	{
		tempdocid = docid - 1;
	}
	else
	{
		tempdocid = mDocs.size() - docid;
	}
	aos_assert_r(tempdocid >= 0 && tempdocid < mDocs.size(), 0);
	return mDocs[tempdocid];
}


bool 	
AosTermCounterOfUWDivOrder::moveTo(const i64 &pos, const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function moves the position
	// to 'pos' in the array. If pos is bigger than the size of the array, 
	// it returns false. Otherwise, it loads the data as needed, and changes
	// the position to 'pos'. 
	//
	// This is the default implementation (check AosQueryTerm::moveTo(pos, rdata).
	// If this is what you need, do not override this function.
	
	mCrtIdx = pos;

	return true;
}


i64		
AosTermCounterOfUWDivOrder::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	return mDocs.size();
}


void	
AosTermCounterOfUWDivOrder::reset(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids, and a position pointer that
	// is used to indicate which element the term is currently at. 
	// This function resets the position pointer to the beginning of the array.
	// If this is what you need (refer to AosQueryTerm::reset()), do not override
	// this member function.
}

void
AosTermCounterOfUWDivOrder::toString(OmnString &str)
{
	// This function converts the term into an XML.
}


bool
AosTermCounterOfUWDivOrder::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;
	mDocsMap.clear();
	bool rslt = true;
	bool finished;
	mDataLoaded = false;
	u64 docid = 0;
	AosXmlTagPtr xml;
	OmnString str;
	double lvalue, rvalue, value; 
	while (rslt)
	{
		rslt = false;
		if (mOrTerm->withValue())
		{
			rslt = mOrTerm->nextDocid(0, docid, str, finished, rdata);
		}
		else
		{
			rslt = mOrTerm->nextDocid(0, docid, finished, rdata);
		}

		if (!rslt) break;
		if(docid == AOS_INVDID && finished) break;

		if(mOrTerm->withValue())
		{
			xml = mOrTerm->getDoc(docid, str, rdata);
		}
		else
		{
			xml = mOrTerm->getDoc(docid, rdata);
		}
		if (!xml)
		{
			OmnAlarm << "Failed to retrieve the doc: " << docid << enderr;
			continue;
		}

		AosXmlTagPtr new_xml = xml->clone(AosMemoryCheckerArgsBegin);
		lvalue = new_xml->getAttrDouble(mLName, 0);
		rvalue = new_xml->getAttrDouble(mRName, 0);
		aos_assert_r(rvalue, false);
		switch(mOpr)
		{
		case eAdd:
			value = lvalue+rvalue;
			break;
		case eMinus:
			value = lvalue-rvalue;
			break;
		case eDivide:
			value = lvalue/rvalue;
			break;
		case eMultiply:
			value = lvalue*rvalue;
			break;
		default :
			break;
		}

		i64 fvalue = 1;
		for (i64 i = 0; i < mDecimals; i++)
		{
			fvalue = fvalue * 10;
		}
		
		char buff[100];
		OmnString str;
		str << "%0." << mDecimals << "f";
		double dvalue = double(i64(value * fvalue + 0.5))/fvalue;
		sprintf(buff, str.data(), dvalue);

		new_xml->setAttr(mValueName, buff);
		mDocsMap.insert(make_pair(value, new_xml));
	}

	i64 tmp_num = mPsize + mStartIdx + 10;

	multimap<double, AosXmlTagPtr>::iterator it;
	for (it = mDocsMap.begin(); it != mDocsMap.end(); it++)
	{
		mDocs.push_back(it->second);
		if ((i64)mDocs.size() > tmp_num)
		{
			mDataLoaded = true;
			return true;
		}
	}

//	aos_assert_r(mDocsMap.size() == mDocs.size(), false);
	mDataLoaded = true;
	return true;
}


bool
AosTermCounterOfUWDivOrder::parse(const AosXmlTagPtr &conds, const AosRundataPtr &rdata)
{
	// 'conds' should be in the form:
	// <stat>
	//  <cond type="YWDivOrderBy" zky_lname="",
	//  	  zky_rname=""  zky_opr="+|-|*|/" 
	//  	  zky_decimal=""
	//  	  zky_valuename = ""
	//  	  zky_reverse_order="true|false"  >
	//      <cond type="AND">
	//          <term type="xxx" .../>
	//          <term type="xxx" .../>
	//          ...
	//          <term type="xxx" .../>
	//      </cond>
	//  </cond>
	//  <stat>

	mIsGood = false;
	aos_assert_r(rdata, false);
	aos_assert_rr(conds, rdata, false);

	i64 guard = 20;
	AosXmlTagPtr p = conds;
	while ((p = p->getParentTag()) && guard-- > 0)
	{
		if (p->getAttrStr("start_idx") != "")
		{
			mStartIdx = p->getAttrInt64("start_idx", 0);
			mPsize = p->getAttrInt64("psize", eDftPsize);
			mReverseOrder = p->getAttrBool("reverse", false);
			mOrderContainer = p->getAttrStr("orderctnr");
			mOrderFname = p->getAttrStr("order");
			break;
		}
	}	

	mLName = conds->getAttrStr("zky_lname");
	aos_assert_r(mLName != "", false);

	mValueName = conds->getAttrStr("zky_valuename");
	aos_assert_r(mValueName != "", false);

	mRName = conds->getAttrStr("zky_rname");
	aos_assert_r(mRName != "", false);

	OmnString opr = conds->getAttrStr("zky_opr");
	aos_assert_r(opr != "", false);

	mOpr = toEnum(opr, rdata);
	mDecimals = conds->getAttrInt64("zky_decimal", 0);
	aos_assert_r(mDecimals, false);

	mReverseOrder = conds->getAttrBool("zky_reverse_order", false);

	AosQueryTermObjPtr cond;

	// 1. Handle all containers. Containers are handled by 'AosTermTag'
	// with the IIL name: ??? + ":" + container name
	OmnString word;

	if (!conds) return false;

	// All other conditions are defined by 'term'. 
	AosXmlTagPtr termtmp;
	guard = 20;
	AosXmlTagPtr andterm = conds->getFirstChild();
	while (guard-- && andterm) 
	{
		OmnString type = andterm->getAttrStr("type");
		if (andterm->getTagname() == AOSTAG_QUERY_STAT)
        {
            // This is a statistics tag
			if (!cond) cond = OmnNew AosTermAnd();
            processStatistics(cond, andterm, rdata);
            andterm = conds->getNextChild();
            continue;
        }

		if (type != "AND")
		{
			rdata->setError() << "Expecting the AND term but failed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (!cond) cond = OmnNew AosTermAnd();
		AosXmlTagPtr term = andterm->getFirstChild();
		termtmp = term;

		while (term)
		{
			//jackie 04/19/2013 start
			OmnString t_type = term->getAttrStr("type");
			if(t_type == "batchquery")
			{
				term->setAttr("startidx", mStartIdx);
				term->setAttr("size", mPsize);
			}
			//jackie 04/19/2013 end
			
			if (rdata->getCid()!= "")
			{
				AosXmlTagPtr objcond = term;
				bool exist;
				OmnString objidlhs = objcond->xpathQuery("lhs/_$text", exist, "");
				if (objidlhs == AOSTAG_OBJID)
				{
					OmnString objidrhs = objcond->xpathQuery("rhs/_$text", exist, "");
					OmnString modobjid  = objidrhs;
					OmnString errmsg;
					bool rslt = AosObjid::procObjid(modobjid, rdata->getCid(), errmsg);
					if (!rslt)
					{
						AosSetError(rdata, errmsg);
					}
					else if (modobjid != objidrhs)
					{
						objcond->setNodeText("rhs", modobjid, true);
					}
				}
			}
			cond->addTerm(term, rdata);
			term = andterm->getNextChild();
		}
		andterm = conds->getNextChild();
	}

	// Check containers. It is possible to 
	if (cond)
	{
		if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
		mOrTerm->addTerm(cond, rdata);
	}

	if (mOrderFname != "" && mOrTerm) 
	{
		mOrTerm->setOrder(mOrderContainer, mOrderFname, mReverseOrder, rdata);
	}

	// Backward Compatibility, Chen Ding, 09/20/2011
	aos_assert_r(mOrTerm, false);
	mOrTerm->setPagesize(mPsize);
	//mOrTerm->setExpectSize(mStartIdx + mPsize);
	
	mOrTerm->loadData(rdata);
	mTotal = mOrTerm->getTotal(rdata);
	mOrTerm->reset(rdata);

	OmnString str;
	if (mOrTerm) mOrTerm->toString(str);
	OmnScreen << "Query: " << str << endl;
	rdata->setOk();

	if (!mDataLoaded) loadData(rdata);

	mIsGood = true;
	return true;
}


bool
AosTermCounterOfUWDivOrder::processStatistics(
		const AosQueryTermObjPtr &cond,
		const AosXmlTagPtr &stat_tag, 
		const AosRundataPtr &rdata)
{
	// This function processes the <stat> tag:
	// 	<conds>
	// 		<cond ...>
	// 			<term .../>
	// 			<term .../>
	//			...
	// 			<term .../>
	// 		</cond>
	// 		<cond .../>
	// 		<stat> 
	// 		 <statterm type="couterSingle||counterMulti||counterSub">
	// 			<zky_ctnr>xxxx</zky_ctnr>
	// 	        <zky_ctype>xxxx</zky_ctype>
	// 	        <zky_starttime>xxxx</zky_starttime>
	// 	        <zky_endtime>xxxx</zky_endtime>
	// 	        <zky_cname>
	// 	           <termname zky_value_type="attr"  zky_xpath="city">
	// 	                <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 	           </termname>
	// 	  	       <termname zky_value_type="const">xxx</termname>
	// 	             ....
	// 	        </zky_cname>
	// 	      </statterm>
	// 		</stat>
	// 	</conds>
	
	aos_assert_rr(stat_tag, rdata, false);
	AosXmlTagPtr statTerm = stat_tag->getFirstChild();
	if(!statTerm)return true;

	AosQueryType::E type = AosQueryType::toEnum(statTerm->getAttrStr("type", ""));
	if (!AosQueryType::isValidCounterType(type))
	{
		rdata->setError() << "Unrecognized statistics type: " << statTerm->getAttrStr("type", "");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (type == AosQueryType::eCounterMulti)
	{
		if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
		bool rslt = mOrTerm->addStatTerm(statTerm, rdata);
		aos_assert_rr(rslt, rdata, false);
		rdata->setOk();
		return true;
	}

	AosQueryTermObjPtr term = cond->addTerm(statTerm, rdata);
	aos_assert_r(term, false);

	if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
	cond->setNeedOrder(false);           
	mOrTerm = OmnNew AosTermOr();
	mOrTerm->addTerm(cond, rdata);
	mOrTerm->setDocRetriever(term);
	mOrTerm->loadData(rdata);
	return true;
}


AosQueryTermObjPtr
AosTermCounterOfUWDivOrder::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermCounterOfUWDivOrder(def, rdata);
}


AosTermCounterOfUWDivOrder::Opr 
AosTermCounterOfUWDivOrder::toEnum(
		const OmnString &opr,
		const AosRundataPtr &rdata)
{
	if (opr == "+") return eAdd;
	if (opr == "-") return eMinus;
	if (opr == "*") return eMultiply;
	if (opr == "/") return eDivide;
	return eInvalid;
}

