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
// Created: 2013/09/23 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_JQLStatement_Ptrs_h
#define Omn_JQLStatement_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(AosJqlStatement, AosJqlStatementPtr)
OmnPtrDecl(AosJqlSelect, AosJqlSelectPtr)
OmnPtrDecl(AosJqlSelectTable, AosJqlSelectTablePtr)
OmnPtrDecl(AosJqlSelectInto, AosJqlSelectIntoPtr)
OmnPtrDecl(AosJqlSingleTable, AosJqlSingleTablePtr)
OmnPtrDecl(AosJqlSubQueryTable, AosJqlSubQueryTablePtr)
OmnPtrDecl(AosJqlJoinTable, AosJqlJoinTablePtr)
OmnPtrDecl(AosJqlWhere, AosJqlWherePtr)
OmnPtrDecl(AosJqlGroupBy, AosJqlGroupByPtr)
OmnPtrDecl(AosJqlHaving, AosJqlHavingPtr)
OmnPtrDecl(AosJqlOrderBy, AosJqlOrderByPtr)
OmnPtrDecl(AosJqlLimit, AosJqlLimitPtr)
OmnPtrDecl(AosJqlColumn, AosJqlColumnPtr)

OmnPtrDecl(AosJqlTableReference, AosJqlTableReferencePtr)
OmnPtrDecl(AosJqlTableFactor, AosJqlTableFactorPtr)
OmnPtrDecl(AosJqlSelectField, AosJqlSelectFieldPtr)
OmnPtrDecl(AosJqlOrderByField, AosJqlOrderByFieldPtr)
OmnPtrDecl(AosJqlQueryWhereCond, AosJqlQueryWhereCondPtr)
OmnPtrDecl(AosJqlQueryWhereConds, AosJqlQueryWhereCondsPtr)

OmnPtrDecl(AosJqlTableVirtulFieldDef, AosJqlTableVirtulFieldDefPtr)
OmnPtrDecl(AosJqlStmtUnion, AosJqlStmtUnionPtr)

OmnPtrDecl(AosJqlStmtRunScriptFile, AosJqlStmtRunScriptFilePtr)
OmnPtrDecl(AosJqlRecordPicker, AosJqlRecordPickerPtr)
OmnPtrDecl(AosJQLDataFieldTypeInfo, AosJQLDataFieldTypeInfoPtr)
OmnPtrDecl(AosJqlStmtAssignment, AosJqlStmtAssignmentPtr)
OmnPtrDecl(AosJqlStmtDataField, AosJqlStmtDataFieldPtr)

OmnPtrDecl(AosJqlStmtQuery, AosJqlStmtQueryPtr)
OmnPtrDecl(AosJqlStmtMap, AosJqlStmtMapPtr)

#endif
