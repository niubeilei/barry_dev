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
// 07/08/2010: Created by Tank
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlEditor.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHtmlEditor();

AosGicHtmlEditor::AosGicHtmlEditor(const bool flag)
:
AosGic(AOSGIC_HTMLEDITOR, AosGicType::eHtmlEditor, flag)
{
}


AosGicHtmlEditor::~AosGicHtmlEditor()
{
}


bool	
AosGicHtmlEditor::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code
	// 	4. Flash code
	 return true;
}

