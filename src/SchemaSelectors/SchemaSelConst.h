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
#ifndef Aos_SchemaSelConsts_SchemaSelConst_h
#define Aos_SchemaSelConsts_SchemaSelConst_h

#include "SchemaSelectors/SchemaSelector.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <hash_map>
using namespace std;


class AosSchemaSelConst : public AosSchemaSelector
{
private:

public:
	AosSchemaSelConst();
	AosSchemaSelConst(const AosSchemaSelConst &rhs);
	AosSchemaSelConst(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &jimo_doc, 
			const OmnString &version);
	~AosSchemaSelConst();

	// AosJimo Interface
	virtual bool run(	const AosRundataPtr &rdata);
	virtual AosJimoPtr cloneJimo(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc)  const;
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
	virtual OmnString toString() const;

	// SchemaSelConst Interface
	virtual AosDataRecordObjPtr selectSchema(
						const AosRundataPtr &rdata, 
						const AosTaskObjPtr &task,
						const AosXmlTagPtr &conf);

	virtual AosDataRecordObjPtr selectSchema(
						const AosRundataPtr &rdata, 
						const AosTaskObjPtr &task,
						const char *record, 
						const int len);
	virtual bool registerMethods();

private:
	bool init( const AosRundataPtr &rdata, const AosXmlTagPtr &jimo_doc);
};
#endif
