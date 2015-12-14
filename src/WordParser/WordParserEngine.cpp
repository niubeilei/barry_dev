////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/18/2010	Created by James Kong
////////////////////////////////////////////////////////////////////////////
#include "WordParser/WordParserEngine.h"

#include "Util/File.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"
#include "Tracer/Tracer.h"
#include "Thread/RwLock.h"

OmnSingletonImpl(AosWordParserEngineSingleton,
                 AosWordParserEngine,
                 AosWordParserEngineSelf,
                "WordParserEngine");

AosWordParserEngine::AosWordParserEngine()
:
mRwLock(OmnNew OmnRwLock())
{
}


AosWordParserEngine::~AosWordParserEngine()
{
	ICTCLAS_Exit();
}

bool      	
AosWordParserEngine::start()
{

	return true;
}


bool        
AosWordParserEngine::stop()
{
	return true;
}


bool		
AosWordParserEngine::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr modules = config->getFirstChild("modules");
	aos_assert_r(modules, false);
	AosXmlTagPtr ictclas = modules->getFirstChild("ICTCLAS");
	aos_assert_r(ictclas, false);
	OmnString dirname = ictclas->getAttrStr("dirname");
	aos_assert_r(dirname != "", false);

	if(!ICTCLAS_Init(dirname.data()))
	{
	    OmnAlarm << "error, can't init the ICTCLAS!" << enderr;  
		return false;
	}
	return true;
}

LPICTCLAS_RESULT 
AosWordParserEngine::paragraphProcessA(
		const char* src, 
		const int strlen, 
		int &rsltlen)
{
	mRwLock->readlock();
	LPICTCLAS_RESULT rslt = ICTCLAS_ParagraphProcessA(src, strlen, rsltlen, CODE_TYPE_UTF8, 1);
	mRwLock->unlock();
	return rslt;
}

void 
AosWordParserEngine::resultFree(LPICTCLAS_RESULT rslt)
{
	ICTCLAS_ResultFree(rslt);
}

void 
AosWordParserEngine::importUserDictFile(const OmnString &usrdic)
{
	mRwLock->writelock();
	OmnString fname = "usrdir.txt";
	static char line = '\n';
	OmnFile::writeToFile(fname, 0, 1, &line);
	OmnFile::writeToFile(fname, 1, usrdic.length(), (char*)usrdic.data());
	ICTCLAS_ImportUserDictFile(fname.data(), CODE_TYPE_UTF8);
	mRwLock->unlock();
}
