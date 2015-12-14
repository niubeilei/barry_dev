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
// 06/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CustomizedActions_Unicom_UnicomActModifyRcd_h
#define Aos_CustomizedActions_Unicom_UnicomActModifyRcd_h

#include "Actions/SdocAction.h"

class AosUnicomActModifyRcd : virtual public AosSdocAction
{
private:

public:
	AosUnicomActModifyRcd(const bool reg);
	AosUnicomActModifyRcd(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosUnicomActModifyRcd();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rundata);
	virtual bool run(const char *record, const int len, const AosRundataPtr &rdata);
	virtual bool serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata) const;

	bool config(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};
#endif

