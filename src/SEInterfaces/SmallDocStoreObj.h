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
// 2013/10/25 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SmallDocStoreObj_h
#define Aos_SEInterfaces_SmallDocStoreObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/MemoryChecker.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/TransId.h"


class AosSmallDocStoreObj : virtual public OmnRCObject
{
	 static AosSmallDocStoreObjPtr  SmallDocStore;
		  
public:
     static void setSmallDocStoreObj(const AosSmallDocStoreObjPtr &d) {SmallDocStore = d;}
     static AosSmallDocStoreObjPtr getSmallDocStoreObj() {return SmallDocStore;}            
public:
	virtual bool retrieveDoc(
					const AosRundataPtr &rdata, 
					const u64 docid,
					AosDataRecordObjPtr &doc) = 0;
	virtual AosBuffPtr   retrieveDoc(                  
			        const AosRundataPtr &rdata,
					const u64 &docid, 
					const u32 &entryNums) = 0;

	virtual bool addDocs(
					const u64 docid,
					const u32 entryNums,
					const AosBuffPtr &contents,
					const AosTransId &trans_id,
					const AosRundataPtr &rdata) = 0;

	virtual bool ageOutDocs(
					const AosRundataPtr &rdata,
					const u64 docid,
					const AosTransId &trans_id) = 0;

};
#endif

