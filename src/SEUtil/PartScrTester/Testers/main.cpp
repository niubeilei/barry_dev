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
          
#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h" 
#include "Util/UtUtil.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
  
AosXmlTagPtr gAosAppConfig;
int 
main(int argc, char **argv)
{
	//Partition  Script
	//OmnFilePtr file = OmnNew OmnFile("1.txt", OmnFile::eReadOnly);
	OmnString cmd = "rm -fr ./Server/*";
	system(cmd);
	OmnFilePtr file = OmnNew OmnFile("/home/ketty/AOS/src/TransServer/Torturer/client_23291", OmnFile::eReadOnly);
	aos_assert_r(file->isGood(),false);
	OmnString contents;
	//file->readToString(contents);
	bool finished;
	int i = 0;
	int j = 0;
	OmnFilePtr file1;//= OmnNew OmnFile("./doc"+j, OmnFile::eReadWrite);
	while (!finished)
	{
		contents = "";
		if (i%100000 == 0)
		{
			cout << "num: "<< j << endl;
			OmnString name= "./Client/doc_";
			name << j;
			file1 = OmnNew OmnFile(name, OmnFile::eReadWrite);
			if (!file1->isGood())
			{
				file1 = OmnNew OmnFile(name, OmnFile::eCreate);
				aos_assert_r(file1->isGood(), NULL);
			}
			//aos_assert_r(file1->getFile()->isGood(), NULL);
			j++;
		}
		contents = file->getLine(finished);
		contents << "\n";
		file1->append(contents, true);
		i++;
	}
	file1->closeFile();
	file->closeFile();
	
	return 0;
} 

/*
bool SegmentationScript()
{

}
*/

bool testxml(OmnString &contents)
{
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(contents, "" AosMemoryCheckerArgs);
	
cout<<"before:"<<contents <<endl;
	//badxml2.txt
	AosXmlTagPtr doc = root->getFirstChild();
	int len = (rand() % 10) + 5;
	char data[40];
	AosRandomLetterStr(len, data);
	OmnString word = data;

	doc->setText(word, (rand()%100) > 60);

cout<<"after:"<<doc->toString()<<endl;
	OmnString str = doc->toString();
	AosXmlParser parser1;
	AosXmlTagPtr xml = parser1.parse(str, "" AosMemoryCheckerArgs);

//	//badxml3.txt
//	AosXmlTagPtr doc = root->getFirstChild();
//	int len = (rand() % 10) + 5;
//	char data[40];
//	AosRandomLetterStr(len, data);
//	OmnString word = data;
//	OmnString tagname = "tagname72";
//
//	doc->setNodeText(tagname, word, (rand()%100) > 60);
//
//	OmnString str = doc->toString();
///	AosXmlParser parser1;
//	AosXmlTagPtr xml = parser1.parse(str, "");
	return true;
	}
	

