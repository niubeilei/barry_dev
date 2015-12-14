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
#ifndef Aos_SdocAction_AosModifyNode_h
#define Aos_SdocAction_AosModifyNode_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include <vector>


class AosActModifyNode : virtual public AosSdocAction
{
private:

public:
	AosActModifyNode(const bool flag);
	~AosActModifyNode();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	virtual bool	config(const AosXmlTagPtr &config);
	bool	addAttrNode(
				const AosXmlTagPtr &logdoc, 
				const OmnString &path,
				const OmnString &value);
	OmnString	isAttrAndText(const OmnString &path);
	bool	addTextNode(
				const AosXmlTagPtr &logdoc,
				const OmnString &path,
				const OmnString &value);
	bool	parseAttrs(const AosXmlTagPtr &sdoc, AosXmlTagPtr &logdoc, const AosRundataPtr &rdata);
};
#endif

