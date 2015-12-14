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
// 11/27/2007: Created by Allen Xu
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RVG_Ptrs_h
#define Aos_RVG_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(AosRecord, AosRecordPtr);
OmnPtrDecl(AosRVG, AosRVGPtr);
OmnPtrDecl(AosRSG, AosRSGPtr);
OmnPtrDecl(AosCharsetRSG, AosCharsetRSGPtr);
OmnPtrDecl(AosEnumRSG, AosEnumRSGPtr);
OmnPtrDecl(AosEnumSeqRSG, AosEnumSeqRSGPtr);
OmnPtrDecl(AosComposeRSG, AosComposeRSGPtr);
OmnPtrDecl(AosValue, AosValuePtr);
OmnPtrDecl(AosTable, AosTablePtr);
OmnPtrDecl(AosColumn, AosColumnPtr);
OmnPtrDecl(AosParm, AosParmPtr);
OmnPtrDecl(AosAndRSG, AosAndRSGPtr);
OmnPtrDecl(AosOrRSG, AosOrRSGPtr);
OmnPtrDecl(AosRVGMultiFromTable, AosRVGMultiFromTablePtr);
//
// can not declare like this, so I move it to RIG.h
//
/*OmnPtrDecl(AosRIG<u8>, AosU8RIGPtr);
OmnPtrDecl(AosRIG<u16>, AosU16RIGPtr);
OmnPtrDecl(AosRIG<u32>, AosU32RIGPtr);
OmnPtrDecl(AosRIG<int8_t>, AosInt8RIGPtr);
OmnPtrDecl(AosRIG<int16_t>, AosInt16RIGPtr);
OmnPtrDecl(AosRIG<int32_t>, AosInt32RIGPtr);*/

#endif
