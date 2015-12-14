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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActDataGenerate_h
#define Aos_SdocAction_ActDataGenerate_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataGenUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosActDataGenerate : virtual public AosSdocAction
{
private:

public:
	AosActDataGenerate();
	AosActDataGenerate(const bool flag);
	~AosActDataGenerate();

	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	bool	config(const AosXmlTagPtr &config);
	AosXmlTagPtr parseDoc(
				const AosXmlTagPtr &config,
				const AosRundataPtr &rdata);

	bool	parse(
				const AosXmlTagPtr &config, 
				vector<AosDataGenUtilPtr> &attrname,
				vector<AosDataGenUtilPtr> &attrvalue,
				AosXmlTagPtr &createddoc,
				const AosRundataPtr &rdata);

	bool	addNode(
				AosXmlTagPtr doc,
				const OmnString &path);

	bool	createData(
				const AosXmlTagPtr &createddoc,
				const vector <AosDataGenUtilPtr> &attrname,
				const vector <AosDataGenUtilPtr> &attrvalue,
				const AosRundataPtr &rdata);
	bool	parseAttribute(
				const AosXmlTagPtr &config,
				vector <AosDataGenUtilPtr> &attrname,
				vector <AosDataGenUtilPtr> &attrvalue,
				const AosRundataPtr &rdata);
};
#endif

