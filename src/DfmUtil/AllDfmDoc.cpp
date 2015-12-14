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
// 04/10/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DfmUtil/AllDfmDoc.h"

#include "UtilData/DfmDocType.h"
#include "DfmUtil/DfmDocBitmap.h"
#include "DfmUtil/DfmDocNorm.h"
#include "DfmUtil/DfmDocDatalet.h"
#include "DfmUtil/DfmDocIIL.h"
#include "DfmUtil/DfmGroupedDoc.h"


AosDfmDocPtr	sgDfmDocs[AosDfmDocType::eMax];
AosAllDfmDoc 	gAosAllDfmDoc;

AosAllDfmDoc::AosAllDfmDoc()
{
static AosDfmDocBitmap	sgDfmDocBitmap(true);
static AosDfmDocNorm 	sgDfmDocNorm(true);
static AosDfmDocDatalet	sgDfmDocDatalet(true);
static AosDfmDocIIL		sgDfmDocIIL(true);
static AosDfmGroupedDoc	sgDfmGroupedDoc(true);
}

