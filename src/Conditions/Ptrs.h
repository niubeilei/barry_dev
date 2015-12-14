////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 12/02/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conditions_Ptrs_h
#define Aos_Conditions_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(AosCondition, AosConditionPtr);
OmnPtrDecl(AosCondInField, AosCondInTablePtr);
OmnPtrDecl(AosCondNotInField, AosCondNotInFieldPtr);
OmnPtrDecl(AosCondEqual, AosCondEqualPtr);
OmnPtrDecl(AosCondNotEqual, AosCondNotEqualPtr);
OmnPtrDecl(AosCondGreater, AosCondGreaterPtr);
OmnPtrDecl(AosCondGreaterEqual, AosCondGreaterEqualPtr);
OmnPtrDecl(AosCondLess, AosCondLessPtr);
OmnPtrDecl(AosCondLessEqual, AosCondLessEqualPtr);
OmnPtrDecl(AosCondInRecord, AosCondInRecordPtr);
OmnPtrDecl(AosCondNotInRecord, AosCondNotInRecordPtr);
OmnPtrDecl(AosExeData, AosExeDataPtr);

OmnPtrDecl(AosCondData, AosCondDataPtr);

#endif
