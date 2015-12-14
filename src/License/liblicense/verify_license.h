////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: verify_license.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef VERIFY_LICENSE_PUBLISH_HEAD_H
#define VERIFY_LICENSE_PUBLISH_HEAD_H

#define LICENSE_PATH "/usr/local/AOS/Config/License.txt"
/*
 *	Verify license correct.
 *	Return value 1 is correct, 0 is false.
*/
extern int verify_license(void);

#endif
