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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtGenericobj_H
#define AOS_JQLStatement_JqlStmtGenericobj_H

#include "JQLStatement/JqlStatement.h"

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/GenericVerbMgrObj.h"
#include "SEInterfaces/GenericObjMgrObj.h"
#include "SEInterfaces/GenericValueObj.h"
#include "Util/String.h"

class AosJqlStmtGenericobj : public AosJqlStatement
{
private:
	OmnString			mVerb;
	OmnString 			mType;
	OmnString			mName;
	vector<AosGenericValueObjPtr> *mParms;

public:
	AosJqlStmtGenericobj();
	~AosJqlStmtGenericobj();

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

public:
	void setType(const AosExprObjPtr &type);
	void setName(const AosExprObjPtr &name);
	void setVerb(const OmnString &verb) {mVerb = verb;}
	void setParms(vector<AosGenericValueObjPtr> *&parms) {mParms = parms;}
	OmnString getName() const {return mName;}

	void setGenericobjValues(vector<AosGenericValueObjPtr> *&parms);
	//void setGenericobjValues(vector<AosExprObjPtr> *&parms);
	//void setGenericobjValues(vector<AosJqlGenericobjValuePtr> *&parms);
private:
	bool procGenericobj(const AosRundataPtr &rdata);
	OmnString toString(const AosRundataPtr &rdata);

};

#endif
