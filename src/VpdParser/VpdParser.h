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
// 	Created: 12/13/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_VpdParser_VpdParser_h
#define Omn_VpdParser_VpdParser_h

#include "Book/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "VpdParser/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include <string.h>


class TiXmlElement;
class TiXmlNode;

class AosVpdParser : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosVpdParser();
	~AosVpdParser(); 

	static void init();
	static bool registerGicProc(const AosGicProcPtr &gic);

	bool	createBkpage(
				TiXmlElement *bookdef,
				AosXmlRc &errcode, 
				OmnString &errmsg);

private:
	bool	procPanel(
				const AosBookPtr &book,
				TiXmlElement *obj,
				TiXmlElement *viewer,
				TiXmlElement *editor,
				AosXmlRc &errcode, 
				OmnString &errmsg);

	bool	procWindow(
				const AosBookPtr &book,
				TiXmlElement *obj,
				TiXmlElement *viewer,
				TiXmlElement *editor,
				AosXmlRc &errcode, 
				OmnString &errmsg);

	bool 	procGics(
				const AosBookPtr &book,
				TiXmlElement *obj,
				TiXmlElement *viewer,
				TiXmlElement *editor,
				AosXmlRc &errcode, 
				OmnString &errmsg);
};
#endif
