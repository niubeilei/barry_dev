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
// 04/17/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "TinyXml/tinyxml.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util_c/global_data.h"
#include "XmlInterface/XmlProc.h"
  
#include <stdlib.h>

bool readFromFile();
bool readFromString();
bool createXml();
bool textNode();
bool traverseDoc();
bool testAttributes();

int 
main(int argc, char **argv)
{
	aos_global_data_init();

	OmnApp theApp(argc, argv);

	int idx = 0;
	std::string filename;
	int seed = 0;
	while (idx < argc)
	{
		if (strcmp(argv[idx], "-f") == 0)
		{
			filename = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-seed") == 0)
		{
			seed = atoi(argv[idx+1]);
			idx += 2;
			continue;
		}

		idx++;
	}

	srand(seed);

	readFromFile();
	readFromString();
	createXml();
	textNode();
	traverseDoc();
	testAttributes();

	sleep(1);
	theApp.exitApp();
	return 0;
} 
 
bool readFromFile()
{
	TiXmlDocument thedoc("test1.xml");
	bool is_good = thedoc.LoadFile();
	cout << "------------------------------------" << endl;
	cout << "Test Case: Read from file" << endl;
	cout << "Is good: " << is_good << endl;
	thedoc.Print();
	cout << "------------------------------------" << endl;
	return true;
}


bool readFromString()
{
	TiXmlDocument thedoc;
	std::string contents = "<mydoc attr1='first_name'><email value='chending'/></mydoc>";
	bool is_good = thedoc.Parse(contents.data());
	cout << "\n\n------------------------------------" << endl;
	cout << "Test Case: Read from string" << endl;
	cout << "Is good: " << is_good << endl;
	cout << "The root: " << (thedoc.FirstChild())->Value() << endl;
	TiXmlElement *elem = (thedoc.FirstChild())->ToElement();
	cout << "The attr: " << elem->Attribute("attr1") << endl;

	thedoc.Print();
	cout << "------------------------------------" << endl;
	return true;
}

bool createXml()
{
	TiXmlDocument doc;
	TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement *element = new TiXmlElement("Hello");
	TiXmlText *text = new TiXmlText("World");
	element->LinkEndChild(text);
	doc.LinkEndChild(decl);
	doc.LinkEndChild(element);
	cout << "\n\n------------------------------------" << endl;
	doc.Print();
	cout << "------------------------------------" << endl;
	return true;
}

bool textNode()
{
	TiXmlText *text = new TiXmlText("world");
	cout << "The value: " << text->Value() << endl;
	return true;
}

void getChild(const TiXmlNode *node)
{
	cout << "Node type: " << node->Type() << endl;
	if (node->NoChildren())
	{
		cout << "Node : " << node->Value() << " has no children!" << endl;
		return;
	}

	cout << "Node: " << node->Value() << " has the following children: " << endl;
	const TiXmlNode *cc = 0;
	while ((cc = node->IterateChildren(cc)))
	{
		cout << "Child: " << cc->Value() << endl;
	}

	cc = 0;
	while ((cc = node->IterateChildren(cc))) getChild(cc);
}


bool traverseDoc()
{
	TiXmlDocument doc("sample.txt");
	doc.LoadFile();

	const TiXmlNode *child = 0;
	cout << "\n\n------------------------------------" << endl;
	while ((child = doc.IterateChildren(child)))
	{
		cout << "Found child: " << child->Value() << endl;
		getChild(child);
	}
	cout << "------------------------------------" << endl;

	return true;
}

bool testAttributes()
{
	TiXmlDocument doc("sample.txt");
	doc.LoadFile();

	cout << "\n\n------------------------------------" << endl;
	const TiXmlNode *child = doc.FirstChild();
	const TiXmlElement *elem = child->ToElement();
	cout << "First child should be 'book': " << elem->Value() << endl;
	cout << "Expecting attribute 'container_bgcolor': " << elem->Attribute("container_bgcolor") << endl;
	cout << "------------------------------------" << endl;
	return true;
}



