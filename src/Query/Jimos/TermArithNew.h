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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermArithNew_h
#define Aos_Query_TermArithNew_h

#include "Jimo/Jimo.h"
#include "Query/TermIILType.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Opr.h"
#include "SEUtil/Ptrs.h"


class AosTermArithNew : virtual public AosTermIILType, virtual  public AosJimo
{
public:
	AosTermArithNew(const int version);

	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;
	virtual bool config(const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

private:
	bool resolveIndex(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
	bool retrieveOpr(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
	bool retrieveValue1(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
	bool retrieveContainer(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
	bool retrieveValue2(const AosRundataPtr &rdatd, const AosXmlTagPtr &def);
	bool retrieveFieldname(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
	bool retrieveQueryFilters(const AosRundataPtr &rdata, const AosXmlTagPtr &def);
};
#endif


