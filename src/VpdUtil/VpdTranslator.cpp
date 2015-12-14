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
// 	Created: 05/16/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VpdUtil/VpdTranslator.h"


bool AosVpdTranslate(
		TiXmlNode *xmlobj, 
		TiXmlNode *obj,
		OmnString errmsg)
{
	// This function translates 'xmlobj' into 'vpd' and 'obj'. The function 
	// assumes 'obj' is empty. For each GIC contained in 'xmlobj', 
	// it inspects all the object bindings. For each object binding, 
	// if the binding is translatable, it adds the attribute
	// to 'obj', if the attribute is not in 'obj' yet. After
	// that, it resets the value to its default. 
	// After the translation, it is a modified 'xmlobj' that should be used
	// as a vpd for 'obj'. 
	//
	errmsg = "";
	aos_assert_r(xmlobj, false);
	aos_assert_r(obj, false);

	// Check whether 'xmlobj' has the '<panels>' tag. 
	TiXmlNode *panels = xmlobj->FirstChild("panels");
	if (panels)
	{
		// It is a VPD of the following format:
		// 	<window ...>
		// 		<panels ...>
		// 			<panel .../>
		// 			<panel .../>
		// 			...
		// 		</panels>
		// 		...
		// 	</window>
		bool result = true;
		TiXmlNode *pre_panel = null;
		TiXmlNode *crt_panel = null;
		while ((crt_panel = panels->IterateChildren("panel", pre_panel) != 0)
		{
			// Found a panel 'crt_panel'. 
			result &= AosVpdTranslate_Panel(crt_panel, obj, errmsg);
			pre_panel = crt_panel;
		}
		return result;
	}

	// Check whether it contains '<pane>'
	TiXmlNode *panel = xmlobj->FirstChild("panel");
	if (panel)
	{
		// It is a VPD of the following format:
		// 	<window ...>
		// 		<panel ...>
		// 		...
		// 	</window>
		return AosVpdTranslate_Panel(panel, obj, errmsg);
	}

	// Check whether it contains '<gicCreators>'
	TiXmlNode *creators = xmlobj->FirstChild("gicCreators");
	if (creators)
	{
		return AosVpdTranslate_Creators(creators, obj, errmsg);
	}

	// This is an error
	errmsg = "Unrecognized XML object format";
	OmnAlarm << errmsg << enderr;
	return false;
}

