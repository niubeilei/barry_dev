////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Uid.h
// Description:
//	This class defines for check whether a user(with password) is the system user.
//	Now we implement the linux part.
// 
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_UserCheck_h
#define Omn_Porting_UserCheck_h



#ifdef OMN_PLATFORM_UNIX

bool	OmnCheckSystemUser(char* username,char* password);




#endif
#endif
