////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: rhcUtil/aosResmgrXml.h
// Description:
//   
//
// Modification History:
// 2007-04-30 Created by Allen
// 2007-04-30 Debuged by CHK
////////////////////////////////////////////////////////////////////////////

#include "rhcUtil/aos_resmgr_xml.h"
#include "TinyXml/TinyXml.h"

const char NODE_STATISTICSDATA[] = "StatisticsData";
const char NODE_CPUMGRSTATITEMNR[] = "CpuMgrStatItemNr";
const char NODE_CPUMGRSTATRSLT[] = "CpuMgrStatRslt";

const int CHILD_NODE_NUM_CPUMGRSTATRSLT = 17;
const char * CHILD_NODE_CPUMGRSTATRSLT [CHILD_NODE_NUM_CPUMGRSTATRSLT] = 
{
	"SecIndex",
	"CpuUsage",
	"TotalJeffie",
	"CpuUsr",
	"CpuNice",
	"CpuSystem",
	"CpuIdle",
	"CpuIowait",
	"CpuIrq",
	"CpuSirq",
	"CpuUsrJeffie",
	"CpuNiceJeffie",
	"CpuSystemJeffie",
	"CpuIdleJeffie",
	"CpuIowaitJeffie",
	"CpuIrqJeffie",
	"CpuSirqJeffie"
};

// generate xml format string
// return 0 means success
// return < 0 means error code
int aos_resmgr_xml_gen_cpu_stat_rslt(char * const str_rslt, const int str_size, const aos_cpumgr_stat_rslt_t * arr_rslt, const int arr_size)
{
	TiXmlDocument xmlDoc;
	
	// insert <?xml version="1.0" encoding="UTF-8"?>
	TiXmlDeclaration xmlDeclare("1.0","UTF-8","Yes");
	xmlDoc.InsertEndChild(xmlDeclare);
	
	TiXmlElement rootElement(NODE_STATISTICSDATA);
	
	// insert <CpuMgrStatItemNr> </CpuMgrStatItemNr>  to root
	TiXmlElement cpumgrstatitemnrElement(NODE_CPUMGRSTATITEMNR);
	char tmpStr[1024];
	sprintf(tmpStr, "%d", arr_size);
	TiXmlText xmlNum(tmpStr);
	cpumgrstatitemnrElement.InsertEndChild(xmlNum);	
	rootElement.InsertEndChild(cpumgrstatitemnrElement);
	
	// insert <CpuMgrStatRslt > node
	int i;
	std::string arrStr[CHILD_NODE_NUM_CPUMGRSTATRSLT];
	for (i=0; i<arr_size; i++)
	{
		TiXmlElement cpuMgrStatRsltElement(NODE_CPUMGRSTATRSLT);
		cpuMgrStatRsltElement.SetAttribute("index", i);
		
		sprintf(tmpStr,"%ld",arr_rslt[i].m_sec_index);
		arrStr[0] = tmpStr;
		sprintf(tmpStr,"%f",arr_rslt[i].m_cpu_usage);
		arrStr[1] = tmpStr;
		sprintf(tmpStr,"%ld",arr_rslt[i].m_jeffiesDuration);
		arrStr[2] = tmpStr;
		sprintf(tmpStr,"%f",arr_rslt[i].m_cpu_user);
		arrStr[3] = tmpStr;
		sprintf(tmpStr,"%f",arr_rslt[i].m_cpu_nice);
		arrStr[4] = tmpStr;
		sprintf(tmpStr,"%f",arr_rslt[i].m_cpu_system);
		arrStr[5] = tmpStr;
		sprintf(tmpStr,"%f",arr_rslt[i].m_cpu_idle);
		arrStr[6] = tmpStr;
		sprintf(tmpStr,"%f",arr_rslt[i].m_cpu_iowait);
		arrStr[7] = tmpStr;
		sprintf(tmpStr,"%f",arr_rslt[i].m_cpu_irq);
		arrStr[8] = tmpStr;
		sprintf(tmpStr,"%f",arr_rslt[i].m_cpu_sirq);
		arrStr[9] = tmpStr;
		sprintf(tmpStr,"%ld",arr_rslt[i].m_jeffies_cpu_user);
		arrStr[10] = tmpStr;
		sprintf(tmpStr,"%ld",arr_rslt[i].m_jeffies_cpu_nice);
		arrStr[11] = tmpStr;
		sprintf(tmpStr,"%ld",arr_rslt[i].m_jeffies_cpu_system);
		arrStr[12] = tmpStr;
		sprintf(tmpStr,"%ld",arr_rslt[i].m_jeffies_cpu_idle);
		arrStr[13] = tmpStr;
		sprintf(tmpStr,"%ld",arr_rslt[i].m_jeffies_cpu_iowait);
		arrStr[14] = tmpStr;
		sprintf(tmpStr,"%ld",arr_rslt[i].m_jeffies_cpu_irq);
		arrStr[15] = tmpStr;
		sprintf(tmpStr,"%ld",arr_rslt[i].m_jeffies_cpu_sirq);
		arrStr[16] = tmpStr;
		
		for (int j=0; j<CHILD_NODE_NUM_CPUMGRSTATRSLT; j++)
		{
			TiXmlElement xmlChildNode(CHILD_NODE_CPUMGRSTATRSLT[j]);
			TiXmlText xmlChildNodeText(arrStr[j]);
			xmlChildNode.InsertEndChild(xmlChildNodeText);
			cpuMgrStatRsltElement.InsertEndChild(xmlChildNode);
		}
		rootElement.InsertEndChild(cpuMgrStatRsltElement);		
	}	
	
	// insert <StatisticsData> </StatisticsData> to xml doc
	xmlDoc.InsertEndChild(rootElement);
	
	std::string outStr;
	outStr << xmlDoc;
	if (outStr.size() > (unsigned int)str_size)
	{
		return -1;
	}
	sprintf(str_rslt, "%s", outStr.c_str());
	return 0;
}


// generate xml format string
// return 0 means success
// return < 0 means error code
// input arr_size is the total space lenghth of arr_rslt, 
// 			return arr_size should be the real meaningful value of arr_rslt
// return -1:parse error
//        -2:size not enough
int aos_resmgr_xml_par_cpu_stat_rslt(aos_cpumgr_stat_rslt_t * const arr_rslt, int * arr_size, const char * str_rslt, const int str_size)
{
	TiXmlDocument xmlDoc;
	if (!xmlDoc.Parse(str_rslt))
	{
		return -1;
	}
	
	const TiXmlElement* rootElement = xmlDoc.FirstChildElement(NODE_STATISTICSDATA);
	if (rootElement == NULL)
	{
		return -1;
	}
	const TiXmlElement* numElement = rootElement->FirstChildElement(NODE_CPUMGRSTATITEMNR);
	if (numElement == NULL)
	{
		return -1;
	}
	int num = atoi(numElement->GetText());
	if (num > *arr_size)
	{
		return -2;
	}
	
	*arr_size = num;
	int realNum=0;
	const TiXmlNode* childNode = rootElement->FirstChild(NODE_CPUMGRSTATRSLT);
	for (;childNode; childNode = childNode->NextSibling())
	{
		realNum++;
		if (realNum > num)
		{
			return -1;
		}
		if (childNode->Type() == TiXmlNode::ELEMENT)	
		{
			const TiXmlElement *cpumgrstatrsltElement = childNode->ToElement();
			const TiXmlNode* child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[0]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			const TiXmlElement *ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_sec_index = atol(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[1]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_cpu_usage = atof(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[2]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_jeffiesDuration = atol(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[3]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_cpu_user = atof(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[4]);
			if (child == NULL || child->Type() != TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_cpu_nice = atof(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[5]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_cpu_system = atof(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[6]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_cpu_idle = atof(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[7]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_cpu_iowait = atof(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[8]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_cpu_irq = atof(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[9]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_cpu_sirq = atof(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[10]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_jeffies_cpu_user = atol(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[11]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_jeffies_cpu_nice = atol(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[12]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_jeffies_cpu_system = atol(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[13]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_jeffies_cpu_idle = atol(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[14]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_jeffies_cpu_iowait = atol(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[15]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_jeffies_cpu_irq = atol(ChildElement->GetText());
			
			child = cpumgrstatrsltElement->FirstChild(CHILD_NODE_CPUMGRSTATRSLT[16]);
			if (child == NULL || child->Type()!= TiXmlNode::ELEMENT)
			{
				return -1;
			}
			ChildElement = child->ToElement();
			arr_rslt[realNum-1].m_jeffies_cpu_sirq = atol(ChildElement->GetText());
		}
	}
	return 0;	
}
