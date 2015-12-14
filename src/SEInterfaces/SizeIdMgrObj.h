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
// 03/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SizeIdMgrObj_h
#define Aos_SEInterfaces_SizeIdMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosSizeIdMgrObj : virtual public OmnRCObject
{
private:
	static AosSizeIdMgrObjPtr smSizeIdMgr;

public:
	virtual u64		createSizeId(
						const int size,
						const AosXmlTagPtr &record,
						const AosRundataPtr &rdata) = 0;

	virtual u64		createSizeId(
						const u64 &record_docid,
						const AosRundataPtr &rdata) = 0;
	virtual u64 	getCrtSizeId(
						const u64 &record_docid,
						const AosRundataPtr &rdata) = 0;
	virtual u64		getNextDocid(
						const u64 &record_docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool	getDocids(
						const u64 &sizeid,
						u64 &start_docid,
						int &num_docids,
						bool &overflow,
						const AosRundataPtr &rdata) = 0;
	
	virtual u64		getSizeIdByDocid(const u64 &docid) = 0;
	virtual u64		getLocalIdByDocid(const u64 &docid) = 0;

	virtual u64		getDataRecordDocidBySizeId(
						const u32 siteid,
						const u64 &sizeid,
						const AosRundataPtr &rdata) = 0;

	virtual AosDataRecordObjPtr	getDataRecordBySizeId(
						const u32 siteid,
						const u64 &sizeid,
						const AosRundataPtr &rdata) = 0;
	
	virtual AosDataRecordObjPtr	getDataRecordByDocid(
						const u32 siteid,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;
	
	virtual int		getDataRecordLenBySizeId(
						const u32 siteid,
						const u64 &sizeid,
						const AosRundataPtr &rdata) = 0;

	virtual int		getDataRecordLenByDocid(
						const u32 siteid,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;


	virtual bool	removeDataRecordBySizeId(
						const u32 siteid,
						const u64 &sizeid,
						const bool flag,
						const AosRundataPtr &rdata) = 0;

	static AosSizeIdMgrObjPtr getSizeIdMgr() {return smSizeIdMgr;}
	static void setSizeIdMgr(const AosSizeIdMgrObjPtr &d) {smSizeIdMgr = d;}
};

#endif

