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
// 11/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/AllSelectors.h"

#include "DocSelector/SelCreatedDoc.h"
#include "DocSelector/SelDocid.h"
#include "DocSelector/SelLocalVar.h"
#include "DocSelector/SelObjid.h"
#include "DocSelector/SelRetrievedDoc.h"
#include "DocSelector/SelSourceDoc.h"
#include "DocSelector/SelTargetDoc.h"
#include "DocSelector/SelReceivedDoc.h"
#include "DocSelector/SelCloudid.h"
#include "DocSelector/SelWorkingDoc.h"
#include "DocSelector/SelRequesterAcct.h"
#include "DocSelector/SelMySQLRecord.h"

AosStr2U32_t    AosDocSelectorType::smNameMap;
AosDocSelectorPtr	sgSelectors[AosDocSelector::eMax+1];
AosAllSelectors sgAllSelector;

AosAllSelectors::AosAllSelectors()
{
static AosDocSelCreatedDoc 		sgSelCreateDoc(true);
static AosDocSelCloudid	  		sgSelCloudid(true);
static AosDocSelLocalVar  		sgSelLocalVar(true);
static AosDocSelObjid   		sgSelObjid(true);
static AosDocSelDocid			sgSelDocid(true);
static AosDocSelRetrievedDoc	sgSelRetrievedDoc(true);
static AosDocSelReceivedDoc     sgSelReceivedDoc(true);
static AosDocSelSourceDoc 		sgSelSourceDoc(true);
static AosDocSelTargetDoc		sgSelTargetDoc(true);
static AosDocSelWorkingDoc		sgSelWorkingDoc(true);
static AosDocSelRequesterAcct	sgSelRequesterAcct(true);
static AosSelMySQLRecord		sgSelMySQLRecord(true);
}

