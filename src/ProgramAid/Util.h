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
// 3/20/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ProgramAid_Util_h
#define Aos_ProgramAid_Util_h

#include "Util/String.h"


extern bool AosCreatePtrFile(
				 const OmnString &projPrefix,
				 const OmnString &dirname,
				 const OmnString &filename);
extern bool AosAddSmartPtr(
				const OmnString &projPrefix, 
				const OmnString &srcDirName, 
				const OmnString &className, 
				const OmnString &filename);

extern OmnString AosProgramAid_getAosHomeDir();
extern bool AosProgramAid_dataTypeSupported(const OmnString &type);
#endif

