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
// 04/18/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#if 0
#include "XmlInterface/Rvg.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "parser/xml_node.h"
#include "rvg_c/rvg.h"
#include "TorturerWrappers/RvgWrapper.h"
#include "Util/a_string.h"
#include "Util/File.h"
#include "util2/value.h"


// 
// Description:
// It modifies the RVG identified by 'objid'. If the object does not exist, 
// it creates one. The new RVG is 'contents', which is an xml definition
// of the RVG. If it failed to create an RVG from 'contents', it is an error.
// Otherwise, it checks whether the new object is correct. If not, it is 
// an error. If yes, it replaces the old definition with the new one.
//
bool AosRvg_Modify(
		aos_xml_node_t *node,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	aos_assert_r(node, false);

	char *objid, *contents;

   	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "objid", &objid) == 1, false);
   	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "contents", &contents) == 1, false);

	// 
	// Create the RVG. 
	//
	aos_rvg_t *rvg = aos_rvg_factory_str(contents);
	if (!rvg)
	{
		OmnAlarm << "Incorrect contents: " << contents << enderr;
		errcode = eAosXmlInt_SyntaxError;
		return false;
	}

	AosRvgWrapper wp;
	wp.setName(objid);
	char *kw = rvg->mf->get_keywords(rvg);
	if (kw) wp.setKeywords(kw);

	char *desc = rvg->mf->get_description(rvg);
	if (desc) wp.setDescription(desc);

	wp.setXml(contents);
	if (!AosRvgWrapper::rvgExist(objid))
	{
		// It is to create a new RVG.
		OmnRslt rslt = wp.addToDb();
		if (!rslt)
		{
			OmnAlarm << "Failed to save to database: " << rslt.getErrmsg() << enderr;
			errcode = eAosXmlInt_DbError;
			errmsg = "Failed to save to database.";
			return false;
		}
	}
	else
	{
		// It is to modify an existing one. We simply replace the existing
		// one with the new one.
		OmnRslt rslt = wp.updateToDb();
		if (!rslt)
		{
			OmnAlarm << "Failed to modify the RVG: " << rslt.getErrmsg() << enderr;
			errcode = eAosXmlInt_DbError;
			errmsg = "Failed to update the database.";
			return false;
		}
	}

	errcode = eAosXmlInt_Ok;
	return true;
}


bool AosRvg_ReadRvg(
		const OmnString &objid, 
		OmnString &contents, 
		AosXmlRc &errcode, 
		OmnString &errmsg) 
{
	AosRvgWrapper wp;
	if (wp.getRvg(objid))
	{
		// Found the RVG. 
		contents = wp.getXml();
		errcode = eAosXmlInt_Ok;
		return true;
	}

	// Did not find the object
	errcode = eAosXmlInt_ObjectNotFound;
	errmsg = "Name: ";
	errmsg << objid;
	return false;
}


bool AosRvg_Run(
		aos_xml_node_t *node,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	errcode = eAosXmlInt_InternalError;
	aos_assert_r(node, false);

	char *name=0, *def=0; 
	int noe=-1, seed=-1;
   	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "objid", &name) == 1, false);
   	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "contents", &def) == 1, false);
   	aos_assert_r(node->mf->get_int_by_attr((aos_field_t *)node, "name", "noe", &noe) == 1, false);
   	aos_assert_r(node->mf->get_int_by_attr((aos_field_t *)node, "name", "seed", &seed) == 1, false);

	aos_assert_r(name, false);
	aos_assert_r(def, false);
	aos_assert_r(noe > 0, false);
	aos_assert_r(seed >= 0, false);

	srand(seed);
	aos_rvg_t *rvg = aos_rvg_factory_str(def);
	if (!rvg)
	{
		OmnAlarm << "Incorrect contents: " << def << enderr;
		errcode = eAosXmlInt_SyntaxError;
		return false;
	}

	// 
	// Results are in the form of and stored in "run_rvg_results.xml"
	// 	<Results>
	// 		<Result seqno="xxx">xxxxx</Result>
	// 		<Result seqno="xxx">xxxxx</Result>
	// 		...
	// 		<Result seqno="xxx">xxxxx</Result>
	//	</Results>
	//
	OmnFile file("/home/chen.ding/OpenLaszlo/lps-4.0.10/Server/lps-4.0.10/TorturerGUI/modules/rvg_execution_result.xml", OmnFile::eCreate AosMemoryCheckerArgs);
	if (!file.isGood())
	{
		errcode = eAosXmlInt_FailedOpenFile;
		OmnAlarm << "Failed to open the file: run_rvg_results.xml" << enderr;
		return false;
	}

	char buff[100000];
	file.put(OmnFile::eNoSeek, "<Results>", 9, false);
	aos_value_t value;
	aos_value_init(&value);
	char *ptr;
	int len;
	for (int i=0; i<noe; i++)
	{
		aos_assert_r(!rvg->mf->next_value(rvg, &value), false);
		aos_assert_r(!value.mf->to_str_s(&value, &ptr, &len), false);
		sprintf(buff, "<Result seqno=\"%d\" len=\"%d\">%s</Result>", i, len, ptr);
		file.put(OmnFile::eNoSeek, buff, strlen(buff), false);
	}
	file.put(OmnFile::eNoSeek, "</Results>", 10, false);

	file.closeFile();
	errcode = eAosXmlInt_Ok;
OmnTrace << "File created" << endl;
	return true;
}

#endif
