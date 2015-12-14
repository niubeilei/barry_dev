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
// 2013/06/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DynJimos_ActionsOne_JimoCreateDatalet_h
#define Aos_DynJimos_ActionsOne_JimoCreateDatalet_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <hash_map>
using namespace std;


class AosJimoCreateDatalet : public AosJimo
{
public:

public:
	AosJimoCreateDatalet(const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);
	AosJimoCreateDatalet(const AosJimoCreateDatalet &rhs);
	~AosJimoCreateDatalet();
	
	virtual bool run(		
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);

	virtual bool serializeTo(
					const AosRundataPtr &rdata, 
					const AosBuffPtr &buff);

	virtual bool serializeFrom(
					const AosRundataPtr &rdata, 
					const AosBuffPtr &buff);

	virtual AosJimoPtr clone(const AosRundataPtr &rdata) const;
	virtual OmnString toString() const;

	virtual void * getMethod(const AosRundataPtr &rdata, 
					const OmnString &name, 
					AosMethodId::E &method_id);

	virtual bool config( 	const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);

private:
	bool init(		const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);

	bool registerMethods();
};
#endif
