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
// 2013/05/29: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApi.h"
#include "SEInterfaces/Jimo.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"

bool AosJimoExampleTester()
{
	// Run the DLL
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	rdata->setInterfaceId(AosJimo::eRunWithSmartDoc);

	OmnString ss = "<xml name=\"Chen Ding\"/>";
	AosXmlTagPtr sdoc = AosStr2Xml(rdata, ss AosMemoryCheckerArgs);

	AosJimoPtr jimo = AosCreateJimo(rdata, "zkyjimo_example", sdoc, "");
	aos_assert_r(jimo, false);
	OmnScreen << "Jimo created" << endl;

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = jimo->serializeTo(rdata, buff);
	aos_assert_r(rslt, false);

	rslt = jimo->serializeFrom(rdata, buff);
	aos_assert_r(rslt, false);

	rslt = jimo->run(rdata, sdoc);
	aos_assert_r(rslt, false);
	return true;
}

