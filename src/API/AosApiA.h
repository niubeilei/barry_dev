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
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiA_h
#define AOS_API_ApiA_h

#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "TransUtil/Ptrs.h"

extern bool	AosAddU64ValueDoc(
				const u64 &iilid, 
				const u64 &key, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				AosRundata *rdata);

extern bool	AosAddU64ValueDocToTable(
				const u64 &iilid, 
				const u64 &key, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);

extern bool AosAddContainerLog(
				const OmnString &container_objid,
				const OmnString &docstr,
				const AosRundataPtr &rdata);

extern bool AosAddLog(
				const AosRundataPtr &rdata, 
				const OmnString &container_objid,
				const OmnString &logname,
				const OmnString &docstr);

extern bool AosAddStrValueDoc(
				//vector<AosBuffPtr> &buffs,
				vector<AosTransPtr>  *allTrans,
				const u32 arr_len,
				const OmnString &iilname, 
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				int &physical_id,
				AosRundata *rdata);

extern bool AosAddStrValueDoc(
				const OmnString &iilname,
				const OmnString &value,
				const u64 &docid,
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata);
extern bool AosAppendBinaryDoc(
				const OmnString &objid, 
				const AosBuffPtr &buff, 
				AosRundata *rdata);

// Chen Ding, 2013/01/13
extern void AosAddXmlBinaryData(
				OmnString &str, 
				const OmnString &tagname, 
				const char *data, 
				const int len);

// Chen Ding, 2013/01/13
extern void AosAddXmlBinaryData(
				OmnString &str, 
				const OmnString &tagname, 
				const AosBuffPtr &buff);

// JACKIE-HADOOP
extern bool AosAppendBlockToHadoop(
        AosRundata *rdata,
        const OmnString &fname,
        AosBuffPtr &buff,
        const int64_t bytes_to_read);
extern bool AosAddVector2D(
		const AosRundataPtr &rdata,
		const u64 &sdocid,
		const OmnString &key_str,
		const AosValueRslt &time_rslt,
		const vector<AosValueRslt> &measures);
#endif
