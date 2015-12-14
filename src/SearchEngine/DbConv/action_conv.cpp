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
// Modification History:
// 04/15/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/DbConv/action_conv.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Util/String.h"
#include "Util/StrSplit.h"

extern bool
AosConvertAction_ProcessParm(
		OmnString &rslts, 
		const char *data,
		const OmnString &pname, 
		int &endidx, 
		const OmnString &action_def);

OmnString
AosConvertSelector(const OmnString &def)
{
	// Selector is used to select members. It is in the form:
	// 	<type>|$|memname
	// 
	// It is converted into:
	// 	<selector type="xxx" name="xxx"/>
	
	AosStrSplit split;
	OmnString parts[10];
	bool finished;
	int nn = split.splitStr(def.data(), "|$|", parts, 10, finished);
	
	OmnString selector;
	if (parts[0] == "attr")
	{
		// <selector type="attr" memname="xxx" attrname="xxx"/>
		if (nn == 2)
		{
			selector << "<selector type=\"attr\" memname=\"" << parts[1] << "\"/>";
			return selector;
		}

		if (nn == 3)
		{
			selector << "<selector type=\"attr\" memname=\"" 
				<< parts[1] << "\" attrname=\"" << parts[2] << "\"/>";
			return selector;
		}

		OmnAlarm << "Incorrect selector: " << def << enderr;
		return "";
	}

	if (parts[0] == "xmlattr")
	{
		// <selector type="xmlattr" path="xxx"/>
		selector << "<selector type=\"xmlattr\" path=\"" << parts[1] << "\"/>";
		return selector;
	}

	if (parts[0] == "xmltag")
	{
		// It is the same as xmlattr
		selector << "<selector type=\"xmlattr\" path=\"" << parts[1] << "\"/>";
		return selector;
	}

	if (parts[0] == "objattr")
	{
		if (nn == 1)
			selector << "<selector type=\"objattr\"/>";
		else
			selector << "<selector type=\"objattr\" path=\"" << parts[1] << "\"/>";
		return selector;
	}

	if (parts[0] == "func")
	{
		switch (nn)
		{
		case 2:
			 selector << "<selector type=\"func\" fname=\"" << parts[1] << "\"/>";
			 return selector;

		case 3:
			 selector << "<selector type=\"func\" fname=\"" << parts[1] 
				 << "\" parm1=\"" << parts[2] << "\"/>";
			 return selector;

		case 4:
			 selector << "<selector type=\"func\" fname=\"" << parts[1] 
				 << "\" parm1=\"" << parts[2]
				 << "\" parm2=\"" << parts[3] << "\"/>";
			 return selector;

		case 5:
			 selector << "<selector type=\"func\" fname=\"" << parts[1] 
				 << "\" parm1=\"" << parts[2]
				 << "\" parm2=\"" << parts[3]
				 << "\" parm3=\"" << parts[4] << "\"/>";
			 return selector;

		default:
			 OmnAlarm << "Too many parameters: " << def << enderr;
			 return "";
		}
	}

	OmnAlarm << "Unrecognized selector: " << def << enderr;
	return "";
}


bool
AosConvertAction(
		OmnString &rslts, 
		const OmnString &event, 
		const OmnString &action)
{
	// The function converts the action 'action' into an XML form:
	// 	<action event="xxx" id="xxx">
	// 		...
	// 	</action>
	//
	// Actions are strings of the following format:
	// 	<action_id>(name:value, name:value, ...)
	//
	// '<action_id>' uniquely identifies the action. If an action has
	// arguments, they are defined in "<name>:<value>" pairs, which
	// are separated by ':'.
	//
	// <value> can be in one of the following format:
	// 1. Strings not start with '$':
	//    a. Without '/': constants
	//    b. With '/': 
	//       <selector-type>[:<attribute selector>]/<object selector>
	//       In this form, '/' is used as a special character. It shall 
	//       appear one and only one time.
	// 2. Strings start with '$':
	//       <selector-type>[:<attribute selector>]|vsep164|<object selector>
	//       In this form, '|vsep164|' is used as the separator between
	//       <objectid> and <object selector>. There are no limitations
	//       on what can be in <attribute selector> and <object selector>
	aos_assert_r(event != "", false);
	aos_assert_r(action != "", false);

	const char *data = action.data();

	// Searching for "("
	const char *ptr = strstr(data, "(");
	aos_assert_r(ptr, false);
	OmnString action_id(data, (ptr-data));
	
	data = ptr+1;

	rslts << "<action event=\""
		<< event << "\" id=\"" << action_id << "\">";

	// Actions should be in the form:
	// 	action_id(parm:xxx, parm:xxx, ...)
	bool isLast = false;
	while (!isLast)
	{
		// skip the leading spaces
		while (data[0] && (data[0] == ' ' || data[0] == '\t')) data++;

		// Get Parm Name
		ptr = strstr(data, ":");
		aos_assert_r(ptr, false);
		OmnString pname(data, (ptr-data));

		// Get Parm Definition. It should stop at ',' or ')'
		// If it is ')', it is the end of the action. Need to 
		// consider the case that actions contain actions. 
		data = ptr+1;

		// Now we got the following:
		// 	pname:			parameter name
		// 	objselector:	the object selector
		// 	attrselector:	the attribute selector
		//
		// Time to process the parm
		int endidx = -1;
		aos_assert_r(AosConvertAction_ProcessParm(
			rslts, data, pname, endidx, action), false);
		aos_assert_r(endidx > 0, false);
		data += endidx;
	}

	return true;
}


bool
AosConvertAction_parseParm(
		const char *data, 
		OmnString &selector_type, 
		OmnString &objselector,
		OmnString &attrselector, 
		OmnString &value, 
		int &endidx)
{
	// <value> can be in one of the following format:
	// 1. Strings not start with '$':
	//    a. Without '/': constants
	//    b. With '/': 
	//       <selector-type>[:<attribute selector>]/<object selector>
	//       In this form, '/' is used as a special character. It shall 
	//       appear one and only one time.
	// 2. Strings start with '$':
	//       <selector-type>[:<attribute selector>]|vsep164|<object selector>
	//       In this form, '|vsep164|' is used as the separator between
	//       <objectid> and <object selector>. There are no limitations
	//       on what can be in <attribute selector> and <object selector>
	
	// First, find the end of the parm, which is ended by either ',' 
	// or ')'. 
	char *pp1 = strstr(data, ",");
	char *pp2 = strstr(data, ")");
	char *endptr = (pp1 < pp2)?pp1:pp2;
	aos_assert_r(endptr, false);
	endidx = endptr - data + 1;

	selector_type = "";
	objselector = "";
	attrselector = "";
	value = "";
	char *ptr = 0;
	if (*data == '$')
	{
		// It is the second form. Looking for '|vsep164|'. 
		data++;
		ptr = strstr(data, "|vsep164|");
		if (!ptr)
		{
			// There is no separator. It is constant. 
			selector_type = "const";
			value.assign(data, endidx);
			return true;
		}
		 
		ptr += 9;
	}
	else
	{
		ptr = strstr(data, "/");
		if (!ptr)
		{
			// There is no separator. It is constant. 
			selector_type = "const";
			value.assign(data, endidx);
			return true;
		}
		ptr++;
	}

	aos_assert_r(ptr < endptr, false);

	// The object selector starts at ptr
	objselector.assign(ptr, endidx - (ptr - data));
	if (selector_type == "pane") selector_type = "cpane";

	// Check whether there is attribute selector
	char *ptr1 = strstr(data, ":");
	if (ptr1 < ptr)
	{
		// There is an attribute selector
		ptr1++;
		attrselector.assign(ptr1, ptr-ptr1);
	}
	else 
	{
		attrselector = "";
	}
	return true;
}
		/*
		// It is the first form. Looking for ',' or ')'. 
		// While looking for the parm delimiter, 
		// it will also look for the index of the last ':' or '/'. 
		int idx = 0;
		char cc;
		bool found = false;
		int attrsel_start = -1;
		int objsel_start = -1;
		while (data[idx] && !found)
		{
			switch (data[idx])
			{
			case ':':
				 if (selector_type == "")
				 {
					 selector_type.assign(data, idx);
					 attrsel_start = idx+1;
				 }

				 idx++;
				 break;

			case '/':
				 if (selector_type == "")
				 {
					 // There is no attribute selector
					 selector_type.assign(data, idx);
				 }
				 else
				 {
					 // There is attribute selector
					 aos_assert_r(attrsel_start > 0, false);
					 attrselector.assign(&data[attrsel_start], idx-attrsel_start);
				 }
				 idx++;
				 objsel_start = idx;
				 break;

			case ',':
				 cc = ',';
				 found = true;
				 isLast = false;
				 aos_assert_r(objsel_start > 0, false);
				 objselector.assign(&data[objsel_start], idx-objsel_start);
				 break;

			case ')':
				 cc = ')';
				 found = true;
				 isLast = true;
				 objselector.assign(&data[objsel_start], idx-objsel_start);
				 break;

			default:
				 idx++;
			}
		}

		aos_assert_r(found, false);

		// Check whether it is constant. If 'selector_type' is empty, 
		// it is constant.
		if (selector_type == "")
		{
			// It is a constant:
			// 	<parm name="xxx" id="const">xxx</parm>
			OmnString value(data, idx++);
			rslts << "<parm name=\"" << pname << "\" id=\"const\">"
				<< value << "</parm>";
			continue;
		}
		
		aos_assert_r(objsel_start > 0, false);
		data = &data[objsel_start];

		if (selector_type == "pane") selector_type = "cpane";
}
		*/


bool
AosConvertAction_ProcessParm(
		OmnString &rslts, 
		const char *data,
		const OmnString &pname, 
		int &endidx, 
		const OmnString &action_def)
{
	OmnString selector_type;
	OmnString objselector;
	OmnString attrselector;
	OmnString value;

	aos_assert_r(AosConvertAction_parseParm(data, 
		selector_type, objselector, attrselector, value, endidx), false);

	rslts << "<parm name=\"" << pname << "\" id=\"" << selector_type<< "\"";

	// Now 'data' points to the beginning of <object_selector>
	if (selector_type == "const")
	{
		// It is a constant:
		// 	<parm name="xxx" id="const">xxx</parm>
		OmnString value(data, endidx);
		rslts << "<parm name=\"" << pname << "\" id=\"const\">"
			<< value << "</parm>";
		goto process_attrsel;
	}
		
	if (selector_type == "selfobj")
	{
		// selfobj/[path]
		rslts << "><![CDATA[" << objselector << "]]>";
		goto process_attrsel;
	}

	if (selector_type == "clipvar")
	{
		// clipvar
		// 	clipvar[:attrname]/name|$|sender|$|delete_flag|$|container
		AosStrSplit split;
		OmnString parts[10];
		bool finished;
		int nn = split.splitStr(objselector.data(), "|$|", parts, 10, finished);
		OmnString name = parts[0];
		OmnString sender = (nn > 1)?parts[1]:"";
		OmnString deleteflag = (nn > 2)?parts[2]:"";
		OmnString container = (nn > 3)?parts[3]:"";

		// Parm should be:
		//	<parm name="xxx" id="clipvar" name="xxx"
		//		sender="xxx" deleteflag="xxx" container="xxx"/>
		rslts << "<parm name=\"" << pname << "\" id=\"clipvar\""
			<< " name=\"" << name;
		if (sender != "") rslts << " sender=\"" << sender << "\"";
		if (deleteflag != "") rslts << " deleteflag=\"" << deleteflag << "\"";
		if (container != "") rslts << " container=\"" << container<< "\"";
		rslts << ">";

		goto process_attrsel;
	}

	if (selector_type == "parentobj")
	{
		// parentobj/
		rslts << ">";
		goto process_attrsel;
		return true;
	}

	if (selector_type == "parent_panel")
	{
		// parent_panel
		rslts << ">";
		goto process_attrsel;
		return true;
	}

	if (selector_type == "parent_pane")
	{
		// parent_pane/[num]
		rslts << " num=\"" << objselector << "\">";
		return true;
		goto process_attrsel;
	}

	if (selector_type == "parent")
	{
		// nparent/[flag]
		if (objselector != "")
		{
			rslts << " flag=\"" << objselector << "\">";
		}
		else
		{
			rslts << ">";
		}
		goto process_attrsel;
		return true;
	}

	if (selector_type == "caller")
	{
		rslts << ">";
		goto process_attrsel;
	}

	if (selector_type == "sibling")
	{
		// sibling/gicid
		rslts << " gicid=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "userdata")
	{
		// userdata/idx
		rslts << " idx=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "canvas")
	{
		rslts << ">";
		goto process_attrsel;
	}

	if (selector_type == "system")
	{
		// system/fname
		rslts << " fname=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "sysgic")
	{
		// sysgic/gicname
		rslts << " gicname=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "systemvar")
	{
		// systemvar/name
		rslts << " varname=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "changenotifier")
	{
		rslts << ">";
		goto process_attrsel;
	}

	if (selector_type == "vpd")
	{
		// vpd/fieldname
		rslts << " fname=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "rcdobj")
	{
		// rcdobj/
		rslts << ">";
		goto process_attrsel;
	}

	if (selector_type == "seldataid")
	{
		// seldataid/[@attrname]
		rslts << " aname=\"" << objselector << "\">";
		goto process_attrsel;
	}

	OmnAlarm << "data selector not recognized: " << selector_type 
		<< ". Action: " << action_def << enderr;
	return false;

process_attrsel:
	if (attrselector != "") 
	{
		rslts << AosConvertSelector(attrselector);
	}
	rslts << "</parm>";
	return true;
}


bool
AosConvertAction(OmnString &rslts, const OmnString &actions)
{
	// 'actions' are in the form:
	// 	<event>|$$|<action>|$$|<event>|$$|<action>...
	AosStrSplit split;
	const int max_actions = 20;
	OmnString acts[max_actions];
	bool finished;
	int nn = split.splitStr(actions.data(), "|$$|", acts, max_actions, finished);

	for (int i=0; i<nn; i+=2)
	{
		aos_assert_r(AosConvertAction(rslts, acts[i], acts[i+1]), false);
	}

	return true;
}

