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
// 11/17/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocOutputDataFileFormat_h
#define Aos_SmartDoc_SdocOutputDataFileFormat_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocOutputDataFileFormat : public AosSmartDoc
{

public:
	AosSdocOutputDataFileFormat(const bool flag);
	~AosSdocOutputDataFileFormat();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocOutputDataFileFormat(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:

	bool		runOutputData(
					AosValueRslt &valueRslt,
					const AosXmlTagPtr &actions,
					const AosRundataPtr &rdata);

	bool 		creatDir(
					const AosXmlTagPtr &userdata,
					const OmnString dirname,
					u32	&dir_num,
					const AosRundataPtr &rdata);

	bool		writeFile(
					const AosXmlTagPtr &userdata,
					const AosRundataPtr &rdata);

};
#endif

