////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/19/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_SystemId_h
#define AOS_StorageMgr_SystemId_h

// min DocFileMgrId is 1,	0 is error check
// max DocFileMgrId is 99(eDocFileMgrIdStart-1)
#define AOS_DFMID_CONFIGMGR					1

// system site id.
#define AOS_SYS_SITEID						1

// temporary File  virtual id.
//#define AOS_TEMPFILE_VIRTUALID					0xffffffff	
#define AOS_TEMPFILE_VIRTUALID						0x70000000	
#endif
