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
// 2013/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef DynJimos_DataConverters_JimoDataConvertersShell_h
#define DynJimos_DataConverters_JimoDataConvertersShell_h

extern bool AosJimoDataConverters_getEpochYear(		
		const AosRundataPtr &rdata, 
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc);

extern bool AosJimoDataConverters_getEpochMonth(		
		const AosRundataPtr &rdata, 
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc);

extern bool AosJimoDataConverters_getEpochDay(		
		const AosRundataPtr &rdata, 
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc);

extern bool AosJimoDataConverters_getEpochHour(		
		const AosRundataPtr &rdata, 
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc);

extern bool AosJimoDataConverters_getEpochTime(		
		const AosRundataPtr &rdata, 
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc);

extern bool AosJimoDataConverters_getStartTime(		
		const AosRundataPtr &rdata, 
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc);

#endif

