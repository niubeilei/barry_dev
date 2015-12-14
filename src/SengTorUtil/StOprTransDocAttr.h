////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StOprTransDocAttr_h
#define AosSengTorUtil_StOprTransDocAttr_h

#include "SengTorUtil/StOprTranslator.h"
#include "XmlUtil/Ptrs.h"


class AosStOprTransDocAttr : public AosStOprTranslator
{
private:
	OmnString		mAttrname;

public:
	AosStOprTransDocAttr(const bool regflag);
	~AosStOprTransDocAttr();

	virtual OmnString setRandContents(const AosRundataPtr &rdata);
	virtual AosStOprTranslatorPtr clone() const
	{
		return OmnNew AosStOprTransDocAttr(false);
	}
	virtual bool pickCommand(OmnString &cmd, 
						OmnString &operation, 
						const AosSengTestThrdPtr &thread);

	// virtual bool checkOperation(
	// 				const AosStContainerPtr &st_ctnr,
	// 				const AosXmlTagPtr &xml_ctnr,
	// 				const AosSengTestThrdPtr &thread);
private:
	OmnString determineMapValues(const AosRundataPtr &rdata);
	OmnString determineValueSel(const AosRundataPtr &rdata);
};
#endif

