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
// 2011/02/19: Created by Ken
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlGrid.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "HtmlModules/DclDb.h"
#include <vector> 
#include <map>
//#include "Porting/GetTime.h"

AosGicHtmlGrid::AosGicHtmlGrid(const bool flag)
:
AosGic(AOSGIC_HTMLGRID, AosGicType::eHtmlGrid, flag)
{
}


AosGicHtmlGrid::~AosGicHtmlGrid()
{
}


bool	
AosGicHtmlGrid::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	// This function will generate:
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code
	// 	4. Flash code
	//test

	
	OmnString grid_table = vpd->getAttrStr("grid_table_css", "");
	OmnString grid_td = vpd->getAttrStr("grid_td_css", "");
	OmnString grid_td_alt = vpd->getAttrStr("grid_td_alt_css", "");
	OmnString grid_th = vpd->getAttrStr("grid_th_css", "");
	OmnString grid_body = vpd->getAttrStr("grid_body_css", "");
	OmnString grid_mouseover_css = vpd->getAttrStr("grid_mouseover_css", "");
	OmnString grid_mousedown_css = vpd->getAttrStr("grid_mousedown_css", "");
	OmnString selectedCell = vpd->getAttrStr("selectedCell", "");
	OmnString gic_groupbd = vpd->getAttrStr("gic_groupbd", "");
	OmnString gic_groupstyle = vpd->getAttrStr("gic_groupstyle", "");
	OmnString gic_filter= vpd->getAttrStr("gic_filter", "");
	bool gic_isgroup = vpd->getAttrBool("gic_isgroup", false);

	if(selectedCell != "")
	{
		int t = selectedCell.findSubString("&amp;", 0);
		if(t != -1)
		{
			selectedCell.replace("&amp;amp;", "$", true);
		}
	}
	
	OmnString grid_css_random;
	grid_css_random << "_" << AosGetHtmlElemId();

	if(grid_table == ""){
		// Ketty 2011/12/15
		//code.mCss <<".grid_table"<< grid_css_random <<"{padding:0;margin:0;width:100%;height:100%;table-layout:fixed;"
		code.mCss <<".grid_table"<< grid_css_random <<"{padding:0;margin:0;width:100%;table-layout:fixed;"
			  			<<"background-color:transparent;border-collapse:collapse;cursor:pointer;"
						<<"font-size:11px;color: #4f6b72;text-align:center;}";
	}else{
		code.mCss << ".grid_table" << grid_css_random <<grid_table;
	}

	if(grid_body == ""){
		code.mCss <<".grid_body"<< grid_css_random <<"{border:#DA70D6 1px solid;}";
	}else{
		code.mCss << ".grid_body" << grid_css_random<< grid_body;
	}
	
	if(grid_td == ""){
		code.mCss <<".grid_td"<< grid_css_random <<" {overflow:hidden;background:#E6E6FA;padding: 0;}";
	}else{
		code.mCss << ".grid_td" << grid_css_random <<grid_td;
	}

	if(grid_td_alt == ""){
		code.mCss <<".grid_td_alt"<< grid_css_random <<" {overflow:hidden;padding: 0; background: #F5FAFA;}";
	}else{
		code.mCss << ".grid_td_alt" << grid_css_random <<grid_td_alt;
	}
	
	if(grid_th == ""){
		code.mCss << ".grid_th"<< grid_css_random <<"{overflow:hidden;text-align:center;height:22px;padding: 0;color: #4f6b72;font-size:15px;background:#ADD8E6  no-repeat;border:#DA70D6 1px solid;}";
	}else{
		code.mCss << ".grid_th" << grid_css_random << grid_th;
	}

	if(grid_mouseover_css == ""){
		code.mCss <<".grid_mouseover"<< grid_css_random <<" {overflow:hidden;background:#CAE8EA;padding: 0;}";
	}else{
		code.mCss << ".grid_mouseover" << grid_css_random <<grid_mouseover_css;
	}
	
	if(grid_mousedown_css == ""){
		code.mCss <<".grid_mousedown"<< grid_css_random <<" {overflow:hidden;background:#B6BAF2;padding: 0;}";
	}else{
		code.mCss << ".grid_mousedown" << grid_css_random <<grid_mousedown_css;
	}
	
	OmnString gic_x = vpd->getAttrStr("gic_x", "0");
	OmnString gic_y = vpd->getAttrStr("gic_y", "0");
	OmnString gic_width = vpd->getAttrStr("gic_width", "0");
	OmnString gic_height = vpd->getAttrStr("gic_height", "0");
	OmnString select_rows=vpd->getAttrStr("select_rows","no");
	OmnString show_snumber = vpd->getAttrStr("show_snumber","false");

	OmnString thStr = "";
	AosXmlTagPtr creators = vpd->getFirstChild("creators");
	if(!creators)
	{
		return false;
	}
	AosXmlTagPtr column = creators->getFirstChild("column");
	int flag = 0;
	while(column){
		OmnString th_width = column->getAttrStr("width","0");
		OmnString cname = column->getAttrStr("name","&nbsp;");
		OmnString cstyle = column->getAttrStr("style","normal");
		if(cname != "&nbsp;"){
			flag++;
		}
		OmnString th_style ="";
		if(th_width !="0"){
			th_style << "style=\"width:" << th_width<<";\"";		
		}
		OmnString tmp_name = cname;
		if(cstyle == "checkbox"){
			tmp_name =  "<input autocomplete=\"off\" disableautocomplete type=\"checkbox\"></input>" ;
		}
		thStr << "<th class=\"grid_th"<< grid_css_random <<"\" "<<th_style<<"><h6>";
		thStr << tmp_name <<"</h6></th>";
		column = creators->getNextChild("column");
	}
	
	OmnString show_title = vpd->getAttrStr("show_title","show");
	OmnString listHtmlCode;
	OmnString th_str = "";
	if(thStr != ""&&show_title=="show"){
		th_str << "<tr>" << thStr <<"</tr>";
	}
	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	OmnString sum_col = vpd->getAttrStr("sum_col");
	OmnString sum_name = vpd->getAttrStr("sum_name", "合计");
	OmnString divHeight = vpd->getAttrStr("gic_height","400");
	OmnString divWidth = vpd->getAttrStr("gic_width","200");
	
	int dH = atoi(divHeight.data());
	int dW = atoi(divWidth.data());
	
	AosXmlTagPtr paging = vpd->getFirstChild("paging");
	OmnString showPaging;
	if(paging){
		showPaging = paging->getAttrStr("page_isshowpaging", "false");
		if(showPaging == "true"){
			dH = dH-20;
		}
	}else{
		showPaging = "false";
	}
	listHtmlCode << "<div style=\"height:"<< dH <<"px;width:"<< dW <<"px;\"><table class=\"grid_table"
				<< grid_css_random<<"\"><thead>"<< th_str<<"</thead><tbody>";
	AosDataColPtr dcl = AosDataCol::getInstance(vpd);
	OmnString data="";
	bool has_data=false;
	// Ketty 2011/12/15
	//bool showNodataTip = vpd->getAttrBool("show_nodata_tip", true);
	bool showNodataTip = true;
	OmnString nodata_tip = vpd->getAttrStr("nodata_tip", "no data");
	if(!dcl)
	{
		if(!showNodataTip)
		{
			createEmptyGrid(vpd ,listHtmlCode, grid_css_random);
		}
	}
	else
	{
		AosXmlTagPtr doc = dcl->retrieveData(htmlPtr, vpd, obj, "");
		if(!doc)
		{
			OmnAlarm << "Failed to retrieve the doc definition: " << vpd->toString() << enderr;
			if(!showNodataTip)
			{
				createEmptyGrid(vpd ,listHtmlCode, grid_css_random);
			}
		}
		else
		{
			AosXmlTagPtr doc_cont = doc->getFirstChild("Contents");
			if(!doc_cont)
			{
				OmnAlarm << "Failed to get the Contents Node form doc" << enderr;
				createEmptyGrid(vpd ,listHtmlCode, grid_css_random);
			}
			else
			{

				
				OmnString cmp_sum_record = datacol->getAttrStr("cmp_sum_record");

				AosXmlTagPtr record = doc_cont->getFirstChild();
				int row = 1;

				if(record)
				{
					has_data = true;
				}
				else if(!showNodataTip)
				{
					createEmptyGrid(vpd ,listHtmlCode, grid_css_random);
				}

				map<OmnString, OmnString> m_sum;
				vector<OmnString> bandname;
				OmnString cmp_psize = datacol->getAttrStr("cmp_psize","20");
				int psize = atoi(cmp_psize.data());
			
				while(record && psize>=row)
				{
					OmnString style;
					AosXmlTagPtr crtrecord = record;
					AosXmlTagPtr nextrecord = doc_cont->getNextChild();
					if (gic_isgroup && nextrecord)
					{
						bool exist;
						OmnString groupid1 = crtrecord->xpathQuery(gic_groupbd, exist, "");
						OmnString groupid2 = nextrecord->xpathQuery(gic_groupbd, exist, "");
						if (groupid1 != "" && groupid2 != "" && groupid1 == groupid2)
						{
							style << "style=\"" << gic_groupstyle << "\"";
						}
					}
					listHtmlCode << "<tr " << style << ">" << createHtmlCode(
							vpd,htmlPtr, crtrecord, row, grid_css_random, bandname) << "</tr>";
					record = nextrecord;
					row++;
				}
				if(cmp_psize!="-1"&&row!=1){
					int af_row = psize - row;
					while(af_row>=0){
						
						listHtmlCode << "<tr>";
						AosXmlTagPtr column = creators->getFirstChild("column");
						int nn=1;	
						while(column){
							AosXmlTagPtr contents = column->getFirstChild("contents");
							OmnString tmp_con = "&nbsp;";
							if(contents){
								OmnString text;
								text = contents->getNodeText();
								OmnString str = generateTemplate(text, htmlPtr);
								int p1 = str.findSubString("<div", 0);
								int p2 = str.findSubString("><div",p1);
								if(p1>=0 && p2>=0) 
									str.replace(p2,str.length()-p2, "></div>");
					//remove simple bind
				    int p3 = str.findSubString("${", 0);
				    int p4 = str.findSubString("}",p3);
					if(p3>=0 && p4>=0)
					{
						str = "&nbsp;"; 
					}
								tmp_con = str;
							}
							OmnString div;
							div <<"<div style=\"width:100%;white-space:normal;word-break:break-all;"
								<<"word-wrap:break-word;overflow:hidden;\">"
								<<tmp_con<<"</div>";
							if(show_snumber == "true" && nn==1)
							{
								div="";
								div<<row++;
							}
							if((psize-af_row)%2==0)
							{
								listHtmlCode << "<td class=\"grid_td_alt"<<grid_css_random 
									<<" grid_body"<<grid_css_random<<"\">"<< div<<"</div></td>";
							}else{
								listHtmlCode << "<td class=\"grid_td"<<grid_css_random
									<<" grid_body"<<grid_css_random<<"\">"<<div<<"</div></td>";
							}
							column = creators->getNextChild("column");
							nn++;			
						}
						listHtmlCode << "</tr>";
						af_row--;
					}
				}	
				if(has_data)
				{
					createStaTab(vpd, doc_cont, listHtmlCode, grid_css_random, bandname, htmlPtr);
				}
				data << doc_cont->toString();
				data.removeNonprintables();
				
			}
		}
	}
	listHtmlCode << "</tbody></table>";
	
	if(!has_data)
	{
		// Ketty 2011/12/15
		//listHtmlCode<<"<div style=\"position:absolute;top:-20px;float:left;overflow:visible;width:"<<divWidth<<"px;height:1px;\">"<<nodata_tip<<"</div>";
		listHtmlCode<<"<div style=\"position:absolute; text-align: center; overflow:visible;width:"<<divWidth<<"px;;\">"<<nodata_tip<<"</div>";
	}
	listHtmlCode <<"</div>";

	code.mHtml <<  listHtmlCode ;
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn", "");
	//OmnString border_size = vpd->getAttrStr("border_size", "1");
	OmnString verborder_size = vpd->getAttrStr("verborder_size", "1");
	OmnString horborder_size = vpd->getAttrStr("horborder_size", "1");
	OmnString row_fixed = vpd->getAttrStr("row_fixed", "free");
	OmnString border_color = vpd->getAttrStr("border_color", "#DA70D6");
	OmnString font_size = vpd->getAttrStr("font_size", "11");
	OmnString font_color = vpd->getAttrStr("font_color", "#4f6b72");
	OmnString text_align = vpd->getAttrStr("text_align", "center");
	OmnString th_height = vpd->getAttrStr("th_height", "22");
	OmnString gic_mberid = vpd->getAttrStr("gic_mberid", "null");
	OmnString gic_grid_bd = vpd->getAttrStr("gic_grid_bd", "");
	OmnString gic_ensunique = vpd->getAttrStr("gic_ensunique", "null");
	OmnString gic_mbrnormzr = vpd->getAttrStr("gic_mbrnormzr", "null");
	OmnString cell_disabled = vpd->getAttrStr("cell_disabled", "atomic|csep624|act_cond(lhs:x,rhs:xx,opr:eq)");
	OmnString selFieldBind = vpd->getAttrStr("sel_field_bind", "");
	OmnString selFieldBound = vpd->getAttrStr("sel_field_bound", "");
	
	
	code.mJson << ",vpdCssRandom:'" << grid_css_random <<"'"
				<< ",gic_groupbd:'" << gic_groupbd <<"'"
				<< ",gic_groupstyle:'" << gic_groupstyle <<"'"
				<< ",gic_filter:'" << gic_filter <<"'"
				<< ",gic_isgroup:" << gic_isgroup
				<< ",gic_lstn:'" << gic_lstn <<"'"
				<< ",select_rows:'" << select_rows <<"'"
				//<< ",border_size:" << border_size 
				<< ",verborder_size:" << verborder_size 
				<< ",horborder_size:" << horborder_size 
				<< ",font_size:" << font_size 
				<< ",font_color:'" << font_color<<"'" 
				<< ",th_height:'" << th_height<<"'" 
				<< ",border_color:'" << border_color <<"'"
				<< ",text_align:'" << text_align <<"'"
				<< ",row_fixed:'" << row_fixed <<"'"
				<< ",showPaging:" << showPaging 
				<< ",show_title:'" << show_title<<"'" 
				<< ",nodata_tip:'" << nodata_tip<<"'" 
				<< ",cell_disabled:'" << cell_disabled<<"'" 
				<< ",gic_mberid:'" << gic_mberid<<"'" 
				<< ",gic_grid_bd:'" << gic_grid_bd<<"'" 
				<< ",gic_ensunique:'" << gic_ensunique<<"'" 
				<< ",gic_grid_bd:'" << gic_grid_bd<<"'"
				<< ",gic_mbrnormzr:'" << gic_mbrnormzr<<"'" 
				<< ",selFieldBind:'" << selFieldBind<<"'" 
				<< ",selFieldBound:'" << selFieldBound<<"'" 
				<< ",sum_col:'" << sum_col <<"'" 
				<< ",sum_name:'" << sum_name <<"'" 
				<< ",selectedCell:'" << selectedCell <<"'" 
				<< ",show_snumber:'" << show_snumber <<"'" 
				<< ",show_total:'" << vpd->getAttrStr("show_total", "false")<<"'"
				<< ",show_average:'" << vpd->getAttrStr("show_average", "false") << "'"
				<< ",show_max:'" << vpd->getAttrStr("show_max", "false") <<"'"
				<< ",show_min:'" << vpd->getAttrStr("show_min", "false") << "'"
				<< ",show_counter:'" << vpd->getAttrStr("show_counter", "false") << "'"
				<< ",gic_sort:" << vpd->getAttrStr("gic_sort", "true")
				<< ",hasmask:" << vpd->getAttrBool("hasmask", false)
				<< ",data:'" << data <<"'";
	return true;
}

bool
AosGicHtmlGrid::createStaTab(const AosXmlTagPtr &vpd, 
								const AosXmlTagPtr &doc_cont,
								OmnString &listHtmlCode,
								const OmnString &grid_css_random,
								const vector<OmnString> &bandname,
								const AosHtmlReqProcPtr &htmlPtr)	
{
	AosXmlTagPtr creators = vpd->getFirstChild("creators");
	OmnString show_total = vpd->getAttrStr("show_total", "false");
	OmnString show_average = vpd->getAttrStr("show_average", "false");
	OmnString show_max = vpd->getAttrStr("show_max", "false");
	OmnString show_min = vpd->getAttrStr("show_min", "false");
	OmnString show_counter = vpd->getAttrStr("show_counter", "false");

//	OmnString show_total="true";
//	OmnString show_average= "true";
//	OmnString show_max = "true";
//	OmnString show_min = "true";
//	OmnString show_counter = "true";
	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	if(!datacol) return false;
	OmnString psize = datacol->getAttrStr("cmp_psize","20");
	
	int cmp_psize = atoi(psize.data());

	int row = 5, row2 = 5;
	while(row){
		int c_num = 0;
		if(show_total != "true" && row == 5)
		{
			row--;
			continue;
		}
		if(show_average != "true" && row == 4)
		{
			row--;
			continue;
		}
		if(show_max != "true" && row == 3)
		{
			row--;
			continue;
		}
		if(show_min != "true" && row == 2)
		{
			row--;
			continue;
		}
		if(show_counter != "true" && row == 1)
		{
			row--;
			continue;
		}
		listHtmlCode <<"<tr>";
		AosXmlTagPtr column = creators->getFirstChild("column");
		while(column )
		{
			OmnString show_name,aname,value="",isShow;
			switch(row)
			{
				case 5:
					show_name = "合计";
					if(bandname[c_num] != "")
					{
						aname << "total_" << bandname[c_num];
						value = doc_cont->getAttrStr(aname);
					}
					isShow = column->getAttrStr("show_total");
					break;
				case 4:
					show_name = "平均值";
					if(bandname[c_num] != "")
					{
						aname << "average_" << bandname[c_num];
						value = doc_cont->getAttrStr(aname);
					}
					isShow = column->getAttrStr("show_average");
					break;
				case 3:
					show_name = "最大值";
					if(bandname[c_num] != "")
					{
						aname << "max_" << bandname[c_num];
						value = doc_cont->getAttrStr(aname);
					}
					isShow = column->getAttrStr("show_max");
					break;
				case 2:
					show_name = "最小值";
					if(bandname[c_num] != "")
					{
						aname << "min_" << bandname[c_num];
						value = doc_cont->getAttrStr(aname);
					}
					isShow = column->getAttrStr("show_min");
					break;
				case 1:
					show_name = "计数";
					if(bandname[c_num] != "")
					{
						aname << "counter_" << bandname[c_num];
						value = doc_cont->getAttrStr(aname);
					}
					isShow = column->getAttrStr("show_counter");
					break;
				default:
					break;
			}
			OmnString classname = "grid_td";
			if((cmp_psize+row2+1)%2!=0)
				classname = "grid_td_alt";
			if(row!=1)
			{
				OmnString r = column->getAttrStr("round");
				parseNum(value, atoi(r.data()));
			}
			
			OmnString tmp_con = "&nbsp;";
			
			if(isShow != "true") value = "&nbsp;";
			if(c_num==0) tmp_con = show_name;
			else
			{
				AosXmlTagPtr contents = column->getFirstChild("contents");
				if(contents)
				{
				    OmnString text;
				    text = contents->getNodeText();
				    OmnString str = generateTemplate(text, htmlPtr);
				    int p1 = str.findSubString("${", 0);
				    int p2 = str.findSubString("</div",p1);

					if (p1 != -1 && p2 != -1 && p2 > p1)
					{
				    	str.replace(p1, p2 - p1 , value);
					}
				    tmp_con = str;
				}
			}
			listHtmlCode << "<td class=\""<<classname<<grid_css_random<<" grid_body"<<grid_css_random<<"\">"<< tmp_con <<"</td>";
			column = creators->getNextChild("column");
			c_num++;
		}
		row--;
		row2--;
		listHtmlCode <<"</tr>";
	}
	return true;
}


bool
AosGicHtmlGrid::createEmptyGrid(const AosXmlTagPtr &vpd, OmnString &listHtmlCode,const OmnString &grid_css_random)
{
//	OmnString gic_row = vpd->getAttrStr("grid_row", "1");
	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	if(!datacol){
		return false;
	}
	OmnString gic_row = datacol->getAttrStr("cmp_psize", "20");
	OmnString gic_col = vpd->getAttrStr("grid_col", "20");
	int r = atoi(gic_row.data());
	int c = atoi(gic_col.data());
	for(int i=0;i<r;i++){
		listHtmlCode << "<tr>";
		AosXmlTagPtr creators = vpd->getFirstChild("creators");
		AosXmlTagPtr column = creators->getFirstChild("column");
		if(column)
		{
			for(int j=0;j<c;j++){
				if(!column)
				{
					continue;
				}
				OmnString td_width = column->getAttrStr("width","0");
				OmnString style = "";
				if(td_width !="0"){
					style << "style=\"width:" << td_width <<"\"";
				}
				OmnString str = "&nbsp;";
				createInnerHtml(column, vpd, str, str, str,vpd);
				
				if(i%2!=0)
				{
					listHtmlCode << "<td isempty=\"true\" class=\"grid_td_alt"<<grid_css_random <<" grid_body"<<grid_css_random<<"\" "<< style<<">" << str << "</td>";
				}else{
					listHtmlCode << "<td class=\"grid_td"<<grid_css_random<<" grid_body"<<grid_css_random<<"\" "<< style <<">" << str << "</td>";
				}
				column = creators->getNextChild("column");
			}
		}
		listHtmlCode << "</tr>";
	}	
	return true;
}

OmnString
AosGicHtmlGrid::createHtmlCode(AosXmlTagPtr &vpd, 
								const AosHtmlReqProcPtr &htmlPtr,	
								const AosXmlTagPtr &record,
								const int row, 
								const OmnString &grid_css_random,
								vector<OmnString> &bandname
								)
{
	OmnString str="";
	OmnString htmlnode="";
	
	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	OmnString cmp_sum_record = datacol->getAttrStr("cmp_sum_record");

	AosXmlTagPtr creators = vpd->getFirstChild("creators");
	AosXmlTagPtr contents;
	OmnString text;
	AosXmlTagPtr column = creators->getFirstChild("column");
	OmnString tmpValue;
	OmnString tmpName;
	int c_num=1;
	while(column){
		contents = column->getFirstChild("contents");
		tmpValue="";
		tmpName="";
		if(contents){
			text = contents->getNodeText();
			str = generateTemplate(text, htmlPtr);
		
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
					value = getRecordValue(record, parts[0], value);
					if(value == "")
					{
						value = parts[0];	
					}
				}
				else
				{
					value = getRecordValue(record, parts[0], parts[2]);
				}
				if(value == "")
				{
					value = "&nbsp;";
				}
				tmpName = str.substr(p1+3,p2-1);
				tmpValue = value;
				if(isNumStr(value))
				{
					OmnString r = column->getAttrStr("round");
					parseNum(value, atoi(r.data()));
				}

				if(cmp_sum_record.indexOf(tmpName, 0)!=-1)
				{
					column->setAttr("show_sum", "true");
				}
				else
				{
					column->setAttr("show_sum", "false");
				}
				str.replace(p1, p2 - p1+1 , value);

				p1 = str.findSubString("${", p1+1);
				p2 = str.findSubString("}",p1);
			}
			OmnString td_width = column->getAttrStr("width","0");
			OmnString td_style = "";
			if(td_width !="0"){
				td_style << "style=\"width:" << td_width <<"\"";
			}
			
			OmnString classname = "";
			if(row%2==0)
			{
				classname << "grid_td_alt"<<grid_css_random;
			}else{
				classname << "grid_td"<<grid_css_random;
			}
			if(row==1)
			{
				bandname.push_back(tmpName);
			}
		
				
			//createInnerHtml(column, record, str, tmpValue,tmpName,vpd);
			
			OmnString show_snumber = vpd->getAttrStr("show_snumber","false");
			if(show_snumber == "true" && c_num==1)
			{
				str = "";
				str<<row;
			}
			
			htmlnode << "<td class=\""<< classname << " grid_body" << grid_css_random <<"\" "<< td_style 
				<<"><div style=\"width: 100%; white-space: normal; word-wrap: "
				<<"break-word; overflow: hidden; text-align: center;\">" << str << "</div></td>";
		}
		column = creators->getNextChild("column");
		c_num++;
	}
	return htmlnode;
}


bool
AosGicHtmlGrid::createInnerHtml(
		const AosXmlTagPtr &column,
		const AosXmlTagPtr &record, 
		OmnString &str, 
		OmnString &value,
		OmnString &name,
		const AosXmlTagPtr &vpd)
{
	OmnString style = column->getAttrStr("style","normal");
	OmnString flag = column->getAttrStr("flag","");

	if(style=="checkbox")
	{
		OmnString checked="";
		if(flag == value && flag!="")
		{
			checked = "checked";
		}
		OmnString checkbox ;
		checkbox << "<div style=\"width:100%;white-space:normal;word-break:break-all;"
			"word-wrap:break-word;overflow:hidden;\">"
			<< "<input autocomplete=\"off\" disableautocomplete type=\"checkbox\" "
			<< checked <<"></input></div>" ;
		str = checkbox;
	}
	else if(style == "input")
	{
		OmnString border_color = vpd->getAttrStr("border_color", "#DA70D6");
		OmnString font_size = vpd->getAttrStr("font_size", "11");
		OmnString font_color = vpd->getAttrStr("font_color", "#4f6b72");
		OmnString text_align = vpd->getAttrStr("text_align", "center");
		OmnString s = "<div style=\"width:100%;white-space:normal;"
			"word-break:break-all;word-wrap:break-word;"
			"overflow:hidden;\"><input autocomplete=\"off\" disableautocomplete type"
			"=\"text\" value=\"";
		s << value<<"\" style=\"width:100%;height:100%;font-size:"<<font_size<<"px;";
		s << "color:"<<font_color<<";text-align:"
			<<text_align<<";border:1px solid "<<border_color<<";\"></input></div>";
		str = s;
	}
	else 
	{
	}
	return true;
}

OmnString
AosGicHtmlGrid::getRecordValue(const AosXmlTagPtr &record, const OmnString &path, OmnString &dft)
{
	bool Bool;
	OmnString str = record->xpathQuery(path, Bool, dft);
	return str;
}

OmnString
AosGicHtmlGrid::generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr)
{
    //int findHandle = cont.findSubString("gAosHtmlHandler",0);
	//if(findHandle != -1)
	//{
	//	htmlPtr->addGic("gAosHtmlHandler");//js file id;
	//}
	int f1 = cont.findSubString("|&&|", 0);
	int f2 = cont.findSubString("|&|", 0);
	int f3 = cont.findSubString("&lt;", 0);
	int f4 = cont.findSubString("&gt;", 0);
	int f5 = cont.findSubString("&#xa;", 0);
	if(f1 != -1)
	{
		cont.replace("|&&|", "\"", true);
	}
	if(f2 != -1){
		cont.replace("|&|", "\'", true);
	}
	if(f3 != -1){
		cont.replace("&lt;", "<", true);
	}
	if(f4 != -1){
		cont.replace("&gt;", ">", true);
	}
	if(f5 != -1){
		cont.replace("&#xa;", "", true);
	}
	if(!htmlPtr->isHomePage())
	{
		bool t = true;
		cont.replace("\'", "\\'", t);
	}
	return cont;
}

int
AosGicHtmlGrid::parseFml(OmnString &str)
{
	OmnString prefix = "$zkyfml{";
	int s_idx = str.findSubString(prefix, 0);
	if(s_idx<0)
	{
		return -2;
	}
	int e_idx = str.findSubString("}", s_idx);
	while(s_idx >0 || e_idx>0)
	{
		int start_idx = s_idx;
		int end_idx = e_idx;
		OmnString v1,v2,v3;
		s_idx += prefix.length();
	
		OmnString tmp = str.substr(s_idx, e_idx-1);
		
		tmp.replace("&nbsp;", "", true);
		tmp.replace(" ", "", true);
		OmnStrParser1 parser(tmp, ",");
		parser.reset();
		int conter = 3;
		while (parser.hasMore() && conter!=0)
		{
		    OmnString word = parser.nextWord();
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
		str.replace(start_idx, end_idx-start_idx+1, num_str);
		s_idx = str.findSubString(prefix, s_idx);
		e_idx = str.findSubString("}", s_idx);
	}
	return e_idx;
}
int
AosGicHtmlGrid::parseNum(OmnString &num_str, int ceil)
{
	if (ceil < 0)
	{
		// This is a configuration of to return a percent.
		int num = atoi(num_str.data());
		aos_assert_r(num>-1, false);
		int n = num*100;
		num_str = "";
		num_str << n << "%";
		return true;
	}
	if (ceil == 0) return true;
	int idx = num_str.findSubString(".", 0);
	if (idx == -1)
	{
		// This mean num_str is a int, do nothing.
		return true;
	}

	// This mean num_str is a float.
	OmnString part[2];
	AosStrSplit split;
	int len = split.splitStr(num_str, ".", part, 2);
	aos_assert_r(len == 2, false);
	aos_assert_r(part[0].data(), false);
	aos_assert_r(part[1].data(), false);
	
	int plen = part[1].length();
	aos_assert_r(plen>0, false);
	
	if (plen < ceil)
	{
		// This mean we should add the adjust "0" to it.
		for (int i=0; i< ceil-plen; i++)
		{
			part[1] << "0";
		}
		num_str = "";
		num_str << part[0] << "." << part[1];
		return true;
	}

	// This mean we should return ceil length of part[1]
	OmnString dStr = part[1].subString(0, ceil);
	num_str = "";
	num_str << part[0] << "." << dStr;
	return true;
}

bool
AosGicHtmlGrid::isNumStr(OmnString &str)
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


///////////////////////////////////////////////////////////////////////////////////////////
int 
AosGicHtmlGrid::parseData(AosXmlTagPtr &datacol,
							AosXmlTagPtr &contents)
{
	// <datacol ... cmp_sum_record = "name1,name2">
	// 		<processes>
	// 			<process type="transition" vbdname="mysum" expression="$zkyfml{${@zky_objid},+,${@zky_objid}}">
	// 			<process type="transition" vbdname="mysum" expression="$zkyfml{${@zky_objid},+,$zkysum{zky_objid}}">
	// 		</processes>
	// 		...
	// 		...
	// </datacol>
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
AosGicHtmlGrid::parseExp(OmnString &rexp,
						const OmnString &vbdname,
						AosXmlTagPtr &contents)
{
	if(rexp == "" || vbdname == "")
	{
		return false;
	}
	AosXmlTagPtr record = contents->getFirstChild();
	while(record)
	{
		OmnString exp = rexp;
		parseBdStr(exp, record);
		parseSum(exp, contents);
		parseFml(exp);

		if(vbdname!="" && isNumStr(exp))
		{
			record->setAttr(vbdname, exp);
		}
		record = contents->getNextChild();
	}
	return true;
}

bool
AosGicHtmlGrid::parseSum(OmnString & str, AosXmlTagPtr &contents)
{
	OmnString pre = "$zkysum{";
	int p1 = str.findSubString(pre, 0);
	int p2 = str.findSubString("}", p1);
	while(p1 >0 && p2>0)
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
AosGicHtmlGrid::parseBdStr(OmnString & str, AosXmlTagPtr &record)
{
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

