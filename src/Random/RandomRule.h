////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 05/19/2011   Created by Chen Ding
// 01/26/2012   Moved from SEUtil/RandomRule
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Random_RandomRule_h
#define AOS_Random_RandomRule_h

#include "alarm_c/alarm.h"
#include "Random/Ptrs.h"
#include "Random/RandomUtil.h"
#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
#include <map>

#define AOS_XMLRANDRULE     			"XmlFormat"

using namespace std;

class AosRandomRule: virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftLevel = 5,
	};

	struct Value
	{
		OmnString range;
		OmnString value;
		OmnString getRandomValue()
		{
			AosRandomRule::setRandomValue(range, value);
			return value;
		}
		OmnString getRandomValue(OmnString _range)
		{
			range = _range;
			AosRandomRule::setRandomValue(_range, value);
			return value;
		}
	};

	AosXmlTagPtr 						mNode;
	OmnString 							mNodeName;

	//this is required of value
	map<OmnString, Value> 				mAttrs_r;
	map<OmnString, AosRandomRulePtr> 	mNodes_r;
	struct Value 						mTexts_r;

	//this is optional of value
	map<OmnString, Value> 				mAttrs_o;
	map<OmnString, AosRandomRulePtr> 	mNodes_o;
	struct Value 						mTexts_o;
	OmnString							mObjid;

	// Percent Definitions
	vector<OmnString>					mPercentNames;
	vector< vector<int> >				mPercentValues;

public:
	AosRandomRule() {}
	AosRandomRule(const AosXmlTagPtr &config);
	AosRandomRule(const OmnString &path);

	static 	void setRandomValue( const OmnString &range, OmnString &value);
	OmnString getNodename(const OmnString &dft) const 
	{
		if (mNodeName != "") return mNodeName;
		return dft;
	}
	void setNodename(const OmnString &n) {mNodeName = n;}
	OmnString getWord(const OmnString &argname);
	OmnString getWord(const OmnString &argname, const OmnString &dft);
	OmnString getEntrySeparator(const OmnString &dft)
	{
		if (OmnRandom::percent(50)) return ",";
		return dft;
	}
	OmnString pickObjid() const;
	int percent(const OmnString &name, const int num);
	int percent(const OmnString &name, const int w1, const int w2);
	int percent(const OmnString &name, const int w1, const int w2, const int w3);

private:
	void setRule(const OmnString type);

	static 	void toRandom(const OmnString &range, OmnString &value);
	static vector<OmnString> split(const OmnString &src, const OmnString &sep);
};

typedef map<OmnString, AosRandomRule::Value>::iterator XmlGenRuleValueIter;
typedef map<OmnString, AosRandomRulePtr>::iterator XmlGenRuleNodeIter;
#endif
