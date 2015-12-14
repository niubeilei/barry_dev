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
// 07/23/2013 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataConverter_DataConverterJson_h
#define Aos_DataConverter_DataConverterJson_h

#include "DataConverter/DataConverter.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosDataConverterJson : public AosDataConverter
{
	OmnDefineRCObject;

public:
	AosDataConverterJson();
	AosDataConverterJson(
        const AosRundataPtr &rdata, 
        const AosXmlTagPtr &jimo_doc, 
        const OmnString &version);
	~AosDataConverterJson();

	// AosJimo Interface
	virtual bool run(   const AosRundataPtr &rdata);
	virtual AosJimoPtr cloneJimo(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc) const;
	virtual bool config(const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker);
	virtual void * getMethod(const AosRundataPtr &rdata, 
						const OmnString &name, 
						AosMethodId::E &method_id);

	// AosDataConverterObj interface
	virtual bool run( 	const AosRundataPtr &rdata,
						const AosXmlTagPtr &data, 
						AosXmlTagPtr &converted_data); 

private:
	bool init(         	const AosRundataPtr &rdata,
						const AosXmlTagPtr &jimo_doc);

	bool registerMethods();
};

#endif

