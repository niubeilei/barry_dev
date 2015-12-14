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
// 06/18/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocServer_DocSvrUtil_h
#define AOS_DocServer_DocSvrUtil_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "XmlUtil/DocTypes.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "QueryUtil/QrUtil.h"

class AosDocSvrUtil: public OmnRCObject 
{
	OmnDefineRCObject;
public:
	AosDocSvrUtil();
	~AosDocSvrUtil();

	static bool isGroupedDoc(const u64 &docid)
	{
		return  AosDocType::getDocType(docid) == AosDocType::eGroupedDoc;
	}

	static bool getDocs(
				const AosRundataPtr &rdata,
				const AosBuffPtr &buff,
				const u32 num_docids,
				const AosBuffPtr &docids_buff,
				map<u64, int> &sizeid_len,
				u64 &read_end_docid,
				const u64 snpa_id);

private:
	static AosBuffPtr readDocsFromFile(
				const AosRundataPtr &rdata,
				const u64 start_docid,
				const u32 &vid,      
				int &num_docs_to_read); 

	static  bool readFixedDoc(
				const AosRundataPtr &rdata,
				const AosBuffPtr &data_buff,
				const AosBuffPtr &docids_buff, 
				const u32 num_docids,
				map<u64, int> sizeid_len,
				u32 &read_num,
				u64 &read_end_docid,
				bool &finish);

	static bool setRecord(
				const AosRundataPtr &rdata,
				const bool &hasfield,
				const vector<AosQrUtil::FieldDef> &fields,
				const AosBuffPtr &data_buff,
				const AosXmlTagPtr &doc,
				const u64 &docid,
				const u64 &docsize);
};

#endif
