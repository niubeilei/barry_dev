////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/XmlGenerateRule.h"
          
#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h" 
#include "SEUtil/XmlTag.h"
#include "SEUtil/SeXmlParser.h"
#include "Util/UtUtil.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"
  
AosXmlTagPtr gAosAppConfig;
void
printinfo(const AosXmlTagPtr &config)
{
	AosXmlGenerateRule xx(config);
	//required
	cout << "current node: " << xx.mNodeName << endl;
	map<OmnString, AosXmlGenerateRule::Value>::iterator ait;
	for(ait=xx.mAttrs_r.begin(); ait!=xx.mAttrs_r.end(); ait++)
	{
		cout << "required: " << ait->first << " =\"" << ait->second.value << "\"" << endl;
	}

	for(ait=xx.mAttrs_o.begin(); ait!=xx.mAttrs_o.end(); ait++)
	{
		cout << "optional: " << ait->first << "=\"" << ait->second.value << "\"" << endl;
	}

	map<OmnString, AosXmlGenerateRulePtr>::iterator nit;
	if (xx.mNodes_r.size() > 0)
	{
		for(nit=xx.mNodes_r.begin(); nit!=xx.mNodes_r.end(); nit++)
		{
			printinfo((nit->second)->mNode);
		}
	}
	else
	{
		if (xx.mTexts_r.value.length() != 0)
		{
			cout << xx.mTexts_r.value << endl;
		}
	}

	if (xx.mNodes_o.size() > 0)
	{
		for(nit=xx.mNodes_o.begin(); nit!=xx.mNodes_o.end(); nit++)
		{
			printinfo((nit->second)->mNode);
		}
	}
	else
	{
		if (xx.mTexts_o.value.length() != 0)
		{
			cout << xx.mTexts_o.value << endl;
		}
	}

}

int 
main(int argc, char **argv)
{

	OmnFilePtr file = OmnNew OmnFile("config.txt", OmnFile::eReadOnly);
	aos_assert_r(file->isGood(),false);
	OmnString contents;
	file->readToString(contents);
	file->closeFile();
	
cout<< contents <<endl;
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(contents, "");
	printinfo(root);
	return 0;
} 
