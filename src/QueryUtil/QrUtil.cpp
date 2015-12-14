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
// 01/02/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryUtil/QrUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterTime/TimeUtil.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEBase/SecUtil.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "Util/ValueRslt.h"
#include "Util/StrSplit.h"
#include "UtilTime/TimeInfo.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlUtil.h"
#include <stdint.h>


bool
AosQrUtil::parseFnames(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &fnames, 
		vector<AosQrUtil::FieldDef> &fielddef) 
{
	// Jozhi, 04/06/2012
	// Added a new attribute: sdoc_objid
	// 'fnames' should be in the form:
	// 	<fnames [type="x"]>
	// 		<fname type="1|2|3|x|j|s|e" exist="true|false" join="xxx">
	// 			<oname><![CDATA[xxx]]></oname>
	// 			<cname><![CDATA[xxx]]></cname>
	// 		</fname>
	// 		...
	// 	</fnames>
	//
	// The new type 'e' indicates the selected is an expression:
	// 	<fname type="e"><![CDATA[expression]]></fname>

	fielddef.clear();
	if (!fnames || fnames->getAttrStr("type") == "x")
	{
		FieldDef field; 
		field.type  = eXml;
		field.exist = false;
		field.mAggregation = AosAggregationType::eInvalid;	// Ketty 2014/04/09
		fielddef.push_back(field);
		return true;
	}

	AosXmlTagPtr fname = fnames->getFirstChild();
	if (!fname)
	{
		AosSetErrorUser(rdata, "failed_retrieving_field_names") << enderr;
		return false;
	}

	OmnString type = "x";
	FieldType ftype;
	bool exist = false;
	int guard = eMaxFields;
	while (fname && guard--)
	{
		type = fname->getAttrStr("type");
		if (type.length() != 1)
		{
			AosSetErrorUser(rdata, "invalid_field_type") << type << enderr;
			return false;
		}

		const char * data = type.data();
		switch (data[0])
		{
		case '1': ftype = AosQrUtil::eAttr;    							break;
		case '2': ftype = AosQrUtil::eText;  							break;
		case '3': ftype = AosQrUtil::eCdata;   							break;
		case '5': ftype = AosQrUtil::eEpoch2TimeStr;  					break;
		case '6': ftype = AosQrUtil::eEpoch2LocalTime; 					break;
		case '7': ftype = AosQrUtil::eEpochDay2TimeStr; 				break;
		case '8': ftype = AosQrUtil::eDayOfWeekToStr;   				break;
		case '9': ftype = AosQrUtil::eEpochHour2TimeStr;				break;
		case 'h': ftype = AosQrUtil::eAttrAdd;							break;
		case 'e': ftype = AosQrUtil::eExpr;								break;
		case 'p': ftype = AosQrUtil::eAttr; 
				  rdata->setArg1(AOSTAG_USING_XML_AS_RESULT,"true");	break;
		case 'x': ftype = AosQrUtil::eXml;     							break;
		case 'f': ftype = AosQrUtil::eFunc;     			break;
		default:
			 AosSetErrorUser(rdata, "invalid_field_type") << type << enderr;
			 return false;
		}

		exist = fname->getAttrBool("exist", false);

		FieldDef field;
		field.type = ftype;
		field.exist = exist;
		field.join_idx = fname->getAttrInt("join_idx", -1);
		field.oname = fname->xpathQuery("oname/_#text", exist, "");
		field.cname = fname->xpathQuery("cname/_#text", exist, "");

		if (ftype != eXml)
		{
			if (field.oname == "")
			{
				AosSetErrorUser(rdata, "missing oname") << enderr;
				return false;
			}
			if (field.cname == "")
			{
				AosSetErrorUser(rdata, "missing_cname") << enderr;
				return false;
			}
		}

		OmnString sdoc_objid = fname->getAttrStr("sdoc_objid");
		if (sdoc_objid != "")
		{
			AosXmlTagPtr sdoc = AosGetDocByObjid(sdoc_objid, rdata.getPtr());
			if (sdoc)
			{
				sdoc = sdoc->clone(AosMemoryCheckerArgsBegin);
				field.sdoc = sdoc;
			}
		}

		// Andy 2014/04/24
		AosXmlTagPtr expr_conf = fname->getFirstChild("expr_str");
		if (expr_conf)
		{
			OmnString data = expr_conf->getNodeText();
			// Chen Ding, 2015/01/31
			// field.mExprPtr = AosParseJQL(data, true, rdata.getPtr());
			OmnString errmsg;
			field.mExprPtr = AosParseExpr(data, errmsg, rdata.getPtr());
			aos_assert_r(field.mExprPtr, false);
		}
		
		// Ketty 2014/04/09
		field.mAggregation = AosAggregationType::eInvalid;
		AosXmlTagPtr agr_type_conf = fname->getFirstChild("agr_type");
		if (agr_type_conf)
		{
			OmnString agr_type_str = agr_type_conf->getAttrStr("type", "");
			field.mAggregation = AosAggregationType::toEnum(agr_type_str);
		}

		fielddef.push_back(field);
		fname = fnames->getNextChild();
	}

	return true;
}


bool
AosQrUtil::parseFnames(
		const AosRundataPtr &rdata,
		const OmnString &fnames,
		vector<FieldDef> &fields, 
		const OmnString &entry_sep, 
		const OmnString &attr_sep) 
{
	//int max_fields = numFields;
	fields.clear();
	bool finished;
	if (fnames == "") return true;
	
	// oname|$|cname|$|1,2,3,4,x|$|exist|$|join_idx
	OmnString parts[eMaxFields];
	AosStrSplit spt1(fnames.data(), entry_sep.data(), parts, eMaxFields, finished);
	OmnString pp[5];
	int guard = eMaxFields;
	for (int i=0; i<spt1.entries() && guard--; i++)
	{
		AosStrSplit spt2(parts[i].data(), attr_sep.data(), pp, 5, finished);
		int num = spt2.entries();
		aos_assert_r(num == 1 || num >= 3, false);

		FieldDef field;
		field.oname = pp[0];
		field.cname = (num >= 3)?pp[1]:pp[0];
		field.exist = (num >= 4)?(pp[3] == "true"):false;
		field.join_idx = (num >= 5)?(atoi(pp[4].data())):-1;

		char typecode = '1';
		if (num >= 3)
		{
			aos_assert_r(pp[2] != "", false);
			typecode = pp[2].data()[0];
		}

		switch (typecode)
		{
		case '1': field.type = AosQrUtil::eAttr;				break;
		case '2': field.type = AosQrUtil::eText;				break;
		case '3': field.type = AosQrUtil::eCdata;				break;
		case '4': field.type = AosQrUtil::eAttrSubtag;			break;
		case '5': field.type = AosQrUtil::eEpoch2TimeStr;		break;
		case '6': field.type = AosQrUtil::eEpoch2LocalTime;		break;
		case '7': field.type = AosQrUtil::eEpochDay2TimeStr;	break;
		case '8': field.type = AosQrUtil::eDayOfWeekToStr;		break;
		case '9': field.type = AosQrUtil::eEpochHour2TimeStr;	break;
		case 'p': field.type = AosQrUtil::eAttr;			
			 rdata->setArg1(AOSTAG_USING_XML_AS_RESULT,"true");	break;
		case 'h': field.type = AosQrUtil::eAttrAdd;				break;
		case 'x': field.type = AosQrUtil::eXml;     			break;
		case 'f': field.type = AosQrUtil::eFunc;     			break;
		default: return false;
		}
		field.mAggregation = AosAggregationType::eInvalid;	// Ketty 2014/04/09
		fields.push_back(field);
	}

	return true;
}


bool
AosQrUtil::createRecord(
		const AosRundataPtr &rdata,
		OmnString &records, 
		const AosXmlTagPtr &xml,
		vector<FieldDef> &fields) 
{
	// It creates a record:
	// 		<record objid="xxx"
	// 			field="xxx"
	// 			...>
	// 			<tag .../>
	// 			<tag .../>
	// 		</record>
	// If the requested field does not exist in 'xml' but the
	// field's 'exist' flag is true, it disqualifies the xml.
	//
	// Field Types:
	// eXml: 
	// The entire XML object is appended to 'records' as:
	// 'records' + '>' + the-contents-of-'xml'.
	//
	// eAttr:
	// Append as an attribute.
	//
	// eText:
	// It is subtag and it is appended as a subtag. 
	//
	// eXml:
	// It appends the entire object as a subtag.
	//
	// eCdata:
	// It appends the object itself as the CDATA of a subtag. 
	int numFields = fields.size();
	aos_assert_r(numFields > 0, false);

	if (numFields == 1 && fields[0].type == eXml)
	{
		// It is to retrieve xml objects. The form should be:
		// 	<Contents total="xxx" start="xxx" num="xxx" time="xxx">
		// 		<the object>
		// 		<the object>
		// 		...
		// 	</Contents>

		// Chen Ding, 07/19/2011
		AosXmlUtil::signAttr(xml, AOSTAG_DOCID, AOSTAG_FULLDOC_SIGNATURE, rdata);

		if (fields[0].sdoc)
		{
			rdata->setWorkingDoc(xml, false);
			AosSmartDocObjPtr sobj = AosSmartDocObj::getSmartDocObj();
			aos_assert_r(sobj, false);
			sobj->runSmartdoc(fields[0].sdoc, rdata);
		}

		records.append((const char *)xml->getData(), xml->getDataLength());
		return true;
	}

	const OmnString docid = xml->getAttrStr(AOSTAG_DOCID, ""); 
	const OmnString objid = xml->getAttrStr(AOSTAG_OBJID, ""); 

	OmnString record = "<record";
	if (docid != "") record << " " << AOSTAG_DOCID << "=\"" << docid << "\"";
	if (objid != "") record << " " << AOSTAG_OBJID << "=\"" << objid << "\"";

	OmnString subtags;
	for (int i=0; i<numFields; i++)
	{
		//Zky2881 Ketty 2011/02/24
		if (docid != "" && fields[i].cname==AOSTAG_DOCID) continue;
		if (objid != "" && fields[i].cname==AOSTAG_OBJID) continue;
		
		procOneField(xml, fields[i], record, subtags, rdata);
	}

	records << record << ">";
	if (subtags != "") records << subtags;
	records << "</record>";
	return true;
}


bool
AosQrUtil::createRecord(
		const AosRundataPtr &rdata,
		OmnString &records, 
		vector<AosXmlTagPtr> &xmls,
		vector<FieldDef> &fields) 
{
	int numFields = fields.size();
	int numXmls = xmls.size();
	if (numFields == 0 || (numFields == 1 && fields[0].type == eXml))
	{
		// It is to retrieve xml objects. The form should be:
		// 	<Contents total="xxx" start="xxx" num="xxx" time="xxx">
		// 		<the object>
		// 		<the object>
		// 		...
		// 	</Contents>
		int xmlidx = (fields[0].join_idx > 0 && fields[0].join_idx < numXmls)?
			fields[0].join_idx:0;
		if (xmlidx < 0 || xmlidx >= numXmls)
		{
			rdata->setError() << "Invalid join index: " << xmlidx << ":" << numXmls;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		AosXmlTagPtr xml = xmls[xmlidx];
		if (!xml)
		{
			rdata->setError() << "Joined XML not found: " << xmlidx << ":" << numXmls;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (fields[0].sdoc)
		{
			rdata->setWorkingDoc(xml, false);
			AosSmartDocObjPtr sobj = AosSmartDocObj::getSmartDocObj();
			aos_assert_r(sobj, false);
			sobj->runSmartdoc(fields[0].sdoc, rdata);
		}

		// Chen Ding, 07/19/2011
		AosXmlUtil::signAttr(xml, AOSTAG_DOCID, AOSTAG_FULLDOC_SIGNATURE, rdata);

		records.append((const char *)xml->getData(), xml->getDataLength());
		return true;
	}

	OmnString subtags;
	aos_assert_r(xmls[0], false);

	// Chen Ding, 04/23/2012
	bool staticsflag = xmls[0]->getAttrBool(AOSTAG_USING_XML_AS_RESULT, false);
	if (rdata->getArg1(AOSTAG_USING_XML_AS_RESULT) == "true" || staticsflag)
	{
		return createRecord(rdata, records, xmls, fields);
	}

	const OmnString docid = xmls[0]->getAttrStr(AOSTAG_DOCID); 
	const OmnString objid = xmls[0]->getAttrStr(AOSTAG_OBJID); 

	OmnString record = "<record";
	if (docid != "") record << " " << AOSTAG_DOCID << "=\"" << docid << "\"";
	if (objid != "") record << " " << AOSTAG_OBJID << "=\"" << objid << "\"";

	OmnString querystr, value;
	bool rslt = true;
	
	for (int i=0; i<numFields && rslt; i++)
	{
		// Determine the XML
		int xmlidx = (fields[i].join_idx > 0 && fields[i].join_idx < numXmls)?
			fields[i].join_idx:0;
		AosXmlTagPtr xml = xmls[xmlidx];
		if (!xml) continue;

		//Zky2881 Ketty 2011/02/24
		if (fields[i].cname==AOSTAG_DOCID) continue;
		if (fields[i].cname==AOSTAG_OBJID) continue;

		// modified By Andy zhang  
		OmnString rcd_conf_str;
		rcd_conf_str << "<datarecord type=\"fixbin\" zky_name=\"tmp\" zky_length=\"120\">"
			<< 	"<datafields>"
			<< 		"<datafield type=\"str\" "
			<<			  "zky_name=\"" << fields[i].cname << "\" zky_offset=\"0\" " 
			<<			  "zky_length=\"120\" zky_datatooshortplc=\"cstr\"/>"
			<< 	"</datafields>"
			<< "</datarecord>";

		AosXmlTagPtr rcd_conf =	AosXmlParser::parse(rcd_conf_str AosMemoryCheckerArgs); 

		fields[i].mRcdPtr = AosDataRecordObj::createDataRecordStatic(rcd_conf, NULL, rdata.getPtr() AosMemoryCheckerArgs);

		rslt = procOneField(xml, fields[i], record, subtags, rdata);
	}

	if (rslt)
	{
		addFieldFlag(record, fields, xmls, rdata);
		records << record;
		if (subtags != "") records << subtags;
		records << "</record>";
	}
	return true;
}


// Version 1
bool
AosQrUtil::procOneField(
		const AosXmlTagPtr &xml,
		const FieldDef &field, 
		OmnString &record,
		OmnString &subtags, 
		const AosRundataPtr &rdata)
{
	if (field.cname==AOSTAG_DOCID) return true;
	if (field.cname==AOSTAG_OBJID) return true;

	aos_assert_r(xml, false);

	OmnString value, querystr;
	bool exist;

	if (field.sdoc)
	{
		rdata->setWorkingDoc(xml, false);
		AosSmartDocObjPtr sobj = AosSmartDocObj::getSmartDocObj();
		aos_assert_r(sobj, false);
		sobj->runSmartdoc(field.sdoc, rdata);
	}

	switch (field.type)
	{
	case AosQrUtil::eAttr:
		 // It is an attribute and it is treated as an attribute
		 // in the response. 
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 if (value != "")
		 {
			//filter special character :
			//& -> &amp;
			//< -> &lt;
			//> -> &gt;
			//" -> &quot;
			//' -> '
			
			value.replace("&", "&amp;", true);
			value.replace("<", "&lt;", true);
			value.replace(">", "&gt;", true);
			value.replace("\"", "&quot;", true);

		 	record << " " << field.cname << "=\"" << value << "\"";
			rdata->setArg1(field.cname, value);
		 }
		 break;

	case AosQrUtil::eText:
		 // The field is a subtag and it is converted as a subtag
		 // of the response. The subtag name is 'cname'.
		 querystr = field.oname;
		 querystr << "/_$text";
		 value = xml->xpathQuery(querystr, exist, "");
		 if (!exist && field.exist)
		 {
			return false;
		 }

		 //filter CDATA 
		 value.replace("<![CDATA[", "", true);
		 value.replace("]]>", "", true);

		 subtags << "<" << field.cname << "><![CDATA["
			<< value << "]]></" << field.cname << ">";
		 rdata->setArg1(field.cname, value);
		 break;
		 
	case AosQrUtil::eXml:
		 // The field is the object itself. It is converted as a subtag. 
		 // The subtag name is 'cname'. If 'cname' is empty, it will 
		 // attach the xml object directly to the record.
		 if (field.cname != "")
		 {
			 subtags << "<" << field.cname << ">"
				 << (char *)xml->getData()
				 << "</" << field.cname << ">";
		 }
		 else
		 {
			 subtags << (char *)xml->getData();
		 }
		 break;

	case AosQrUtil::eCdata:
		 // The field is the object itself. It is converted as the CDATA
		 // of a subtag. The subtag name is 'cname'. If 'cname' is empty, 
		 // it will attach the XML object directly to the record.
		 // Ken Lee, 2014/08/05
		 // now oname is xpath, cname is subtag name.
		 if (field.oname != "")
		 {
		 	value = xml->xpathQuery(field.oname, exist, "");
		 	if (!exist && field.exist)
		 	{
		 		return false;
		 	}
		 
			subtags << "<" << field.cname << "><![CDATA["
				<< value << "]]></" << field.cname << ">";
			rdata->setArg1(field.cname, value);
		 	break;
		 }

		 if (field.cname != "")
		 {
			 subtags << "<" << field.cname << "><![CDATA["
				 << (char *)xml->getData() << "]]></" 
				 << field.cname << ">";
		 }
		 else
		 {
			 subtags << "<![CDATA[" << (char *)xml->getData() << "]]>";
		 }
		 break;

	case AosQrUtil::eEpoch2TimeStr:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 if (value != "")
		 {
			 // Chen Ding, 2014/01/29
			 // u64 vv = atoll(value.data());
			 // if (vv > 0)
			 // {
			 // 	value = AosTimeUtil::EpochToTime(vv);
			 // 	record << " " << field.cname << "=\"" << value << "\"";
			 // }
			 i64 vv = atoll(value.data());
			 value = AosTimeUtil::EpochToTimeNew(vv);
			 record << " " << field.cname << "=\"" << value << "\"";
		 }
		 break;

	case AosQrUtil::eEpoch2LocalTime:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 if (value != "")
		 {
			// Chen Ding, 2014/01/29
			// u64 vv = atoll(value.data());
			// if (vv > 0)
			// {
			// 	value = AosTimeUtil::EpochToLocalTime(rdata, vv);
			// 	record << " " << field.cname << "=\"" << value << "\"";
			// }
			i64 vv = atoll(value.data());
			value = AosTimeUtil::EpochToLocalTimeNew(rdata, vv);
			record << " " << field.cname << "=\"" << value << "\"";
		 }
		 break;
	
	case AosQrUtil::eEpochDay2TimeStr:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
			 return false;
		 }

		 if (value != "")
		 {
			 // Chen Ding, 2014/01/29
			 // u64 vv = atoll(value.data());
			 // if (vv > 0)
			 // {
			 // 	 vv = vv * 24 * 60 * 60;
			 // 	 value = AosTimeUtil::EpochToTime(vv, "%Y-%m-%d");
			 // 	 record << " " << field.cname << "=\"" << value << "\"";
			 // }
			 i64 vv = atoll(value.data());
			 vv = vv * 24 * 60 * 60;
			 value = AosTimeUtil::EpochToTimeNew(vv, "%Y-%m-%d");
			 record << " " << field.cname << "=\"" << value << "\"";
		 }
		 break;

	case AosQrUtil::eDayOfWeekToStr:
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 if (value != "")
		 {
			 u64 vv = atoll(value.data());
			 value = AosTimeUtil::DayOfWeekToString(rdata, vv);
			 record << " " << field.cname << "=\"" << value << "\"";
		 }
		 break;

	case AosQrUtil::eEpochHour2TimeStr:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
			 return false;
		 }

		 if (value != "")
		 {
			 // Chen Ding, 2014/01/29
			 // u64 vv = atoll(value.data());
			 // if (vv > 0)
			 // {
			 // 	 vv = vv * 60 * 60;
			 // 	 value = AosTimeUtil::EpochToTime(vv, "%Y-%m-%d %H:%M:%S");
			 // 	 record << " " << field.cname << "=\"" << value << "\"";
			 // }
			 i64 vv = atoll(value.data());
			 vv = vv * 60 * 60;
			 value = AosTimeUtil::EpochToTimeNew(vv, "%Y-%m-%d %H:%M:%S");
			 record << " " << field.cname << "=\"" << value << "\"";
		 }
		 break;

	case AosQrUtil::eAttrAdd:
		 // It is an attribute and it is treated as an attribute
		 // in the response. 
		 if (!exist && field.exist)
		 {
			return false;
		 }

		 if (field.cname != "")
		 {
			record << " " << field.oname << "=\"" << field.cname << "\"";
			rdata->setArg1(field.oname, field.cname);
		 }
		 break;

	case AosQrUtil::eFunc:
		 {
			 value = xml->xpathQuery(field.oname, exist, "");
			 bool outmem = false;
			 AosValueRslt v(value);
			 field.mRcdPtr->setFieldValue(0, v, outmem, rdata.getPtr());
			 AosValueRslt vv;
			 bool rslt = field.mExprPtr->getValue(rdata.getPtr(), field.mRcdPtr.getPtr(), vv);
			 aos_assert_r(rslt, false);
			 record << " " << field.oname << "=\"" << vv.getStr()<< "\"";
			 rdata->setArg1(field.oname, vv.getStr());
		 }
	default:
		 rdata->setError() << "Unrecognized field type: " << field.type;
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}

	return true;
}


bool
AosQrUtil::addFieldFlag(
		OmnString &record,
		vector<FieldDef> &fields, 
		vector<AosXmlTagPtr> &xmls,
		const AosRundataPtr &rdata)
{
	// Ken Lee, 2013/07/30
	record << ">";
	return true;

	OmnString ss;
	OmnString stype;
	OmnString join;
	int numFields = fields.size();
	int numXmls = xmls.size();
	for (int i=0; i<numFields; i++)
	{
		switch (fields[i].type)
		{
		case eAttr:     		stype = "eAttr";        		break;
		case eAttrAdd:          stype = "eAttrAdd";             break;
		case eText:    			stype = "eText";        		break;
		case eXml:      		stype = "eXml";         		break;
		case eCdata:    		stype = "eCdata";       		break;
		case eEpoch2TimeStr: 	stype = "eEpoch2TimeStr"; 		break;
		case eEpoch2LocalTime: 	stype = "eEpoch2LocalTime";		break;
		case eEpochDay2TimeStr: stype = "eEpochDay2TimeStr";	break;
		case eDayOfWeekToStr: 	stype = "eDayOfWeekToStr";  	break;
		case eEpochHour2TimeStr:stype = "eEpochHour2TimeStr"; 	break;
		case eFunc:				stype = "eFunc";			 	break;
		default:
			 rdata->setError() << "Unrecognized field type: " << fields[i].type;
			 OmnAlarm << rdata->getErrmsg() << enderr;
			 stype ="";
			 break;
		}

		int xmlidx = (fields[0].join_idx > 0 && fields[0].join_idx < numXmls)?
			fields[0].join_idx:0;
		AosXmlTagPtr xml = xmls[xmlidx];
		if (xml && xmlidx != 0)
		{
			join = xml->getAttrStr(AOSTAG_DOCID);
		}
		else
		{
			join = "0";
		}
	
		if (ss != "") ss << ":";
		ss <<fields[i].oname
			<<"|$|" << fields[i].cname << "|$|" << stype <<"|$|" <<fields[i].exist
			<< "|$|" << join;
	}
	
	OmnString signature = AosSecUtil::signValue(ss);
	if (signature == "")
	{
		rdata->setError() << "Failed signing the value";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}
	else
	{
		record << " " <<AOSTAG_PARTIAL_DOC_FNAMES << " =\"" << ss << "\" " 
			<< AOSTAG_PARTIAL_DOC_SIGNATURE << "=\"" << signature << "\">";
	}
	return true;
}


bool
AosQrUtil::calculateTimeToEpoch(
		const int number,
		const OmnString &timetype,
		i64 &crt_epoch,
		i64 &cal_epoch,
		const AosRundataPtr &rdata)
{
	ptime p1, p2;
	return AosTimeUtil::calculateTimeNew(number, timetype, p1, p2, crt_epoch, cal_epoch, rdata);
}


void 
AosQrUtil::procQueryVars(
			OmnString &value, 
			const OmnString &dftproc, 
			const OmnString &dftvalue, 
			bool &ignore,
			const AosRundataPtr &rdata)
{
	ignore = false;
	int len = value.length();
	const char *data = value.data();
	//if (len == 24 && value == "logininfo/zky_cloudid__a")
	bool exist;
	if (!strncmp("logininfo", data, 9))
	{
		AosXmlTagPtr logindoc = AosDocClientObj::getDocClient()->getDocByCloudid(
			rdata->getCid(), rdata);
		aos_assert(logindoc);
		int pos = value.indexOf(0, '/');
		if (pos == -1) goto dft_proc;

		OmnString xpath(&data[pos+1], len-pos-1);
		value = logindoc->xpathQuery(xpath, exist, "");
		goto dft_proc;
	}

	if (len > 8 && strncmp(data, "clipvar/", 8) == 0)
	{
		// This means the data is configured to listen to the 
		// clipvar. Will ignore it.
		value = "";
		goto dft_proc;
	}

	// Check the form: var.$cloudid
	if (value != "")
	{
		int idx = value.findSubString(AOSVALUE_CLOUDID_VAR, 0);
		if (idx != -1)
		{
			int strlen = len - 9;
			OmnString substr = value.substr(strlen, len - 1);
			if (len > 9 && strcmp(substr.data(), AOSVALUE_CLOUDID_VAR) == 0)
			{
				OmnString cid = rdata->getCid();
				if (cid != "")
				{
					value.setLength(len - 9);
					value << "." << cid;
				}
			}
		}
	}	
	goto dft_proc;

dft_proc:
	if (value != "" && value != AOS_NULL_STRING) return;

	// It supports three default value processing:
	// 	1. IGNORE: the query shall be ignored. The caller should set the term
	// 	   to be invalid.
	// 	2. USE DEFAULT: use the default value.
	// 	3. USE IT: just use the empty vallue.

	if (dftproc == AOSVALUE_USEDEFAULT)
	{
		value = dftvalue;
		return;
	}

	if (dftproc == AOSVALUE_USEIT)
	{
		value = "";
		return;
	}

	// The default is AOSVALUE_IGNORE
	ignore = true;
}


bool
AosQrUtil::createRecordByXml(
		const AosRundataPtr &rdata,
		OmnString &records, 
		vector<AosXmlTagPtr> &xmls,
		vector<FieldDef> &fields) 
{
	// This function creates a new record by modifying xmls[0] and then 
	// append it to 'records'. 
	int numFields = fields.size();
	int numXmls = xmls.size();
	if (numFields == 0 || (numFields == 1 && fields[0].type == eXml))
	{
		// It is to retrieve xml objects. The form should be:
		// 	<Contents total="xxx" start="xxx" num="xxx" time="xxx">
		// 		<the object>
		// 		<the object>
		// 		...
		// 	</Contents>
		int xmlidx = (fields[0].join_idx > 0 && fields[0].join_idx < numXmls)?
			fields[0].join_idx:0;
		if (xmlidx < 0 || xmlidx >= numXmls)
		{
			rdata->setError() << "Invalid join index: " << xmlidx << ":" << numXmls;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		AosXmlTagPtr xml = xmls[xmlidx];
		if (!xml)
		{
			rdata->setError() << "Joined XML not found: " << xmlidx << ":" << numXmls;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		// Chen Ding, 07/19/2011
		AosXmlUtil::signAttr(xml, AOSTAG_DOCID, AOSTAG_FULLDOC_SIGNATURE, rdata);
		records.append((const char *)xml->getData(), xml->getDataLength());
		return true;
	}

	OmnString subtags;
	aos_assert_r(xmls[0], false);

	// Medified by Andy zhang 2013-08-01
	OmnString str_xml = "<record />";
	AosXmlTagPtr record = AosStr2Xml(rdata.getPtr(), str_xml AosMemoryCheckerArgs);
	OmnString docid = xmls[0]->getAttrStr(AOSTAG_DOCID, "");
	aos_assert_r(docid != "", false);
	OmnString objid = xmls[0]->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(objid != "", false);
	record->setAttr(AOSTAG_DOCID, docid.data());
	record->setAttr(AOSTAG_OBJID, objid.data());

	// const OmnString docid = xmls[0]->getAttrStr(AOSTAG_DOCID, ""); 
	// const OmnString objid = xmls[0]->getAttrStr(AOSTAG_OBJID, ""); 
	//The record format is:
	//<record  ...>
	//	<time .../>
	//	<time .../>
	//	<time .../>
	//	<time .../>
	//</record>

	for (int i=0; i<numFields; i++)
	{
		// Determine the XML
		int xmlidx = (fields[i].join_idx > 0 && fields[i].join_idx < numXmls)?
			fields[i].join_idx:0;
		AosXmlTagPtr xml = xmls[xmlidx];
		if (!xml) continue;

		procOneField(record, xml, fields[i], rdata);
	}

	addFieldFlag(record, fields, xmls, rdata);
	records << record->toString();
	return true;
}


// Version 2
bool
AosQrUtil::procOneField(
		const AosXmlTagPtr &record,
		const AosXmlTagPtr &xml,
		const FieldDef &field, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	OmnString value, querystr;
	bool exist;

	if (field.sdoc)
	{
		rdata->setWorkingDoc(xml, false);
		AosSmartDocObjPtr sobj = AosSmartDocObj::getSmartDocObj();
		aos_assert_r(sobj, false);
		sobj->runSmartdoc(field.sdoc, rdata);
	}

	switch (field.type)
	{
	case AosQrUtil::eAttr:
		 // It is an attribute and it is treated as an attribute
		 // in the response. 
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 if (value != "")
		 {
			//filter special character :
			//& -> &amp;
			//< -> &lt;
			//> -> &gt;
			//" -> &quot;
			//' -> '
			
			value.replace("&", "&amp;", true);
			value.replace("<", "&lt;", true);
			value.replace(">", "&gt;", true);
			value.replace("\"", "&quot;", true);

			record->xpathSetAttr(field.cname, value, true);
			rdata->setArg1(field.cname, value);
		 }
		 break;

	case AosQrUtil::eText:
		 // The field is a subtag and it is converted as a subtag
		 // of the response. The subtag name is 'cname'.
		 querystr = field.oname;
		 querystr << "/_$text";
		 value = xml->xpathQuery(querystr, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 //filter CDATA 
		 value.replace("<![CDATA[", "", true);
		 value.replace("]]>", "", true);

		 record->setNodeText(field.cname, value, true);
		 rdata->setArg1(field.cname, value);
		 break;
		 
	case AosQrUtil::eXml:
		 // The field is the object itself. It is converted as a subtag. 
		 // The subtag name is 'cname'. If 'cname' is empty, it will 
		 // attach the xml object directly to the record.
		 if (field.cname != "")
		 {
			AosXmlTagPtr clone_xml = xml->clone(AosMemoryCheckerArgsBegin);
			//subtags << "<" << field.cname << ">"
			//	 << (char *)xml->getData()
			//	 << "</" << field.cname << ">";
			record->addNode(clone_xml);
		 }
		 else
		 {
			AosXmlTagPtr clone_xml = xml->clone(AosMemoryCheckerArgsBegin);
			//subtags << (char *)xml->getData();
			record->addNode(clone_xml);
		 }
		 break;

	case AosQrUtil::eCdata:
		 // The field is the object itself. It is converted as the CDATA
		 // of a subtag. The subtag name is 'cname'. If 'cname' is empty, 
		 // it will attach the XML object directly to the record.
		 // Ken Lee, 2014/08/05
		 // now oname is xpath, cname is subtag name.
		 if (field.oname != "")
		 {
		 	value = xml->xpathQuery(field.oname, exist, "");
		 	if (!exist && field.exist)
		 	{
		 		return false;
		 	}
		 
			record->setNodeText(field.cname, value, true);
			rdata->setArg1(field.cname, value);
		 	break;
		 }

		 if (field.cname != "")
		 {
			 //subtags << "<" << field.cname << "><![CDATA["
			//	 << (char *)xml->getData() << "]]></" 
			//	 << field.cname << ">";
			record->setNodeText(field.cname, (char *)xml->getData(), true);
		 }
		 else
		 {
			 //subtags << "<![CDATA[" << (char *)xml->getData() << "]]>";
			 record->setNodeText((char *)xml->getData(), true);
		 }
		 break;

	case AosQrUtil::eEpoch2TimeStr:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 if (value != "")
		 {
			// Chen Ding, 2014/01/29
			// u64 vv = atoll(value.data());
			// if (vv != 0)
			// {
			// 	value = AosTimeUtil::EpochToTime(vv);
			// 	record->setAttr(field.cname, value);
			// }
			i64 vv = atoll(value.data());
			value = AosTimeUtil::EpochToTimeNew(vv);
			record->setAttr(field.cname, value);
		 }
		 break;

	case AosQrUtil::eEpoch2LocalTime:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 if (value != "")
		 {
			// Chen Ding, 2014/01/29
			// u64 vv = atoll(value.data());
			// if (vv != 0)
			// {
			// 	value = AosTimeUtil::EpochToLocalTime(rdata, vv);
			// 	record->setAttr(field.cname, value);
			// }
			i64 vv = atoll(value.data());
			value = AosTimeUtil::EpochToLocalTimeNew(rdata, vv);
			record->setAttr(field.cname, value);
		 }
		 break;
	
	case AosQrUtil::eEpochDay2TimeStr:
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist)
		 {
			 AosXmlTagPtr root;
			 if (xml->isRootTag()) root = xml->getFirstChild();
			 AosXmlTagPtr child = root->getFirstChild();
			 AosXmlTagPtr root2;
			 if (record->isRootTag()) root2 = record->getFirstChild();
			 AosXmlTagPtr element = root2->getFirstChild();
			 aos_assert_r(child, false);
			 while(child)
			 {
				 value = child->getAttrStr(field.oname, "");

				 // Chen Ding, 2014/01/29
				 // u64 vv = atoll(value.data());
				 i64 vv = atoll(value.data());
				 vv = vv * 24 * 60 * 60;
				 value = AosTimeUtil::EpochDayToTimeNew(rdata, vv);
				 aos_assert_r(element, false);
				 element->setAttr(field.cname, value);
				 child = xml->getNextChild();
				 element = record->getNextChild();
			 }
			 return true;
		 }

		 if (value != "")
		 {
			 // Chen Ding, 2014/01/29
			 // u64 vv = atoll(value.data());
			 // if (vv != 0)
			 // {
			 // 	 vv = vv * 24 * 60 * 60;
			 // 	 value = AosTimeUtil::EpochDayToTime(rdata, vv);
			 // 	 record->setAttr(field.cname, value);
			 // }
			 i64 vv = atoll(value.data());
			 vv = vv * 24 * 60 * 60;
			 value = AosTimeUtil::EpochDayToTimeNew(rdata, vv);
			 record->setAttr(field.cname, value);
		 }
		 break;

	case AosQrUtil::eDayOfWeekToStr:
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist)
		 {
			AosXmlTagPtr root;
			if (xml->isRootTag()) root = xml->getFirstChild();
			AosXmlTagPtr child = root->getFirstChild();  
			AosXmlTagPtr root2;
			if (record->isRootTag()) root2 = record->getFirstChild();
			AosXmlTagPtr element = root2->getFirstChild();  
			aos_assert_r(child, false);
			while(child)
			{
		 		value = child->getAttrStr(field.oname, "");
			 	u64 vv = atoll(value.data());
				value = AosTimeUtil::DayOfWeekToString(rdata, vv);
				aos_assert_r(element, false);
				element->setAttr(field.cname, value);
				child = xml->getNextChild();
				element = record->getNextChild();
			}
			return true;
		 }

		 if (value != "")
		 {
			 u64 vv = atoll(value.data());
			 value = AosTimeUtil::DayOfWeekToString(rdata, vv);
			 record->setAttr(field.cname, value);
		 }
		 break;

	case AosQrUtil::eEpochHour2TimeStr:
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist)
		 {
			 AosXmlTagPtr root;
			 if (xml->isRootTag()) root = xml->getFirstChild();
			 AosXmlTagPtr child = root->getFirstChild();
			 AosXmlTagPtr root2;
			 if (record->isRootTag()) root2 = record->getFirstChild();
			 AosXmlTagPtr element = root2->getFirstChild();
			 aos_assert_r(child, false);
			 while(child)
			 {
				 value = child->getAttrStr(field.oname, "");

				 // Chen Ding, 2014/01/29
				 // u64 vv = atoll(value.data());
				 i64 vv = atoll(value.data());
				 vv = vv * 60 * 60;
				 value = AosTimeUtil::EpochToTimeNew(vv, "%Y-%m-%d %H:%M:%S");
				 aos_assert_r(element, false);
				 element->setAttr(field.cname, value);
				 child = xml->getNextChild();
				 element = record->getNextChild();
			 }
			 return true;
		 }

		 if (value != "")
		 {
			 // Chen Ding, 2014/01/29
			 // u64 vv = atoll(value.data());
			 // if (vv != 0)
			 // {
			 // 	 vv = vv * 60 * 60;
			 // 	 value = AosTimeUtil::EpochDayToTime(rdata, vv);
			 // 	 record->setAttr(field.cname, value);
			 // }
			 i64 vv = atoll(value.data());
			 vv = vv * 60 * 60;
			 value = AosTimeUtil::EpochDayToTimeNew(rdata, vv);
			 record->setAttr(field.cname, value);
		 }
		 break;

	case AosQrUtil::eAttrAdd:
		 // It is an attribute and it is treated as an attribute
		 // in the response. 
		 if (!exist && field.exist)
		 {
			return false;
		 }

		 if (field.cname != "")
		 {
			record->xpathSetAttr(field.oname, field.cname, true);
			rdata->setArg1(field.oname, field.cname);
		 }
		 break;
		 
	case AosQrUtil::eFunc:
		 {
			 value = xml->xpathQuery(field.oname, exist, "");
			 bool outmem = false;
			 AosValueRslt v(value);
			 field.mRcdPtr->setFieldValue(0, v, outmem, rdata.getPtr());
			 AosValueRslt vv;
			 bool rslt = field.mExprPtr->getValue(rdata.getPtr(), field.mRcdPtr.getPtr(), vv);
			 aos_assert_r(rslt, false);
			 record->xpathSetAttr(field.oname, vv.getStr());
			 rdata->setArg1(field.oname, vv.getStr());
			 break;
		 }
	default:
		 rdata->setError() << "Unrecognized field type: " << field.type;
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}

	return true;
}


bool
AosQrUtil::addFieldFlag(
		const AosXmlTagPtr &record,
		vector<FieldDef> &fields, 
		vector<AosXmlTagPtr> &xmls,
		const AosRundataPtr &rdata)
{
	// Ken Lee, 2013/07/30
	return true;

	OmnString ss;
	OmnString stype;
	OmnString join;
	int numFields = fields.size();
	int numXmls = xmls.size();
	for (int i=0; i<numFields; i++)
	{
		switch (fields[i].type)
		{
		case eAttr:     		stype = "eAttr";        		break;
		case eText:     		stype = "eText";       			break;
		case eXml:      		stype = "eXml";       			break;
		case eCdata:   			stype = "eCdata";      			break;
		case eEpoch2TimeStr: 	stype = "eEpoch2TimeStr";   	break;
		case eEpoch2LocalTime: 	stype = "eEpoch2LocalTime"; 	break;
		case eEpochDay2TimeStr: stype = "eEpochDay2TimeStr";	break;
		case eDayOfWeekToStr:   stype = "eDayOfWeekToStr";  	break;
		case eEpochHour2TimeStr:stype = "eEpochHour2TimeStr";	break;
		case eFunc:				stype = "eFunc";				break;
		default:
			 rdata->setError() << "Unrecognized field type: " << fields[i].type;
			 OmnAlarm << rdata->getErrmsg() << enderr;
			 stype ="";
			 break;
		}

		int xmlidx = (fields[0].join_idx > 0 && fields[0].join_idx < numXmls)?
			fields[0].join_idx:0;
		AosXmlTagPtr xml = xmls[xmlidx];
		if (xml && xmlidx != 0)
		{
			join = xml->getAttrStr(AOSTAG_DOCID);
		}
		else
		{
			join = "0";
		}
	
		if (ss != "") ss << ":";
		ss <<fields[i].oname
			<<"|$|" << fields[i].cname << "|$|" << stype <<"|$|" <<fields[i].exist
			<< "|$|" << join;
	}
	
	OmnString signature = AosSecUtil::signValue(ss);
	if (signature == "")
	{
		rdata->setError() << "Failed signing the value";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}
	else
	{
		//record << " " <<AOSTAG_PARTIAL_DOC_FNAMES << " =\"" << ss << "\" " 
		//	<< AOSTAG_PARTIAL_DOC_SIGNATURE << "=\"" << signature << "\" >";
		record->setAttr(AOSTAG_PARTIAL_DOC_FNAMES, ss);
		record->setAttr(AOSTAG_PARTIAL_DOC_SIGNATURE, signature);
	}
	return true;
}


bool
AosQrUtil::createRecords(
		const AosRundataPtr &rdata,
		map<OmnString, AosBuffPtr> &columns,
		vector<FieldDef> &fields,
		vector<AosBuffPtr> &results, 
		const AosAsyncRespCallerPtr &resp_caller,
		int	&num_async_calls)
{
	// It creates the final results from the input data 'queried_data'.
	// 		[docid, field, field, ..., field]
	// 		[docid, field, field, ..., field]
	// 		...
	// 		[docid, field, field, ..., field]
	// This results will be used by another jimo that will convert these 
	// data into the final format. 
	int numFields = fields.size();
	if (numFields == 0 || (numFields == 1 && fields[0].type == eXml))
	{
		//need to be implemented!
		return true;
	}

	for (int i=0; i<numFields; i++)
	{
		OmnString column_name = fields[i].cname;
		map<OmnString, AosBuffPtr>::iterator itr = columns.find(column_name);
		aos_assert_r(itr != columns.end(),  false);
		AosBuffPtr column = itr->second;
		procOneColumn(rdata, i, fields[i], column, results, resp_caller, num_async_calls);
	}

	return true;
}


bool
AosQrUtil::procOneColumn(
		const AosRundataPtr &rdata,
		int	&field_idx,
		const FieldDef &field, 
		const AosBuffPtr &column,
		vector<AosBuffPtr> &results, 
		const AosAsyncRespCallerPtr &resp_caller,	
		int &num_async_calls)
{
	switch (field.type)
	{
	case AosQrUtil::eAttr:
		 if (field.sdoc_objid != "")
		 {
			 rdata->setColumn(column);
			 AosSmartDocObjPtr sobj = AosSmartDocObj::getSmartDocObj();
			 aos_assert_r(sobj, false);
			 rdata->setArg1("field_idx", field_idx);
			 rdata->setArg1("is_async_call", 0);
			 rdata->setAsyncRespCaller(resp_caller);
			 sobj->procSmartdocs(field.sdoc_objid, rdata);
			 OmnString r = rdata->getArg1("is_async_call");

			 if (r == "1")
			 {
				 num_async_calls++;
//				 results.push_back(0);
			 }
			 else
			 {
//			 	AosBuffPtr column = rdata->getColumnValue();
			 	results.push_back(column);
			 }
			 return true;
		 }
		 results.push_back(column);
		 return true;

	case AosQrUtil::eText:
	case AosQrUtil::eCdata:
	case AosQrUtil::eEpoch2TimeStr:
	case AosQrUtil::eEpoch2LocalTime:
	case AosQrUtil::eEpochDay2TimeStr:
	case AosQrUtil::eEpochHour2TimeStr:
	//case AosQrUtil::eFunc:
		 results.push_back(column);
		 return true;

	default:
		 break;
	}

	rdata->setError() << "Unrecognized field type: " << field.type;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}


/*
while (doc = getDoc())
{
	for (u32 i=0; i<fields.size(); i++)
	{
		procOneField(rdata, doc, fields[i], columns[i]);
	}
}
*/


// Version 3, Jackie, 2013/09/28
bool
AosQrUtil::procOneField(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &xml,
		const FieldDef &field, 
		OmnString &value)
{
	// This function retrieves the value of the field 'field' from the
	// doc 'xml'. The results are pushed to 'buff'.
	
	bool exist = false;
	OmnString querystr;
	OmnString subtags;
	OmnString record;
	switch (field.type)
	{
	case AosQrUtil::eAttr:
		 // It is an attribute and it is treated as an attribute
		 // in the response. 
		 
		 value = xml->xpathQuery(field.oname, exist, "");
		 //filter special character :
		 //& -> &amp;
		 //< -> &lt;
		 //> -> &gt;
		 //" -> &quot;
		 //' -> '
		
		 value.replace("&", "&amp;", true);
		 value.replace("<", "&lt;", true);
		 value.replace(">", "&gt;", true);
		 value.replace("\"", "&quot;", true);
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 break;

	case AosQrUtil::eText:
		 // The field is a subtag and it is converted as a subtag
		 // of the response. The subtag name is 'cname'.
		 querystr = field.oname;
		 querystr << "/_$text";
		 value = xml->xpathQuery(querystr, exist, "");
		 if (!exist && field.exist)
		 {
			return false;
		 }
		 for (int i=0; i<value.length(); i++)
		 {
			 if (value[i] == 0)
			 {
				 OmnAlarm << "value is empty" << enderr;
				 return false;
			 }
		 }

		 //filter CDATA 
		 value.replace("<![CDATA[", "", true);
		 value.replace("]]>", "", true);


		 subtags << "<" << field.cname << "><![CDATA["
			<< value 
			<< "]]></" << field.cname << ">";

		 rdata->setArg1(field.cname, value);
		 break;
		 
	case AosQrUtil::eXml:
		 // The field is the object itself. It is converted as a subtag. 
		 // The subtag name is 'cname'. If 'cname' is empty, it will 
		 // attach the xml object directly to the record.
		 if (field.cname != "")
		 {
			 subtags << "<" << field.cname << ">"
				 << (char *)xml->getData()
				 << "</" << field.cname << ">";
		 }
		 else
		 {
			 subtags << (char *)xml->getData();
		 }
		 break;

	case AosQrUtil::eCdata:
		 // The field is the object itself. It is converted as the CDATA
		 // of a subtag. The subtag name is 'cname'. If 'cname' is empty, 
		 // it will attach the XML object directly to the record.
		 if (field.cname != "")
		 {
			 subtags << "<" << field.cname << "><![CDATA["
				 << (char *)xml->getData() << "]]></" 
				 << field.cname << ">";
		 }
		 else
		 {
			 subtags << "<![CDATA[" << (char *)xml->getData() << "]]>";
		 }
		 break;

	case AosQrUtil::eEpoch2TimeStr:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 if (value != "")
		 {
			// Chen Ding, 2014/01/29
			// u64 vv = atoll(value.data());
			// if (vv > 0)
			// {
			// 	value = AosTimeUtil::EpochToTime(vv);
			// 	record << " " << field.cname << "=\"" << value << "\"";
			// }
			i64 vv = atoll(value.data());
			value = AosTimeUtil::EpochToTimeNew(vv);
			record << " " << field.cname << "=\"" << value << "\"";
		 }
		 break;

	case AosQrUtil::eEpoch2LocalTime:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
		 	return false;
		 }

		 if (value != "")
		 {
			// Chen Ding, 2014/01/29
			// u64 vv = atoll(value.data());
			// if (vv > 0)
			// {
			// 	value = AosTimeUtil::EpochToLocalTime(rdata, vv);
			// 	record << " " << field.cname << "=\"" << value << "\"";
			// }
			i64 vv = atoll(value.data());
			value = AosTimeUtil::EpochToLocalTimeNew(rdata, vv);
			record << " " << field.cname << "=\"" << value << "\"";
		 }
		 break;
	
	case AosQrUtil::eEpochDay2TimeStr:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
			 return false;
		 }

		 if (value != "")
		 {
			 // Chen Ding, 2014/01/29
			 // u64 vv = atoll(value.data());
			 // if (vv > 0)
			 // {
			 // 	 vv = vv * 24 * 60 * 60;
			 // 	 value = AosTimeUtil::EpochToTime(vv, "%Y-%m-%d");
			 // 	 record << " " << field.cname << "=\"" << value << "\"";
			 // }
			 i64 vv = atoll(value.data());
			 vv = vv * 24 * 60 * 60;
			 value = AosTimeUtil::EpochToTimeNew(vv, "%Y-%m-%d");
			 record << " " << field.cname << "=\"" << value << "\"";
		 }
		 break;

	case AosQrUtil::eEpochHour2TimeStr:
		 //Convert from epoch to human readable date
		 value = xml->xpathQuery(field.oname, exist, "");
		 if (!exist && field.exist)
		 {
			 return false;
		 }

		 if (value != "")
		 {
			 // Chen Ding, 2014/01/29
			 // u64 vv = atoll(value.data());
			 // if (vv > 0)
			 // {
			 // 	 vv = vv * 60 * 60;
			 // 	 value = AosTimeUtil::EpochToTime(vv, "%Y-%m-%d %H:%M:%S");
			 // 	 record << " " << field.cname << "=\"" << value << "\"";
			 // }
			 i64 vv = atoll(value.data());
			 vv = vv * 60 * 60;
			 value = AosTimeUtil::EpochToTimeNew(vv, "%Y-%m-%d %H:%M:%S");
			 record << " " << field.cname << "=\"" << value << "\"";
		 }
		 break;

	case AosQrUtil::eFunc:
		 {
			 bool outofmem = false;
			 value = xml->xpathQuery(field.oname, exist, "");
			 AosValueRslt v(value);
			 field.mRcdPtr->setFieldValue(0, v, outofmem, rdata.getPtr());
			 AosValueRslt vv;
			 bool rslt = field.mExprPtr->getValue(rdata.getPtr(), field.mRcdPtr.getPtr(), vv);
			 aos_assert_r(rslt, false);
			 record << " " << field.cname << "=\"" << vv.getStr() << "\"";
			 break;
		 }

	default:
		 rdata->setError() << "Unrecognized field type: " << field.type;
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}

	return true;
}


