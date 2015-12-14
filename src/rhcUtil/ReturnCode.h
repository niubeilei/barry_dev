////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ReturnCode.h
// Description:
//   
//
// Modification History:
// 			Created by CHK 2007-03-09
////////////////////////////////////////////////////////////////////////////

#ifndef aos_rhcUtil_ReturnCode_h
#define aos_rhcUtil_ReturnCode_h

#include "aos/aosReturnCode.h"
#include <linux/errno.h>

enum
{
	// By CHK 2007-02-28
	// Description: Linux defined error numbers 
	//				Values' definition can be checked by command "man errno"
	// Begin
	eAosRc_Linux_EINVAL = EINVAL, /* 22 */
	eAosRc_Linux_ESRCH  = ESRCH,  /* 3 */
	eAosRc_Linux_EPERM  = EPERM,  /* 1 */
	eAosRc_Linux_EACCES = EACCES, /* 13 */
	// End

	eAosRc_ResMgrUnknownError = eAosRc_ResMgrStart+1,
	eAosRc_ResMgrNotStarted, 
	
	eAosRc_ResMgrLoadConfigFail,
	eAosRc_ResMgrSaveConfigFail,
	eAosRc_ResMgrFileNotFound,
	eAosRc_ResMgrFileFailRead,
	eRosRc_ResMgrDiskNotFound,
	eRosRc_ResMgrPartitionNotFound,

	eRosRc_ResMgrIntervalOutOfRange,
	eRosRc_ResMgrDurationOutOfRange,
	eRosRc_ResMgrIllegalRecordTime,
	eRosRc_ResMgrStartTimeIllegal, 		/* The input start_time value is out of record limitation of our resource manager. */
	eRosRc_ResMgrEndTimeIllegal, 		/* The input end_time value is out of record limitation of our resource manager. */
	eRosRc_ResMgrStatisticIntervalTime, /* We have a record limitation for any module of resource manager. The ratio value of the interval and the duration is out of limitation of our resource manager. */
	eAosRc_ResMgrInputInvalid, 			/* Input is invalid */
	eAosRc_ResMgrRcdNotFound, 			/* Not found the status record in our data base */
	eAosRc_ResMgrDataOutOfBuffer, 		/* The retrieved record data is out of buffer size */
	
	eAosRc_ResMgrInvalidBusyThreshold, 
	eAosRc_ResMgrInvalidNormalThreshold, 

	eAosRc_DiskMgrTypeExist, 			/* If the file type already exists in disk manager¡¯s record, an error is reported. */
	eAosRc_DiskMgrTypeNotExist, 		/* If the file type does not exists in disk manager¡¯s record, an error is reported. */
	eAosRc_ResMgrSizeInsuffic, 			/* The caller input array size is not sufficient for the output values. */
	eAosRc_HardDiskNotFound, 			/* The specified hard disk not found. */
};

#endif // aos_rhcUtil_ReturnCode_h
