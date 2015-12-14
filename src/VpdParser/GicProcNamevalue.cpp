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
// 	Created: 12/14/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VpdParser/GicProcNamevalue.h"

#include "alarm_c/alarm.h"
#include "Book/Book.h"
#include "Debug/Debug.h"
#include "TinyXml/tinyxml.h"
#include "VpdParser/GicUtil.h"


AosGicProcNamevalue::~AosGicProcNamevalue()
{
}


bool 
AosGicProcNamevalue::procGic(
		const AosBookPtr &book,
		TiXmlElement *obj, 
		TiXmlElement *viewer, 
		TiXmlElement *editor, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 'obj' is the object to be created based on 'viewer'.
	// 'viewer' is pointing to a GIC for the viewer,
	// 'editor' is pointing to a GIC for the editor. 
	// We need to do the following:
	// 	1. Add an attribute to 'obj'. The attribute name is to be determined
	// 	   by the book. The value is determined by 'viewer'.  
	// 	2. Keep the viewer the same, but bind it to the newly created attribute
	// 	3. Change the editor to Input GIC and bind it to the newly created attribute
	//
	
	// 1. Add an attribute to the object
	OmnString attr_name = viewer->Attribute("attr_name");
	if (attr_name.length() == 0)
	{
	 	attr_name = book->getNewAttrName();
	}

	OmnString value = viewer->Attribute("val_text");
	aos_assert_r(value.length() > 0, false);

	obj->SetAttribute(attr_name.data(), value.data());

	// 2. Keep the viewer the same, but bind it to the attribute
	viewer->SetAttribute("val_bind", attr_name.data());

	// 3. Change the editor to Input GIC and bind it to the attribute
	TiXmlElement *input_gic = createInputGic(editor);
	aos_assert_r(input_gic, false);
	input_gic->SetAttribute("edt_query", attr_name.data());
	TiXmlNode *parent = editor->Parent();
	parent->ReplaceChild(editor, *input_gic);
	return true;
}


TiXmlElement *
AosGicProcNamevalue::createInputGic(TiXmlElement *nv)
{
	// This function creates a new TiXmlElement for the Input GIC
	// based on the 'namevale' TiXmlElement. 'namevalue' should
	// be a node for Namevalue GIC.
	aos_assert_r(nv, 0);

	TiXmlElement *input = new TiXmlElement("input");
	AosGicUtil::convertAttr(input, nv, "val_maxlength", "edt_maxlength", "-1");
	AosGicUtil::convertAttr(input, nv, "val_multiline", "edt_multiline", "false");
	AosGicUtil::convertAttr(input, nv, "val_fsize", 	"edt_fsize", 	 "12");
	AosGicUtil::convertAttr(input, nv, "val_fstyle", 	"edt_fstyle", 	 "plain");

	AosGicUtil::convertAttr(input, nv, "ptg_layout", 	"ptg_layout", 	 "horizontal");
	AosGicUtil::convertAttr(input, nv, "ptg_halign", 	"ptg_halign", 	 "left");
	AosGicUtil::convertAttr(input, nv, "ptg_valign", 	"ptg_valign", 	 "center");
	AosGicUtil::convertAttr(input, nv, "ptg_prompt_text","ptg_prompt_text",	 "");
	AosGicUtil::convertAttr(input, nv, "ptg_fsize",		"ptg_fsize",	 "12");
	AosGicUtil::convertAttr(input, nv, "ptg_fstyle",	"ptg_fstyle",	 "plain");
	AosGicUtil::convertAttr(input, nv, "ptg_spacing",	"ptg_spacing",	 "5");

	AosGicUtil::convertAttr(input, nv, "gic_width", 	"gic_width", 	 "200");
	AosGicUtil::convertAttr(input, nv, "gic_height", 	"gic_height", 	 "25");
	AosGicUtil::convertAttr(input, nv, "gic_x", 		"gic_x", 	 	 "0");
	AosGicUtil::convertAttr(input, nv, "gic_y", 		"gic_y", 	 	 "0");
	AosGicUtil::convertAttr(input, nv, "gic_topinset", 	"gic_topinset",  "0");
	AosGicUtil::convertAttr(input, nv, "gic_leftinset", "gic_leftinset", "0");
	AosGicUtil::convertAttr(input, nv, "gic_rightinset","gic_rightinset","0");
	AosGicUtil::convertAttr(input, nv, "gic_bottominset","gic_bottominset","0");

	return input;
}

