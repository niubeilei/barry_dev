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
// 07/16/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/DclDb.h"

#include "HtmlUtil/HtmlRc.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlReqProc.h"
#include "Query/QueryTerm.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/String.h"

#include <float.h>

static OmnString	sgDefaultDatacolName = "datacol";
static OmnString	sgDftPagesize = "20";

AosDclDb::AosDclDb()
{
}


OmnString
AosDclDb::getJsonConfig(const AosXmlTagPtr &vpd)
{
	return "";
}


AosXmlTagPtr 
AosDclDb::retrieveData(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd,		//where the vpd from? 
		const AosXmlTagPtr &obj,
		const OmnString &tagname)
{
	// The function returns an XML tag with the following
	// format:
	// 	<records>
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 	</records>
	aos_assert_r(vpd, 0);
	OmnString tname = tagname;
	if (tname == "") tname = sgDefaultDatacolName;
	OmnString cmd;
	AosXmlTagPtr datacol_vpd = vpd->getFirstChild(tname);
	if (!datacol_vpd)
	{
		return 0;
	}

	bool defaultLoadData = datacol_vpd->getAttrBool("cmp_defaultLoadData", true);
	if(!defaultLoadData)
	{
		OmnString str = "<response><Contents /></response>";
		AosXmlParser parsers;
		AosXmlTagPtr xml = parsers.parse(str, "" AosMemoryCheckerArgs);
		return xml;
	}
	
	AosRundataPtr rdata = htmlPtr->getRundata();
	
	//felicia , 07/27/2011, for adding zky_compid
	/*htmlPtr->getInstanceId(datacol_vpd);
	
	AosXmlTagPtr vpdroot = vpd->getRoot();
	aos_assert_rr(vpdroot, rdata, false);
	vpdroot = vpdroot->getFirstChild("Contents");
	aos_assert_rr(vpdroot, rdata, false);
	vpdroot = vpdroot->getFirstChild();
	aos_assert_rr(vpdroot, rdata, false);
	OmnString objid = vpdroot->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_rr(objid, rdata, false);
	OmnScreen << "####### objid = " << objid << " ######" << endl;

	//delete the cmp_order = "stime"
	OmnString order = datacol_vpd->getAttrStr("cmp_order");
	if(order == "stime")
	{
		datacol_vpd->removeAttr("cmp_order");
	}

	AosXmlTagPtr dclSub = datacol_vpd->getFirstChild("conds");
	OmnScreen << "==== convert dcl =====" << endl;
	OmnScreen << datacol_vpd->toString() << endl;
	AosXmlTagPtr condSub;
	if(dclSub)
	{
		condSub = dclSub->getFirstChild("cond");
	}
	AosXmlTagPtr termTag;
	if(condSub)
	{
		termTag = condSub->getFirstChild("term");
	}
	if(dclSub && condSub && termTag)
	{
	    //xml query,the format like
	    //<conds>
	    //   <cond type="AND">
	    //       <term ....>
	    //          ...
	    //       </term>
	    //       ...
	   //   </cond>
	   //   ....
	   //</conds>
	   AosXmlTagPtr cond = dclSub->getFirstChild("cond");
	   AosXmlTagPtr term;
	   bool rslt;
	   while(cond)
	   {
	       term = cond->getFirstChild("term");
	       while(term)
           {
               rslt = AosSengAdmin::getSelf()->convertQuery(term, rdata);
               //aos_assert_r(rslt, 0);
			   if(!rslt)
			   {
               		rslt = AosSengAdmin::getSelf()->convertQuery(term, rdata);
			   		return false;
			   }
               term = cond->getNextChild();
           }
           cond = dclSub->getNextChild();
       }
	   OmnScreen << "==== success convert xml dcl =====" << endl;
	   OmnScreen << datacol_vpd->toString() << endl;
    }
    else
	{
		// The code below is used to convert query. It should be removed
		// after the conversions.
		// convert query
		if(dclSub)
		{
			datacol_vpd->removeNode("conds", false, true);
		}

		bool rslt = AosSengAdmin::getSelf()->convertDatacolQuery(datacol_vpd, rdata);
		//aos_assert_r(rslt, false);
		if(!rslt)
		{
			return false;
		}
		
		// convert fnames
		rslt = AosSengAdmin::getSelf()->convertFnames(datacol_vpd, rdata);
		if(!rslt)
		{
			return false;
		}
		//aos_assert_r(rslt, false);

	   	OmnScreen << "==== success convert string  dcl =====" << endl;
	   	OmnScreen << datacol_vpd->toString() << endl;
	

		// End of query conversion.
	}
	*/	

	// if datacol tag is xml format...
	// phnix 04/13/2011
	AosXmlTagPtr datacolSub = datacol_vpd->getFirstChild("conds");
	if (datacolSub)
	{
	/*	datacol_vpd->setAttr("opr", "retlist");
		cmd = datacol_vpd->toString();
		OmnString cmdTmp = "<command>";
		cmdTmp << cmd << "</command>";
		cmd = cmdTmp;
	*/	
		// Tom 2011 06 22 	
		AosXmlTagPtr tmpvpd = datacol_vpd->clone(AosMemoryCheckerArgsBegin);
		tmpvpd->setAttr("reverse", tmpvpd->getAttrStr("cmp_reverse", "false"));
		tmpvpd->setAttr("psize", tmpvpd->getAttrStr("cmp_psize", sgDftPagesize));
		tmpvpd->setAttr("fnames", tmpvpd->getAttrStr("cmp_fnames"));
		tmpvpd->setAttr("order", tmpvpd->getAttrStr("cmp_order"));
		tmpvpd->setAttr("order2", tmpvpd->getAttrStr("cmp_order2"));
		tmpvpd->setAttr("sortflag", tmpvpd->getAttrStr("cmp_sortflag"));
		tmpvpd->setAttr("increase", tmpvpd->getAttrStr("cmp_increase"));
		tmpvpd->setAttr("joins", tmpvpd->getAttrStr("cmp_joins"));
		tmpvpd->setAttr("get_total", "true");
		tmpvpd->setAttr("opr", "retlist");
		cmd = tmpvpd->toString();
		OmnString cmdTmp = "<command>";
		cmdTmp << cmd << "</command>";
		cmd = cmdTmp;
	}
	else
	{
		bool rslt = createQuery(htmlPtr, datacol_vpd, obj, cmd);
		aos_assert_r(rslt, 0);
	}
	// phnix end.....

	AosXmlTagPtr xmls;
	bool rslt = AosSengAdmin::getSelf()->retrieveObjs(rdata->getSiteid(),
			htmlPtr->getSsid(),
			htmlPtr->getUrldocid(),
			"", 
			"", 
			cmd, 
			xmls);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve the data!" << enderr;
		return 0;
	}
	
	if (!xmls)
	{
		OmnAlarm << "Internal error!" << enderr;
		return 0;
	}

	// Tom, 06/01/2011
	// // Check whether it needs to add a sum record
	//OmnString sumrcd = "zky_docid,zky_cntrd";//datacol_vpd->getAttrStr(AOSTAG_SUM_RECORD);
	
	// Tom, 06/08/2011
	// Check whether it needs to collect counters. This is determined by:
	// 	<datadcl ...>
	// 		<counters type="xxx" starttime="xxx" endtime="xxx"/>
	// 		...
	// 	</datadcl>
	AosXmlTagPtr contents = xmls->getFirstChild("Contents");
	if (!contents)
	{
		OmnAlarm << "The response missing contents node" << enderr;
		return 0;
	}

	int total = contents->getAttrInt("total", 0);
	if (total == 0)
	{
		OmnString dftobjid = datacol_vpd->getAttrStr("cmp_dftdata", "");
		if (dftobjid != "")
		{
			AosXmlTagPtr dftdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
									rdata->getSiteid(), 
									htmlPtr->getSsid(), 
									htmlPtr->getUrldocid(), 
									dftobjid);
			if (dftdoc)
			{
				contents = dftdoc;
				xmls->removeNode("Contents", false, false);
				xmls->addNode(contents);
			}
		}
	}

	AosXmlTagPtr counters = datacol_vpd->getFirstChild("counters");
	//AosRundataPtr rdata = OmnNew AosRundata();
	if (counters)
	{
		processCounters(counters, contents, rdata);
	}
	
	parseSumRecord(datacol_vpd, contents);
	parseData(datacol_vpd, contents);
	parseSumRecord(datacol_vpd, contents);
	
	//**************** Tom end *********************//
	return xmls;

	/*
	// Tmp
	OmnString str = 
		"<records>"
			"<record type=\"true\">"
				"<firstname>Ken</firstname>"
				"<lastname>Li</lastname>"
				"<email>ken.li@zykie.com</email>"
			"</record>"
			"<record type=\"true\">"
				"<firstname>John</firstname>"
				"<lastname>Huang</lastname>"
				"<email>john.huang@zykie.com</email>"
			"</record>"
			"<record type=\"false\">"
				"<firstname>Tank</firstname>"
				"<lastname>Wang</lastname>"
				"<email>tank.wang@zykie.com</email>"
			"</record>"
			"<record type=\"false\">"
				"<firstname>Max</firstname>"
				"<lastname>Mao</lastname>"
				"<email>max.mao@zykie.com</email>"
			"</record>"
			"<record type=\"true\">"
				"<firstname>Tracy</firstname>"
				"<lastname>Huang</lastname>"
				"<email>tracy.huang@zykie.com</email>"
			"</record>"
		"</records>";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(str, "");
	return doc;
	*/
}


bool
AosDclDb::createQuery(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		OmnString &cmd) 
{
	// 'vpd' points to "<datacol>", which is in the following format:
	// 	<datacol 
	// 		cmp_query="xxx"
	// 		cmp_query="xxx"
	// 		cmp_order="xxx"
	// 		cmp_dftqry="xxx"
	// 		cmp_fnames="xxx">
	// 	</datacol>
	//
	// This function creates the command for retrieve the data. The command
	// should be in the form:
	//      <command>
	//          <cmd start_idx="xxx"
	//               reverse="true|false"
	//               psize="xxx"
	//               fnames="xxx"
	//               order="xxx"            // Order field name
	//               query="xxx"
	//               get_total="true|false"
	//               opr="retlist"/>
	//      </command>
	//
	
	cmd = "<command>" "<cmd start_idx=\"-1\" ";
	cmd << "reverse=\"" << vpd->getAttrStr("cmp_reverse", "false") << "\" "
		<< "psize=\"" << vpd->getAttrStr("cmp_psize", sgDftPagesize) << "\" "
		<< "fnames=\"" << vpd->getAttrStr("cmp_fnames") << "\" "
		<< "order=\"" << vpd->getAttrStr("cmp_order") << "\" "
		<< "order2=\"" << vpd->getAttrStr("cmp_order2") << "\" "
		<< "sortflag=\"" << vpd->getAttrStr("cmp_sortflag") << "\" "
		<< "increase=\"" << vpd->getAttrStr("cmp_increase") << "\" "
		<< "joins=\"" << vpd->getAttrStr("cmp_joins") << "\" "
		<< "get_total=\"true\" "
		<< "opr=\"retlist\" ";

	OmnString ctnr = getContainer(vpd, obj);
	if (ctnr != "") cmd << "tname=\"" << ctnr << "\" ";

	OmnString tags = getTags(vpd, obj);
	OmnString words = getWords(vpd, obj);

	// 1. Retrieve the query
	OmnString dft_query = vpd->getAttrStr("cmp_query");
	if (dft_query  == "") dft_query = vpd->getAttrStr("cmp_dftqry");
	// phnix 04/06/2011
	if (dft_query.findSubString("clipvar/", 0) != -1)
	{
		deleteClipvar(dft_query);
	}
	OmnString logininfo;
	if (dft_query.findSubString("logininfo/", 0) != -1)
	{
		parseLoginInfo(dft_query, htmlPtr);
	}

	// 2. Check the query binding, which can be bound to either the vpd
	// or obj, depending on the query binding type.
	OmnString query_bind_type = vpd->getAttrStr("cmp_querybt");
	//AosXmlTagPtr boundobj = (query_bind_type == "vpd")?vpd:obj;
	AosXmlTagPtr boundobj = vpd;
	OmnString query_bind = vpd->getAttrStr("cmp_querybd");

	AosXmlTagPtr queryXml;
	OmnString querystr;
	int rslt = AosGetXmlValue(boundobj, query_bind, dft_query, querystr, queryXml);
	aos_assert_r(rslt == eAosRc_Ok, false);
	if (querystr.data()[0]=='<')
	{
		cmd << ">" << querystr;
		AosXmlTagPtr fnames = vpd->getFirstChild("fnames");

		if (fnames)
		{
			cmd << fnames->toString();
		}
	}
	else if (querystr != "")
	{
		cmd << "query=\"" << querystr << "\">";
	}
	else
	{
		// There is no query. If container is empty, this is not a valid query.
	//	if (ctnr == "")
	//	{
			//OmnAlarm << "Invalid query: " << vpd->toString() << enderr;
			//return false;
	//	}
	
		cmd << ">";
	}

	if (tags != "")
	{
		cmd << "<tags type=\"tg\">" << tags << "</tags>";
	}

	if (words != "")
	{
		cmd << "<words type=\"wd\">" << words << "</words>";
	}

	cmd << "</cmd></command>";
OmnScreen << cmd<< endl;
	return true;
}


OmnString
AosDclDb::getContainer(
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj)
{
	OmnString ctnr = vpd->getAttrStr("cmp_tname");
	if (!obj) return ctnr;

	OmnString ctnrbd = vpd->getAttrStr("cmp_tnamebd");
	if (ctnrbd == "") return ctnr;

	AosXmlTagPtr xml;
	OmnString str;
	int rslt = AosGetXmlValue(obj, ctnrbd, ctnr, str, xml);
	if (rslt != eAosRc_Ok) return ctnr;
	if (str == "") return ctnr;
	return str;
}


OmnString 
AosDclDb::getTags(
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj)
{
	OmnString tags = vpd->getAttrStr("cmp_tags");
	if (!obj) return tags;

	OmnString tagsbd = vpd->getAttrStr("cmp_tagsbd");
	if (tagsbd == "") return tags;

	AosXmlTagPtr xml;
	OmnString str;
	int rslt = AosGetXmlValue(obj, tagsbd, tags, str, xml);
	if (rslt != eAosRc_Ok) return tags;
	if (str == "") return tags;
	return str;
}


OmnString 
AosDclDb::getWords(
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj)
{
	OmnString words = vpd->getAttrStr("cmp_words");
	if (!obj) return words;

	OmnString wordsbd = vpd->getAttrStr("cmp_wordsbd");
	if (wordsbd == "") return words;

	AosXmlTagPtr xml;
	OmnString str;
	int rslt = AosGetXmlValue(obj, wordsbd, words, str, xml);
	if (rslt != eAosRc_Ok) return words;
	if (str == "") return words;
	return str;
}

bool
AosDclDb::deleteClipvar(
		OmnString &queryStr)
{
	aos_assert_r(queryStr != "", false);
	AosStrSplit parser(queryStr, "|$$|");
	OmnString subQuery;
	vector<OmnString> queryItem;
	while( (subQuery = parser.nextWord()) != "")
	{
		int pos = subQuery.findSubString("clipvar/", 0);
		if (pos == -1)
		{
			queryItem.push_back(subQuery);
		}
	}
	queryStr = "";
	for (u32 i = 0; i < queryItem.size(); i++)
	{
		queryStr << queryItem[i];
		if (i < queryItem.size() - 1)
		{
			queryStr << "|$$|";
		}
	}
	return true;
}


bool 
AosDclDb::parseLoginInfo(
		OmnString &queryStr,
		const AosHtmlReqProcPtr &htmlPtr)
{
	aos_assert_r(queryStr != "", false);
	AosStrSplit parser(queryStr, "|$$|");
	OmnString subQuery;
	vector<OmnString> queryItem;
	AosXmlTagPtr loginObj = htmlPtr->getLoginInfo();
	aos_assert_r(loginObj, false);
	while( (subQuery = parser.nextWord()) != "")
	{
		int pos = subQuery.findSubString("logininfo/", 0);
		if (pos != -1)
		{
			int endPos = subQuery.findSubString("|$|", pos);
			OmnString info = subQuery.substr(pos, endPos-1);
			AosStrSplit split;
			bool finished;
			OmnString pair[2];
			int nn = split.splitStr(info.data(), "/", pair, 2, finished);
			if (nn != 2)
			{
				OmnAlarm << "This query is mistak..." << enderr;
				return false;
			}
			OmnString subinfo = pair[1];
			OmnString usefulInfo = loginObj->getAttrStr(subinfo, "");
			if (usefulInfo != "")
			{
				subQuery.replace(info, usefulInfo, true);
			}
		}
		queryItem.push_back(subQuery);
	}
	queryStr = "";
	for (u32 i = 0; i < queryItem.size(); i++)
	{
		queryStr << queryItem[i];
		if (i < queryItem.size() - 1)
		{
			queryStr << "|$$|";
		}
	}
	return true;
}

//// Tom 2011 06 07 //////////
int
AosDclDb::parseFml(OmnString &str)
{
	if(str == "")
	{
		return 0;
	}
	OmnString prefix = "$zkyfml{";
	int s_idx = str.findSubString(prefix, 0);
	if(s_idx<0)
	{
		return -2;
	}
	int e_idx = str.findSubString("}", s_idx);
	while(s_idx != -1 && e_idx !=-1)
	{
		int start_idx = s_idx;
		int end_idx = e_idx;
		OmnString v1,v2,v3;
		s_idx += prefix.length();
	
		OmnString tmp = str.substr(s_idx, e_idx-1);
		
		tmp.replace("&nbsp;", "", true);
		tmp.replace(" ", "", true);
		AosStrSplit parser(tmp, ",");
		parser.reset();
		int conter = 3;
		while (parser.hasMore() && conter!=0)
		{
		    OmnString word = parser.nextWord();
			if(word == "") return -1;
			if(conter==3)
			{
				v1 = word;
				if(!isNumStr(v1)) return -1;
			}
			if(conter == 2)
			{
				v2 = word;
			}
			if(conter == 1)
			{
				v3 = word;
				if(!isNumStr(v3)) return -1;
			}
			conter--;
		}
		
		if(conter!=0)
		{
			return -1;	
		}
		OmnString num_str;
		
		if(v2 == "*")
		{
			num_str << atof(v1.data())*atof(v3.data());
		}
		else if(v2 == "+")
		{
			num_str << atof(v1.data())+atof(v3.data());
		}
		else if(v2 == "-")
		{
			num_str << atof(v1.data())-atof(v3.data());
		}
		else if(v2 == "/")
		{
			float ff = atof(v3.data());
			if (ff)
				num_str << atof(v1.data())/ff;
		}
		else
		{
			return -1;
		}
		parseNum(num_str);
		str.replace(start_idx, end_idx-start_idx+1, num_str);
		s_idx = str.findSubString(prefix, s_idx);
		e_idx = str.findSubString("}", s_idx);
	}
	return e_idx;
}


int
AosDclDb::parseNum(OmnString &num_str)
{
	return 1;
	int pt = num_str.findSubString(".",0);
	if(pt != -1)
	{
		OmnString last_str = num_str.substr(pt+1, num_str.length());
		if(atoi(last_str.data())==0){
			if(pt-1==0)
			{
				const char c = num_str.data()[0];
				OmnString t;
				t << c;
				num_str = t;
			}
			else
				num_str = num_str.substr(0, pt-1);
		}else{
			num_str = num_str.substr(0,pt+2);
		}
	}
	return 1;
}


bool
AosDclDb::isNumStr(OmnString &str)
{
	if(str.length()==0)
	{
		return false;
	}
	if(str == "0")
	{
		return true;
	}
	if(atof(str.data())!=0)
	{
		return true;
	}
	return false;
}


int 
AosDclDb::parseData(AosXmlTagPtr &datacol, AosXmlTagPtr &contents)
{
	// <datacol ... cmp_sum_record = "name1,name2">
	// 		<processes>
	// 			<process type="transition" vbdname="mysum" expression="$zkyfml{${@zky_objid},+,${@zky_objid}}">
	// 			<process type="transition" vbdname="mysum" expression="$zkyfml{${@zky_objid},+,$zkysum{zky_objid}}">
	// 		</processes>
	// 		...
	// 		...
	// </datacol>
	if(!datacol || !contents)
	{
		return -1;
	}
	AosXmlTagPtr processes = datacol->getFirstChild("processes");
	if(!processes) return -1;
	AosXmlTagPtr process = processes->getFirstChild();
	while(process)
	{
		OmnString exp = process->getAttrStr("expression");
		OmnString vbdname = process->getAttrStr("vbdname");
		process = processes->getNextChild();
		if(vbdname == "" || exp == "") continue;
		
		parseExp(exp, vbdname, contents);
	}
	return 1;
}


bool 
AosDclDb::parseExp(OmnString &rexp,
						const OmnString &vbdname,
						AosXmlTagPtr &contents)
{
	if(rexp == "" || vbdname =="" || !contents)
	{
		return false;
	}
	AosXmlTagPtr record = contents->getFirstChild();
	while(record)
	{
		OmnString exp = rexp;
		parseBdStr(exp, record); // ${@zky_objid}->number
		parseSum(exp, contents); //$zkysum{zky_objid} ->number
		parseFml(exp);			 // $zkyfml{${@zky_objid},+,$zkysum{zky_objid}} ->number
		
		parseSumOfRow(exp, record);
		
		if(vbdname!="" && isNumStr(exp))
		{
			record->setAttr(vbdname, exp);
		}
		record = contents->getNextChild();
	}
	return true;
}

bool
AosDclDb::parseSum(OmnString & str, AosXmlTagPtr &contents)
{
	if(str == "" || !contents)
	{
		return false;
	}
	OmnString pre = "$zkysum{";
	int p1 = str.findSubString(pre, 0);
	int p2 = str.findSubString("}", p1);
	while(p1 != -1 && p2!=-1)
	{
		OmnString name = str.substr(p1+pre.length(), p2-1);
		name.replace("&nbsp;", "", true);
		name.replace(" ", "", true);
		OmnString value = contents->getAttrStr(name);
		if(isNumStr(value))
		{
			str.replace(p1, p2-p1+1, value);
		}
		p1 = str.findSubString(pre, p1+pre.length());
		p2 = str.findSubString("}",p1);
	}	
	return true;
}
bool
AosDclDb::parseSumOfRow(OmnString & str, const AosXmlTagPtr &record)
{
	OmnString pre = "$zkysumrow{";
	int p1 = str.findSubString(pre, 0);
	int p2 = str.findSubString("}", p1);
	while(p1 != -1 && p2 != -1)
	{
		OmnString names = str.substr(p1+pre.length(), p2-1);

		names.replace("&nbsp;", "", true);
		names.replace(" ", "", true);
		
		AosStrSplit parser(names, ",");
		parser.reset();
		float f = 0;
		while (parser.hasMore())
		{
			OmnString fname = parser.nextWord();
			if(fname != "")
			{
				OmnString value = record->getAttrStr(fname);
				f += atof(value.data());
			}
		}
			
		OmnString num_str;
		num_str<<f;
		str.replace(p1, p2-p1+1, num_str);
		p1 = str.findSubString(pre, p1+pre.length());
		p2 = str.findSubString("}",p1);
	}	
	return true;
}

	
	
	bool
AosDclDb::parseBdStr(OmnString & str, AosXmlTagPtr &record)
{
	if(str == "" || !record)
	{
		return false;
	}
	int p1 = str.findSubString("${", 0);
	int p2 = str.findSubString("}",p1);
	while(p1 != -1 && p2 != -1)
	{
		OmnString cont = str.substr(p1+2, p2-1);
		//name:type:dft
		
		OmnString value;

		AosStrSplit split;
		OmnString parts[3];
		split.splitStr(cont, ":", parts, 3);

		if(parts[1] == "gen")
		{
			value << "${" << parts[0] << "}";
		}
		else if(parts[1] == "noreplace")
		{
			bool Bool;
			value = record->xpathQuery(parts[0], Bool, parts[2]);
			if(value == "")
			{
				value = parts[0];	
			}
		}
		else
		{
			bool Bool;
			value = record->xpathQuery(parts[0], Bool, parts[2]);
		}
		str.replace(p1, p2 - p1+1 , value);

		p1 = str.findSubString("${", p1+1);
		p2 = str.findSubString("}",p1);
	}
	return true;
}
	bool
AosDclDb::parseSumRecord(AosXmlTagPtr &datacol,
						AosXmlTagPtr &contents)
{
	if(!datacol || !contents) return false;
	OmnString cmp_sum_record = datacol->getAttrStr("cmp_sum_record");
	if(cmp_sum_record != "")
	{
		AosStrSplit parser(cmp_sum_record, ",");
		parser.reset();
		while (parser.hasMore())
		{
			OmnString fname = parser.nextWord();
			if (fname != "")
			{
				AosXmlTagPtr record = contents->getFirstChild();
				if(!record) return false;
				float total = 0;
				float max = FLT_MIN;
				float min = FLT_MAX;
				int counter = 0;
				while(record)
				{
					OmnString num_str = record->getAttrStr(fname);
					float nn = atof(num_str.data());
					total += nn;
					if(max < nn) max = nn;
					if(min > nn) min = nn;
					record = contents->getNextChild();
					counter++;
				}
				OmnString name;
				OmnString value;
				//name << "total_" << fname;
				name  << fname;
				value << total;
				contents->setAttr(name, value);
				name = "";
				name << "total_" << fname;
				contents->setAttr(name, value);
				
				name = "", value = "";
				name << "average_" << fname;
				if(counter!=0)
					value << total/float(counter);
				else 
					value << "0";
				contents->setAttr(name, value);

				name = "", value="";
				name << "max_" << fname;
				value << max;
				contents->setAttr(name, value);
				name = "", value="";
				name << "min_" << fname;
				value << min;
				contents->setAttr(name, value);
				name = "", value="";
				name << "counter_" << fname;
				value << counter;
				contents->setAttr(name, value);
			}
		}
	}
	return true;
}


bool
AosDclDb::processCounters(
		const AosXmlTagPtr &counters, 
		AosXmlTagPtr &records, 
		const AosRundataPtr &rdata)
{
	// This function processes counters. 'counters' is an XML tag that defines
	// how to retrieve the counters:
	// 	<counters type="xxx" .../>
	aos_assert_r(counters, false);
	OmnString type = counters->getAttrStr(AOSTAG_TYPE);

	//ken 2012/02/09
	if(type == "") return true;

	if (type == "")
	{
		rdata->setError() << "Missing counter type";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (type == "bymonth")
	{
		// This is to retrieve vaules by month. It assumes:
		// 	<counters ctype="MMYYYY(mmyyyy)"  start="mm.yyyy" end="mm.yyyy" countername="xxx"/>
		// For example:
		// 	<counters type="bymonth" 
		// 		start="2009.11" 
		// 		end="2010.10" 
		// 		fname="values"
		// 		countername="$appvar{industry}.1234"/>
		// 		countername="${01:industry}|$$|docid|$|1234"/>
		// In this example, it retrieves the counters for $appvar{industry}.1234
		// starting from 2009.11 to 2010.10 (totally 13 months)
	//	bool rslt = AosCounterClt::getSelf()->retrieveCounterSet(counters, records, rdata);
	//	if (!ralt) return false;
	}
	return true;
}


// Chen Ding, 09/24/2011
bool
AosDclDb::retrieveContainers(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		const OmnString &tagname,
		AosHtmlCode &code)
{
	// This function retrieves the containers used by this data collector.
	// The data collector is defined by 'vpd'. If 'tagname' is not empty, 
	// it is the name of the tag through which the data collector is defined. 
	// Otherwise, the default tagname 'sgDefaultDatacolName' is used. 
	//
	// Data collector is defined as:
	// 	<datacol ...>
	// 		<conds>
	// 			<cond type="AND" .../>
	// 				<term .../>
	// 				<term .../>
	// 				...
	// 			</cond>
	// 		</conds>
	// 	</datacol>
	//
	// Note that tag names are normally insignificant.
	
	AosRundataPtr rdata = htmlPtr->getRundata();
	OmnString tname = tagname;
	if (tname == "") tname = sgDefaultDatacolName;
	AosXmlTagPtr datacol_vpd = vpd->getFirstChild(tname);
	if (!datacol_vpd)
	{
		// There is no data collector. Do nothing.
		return true;
	}
	
	return true;//AosQueryTerm::retrieveContainersStatic(datacol_vpd, code, rdata);
}


