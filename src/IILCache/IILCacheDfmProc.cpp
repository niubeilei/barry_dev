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
// 2013/03/02	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILCache/IILCacheDfmProc.h"

#include "DocFileMgr/DfmDocNorm.h"

AosDfmDocPtr	AosIILCacheDfmProc::smDfmDoc = OmnNew AosDfmDocIILCache(0);

OmnString 	
AosIILCacheDfmProc::getFnamePrefix()
{ 
	return "iil_cache"; 
}


u32 		
AosIILCacheDfmProc::getDocHeaderSize()
{
	return AosDfmDocIILCache::getHeaderSize();
}


bool		
AosIILCacheDfmProc::finishTrans(const vector<u64> &total_trans, const AosRundataPtr &rdata)
{
	return true;
}


AosDfmDocPtr  
AosIILCacheDfmProc::getTempDfmDoc()
{
	return smDfmDoc->clone(0);
}


bool		
AosIILCacheDfmProc::needDeltaHeader()
{
	return false;
}


u64			
AosIILCacheDfmProc::parseLocId(const u64 loc_id, const u32 virtual_id)
{
	return loc_id;
}
#endif
