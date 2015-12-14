////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenSeqno/AllDataGenSeqno.h"

#include "DataGenSeqno/DataGenArabicSeqno.h"
#include "DataGenSeqno/DataGenCharSeqno.h"
#include "DataGenSeqno/DataGenRomanSeqno.h"
#include "DataGenSeqno/DataGenHeavenlyStems.h"
#include "DataGenSeqno/DataGenEarthlyBranches.h"
#include "DataGenSeqno/DataGenChineseSeqno.h"

AosDataGenSeqnoPtr   sgSeqno[AosDataGenSeqno::eMax+1];
AosStr2U32_t   AosDataGenSeqnoType::smNameMap;

AosAllDataGenSeqno  gAosAllDataGenSeqno;

AosAllDataGenSeqno::AosAllDataGenSeqno()
{
	static AosDataGenArabicSeqno		sgDataGenArabicSeqno(true);
	static AosDataGenCharSeqno			sgDataGenCharSeqno(true);
	static AosDataGenRomanSeqno			sgDataGenRomanSeqno(true);
	static AosDataGenHeavenlyStems		sgDataGenHeavenlyStems(true);
	static AosDataGenEarthlyBranches	sgDataGenEarthlyBranches(true);
	static AosDataGenChineseSeqno		sgDataGenChineseSeqno(true);
}

