#include "HtmlModules/HandleImage.h"

//#include "SEUtil/SeXmlParser.h"


void AosHandleImage::setEndImage(OmnString& htmlcode)
{
	string str(htmlcode.data());
	string r = "(<img[^><]+)(>)";
	string ss = "$1/$2";
	try
	{
		boost::regex reg(r);
		str = boost::regex_replace(str, reg, ss);
		htmlcode = "";
		htmlcode << str;
	}
	catch(const boost::bad_expression& e)
	{
		OmnAlarm << "That's not a valid regular expression!" << enderr;
	}	
}

bool AosHandleImage::processImage(OmnString& htmlcode)
{

	int start = 0;
	setEndImage(htmlcode);	
	int len = htmlcode.length();
	while (start < len && start < eMaxHtmlCodeLength)
	{
		int img_start = htmlcode.findSubString("<img ", start);
		if (img_start < 0) return true;
		if (img_start == start)
		{
			start = img_start + 5;
			continue;
		}

		int div_start = htmlcode.findSubString("<div ", img_start, true);
		//1
		//int div_end = htmlcode.findSubString("</div>", img_start);1
		if (div_start < 0)
		{
			start = img_start + 5;
			continue;
		}

		// We found 
		// 	<div ...>
		// 		<img .../>
		// 	</div>
		// We will convert this one into XML, then process it.
		AosXmlParser parser;
		//this tag will be xml
		//<div ...>
		//	<img .../>
		//</div>
		AosXmlTagPtr tag = parser.parse(htmlcode, div_start AosMemoryCheckerArgs);
		//const char* tt = htmlcode.data();
		//1:2
		//OmnString div;
		//OmnString htmlCopy = htmlcode;
		//div << htmlCopy.substr(div_start, div_end + 5);
		//AosXmlTagPtr tag = parser.parse(div.data(), "");
		if (!tag || !tag->getFirstChild())
		{
			// It is incorrect. 
			start = img_start + 5;
			continue;
		}
		tag = tag->getFirstChild();
		OmnString ttt = tag->toString();
		OmnString tagName = tag->getTagname();
		if (tag->getTagname() != "div" && tag->getTagname() != "DIV")
		{
			start = img_start + 5;
			continue;
		}

		OmnString style = tag->getAttrStr("style");
		if (style == "")
		{
			start = img_start + 5;
			continue;
		}
		
		// Find the width and height.
		int space = style.findSubString(" ", 0);
		int tab = style.findSubString("\t", 0);
		int enter = style.findSubString("\n", 0);
		if(space != -1)
		{
			style.replace(" ", NULL, true);	
		}
		if(tab != -1)
		{
			style.replace("\t", NULL, true);
		}
		if(enter != -1)
		{
			style.replace("\n", NULL, true);
		}
		
		OmnString nameValue, parts[2], widthStr, heightStr;
		AosStrSplit split;
		OmnStrParser1 parsers(style, ";");
		int height, width;
		nameValue = parsers.nextWord();
		while(nameValue != "")
		{
			split.splitStr(nameValue, ":", parts, 2);
			if(parts[0] == "height")
			{
				heightStr = parts[1];
			}
			else if(parts[0] == "width")
			{
				widthStr = parts[1];
			}
			if(widthStr != "" && heightStr != "")
			{
				break;
			}
			nameValue = parsers.nextWord();
		}
		bool exist;
		OmnString src = tag->xpathQuery("img/@src", exist, ""), srcNew;
		if (src == "")
		{
			OmnAlarm << "Failed to retrieve the src!" << enderr;
			start = img_start + 5;
			continue;
		}

		if (heightStr != "" && widthStr != "" && src != "" )
		{
			height = heightStr.toInt();
			width = widthStr.toInt();
			srcNew = changeImgSrc(height, width, src);
			//tag->xpathSetAttr("img/@src", src);
		}
		htmlcode.replace(src, srcNew, false);
		start = img_start + 5;
	}
	return true;
}

OmnString AosHandleImage::changeImgSrc(int height, 
		int width, const OmnString& src)
{	
	OmnString str;
	float max = height > width?height:width;
	string ss(src.data()), stt;
	int last = ss.find_last_of("/"), length = 0;
	char first, second;
	if(last == -1)
	{
	  return str;
	}
	length = ss.length() - 1 - last;
	stt = ss.substr(last+1, length);
	str << ss.substr(0, last);
	first = stt[0];
	second = stt[1];
	switch(first)
	{
		case 'a':
			// There is only thumbnail
			if (max >= 80) {second = 'o';}
			break;                
	    case 'b':
	   		// There are thumbnail and small.
	   		if (max >= 150) {second = 'o';}
	   	 	else if (max >= 80) {second = 's';}
	 		else {second = 't';}
	       	break;
	    case 'c':
	   	 	// There are thumbnail, small, and medium
		   	if (max >= 300) {second = 'o';}
		   	else if (max >= 150) {second = 'm';}
		   	else if (max >= 80) {second = 's';}
		   	else {first = 't';}
		   	break;
		case 'd':
			// There are thumbnail, small, medium, and large
			if (max >= 700) {second = 'o';}
			else if (max >= 300) {second = 'l';}
			else if (max >= 150) {second = 'm';}
			else if (max >= 80) {second = 's';}
			else {second = 't';}
			break;
		case 'e':
			// There are thumbnail, small, medium, large, and huge
			if (max >= 1100) {second = 'o';}
			else if (max >= 700) {second = 'h';}
			else if (max >= 300) {second = 'l';}
			else if (max >= 150) {second = 'm';}
			else if (max >= 80) {second = 's';}
			else {second = 't';}
			break;
	}
	if(second != stt[1])
	{
		stt.replace(1, 1, &second, 1);
	}
	str << '/' << stt;
	return str;
}
