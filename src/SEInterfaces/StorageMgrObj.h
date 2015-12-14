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
// 01/20/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SEInterfaces_StorageMgrObj_h
#define Aos_SEInterfaces_StorageMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/Docid.h"
#include "StorageMgrUtil/Ptrs.h"
#include "TransUtil/ModuleId.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosStorageMgrObj : virtual public OmnRCObject
{
private:
	static AosStorageMgrObjPtr 	smStorageMgr;

public:
	// virtual u64 createVirtualDir(
	// 					const OmnString &name,
	// 					const AosRundataPtr &rdata) = 0;

	// Chen Ding, 12/10/2012
	virtual AosDocFileMgrObjPtr createDocFileMgr(
				const u32 virtual_id,
			    const AosModuleId::E module_id,
				const OmnString &prefix,
				const AosDevLocationPtr &location,
				const AosRundataPtr &rdata) = 0;
	
	virtual OmnFilePtr openFileByKey1(
					const int virtual_id,
					const OmnString &str_key,
					const bool create_flag,
					const AosRundataPtr &rdata AosMemoryCheckDecl) = 0;

	// Chen Ding, 2013/01/09
	virtual AosDocFileMgrObjPtr retrieveDocFileMgr(
				const u32	virtual_id,
				const AosModuleId::E module_id,
				const AosRundataPtr &rdata) = 0;

	static AosStorageMgrObjPtr getStorageMgr() {return smStorageMgr;}
	static void setStorageMgr(const AosStorageMgrObjPtr &o) {smStorageMgr = o;}
};

/* Moved to API. Chen Ding, 12/10/2012
inline u64 AosCreateVirtualDir(
		const OmnString &name, 
		const AosRundataPtr &rdata)
{
	AosStorageMgrObjPtr storagemgr = AosStorageMgrObj::getStorageMgr();
	aos_assert_r(storagemgr, 0);
	return storagemgr->createVirtualDir(name, rdata);
}

inline OmnString AosGetFileName(
		const u64 &virtual_dir,
		const u32 seqno,
		const OmnString &prefix,
		const AosRundataPtr &rdata)
{
	AosStorageMgrObjPtr storagemgr = AosStorageMgrObj::getStorageMgr();
	aos_assert_r(storagemgr, 0);
	return storagemgr->getFileName(virtual_dir, seqno, prefix, rdata);
}

inline OmnFilePtr AosStorageMgrOpenSysFile(
		const OmnString &filekey, 
		const bool create_flag, 
		const AosRundataPtr &rdata)
{
	AosStorageMgrObjPtr storagemgr = AosStorageMgrObj::getStorageMgr();
	aos_assert_r(storagemgr, 0);
	return storagemgr->openSysFile(filekey, create_flag, rdata);
}
*/

#endif
#endif
