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
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermDeletedDocs_h
#define Aos_Query_TermDeletedDocs_h

#include "Query/TermIILType.h"


class AosTermDeletedDocs : public AosTermIILType 
{
private:

public:
	AosTermDeletedDocs(const bool regflag);
	AosTermDeletedDocs(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermDeletedDocs();

	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

private:
	bool parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

