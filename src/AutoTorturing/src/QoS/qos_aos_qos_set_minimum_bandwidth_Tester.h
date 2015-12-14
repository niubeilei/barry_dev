//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This file is automatically generated by the TorturerGen facility. 
//
// Modification History:
// 7/18/2007 : Created by TorturerGen Facility
//////////////////////////////////////////////////////////////////////////
#ifndef Aos_AutoTorturing_src_QoS_qos_aos_qos_set_minimum_bandwidth_Tester_h
#define Aos_AutoTorturing_src_QoS_qos_aos_qos_set_minimum_bandwidth_Tester_h

#include "CliTorturer/Ptrs.h"
#include "CliTorturer/ApiTorturer.h"
#include "CliTorturer/Tester/Ptrs.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Torturer/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"

#include "aosApi.h"


class AosClass_aos_qos_set_minimum_bandwidth : public AosApiTorturer 
{
	OmnDefineRCObject;

private:
        u16 mArg0; 

public:
	AosClass_aos_qos_set_minimum_bandwidth();
	virtual ~AosClass_aos_qos_set_minimum_bandwidth() {}
	
	virtual AosApiTorturerPtr	createInstance(
					const OmnXmlItemPtr &def,
					const AosCliTorturerPtr &cliTorturer); 
	virtual bool	runFunc(bool &correctOnly, 
							const AosCliTorturerPtr &torturer,
							const OmnTestMgrPtr &testMgr, 
							bool &cmdCorrect);
	virtual bool    preAddToTableProc(const AosGenTablePtr &table,
	                        const AosGenRecordPtr &record,
							bool &cmdCorrect,
							OmnString &cmdErrmsg);

	static bool 	registInst();
};

#endif

