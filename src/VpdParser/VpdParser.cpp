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
//	Created: 12/13/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VpdParser/VpdParser.h"

#include "alarm_c/alarm.h"
#include "Book/BookMgr.h"
#include "Book/Book.h"
#include "Debug/Debug.h"
#include "parser/xml_node.h"
#include "TinyXml/tinyxml.h"
#include "util_c/strutil.h"
#include "VpdParser/GicName.h"
#include "VpdParser/AllGics.h"
#include "VpdParser/GicProc.h"
#include "VpdParser/GicProcInput.h"
#include "VpdParser/GicProcNamevalue.h"
#include "VpdParser/GicProcPrompt.h"
#include <string.h>


static AosGicProcPtr sgGicProcs[AosGicName::eMaximum];

void AosVpdParser::init()
{
	sgGicProcs[AosGicName::eInput] = new AosGicProcInput();
	sgGicProcs[AosGicName::ePrompt] = new AosGicProcPrompt();
	sgGicProcs[AosGicName::eNameValue] = new AosGicProcNamevalue();
}


bool
AosVpdParser::registerGicProc(const AosGicProcPtr &gic)
{
	AosGicName::E code = gic->getGicId();
	aos_assert_r(code > AosGicName::eUnknown && code < AosGicName::eMaximum, false);
	sgGicProcs[code] = gic;
	return true;
}


AosVpdParser::AosVpdParser()
{
}


AosVpdParser::~AosVpdParser()
{
}


bool
AosVpdParser::createBkpage(
		TiXmlElement *obj, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	aos_assert_r(obj, false);
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Internal error";

	// Get the objid
	const char * objid = obj->Attribute("objid");
	aos_assert_r(objid, false);

	AosBookPtr book = AosBookMgr::getBook(objid);
   	if (!book)
	{
		errcode = eAosXmlInt_General;
		errmsg = "Book not found in database";
		return false;
	}

	// Create the Obj
	OmnString str = "<object/>";
	TiXmlDocument newobj_doc;
	newobj_doc.Parse(str.data());
	TiXmlNode *root = newobj_doc.FirstChild();
	TiXmlElement *newobj = root->ToElement();

	// Create the viewer from 'contents'
	// TiXmlDocument thedoc2;
	// thedoc2.Parse(contents);
	// root = thedoc2.FirstChild();
	// TiXmlElement *viewer = obj->ToElement();
	TiXmlElement *viewer = obj->Clone()->ToElement();

	// Create the editor from 'contents'
	// TiXmlDocument thedoc3;
	// thedoc3.Parse(contents);
	// root = thedoc3.FirstChild();
	// TiXmlElement *editor = root->ToElement();
	TiXmlElement *editor = obj->Clone()->ToElement();

	// 'elem' is the root of the VPD. Check whether it is in the form:
	// 	<vpd ...>
	// 		<gic_creators>
	// 		...
	// 	</vpd>
	TiXmlNode *nn = obj->FirstChild("gic_creators");
	if (nn)
	{
		bool rslt = procPanel(book, newobj, viewer, editor, errcode, errmsg);
		if (rslt)
		{
			rslt = book->addObjects(newobj, viewer, editor, errcode, errmsg);
		}
		if (rslt) errcode = eAosXmlInt_Ok;
		return rslt;
	}

	// Check whether it is the form: 
	// 	<vpd ...
	// 		pane_type="PWT"
	// 		...
	// 		<pane ...>
	// 		...
	// 	</vpd>
	const char *attr = obj->Attribute("pane_type");
	if (attr && strcmp(attr, "PWT") == 0)
	{
		bool rslt = procWindow(book, newobj, viewer, editor, errcode, errmsg);
		if (rslt) 
		{
			rslt = book->addObjects(newobj, viewer, editor, errcode, errmsg);
		}
		if (rslt) errcode = eAosXmlInt_Ok;
		return rslt;
	}

	errcode = eAosXmlInt_SyntaxError;
	errmsg = "VPD form not recognized";
	return false;
}


bool
AosVpdParser::procPanel(
		const AosBookPtr &book,
		TiXmlElement *obj, 
		TiXmlElement *viewer, 
		TiXmlElement *editor, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 
	// The VPD should be in the form:
	// 	<vpd ...
	// 		<gic_creators>
	// 			<gic .../>
	// 			...
	// 		</gic_creators>
	// 	</vpd>
	//
	// This function will create three objects: 
	// 	Obj
	// 	Viewer
	// 	Editor
	// based on 'obj'. 
	//
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Failed the processing";

	TiXmlNode *tmp = viewer->FirstChild("gic_creators");
	if (!tmp)
	{
		// There is no child for this panel. This means that the three
		// objects (i.e., obj, viewer, and editor) will all be the same.
		return true;
	}
	TiXmlElement *viewergic = tmp->ToElement();
	TiXmlElement *editorgic = editor->FirstChild("gic_creators")->ToElement();
	return procGics(book, obj, viewergic, editorgic, errcode, errmsg);
}


// 
// This function 'viewer' is an element pointing to "<gic_creators>". 
// 'obj' is the object to be created based on 'viewer'. 'editor' is 
// also pointing to "<gic_creators>". The function will process all the 
// gics contained in "<gic_creators>", constructing the 'obj', 'viewer', 
// and 'editor' based on the definition of each gic. 
//
// Upon success, it returns true. Otherwise, it returns false. 
//
bool
AosVpdParser::procGics(
		const AosBookPtr &book,
		TiXmlElement *obj, 
		TiXmlElement *viewer, 
		TiXmlElement *editor, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Processing failed";

	TiXmlNode *viewergic = 0;
	TiXmlNode *editorgic = 0;
	TiXmlElement *gic = 0;
	while ((viewergic = viewer->IterateChildren(viewergic)))
	{
		editorgic = editor->IterateChildren(editorgic);

		gic = viewergic->ToElement();
		aos_assert_r(gic, false);

		const char *type = gic->Attribute("gic_type");
		AosGicName::E gic_type = AosGicName::toEnum(type);	
		AosGicProcPtr proc = sgGicProcs[gic_type];
		if (!proc)
		{
			errcode = eAosXmlInt_General;
			errmsg = "Failed finding the GIC's processor: ";
			errmsg += type;
			return false;
		}

		TiXmlElement *eegic = editorgic->ToElement();

		aos_assert_r(proc->procGic(book, obj, gic, eegic, errcode, errmsg), false);	
	}

	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosVpdParser::procWindow(
		const AosBookPtr &book,
		TiXmlElement *obj, 
		TiXmlElement *viewer, 
		TiXmlElement *editor, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	return true;
}

