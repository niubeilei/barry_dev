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
int 
main(int argc, char **argv)
{

	OmnFilePtr file = OmnNew OmnFile("../badxml2.txt", OmnFile::eReadOnly);
	aos_assert_r(file->isGood(),false);
	OmnString contents;
	file->readToString(contents);
	file->closeFile();
	
cout<<"before:"<<contents <<endl;
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(contents, "");

	/*
	cout << "attr size: " << attrArray.size() << endl;
	cout << "node size: " << nodeArray.size() << endl;
	cout << "text : " << txt << endl;
	*/
	return 0;
} 

