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
#ifndef Aos_SEInterfaces_QueryIntoLocalFileObj_h
#define Aos_SEInterfaces_QueryIntoLocalFileObj_h

#include "Util/RCObject.h"
#include "XmlUtil/XmlTag.h"

class AosQueryIntoObj : virtual public OmnRCObject    
{
public:

	static AosQueryIntoObjPtr createQueryIntoStatic(
			const AosXmlTagPtr &def,
			AosRundata *rdata);

	virtual AosQueryIntoObjPtr clone() const = 0;

	virtual bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata) = 0;

	virtual bool 	appendEntry(
				AosDataRecordObj *record,
		        AosRundata *rdata) = 0;

	virtual bool	flush(AosRundata *rdata) = 0;


};

#endif
