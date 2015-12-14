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
// 05/06/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_ReturnCode_h
#define Omn_RandomValueGen_ReturnCode_h

#include "Util/String.h"


enum AosRVGReturnCode
{
	eAosRVGRc_Success,
	eAosRVGRc_ProgramError,
	eAosRVGRc_ExistInTable,
	eAosRVGRc_NotFoundInTable,
	eAosRVGRc_NotInRange,
	eAosRVGRc_Incorrect, 
	eAosRVGRc_TooShort,
	eAosRVGRc_TooLong,
	eAosRVGRc_NullValue,
	eAosRVGRc_FromInvalidSet,
	eAosRVGRc_SelectFromRecord,
	eAosRVGRc_SelectFromTable,
	eAosRVGRc_UsedByOtherTables,
	eAosRVGRc_NotUnique,
	eAosRVGRc_OutOfOrder,
	eAosRVGRc_TooFewValues,
	eAosRVGRc_TooManyValues,
	eAosRVGRc_NotInList,

	eAosRVGRc_LastEntry
};

extern OmnString AosRVGRc_toStr(const AosRVGReturnCode code);

#endif

