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
#include "Debug/Debug.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "Util/String.h"
#include "Util/StrSplit.h"

extern bool
AosConvertAction_ProcessParm(
		OmnString &rslts, 
		const char *data,
		const OmnString &pname, 
		int &endidx, 
		bool &isLast,
		const OmnString &action_def, 
		const OmnString &objid,
		OmnString &errmsg);

bool
AosConvertAttrSelector(
		OmnString &rslts,
		const OmnString &def, 
		OmnString &errmsg)
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
	
	if (parts[0] == "attr")
	{
		// "attr|$|member[|$|xmlattr]"
		// <selector type="xmlattr" mname="xxx" aname="xxx"/>
		if (nn == 2)
		{
			rslts << "<selector type=\"attr\" aname=\"" 
				<< parts[1] << "\"/>";
			return true;
		}

		if (nn == 3)
		{
			rslts << "<selector type=\"xmlattr\" mname=\"" 
				<< parts[1] << "\" aname=\"" << parts[2] << "\"/>";
			return true;
		}

		errmsg = "Incorrect attribute selector: ";
		errmsg << parts[0];
		return false;
		return "";
	}

	if (parts[0] == "xmlattr")
	{
		// <selector type="xmlattr" aname="xxx"/>
		rslts << "<selector type=\"xmlattr\" aname=\"" << parts[1] 
			<< "\"/>";
		return true;
	}

	if (parts[0] == "xmltag")
	{
		// It is the same as xmlattr
		rslts << "<selector type=\"xmltag\" path=\"" 
			<< parts[1] << "\"/>";
		return true;
	}

	if (parts[0] == "objattr")
	{
		if (nn == 1)
		{
			errmsg = "Missing objattr!";
			return false;
		}
		rslts << "<selector type=\"xmlattr\" mname=\"obj\" aname=\""
			<< parts[1] << "\"/>";
		return true;
	}

	if (parts[0] == "memvpd")
	{
		// "memvpd|$|memname"
		// <selector type="mem" mtype="vpd" mname="xxx"/>
		rslts << "selector type=\"attr\" aname=\"vpd\" mname=\""
			<< parts[1] << "\"/>";
		return true;
	}

	if (parts[0] == "func")
	{
		switch (nn)
		{
		case 2:
			 rslts << "<selector type=\"func\" fname=\"" << parts[1] << "\"/>";
			 return true;

		case 3:
			 rslts << "<selector type=\"func\" fname=\"" << parts[1] 
				 << "\"><parms><parm id=\"const\">"
				 << parts[2] << "</parm></parms>";
			 return true;

		case 4:
			 rslts << "<selector type=\"func\" fname=\"" << parts[1] << "\"><parms>"
				 << "<parm id=\"const\">" << parts[2] << "</parm>"
				 << "<parm id=\"const\">" << parts[3] << "</parm>"
				 << "</parms>";
			 return true;

		case 5:
			 rslts << "<selector type=\"func\" fname=\"" << parts[1] << "\"><parms>"
				 << "<parm id=\"const\">" << parts[2] << "</parm>"
				 << "<parm id=\"const\">" << parts[3] << "</parm>"
				 << "<parm id=\"const\">" << parts[4] << "</parm>"
				 << "</parms>";
			 return true;

		case 6:
			 rslts << "<selector type=\"func\" fname=\"" << parts[1] << "\"><parms>"
				 << "<parm id=\"const\">" << parts[2] << "</parm>"
				 << "<parm id=\"const\">" << parts[3] << "</parm>"
				 << "<parm id=\"const\">" << parts[4] << "</parm>"
				 << "<parm id=\"const\">" << parts[5] << "</parm>"
				 << "</parms>";
			 return true;

		case 7:
			 rslts << "<selector type=\"func\" fname=\"" << parts[1] << "\"><parms>"
				 << "<parm id=\"const\">" << parts[2] << "</parm>"
				 << "<parm id=\"const\">" << parts[3] << "</parm>"
				 << "<parm id=\"const\">" << parts[4] << "</parm>"
				 << "<parm id=\"const\">" << parts[5] << "</parm>"
				 << "<parm id=\"const\">" << parts[6] << "</parm>"
				 << "</parms>";
			 return true;

		default:
			 errmsg = "Too many parameters in attr selector: ";
			 errmsg << def;
			 return false; 
		}
	}

	errmsg = "Unrecognized attribute selector: ";
	errmsg << parts[0] << ". Def: " << def;
	return false; 
}


bool
AosConvertAction(
		const OmnString &objid,
		OmnString &rslts, 
		const OmnString &event, 
		const OmnString &action)
{
	// The function converts the action 'action' into an XML form:
	// 	<action event="xxx" id="xxx" subid="xxx">
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
	if (!ptr)
	{
		OmnScreen << "Old Action" << objid << ":" << action << endl;
		return true;
	}

	OmnString errmsg;
	aos_assert_r(ptr, false);
	OmnString action_id(data, (ptr-data));
	
	data = ptr+1;

	OmnString oneaction;
	oneaction << "<action event=\""
		<< event << "\" id=\"" << action_id << "\">";

	// Actions should be in the form:
	// 	action_id(parm:xxx, parm:xxx, ...)
	bool isLast = false;
	int numArgs = 0;
	while (!isLast)
	{
		// skip the leading spaces
		while (data[0] && (data[0] == ' ' || data[0] == '\t')) data++;

		// Get Parm Name
		ptr = strstr(data, ":");
		if (!ptr)
		{
			// This happens only when there is no arguments in the
			// action. 
			aos_assert_r(numArgs == 0, false);
			break;
		}

		OmnString pname(data, (ptr-data));

		// Get Parm Definition. It should stop at ',' or ')'
		// If it is ')', it is the end of the action. Need to 
		// consider the case that actions contain actions. 
		data = ptr+1;

		// Skip the white spaces
		while (*data && *data == ' ') data++;

		// Now we got the following:
		// 	pname:			parameter name
		// 	objselector:	the object selector
		// 	attrselector:	the attribute selector
		//
		// Time to process the parm
		int endidx = -1;
		if (!AosConvertAction_ProcessParm(
			oneaction, data, pname, endidx, isLast, action, objid, errmsg))
		{
			OmnScreen << "Failed the action: " << errmsg << ". Objid: "
				<< objid << ". Action: " << action << endl;
			return false;
		}

		aos_assert_r(endidx > 0, false);
		data += endidx;
		numArgs++;
	}
	oneaction << "</action>";

	// Verify the converted action is syntactically correct
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(oneaction, "");
	if (!doc)
	{
		errmsg = "Failed the conversion. Action: ";
		errmsg << action << ". \nConverted: " << oneaction;
		return false;
	}

	rslts << oneaction;
	return true;
}


bool
AosConvertAction_parseParm(
		const char *data, 
		OmnString &selector_type, 
		OmnString &objselector,
		OmnString &attrselector, 
		OmnString &value, 
		int &endidx, 
		bool &isLast, 
		OmnString &errmsg)
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
	isLast = false;
	char *pp1 = (char *)strstr(data, ",");
	char *pp2 = (char *)strstr(data, ")");
	char *endptr = 0;
	if (!pp1)
	{
		endptr = pp2;
		isLast = true;
	}
	else
	{
		if (pp1 < pp2)
		{
			endptr = pp1;
		}
		else
		{
			endptr = pp2;
			isLast = true;
		}
	}

	if (!endptr)
	{
		errmsg = "Missing ')'";
		return false;
	}

	endidx = endptr - data + 1;

	selector_type = "";
	objselector = "";
	attrselector = "";
	value = "";
	char *ptr = 0;
	bool isSpecialSep = false;
	if (*data == '$')
	{
		// It is the second form. Looking for '|vsep164|'. 
		data++;
		isSpecialSep = true;
		ptr = (char *)strstr(data, "|vsep164|");
		if (ptr > endptr) 
		{
			// This means that '|vsep164|' is not in this parm
			ptr = 0;
		}

		if (ptr) 
		{
			ptr += 9;
		}
	}
	else
	{
		ptr = (char *)strstr(data, "/");
		if (ptr > endptr)
		{
			// This means that '/' is not in this param. 
			ptr = 0;
		}
		if (ptr) ptr++;
	}

	if (!ptr)
	{
		// There is no separator. It is constant. 
		selector_type = "const";
		if (isSpecialSep)
		{
			value.assign(data, endidx-2);
		}
		else
		{
			value.assign(data, endidx-1);
		}
		return true;
	}

	aos_assert_r(ptr <= endptr, false);

	// Determine the selector_type. If a ":" is found before 'ptr', 
	// the selector_type is delimited by that ':'. Otherwise, it is
	// delimited by 'ptr'.
	char *colfound = (char *)strstr(data, ":");
	if (colfound && colfound < ptr)
	{
		selector_type.assign(data, colfound - data);
		colfound++;

		if (isSpecialSep)
		{
			attrselector.assign(colfound, ptr-colfound-9);
		}
		else
		{
			attrselector.assign(colfound, ptr-colfound-1);
		}
	}
	else
	{
		if (isSpecialSep)
		{
			selector_type.assign(data, ptr-data-9);
		}
		else
		{
			selector_type.assign(data, ptr-data-1);
		}
		attrselector = "";
	}

	// The object selector starts at ptr
	if (isSpecialSep)
	{
		objselector.assign(ptr, endidx - (ptr - data)-2);
	}
	else
	{
		objselector.assign(ptr, endidx - (ptr - data)-1);
	}
	if (selector_type == "cpane") selector_type = "pane";
	if (selector_type == "selzky_objid") selector_type = "seldataid";
	return true;
}


bool
AosConvertAction_ProcessParm(
		OmnString &rslts, 
		const char *data,
		const OmnString &pname, 
		int &endidx, 
		bool &isLast,
		const OmnString &action_def, 
		const OmnString &objid,
		OmnString &errmsg)
{
	OmnString selector_type;
	OmnString objselector;
	OmnString attrselector;
	OmnString value;

	if (!AosConvertAction_parseParm(data, 
		selector_type, objselector, attrselector, 
		value, endidx, isLast, errmsg)) return false;

	if (pname == "funcs")
	{
		// It is in the form:
		// 		funcs:obj|$|fname|$|parm1|$|parm2...|$$|...
		endidx = 0;
		while (data[endidx] && data[endidx] != ',' && data[endidx] != ')') endidx++;
		if (data[endidx] == ')') isLast = true;
		OmnString value(data, endidx);
		if (value == "parent/|$|reloadObj|$|true")
		{
			rslts << "<parm pname=\"fcs\" id=\"cfunc\">"
				<< "<func>"
					<< "<obj pname=\"obj\" id=\"member\" mid=\"parent\"/>"
					<< "<fname pname=\"fname\" id=\"const\">reloadObj</fname>"
					<< "<args>"
						<< "<arg pname=\"arg\" id=\"const\">true</arg>"
					<< "</args>"
				<< "</func>"
				<< "</parm>";
			return true;
		}

		if (value == "parent/|$|reloadObj|$|false")
		{
			rslts << "<parm pname=\"fcs\" id=\"cfunc\">"
				<< "<func>"
					<< "<obj pname=\"obj\" id=\"member\" mid=\"parent\"/>"
					<< "<fname pname=\"fname\" id=\"const\">reloadObj</fname>"
					<< "<args>"
						<< "<arg pname=\"arg\" id=\"const\">false</arg>"
					<< "</args>"
				<< "</func>"
				<< "</parm>";
			return true;
		}

		if (value == "caller/|$|reloadVpd|$|true")
		{
			rslts << "<parm pname=\"fcs\" id=\"cfunc\">"
				<< "<func>"
					<< "<obj pname=\"obj\" id=\"member\" mid=\"caller\"/>"
					<< "<fname pname=\"fname\" id=\"const\">reloadVpd</fname>"
					<< "<args>"
						<< "<arg pname=\"arg\" id=\"const\">true</arg>"
					<< "</args>"
				<< "</func>"
				<< "</parm>";
			return true;
		}

		OmnAlarm << "Unrecognized funcs: " << enderr;
		return false;
	}

	if ((strncmp(data, "http://", 7) == 0 ||
		 strncmp(data, "https://", 8) == 0) && pname == "url")
	{
		// It is a url. Convert it as a constant:
		// <parm name="url" id="const"><![CDATA[xxx]]></parm>
		// Find the end of the parm, which is ended by either ',' or
		// ')'. 
		endidx = 0;
		while (data[endidx] && data[endidx] != ',' && data[endidx] != ')') endidx++;
		if (data[endidx] == ')') isLast = true;
		OmnString url(data, endidx);
		rslts << "<parm pname=\"url\" id=\"const\"><![CDATA["
			<< url << "]]></parm>";
		return true;
	}

	if ((pname == "postact" || pname == "okactions") && 
		 selector_type == "act_obj_call_memfunc(object")
	{
		// This is an action inside an action. Example:
		// act_create_cobj(object:selfobj/, 
		// 		ctnr:objtable, 
		// 		dialog:true, 
		// 		container:objtable_a000003HN, 
		// 		vvpd:vpd_smseditor, 
		// 		evpd:vpd_smseditor, 
		// 		postact:act_obj_call_memfunc(object:sibling/messagelist1, fname:loadO
		// Need to conver the action in the form:
		// 	<action ...>
		// 		...
		// 		<parm pname="poastact" id="action">
		// 			<action .../>
		// 		</parm>
		// 	</action>
		rslts << "<parm pname=\"" << pname << "\" id=\"action\">";
		char *ptr = (char *)strstr(data, ")");
		ptr++;
		isLast = *ptr == ')'; 
		endidx = ptr - data + 1;

		OmnString action(data, ptr-data);
		AosConvertAction(objid, rslts, "*", action);
		rslts << "</parm>";

		
		return true;
	}

	if (pname == "synattrs")
	{
		// It is in the form:
		// 	<left>|asep326|<right>|asep|<flag>|asep327|<left>|asep326|<right>...
		// It converts to:
		// 	<parm pname="sattr" id="sattr">
		// 		<attr left="xxx" right="xxx"/>
		// 		<attr left="xxx" right="xxx"/>
		// 		...
		// 	</parm>
		AosStrSplit split;
		OmnString parts[10];
		bool finished;

		// Find the end of the parm, which is ended by either ',' or ')'
		endidx = 0;
		while (data[endidx] && data[endidx] != ',' && data[endidx] != ')') endidx++;
		
		OmnString vv(data, endidx);
		int nn = split.splitStr(vv.data(), "|asep327|", parts, 10, finished);

		rslts << "<parm pname=\"sattr\" id=\"sattr\">";
		for (int i=0; i<nn; i++)
		{
			OmnString pair[3];
			int mm = split.splitStr(parts[i], "|asep326|", pair, 2, finished);
			const char *data1 = pair[0].data();
			const char *data2 = pair[1].data();
			int idx1 = (data1[0] == '$')?1:0;
			int idx2 = (data2[0] == '$')?1:0;
			if (mm == 3)
			{
				rslts << "<attr left=\"" << &data1[idx1] << "\" "
					<< "right=\"" << &data2[idx2] << "\" flag=\"true\"/>";
			}
			else
			{
				rslts << "<attr left=\"" << &data1[idx1] << "\" "
					<< "right=\"" << &data2[idx2] << "\"/>";
			}
		}
		rslts << "</parm>";

		if (data[endidx] == ',') endidx++;
		if (data[endidx] == ' ') endidx++;
		return true;
	}

	rslts << "<parm pname=\"" << pname << "\"";

	// Now 'data' points to the beginning of <object_selector>
	if (selector_type == "const")
	{
		// It is a constant:
		// 	<parm pname="xxx" id="const">xxx</parm>
		rslts << " id=\"const\"><![CDATA[" << value << "]]>";
		goto process_attrsel;
	}
		
	if (selector_type == "selfobj")
	{
		// selfobj/[path]
		// <parm pname="xxx" id="member" mtype="obj" mid="self">
		// </parm>
		rslts << " id=\"member\" mtype=\"obj\" mid=\"caller\">";
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
		//	<parm pname="xxx" id="clipvar" 
		//		varname="xxx"
		//		sender="xxx" 
		//		del="true|false" 
		//		ctnr="xxx"/>
		rslts << " id=\"clipvar\" varname=\"" << name << "\"";
		if (sender != "") rslts << " sender=\"" << sender << "\"";
		if (deleteflag == "true") rslts << " del=\"true\"";
		if (container != "") rslts << " ctnr=\"" << container<< "\"";
		rslts << ">";
		goto process_attrsel;
	}

	if (selector_type == "parentobj")
	{
		// <parm pname="xxx" id="member" cname="obj" type="parent"/>
		rslts << " id=\"member\" mname=\"obj\" mid=\"parent\">";
		goto process_attrsel;
	}

	if (selector_type == "parent_panel")
	{
		// parent_panel
		// <parm pname="xxx" id="member" mtype="panel" mid="self"
		rslts << " id=\"member\" mtype=\"panel\" mid=\"caller\">";
		goto process_attrsel;
	}

	if (selector_type == "parent_pane")
	{
		// parent_pane/[num]
		// <parm pname="xxx" id="member" mtype="pane" mid="self"/> or
		// <parm pname="xxx" id="member" mtype="pane" mid="parent" level="nnn"/>
		if (objselector == "")
		{
			rslts << " id=\"member\" mtype=\"pane\" mid=\"caller\">";
		}
		else
		{
			rslts << " id=\"member\" flag=\"aos_atomic_pane_flag\" mid=\"parent\" level=\""
				<< objselector << "\">";
		}
		goto process_attrsel;
	}

	if (selector_type == "pane")
	{
		// pane/pane_id. Convert to:
		// <parm id="pane" pid="xxx"/>
		rslts << " id=\"pane\" pid=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "winobj")
	{
		rslts << " id=\"member\" mtype=\"obj\" mid=\"window\">";
		goto process_attrsel;
	}

	if (selector_type == "winvpd")
	{
		rslts << " id=\"member\" mtype=\"vpd\" type=\"window\">";
		goto process_attrsel;
	}

	if (selector_type == "parent")
	{
		// <parm id="member" mid="parent"/>
		rslts << " id=\"member\" mid=\"parent\">";
		goto process_attrsel;
	}

	if (selector_type == "nparent")
	{
		// nparent/[flag]
		// <parm id="parent" mid="parent" level="nnn"/>
		if (objselector != "")
		{
			rslts << " id=\"member\" mid=\"parent\" level=\"" << objselector << "\">";
		}
		else
		{
			rslts << " id=\"member\" mid=\"parent\">";
		}
		goto process_attrsel;
	}

	if (selector_type == "logininfo")
	{
		// Old format: "logininfo/[membername]
		// New format: "<parm id="logininfo" mid="membername"/>
		rslts << " id=\"logininfo\" mid=\"" << objselector << "\">";
OmnScreen << "Logininfoconverted: " << rslts << endl;
		goto process_attrsel;
	}

	if (selector_type == "caller")
	{
		// <parm id="member" mid="caller"/>
		rslts << " id=\"member\" mid=\"caller\">";
		goto process_attrsel;
	}

	if (selector_type == "sibling")
	{
		// sibling/gicid
		// <parm id="sibling" sibling="xxx">
		rslts << " id=\"sibling\" sibling=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "userdata")
	{
		// userdata/idx
		rslts << " id=\"udata\" idx=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "canvas")
	{
		rslts << " id=\"member\" mid=\"canvas\">";
		goto process_attrsel;
	}

	if (selector_type == "system")
	{
		// system/fname
		// <parm id="system" mid="self|func"/>
		if (objselector == "self")
		{
			rslts << " id=\"system\" mid=\"self\">";
		}
		else
		{
			rslts << " id=\"system\" mid=\"func\" fname=\"" << objselector << "\">";
		}
		goto process_attrsel;
	}

	if (selector_type == "sysgic")
	{
		// sysgic/gicname
		// <parm id="system" type="sysgic" gname="xxx"/>
		rslts << " id=\"system\" type=\"sysgic\" gname=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "systemvar")
	{
		// systemvar/name
		// <parm id="system" type="sysvar" vname="xxx"/>
		rslts << " id=\"system\" type=\"sysvar\" vname=\"" << objselector << "\">";
		goto process_attrsel;
	}

	if (selector_type == "changenotifier")
	{
		rslts << " id=\"chntf\">";
		goto process_attrsel;
	}

	if (selector_type == "vpd")
	{
		// vpd/fieldname
		// <parm id="member" mtype="vpd" mid="self">
		//     <selector type="xmlobj" aname="xxx"/>
		// </parm>
		rslts << " id=\"member\" mtype=\"vpd\" mid=\"caller\">"
			<< "<selector type=\"xmlobj\" aname=\"" << objselector << "\"/>";
		goto process_attrsel;
	}

	if (selector_type == "rcdobj")
	{
		// rcdobj/
		// <parm id="record" type="rcdobj"/>
		rslts << " id=\"record\" type=\"rcdobj\">";
		goto process_attrsel;
	}

	if (selector_type == "seldataid" || 
		selector_type == "selzky_objid" ||
		selector_type == "record")
	{
		// seldataid/[@attrname]
		// <parm id="record" type="member">xxx</parm>
		if (objselector != "")
		{
			rslts << " id=\"record\" type=\"member\">" << objselector;
		}
		else
		{
			rslts << " id=\"record\" type=\"member\">";
		}
		goto process_attrsel;
	}

	errmsg = "Selector type not recognized: ";
	errmsg << selector_type;
	return false;

process_attrsel:
	if (attrselector != "") 
	{
		if (!AosConvertAttrSelector(rslts, attrselector, errmsg)) return false;
	}
	rslts << "</parm>";
	return true;
}

