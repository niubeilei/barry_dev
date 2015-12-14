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
// 2007-04-30 Created by CHK
////////////////////////////////////////////////////////////////////////////

#ifndef aos_rhcUtil_resmgr_xml_h
#define aos_rhcUtil_resmgr_xml_h

typedef struct aos_cpumgr_stat_rslt
{
		long m_sec_index;
		float m_cpu_usage; /* (percentage)m_cpu_usage = 1 - (m_jeffies_cpu_idle) / m_jeffiesDuration */
		unsigned long m_jeffiesDuration;
		float m_cpu_user;
		float m_cpu_nice;
		float m_cpu_system;
		float m_cpu_idle;
		float m_cpu_iowait;
		float m_cpu_irq;
		float m_cpu_sirq;
		unsigned long m_jeffies_cpu_user;
		unsigned long m_jeffies_cpu_nice;
		unsigned long m_jeffies_cpu_system;
		unsigned long m_jeffies_cpu_idle;
		unsigned long m_jeffies_cpu_iowait;
		unsigned long m_jeffies_cpu_irq;
		unsigned long m_jeffies_cpu_sirq; 
} aos_cpumgr_stat_rslt_t;

// generate xml format string
// return 0 means success
// return < 0 means error code
int aos_resmgr_xml_gen_cpu_stat_rslt(char * const str_rslt, const int str_size, const aos_cpumgr_stat_rslt_t * arr_rslt, const int arr_size);

// generate xml format string
// return 0 means success
// return < 0 means error code
// input arr_size is the total space lenghth of arr_rslt, 
// 			return arr_size should be the real meaningful value of arr_rslt
int aos_resmgr_xml_par_cpu_stat_rslt(aos_cpumgr_stat_rslt_t * const arr_rslt, int * arr_size, const char * str_rslt, const int str_size);

#endif // aos_rhcUtil_resmgr_xml_h
