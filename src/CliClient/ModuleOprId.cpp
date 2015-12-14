////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ModuleOprId.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliClient/ModuleOprId.h"
#include "Util/String.h"

OmnString AosModuleOprId::ePMIStartModule = "pmiserver-start-module";
OmnString AosModuleOprId::ePMIStopModule  = "pmiserver-stop-module";
OmnString AosModuleOprId::ePMIResponseOn  = "pmiserver-resp-on";
OmnString AosModuleOprId::ePMIResponseOff = "pmiserver-resp-off";

OmnString AosModuleOprId::eOCSPStartModule = "ocspserver-start-module";
OmnString AosModuleOprId::eOCSPStopModule  = "ocspserver-stop-module";
OmnString AosModuleOprId::eOCSPResponseOn  = "ocspserver-resp-on";
OmnString AosModuleOprId::eOCSPResponseOff = "ocspserver-resp-off";
OmnString AosModuleOprId::eCRLChangeList = "crlserver-change-list";

OmnString AosModuleOprId::eAttackMgrAddAttacker = "attackmgr-add-attacker";
OmnString AosModuleOprId::eAttackMgrRemoveAttacker = "attackmgr-remove-attacker";

OmnString AosModuleOprId::eBouncerMgrAddBouncer        = "bouncermgr-add-bouncer";
OmnString AosModuleOprId::eBouncerMgrRemoveBouncer     = "bouncermgr-remove-bouncer";
OmnString AosModuleOprId::eBouncerMgrSetPinHoleAttack  = "bouncermgr-set-pinhole-attack";
OmnString AosModuleOprId::eBouncerMgrSetBandWidth      = "bouncermgr-set-bandwidth";

OmnString AosModuleOprId::eBouncerMgrGetBandwidth      = "bouncermgr-get-bandwidth";
OmnString AosModuleOprId::eBouncerMgrResetStat         = "bouncermgr-reset-stat";

