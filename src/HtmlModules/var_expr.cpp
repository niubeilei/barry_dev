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
// 07/19/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/var_expr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "HtmlModules/HmTypes.h"
#include "HtmlModules/HtmlRuntime.h"
#include "HtmlModules/Ptrs.h"
#include "HtmlUtil/HtmlRc.h"
#include "XmlUtil/XmlTag.h"


AosVarEval::AosVarEval()
{
}


AosVarEval::~AosVarEval()
{
}


int 
AosVarEval::retrieveVar(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj, 
		OmnString &value, 
		AosXmlTagPtr &target)
{
	// This function evaluates the parameter defined in 'parm', which 
	// is an XML object of the following format:
	// 	<parm pname="xxx" .../>
	//
	// It may retrieve a value, which is returned through 'value', or 
	// a target (which is a class instance) that is returned through 
	// 'target'. 'obj' is the application object, which may
	// be null. 
	//
	// Returns: 
	// eAosRc_ValueRetrieved:	the retrieved is a value
	// eAosRc_ObjectRetrieved:	the retrieved is an object
	// eAosRc_Error:			indicates an error
	aos_assert_r(parm, false);

	int rslt = retrieveValPriv(runtime, parm, obj, value, target);
	switch (rslt)
	{
	case eAosRc_Error:
		 return rslt;

	case eAosRc_ValueRetrieved:
		 return rslt;

	case eAosRc_ObjectRetrieved:
		 {
		 	aos_assert_r(obj, eAosRc_Failed);
			AosXmlTagPtr selector = parm->getFirstChild("selector");
			if (!selector)
			{
				return eAosRc_ObjectRetrieved;
			}
			return retrieveMember(runtime, parm, obj, value, target);
		 }
		 break;

	default:
		 OmnAlarm << "Unrecognized return code: " << rslt << enderr;
		 return eAosRc_Error;
	}

	OmnShouldNeverComeHere;
	return eAosRc_Error;
}


int 
AosVarEval::retrieveValPriv(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target)
{
	AosParmId type = AosParmId2Enum(parm->getAttrStr("id"));
	switch (type)
	{
	case eAosParmId_CompStr:
		 return composeStr(runtime, parm, obj, value, target);

	case eAosParmId_Const:
		 value = parm->getNodeText();
		 return eAosRc_ValueRetrieved;

	case eAosParmId_Clipvar:
		 return eAosRc_NotAble;

	case eAosParmId_Pane:
		 return retrievePane(runtime, parm, obj, value, target);

	case eAosParmId_Member:
		 return retrieveMember(runtime, parm, obj, value, target);

	case eAosParmId_Sibling:
		 return retrieveSibling(runtime, parm, obj, value, target);

	case eAosParmId_Record:
		 return retrieveRecord(runtime, parm, obj, value, target);

	case eAosParmId_Udata:
		 return retrieveUdata(runtime, parm, obj, value, target);

	case eAosParmId_Canvas:
		 return retrieveCanvas(runtime, parm, obj, value, target);

	case eAosParmId_System:
		 return retrieveSystem(runtime, parm, obj, value, target);

	case eAosParmId_ChangeNotifier:
		 return eAosRc_NotAble;

	case eAosParmId_LoginInfo:
	case eAosParmId_CallFunc:
		 return eAosRc_NotAble;

	default:
		 OmnAlarm << "Unrecognized parm: " << type 
			 << ". Parm: " << parm->toString() << enderr;
		 return eAosRc_Error;
	}

	OmnShouldNeverComeHere;
	return eAosRc_Error;
}


int
AosVarEval::composeStr(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target)
{
	OmnNotImplementedYet;
	return eAosRc_Error;
}


int
AosVarEval::retrieveSystem(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target)
{
	// <parm id="system" mid="xxx"/>
	// where 'mid' can be 'sysgic', 'sysvar', 'func', or 'system'. 
	OmnNotImplementedYet;
	return -1;
	AosParmMid mid = AosParmMid2Enum(parm->getAttrStr("mid"));
	switch (mid)
	{
	case eAosParmMid_Sysgic:
		 {
		 	// This is the same as get element by id. 
			//
			// IMPORTANT: Note that at runtime, 
			// it retrieves the GIC but at HTML time, it retrieves the GIC's
			// VPD, not the GIC.
			OmnString gname = parm->getAttrStr("gname");
			target = runtime->getGicById(gname);
			if (target) return eAosRc_ObjectRetrieved;
			return eAosRc_Error;
		 }

	case eAosParmMid_Sysvar:
		 {
		 	// This is the same as get element by id. 
			//
			// IMPORTANT: Note that at runtime, 
			// it retrieves the variable but at HTML time, it retrieves an 
			// XML (normally a VPD), which may or may not be what the 
			// parm really wants. 
			OmnString id = parm->getAttrStr("vname");
			target = runtime->getVarById(id);
			if (target) return eAosRc_ObjectRetrieved;
			return eAosRc_Error;
		 }

	case eAosParmMid_Func:
		 return eAosRc_NotAble;

	default:
		 return eAosRc_NotAble;
	}

	OmnShouldNeverComeHere;
	return eAosRc_Error;
}


int
AosVarEval::retrieveCanvas(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target)
{
	return eAosRc_NotAble;
}


int
AosVarEval::retrieveUdata(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &target)
{
	// <parm id="udata" idx="xxx"/>
	// userdata/idx
	// Note: for userdata 0, 1, and 2, it is not in
	// cdata.udata[0, 1, 2], but cdata[0][0, 1, 2]
	//
	// In the current implementation, udata cannot be retrieved at HTML time.
	return eAosRc_NotAble;
}


int
AosVarEval::retrievePane(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj,
		OmnString &value, 
		AosXmlTagPtr &object)
{
	/*
	OmnString id = parm->getAttrStr("pid");
	object = AosHmtlRuntime::getSelf()->getPane(id);
	if (!object) return eAosRc_Failed;
	return eAosRc_Ok;
	*/
	OmnNotImplementedYet;
	return -1;
}


int
AosVarEval::retrieveMember(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj, 
		OmnString &value, 
		AosXmlTagPtr &target)
{
	// <parm id="member" mtype="xxx" mid="xxx">xxx</parm>
	OmnString mtype = parm->getAttrStr("mtype");
	AosParmMid mid = AosParmMid2Enum(parm->getAttrStr("@mid"));
	switch (mid)
	{
	case eAosParmMid_Self:
	case eAosParmMid_Caller:
		 // This cannot be retrieved at HTML time
		 return eAosRc_NotAble;
		 break;

	case eAosParmMid_Level:
	case eAosParmMid_Parent:
	case eAosParmMid_Id:
	case eAosParmMid_Window:
		 // This cannot be retrieved at HTML time
		 return eAosRc_NotAble;
		 break;

	case eAosParmMid_Canvas:
		 // This is equivalent to HTML body, but cannot be retrieved
		 // at HTML generation time.
		 return eAosRc_NotAble;
		 break;

	default:
		 OmnAlarm << "Unrecognized MID: " << mid << enderr;
		 return eAosRc_Error;
	}

	return eAosRc_NotAble;
}


int
AosVarEval::retrieveSibling(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj, 
		OmnString &value, 
		AosXmlTagPtr &object)
{
	/*
	// <parm id="sibling" sibling="xxx"/>
	// Note that sibling now is equivalent as "elembyid"
	OmnString sid = parm->getAttrStr("sibling");
	aos_assert_r(sid != "", eAosRc_Error);
	object = AosHtmlRuntime::getSelf()->getElement(sid);
	if (object) return eAosRc_Ok;
	return eAosRc_Failed;
	*/
	OmnNotImplementedYet;
	return -1;
}


int
AosVarEval::retrieveRecord(
		const AosHtmlRuntimePtr &runtime,
		const AosXmlTagPtr &parm, 
		const AosXmlTagPtr &obj, 
		OmnString &value, 
		AosXmlTagPtr &object)
{
	OmnNotImplementedYet;
	return -1;
#if 0
	// <parm id="record" type="xxx">path</parm>
	// The object should be <record><xml><record objid="xxx".../>
	// 
	var record = cdata.udata[0];
	if (!record) return null;
	var type = parm.xpathQuery('@type');
	switch (type)
	{
	case 'rcdobj':
		 return record['obj'];

	case 'comp':
		 var path = parm.getNodeText();
		 if (!record['obj']) return null;
		 if (!path) 
		 {
			var dp = record['obj'].dupePointer();
			if (dp.setXPath("*/*/")) return dp;
			return null;
		 }
				 	  var vv = AosGetXmlValue(record['obj'], "*/*/" + path);
				 	  return vv;

				 case 'member':
				 	  // The object should be <record objid="xxx" ...</record>
					  // Normally, this is used to select objid. If it is tree
					  // node, there may not be an object associated with the record.
					  // It retrieves the objid through 'mDataId'. Note that it
					  // may also be used to retrieve any attribute/texts, similar
					  // to 'comp' except that it assumes the object is not in the form:
					  // 	"<record><xml><object .../></xml></record>"
					  // instead, it should be in the form:
					  //	"<object .../>"
				 	  if (canvas['mDebug']) Debug.write("To get selected seldataid");
				 	  var aname = parm.getNodeText();
				 	  if (!record['obj']) 
				 	  {
				 	 	  if (canvas['mDebug']) Debug.write("Object is null!");
				 	 	  if (canvas['mDebug']) Debug.write("Record: ", record);
					 	  // If it is tree node, objid is in record['mObjid']
					 	  var vv = record['mDataId'];
					 	  if (vv) 
					 	  {
				 	 		  if (canvas['mDebug']) Debug.write("Tree node objid: ", vv);
							  return vv;
					 	  }
					 	  if (canvas['mDebug']) Debug.write("Object is null."); 
					 	  return null;
				 	  }

				 	  if (!aname) aname = AOSTAG_OBJID;
				 	  vv = AosGetXmlValue(record['obj'], aname, "");
				 	  if (canvas['mDebug']) Debug.write("Retrieved, name: ", 
							  aname, "; value: ", vv);
				 	  return vv;

				 default:
				 	  gAosLog.alarm("var_selector.lzx", "002", 
					  	  "Unrecognized record selector type: " + type);
					  return null;
				 }
				 break;
		function AosRetrieveMember(parm, obj, caller)
		{
			<![CDATA[
			// <selector type="xxx" mname="xxx" .../>
			var type = parm.xpathQuery('@type');
			var mname = parm.xpathQuery('@mname');
			if (mname) obj = obj[mname];
			if (!obj) return null;

			switch (type)
			{
			case 'func':
				 // <parm id="func" fname="xxx">
				 //		<parms>
				 //			<parm .../>
				 //			<parm .../>
				 //			...
				 //		</parms>
				 //	</parm>
				 var fname = parm.xpathQuery('@fname');
				 if (!fname) return null;
				 var dp = parm.dupePointer();
				 if (!dp.setXPath("parms") || !dp.selectChild()) 
				 {
				 	 return obj[fname]();
				 }

				 var nn = 1;
				 var parms = OmnNew Array();
				 while (1)
				 {
				  	parms.push(AosRetrieveVal(dp, caller, cdata));
					if (!dp.selectNext()) break;
				 }

				 switch (nn)
				 {
				 case 1:
				 	  return obj[fname](parms[0]);

				 case 2:
					  return obj[fname](parms[0], parms[1]);

				 case 3:
					  return obj[fname](parms[0], parms[1], parms[2]);

				 case 4:
					  return obj[fname](parms[0], parms[1], parms[2], parms[3]);

				 case 5:
					  return obj[fname](parms[0], parms[1], parms[2], parms[3], parms[4]);

				 default:
					  gAosLog.alarm("val_selector.lzx", "003", 
					      "Too many parms: " + nn);
					  return null;
				 }
				 break;

		    case 'attr':
				 // It retrieves the member's attribute. Attribute name is 
				 // identified by 'aname'. Member is identified by 'mname'. 
				 // 	<selector type="attr" mname="xxx" aname="xxx"/>
				 var aname = parm.xpathQuery('@aname');
				 if (!aname) return null;
				 return obj[aname];

			case 'xmlattr':
				 // It retrieves the member's xml value. The member is identified
				 // by 'mname'. The member has to be an xml object. The attribute
				 // is identified by 'aname', which can be either an attribute
				 // name or a path. It is possible to specify a default value.
			     // <selector type="xmlattr" aname="xxx">default value</selector>
				 // 'obj' must be an XML object
				 var dft = parm.getNodeText();
				 return AosGetXmlValue(obj, parm.xpathQuery('@aname'), dft, caller);

			case 'xmltag':
				 // It retrieves the member's subtag. The member is identified
				 // by 'mname'. The member must be an XML object. The path is
				 // identified by 'path'. 
				 //		<selector type="xmltag" path="xxx"/>
				 var path = parm.xpathQuery('@path');
				 if (!path) return null;
				 var dp = obj.dupePointer();
				 if (!dp.setXPath(path)) return null;
				 return dp;

		    default:
			     gAosLog.alarm("val_selector.lzx", "004", 
				 	"Unrecognized attr selector: " + type);
				 return null;
			}
			return null;
			]]>
		}



				 // The format is:
				 //	<parm pname="xxx" id="compstr">
				 //		<value type="const">xxx</value>
				 //		<value type="var">
				 //			<parm .../>
				 //		</value>
				 //		...
				 // </parm>
				 var dp = parm.dupePointer();
				 if (!dp.selectChild()) return null;
				 var value = "";
				 while (1)
				 {
					 switch (dp.xpathQuery('@type'))
					 {
					 case 'const':
						  value += dp.getNodeText();
						  break;
				
					 default:
					 	  var dd = dp.dupePointer();
						  dd.selectChild();
						  var rr = AosRetrieveVar(dd, caller, cdata);
						  if (rr) value += rr;
					 }
					 if (!dp.selectNext()) break;
				 }
				 return value;

	</script>
</library>
#endif
}

#if 0
AosRetrieveVarOld(expr, caller, cdata)
		{
			<![CDATA[
			// It evaluates the expression 'expr'. If 'expr' is not
			// in the form of "<type>/", 'expr' is a literal. It is
			// returned as it is.
			// Otherwise, it is evaluated based on <type>.
			// <type> is in the form: <objid|$$|memberid>. Evaluation
			// is divided
			// in two steps: first, find the object, and then (optional)
			// find the member. 'objid' is used to determine the object
			// and 'memberid' is used to determine the member.
			if (!expr) return null;
			// Note: in the future, we should change '/' to '://'.
			var expr_parts;
			if (expr.charAt(0) == '$')
			{
				expr = expr.substr(1, expr.length-1);
				expr_parts = expr.split('|vsep164|');
			}
			else
			{
				expr_parts = expr.split('/');
			}
			if (expr_parts.length == 1)
			{
				return expr;
			}
			// Separate the object id from member id
			var header = expr_parts[0].split(':');
			if (expr_parts.length > 2)
			{
				// This is because there are extra '/', such as
				// selfobj:xmlattr|$|xml/*/objid/.
				// Need to pack the extra '/' back.
				for (var i=1; i<expr_parts.length-1; i++)
					header[1] += '/' + expr_parts[i];
			}
			var oo = AosRetrieveObjOld(header[0], expr_parts,
			expr, caller, cdata);
			if (!oo) return null;
			if (header.length == 1) return oo;
			var vv = AosRetrieveMemberOld(oo, header[1], expr)
			return vv;
			]]>
		}

		function AosRetrieveObjOld(objid, expr_parts, expr, caller, cdata)
		{
			<![CDATA[
			var oo = null;
			switch (objid)
			{
				case 'compstr':
				// The format is:
				//     "compstr/type|$164$|value|$165$|..."
				var entries = expr_parts[1].split("|$165$|");
				var rslts = "";
				for (var i=0; i<entries.length; i++)
				{
					var pair = entries[i].split("|$164$|");
					switch (pair[0])
					{
						case 'const':
						rslts += pair[1];
						break;
						default:
						var rr = AosRetrieveVarOld(pair[1], caller, cdata);
						rslts += rr;
					}
				}
				//-zky-610--michael--2010/6/9
				var rs = rslts.split('*=*');
				if(rs.length>1)
				{
					rslts=rs[0];
					for(var i=1;i<rs.length ; i++)
					{
						rslts+=','+rs[i];
					}
				}
				return rslts;

				case 'selfobj':
				// selfobj/path
				if (canvas['mDebug']) Debug.write("Get selfobj: ", caller['obj']);
				if (!caller['obj']) return null;
				if (expr_parts.length > 1 && expr_parts[1])
				{
					var vv = AosGetXmlValue(caller.obj, expr_parts[1], "",caller);
					if (canvas['mDebug']) Debug.write("Get selfobj1: ", vv, expr_parts[1]);
					return vv;
				}
				if (canvas['mDebug']) Debug.write("Get selfobj2: ", caller['obj']);
				return caller.obj.dupePointer();

				case 'clipvar':
				// Retrieve a system clipboard variable:
				//     clipvar[:attrname]/name|$|sender|$|delete_flag|$|ctnr
				// both 'sender' and 'delete_flag' are optional.
				var tt = expr_parts[1].split('|$|');
				var sender = (tt.length > 1)?tt[1]:null;
				var container = (tt.length > 3)?tt[3]:null;
				var delete_flag = (tt.length > 2)?(tt[2] == "true"):false;
				oo = gAosClipboard.getVar(tt[0], container,
				sender, delete_flag);
				if (canvas['mDebug']) Debug.write("Get clipvar: ", expr_parts[1], tt[0]);
				break;
				case 'parentobj':
				oo = caller.parent['obj'];
				break;
				case 'parent_panel':
				oo = caller.searchParents('aos_panel_flag');
				if (canvas['mDebug']) Debug.write("parent_panel: ", oo, caller);
				break; 

				case 'parent_pane':
				// parent_pane/[num]
				//james 2010 3 22 ZKY 167
				if (!expr_parts[1])
				{
					oo = caller.searchParents('aos_atomic_pane_flag');
				}
				else
				{
					var nn = Number(expr_parts[1]);
					oo = caller.searchParents('aos_atomic_pane_flag');
					if (!isNaN(nn))
					{
						for (var i=2; i<=nn; i++)
						{
							oo = oo.searchParents("aos_atomic_pane_flag");
							if (!oo) break;
						}
					}
				}
				break;

				case 'parent':
				oo = caller.parent;
				break; 

				case 'nparent':
				// nparent/[flag]
				var flag = (expr_parts.length > 1 && expr_parts[1] != "")?
				expr_parts[1]:"aos_window_flag";
				oo = caller.searchParents(flag);
				if (canvas['mDebug']) Debug.write("To get nparent: ", oo, flag, expr_parts[1], caller);
				break;

				case 'winvpd':
				// winvpd/
				var ww = caller.searchParents("aos_window_flag");
				if (ww) return ww.vpd;
				return null;

				case 'win':
				// win/
				var ww = caller.searchParents("aos_window_flag");
				if (ww) return ww;
				return null;

				case 'caller':
				if (canvas['mDebug']) Debug.write("To retrieve caller: ", caller);
				return caller;

				case 'sibling':
				// sibling/gic_instid
				if (canvas['mDebug']) Debug.write("To get sibling: ", expr_parts[1], ":", caller);
				if (!caller.parent['getGic']) return null;
				var vv = caller.parent.getGic(expr_parts[1]);
				if (canvas['mDebug']) Debug.write("Sibling: ", vv);
				return vv;
				
				case 'pane':
				case 'cpane':
				// cpane/pane_id
				if (expr_parts[1] == "self")
				{
					var pp = caller.searchParents("aos_atomic_pane_flag");
					if (pp) return pp;
					pp = caller.searchParents("aos_panel_flag");
					if (pp) return pp;
					pp = caller.searchParents("aos_window_flag");
					if (pp) return pp;
					return null;
				}

				//ZKY-422 Ice 2010/5/10
				//ww = caller.searchParents('aos_window_flag');
				//if (canvas['mDebug']) Debug.write("Trying ww: ", ww);
				//if (!ww)
				//{
					//      if (canvas['mDebug']) Debug.write("Failed to get window!");
					//  return null;
				//}
				//var pp = ww.getPane(expr_parts[1]);
				var pp=gAosSystem.getPane(expr_parts[1]);
				if (canvas['mDebug']) Debug.write("To retrieve pane, name: ",
					expr_parts[1], "; pane: ", pp);
				return pp;
				
				case 'userdata':
				// userdata/idx
				// Note: for userdata 0, 1, and 2, it is not in
				// cdata.udata[0, 1, 2], but cdata[0][0, 1, 2]
				var idx = Number(expr_parts[1]);
				oo = cdata.udata[idx];
				if (canvas['mDebug']) Debug.write("Get userdata: ", expr_parts[1], ":", oo);
				break;

				case 'canvas':
				oo = canvas;
				break;

				case 'system':
				// system/fname
				if (expr_parts[1] == "self") return gAosSystem;
				oo = gAosSystem[expr_parts[1]]();
				if (canvas['mDebug']) Debug.write("System: ", expr_parts[1], oo);
				break;

				case 'sysgic':
				// sysgic/gicname
				oo = gAosSystem.getGic(expr_parts[1]);
				if (canvas['mDebug']) Debug.write("Get gic: ", expr_parts[1], oo);    //ZKY-165
					break;

				case 'systemvar':
				// systemvar/name
				oo = gAosSystem.getSystemVar(expr_parts[1]);
				break;

				case 'changenotifier':
				if (!caller['obj']) return null;
				var oo = caller.obj.p._change_notifier;
				return oo;

				case 'vpd':
				// vpd/fieldname
				// Tracy, 2010-2-9
				//                               if (expr_parts.length == 2) return caller['vpd'];
				//                               var vv = AosGetXmlAttr(caller['vpd'], expr_parts[1], "");
				var vv = AosGetXmlValue(caller['vpd'], expr_parts[1], "",caller);
				return vv;

				case 'record':
				// record/fieldname
				// The object should be <record><xml><record objid="xxx".../>
				var record = cdata.udata[0];
				var vv = AosGetXmlValue(record['obj'], "*/*/" + expr_parts[1],"",caller);
				return vv;

				case 'rcdobj':
				var record = cdata.udata[0];
				if (canvas['mDebug']) Debug.write("Record obj: ", record);
				if (!record)
				{
					if (canvas['mDebug']) Debug.write("No record found!");
					return null;
				}
				if (canvas['mDebug']) Debug.write("Rcdobj: ", record['obj']);
				return record['obj'];

				case 'seldataid':
				// seldataid/[@attrname]
				// The object should be <record objid="xxx" ...</record>
				if (canvas['mDebug']) Debug.write("To get selected seldataid");
				var record = cdata.udata[0];
				var aname = expr_parts[1];
				if (!record)
				{
					if (canvas['mDebug']) Debug.write("No record found!");
					return null;
				}
				if (!record['obj'])
				{
					if (canvas['mDebug']) Debug.write("Object is null!");
					if (canvas['mDebug']) Debug.write("Record: ", record);
					// If it is tree node, objid is in record['mObjid']
					var vv = record['mDataId'];
					if (vv)
					{
						if (canvas['mDebug']) Debug.write("Tree node objid: ", vv);
						return vv;
					}
					return null;
				}
				//if (!aname) aname = AOSTAG_OBJID;
				if (!aname) aname = AOSTAG_OBJID;
				vv = AosGetXmlValue(record['obj'], aname, "",caller);
				if (canvas['mDebug']) Debug.write("Retrieved, name: ",
					aname, "; value: ", vv);
				return vv; 
				//ZKY-564 Linda 2010/6/7

				case 'logininfo':
				// logininfo/[@attrname]
				if (gAosSystem['mLoginObj'])
				{
					var aname= expr_parts[1];
					if (!aname)
						return gAosSystem.mLoginObj;
					else
						return AosGetXmlValue(gAosSystem.mLoginObj,aname,"");
				}
				else
					return null;
				break;

				default:
				gAosLog.alarm("actions/var_expr.lzx", "003",
				"Unrecognized type: " + objid
				+ ". Expression: " + expr);
				return null;
			}
			return oo;
			]]>
		}

		function AosRetrieveMemberOld(oo, member, expr)
		{
			<![CDATA[
			var pp = member.split("|$|");
			switch (pp[0])
			{ 
				case 'memvpd':
				// cpane:memvpd|$|memname/right_pane
				// "cpane:memvpd|$|current_child/right_pane"
				var mem = oo[pp[1]];
				if (!mem) return null;
				return mem['vpd'];

				case 'attr':
				// Retrieve the attribute of the object 'oo' or the attribute value
				// of a member of the object.
				// Must be in the form:
				//             attr|$|memname[|$|attrname]
				if (canvas['mDebug']) Debug.write("Retrieve mem: ", pp, ":", member, ":", oo[pp[1]]);
				if (pp.length == 2) return oo[pp[1]];
				var mem = oo[pp[1]];
				if (!mem) return null;
				return AosGetXmlValue(mem, pp[2], null);

				case 'xmlattr':
				// Retrieve the attribute of the xml object 'oo'.
				// Must be in the form:
				//             xmlattr|$|path_and_name
				if (pp.length > 1)
				{
					var path = "";
					for (var i=1; i<pp.length; i++)
					{
						if (path != "") path += "/";
						path += pp[i];
					}
					var vv = AosGetXmlValue(oo, path);
					return vv;
				}
				break;

				case 'xmltag':
				// Retrieve a specified tag. Must be in the form:
				//             xmltag|$|path
				if (pp.length > 1)
				{
					var path = "";
					for (var i=1; i<pp.length; i++)
					{
						if (path != "") path += "/";
						path += pp[i];
					}
					oo.setXPath(path);
					return oo;
				}
				break;

				case 'objattr':
				// Retrieve attr from oo['obj'];
				var obj = oo['obj'];u
				if (!obj) return null;
				if (pp.length > 1)
				{
					var path = "";
					for (var i=1; i<pp.length; i++)
					{
						if (path != "") path += "/";
						path += pp[i];
					}
					var vv = AosGetXmlValue(obj, path);
					return vv;
				}
				break; 
			
				case 'func':
				// The value is retrieved by calling the oo's
				// member function.
				//             func|$|fname|$|parm1|$|parm2|$|parm3
				// where 'fname' is the function name. It may pass
				// up to three parameters.
				switch (pp.length)
				{
					case 2:
					var vv = oo[pp[1]]();
					var vv1 = oo['getValue']();
					return vv;

					case 3:
					return oo[pp[1]](pp[2]);

					case 4:
					return oo[pp[1]](pp[2], pp[3]);

					case 5:
					return oo[pp[1]](pp[2], pp[3], pp[4]);
				}
				break;
			}

			gAosLog.alarm("action/var_expr.lzx", "004",
			"Something is wrong in retrieving the variable value: " + expr);
			return null;
		]]>
		}
#endif

