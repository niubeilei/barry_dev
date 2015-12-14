//////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2005
//
//	Define html compiler util
//	create by lynch yang 2010/05/28
///////////////////////////////////////////////////////////////////////////////////
#include "HtmlServer/HtmlUtil.h"
#include "Alarm/Alarm.h"
#include <string>

using namespace std;

AosHtmlUtil::AosHtmlUtil()
{
}


AosHtmlUtil::~AosHtmlUtil()
{
}


bool
AosHtmlUtil::setOverflowStyle(
		OmnString &style,
		const OmnString ctn_dhsb,
		const OmnString ctn_dvsb)
{
	OmnString overflow ;
	OmnString overflowx ;
	OmnString overflowy ;

	if(ctn_dhsb == "true" && ctn_dvsb == "true")
    {
        overflow = "hidden";
    }
    else if(ctn_dhsb == "false" && ctn_dvsb == "false")
    {
        overflow = "auto";
    }
    else if((ctn_dhsb == "true") && (ctn_dvsb == "false" ))
    {
        overflowx = "hidden";
        overflowy = "auto";
    }
    else if((ctn_dhsb == "false") && (ctn_dvsb == "true" ))
    {
	     overflowx = "auto";
	     overflowy = "hidden";
 	 }
	 else
	 {
		  overflow = "auto";
	 }
     if(overflow != "")
     {
          style << "overflow:" << overflow << ";";
     }
     else
     {
          style << "overflow-x:" << overflowx << ";";
	      style << "overflow-y:" << overflowy << ";";
	 }
	 return true;
}


OmnString
AosHtmlUtil::getWebColor(const OmnString &color)
{
	const int len = color.length();
	const char *data = color.data();
	char a, b, c, d, e, f, g, h;
	OmnString new_color;
	switch(len)
	{
		case 0 :
			 break;
			
		case 1 :
			 a = data[0];
			 if(checkABC(a))
			 {
				new_color << '#' << string(6, a);
			 }
			 break;
		
		case 2 :
			 a = data[0], b = data[1];
			 if(a == '#' && checkABC(b))
			 {
				 new_color << '#' << string(6, b);
			 }
			 else if(checkABC(a) && checkABC(b))
			 {
			 	new_color << '#' << string(3, a) << string(3, b);
			 }
			 break;

		case 3 :
			 a = data[0], b = data[1], c = data[2];
			 if(a == '#' && checkABC(b) && checkABC(c))
			 {
			 	new_color << '#' << string(3, b) << string(3, c);
			 }
			 else if(a == '0' && (b == 'x' || b == 'X') && checkABC(c))
			 {
			 	new_color << '#' << string(3, c);
			 }
			 else if(checkABC(a) && checkABC(b) && checkABC(c))
			 {
			 	new_color << '#' << string(2, a) << string(2, b) << string(2, c);
			 }
			 break;

		case 4 :
			 a = data[0], b = data[1], c = data[2], d = data[3];
			 if(a == '#' && checkABC(b) && checkABC(c) && checkABC(d))
			 {
			 	new_color << '#' << string(2, b) << string(2, c) << string(2, d);
			 }
			 else if(a == '0' && (b == 'x' || b == 'X') && checkABC(c) && checkABC(d))
			 {
			 	new_color << '#' << string(3, c) << string(3, d);
			 }
			 else if(checkABC(a) && checkABC(b) && checkABC(c) && checkABC(d))
			 {
			 	new_color << "#00" << color;
			 }
			 break;

		case 5 :
			 a = data[0], b = data[1], c = data[2], d = data[3], e = data[4];
			 if(data[0] == '#' && checkABC(b) && checkABC(c) && checkABC(d) && checkABC(e))
			 {
			 	new_color << "#00" << color.substr(1, 4);
			 }
			 else if(a == '0' && (b == 'x' || b == 'X') && checkABC(c) && checkABC(d) && checkABC(e))
			 {
			 	new_color << '#' << string(2, c) << string(2, d) << string(2, e);
			 }
			 else if(checkABC(a) && checkABC(b) && checkABC(c) && checkABC(d) && checkABC(e))
			 {
			 	new_color << "#0" << color; 
			 }
			 break;
		case 6 :
			 a = data[0], b = data[1], c = data[2], d = data[3], e = data[4], f = data[5];
			 if(data[0] == '#' && checkABC(b) && checkABC(c) && checkABC(d) && checkABC(e) && checkABC(f))
			 {
			 	new_color << "#0" << color.substr(1, 5);
			 }
			 else if(a == '0' && (b == 'x' || b == 'X') && checkABC(c) && checkABC(d) && checkABC(e) && checkABC(f))
			 {
			 	new_color << "#00" << color.substr(2, 5);
			 }
			 else if(checkABC(a) && checkABC(b) && checkABC(c) && checkABC(d) && checkABC(e) && checkABC(f))
			 {
			 	new_color << '#' << color;
			 }
			 break;
		case 7 :
			 a = data[0], b = data[1], c = data[2], d = data[3], e = data[4], f = data[5], g = data[6];
			 if(data[0] == '#' && checkABC(b) && checkABC(c) && checkABC(d) && checkABC(e) && checkABC(f) && checkABC(g))
			 {
			 	new_color = color; 
			 }
			 else if(a == '0' && (b == 'x' || b == 'X') && checkABC(c) && checkABC(d) && checkABC(e) && checkABC(f) && checkABC(g))
			 {
			 	new_color << "#0" << color.substr(2, 6);
			 }
			 break;
		case 8 :
			 a = data[0], b = data[1], c = data[2], d = data[3], e = data[4], f = data[5], g = data[6], h = data[7];
			 if(a == '0' && (b == 'x' || b == 'X') && checkABC(c) && checkABC(d) && checkABC(e) && checkABC(f) && checkABC(g) && checkABC(h))
			 {
			 	new_color << '#' << color.substr(2, 7); 
			 }
			 break;
	}
	if(new_color == "")
	{
		new_color << color;
	}
	return new_color;
}

	
bool
AosHtmlUtil::checkABC(const char &a)
{
	if((a>='0' && a<='9')
		||(a>='a' && a<='f')
		||(a>='A' && a<='F'))
	{
		return true;
	}
	return false;
}
