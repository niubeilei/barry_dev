////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: verify.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef VERIFY_LICENSE_HEAD_H
#define VERIFY_LICENSE_HEAD_H

#define LICENSE_PATH "/usr/local/AOS/Config/License.txt"



int generate_local_license(unsigned char *local_license, unsigned char *feature_list);
int verify_license(void);
int resume_hd_info(unsigned char *serial_number, unsigned char *mac, unsigned char *b64_license);

#endif


