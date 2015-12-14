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
#ifndef Aos_SdocAction_ActCreateDoc_h
#define Aos_SdocAction_ActCreateDoc_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include <vector>
#if 0

class AosActCreateDoc : virtual public AosSdocAction
{
private:

public:
	AosActCreateDoc(const bool flag);
	AosActCreateDoc(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActCreateDoc();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	bool		config(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	OmnString	isAttrAndText(const OmnString &path);

	bool	addAttrNode(
				const AosXmlTagPtr &logdoc, 
				const OmnString &path,
				const OmnString &value);

	bool	addTextNode(
				const AosXmlTagPtr &logdoc,
				const OmnString &path,
				const OmnString &value);

	bool	parseAttrs(
				const AosXmlTagPtr &sdoc, 
				AosXmlTagPtr &logdoc, 
				const AosRundataPtr &rdata);

	bool	parseNode(
				const AosXmlTagPtr &sdoc, 
				AosXmlTagPtr &logdoc, 
				const AosRundataPtr &rdata);

	bool 	addPathNode(
				const AosXmlTagPtr &logdoc,
				const OmnString &path,
				const AosXmlTagPtr &nodexml);
};
#endif
#endif

