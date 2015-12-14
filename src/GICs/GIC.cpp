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
// 06/28/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GICs/GIC.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/XmlTag.h"
#include "HtmlModules/DclDb.h"
#include "HtmlModules/Paging.h"
#include "HtmlModules/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include <math.h>


static OmnMutex sgLock;

AosGic::AosGic(
		const OmnString &name, 
		const AosGicType::E type, 
		const bool flag)
:
mGicType(type),
mName(name)
{
	if (flag)
	{
		OmnString errmsg;
		AosGicPtr thisptr(this, false);
		if (!registerGic(name, thisptr, errmsg))
		{
			OmnThrowException(errmsg);
		}
	}
}


AosGic::AosGic(const AosXmlTagPtr &vpd)
{
}


AosGic::~AosGic()
{
}


bool
AosGic::registerGic(const OmnString &name, const AosGicPtr &gic, OmnString &errmsg)
{
	sgLock.lock();
	if (!AosGicType::isValid(gic->mGicType))
	{
		sgLock.unlock();
		errmsg = "Incorrect reqid: ";
		errmsg << gic->mGicType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (mGics[gic->mGicType])
	{
		sgLock.unlock();
		errmsg = "SeProc already registered: ";
		errmsg << gic->mGicType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	mGics[gic->mGicType] = gic;
	bool rslt = AosGicType::addName(name, gic->mGicType, errmsg);
	sgLock.unlock();
	return rslt;
}


bool
AosGic::createGic(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const OmnString &parentId,
		AosHtmlCode &code, 
		const int pwidth, 
		const int pheight)
{
	OmnString gicid = htmlPtr->getInstanceId(vpd);
	OmnString gictypeStr = vpd->getAttrStrSimp("gic_type");
	AosGicType::E gictype = AosGicType::toEnum(gictypeStr);

	if (!AosGicType::isValid(gictype))
	{
		OmnAlarm << "Unrecognized GIC: " << gictypeStr << enderr;
		return false;
	}

	if(!mGics[gictype])
	{
		OmnAlarm << "GIC not found: " << gictypeStr << enderr;
		return false;
	}
	
	//OmnString gic_event =  vpd->getAttrStrSimp("gic_event","click"); //ken 2011/12/12
	int gic_x =  vpd->getAttrInt("gic_x",0);
	int gic_y = vpd->getAttrInt("gic_y",0);
	int gic_width = vpd->getAttrInt("gic_width",0);
	int gic_height = vpd->getAttrInt("gic_height",0);
	OmnString position = vpd->getAttrStr("gic_position", "absolute");
	if (position == "relative")
	{
		gic_x = 0;
		gic_y = 0;
	}

	// Ketty 2011/09/22
	u32 width_u32 = gic_width, height_u32 = gic_height;
	if(vpd->getAttrStrSimp("gic_width_policy","fixed") == "percent" && pwidth != -1)
	{
		width_u32 = pwidth * width_u32/100;
	}
	if(vpd->getAttrStrSimp("gic_width_policy","fixed") == "rest")
	{
		width_u32 = pwidth - gic_x;
	}
	if(vpd->getAttrStrSimp("gic_height_policy","fixed") == "percent" && pheight != -1)
	{
		height_u32 = pheight * height_u32/100;
	}
	if(vpd->getAttrStrSimp("gic_height_policy","fixed") == "rest")
	{
		height_u32 = pheight - gic_y;
	}
	// Ketty End.

	// get tip text
	OmnString gic_tiptext;
	AosXmlTagPtr flytext_tag = vpd->getFirstChild("flytext");
	if (flytext_tag)
	{
		gic_tiptext = flytext_tag->getNodeText();
	}

	OmnString gic_opacity = vpd->getAttrStrSimp("gic_opacity","1");
	//OmnString gic_layer = vpd->getAttrStrSimp("gic_layer", "1");
	//OmnString gic_enbcond =  vpd->getAttrStrSimp("gic_enbcond", "");
	//OmnString gic_enshow =  vpd->getAttrStrSimp("gic_enshow", "");
	//OmnString gic_base_lstn =  vpd->getAttrStrSimp("gic_base_lstn", "");
	//OmnString gic_movable= vpd->getAttrStrSimp("moving","false");
	//OmnString gic_resizable= vpd->getAttrStrSimp("resizing","false");
	//OmnString gic_notrld= vpd->getAttrStrSimp("gic_notrld","false");
	//OmnString gic_bgcolor = vpd->getAttrStrSimp("gic_bgcolor","transparent");
	//if(gic_bgcolor != "")
	//{
	//	gic_bgcolor = AosHtmlUtil::getWebColor(gic_bgcolor);
	//}
	//OmnString gic_rotation= vpd->getAttrStrSimp("gic_rotation","");
	//OmnString rotatexy= vpd->getAttrStrSimp("rotatexy","0,0");
	//int gic_tabindex= vpd->getAttrInt("gic_tabindex", -1);
	bool gic_framerflag= vpd->getAttrBool("gic_framerflag",false);
	//OmnString gic_binder= vpd->getAttrStrSimp("gic_binder","false");
	//OmnString gic_bounder= vpd->getAttrStrSimp("gic_bounder","false");
	// editor
	OmnString isInEditor;
	isInEditor << htmlPtr->isInEditor();

	// Chen Ding, 08/18/2010
	// If it is 'gic_simple_list', we need to look at the following:
	// 	<gic ...>
	// 		<creators>
	// 			<creator gic_type="gic_html|gic_image" ...>
	// 		</creators>
	// 	</gic>
	if (gictype == AosGicType::eGrid)
	{
		AosXmlTagPtr child = vpd->getFirstChild("creators");
		if (child)
		{
			child = child->getFirstChild();
			if (child)
			{
				//OmnString tt = child->getAttrStrSimp("gic_type");
				if (child->getAttrStrSimp("gic_type") == "gic_image")
				{
					gictype = AosGicType::eListIcon;
					gictypeStr = "gic_iconlist";
				}
			}
		}
	}
	
	//jackie 2011-02-25 creater framer
	//if it is 'gic_html_framer', we need to look at the following:
	// <gic ...>
	// 	 <framer gic_type='gic_html_framer' cmp_sizedef="99:102" cmp_src="8pc010">
	//  </gic>
	
	bool framerflag = false;
	AosGicType::E framertype;
	if(gic_framerflag)
	{
		AosXmlTagPtr framertag = vpd->getFirstChild("framer");
		if(framertag)
		{
			OmnString gicframertypeStr = framertag->getAttrStrSimp("gic_type");
			framertype = AosGicType::toEnum(gicframertypeStr);
			if (AosGicType::isValid(framertype))
			{
				framerflag=true;
			}

		}
	}
	htmlPtr->addGic(gictypeStr);
	htmlPtr->addGicAction(vpd);

	OmnString instid = vpd->getAttrStrSimp("gic_instid");
	int num = htmlPtr->getVpdNum();

	code.mJson << "{"
		<< "el:" << gicid << "\", "
		<< "id:" << gicid << "\", "
		<< "instid:\"" << instid << "\", "  
		<< "gic_position:\"" << position << "\", "  
		<< "isInEditor:" << isInEditor << ", "
	//	<< "gic_event:\"" << gic_event << "\", "	//ken 2011/12/12
		<< "xtype:\"" << gictypeStr << "\", "
		<< "mVpdNum:" << num << ", "
		<< "aosobj:aosobj[" << num << "], "
		<< "x:" << gic_x << ", "
		<< "y:" << gic_y << ", "
		<< "gic_opacity:" << gic_opacity << ", "
		<< "gic_layer:" << vpd->getAttrStrSimp("gic_layer", "1") << ", "
		<< "gic_enbcond:\"" << vpd->getAttrStrSimp("gic_enbcond", "") << "\", "
		<< "gic_enshow:\"" << vpd->getAttrStrSimp("gic_enshow", "") << "\", "
		<< "gic_base_lstn:\"" << vpd->getAttrStrSimp("gic_base_lstn", "") << "\", "
		<< "gic_movable:" << vpd->getAttrStrSimp("moving","false")<< ", "
		<< "gic_resizable:" << vpd->getAttrStrSimp("resizing","false") << ", "
		<< "gic_visible:" << vpd->getAttrStrSimp("gic_visible", "true") << ", "
		<< "gic_tiptype:\"" << vpd->getAttrStrSimp("gic_tiptype","") <<"\", "
		<< "gic_setattrs:\"" << vpd->getAttrStrSimp("gic_setattrs","") <<"\", "
		<< "gic_bindobj:\"" << vpd->getAttrStrSimp("gic_bindobj","") <<"\", "
		<< "gic_creatorid:\"" << vpd->getAttrStrSimp("gic_creatorid","")<<"\", "
		<< "gic_tiptext:\"" << gic_tiptext <<"\", "
		<< "gic_notrld:" << vpd->getAttrStrSimp("gic_notrld","false") <<", "
		<< "gic_bgcolor:\"" << AosHtmlUtil::getWebColor(vpd->getAttrStrSimp("gic_bgcolor","transparent")) <<"\", "
		<< "gic_rotation:\"" << vpd->getAttrStrSimp("gic_rotation","") <<"\", "
		<< "rotatexy:\"" << vpd->getAttrStrSimp("rotatexy","0,0") <<"\", "
		<< "gic_tabindex:" << vpd->getAttrInt("gic_tabindex", -1) << ", "
		<< "gic_framerflag:" << gic_framerflag <<", "
		<< "gic_binder:\"" << vpd->getAttrStrSimp("gic_binder","false") <<"\", "
		<< "gic_bounder:\"" << vpd->getAttrStrSimp("gic_bounder","false") <<"\", "
		//<< "width:"<<gic_width<<", "	
		//<< "height:"<<gic_height << ", "
		<< "width:" << width_u32 <<", "	
		<< "height:" << height_u32 << ", "
		<< "mlVpdWidth:" << gic_width << ", "		//Ketty 2011/09/22
		<< "mlVpdHeight:" << gic_height << ", "	//Ketty 2011/09/22
 		<< "mlWdef:\"" << vpd->getAttrStrSimp("gic_width_policy","fixed")<< "\", "      //Ketty 2011/09/22
		<< "mlHdef:\"" << vpd->getAttrStrSimp("gic_height_policy","fixed") << "\", "   //Ketty 2011/09/22
		<< "gic_font_family:\"" << vpd->getAttrStrSimp("gic_font_family","")<<"\", ";

	//ken 2011-11-24
	//OmnString gic_selfDefAttr = vpd->getAttrStrSimp("gic_selfDefAttr","");
	code.mJson << "gic_selfDefAttr:\"" << vpd->getAttrStrSimp("gic_selfDefAttr","") << "\" ";

	//Ken 2010-12-06
	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	if(datacol)
	{
	    //felicia,2011/08/22,for query convert
		////it must be delete after convert
		/*AosDataColPtr dcl = AosDataCol::getInstance(vpd);
		if(dcl)
		{
		    AosXmlTagPtr doc = dcl->retrieveData(htmlPtr, vpd, obj, "");
		}*/
		
		OmnString cmp = "cmp:{";
		OmnString dcl_type = datacol->getAttrStrSimp("gic_type");
		if(dcl_type == "db_datacol")
		{
			//OmnString cmp_psize = datacol->getAttrStrSimp("cmp_psize", "20");
			//OmnString cmp_joins	= datacol->getAttrStrSimp("cmp_joins");
			//OmnString cmp_tnamebd = datacol->getAttrStrSimp("cmp_tnamebd");
			//OmnString cmp_objpath = datacol->getAttrStrSimp("cmp_objpath");
			//OmnString cmp_dftdata = datacol->getAttrStrSimp("cmp_dftdata");
			//OmnString cmp_order = datacol->getAttrStrSimp("cmp_order");
			//OmnString cmp_cliname = datacol->getAttrStrSimp("cmp_cliname");
			//bool cmp_reverse = datacol->getAttrBool("cmp_reverse", false);
			//bool cmp_nobuffer = datacol->getAttrBool("cmp_nobuffer", false);
			//OmnString cmp_bufferTimeout = datacol->getAttrStrSimp("cmp_bufferTimeout");
			//bool cmp_defaultLoadData = datacol->getAttrBool("cmp_defaultLoadData", false);	//ken 2012/02/09
		
			//need check and delete this attrs;
			//ken 2012/02/14
			//OmnString cmp_tname = datacol->getAttrStrSimp("cmp_tname");
			//OmnString cmp_query = datacol->getAttrStrSimp("cmp_query");
			//OmnString cmp_querybd = datacol->getAttrStrSimp("cmp_querybd");
			//OmnString cmp_fieldbd = datacol->getAttrStrSimp("cmp_fieldbd");
			//OmnString cmp_querybt = datacol->getAttrStrSimp("cmp_querybt");
			//OmnString cmp_datapath = datacol->getAttrStrSimp("cmp_datapath");
			//OmnString cmp_rmmtdt = datacol->getAttrStrSimp("cmp_rmmtdt");
			//OmnString cmp_fnames = datacol->getAttrStrSimp("cmp_fnames");
			//OmnString cmp_qrm = datacol->getAttrStrSimp("cmp_qrm");
			//OmnString cmp_dftqry = datacol->getAttrStrSimp("cmp_dftqry");
			//OmnString cmp_retobj = datacol->getAttrStrSimp("cmp_retobj");
			//OmnString cmp_seldoc = datacol->getAttrStrSimp("cmp_seldoc");
			//OmnString cmp_conntoproc = datacol->getAttrStrSimp("cmp_conntoproc");
			
			cmp << "cmp_psize:\"" << datacol->getAttrStrSimp("cmp_psize", "20") << "\", "
				<< "cmp_joins:\"" << datacol->getAttrStrSimp("cmp_joins") << "\", "
				<< "cmp_tnamebd:\"" << datacol->getAttrStrSimp("cmp_tnamebd") << "\", "
				<< "cmp_objpath:\"" << datacol->getAttrStrSimp("cmp_objpath") << "\", "
				<< "cmp_dftdata:\"" << datacol->getAttrStrSimp("cmp_dftdata") << "\", "
				<< "cmp_order:\"" << datacol->getAttrStrSimp("cmp_order") << "\", "
				<< "cmp_cliname:\"" << datacol->getAttrStrSimp("cmp_cliname") << "\", "
				<< "cmp_reverse:" << datacol->getAttrBool("cmp_reverse", false) << ", "
				<< "cmp_reversevar:\"" << datacol->getAttrStrSimp("cmp_reversevar") << "\", "
				<< "cmp_ordervar:\"" << datacol->getAttrStrSimp("cmp_ordervar") << "\", "
				<< "cmp_nobuffer:" << datacol->getAttrBool("cmp_nobuffer", false) << ", "
				<< "cmp_bufferTimeout:\"" << datacol->getAttrStrSimp("cmp_bufferTimeout") << "\", "
				<< "cmp_defaultLoadData:" << datacol->getAttrBool("cmp_defaultLoadData", true) << ", "		//ken 2012/02/09

			//need check and delete this attrs;
			//ken 2012/02/14
				<< "cmp_tname:\"" << datacol->getAttrStrSimp("cmp_tname") << "\","
				<< "cmp_query:\"" << datacol->getAttrStrSimp("cmp_query") << "\","
				<< "cmp_querybd:\"" << datacol->getAttrStrSimp("cmp_querybd") << "\","
				<< "cmp_fieldbd:\"" << datacol->getAttrStrSimp("cmp_fieldbd") << "\","
				<< "cmp_querybt:\"" << datacol->getAttrStrSimp("cmp_querybt") << "\","
				<< "cmp_datapath:\"" << datacol->getAttrStrSimp("cmp_datapath")<< "\","
				<< "cmp_rmmtdt:\"" << datacol->getAttrStrSimp("cmp_rmmtdt") << "\","
				<< "cmp_fnames:\"" << datacol->getAttrStrSimp("cmp_fnames") << "\","
				<< "cmp_qrm:\"" << datacol->getAttrStrSimp("cmp_qrm") << "\","
				<< "cmp_dftqry:\"" << datacol->getAttrStrSimp("cmp_dftqry") << "\","
				<< "cmp_seldoc:\"" << datacol->getAttrStrSimp("cmp_seldoc") << "\","
				<< "cmp_retobj:\"" << datacol->getAttrStrSimp("cmp_retobj") << "\","
				<< "cmp_conntoproc:\"" << datacol->getAttrStrSimp("cmp_conntoproc") << "\"";
		}
		else if(dcl_type == "str_datacol")
		{
			//OmnString cmp_anames = datacol->getAttrStrSimp("cmp_anames");
			//OmnString cmp_entry_sep = datacol->getAttrStrSimp("cmp_entry_sep", "|$$|");
			//OmnString cmp_field_sep = datacol->getAttrStrSimp("cmp_field_sep", "|$|");
			//OmnString cmp_attr_sep = datacol->getAttrStrSimp("cmp_attr_sep", "|^|");
			//OmnString cmp_contents = datacol->getAttrStrSimp("cmp_contents");
			//OmnString cmp_valbd = datacol->getAttrStrSimp("cmp_valbd");

			cmp << "anames:\"" << datacol->getAttrStrSimp("cmp_anames") << "\","
				<< "rsep:\"" << datacol->getAttrStrSimp("cmp_entry_sep", "|$$|") << "\","
				<< "fsep:\"" << datacol->getAttrStrSimp("cmp_field_sep", "|$|") << "\","
				<< "asep:\"" << datacol->getAttrStrSimp("cmp_attr_sep", "|^|") << "\","
				<< "mContent:\"" << datacol->getAttrStrSimp("cmp_contents") << "\","
				<< "valbd:\"" << datacol->getAttrStrSimp("cmp_valbd") << "\"";
		}
		else if(dcl_type == "embobj_datacol")
		{
			//OmnString cmp_autoid = datacol->getAttrStrSimp("cmp_autoid","false");
			//OmnString cmp_path = datacol->getAttrStrSimp("cmp_path");
			//OmnString cmp_sort = datacol->getAttrStrSimp("cmp_sort");
			//OmnString cmp_filter = datacol->getAttrStrSimp("cmp_filter");
			
			cmp << "cmp_path:\"" << datacol->getAttrStrSimp("cmp_path") << "\","
				<< "cmp_sort:\"" << datacol->getAttrStrSimp("cmp_sort") << "\","
				<< "cmp_filter:\"" << datacol->getAttrStrSimp("cmp_filter")<< "\","
				<< "cmp_autoid:\"" << datacol->getAttrStrSimp("cmp_autoid","false") << "\"";
		}
		cmp << "}";
		code.mJson << ", dcl_type:\"" << dcl_type << "\", " << cmp;
	}
	
	AosXmlTagPtr query = vpd->getFirstChild("query");
	if(query)
	{
		OmnString query_cmp = ", query_cmp:{";
		//OmnString qry_obj = query->getAttrStrSimp("qry_obj","");
		//OmnString qry_name = query->getAttrStrSimp("qry_name","");
		//OmnString qry_dftval = query->getAttrStrSimp("qry_dftval","");
		//OmnString qry_clp = query->getAttrStrSimp("qry_clp","");
		//OmnString qry_cond = query->getAttrStrSimp("qry_cond","AND");
		//OmnString qry_arith = query->getAttrStrSimp("qry_arith","eq");
		query_cmp << "qry_obj:\"" << query->getAttrStrSimp("qry_obj","") << "\", "
				  << "qry_name:\"" << query->getAttrStrSimp("qry_name","") << "\", "
			      << "qry_dftval:\"" << query->getAttrStrSimp("qry_dftval","") << "\", "
			      << "qry_clp:\"" << query->getAttrStrSimp("qry_clp","") << "\", "
			      << "qry_arith:\"" << query->getAttrStrSimp("qry_arith","eq") << "\", "
			      << "qry_cond:\"" << query->getAttrStrSimp("qry_cond","AND") << "\" "
		          << "}";

		code.mJson << query_cmp;
	}
	
	if (parentId !="")
	{
		code.mJson << ", parentId:" << parentId <<"\"";
	}


	if(htmlPtr->isHomePage())
		code.mHtml << "<div id=" << gicid << "\"";
	else
		code.mHtml << "<div id='+" << gicid << "\"+'";


//	code.mHtml << " instid=\"" << instid << "\""
    code.mHtml << " style=\"position:" << position << ";z-index:"
		<< vpd->getAttrStrSimp("gic_layer", "1") << ";width:" 
		<< gic_width << "px;height:" 
		<< gic_height <<"px;left:" 
		<< gic_x << "px;top:" << gic_y << "px;";

	//ken 2011-3-30	
	float opactity = atof(gic_opacity.data());
	if(fabs(opactity - 1) >= 0.000001)
	{
		code.mHtml << "filter:alpha(opacity=" << opactity*100 
			<< ");opacity:" << opactity << ";";
	}
	// Ketty 2011/10/25 for test.
	code.mHtml << "\">";
	//code.mHtml << "\" class=\"x-abs-layout-item\" >";

	//jackie 2011-02-25 add framer
	if(framerflag && gic_framerflag)
	{
		if(mGics[framertype])
		{
			mGics[framertype]->getFramerCode(htmlPtr, vpd, obj, gicid, code);
		}
	}

	mGics[gictype]->generateCode(htmlPtr, vpd, obj, gicid, code);
	if(framerflag && gic_framerflag)
	{
		code.mHtml << "</div>";
	}
	
	// Ken Lee, 2010-08-23
	AosXmlTagPtr crts = vpd->getFirstChild("Creators");
	AosXmlTagPtr crtsfc, pulldown;
	if(crts)
	{
		crtsfc = crts->getFirstChild();
		if(crtsfc) 
		{
			code.mJson << ", items:[";
			while(crtsfc)
			{
				if(crtsfc->getTagname() !="pulldown")
				{
					createGic(htmlPtr, crtsfc, obj, gicid, code, pwidth, pheight);
				}
				crtsfc = crts->getNextChild();
				if(crtsfc) code.mJson << ",";
			}
			code.mJson << "]";
		}
		
		pulldown = crts->getFirstChild("pulldown");
		if(pulldown)
		{
			AosHtmlCode pdcode;
			pulldown->setAttrSimp("gic_type", "gic_pulldownmenu");
			pdcode.mJson << ", pulldown:";	
			createGic(htmlPtr, pulldown, obj, gicid, pdcode, pwidth, pheight);
			code.append(pdcode);
		}
	}

	pulldown = vpd->getFirstChild("pulldown");
	if(pulldown)
	{
		AosHtmlCode pdcode;
		pdcode.mJson << ", pulldown:";
		pulldown->setAttrSimp("gic_type", "gic_pulldownmenu");
		createGic(htmlPtr, pulldown, obj, gicid, pdcode, pwidth, pheight);
		code.append(pdcode);
	}

	AosXmlTagPtr page = vpd->getFirstChild("paging");
	if(page)
	{
		AosPaging pg;
		OmnString attr = pg.retrievePaging("100", page, page, "", "");
		code.mJson << attr;
	}
	code.mHtml << "</div>";
	code.mJson<< "}";
	return true;
}

AosGicPtr 
AosGic::getGic(const AosGicType::E type)
{
	aos_assert_r(AosGicType::isValid(type), 0);
	return mGics[type];
}

/*
OmnString
AosGic::generateVpdCode()
{
	//this function generate a vpd of gics.
	int nn = rand() % eMaxGics;

	OmnString str = generateVpdHead();
	for (int i=0; i<nn; i++)
	{
		str << generateOneGicVpd("");
	}
	str << generateVpdTail();
	return str;
}


OmnString 
AosGic::generateVpdHead()
{
    return "<gic_creators>";
}


OmnString
AosGic::generateVpdTail()
{
    return "</gic_creators>";
}


OmnString 
AosGic::generateOneGicVpd(const OmnString &gictypeStr)
{
	if(gictypeStr == "")
	{
	    while (1)
	    {		
	        int idx = rand() % AosGicType::eMax;
		    if (mGics[idx])
		    {
			    return mGics[AosGicType::eCheckBox]->generateGicVpd();
		    }
	    }
	}
	
	AosGicType gictype = AosGicType_strToCode(gictypeStr);
	aos_assert_r(gictype >= eAosGicType_Invalid ||
				 gictype <= eAosGicType_Max, false);
	if(!mGics[gictype])
	{
		OmnString errmsg = "Failed to retrieve gic";
		errmsg << " : " << gictype ;
        OmnAlarm << errmsg << enderr;
		return "";
	}
	mGics[gictype]->generateGicVpd();
    return "";	
}


OmnString 
AosGic::generateGicVpd()
{
    return "";
}
*/
