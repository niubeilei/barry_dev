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
// 03/22/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
#if 0
#include "DocServer/ProcBatchVariable.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"



AosProcBatchVariable::AosProcBatchVariable(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_BATCHVARIABLE, AosDocSvrProcId::eBatchVariable, regflag)
{
}


AosProcBatchVariable::~AosProcBatchVariable()
{
}


bool
AosProcBatchVariable::proc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &request, 
		const AosBigTransPtr &trans)
{
	// The request is in the form:
	// 	<trans AOSTAG_NUM_DOCS="xxx">
	// 		<docids><![BDATA[xxx]]></docids>
	// 		<buffs><![BDATA[xxx]]></buffs>
	// 	</trans>
	OmnNotImplementedYet;
	/*
	int num_docs = request->getAttrInt(AOSTAG_NUM_DOCS, -1);
	if (num_docs <= 0)
	{
		AosSetErrorU(rdata, "invalid_num_docs") << ": " << num_docs;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve docids
	AosXmlTagPtr tag = request->getFirstChild("docids");
	aos_assert_rr(tag, rdata, false);	
	u64 *docids = 0;
	int len = tag->getNodeTextBinary((char **)&docids);
	aos_assert_r(len > 0, false);

	// Retrieve offsets 
	tag = request->getFirstChild("offsets");
	aos_assert_rr(tag, rdata, false);	
	u32 *offsets = 0;
	len = tag->getNodeTextBinary((char **)&offsets);
	aos_assert_r(len > 0, false);

	// Retrieve buffs
	tag = request->getFirstChild("buffs");
	aos_assert_rr(tag, rdata, false);	
	AosBuffPtr buff = tag->getNodeTextBinary();
	aos_assert_rr(buff, rdata, false);

	rdata->setOk();
	*/
	return true;
}


AosDocSvrProcPtr
AosProcBatchVariable::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcBatchVariable(false);
	return proc;
}
#endif
