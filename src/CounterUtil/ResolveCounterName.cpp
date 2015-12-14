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
// A counter name is constructed as:
// 	ddd1:ddd2:...:dddn
//
// This is a utility class. Every time its 'nextName()' is called, it 
// returns the next segment as:
// 	"ddd1:ddd2:ddd3:ddd4:ddd5"
// 	"ddd2:ddd3:ddd4:ddd5"
// 	"ddd3:ddd4:ddd5"
// 	"ddd4:ddd5"
// 	"ddd5"
//
// Modification History:
// 06/15/211: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "CounterUtil/ResolveCounterName.h"

#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/CounterVarType.h"
#include "ValueSel/ValueSel.h"

AosResolveCounterName::AosResolveCounterName()
{
}


bool
AosResolveCounterName::resolveCounterName(
		const AosXmlTagPtr &def,
		const OmnString &tagname,
		OmnString &resolvedName,
		const bool need_proc_member,
		const AosRundataPtr &rdata)
{
	resolvedName = "";
	aos_assert_rr(def, rdata, false);
	aos_assert_rr(tagname != "", rdata, false);
	AosXmlTagPtr child = def->getFirstChild(tagname);
	if (!child) return true;
	return resolveCounterName(child, resolvedName, need_proc_member, rdata);
}


bool
AosResolveCounterName::resolveCounterName(
		const AosXmlTagPtr &cNameTmp,
        OmnString &resolvedName,
		const bool need_proc_member,
		const AosRundataPtr &rdata)
{
	// This function resolves a counter name template into an actual
	// counter name (i.e., a counter name that contains no counter
	// variables). Counter names format is:
	// 	<zky_cname>
	// 	    <termname zky_value_type="const">cool</termname>
	// 	    <termname zky_value_type="const">cn</termname>
	// 	    <termname zky_value_type="const">js</termname>
	// 	    <termname zky_value_type="attr"  zky_xpath="city">
	// 	            <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 	    </termname>
	// 	    .... 
	// 	</zky_cname>
	// cname should be "${xxx}|$$|${xxx}|$$|docid|$|xxx" , 
	// 				   "xxx|$$|${did:xxx}|$$|docid|$|xxx" ,
	// 				   "xxx|$$|${xxx}|$$|docid|$|xxx"
	// 	It is not time based: cool|$$|cn|$$|js|$_|sz
	// 	time based: cool|$$|cn|$$|js|$$|sz_20120518
	aos_assert_r(cNameTmp, false);
	AosXmlTagPtr cNameTermXml = cNameTmp->getFirstChild();
	AosValueRslt cNameValue;
	resolvedName = "";
	OmnString member = "";
	while(cNameTermXml)
	{
		OmnString segmentName;
		if (!AosValueSel::getValueStatic(cNameValue, cNameTermXml, rdata)) 
		{
			cNameTermXml = cNameTmp->getNextChild();
			continue;
		}
		cNameTermXml = cNameTmp->getNextChild();
		segmentName = cNameValue.getStr();
		if (segmentName != "")
		{
			if (need_proc_member && !cNameTermXml) 
			{
				member = segmentName;
			}
			else
			{
				if (resolvedName != "") resolvedName << AOS_COUNTER_SEGMENT_SEP;
				resolvedName << segmentName;
			}
		}
	}

	if (resolvedName != "" && need_proc_member) 
	{
		resolvedName << AOS_COUNTER_PART_SEP << member; 
	}

	if (resolvedName == "") resolvedName = member;
	return true;
}


bool
AosResolveCounterName::resolveCounterName2(
		const AosXmlTagPtr &cNameTmp,
        OmnString &resolvedName,
		const AosRundataPtr &rdata)
{
	// This function resolves a counter name template into an actual
	// counter name (i.e., a counter name that contains no counter
	// variables). Counter names format is:
	// 	<zky_cname>
	// 	    <termname zky_value_type="const">cool</termname>
	// 	    <termname zky_value_type="const">cn</termname>
	// 	    <termname zky_value_type="const">js</termname>
	// 	    <termname zky_value_type="attr"  zky_xpath="city">
	// 	            <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 	    </termname>
	// 	    .... 
	// 	</zky_cname>
	// 	It is not time based: cool|$$|cn|$$|js|$_|sz
	// 	time based: cool|$$|cn|$$|js|$$|sz_20120518
	
	aos_assert_r(cNameTmp, false);
	AosXmlTagPtr cNameTermXml = cNameTmp->getFirstChild();
	AosValueRslt cNameValue;
	resolvedName = "";
	while(cNameTermXml)
	{
		OmnString segmentName;
		if (!AosValueSel::getValueStatic(cNameValue, cNameTermXml, rdata)) 
		{
			cNameTermXml = cNameTmp->getNextChild();
			continue;
		}

		segmentName = cNameValue.getStr();
		if (segmentName != "")
		{
			if (resolvedName != "") resolvedName << AOS_COUNTER_SEGMENT_SEP2;
			resolvedName << segmentName;
		}
		cNameTermXml = cNameTmp->getNextChild();
	}

	return true;
}

bool 
AosResolveCounterName::resolveCounterName2(
			const AosXmlTagPtr &def,
			const OmnString &tagname,
			OmnString &resolvedName,
			const AosRundataPtr &rdata)
{
	i64 level = 0;
	return resolveCounterName2(def, tagname, resolvedName, level, rdata);
}
	
bool 
AosResolveCounterName::resolveCounterName2(
			const AosXmlTagPtr &def,
			const OmnString &tagname,
			OmnString &resolvedName,
			i64 &level,
			const AosRundataPtr &rdata)
{
	resolvedName = "";
	aos_assert_rr(def, rdata, false);
	level = def->getAttrInt("zky_level", -1);
	if (level == -1) 
	{
		OmnString levelstr = def->getNodeText("zky_level");
		level = atoi(levelstr.data());
	}

	aos_assert_rr(tagname != "", rdata, false);
	AosXmlTagPtr child = def->getFirstChild(tagname);
	if (!child) return true;
	return resolveCounterName2(child, resolvedName, rdata);
}


bool
AosResolveCounterName::resolveCounterName3(
		const AosXmlTagPtr &cNameTmp,
        OmnString &resolvedName1,
        OmnString &resolvedName2,
		const AosRundataPtr &rdata)
{
	// This function resolves a counter name template into an actual
	// counter name (i.e., a counter name that contains no counter
	// variables). Counter names format is:
	// 	<zky_cname>
	// 	    <termname zky_value_type="const">cool</termname>
	// 	    <termname zky_value_type="const">cn</termname>
	// 	    <termname zky_value_type="const">js</termname>
	// 	    <termname zky_value_type="attr"  zky_xpath="city">
	// 	            <zky_docselector zky_docselector_type="receiveddoc"></zky_docselector>
	// 	    </termname>
	// 	    .... 
	// 	</zky_cname>
	// 	It is not time based: cool|$$|cn|$$|js|$_|sz
	// 	time based: cool|$$|cn|$$|js|$$|sz_20120518
	
	aos_assert_r(cNameTmp, false);
	AosXmlTagPtr cNameTermXml = cNameTmp->getFirstChild();
	AosValueRslt cNameValue;
	resolvedName1 = "";
	resolvedName2 = "";
	while(cNameTermXml)
	{
		OmnString segmentName;
		if (!AosValueSel::getValueStatic(cNameValue, cNameTermXml, rdata)) 
		{
			cNameTermXml = cNameTmp->getNextChild();
			continue;
		}

		segmentName = cNameValue.getStr();
		if (segmentName != "")
		{
			if (resolvedName1 != "") resolvedName1 << AOS_COUNTER_SEGMENT_SEP2;
			if (resolvedName2 != "") resolvedName2 << AOS_COUNTER_SEGMENT_SEP2;

			bool finished = true;
			vector<OmnString> str;
			AosStrSplit::splitStrBySubstr(
				segmentName.data(), "$$", str, 100, finished);
			if (str.size() == 1)
			{
				resolvedName1 << str[0];
				resolvedName2 << str[0];
			}
			else if (str.size() == 2) 
			{
				resolvedName1 << str[0];
				resolvedName2 << str[1];
			}
		}
		cNameTermXml = cNameTmp->getNextChild();
	}

	return true;
}

	
bool 
AosResolveCounterName::resolveCounterName3(
			const AosXmlTagPtr &def,
			const OmnString &tagname,
			OmnString &resolvedName1,
			OmnString &resolvedName2,
			i64 &level,
			const AosRundataPtr &rdata)
{
	resolvedName1 = "";
	resolvedName2 = "";
	aos_assert_rr(def, rdata, false);
	level = def->getAttrInt("zky_level", -1);
	if (level == -1) 
	{
		OmnString levelstr = def->getNodeText("zky_level");
		level = atoi(levelstr.data());
	}

	aos_assert_rr(tagname != "", rdata, false);
	AosXmlTagPtr child = def->getFirstChild(tagname);
	if (!child) return true;
	return resolveCounterName3(child, resolvedName1, resolvedName2, rdata);
}

