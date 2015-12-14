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
// 12/4/2007: Created by Allen Xu
////////////////////////////////////////////////////////////////////////////

#include "AppMgr/App.h"
//#include "Debug/Debug.h" 
#include "RVG/CommonTypes.h"
#include "RVG/EnumRSG.h"
#include "RVG/AndRSG.h"
#include "RVG/EnumSeqRSG.h"
#include "RVG/CharsetRSG.h"
#include "RVG/OrRSG.h"
#include "RVG/RIG.h"
#include "Util/OmnNew.h"
#include "RVG/ConfigMgr.h"
  
//#define TEST_CONFIG_MGR
//#define TEST_AosCharsetRSG
//#define TEST_AosEnumRSG
//#define TEST_XML_CONFIG
//#define TEST_AosEnumSeqRSG
//#define TEST_AosComposeRSG
//#define TEST_AndRSG
//#define TEST_AosRIG
#define TEST_READ_CONFIG
 

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}
#ifdef TEST_CONFIG_MGR
	std::string dir;
	bool ret = AosConfigMgr::getConfigDir("sslvpn", AosConfigMgr::eTorturer, dir);
	std::cout << "AosConfigMgr::getConfigDir() return " 
		<< ret
		<< std::endl;
#endif

#ifdef TEST_AosCharsetRSG
	AosCharsetRSG charRSG;
	charRSG.setMinLen(3);
	charRSG.setMaxLen(20);
	charRSG.setUniqueFlag(true);
	charRSG.setOrderedFlag(true);
	charRSG.addCharset("abcdefghijklmn");


	TiXmlDocument doc("charsetRSG.xml");
	TiXmlElement element("");
	charRSG.serialize(element);
	doc.InsertEndChild(element);
	doc.SaveFile();
	std::cout<<doc<<std::endl;

	AosValue value;
	for (int i=0; i<100; i++)
	{
		charRSG.nextValue(value);
	//	sleep(1);
		std::cout<<value.toString()<<std::endl;
	}


#endif


#ifdef TEST_AosEnumRSG
	// 
	//	test for EnumRSG
	//
	AosEnumRSG rsg;
	rsg.setEnumString(".", 1);
	rsg.setEnumString("bbb", 10);
	rsg.setEnumString("ccc", 10);
	
	TiXmlDocument doc("enumRSG.xml");
	TiXmlElement element("");
	rsg.serialize(element);
	doc.InsertEndChild(element);
	doc.SaveFile();
	std::cout<<doc<<std::endl;

	AosValue value;
	for (int i=0; i<10; i++)
	{
		rsg.nextValue(value);
	//	sleep(1);
		std::cout<<value.toString()<<std::endl;
	}
#endif

#ifdef TEST_XML_CONFIG
	TiXmlDocument doc("test.xml");
	doc.LoadFile();
	TiXmlHandle handle(&doc);
	TiXmlElement* element = handle.FirstChild("EnumRSG").ToElement();
	AosEnumRSG rsg;
	std::cout<<"parse xml config result:"<<rsg.deserialize(element)<<std::endl;

	AosValue value;
	for (int i=0; i<10; i++)
	{
		rsg.nextValue(value);
	//	sleep(1);
		std::cout<<value.toString()<<std::endl;
	}
	
#endif

#ifdef TEST_AosEnumSeqRSG
	//
	// test for EnumSeqRSG
	//
	AosEnumSeqRSG seqrsg;
	seqrsg.setEnumString("aaa", 20);
	seqrsg.setEnumString("bbb", 10);
	seqrsg.setEnumString("ccc", 10);
	seqrsg.setEnumString("ddd", 10);
	seqrsg.setEnumString("eee", 10);
	seqrsg.setMinNum(1);
	seqrsg.setMaxNum(10);
	seqrsg.setSeperator(' ');
	seqrsg.setUniqueFlag(true);
	seqrsg.setOrderedFlag(false);

	TiXmlDocument doc("enumSeqRSG.xml");
	TiXmlElement element("");
	seqrsg.serialize(element);
	doc.InsertEndChild(element);
	doc.SaveFile();
	std::cout<<doc<<std::endl;

	AosValue value;
	for (int i=0; i<100; i++)
	{
		seqrsg.nextValue(value);
		//sleep(1);
		std::cout<<value.toString()<<std::endl;
	}

#endif

#ifdef TEST_AosComposeRSG
	AosCharsetRSGPtr pCharRSG = OmnNew AosCharsetRSG;
	pCharRSG->setMinLen(3);
	pCharRSG->setMaxLen(10);
	pCharRSG->setUniqueFlag(true);
	pCharRSG->setOrderedFlag(true);
	pCharRSG->addCharset("abcdefg");
	pCharRSG->setComposeWeight(100);

	AosCharsetRSGPtr pCharRSG1 = OmnNew AosCharsetRSG;
	pCharRSG1->setMinLen(3);
	pCharRSG1->setMaxLen(10);
	pCharRSG1->setUniqueFlag(true);
	pCharRSG1->setOrderedFlag(true);
	pCharRSG1->addCharset("1234567890");
	pCharRSG1->setComposeWeight(100);

	AosEnumSeqRSGPtr pseqrsg = OmnNew AosEnumSeqRSG;
	pseqrsg->setEnumString("@@@", 20);
	pseqrsg->setEnumString("###", 10);
	pseqrsg->setEnumString("$$$", 10);
	pseqrsg->setEnumString("%%%", 10);
	pseqrsg->setEnumString("&&&", 10);
	pseqrsg->setMinNum(1);
	pseqrsg->setMaxNum(2);
	pseqrsg->setSeperator(',');
	pseqrsg->setUniqueFlag(true);
	pseqrsg->setOrderedFlag(false);
	pseqrsg->setComposeWeight(50);

	AosOrRSG composeRSG;
	composeRSG.pushBRSG(pCharRSG);
	composeRSG.pushBRSG(pseqrsg);
	composeRSG.pushBRSG(pCharRSG1);
//	composeRSG.setComposeType(eRSGComposeType_AND);

	AosValue value;
	for (int i=0; i<100; i++)
	{
		composeRSG.nextValue(value);
		//sleep(1);
		std::cout<<value.toString()<<std::endl;
	}

#endif

#ifdef TEST_AndRSG
	OmnSPtr< AosRIG<u8> > u1,u2,u3,u4;
	u1 = OmnNew AosRIG<u8>;
	u1->setIntegerPair(192,192,1);
	u2 = OmnNew AosRIG<u8>;
	u2->setIntegerPair(168,168,1);
	u3 = OmnNew AosRIG<u8>;
	u3->setIntegerPair(252,254,1);
	u4 = OmnNew AosRIG<u8>;
	u4->setIntegerPair(1,254,1);

	AosEnumRSGPtr pEnum1 = OmnNew AosEnumRSG;
	pEnum1->setEnumString(".", 1);
	AosEnumRSGPtr pEnum2 = OmnNew AosEnumRSG;
	pEnum2->setEnumString(".", 1);
	AosEnumRSGPtr pEnum3 = OmnNew AosEnumRSG;
	pEnum3->setEnumString(".", 1);

	AosAndRSG andRSG;
	andRSG.pushBRSG(u1);
	andRSG.pushBRSG(pEnum1);
	andRSG.pushBRSG(u2);
	andRSG.pushBRSG(pEnum2);
	andRSG.pushBRSG(u3);
	andRSG.pushBRSG(pEnum3);
	andRSG.pushBRSG(u4);

	TiXmlDocument doc("andRSG.xml");
	TiXmlElement element("");
	andRSG.serialize(element);
	doc.InsertEndChild(element);
	doc.SaveFile();
	std::cout<<doc<<std::endl;

	AosValue value;
	for (int i=0; i<20; i++)
	{
		andRSG.nextValue(value);
		//sleep(1);
		std::cout<<value.toString()<<std::endl;
	}
#endif

#ifdef TEST_AosRIG
	AosRIG<u16> rig;
	rig.setIntegerPair(1,10,100);
	rig.setIntegerPair(11,100,100);
	rig.setIntegerPair(101,1000,100);
	rig.setIntegerPair(1001,10000,100);

	TiXmlDocument doc("rig.xml");
	TiXmlElement element("");
	rig.serialize(element);
	doc.InsertEndChild(element);
	doc.SaveFile();
	std::cout<<doc<<std::endl;

	AosValue value;
	for (int i=0; i<100; i++)
	{
		rig.nextValue(value);
		//sleep(1);
		std::string tmp = value.toString();
		std::cout<<tmp<<std::endl;
	}

#endif
#ifdef TEST_READ_CONFIG
	AosRVGPtr rvg = AosRVG::RVGFactory("andRSG.xml");
	if (rvg)
	{
		AosValue value;
		rvg->nextValue(value);
		std::cout<<value.toString()<<std::endl;
	}
#endif
	theApp.exitApp();
	return 0;
} 

 
