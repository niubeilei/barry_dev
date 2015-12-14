#include "ParentCtrl/webwall_util.h"
#include "ParentCtrl/webwall_common.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

char* get_file_name(char* path)
{
	static char file_name[1024];
	char* p;
	
	p = path + strlen(path) - 1;
	while (p != path) {
		if (*p == '/')
			break;
		p--;
	}
	if (*p == '/')
		p++;
	memset(file_name, 0, 1024);
	strcpy(file_name, p);
	
	return file_name;
}

int is_running_by_name(char* pidName)
{
	DIR *dir;
	struct dirent *next;
    
    dir = opendir("/proc");  
    if (!dir) {
    	printf("Cannot   open   /proc");
    	return -1;
    }
    
	#define READ_BUF_SIZE 256   
	
    while ((next = readdir(dir)) != NULL) {  
    	FILE *status;
    	char filename[READ_BUF_SIZE];  
        char buffer[READ_BUF_SIZE];  
        char name[READ_BUF_SIZE];  
  		/*   Must   skip   ".."   since   that   is   outside   /proc   */
  		if (strcmp(next->d_name, "..") == 0)  
        	continue;  
   		
   		/*   If   it   isn't   a   number,   we   don't   want   it   */
   		if (!isdigit(*next->d_name))
   			continue;  
  		
  		sprintf(filename,   "/proc/%s/status",   next->d_name);  
        if (!(status = fopen(filename, "r"))) {
        	continue;
        }  
        if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL) {
        	fclose(status);
        	continue;
        }
        fclose(status);
        
        /*   Buffer   should   contain   a   string   like   "Name:       binary_name"   */
        sscanf(buffer,   "%*s   %s",   name);  
        if (strcmp(name, pidName) == 0) {
        	return 0;
        }
	}
	
	return -1;
}

/*
 * check the valid of file name
 * the principal is that permiting [a-zA-Z] and [-_]
 * @file_name
 * @return -1 if invalid file name
 */
int is_valid_file_name(char* file_name)
{
	char* p;

	if (file_name == NULL)
		return -1;
	
	p = file_name;
	while (*p) {
		if ((*p >= 'a' && *p <= 'z')
			|| (*p >= 'A' && *p <= 'Z')
			|| (*p == '-' || *p == '_')
			|| (*p >= '0' && *p <= '9')) {
			p++;
			continue;
		} else
			return -1;
	}

	return 0;
}

