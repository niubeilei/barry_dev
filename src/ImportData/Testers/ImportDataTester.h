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
// 03/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ImportData_Testers_ImportDataTester_h
#define Aos_ImportData_Testers_ImportDataTester_h

#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "ImportData/ImportListener.h"
#include "XmlUtil/XmlTag.h"


//class AosImportDataTester : virtual public OmnTestPkg,
//							virtual public AosImportListener
class AosImportDataTester : virtual public OmnTestPkg
{
public:
	AosImportDataTester();
	~AosImportDataTester() {};

	virtual bool		start();
    virtual void importFinished(
						const bool rslt, 
						const AosXmlTagPtr &status_doc,
						const AosRundataPtr &rdata)
	{}
		
private:
	bool		basicTest();
};
#endif
