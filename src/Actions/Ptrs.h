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
// 	03/03/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_Ptrs_h
#define	Aos_Actions_Ptrs_h

#include "Util/SPtr.h"

// OmnPtrDecl(AosSdocAction, AosSdocActionPtr)
OmnPtrDecl(AosActImportDocFixed, AosActImportDocFixedPtr)
OmnPtrDecl(AosActImportDocCSV, AosActImportDocCSVPtr)
OmnPtrDecl(AosActMergeFile, AosActMergeFilePtr)
OmnPtrDecl(AosActIILBatchOpr, AosActIILBatchOprPtr)
OmnPtrDecl(AosActIILPatternOpr, AosActIILPatternOprPtr)
OmnPtrDecl(AosActIILPattern2Opr, AosActIILPattern2OprPtr)
OmnPtrDecl(AosActRebuildBitmapOpr, AosActRebuildBitmapOprPtr)
OmnPtrDecl(AosActJimoTableBatchOpr, AosActJimoTableBatchOprPtr)

#endif
