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
// 07/23/2012 Created by Felicia
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataSamplerObj_h
#define Aos_SEInterfaces_DataSamplerObj_h

#include "SEInterfaces/DataSamplerCreatorObj.h"
#include "SEInterfaces/DataSamplerType.h"
#include "SEInterfaces/DataProcStatus.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DataTypes.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/BuffArray.h"

#include <vector>

class AosValueRslt;

class AosDataSamplerObj : virtual public OmnRCObject
{
protected:
	static AosDataSamplerCreatorObjPtr		smCreator;

	AosDataSamplerType::E	mType;

public:
	virtual bool createSampler(
						const OmnString &fname,
						const int record_len,
						vector<OmnString> &border_values,
						const AosRundataPtr &rdata) = 0;

	virtual bool createSampler(
						const AosBuffArrayPtr &orig_array,
						AosBuffArrayPtr &result_array,
						const AosRundataPtr &rdata) = 0;

	virtual AosDataSamplerObjPtr clone() = 0;

	virtual AosDataSamplerObjPtr clone(
						const AosXmlTagPtr &conf, 
						const AosRundataPtr &rdata) = 0;

	virtual bool    serializeTo(
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata) = 0;

	virtual bool    serializeFrom(
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata) = 0;

	bool distributeData(
					vector<AosBuffArrayPtr> &buckets,
					const AosBuffArrayPtr &orig_array,
					const AosBuffArrayPtr &result_array,
					const AosRundataPtr &rdata);

	AosDataSamplerType::E getType() const {return mType;}

	static AosDataSamplerObjPtr createDataSampler(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

	
	static bool registerDataSampler(const OmnString &name, const AosDataSamplerObjPtr &cacher);

	static AosDataSamplerObjPtr serializeFromStatic(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata);

	static void setCreator(const AosDataSamplerCreatorObjPtr &creator) {smCreator = creator;}
	static AosDataSamplerCreatorObjPtr getCreator() {return smCreator;}
};

#endif

