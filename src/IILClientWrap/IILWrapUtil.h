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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
//	All documents are stored in 'mTransFilename' + seqno. There is 
//	a Document Index, that is stored in files 'mTransFilename' + 
//		'Idx_' + seqno
//
// Modification History:
// 05/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILClientWrap_IILWrapUtil_h
#define AOS_IILClientWrap_IILWrapUtil_h

#include "Util/HashUtil.h"
#include <hash_map>

using namespace std;

typedef hash_map<u64, u32, AosU64Hash1> AosIILProcMapType;
#endif
