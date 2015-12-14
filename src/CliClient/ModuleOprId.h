////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ModuleOprId.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliDeamon_ModuleOprId_h
#define Omn_CliDeamon_ModuleOprId_h

#include "Util/String.h"

class AosModuleOprId
{
public:
	static OmnString	ePMIStartModule; 
	static OmnString	ePMIStopModule ; 
	static OmnString	ePMIResponseOn ; 
	static OmnString	ePMIResponseOff; 
	static OmnString	eOCSPStartModule; 
	static OmnString	eOCSPStopModule ; 
	static OmnString	eOCSPResponseOn ; 
	static OmnString	eOCSPResponseOff; 
	static OmnString	eCRLChangeList; 

	static OmnString 	eAttackMgrAddAttacker; 
	static OmnString 	eAttackMgrRemoveAttacker;
                              
	static OmnString 	eBouncerMgrAddBouncer;
	static OmnString 	eBouncerMgrRemoveBouncer;
	static OmnString 	eBouncerMgrSetPinHoleAttack;
	static OmnString 	eBouncerMgrSetBandWidth;
	static OmnString 	eBouncerMgrGetBandwidth;
	static OmnString 	eBouncerMgrResetStat;
};                             
#endif

