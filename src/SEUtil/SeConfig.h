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
// 	Created: 10/18/2010 Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_SeConfig_h
#define Omn_SEUtil_SeConfig_h

//const u64 eAosMaxFileSize = (u64)1000000000 * (u64)100;		// 100G
const u64 eAosMaxFileSize = (u64)1000000000 * 8;		// 8G
const u32 eAosMinDocFiles = 1000;
const u32 eAosMinHeaderFiles = 100;

const u32 eAosSizePerRead = 1000000;      // 1M		Ketty 2012/09/13
const u32 eAosMaxServerId = 2000;		// Ketty 2012/10/18 
const u32 eAosMaxVirtualId = 50000;		// Ketty 2012/10/18
const u32 eAosMaxProcId = 2000;			// Ketty 2013/07/17

const u32 eAosHeaderNumPerRead = 200;		// Ketty 2013/01/19 for docFileMgr

#endif
