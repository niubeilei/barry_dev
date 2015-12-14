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
// 2013/05/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Jimo/JimoVector/JimoVector.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Jimo/JimoVector/JimoVectorImplU64.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoVector(
 		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
 		const AosXmlTagPtr &jimo_doc) 
{
	rdata->setDLLObj(0);
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosJimoVector(rdata, worker_doc, jimo_doc);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosJimoVector::AosJimoVector(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
:
AosJimo(rdata, jimo_doc)
{
	if (!config(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosJimoVector::~AosJimoVector()
{
}


bool
AosJimoVector::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	aos_assert_rr(worker_doc, rdata, false);
	OmnString type = worker_doc->getAttrStr("vector_type");
	try
	{
		if (type == "u64")
		{
			mVector = OmnNew AosJimoVectorImplU64();
		}

		if (type == "")
		{
			AosSetErrorU(rdata, "missing_vector_type") << enderr;
			return false;
		}

		AosSetErrorU(rdata, "unrecognized_vector_type") << type << enderr;
		return false;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_vector") << enderr;
		return false;
	}

	return true;
}


OmnString
AosJimoVector::toString() const
{
	OmnString ss = AosJimo::toString();
	return ss;
}


bool 
AosJimoVector::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) const
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool 
AosJimoVector::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


AosJimoPtr 
AosJimoVector::clone(const AosRundataPtr &rdata) const 
{
	try
	{
		return OmnNew AosJimoVector(*this);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_cloning_jimo") << toString() << enderr;
		return 0;
	}
}


bool 
AosJimoVector::run(		
		const AosRundataPtr &rdata, 
		const AosMethod::E method,
		const AosXmlTagPtr &worker_doc)
{
	return mVector->run(rdata, method, worker_doc);
}


bool 
AosJimoVector::supportInterface(
		const AosRundataPtr &rdata, 
		const OmnString &interface_objid) const
{
	AosXmlTagPtr doc = AosGetDocByObjid(interface_objid, rdata);
	if (!doc)
	{
		return false;
	}

	int id = doc->getAttrInt(AOSTAG_INTERFACE_ID, -1);
	return supportInterface(rdata, id);
}


bool 
AosJimoVector::supportInterface(
		const AosRundataPtr &rdata, 
		const int interface_id) const
{
	aos_assert_rr(mVector, rdata, false);
	return mVector->supportInterface(rdata, interface_id);
}


bool
AosJimoVector::runWithSmartdoc(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc)
{
	OmnScreen << "Running jimo example: " << toString() << endl;
	return true;
}

