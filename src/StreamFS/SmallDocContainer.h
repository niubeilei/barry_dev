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
// Each record is stored in the following format:
//
// Modification History:
// 2013/10/25 Created by CDing
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StreamFS_SmallDocContainer_h
#define AOS_StreamFS_SmallDocContainer_h

#include "DfmUtil/DfmConfig.h"
#include "DfmUtil/DfmDocDatalet.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/TransId.h"
#include "XmlUtil/DocTypes.h"

class AosSmallDocContainer : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
public:
	AosSmallDocContainer() {}
	~AosSmallDocContainer() {}

	bool 		addDocs(
					const u64 &docid,
					const AosBuffPtr &contents,
					const AosTransId &trans_id,
					const AosRundataPtr &rdata);
	bool 		retrieveDoc(
					const AosRundataPtr &rdata,
					const u64 docid,
					AosDataRecordObjPtr &doc);
	bool		ageOutDocs(
					const AosRundataPtr &rdata,
					const u64 group_id,
					const AosTransId &trans_id);

public:
	AosBuffPtr 	retrieveDoc(
					const AosRundataPtr &rdata,
					const u64 &docid, 
					const u32 &entryNums);

private:

	u64 		getRealDocid(
					const u64 smallDocid, 
					const u32 entryNums);

	u32 		getSmallDocIdx(
					const u64 smallDoicid, 
					const u32 entryNums);

};
#endif
