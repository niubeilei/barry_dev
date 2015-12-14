////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ConfigReader.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_Config_ConfigReader_h
#define Omn_Config_ConfigReader_h


#include "Config/Ptrs.h"
#include "Debug/Rslt.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "Util/File.h"
#include "XmlParser/Ptrs.h"


class OmnConfigEntity;

typedef OmnVList<OmnConfigEntity *>	OmnConfigEntityList;




class OmnConfigReader 
{
public:
	enum
	{
		eBuffSize = 30000
	};

private:
	static OmnString	eDefaultConfigFileName;

	OmnFilePtr			mFile;

	OmnString			mData;
	OmnXmlParserPtr		mParser;
	OmnMutexPtr			mLock;

	OmnConfigEntityList	mEntity;

public:
    OmnConfigReader();
    OmnConfigReader(const OmnString &configFileName);
    OmnConfigReader(const int argc, char **argv);
    ~OmnConfigReader();

	OmnXmlParserPtr		getParser() const;

private:
    OmnRslt				createParser();
	OmnRslt				readData();
};
#endif
#endif
