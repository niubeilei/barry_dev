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
#include "Query/TermGroup.h"

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
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/BuffArray.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"


AosTermGroup::AosTermGroup(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_GROUP, AosQueryType::eGroup, regflag),
mPsize(eDftPsize),
mStartIdx(0),
mCrtIdx(0),
mDocsNum(0),
mGroupKeyBegin(0),
mGroupKeyEnd(0),
mStep(0),
mDataLoaded(false),
mIsShortfall(false),
mIsSort(true)
{
}


AosTermGroup::AosTermGroup(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_GROUP, AosQueryType::eGroup, false),
mPsize(eDftPsize),
mStartIdx(0),
mCrtIdx(0),
mDocsNum(0),
mGroupKeyBegin(0),
mGroupKeyEnd(0),
mStep(0),
mDataLoaded(false),
mIsShortfall(false),
mIsSort(true)
{
	mIsGood = parse(def, rdata);
}


AosTermGroup::~AosTermGroup()
{
}


bool 	
AosTermGroup::nextDocid(
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

	if (mCrtIdx >= (i64)mDocMap.size())
	{
		finished = true;
		return true;
	}
	docid = ++mCrtIdx;
	return true;
}


bool
AosTermGroup::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermGroup::getDocidsFromIIL(
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
AosTermGroup::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
{
	aos_assert_r(docid > 0, 0);
	u64 tempdocid = docid - 1;
	u64 tmp = 0;
	if (!mReverseOrder)
	{
		map<OmnString, vector<AosXmlTagPtr> >::iterator it;
		for (it = mDocMap.begin(); it != mDocMap.end(); ++it)
		{
			if (tmp == tempdocid)
			{
				return it->second[0];
			}
			tmp++;
		}
	}
	else
	{
		map<OmnString, vector<AosXmlTagPtr> >::reverse_iterator rit;
		for (rit = mDocMap.rbegin(); rit != mDocMap.rend(); ++rit)
		{
			if (tmp == tempdocid)
			{
				return rit->second[0];
			}
			tmp++;
		}
	}
	return 0;
}


bool 	
AosTermGroup::moveTo(const i64 &pos, const AosRundataPtr &rdata)
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
AosTermGroup::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	return mDocsNum;
}


void	
AosTermGroup::reset(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids, and a position pointer that
	// is used to indicate which element the term is currently at. 
	// This function resets the position pointer to the beginning of the array.
	// If this is what you need (refer to AosQueryTerm::reset()), do not override
	// this member function.
}

void
AosTermGroup::toString(OmnString &str)
{
	// This function converts the term into an XML.
}


bool
AosTermGroup::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;

	std::map<OmnString, vector<AosXmlTagPtr> > ::iterator it;

	u64 docsSize = 0;
	u64 docid; 
	i64 tmpsize = 0;
	bool finished = false;
	AosXmlTagPtr xml;
	AosXmlTagPtr xmltemp;
	OmnString key;

	while (mOrTerm->nextDocid(0, docid, finished, rdata))
	{
		if(docid == AOS_INVDID && finished) break;

		xml = mOrTerm->getDoc(docid, rdata);
		if (!xml)
		{
			OmnAlarm << "Failed to retrieve the doc: " << docid << enderr;
			continue;
		}

		docsSize += xml->getDocsize();

		if (docsSize > eMapContentMax)
		{
			OmnAlarm << "TermGroup's map is to big! map size : " << docsSize<< enderr;
			break;
		}
		tmpsize++;

		bool exist;
		if (mGroupKeyType == "number")
		{
			int val = (xml->xpathQuery(mGroupCond, exist, "noAttr")).toInt(); 
			char buff[100];
			sprintf(buff, "%100d", val);
			key = buff;
		}
		else
		{
			key = xml->xpathQuery(mGroupCond, exist, "noAttr");
		}

		if (!exist) continue;

		it = mDocMap.find(key);
		if(it == mDocMap.end())
		{
			mKeyGroup.push_back(key);
			vector<AosXmlTagPtr> xmls;
			xmls.push_back(xml);
			mDocMap.insert(make_pair(key, xmls));
		}
		else
		{
			if (mOpr == eFirst) continue;
			if (mOpr == eLast)
			{
				it->second[0] = xml;
				continue;
			}
			it->second.push_back(xml);
		}

		if (mIsSort)
		{
			if (mDocMap.size() > (mPsize + mStartIdx)) break;
		}
	}

	bool rslt = doGroup(rdata, tmpsize);
	aos_assert_r(rslt, false);

	if(rslt) mDataLoaded = true;

	return true;
}


bool
AosTermGroup::doGroup(
		const AosRundataPtr &rdata,
		const i64 &tmpsize)
{
	bool rslt = false;
	switch(mOpr)
	{
	case eSum      : rslt = getSum(rdata);		break;
	case eMinus    : rslt = getMinus(rdata);	break;
	case eGroupAll : rslt = getGroupAll(rdata);	break;
	default : rslt = getFirst(rdata);			break;
	}

	i64 total = mOrTerm->getTotal(rdata);
	if (total > 0 && tmpsize >= (i64)mDocMap.size())
	{
		mDocsNum = i64( total / (tmpsize / (float)mDocMap.size()) + 0.5 );
	}
	else
	{
		mDocsNum = mDocMap.size();
	}

	if (mGroupKeyType != "" && mIsShortfall)
	{	
		shortfallRecord(rdata, mGroupCond, mDocMap);
	}

	return rslt;
}


bool
AosTermGroup::shortfallRecord2(const AosRundataPtr &rdata)
{
	map<OmnString, vector<AosXmlTagPtr> >::iterator it;
	aos_assert_r(mGroupAllType != "", false);
	OmnString key;

	for (it = mDocMap.begin(); it != mDocMap.end(); it++)
	{
		AosXmlTagPtr xml = it->second[0];
		map<OmnString, vector<AosXmlTagPtr> > docsmap;
		map<OmnString, vector<AosXmlTagPtr> >::iterator itr;
		AosXmlTagPtr child_xml = xml->getFirstChild(true);
		while(child_xml)
		{
			key = child_xml->getAttrStr(mGroupAllType, "noAttr");
			vector<AosXmlTagPtr> xmls;
			AosXmlTagPtr new_xml = child_xml->clone(AosMemoryCheckerArgsBegin);
			xmls.push_back(new_xml);
			docsmap[key] = xmls;
			child_xml = xml->getNextChild();
		}
		shortfallRecord(rdata, mGroupAllType, docsmap);

		xml->removeNode("record", true, true);

		for (itr = docsmap.begin(); itr != docsmap.end(); itr++)
		{
			xml->addNode(itr->second[0]);
		}
	}
	return true;
}


bool
AosTermGroup::shortfallRecord(
		const AosRundataPtr &rdata,
		const OmnString &groupCond,
		map<OmnString, vector<AosXmlTagPtr> > &docsmap)
{
	for (int i = mGroupKeyBegin; i <= mGroupKeyEnd; i = i + mStep)
	{
		OmnString strkey;
		char buff[100];
		OmnString str_xml = "<record />";
		AosXmlTagPtr record = AosStr2Xml(rdata.getPtr(), str_xml AosMemoryCheckerArgs);
		if (mGroupKeyType != "str")
		{
			sprintf(buff, "%100d", i); 
			strkey << buff;
			if (docsmap[strkey].size() > 0)
			{
				continue;
			}
			record->setAttr(mGroupValue, mDefValue);
			record->setAttr(groupCond, i);
		}
		if (mGroupKeyType == "str")
		{
			strkey << mStrValues[i];
			if (mDocMap[mStrValues[i]].size() > 0) continue;
			record->setAttr(mGroupValue, mDefValue);
			record->setAttr(groupCond, mStrValues[i]);
		}
		if (strkey == "") continue;
		vector<AosXmlTagPtr> xmls;
		xmls.push_back(record);
		docsmap[strkey] = xmls;

//OmnScreen << "TermGroup vaector size is : " << xmls.size()<< endl;
//OmnScreen << "TermGroup key  : " << strkey  << endl;
//OmnScreen << "TermGroup value : " << mDocMap[strkey].size() <<endl;
	}
	return true;
}

bool
AosTermGroup::getFirst(const AosRundataPtr &rdata)
{
	std::map<OmnString, vector<AosXmlTagPtr> > ::iterator it;
	for(u32 i = 0; i < mKeyGroup.size(); i++)
	{
		it = mDocMap.find(mKeyGroup[i]);
		aos_assert_r(it != mDocMap.end(), false);
		aos_assert_r(it->second.size() > 0, false);
		AosXmlTagPtr doc = it->second[0]->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(doc, false);
		it->second.clear();
		it->second.push_back(doc);
	}
	return true;
}


bool
AosTermGroup::getGroupAll(const AosRundataPtr &rdata)
{
	OmnString str_xml = "<record />";
	AosXmlTagPtr record = AosStr2Xml(rdata.getPtr(), str_xml AosMemoryCheckerArgs); 
	OmnString value;
	bool exist;

	std::map<OmnString, vector<AosXmlTagPtr> >::iterator it;
	for (u32 i = 0; i < mKeyGroup.size(); i++)
	{
		it = mDocMap.find(mKeyGroup[i]);
		aos_assert_r(it != mDocMap.end(), false);
		aos_assert_r(it->second.size() > 0, false);
		AosXmlTagPtr newRecord;
		for (u32 j = 0; j < it->second.size(); j++)
		{
			AosXmlTagPtr newRecord2;
			newRecord = record->clone(AosMemoryCheckerArgsBegin);
			OmnString vv = it->second[0]->getAttrStr(mGroupValue);
			aos_assert_r(vv != "", false);
			newRecord->setAttr(mGroupValue, vv);
			aos_assert_r(newRecord, false);

			AosXmlTagPtr doc = it->second[j];
			for (u32 k = 0; k < mKeyPathFrom.size(); k++)
			{
				newRecord2 = record->clone(AosMemoryCheckerArgsBegin);
				value = doc->xpathQuery(mKeyPathFrom[k], exist, "");
				newRecord2->setAttr(mKeyPathTo[k], value);
			}
			newRecord->addNode(newRecord2);
		}
		it->second.clear();
		it->second.push_back(newRecord);
	}
	shortfallRecord2(rdata);
	return true;

}

bool
AosTermGroup::getLast(const AosRundataPtr &rdata)
{
	std::map<OmnString, vector<AosXmlTagPtr> > ::iterator it;
	for(u32 i = 0; i < mKeyGroup.size(); i++)
	{
		it = mDocMap.find(mKeyGroup[i]);
		aos_assert_r(it != mDocMap.end(), false);
		aos_assert_r(it->second.size() > 0, false);

		u32 idx = it->second.size() - 1;
		AosXmlTagPtr doc = it->second[idx]->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(doc, false);

		it->second.clear();
		it->second.push_back(doc);
	}
	return true;
}


bool
AosTermGroup::getSum(const AosRundataPtr &rdata)
{
	aos_assert_r(mGroupValue != "", false);
	std::map<OmnString, vector<AosXmlTagPtr> > ::iterator it;
	for(u32 i = 0; i < mKeyGroup.size(); i++)
	{
		it = mDocMap.find(mKeyGroup[i]);
		aos_assert_r(it != mDocMap.end(), false);
		aos_assert_r(it->second.size() > 0, false);

		AosXmlTagPtr doc = it->second[0]->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(doc, false);

		i64 sum = 0;
		for (u32 j = 0; j < it->second.size(); j++)
		{
			sum += it->second[j]->getAttrInt64(mGroupValue, 0);
		}

		doc->setAttr(mGroupValue, sum);
		it->second.clear();
		it->second.push_back(doc);
	}
	return true;
}


bool
AosTermGroup::getMinus(const AosRundataPtr &rdata)
{
	return true;
}

bool
AosTermGroup::parse(const AosXmlTagPtr &conds, const AosRundataPtr &rdata)
{
	// 'conds' should be in the form:
	//  <conds	zky_group_type="xxxx" zky_group_begin="" 
	//  		zky_group_end="" zky_group_value_type="number||str"
	//  		zky_is_sort="true||false" zky_group_value="xxxx"
	//  		zky_strvalues="xxxxx,xxxxxxx,xxxx," 
	//  		zky_group_cond="xxxx"
	//  		zky_sep=","
	//			zky_groupall_type=""		
	//  		zky_defvalue="xx"
	//  		zky_keyPathFrom="xxxxx,xxxx,xxx"
	//  		zky_keyPathTo="xxxxx,xxxx,xxx">
	//      <cond type="AND">
	//          <term type="xxx" .../>
	//          <term type="xxx" .../>
	//          ...
	//          <term type="xxx" .../>
	//      </cond>
	//      <cond type="AND">
	//          <term type="xxx" .../>
	//          <term type="xxx" .../>
	//          ...
	//          <term type="xxx" .../>
	//      </cond>
	//  </conds>
	//

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

	mGroupKeyType = conds->getAttrStr("zky_group_value_type", "str");
	mGroupAllType = conds->getAttrStr("zky_groupall_type", "");
	if(mIsShortfall)
	{
		if (mGroupKeyType == "number")
		{
			mGroupKeyBegin = conds->getAttrInt64("zky_group_begin", 0);
			mGroupKeyEnd = conds->getAttrInt64("zky_group_end", 0);
		}

		mSep = conds->getAttrStr("zky_sep", ",");		
		if (mGroupKeyType == "str")
		{
			OmnString strvalues = conds->getAttrStr("zky_strvalues", "");
			aos_assert_r(strvalues != "", false);
			AosStrSplit split(strvalues, mSep.data());    
			mStrValues = split.entriesV();
			mGroupKeyBegin = 0; 
			mGroupKeyEnd = mStrValues.size() - 1; 
		}

		OmnString keyPathFrom = conds->getAttrStr("zky_keyPathFrom", "");
		aos_assert_r(keyPathFrom != "", false);
		OmnString keyPathTo = conds->getAttrStr("zky_keyPathTo", "");
		aos_assert_r(keyPathTo != "", false);
		AosStrSplit split(keyPathFrom, mSep.data());    
	
		mKeyPathFrom = split.entriesV();
		split.splitStr(keyPathTo.data(), mSep.data());
		mKeyPathTo = split.entriesV();
		mGroupValue = conds->getAttrStr("zky_group_value", ""); 
		aos_assert_r(mGroupValue != "", false);
	}

	
	mDefValue = conds->getAttrStr("zky_defvalue", "");
	mStep = conds->getAttrInt64("zky_step", 1);

	mIsSort = conds->getAttrBool("zky_is_sort", true);
	OmnString group_type = conds->getAttrStr("zky_group_type", ""); 
	mOpr = toEnum(group_type); 
	mGroupCond = conds->getAttrStr("zky_group_cond", ""); 
	aos_assert_r(mGroupCond != "", false);

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

		//if (andterm->getTagname() == group_conds->getTagname())
		//	andterm = 0;

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
	mOrTerm->setStartIdx(mStartIdx);
	
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
AosTermGroup::processStatistics(
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
AosTermGroup::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermGroup(def, rdata);
}


AosTermGroup::Opr                               
AosTermGroup::toEnum(const OmnString &opr)
{
	if (opr == "sum") return eSum;
	if (opr == "first") return eFirst;
	if (opr == "last") return eLast;
	if (opr == "minus") return eMinus;
	if (opr == "groupall") return eGroupAll;
	return eInvalid;
}

