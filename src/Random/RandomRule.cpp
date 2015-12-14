///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 05/19/2011   Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomRule.h"

#include "Alarm/Alarm.h"
#include "Random/CommonValues.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "XmlUtil/SeXmlParser.h"



AosRandomRule::AosRandomRule(const AosXmlTagPtr &config)
{
	mNode = config;
	mNodeName = config->getAttrStr("name");
	setRule("required");
	setRule("optional");
}


AosRandomRule::AosRandomRule(const OmnString &type)
{
	OmnString name = AOS_XMLRANDRULE;
	name << "/" << type << ".xml";
	OmnFilePtr file = OmnNew OmnFile(name, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert(file->isGood());
	OmnString contents;
	file->readToString(contents);
	file = 0;

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(contents, "" AosMemoryCheckerArgs);

	mNode = root;
	mNodeName = root->getAttrStr("name");
	setRule("required");
	setRule("optional");
}


void
AosRandomRule::toRandom(
		const OmnString &range,
		OmnString &value)
{
	OmnString rr = range.substr(1, range.length()-2);
	OmnString sep = "#";
	vector<OmnString> ranges;
	char type = range.data()[0];
	switch(type)
	{
	case '[':
		 {
			ranges = split(rr, sep);
			int idx = rand()%(ranges.size());
			value = ranges[idx];
		 }
		 break;
	case '(':
		 {
			ranges = split(rr, sep);
			OmnString s = ranges[0];
			OmnString e = ranges[1];
			OmnString sepp = ".";
			int a1, a2;
			if (s.find('.', false) <0)
			{
				a1 = 0;
			}
			else
			{
				a1 = split(s, sepp)[1].length();
			}
			if (s.find('.', false) <0)
			{
				a2 = 0;
			}
			else
			{
				a2 = split(e, sepp)[1].length();
			}
			int num = a1>a2?a1:a2;
			if (num == 0)
			{
				int r = s.toInt() + rand()%(e.toInt()-s.toInt()+1);
				OmnString tmpint;
				tmpint << r;
				value = tmpint;
			}
			else
			{
				int trimnum = 6 - num;
				double acc = 1.0;
				while(num--)
				{
					acc *=10;
				}
				double sd, ed;
				u32 sl = s.length();
				u32 el = e.length();
				s.parseDouble(0, sl, sd);
				e.parseDouble(0, el, ed);
				double r = sd + (rand()%(int)((ed-sd)*acc+1))/(acc);
				OmnString tmpdouble;
				tmpdouble << r;
				tmpdouble.trim(trimnum);
				value = tmpdouble;
			}
		 }
		 break;

	case '{':
		 {
			int len = rand() % 20;
			OmnString tmpstr;
			for(int i=0; i<len; i++)
			{
				uint idx = rand()%rr.length();
				tmpstr << rr[idx];
			}
			value = tmpstr;
		 }
		 break;
	}
}


OmnString 
AosRandomRule::getWord(const OmnString &argname)
{
	OmnNotImplementedYet;
	return "";
}


OmnString 
AosRandomRule::getWord(const OmnString &argname, const OmnString &dft)
{
	OmnNotImplementedYet;
	return "";
}


vector<OmnString>
AosRandomRule::split(
		const OmnString &src, 
		const OmnString &sep) 
{
	vector<OmnString> strArray;
	OmnString vstr = src;
	while(1)
	{
		int idx = vstr.indexOf(sep, 0);
		if (idx != -1)
		{
			OmnString vv;
			if (idx == 1)
			{
				vv << vstr.data()[0];
			}
			else
			{
				vv = vstr.substr(0, idx-1);
			}
			strArray.push_back(vv);
		}
		else
		{
			strArray.push_back(vstr);
			break;
		}
		vstr = vstr.substr(idx+1, vstr.length()-1);
	}
	return strArray;
}


//valueStr format: 
//	 value    		ratio						range type
//	(xxxxxxxx#xxxxxxx),xxxx$(xxxxxx#xxxxxx),xxxx 	int 
//	[xxxi#xxx#xxx#xxx],xxxx$[xxxxxx#xxxxxx],xxxx	string
//	{xxxxxxxxxxxxxxxx},xxxx${xxxxxxxxxxxxx},xxxx	string
void
AosRandomRule::setRandomValue(
		const OmnString &valueStr,
		OmnString &value)
{
	OmnString vstr;
	OmnString sep1 = "$";
	OmnString sep2 = ",";
	vector<OmnString> rangeArray;
	rangeArray = split(valueStr, sep1);
	uint len = rangeArray.size();
	OmnString strTemp;
	for(uint i=0; i<len-1; i++)
	{
		for(uint j=i+1; j<len; j++)
		{
			vector<OmnString> jj = split(rangeArray[j], sep2);
			vector<OmnString> ii = split(rangeArray[i], sep2);
			if (jj.size()!=2 || ii.size()!=2)
			{
				break;
			}
			if (ii[1].toInt() < jj[1].toInt())
			{
				strTemp = rangeArray[i];
				rangeArray[i] = rangeArray[j];
				rangeArray[j] = strTemp;
			}
		}
	}

	int total = 0;
	if (len > 1)
	{
		for(uint i=0; i<len; i++)
		{
			total += split(rangeArray[i], sep2)[1].toInt();
		}
	}
	else
	{
		total = 100;
	}
	int ra = rand()%total;

	int tmp = 0;
	for(uint i=0; i<len; i++)
	{
		vector<OmnString> rr = split(rangeArray[i], sep2);
		if (rr.size() == 2)
		{
			tmp += rr[1].toInt();
			if (ra < tmp)
			{
				vstr = rr[0];
				break;
			}
		}
		else
		{
			vstr = rr[0];
			break;
		}
	}
	toRandom(vstr, value);
}

void 
AosRandomRule::setRule(const OmnString type)
{
	AosXmlTagPtr child = mNode->getFirstChild(type);
	if (child)
	{
		AosXmlTagPtr attrs = child->getFirstChild("attrs");
		if (attrs)
		{
			AosXmlTagPtr attr = attrs->getFirstChild("attr");
			while(attr)
			{
				OmnString name = attr->getAttrStr("name");
				OmnString range = attr->getNodeText();
				struct Value attrObj;
				attrObj.range = range;
				if (type == "required")
				{
					mAttrs_r.insert(pair<OmnString, struct Value>(name, attrObj));
				}
				else
				{
					mAttrs_o.insert(pair<OmnString, struct Value>(name, attrObj));
				}
				attr = attrs->getNextChild();
			}
		}
		AosXmlTagPtr nodes = child->getFirstChild("childnodes");
		if (nodes)
		{
			AosXmlTagPtr node = nodes->getFirstChild("node");
			while(node)
			{
				AosRandomRulePtr xmlRule = OmnNew AosRandomRule(node);
				if (type == "required")
				{
					mNodes_r.insert(pair<OmnString, AosRandomRulePtr>(xmlRule->mNodeName, xmlRule));
				}
				else
				{
					mNodes_o.insert(pair<OmnString, AosRandomRulePtr>(xmlRule->mNodeName, xmlRule));
				}
				node = nodes->getNextChild();
			}
		}
		else
		{
			AosXmlTagPtr texts = child->getFirstChild("texts");
			if (texts)
			{
				OmnString range = texts->getNodeText();
				if (type == "required")
				{
					mTexts_r.range = range;
				}
				else
				{
					mTexts_o.range = range;
				}
			}
		}
	}
}


OmnString 
AosRandomRule::pickObjid() const
{
	if (mObjid != "") return mObjid;
	return AosCommonValues::pickObjid();
}


int 
AosRandomRule::percent(const OmnString &name, const int pct)
{
	for (u32 i=0; i<mPercentNames.size(); i++)
	{
		if (mPercentNames[i] == name)
		{
			return OmnRandom::percent(mPercentValues[i][0]);
		}
	}

	return OmnRandom::percent(pct);
}


int 
AosRandomRule::percent(const OmnString &name, const int w1, const int w2)
{
	for (u32 i=0; i<mPercentNames.size(); i++)
	{
		if (mPercentNames[i] == name)
		{
			return OmnRandom::percent(mPercentValues[i][0], mPercentValues[i][1]);
		}
	}

	return OmnRandom::percent(w1, w2);
}


int 
AosRandomRule::percent(const OmnString &name, const int w1, const int w2, const int w3)
{
	for (u32 i=0; i<mPercentNames.size(); i++)
	{
		if (mPercentNames[i] == name)
		{
			return OmnRandom::percent(
					mPercentValues[i][0], 
					mPercentValues[i][1],
					mPercentValues[i][2]);
		}
	}

	return OmnRandom::percent(w1, w2, w3);
}

