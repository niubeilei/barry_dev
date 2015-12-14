////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ResMgrTester.cpp
// Description:
//   
//
// Modification History:
// Created by CHK 2007-04-04
////////////////////////////////////////////////////////////////////////////

#include "resmgr.h"
#include <stdio.h>

// for debug trace
#include <stdarg.h>
#include <string.h>

// for sleep
#include <unistd.h>

char * 
api_debug(const int ret, char *msg, ...)
{
	static char szBuffer[1000];
	va_list args;
	va_start(args, msg);
	if(ret)
	{
		sprintf(szBuffer, " ...Fail[%d]\n", ret);
		vsprintf((szBuffer + strlen(szBuffer)), msg, args);
	}
	else
	{
		sprintf(szBuffer, " ...OK \n");
		vsprintf((szBuffer + strlen(szBuffer)), msg, args);
	}
	va_end(args);
	return szBuffer;
}

#define _API_DEBUG_TRACE(ret, format, x...) \
			{ \
				printf(#ret " file[%s] line[%d]\t %s", __FILE__, __LINE__, api_debug(ret, format, ##x)); \
			} 
// 				if(ret != 0){ return ret; }\


int ResMgrTest(int argc,char **argv)
{
	int ret = 0;
	u32 arr_size = 0;
	u32 n_danger_threshold, n_danger_duration, n_normal_threshold, n_normal_duration;
	char rslt[5000]; 
	u32 rslt_size = 5000;

	_API_DEBUG_TRACE(aos_resmgr_start(),"aos_resmgr_start start the resource manager.\n");

	////////////////////////////////////////////////////////////////////////////
	// resource manager API start                                         
	////////////////////////////////////////////////////////////////////////////
	_API_DEBUG_TRACE(aos_resmgr_clear_config(), "");
	_API_DEBUG_TRACE(aos_resmgr_retrieve_config(rslt, rslt_size), "");
	_API_DEBUG_TRACE(aos_resmgr_save_config("~/abc"), "");
	_API_DEBUG_TRACE(aos_resmgr_load_config("~/abc"), "");
	_API_DEBUG_TRACE(aos_resmgr_nice(1, 3), "");

	////////////////////////////////////////////////////////////////////////////
	// CPU manager API                                             
	////////////////////////////////////////////////////////////////////////////
	_API_DEBUG_TRACE(aos_cpumgr_start(),"aos_cpumgr_start start the cpu manager.");
	_API_DEBUG_TRACE(aos_cpumgr_set_record_time(5, 1*24*60*60), "");
	aos_cpumgr_stat_rslt_t cpu_stat_rslt[128];
	arr_size = 128;
	sleep(15);
	ret = aos_cpumgr_retrieve_cpu(cpu_stat_rslt, &arr_size, -15,  -5,  10);  
	_API_DEBUG_TRACE(0, "aos_cpumgr_retrieve_cpu(cpu_stat_rslt, &arr_size, -30,  -5,  15) ErrorCode[%d]", ret); 
	aos_cpumgr_process_stat_rslt_t cpu_proc_stat_rslt;
	_API_DEBUG_TRACE(aos_cpumgr_retrieve_process(&cpu_proc_stat_rslt, \
						-30,  \
						-5,  \
						15), ""); 
	_API_DEBUG_TRACE(aos_cpumgr_set_threshold( \
												65,\
												60,\
												90,\
												65), "");

	_API_DEBUG_TRACE(aos_cpumgr_get_threshold(\
									&n_danger_threshold, \
									&n_danger_duration, \
									&n_normal_threshold, \
									&n_normal_duration), "");

	_API_DEBUG_TRACE(aos_cpumgr_stop()," \n");

	////////////////////////////////////////////////////////////////////////////
	// disk manager API                                             
	////////////////////////////////////////////////////////////////////////////
// 1. 
	_API_DEBUG_TRACE(aos_diskmgr_start(),"aos_diskmgr_start start the disk manager.");

// 3. 
	_API_DEBUG_TRACE(aos_diskmgr_set_record_time(1*60*60, 1*24*60*60), "");
// 4. 
	_API_DEBUG_TRACE(aos_diskmgr_set_threshold( \
												65,\
												60,\
												90,\
												65), "");

// 5. 
	_API_DEBUG_TRACE(aos_diskmgr_get_threshold(\
									&n_danger_threshold, \
									&n_danger_duration, \
									&n_normal_threshold, \
									&n_normal_duration), "");

// 6. 
	aos_diskmgr_file_stat_t disk_stat_rslt[128];
	arr_size = 128;
	sleep(15);
	_API_DEBUG_TRACE(aos_diskmgr_retrieve_statistics(disk_stat_rslt, &arr_size), "");
// 7. 
//	_API_DEBUG_TRACE(aos_diskmgr_filetype_retrieve(aos_diskmgr_filetype_t * const rslt, 
//								  int * const arr_size), "");

// 8. 
//	_API_DEBUG_TRACE(aos_diskmgr_get_num_hd(int * hd_num), "");
// 9. 
//	_API_DEBUG_TRACE(aos_diskmgr_get_num_partition(int * part_num, const char * dev_path), "");
// 10. 
//	_API_DEBUG_TRACE(aos_diskmgr_get_partition_info(
//				aos_diskmgr_partition_info_t * const part_info, 
//				const char * dev_path), "");
// 11. 
//	_API_DEBUG_TRACE(aos_diskmgr_get_harddisk_info(aos_diskmgr_harddisk_info_t * const d, 
//								   const char * dev_path), "");
// 12. 
//	_API_DEBUG_TRACE(aos_diskmgr_get_all_hard_disks(aos_diskmgr_harddisk_info_t * const d, 
//								   int *num_hd), "");
// 13. 
//	_API_DEBUG_TRACE(aos_diskmgr_format_hard_disk(aos_diskmgr_harddisk_info_t * const harddisk_info, 
//								 const char* hd_dev, 
//								 const struct aos_harddisk_set_info* set_info), "");

// 14. 
//	_API_DEBUG_TRACE(aos_diskmgr_mount_folder("/tmp", "/dev/sda2"), "");
// 15. 
//	_API_DEBUG_TRACE(aos_diskmgr_unmount_folder("/tmp"), "");
// 16. 
	_API_DEBUG_TRACE(aos_diskmgr_filetype_add("chk"), "");

// 16. 
	ret = aos_diskmgr_filetype_add(NULL);
	_API_DEBUG_TRACE(0, "aos_diskmgr_filetype_add = %d \n", ret);
	ret = aos_diskmgr_filetype_add("chk");
	_API_DEBUG_TRACE(0, "aos_diskmgr_filetype_add = %d \n", ret);

// 17. 
	_API_DEBUG_TRACE(aos_diskmgr_filetype_remove("chk"), "");
// 18. 
//	_API_DEBUG_TRACE(aos_diskmgr_filetype_clear(), "");

// 2. 
	_API_DEBUG_TRACE(aos_diskmgr_stop(),"aos_diskmgr_stop \n");

	////////////////////////////////////////////////////////////////////////////
	// memory manager API                                             
	////////////////////////////////////////////////////////////////////////////
	_API_DEBUG_TRACE(aos_memmgr_start(),"aos_memmgr_start start the disk manager.\n");

	_API_DEBUG_TRACE(aos_memmgr_set_record_time(5, 1*24*60*60), "");

	aos_memmgr_stat_rslt_t mem_stat_rslt[128];
	arr_size = 128;
	sleep(15);
	_API_DEBUG_TRACE(aos_memmgr_retrieve_mem(mem_stat_rslt, &arr_size, -15,  -1,  8), "");

	aos_memmgr_process_stat_rslt_t mem_proc_stat_rslt;
	arr_size = 128;
	_API_DEBUG_TRACE(aos_memmgr_retrieve_process(&mem_proc_stat_rslt, -15,  -1,  8), "");
	                                
	_API_DEBUG_TRACE(aos_memmgr_set_threshold( \
												65,\
												60,\
												90,\
												65), "");
	
	_API_DEBUG_TRACE(aos_memmgr_get_threshold(\
									&n_danger_threshold, \
									&n_danger_duration, \
									&n_normal_threshold, \
									&n_normal_duration), "");                    
	u32 n_total_memory, n_total_used, 
		  n_total_swapped, n_total_cached;
	int   n_self_used_mem,
		  n_app_used_mem,
		  n_proc_used_mem;
	_API_DEBUG_TRACE(aos_memmgr_retrieve_system_usage(\
									&n_total_memory, \
									&n_total_used, \
									&n_total_swapped, \
									&n_total_cached), "");

	_API_DEBUG_TRACE(aos_memmgr_retrieve_self_memory_usage(&n_self_used_mem), "");
	_API_DEBUG_TRACE(aos_memmgr_retrieve_app_memory_usage(&n_app_used_mem, ("SystemCli")), "");
	_API_DEBUG_TRACE(aos_memmgr_retrieve_proc_memory_usage(&n_proc_used_mem, 1), "");

	_API_DEBUG_TRACE(aos_memmgr_stop(),"aos_memmgr_stop \n");

	////////////////////////////////////////////////////////////////////////////
	// resource manager API end                                          
	////////////////////////////////////////////////////////////////////////////
	_API_DEBUG_TRACE(aos_resmgr_stop(),"aos_resmgr_stop \n");

	return 0;
}
//                                                                
// ////////////////////////////////////////////////////////////////////////////
// // CPU manager API                                             
// ////////////////////////////////////////////////////////////////////////////
// typedef struct aos_cpumgr_stat_rslt                            
// {                                                              
// 		long m_sec_index;                                      
// 		float m_cpu_usage; /* (percentage)m_cpu_usage = 1 - (m_jeffies_cpu_idle) / m_jeffiesDuration */
// 		unsigned long m_jeffiesDuration;                       
// 		float m_cpu_user;                                      
// 		float m_cpu_nice;                                      
// 		float m_cpu_system;                                    
// 		float m_cpu_idle;                                                    
// 		float m_cpu_iowait;                                                  
// 		float m_cpu_irq;                                       
// 		float m_cpu_sirq;                                      
// 		unsigned long m_jeffies_cpu_user;                      
// 		unsigned long m_jeffies_cpu_nice;                      
// 		unsigned long m_jeffies_cpu_system;                    
// 		unsigned long m_jeffies_cpu_idle;                      
// 		unsigned long m_jeffies_cpu_iowait;                    
// 		unsigned long m_jeffies_cpu_irq;                       
// 		unsigned long m_jeffies_cpu_sirq;                      
// } aos_cpumgr_stat_rslt_t;                                      
//                                                                
// #define AOS_RESMGR_MAX_PROCESS_NUM 256                         
// #define AOS_RESMGR_MAX_PROCESS_NAME_LEN 32                     
//                                                                
// typedef struct aos_cpumgr_process_stat_entry                   
// {                                                              
//    unsigned long pid;                                          
//    char 	name[AOS_RESMGR_MAX_PROCESS_NAME_LEN];             
//    unsigned long utime;                                        
//    unsigned long stime;                                        
//    float using_ratio;                                          
// } aos_cpumgr_process_stat_entry_t;                             
//                                                                
// typedef struct aos_cpumgr_process_stat_rslt                    
// {                                                              
// 	unsigned long jeffiesDuration;                             
// 	float cpu_user;                                            
// 	float cpu_nice;                                            
// 	float cpu_system;                                          
// 	float cpu_idle;                                            
// 	float cpu_iowait;                                          
// 	float cpu_irq;                                             
// 	float cpu_sirq;                                            
// 	unsigned long jeffies_cpu_user;                            
// 	unsigned long jeffies_cpu_nice;                            
// 	unsigned long jeffies_cpu_system;                          
// 	unsigned long jeffies_cpu_idle;                            
// 	unsigned long jeffies_cpu_iowait;                          
// 	unsigned long jeffies_cpu_irq;                             
// 	unsigned long jeffies_cpu_sirq;                            
// 	int pnum;                                                  
// 	aos_cpumgr_process_stat_entry_t p[AOS_RESMGR_MAX_PROCESS_NUM];
// } aos_cpumgr_process_stat_rslt_t;                              
//                                                              
// int aos_cpumgr_start();   
// int aos_cpumgr_stop();   
// int aos_cpumgr_set_record_time(const __u16 interval, const long duration);
// int aos_cpumgr_retrieve_cpu(aos_cpumgr_stat_rslt_t * const rslt, 
// 							u32 *arr_size, const long start_time, 
// 							const long end_time,               
// 							const long interval);           
// int aos_cpumgr_retrieve_process(aos_cpumgr_process_stat_rslt_t * const rslt_ptr, 
// 							const long start_time,             
// 							const long end_time,               
// 							const int num);
// int aos_cpumgr_set_threshold(                                  
// 			const u32 busy_threshold,                        
// 			const u32 busy_duration,                         
// 			const u32 normal_threshold,                      
// 			const u32 normal_duration);                      
// 
// int aos_cpumgr_get_threshold(                                  
// 			u32 * const busy_threshold,                      
// 			u32 * const busy_duration,                       
// 			u32 * const normal_threshold,                    
// 			u32 * const normal_duration);                    
//                                                                
// ////////////////////////////////////////////////////////////////////////////
// // Memory manager API                                          
// ////////////////////////////////////////////////////////////////////////////
// typedef struct aos_memmgr_stat_rslt                            
// {                                                              
// 	long m_sec_index;                                          
// 	float m_total_usage_ratio;                                 
// 	long m_total_memory;                                       
// 	long m_total_free;                                         
// 	long m_total_buffer;                                       
// 	long m_total_cached;                                       
// } aos_memmgr_stat_rslt_t;                                      
//                                                                
// typedef struct aos_memmgr_process_stat_entry                   
// {                                                              
// 	unsigned long m_pid;                                       
// 	char 	m_name[AOS_RESMGR_MAX_PROCESS_NAME_LEN];           
// 	int 	m_intensve_nr;                                     
// 	long 	m_peak_index;                                      
// 	float 	m_peak_usage_ratio;                                
// } aos_memmgr_process_stat_entry_t;                             
//                                                                
// typedef struct aos_memmgr_process_stat_rslt                    
// {                                                              
// 	long  m_beg_index;                                         
// 	long  m_end_index;                                         
// 	long  m_peak_index;                                        
// 	float m_peak_usage_ratio;                                  
// 	int   m_pnum;                                              
// 	aos_memmgr_process_stat_entry_t m_p[AOS_RESMGR_MAX_PROCESS_NUM];
// } aos_memmgr_process_stat_rslt_t;                              
//     
// int aos_memmgr_start(); 
// int aos_memmgr_stop();     
// int aos_memmgr_set_record_time(const __u16 interval, const long duration);
// int aos_memmgr_retrieve_mem(aos_memmgr_stat_rslt_t * const rslt, 
// 							u32 *arr_size, const long start_time, 
// 							const long end_time,               
// 							const long interval);              
//                                                      
// int aos_memmgr_retrieve_process(aos_memmgr_process_stat_rslt_t * const rslt_ptr, 
// 							const long start_time,             
// 							const long end_time,               
// 							const int num);                    
//                                     
// int aos_memmgr_set_threshold(                                  
// 			const u32 busy_threshold,                        
// 			const u32 busy_duration,                         
// 			const u32 normal_threshold,                      
// 			const u32 normal_duration);                      
//      
// int aos_memmgr_get_threshold(                                  
// 			u32 * const busy_threshold,                      
// 			u32 * const busy_duration,                       
// 			u32 * const normal_threshold,                    
// 			u32 * const normal_duration);                    
//                     
// int aos_memmgr_retrieve_system_usage(                          
// 		u32 *total_memory,                                   
// 		u32 *total_used,                                     
// 		u32 *total_swapped,                                  
// 		u32 *total_cached);                                  
//       
// int aos_memmgr_retrieve_self_memory_usage(int * const ret_val);
// int aos_memmgr_retrieve_app_memory_usage(int * const ret_val, const char *app_name);
// int aos_memmgr_retrieve_proc_memory_usage(int * const ret_val, const u32 pid);
//                                                                
// ////////////////////////////////////////////////////////////////////////////
// // Disk manager API                                            
// ////////////////////////////////////////////////////////////////////////////
// #define AOS_DISKMGR_MAX_HDS 32                                 
// #define AOS_DISKMGR_MAX_PARTNUM_IN_HD 16                       
// #define AOS_RESMGR_MAX_DEV_PATH_LEN 128                        
// #define AOS_HD_VENDOR_NAME_LEN 256                             
//                                                                
// typedef struct aos_diskmgr_partition_info                      
// {                                                              
// 		char m_dev_path[AOS_RESMGR_MAX_DEV_PATH_LEN]; /* ID of device */
// 		float m_usage_ratio;                                   
// 		long m_space;                                          
// 		long m_used;                                           
// 		unsigned int m_starting_cyls;                          
// 		unsigned int m_nr_of_cyls;                             
// 		unsigned char m_type_id; /* Each partition has a type, its Id */ 
// 		long m_num_files;                                      
// } aos_diskmgr_partition_info_t;                                
//                                                                
// typedef struct aos_diskmgr_harddisk_info                       
// {                                                              
// 		char m_dev_path[AOS_RESMGR_MAX_DEV_PATH_LEN]; /* ID of device */
// 		char m_vendor[AOS_HD_VENDOR_NAME_LEN]; /* here we just get the information on hard-disk¡¯s chip */
// 		long m_space;                                          
// 		long m_used;                                           
// 		long m_num_files;                                      
// 		long m_pnum;                                           
// 		aos_diskmgr_partition_info_t m_p [AOS_DISKMGR_MAX_PARTNUM_IN_HD];
// } aos_diskmgr_harddisk_info_t;                                 
//                                                                
// typedef struct aos_format_harddisk_set_info                    
// {                                                              
// 		int m_size; /* unit is mega-bytes */                   
// 		char m_part_format[16];                                
// } aos_format_harddisk_input_t;                                 
//                                                                
// typedef struct aos_diskmgr_file_stat                           
// {                                                              
// 		char m_file_type[16];                                  
// 		unsigned long m_file_type_size;                        
// } aos_diskmgr_file_stat_t;                                     
//                                                                
// typedef struct aos_diskmgr_filetype                            
// {                                                              
// 		char m_file_type[16];                                  
// } aos_diskmgr_filetype_t;                                      
//                                                               
// int aos_diskmgr_start();                                       
// int aos_diskmgr_stop();                                        
// int aos_diskmgr_set_record_time(const __u16 interval, const long duration);
// int aos_diskmgr_set_threshold(                                 
// 			const u32 busy_threshold,                        
// 			const u32 busy_duration,                         
// 			const u32 normal_threshold,                      
// 			const u32 normal_duration);                      
//                                                   
// int aos_diskmgr_get_threshold(                                 
// 			u32 * const busy_threshold,                      
// 			u32 * const busy_duration,                       
// 			u32 * const normal_threshold,                    
// 			u32 * const normal_duration);                    
//                     
// int aos_diskmgr_retrieve_statistics(aos_diskmgr_file_stat_t * rslt,
// 									u32 * const arr_size);   
//                                                                
// int aos_diskmgr_filetype_retrieve(aos_diskmgr_filetype_t * const rslt, 
// 								  int * const arr_size);       
//                                                                
// int aos_diskmgr_get_num_hd(int * hd_num);                      
// int aos_diskmgr_get_num_partition(int * part_num, const char * dev_path);
// int aos_diskmgr_get_partition_info(                            
// 				aos_diskmgr_partition_info_t * const part_info, 
// 				const char * dev_path);                        
// int aos_diskmgr_get_harddisk_info(aos_diskmgr_harddisk_info_t * const d, 
// 								   const char * dev_path);     
// int aos_diskmgr_get_all_hard_disks(aos_diskmgr_harddisk_info_t * const d, 
// 								   int *num_hd);               
// int aos_diskmgr_format_hard_disk(aos_diskmgr_harddisk_info_t * const harddisk_info, 
// 								 const char* hd_dev,           
// 								 const struct aos_harddisk_set_info* set_info);
//                                                             
// int aos_diskmgr_mount_folder(const char *dest_path,            
// 								const char *dev_path);         
// int aos_diskmgr_unmount_folder(char *path);                            
// int aos_diskmgr_filetype_add(const char * file_type);      
// int aos_diskmgr_filetype_remove(const char * file_type);   
// int aos_diskmgr_filetype_clear();                              
                                                               
///////////////////////////////////////////////////////////////

