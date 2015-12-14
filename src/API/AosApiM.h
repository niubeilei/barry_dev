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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiM_h
#define AOS_API_ApiM_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/DocClientObj.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

extern bool AosModifyDocAttrStr(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc, 
		const OmnString &aname, 
		const OmnString &value, 
		const bool value_unique = false, 
		const bool docid_unique = false);


extern bool AosModifyDocAttrStr(
		const AosRundataPtr &rdata,
		const u64 &docid, 
		const OmnString &aname, 
		const OmnString &value, 
		const bool value_unique = false, 
		const bool docid_unique = false);


extern bool AosModifyDocAttrU64(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc, 
		const OmnString &aname, 
		const u64 value, 
		const bool value_unique = false, 
		const bool docid_unique = false);

extern bool AosModifyDoc(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata);

extern int AosMaxFilesPerDir();

extern bool AosModifyDocAttrByWorker(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc);

//extern bool AosModifyBinaryDoc(
//				const AosXmlTagPtr &doc,
//				const AosBuffPtr &buff, 
//				AosRundata *rdata);

extern bool AosModifyIndexEntry(
				AosRundata *rdata,
				const AosXmlTagPtr &def);
#endif
