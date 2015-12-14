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
// 02/10/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_CliTorturer_CliCmdGeneric_h
#define Omn_CliTorturer_CliCmdGeneric_h

#include "CliTorturer/CliCmdTorturer.h"
#include "CliTorturer/Ptrs.h"
#include "Parms/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Ptrs.h"
#include "Util/DynArray.h"



class AosCliCmdGeneric : public AosCliCmdTorturer
{
	OmnDefineRCObject;

public:

private:
	OmnString	mCmdPrefix;

public:
	AosCliCmdGeneric(
					const AosCliTorturerLogType logType, 
					const AosCliTorturerPtr &cliTorturer);
	virtual ~AosCliCmdGeneric();

	bool 	config(const OmnXmlItemPtr &def, const AosCliTorturerPtr &tort);
	virtual bool	run(bool &cmdGood, 
						const bool correctOnly,
						const OmnTestMgrPtr &tm, 
						bool &ignore);
	virtual bool 	check();

	OmnString	toString() const;
};

#endif

