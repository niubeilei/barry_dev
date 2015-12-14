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
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Jimo/JimoDummy.h"

#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"


AosJimoDummy::AosJimoDummy()
:
AosJimo(AosJimoType::eDummy, 1)
{
}


AosJimoDummy::~AosJimoDummy()
{
}


bool 
AosJimoDummy::run(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosJimoDummy::run(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	OmnShouldNeverComeHere;
	return false;
}


void *
AosJimoDummy::getMethod( 		
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosJimoUtil::funcmap_t &map, 
		AosMethodId::E &method_id)
{
	OmnShouldNeverComeHere;
	return 0;
}


AosJimoPtr AosJimoDummy::cloneJimo()  const
{
	return OmnNew AosJimoDummy();
}


OmnString AosJimoDummy::toString() const
{
	return "";
}

