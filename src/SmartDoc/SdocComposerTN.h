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
// 2011/02/15	Created by Phnix
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SDocComposerTN_h
#define Aos_SmartDoc_SDocComposerTN_h

#if 0
#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosSdocComposerTN : public AosSmartDoc
{
private:

	enum
	{
		eMaxImages = 100
	};

	struct IMAGE
	{
		int layer;
		int width;
		int height;
		int left;
		int top;
		OmnString src;
	}struct_image;

	struct IMAGE_CMP 
	{
		bool operator ()(const IMAGE &s1, const IMAGE &s2)
		{
			return s1.layer < s2.layer;
		}
	} imgcmp;

	vector<IMAGE> mImage;

public:
	AosSdocComposerTN(const bool flag);
	~AosSdocComposerTN();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocComposerTN(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	int findValue(const OmnString &src,
			const OmnString &findstr);
};
#endif
#endif
