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
// 2013/05/07 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActCreateFile_h
#define Aos_SdocAction_ActCreateFile_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosActCreateFiles : virtual public AosSdocAction
{
private:
	enum
	{
		eBuffSize = 50 * 1000 * 1000 + 100
	};

public:
	AosActCreateFiles(const bool flag);
	~AosActCreateFiles();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

	bool readFiles(const OmnString &path, const int rec_level, const int crt_level, const AosXmlTagPtr &doc, const AosRundataPtr &rdata);

};

#endif

