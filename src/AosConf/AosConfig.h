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
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AosConf_AosConfig_h
#define Aos_AosConf_AosConfig_h

#include "alarm_c/alarm.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

#include <map>
#include <string>
using namespace std;

namespace AosConf 
{
class AosConfBase 
{
protected:
	map<string, string> mAttrs;
	string mTagName;
	string mConf;

public:
	void setAttribute(const string &name, const string &value) {
		if (value != "") mAttrs[name] = value;
	}
	void setTagName(const string &name)
	{
		if (name != "") mTagName = name;
	}

	void setAttribute(const string &name, int64_t value) {
		char buff[100]; 
		sprintf(buff, "%ld", value);
		mAttrs[name] = buff;
	} 

	string getAttribute(const string &name) {
		if (mAttrs.count(name)) return mAttrs[name];
		return "";
	}

	map<string, string> getAttributes() {
		return mAttrs;	
	}
	virtual string getConfig() =0;


};
};

#endif
