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
// 2013/07/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SchemaSelectors_SchemaSelector_h
#define Aos_SchemaSelectors_SchemaSelector_h

#include "SEInterfaces/SchemaSelectorObj.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <hash_map>
using namespace std;


class AosSchemaSelector : public AosSchemaSelectorObj
{
	OmnDefineRCObject;

private:
	OmnString		mVersion;
	OmnString		mJimoName;
	OmnString		mSchemaSelectorType;

public:
	AosSchemaSelector();
	AosSchemaSelector(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &jimo_doc, 
			const OmnString &name,
			const OmnString &version, 
			const OmnString &type);
	~AosSchemaSelector();
	
	// AosJimo Interface
	virtual AosJimoType::E getJimoType() const {return AosJimoType::eSchemaSelector;}
	virtual OmnString getVersion() const {return mVersion;}
	virtual OmnString getJimoName() const {return mJimoName;}
	virtual OmnString getSchemaSelectorType() const {return mSchemaSelectorType;}

	virtual bool config(const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc);

	virtual bool serializeTo(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual bool serializeFrom(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual void * getMethod(const AosRundataPtr &rdata, 
						const OmnString &name, 
						AosMethodId::E &method_id);

	// SchemaSelector Interface
	virtual AosDataRecordObjPtr selectSchema(
				const AosRundataPtr &rdata, 
				const AosTaskObjPtr &task,
				const AosXmlTagPtr &conf);

	virtual AosDataRecordObjPtr selectSchema(
				const AosRundataPtr &rdata, 
				const AosTaskObjPtr &task,
				const char *record, 
				const int len);
};
#endif
