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
// 08/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_FilterObj_h
#define Aos_SEInterfaces_FilterObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/FilterType.h"
#include "SEInterfaces/FilterCreatorObj.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosFilterObj : public OmnRCObject
{
protected:
	AosFilterType::E				mType;
	static AosFilterCreatorObjPtr	smCreator;

public:
	AosFilterObj();
	AosFilterObj(const OmnString &name, const AosFilterType::E type, const bool flag);
	~AosFilterObj();

	virtual bool filterData(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata) = 0;
	virtual bool filterData(
						const char *data, 
						const int len, 
						const bool dft, 
						AosRundataPtr &rdata) = 0;

	static AosFilterObjPtr createFilter(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	static void setCreator(const AosFilterCreatorObjPtr &creator) {smCreator = creator;}
	static AosFilterCreatorObjPtr getCreator() {return smCreator;}

private:
	bool registerFilter(const OmnString &name, const AosFilterObjPtr &filter);
};
#endif

