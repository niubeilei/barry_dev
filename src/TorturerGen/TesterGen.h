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
//
// Modification History:
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerGen_TesterGen_h
#define Omn_TorturerGen_TesterGen_h

#include "TorturerGen/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"


class AosTesterGen : public OmnRCObject
{
private:

protected:
	OmnString					mName;
	OmnString					mClassName;
	OmnString					mSrcDirName;
	OmnString					mTestDirName;
	OmnDynArray<AosTestGenPtr>	mTests;

public:
	AosTesterGen(const OmnString &srcDirName, 
				 const OmnString &testDirName); 
	virtual ~AosTesterGen();

	virtual bool	generateCode(const bool force) = 0;	
	static AosTesterGenPtr createTester(
				const OmnString &srcDirName, 
				const OmnString &testDirName, 
				const OmnString &filename, 
				const OmnXmlItemPtr &def);

	virtual bool		config(const OmnXmlItemPtr &def) = 0;
	virtual OmnString	getHeaderFileName() const {return mClassName;}
	OmnString		getClassName() const {return mClassName;}
	OmnString		getFilename() const;
	OmnString		getDirectoryName() const;
	OmnString		getIncludes(const OmnString &files) const;
	bool			appendTest(const AosTestGenPtr &test);

private:
};

#endif

