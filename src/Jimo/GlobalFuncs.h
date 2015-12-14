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
// 2013/11/23 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_GlobalFuncs_h
#define Aos_Jimo_GlobalFuncs_h

#include "Jimo/Ptrs.h"
#include "Util/String.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"


extern AosJimoPtr AosCreateJimoByClassname(
		const AosRundataPtr &rdata, 
		const OmnString &classname, 
		const int version);

extern AosJimoPtr AosCreateJimoByClassname(
		AosRundata *rdata, 
		const OmnString &classname, 
		const int version); 

// Chen Ding, 2014/11/30
extern AosJimoPtr AosCreateJimoByName(
		AosRundata *rdata, 
		const OmnString &jimo_name, 
		const int version); 

extern AosJimoPtr AosCreateJimoByName(
		const AosRundataPtr &rdata, 
		const OmnString &jimo_name, 
		const int version); 

//xuqi, 2015/9/12
extern AosJimoPtr AosCreateJimoByName(
		AosRundata *rdata, 
		const OmnString &jimo_namespace,
		const OmnString &jimo_name, 
		const int version); 

extern AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const OmnString &objid, 
		const int version); 

extern AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const int version = -1);

extern AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc); 

extern AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const OmnString &jimo_objid);

extern AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc,
		const int version);

#endif

