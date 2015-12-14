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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlOrderBy_H
#define AOS_JQLStatement_JqlOrderBy_H

#include "JQLStatement/JqlSelectField.h"
#include "JQLStatement/JqlDataStruct.h"
#include "JQLStatement/JqlStatement.h"

#include "SEInterfaces/ExprObj.h"      
#include "Util/String.h"

class AosJqlOrderByField : public AosJqlDataStruct
{
public:
	AosJqlSelectFieldPtr	mField;
	bool					mIsAsc;
	bool					mIsInWhereCond;

	AosJqlOrderByField() {
		mIsAsc = true;
		mIsInWhereCond = false;
	}

	~AosJqlOrderByField(){}
	AosJqlOrderByFieldPtr clone() const
	{
		try{
			AosJqlOrderByFieldPtr v = OmnNew AosJqlOrderByField();
			if (mField) v->mField = mField->clone();
			v->mIsAsc = mIsAsc;
			v->mIsInWhereCond = mIsInWhereCond;
			return v;
		}
		catch(...)
		{
			OmnAlarm << "AosJqlOrderByField clone error." << enderr;
			return NULL;
		}
		return NULL;
	}
};

typedef vector<AosJqlOrderByFieldPtr> AosJqlOrderByFieldList;

class AosJqlOrderBy : public AosJqlDataStruct
{
public:
	public:
	AosJqlOrderByFieldList*		 mOrderFieldList;

public:
	AosJqlOrderBy();
	~AosJqlOrderBy();

	void setOrderFieldList(AosJqlOrderByFieldList* field_list);
	AosJqlOrderByFieldList* getOrderFieldList();
};

#endif
