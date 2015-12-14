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
// 05/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActModifyStr_h
#define Aos_SdocAction_ActModifyStr_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include <vector>

class AosActModifyStr : virtual public AosSdocAction
{
private:
	int					mStartPos;
	int					mMaxLength;
	bool				mLeftAlign;
	char				mPadding;
	AosValueSelObjPtr	mValueSelector;

public:
	AosActModifyStr(const bool flag);
	AosActModifyStr(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosActModifyStr();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool run(const char *&record, int &len, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
	bool config(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};
#endif

