////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AmConfig.h
// Description:
//   
//
// Modification History:
// 2007-04-15 Created by CHK
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef aos_AccessManager_AmConfig_h
#define aos_AccessManager_AmConfig_h

#include "aosUtil/Types.h"

//=================================================================
// Simple for API begin
//=================================================================
// Create the structure of the database
// return 0  for successful
// return <0 for fail for some reason, which is represented by returned code
int aos_am_recover_db_structure();

////////////////////////////////////////////////////////
// User 
////////////////////////////////////////////////////////

// return 0  for successful
// return <0 for fail for some reason, which is represented by returned code
int aos_am_insert_usr_info(char* UserName);

// we can set insert and update of the user information together
int aos_am_update_usr_info(u32 UserId, char* UserName);

// return 0  for successful
// return <0 for fail for some reason, which is represented by returned code
int aos_am_delete_usr_info(u32 UserId);

// return 0  for successful
// return <0 for fail for some reason, which is represented by returned code
int aos_am_delete_usr_info(char* UserName);


/////////////////////////////////////////////////////////
// Application
/////////////////////////////////////////////////////////
int aos_am_insert_app_info(char* AppName, int version);

int aos_am_update_app_info(u32 AppId);



/////////////////////////////////////////////////////////
// Operation
/////////////////////////////////////////////////////////


//=================================================================
// Simple for API end
//=================================================================

#endif //  aos_AccessManager_AmConfig_h
#endif
