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
// 09/11/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryFormatObj_h
#define Aos_SEInterfaces_QueryFormatObj_h

#include "Util/RCObject.h"
#include "QueryInto/QueryIntoLocalFile.h"

class AosQueryFormatObj : virtual public OmnRCObject    
{

public:

	static AosQueryFormatObjPtr createQueryFormatStatic(
						const AosXmlTagPtr &def,
						AosRundata *rdata);

	virtual bool	 config(
				const AosXmlTagPtr &format,
				const AosXmlTagPtr &output_record,
				const AosRundataPtr &rdata) = 0;

	virtual void 	setQueryInto(const AosQueryIntoObjPtr &query_into) = 0;

	virtual bool 	proc(
						const AosDatasetObjPtr &dataset,
						AosRundata* rdata) = 0;

	
};

#endif
