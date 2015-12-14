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
// 05/26/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_CliTorturer_ApiTorturer_h
#define Omn_CliTorturer_ApiTorturer_h

#include "CliTorturer/CliCmdTorturer.h"
#include "CliTorturer/Ptrs.h"
#include "Tester/TestMgr.h"
#include "Parms/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"


class AosApiTorturer : public AosCliCmdTorturer
{
	OmnDefineRCObject;

public:

protected:
//	u32				mSelectFromTablePct;
	OmnString		mFuncName;
	OmnString		mFuncRet;			// Function return value
	bool			mExecutionFailed;	// Indicates whether the execution failed
	OmnString		mExecutionErrmsg;	

	static OmnDynArray<AosApiTorturerPtr>	mApiTorturers;

public:
	AosApiTorturer(const OmnString &name);
	AosApiTorturer(const AosCliTorturerLogType logType, 
				   const AosCliTorturerPtr &cliTorturer);
	virtual ~AosApiTorturer();

	virtual bool	config(const OmnXmlItemPtr &def, 
						   const AosCliTorturerPtr &torturer);
	virtual bool	run(bool &cmdGood, 
						const bool correctOnly,
						const OmnTestMgrPtr &tm, 
						bool &ignore);
	virtual bool 	check();
	virtual bool	runFunc(const bool correctOnly, 
							const AosCliTorturerPtr &torturer,
							const OmnTestMgrPtr &testMgr, 
							bool &cmdCorrect, 
							OmnString &errmsg, 
							bool &cmdExecuted) = 0;
	virtual AosApiTorturerPtr	createInstance(
					const OmnXmlItemPtr &def,
					const AosCliTorturerPtr &cliTorturer) = 0; 
	virtual bool	checkCmdConditions(
							bool &cmdGood, 
							AosRVGReturnCode &rcode, 
							OmnString &errmsg, 
							const bool beforeExecution);
					

	static AosApiTorturerPtr createTorturer(
					const OmnXmlItemPtr &def, 
					const AosCliTorturerLogType logType, 
					const AosCliTorturerPtr &cliTorturer); 

	OmnString	toString() const;
	static AosApiTorturerPtr	getTorturer(const OmnString &name);

	static bool	addApiTorturer(const AosApiTorturerPtr &t);
};

#endif

