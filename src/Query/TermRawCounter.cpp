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
// Modification History:
// 06/07/2012 Created by Felicia Peng
////////////////////////////////////////////////////////////////////////////
#include "Query/TermRawCounter.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/ResolveCounterName.h"
#include "SEInterfaces/DocClientObj.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "QueryCond/CondNames.h"
#include "QueryUtil/QrUtil.h"
#include "QueryUtil/QueryFilter.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"


AosTermRawCounter::AosTermRawCounter(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_RAWCOUNTER, AosQueryType::eRawCounter, regflag),
mSaveToFile(true)
{
}

AosTermRawCounter::~AosTermRawCounter()
{
}

AosTermRawCounter::AosTermRawCounter(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_RAWCOUNTER, AosQueryType::eRawCounter, false),
mSaveToFile(true)
{
	// 'def' is in the form:
	// 	<term type="lt|le|eq|ne|gt|ge|an" odrattr="true|false" odrctnr="xxx">
	// 		<lhs><![CDATA[xxx]]></lhs>
	// 		<rhs><![CDATA[xxx]]></rhs>
	//	</term>
	//
	// The new form will be:
	// 	<term type="arith" 
	// 		order="true|false" 
	// 		reverse="true|false"
	// 		AOSTAG_VALUE_DFT_PROC="xxx"
	// 		AOSTAG_CTNR_DFT_PROC="xxx">
	// 		<selector type=AOSIILSELTYPE_ATTR 
	// 			container="xxx" 
	// 			AOSTAG_ANAME="xxx"/>
	// 		<cond .../>
	// 		<AOSTAG_VALUE_DFT_VALUE>default_value</AOSTAG_VALUE_DFT_VALUE>
	// 	</term>
	//
	
	// Chen Ding, 08/23/2011
	// Backward compatibility
	bool ignore_ctnr = false;
	bool ignore_value = false;
	if (!def->getFirstChild(AOSTAG_SELECTOR))
	{
		mFieldname = def->xpathQuery("lhs/_#text");
		mIILName = AosIILName::composeCtnrMemberListing("", mFieldname);
		OmnString opr = def->getAttrStr("type");
		mValue = def->xpathQuery("rhs/_#text");
		if (opr == "eq")
		{
			mOpr = eAosOpr_eq;
		}
		mIsGood = true;
	}
	else
	{
		mFieldname = def->xpathQuery("selector/@aname");
		mIsGood = AosIILSelector::resolveIIL(def, mIILId, rdata);
		if (!mIsGood || !mIILId)
		{
			mIsGood = AosIILSelector::resolveIIL(def, mIILName, rdata);
		}
		if (!mIsGood) return;

		bool exist;
		mOpr = AosOpr_toEnum(def->xpathQuery("cond/@zky_opr", exist, ""));
		mValue = def->xpathQuery("cond/_$text", exist, "");
		OmnString tmpvalue = mValue;
		AosXmlTagPtr varlag = def->xpathGetChild("cond/parm");
		if(mValue == "" && (varlag || (tmpvalue.length() > 8 &&strncmp(tmpvalue.data(), "clipvar/", 8) == 0)))
		{
			mValue = def->xpathQuery("cond/@zky_initvalue", exist, "");
		}
		mCtnrObjid = def->xpathQuery("selector/@container", "");

		OmnString xpath = AOSTAG_CTNR_DFT_VALUE;
		xpath << "/_#text";
		OmnString dftproc = def->getAttrStr(AOSTAG_CTNR_DFT_PROC);
		OmnString dftvalue = def->xpathQuery(xpath);
		AosQrUtil::procQueryVars(mCtnrObjid, dftproc, dftvalue, ignore_ctnr, rdata);

		xpath = AOSTAG_VALUE_DFT_VALUE;
		xpath << "/_#text";
		dftproc = def->getAttrStr(AOSTAG_VALUE_DFT_PROC);
		dftvalue = def->xpathQuery(xpath);
		AosQrUtil::procQueryVars(mValue, dftproc, dftvalue, ignore_value, rdata);
	}

	mQueryRsltOpr = def->getAttrStr("operator", "index");
	mSaveToFile = def->getAttrBool("save_to_file", true);

	AosXmlTagPtr stat_tag = def->getFirstChild("stat");
	if (stat_tag)
	{
		i64 level = 0;
		OmnString str_header = "";
		bool rslt = AosResolveCounterName::resolveCounterName2(
			stat_tag, "counter_header", str_header, level, rdata);
		aos_assert(rslt);

		OmnString cname;
		rslt = AosResolveCounterName::resolveCounterName2(
			stat_tag, AOSTAG_COUNTERNAMES, cname, level, rdata);
		aos_assert(rslt);

		// compose Entry Type
		aos_assert(level >= 0);
		OmnString append_bit_flag = stat_tag->getAttrStr("append_bit", "false");
		if (append_bit_flag == "false")
		{
			append_bit_flag = stat_tag->getNodeText("append_bit");
		}
		i64 append_bit = append_bit_flag == "true" ? 1 : 0; 

		OmnString timegran = stat_tag->getAttrStr(AOSTAG_TIMEGRAN);
		if (timegran == "") timegran = stat_tag->getNodeText(AOSTAG_TIMEGRAN);
		if (timegran == "") timegran = "ntm"; 
		AosTimeGran::E time_gran = AosTimeGran::toEnum(timegran);
		aos_assert(AosTimeGran::isValid(time_gran));

		u16 entry_type = AosCounterUtil::composeTimeEntryType(
			level, append_bit, AosStatType::eSum, time_gran);

		mKey = AosCounterUtil::composeTerm2(
			entry_type, str_header, cname, 0);
	}

	mIsGood = !ignore_value;
}


void
AosTermRawCounter::toString(OmnString &str)
{
	// 	<arith iilname="xxx" reverse="xxx" opr="xxx" value="xxx"/>
	/*OmnString od = (mReverse)?"true":"false";
	str << "<arith iilname=\"" << mIILName
		<< "\" reverse=\"" << od
		<< "\" order=\"" << mOrder
		<< "\" odrattr=\"" << mCtnrObjid
		<< "\" opr=\"" << AosOpr_toStr(mOpr)
		<< "\" value=\"" << mValue << "\"/>";
	*/
}


AosQueryTermObjPtr
AosTermRawCounter::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	return OmnNew AosTermRawCounter(def, rdata);
}


AosXmlTagPtr
AosTermRawCounter::getDoc(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}

	
AosXmlTagPtr
AosTermRawCounter::getDoc(
		const u64 &docid,
		const OmnString &k,
		const AosRundataPtr &rdata)
{
	// It will create an XML doc:
	// 		<record AOSTAG_KEY="xxx" AOSTAG_VALUE="xxx"/>
	
	if (mSaveToFile)
	{
		static AosXmlTagPtr doc;
		if (!doc)
		{
			OmnString docstr = "<record/>";
			doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
			aos_assert_r(doc, 0);
		}

		OmnString str1;
		OmnString key = AosCounterUtil::getAllTerm2(k);
		aos_assert_r(key !="", 0);
		vector<OmnString> str;
		AosCounterUtil::splitTerm2(key, str);
		for (u32 i = 0; i < str.size()-1; i++)
		{
			if (i == 0)
			{
				u64 vv = atoll(str[i].data());
				if (vv > 0)
				{
					vv = vv * 24 * 60 * 60;
					str[i] = AosTimeUtil::EpochToTimeNew(vv, "%Y-%m-%d");
				}
			}
			str1 << str[i] << ",";
		}
		str1 << str[str.size()-1] << ",";
		str1 << docid <<"\n";
		if (!mFile) mFile = createFile(rdata);
		aos_assert_r(mFile, 0);
		mFile->append(str1, str1.length());
		return doc;
	}
	else
	{
		OmnString docstr = "<record";
		OmnString key = AosCounterUtil::getAllTerm2(k);
		aos_assert_r(key !="", 0);
		vector<OmnString> str;
		AosCounterUtil::splitTerm2(key, str);
		for (u32 i = 0; i< str.size(); i++)
		{
			docstr << " " << AOSTAG_CNAME << i << "=\"" << str[i] << "\"";
		}
		docstr << " " << AOSTAG_VALUE << "=\"" << docid << "\"";
		docstr << "/>";
	
		AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
		aos_assert_r(doc, 0);
		return doc;
	}
	return 0;
}


OmnFilePtr
AosTermRawCounter::createFile(const AosRundataPtr &rdata)
{
	u64 userid = rdata->getUserid();
	if (userid == 0)
	{
		userid = 307;
	}
	aos_assert_rr(userid != 0, rdata ,NULL);
	AosUserAcctObjPtr userdoc = AosDocClientObj::getDocClient()->getUserAcct(userid, rdata);
	aos_assert_rr(userdoc, rdata ,NULL);
	OmnString cid = userdoc->getAttrStr(AOSTAG_CLOUDID);

	OmnString zky_rscfn = "query_rowcounter_";
	zky_rscfn << mIILName << "_"
		<< OmnGetTime("YYYYMMDDhhmmss") << "_"
		<< (unsigned int)OmnGetCurrentThreadId() << "_"
		<< OmnRandom::letterDigitStr(6,6) << ".txt";

	OmnString usedir = userdoc->getAttrStr(AOSTAG_USER_RSCDIR);
	OmnString fname;
	fname << usedir << "/" << zky_rscfn;
	OmnString dir = OmnApp::getAppConfig()->getAttrStr("user_basedir", "User");
	OmnString fdir;
	if (dir[0] == '/')
	{                                                                                  
			dir << "/" << fname;
				fdir = dir;
	}
	else
	{
			fdir = OmnApp::getAppBaseDir();
				fdir << "/" << dir << "/" << fname;
	}
	OmnScreen << "file name:" << fdir << endl;
	OmnFilePtr file = OmnNew OmnFile(fdir, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_rr(file->isGood(), rdata ,NULL);
	return file;
}


bool 	
AosTermRawCounter::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	docid = AOS_INVDID;
	finished = true;
	return 0;
}


bool 	
AosTermRawCounter::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		OmnString &value, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	if (!mQueryRslt)
	{
		docid = AOS_INVDID;
		finished = true;
		return true;
	}
	return mQueryRslt->nextDocidValue(docid, value, finished, rdata);
}


bool
AosTermRawCounter::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(query_rslt, rdata, false);
	query_rslt->setWithValues(true);
	query_rslt->setOperator(AosDataColOpr::toEnum(mQueryRsltOpr));

	if(query_context)
	{
		if (mKey != "")
		{
			query_context->setOpr(eAosOpr_prefix);
			query_context->setStrValue(mKey);
			query_context->setReverse(mReverse);
			if (mOpr != eAosOpr_an)
			{
				AosQueryFilterValue * filter = OmnNew AosQueryFilterValue();
				filter->mOpr = mOpr;
				filter->mValue1 = atoll(mValue.data());
				query_context->addFilter(filter);	
			}
		}
		else if (mValue != "")
		{
			u16 entry_type = AosCounterUtil::composeTimeEntryType(4, 0, 
					AosStatType::eSum, AosTimeGran::eDaily);

			OmnString value1 = mValue;
			OmnString value2 = mValue;
			if (mValue.indexOf("$$", 0) != -1)
			{
				vector<OmnString> strs;
				AosStrSplit::splitStrBySubstr(mValue, "$$", strs, 100);
				aos_assert_r(strs.size() == 2, false);
				value1 = strs[0];
				value2 = strs[1];
			}

			value1 = AosCounterUtil::composeTerm2(entry_type, "", "", atoi(value1.data()));
			value1.append((char*)"\1", 1);
			query_context->setOpr(eAosOpr_range_ge_le);
			query_context->setStrValue(value1);

			value2 = AosCounterUtil::composeTerm2(entry_type, "", "", atoi(value2.data()));
			value2.append((char*)"\255", 1);
			query_context->setStrValue2(value2);

			query_context->setReverse(mReverse);
		}
		else 
		{
			query_context->setOpr(mOpr);
			query_context->setStrValue(mValue);
			query_context->setReverse(mReverse);
		}
	}
	
	if (mIILId)
	{
		return AosQueryColumn(mIILId, query_rslt, query_bitmap, query_context, rdata);
	}

	return AosQueryColumn(mIILName, query_rslt, query_bitmap, query_context, rdata);
}

