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
// 2013/11/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_DiskStat_h
#define Omn_Util_DiskStat_h

#include "Debug/Debug.h"

struct AosDiskStat
{
	enum E
	{
		eOk = 0,
		eSvrDown = 1,
		eReadErr = 2,
		eUnknow = 3
	};

	int64_t 	total_opt_time;		
	int64_t 	total_bytes_read;	// The number of total bytes being read
	int64_t 	disk_bytes_read;	// The number of bytes being read from disks
	int64_t 	ssd_bytes_read;		// The number of bytes being read from ssd
	int			total_num_ios;		// The total number of I/Os involved
	int			disk_num_ios;		// The number of disk I/Os involved
	int			ssd_num_ios;		// The number of ssd I/Os involved
	char		local_read;			// 1 if it is a local read
									// 2 if it is a network read
	int			err_no;

	AosDiskStat()
	:
	total_bytes_read(0),
	disk_bytes_read(0),
	ssd_bytes_read(0),
	total_num_ios(0),
	disk_num_ios(0),
	ssd_num_ios(0),
	local_read(0),
	err_no(0)
	//server_is_down(false)
	{
	}

	bool setMetadata(const AosXmlTagPtr &doc);
	bool addStat(const AosDiskStat &stat);
	bool setServerIsDown(const bool server_down);
	bool serverIsDown() const { return err_no == eSvrDown;}
	void setError(const int i) {err_no = i;} 
	int getError() const {return err_no;} 
};

#endif
