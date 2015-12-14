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
// 03/22/2012 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "ImportData/Testers/TestSuite.h"

#include "DataFormat/Ptrs.h"
#include "DataFormat/DataFormatterXml.h"
#include "IILMgr/IILStr.h"
#include "IILMgr/IILU64.h"
#include "ImportData/Ptrs.h"
#include "ImportData/Testers/ImportDataTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosImportDataTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("ImportData", "Util Library Test Suite");

	if (strcmp(type.data(), "importdatatest") == 0)
	{
		//suite->addTestPkg(OmnNew AosImportDataTester());
	}
	
	if(strcmp(type.data(), "help") == 0 ||strcmp(type.data(), "--help") == 0 )
	{
		cout << "Testers can be :\n"
			 << "importdatatest\n";
		exit(0);
	}
	return suite;

	// suite->addTestPkg(OmnNew AosIILU64Tester());
}


bool
AosImportDataTestSuite::createData(
		const AosXmlTagPtr &config, 
		const AosRundataPtr &rdata)
{
	// The format is:
	// 	<importdata fname_base="xxx"
	// 		num_files="xxx"
	// 		batch_size="xxx"
	// 		num_docs_per_file="xxx">
	// 		<elements AOSTAG_NAME="doctagname"
	// 			AOSTAG_FLUSH_FLAG="true|false">
	// 			<element AOSTAG_TYPE="AOSDATAELEMTYPE_U64"
	// 			 	AOSTAG_ATTRNAME="xxx"
	// 			  	AOSTAG_OFFSET="xxx"
	// 			   	AOSTAG_NEED_CONVERT="true|false"
	// 			    AOSTAG_FIXED_POSITION="true|false"/>
	// 			...
	// 		</elements>
	// 	</importdata>
	aos_assert_rr(config, rdata, false);
	
	AosXmlTagPtr impcfg = config->getFirstChild("importdata");
	aos_assert_rr(impcfg, rdata, false);
	
	OmnString dir_base = impcfg->getAttrStr("dir_base", "");
	OmnString fname_base = impcfg->getAttrStr("fname_base");
	aos_assert_rr(fname_base != "", rdata, false);

	int num_files = impcfg->getAttrInt("num_files", -1);
	aos_assert_rr(num_files > 0, rdata, false);

	int batch_size = impcfg->getAttrInt("batch_size", 1000);
	u64 num_docs_per_file = impcfg->getAttrU64("num_docs_per_file", 10000000);

	// This function creates a number of files. 
	AosXmlTagPtr format_def = impcfg->getFirstChild(AOSTAG_FORMATTER);
	aos_assert_rr(format_def, rdata, false);
	
	AosDataFormatterXmlPtr formatter = OmnNew AosDataFormatterXml(format_def, rdata);	
	u64 docid = 0;
	u32 start_sec = OmnGetSecond();
	OmnString fname;
	for (int i=0; i<num_files; i++)
	{
		fname = "";
		fname << dir_base << "/" << fname_base << "_" << i;
		AosAppendFilePtr file = OmnNew AosAppendFile(fname);

		for (u64 mm=0; mm<num_docs_per_file; mm+=(u64)batch_size)
		{
			AosBuffPtr buff = OmnNew AosBuff(10000000, 10000000 AosMemoryCheckerArgs);
			if (docid % 100000 == 0)
			{
				u32 dur = OmnGetSecond() - start_sec;
				if (dur > 0)
				{
					OmnScreen << "Docs Created: " << docid << "(" << docid / 1000000 << ")"
						<< ", Time: " << dur 
						<< ", Rate: " << docid/dur << endl;
				}
			}
			for (int k=0; k<batch_size; k++)
			{
				docid++;
				formatter->createRandomDoc(buff, rdata);
			}
			file->append(buff->data(), buff->dataLen(), true);
		}
	}

	u32 end_sec = OmnGetSecond();
	u32 duration = end_sec - start_sec;
	if (duration > 0)
	{
		OmnScreen << "Finished" << endl
			  << "     Total docs created: " << docid << endl
			  << "     Total Second:       " << duration << endl
			  << "     Rate:               " << 1.0 * docid / duration << endl;
	}

	return true;
}


