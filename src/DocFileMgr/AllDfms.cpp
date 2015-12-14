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
// 04/10/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/AllDfms.h"

#include "DocFileMgr/DfmHeaderFile.h"
#include "DocFileMgr/DfmDiskFile.h"
#include "DocFileMgr/DfmSlabFile.h"
#include "DocFileMgr/DfmReadOnlyFile.h"

#include "Thread/Mutex.h"

//AosDocFileMgrPtr	sgDfms[AosDfmType::eMax];
//AosAllDfms 	   gAosAllDfms;

//AosAllDfms::AosAllDfms()
//{
////static AosSnapShotDfm	sgSnapShotDfm(true);
//static AosNormDfm		sgNormDfm(true);
//
//}


//--------------DfmFiles-----------
AosDfmFilePtr	sgDfmFiles[AosDfmFileType::eMax];
AosAllDfmFiles gAosAllDfmFiles;

AosAllDfmFiles::AosAllDfmFiles()
{
static AosDfmHeaderFile		sgHeaderFile(true);
static AosDfmDiskFile		sgDiskFile(true);
static AosDfmSlabFile		sgSlabFile(true);
static AosDfmReadOnlyFile	sgReadOnlyFile(true);

}



