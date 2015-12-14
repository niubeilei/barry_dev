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
// Modification History:
// 2015/02/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryNormalizer_QcTermSubquerySelected_h
#define Aos_QueryNormalizer_QcTermSubquerySelected_h

#include "QueryNormalizer/QcTerm.h"

class AosQcTermSubquerySelected : public AosQcTerm
{
private:
	AosJqlSelectPtr		mSubquery;
	AosJqlSelectPtr		mParentQuery;
	
public:
	AosQcTermSubquerySelected();
	~AosQcTermSubquerySelected();
};
#endif

