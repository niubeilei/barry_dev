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
// 04/29/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlInterface_XmlRc_h
#define Aos_XmlInterface_XmlRc_h

#include "Util/String.h"

enum AosXmlRc
{
	eAosXmlInt_Ok = 200,

	eAosXmlInt_InternalError 		= 450,
	eAosXmlInt_General      		= 451,
	eAosXmlInt_SyntaxError			= 452,
	eAosXmlInt_IncorrectOperation	= 453,
	eAosXmlInt_IncorrectObjType		= 454,
	eAosXmlInt_ObjectExist			= 455,
	eAosXmlInt_ObjectNotFound		= 456,
	eAosXmlInt_DbError				= 457,
	eAosXmlInt_FileNotFound			= 458,
	eAosXmlInt_FailedOpenFile		= 459,
	eAosXmlInt_Denied				= 460,
	// eAosXmlInt_ObjidChanged			= 461,	// Chen Ding, 2013/07/27
	eAosXmlInt_ObjidNotUnique		= 462,
	eAosXmlInt_ContainerNotExist	= 463,
	eAosXmlInt_MissingDocid			= 464,
	eAosXmlInt_UnrecOperation		= 465,	// Unrecognized operation
	eAosXmlInt_NotLoggedIn			= 466,	// Not logged in
	eAosXmlInt_Relogin				= 467,	// Need to relogin
	eAosXmlInt_ResetSession			= 468,	// Need to reset session
	eAosXmlInt_ReloginAgain			= 469,	// Relogin again
	eAosXmlInt_Vote                 = 470,  // Vote
	eAosXmlInt_Kickout              = 471,  // Vote
	eAosXmlInt_AccessDenied			= 472,	// User not defined, 01/05/2011, ken 
	eAosXmlInt_MissingObjid			= 473,	// 01/29/2011, Chen ding 

};

extern OmnString AosXmlInt_GetErrmsg(const AosXmlRc code);

#endif

