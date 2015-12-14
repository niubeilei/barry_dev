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
// 2013/10/31 Created by Linda Lin
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/StructProcObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoType.h"

#if 0
AosStructProcObj::AosStructProcObj(const int version)
:
AosJimo(AosJimoType::eStructProc, version)
{
}


AosStructProcObj::~AosStructProcObj()
{
}


AosStructProcObjPtr 
AosStructProcObj::createStructProcStatic(
			AosRundata *rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_doc);
	if (!jimo)
	{
		AosSetErrorUser(rdata, "failed_creating_struct_proc") << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eStructProc)
	{
		AosSetErrorUser(rdata, "not_a_struct_struct_jimo") << enderr;
		return 0;
	}

	AosStructProcObjPtr proc = dynamic_cast<AosStructProcObj*>(jimo.getPtr());
	if (!proc)
	{
		AosSetErrorUser(rdata, "invalid_struct_proc") << enderr;
		return 0;
	}

	return proc;
}


AosStructProcObjPtr 
AosStructProcObj::createStructProcStatic(
			AosRundata *rdata, 
			const AosXmlTagPtr &worker_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo)
	{
		AosSetErrorUser(rdata, "failed_creating_struct_proc") << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eStructProc)
	{
		AosSetErrorUser(rdata, "not_a_struct_proc_jimo") << enderr;
		return 0;
	}

	AosStructProcObjPtr proc = dynamic_cast<AosStructProcObj*>(jimo.getPtr());
	if (!proc)
	{
		AosSetErrorUser(rdata, "invalid_struct_proc") << enderr;
		return 0;
	}

	return proc;
}
#endif
