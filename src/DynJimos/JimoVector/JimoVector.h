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
#ifndef Aos_Jimo_JimoVector_JimoVector_h
#define Aos_Jimo_JimoVector_JimoVector_h

#include "Jimo/JimoVector/Ptrs.h"
#include "SEInterfaces/Jimo.h"
#include "Util/String.h"
#include <vector>
using namespace std;

class AosJimoVector : public AosJimo
{
private:
	AosJimoVectorImplPtr	mVector;

public:
	AosJimoVector(const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);
	~AosJimoVector();

	// Jimo Interface
	virtual bool run(		const AosRundataPtr &rdata, 
							const AosMethod::E method,
							const AosXmlTagPtr &sdoc);

	virtual bool serializeTo(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff) const;
	virtual bool serializeFrom(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff);
	virtual AosJimoPtr clone(const AosRundataPtr &rdata) const;
	virtual OmnString toString() const;
	virtual bool supportInterface(
							const AosRundataPtr &rdata, 
							const OmnString &interface_objid) const;
	virtual bool supportInterface(
							const AosRundataPtr &rdata, 
							const int interface_id) const;

private:
	bool config( 			const AosRundataPtr &rdata,
							const AosXmlTagPtr &worker_doc,
							const AosXmlTagPtr &def);
	bool runWithSmartdoc(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &sdoc);
};
#endif
