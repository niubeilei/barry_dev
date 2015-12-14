////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: update.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#define UPDATE_DIR "/tmp"

int copy(const char *filename_src, const char *filename_des){
	FILE *fp_src, *fp_des;
	int ret;
	long int length;
	unsigned char *buffer;
	if(strlen(filename_src)==0){
		return 0;
	}
	if(strlen(filename_des)==0){
		return 0;
	}
	
	fp_src=fopen(filename_src, "rb");
	if(fp_src==NULL){
		fprintf(stderr, "Source file not exist\n");
		return -1;
	}
	fp_des=fopen(filename_des, "wb");
	if(fp_des==NULL){
		fprintf(stderr, "Open destation file error\n");
		return -1;
	}
	fseek(fp_src, 0, SEEK_END);
	length=ftell(fp_src);
	fseek(fp_src, 0, SEEK_CUR);
	buffer=(unsigned char *)malloc(length);
	if(buffer==NULL){
		fprintf(stderr, "allocal memory error!\n");
		return -1;
	}
	ret=fread(buffer, length, 1, fp_src);
	printf("ret=%d\n");
	ret=fwrite(buffer, length, 1, fp_des);
	printf("ret=%d\n");
	fclose(fp_src);
	fclose(fp_des);
	return 0;
}	

int select_kernel_old(const struct dirent *dir){
	if(memcmp(dir->d_name, "vmlinuz-sslproxy", 16)==0){
		if(strstr(dir->d_name, "bak")!=NULL){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

int select_initrd(const struct dirent *dir){
	if(memcmp(dir->d_name, "initrd", 6)==0){
		if(strstr(dir->d_name, "bak")==NULL){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

int select_initrd_old(const struct dirent *dir){
	if(memcmp(dir->d_name, "initrd", 6)==0){
		if(strstr(dir->d_name, "bak")!=NULL){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}
	
	
int select_file_new(const struct dirent *dir){
	if(memcmp(dir->d_name, "sslproxy-rel", 12)==0){
		if(strstr(dir->d_name, "bak")==NULL){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

int select_file_old(const struct dirent *dir){
	if(memcmp(dir->d_name, "sslproxy-rel", 12)==0){
		if(strstr(dir->d_name, "bak")==NULL){
			return 0;
		}else{
			return 1;
		}
	}else{
		return 0;
	}
}

int select_kernel(const struct dirent *dir){
	if(memcmp(dir->d_name, "vmlinuz-sslproxy", 16)==0){
		if(strstr(dir->d_name, "bak")==NULL){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

int select_old_kernel(const struct dirent *dir){
	if(memcmp(dir->d_name, "vmlinuz-sslproxy", 16)==0){
		if(strstr(dir->d_name, "bak")!=NULL){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}	

int insert_bak(char *filename){
	int i;
	char *b;

	b=strstr((const char *)filename, ".tar");
	i=b-filename;
	memcpy(filename+i, "-bak", 4);
	memcpy(filename+i+4, ".tar", 4);
	return 0;
}

int main(int argc, char *argv[]){
	int ret;
	char cmd[128];
	char filename[128];
	struct dirent **dir_file;
	int total;
	//char bak_filename[128];
	int major, minor, min;
	int major_old, minor_old, min_old;
	FILE *fp;
	int i, j;

	
	chdir("/usr/local/AOS/Upgrade");
	if(argc==1){
		total=scandir("./", &dir_file, select_file_old, 0);
		if(total==1){
			bzero(filename, 128);
			strcpy(filename, (*dir_file)->d_name);
			free(*dir_file);
		}else{
			printf("update file no exist\n");
			exit(1);
		}	
	}else{
		total=scandir("./", &dir_file, select_file_old, 0);
		if(total==1){
			memset(cmd, 0, 128);
			sprintf(cmd, "rm -f %s", (*dir_file)->d_name);
			system(cmd);
			free(*dir_file);
		}
		total=scandir("./", &dir_file, select_file_new, 0);
		printf("total is %d\n", total);
		if(total>1){
			memset(filename, 0, 128);
			if(strcmp(dir_file[0]->d_name, dir_file[1]->d_name)>0){
				memset(cmd, 0, 128);
				strcpy(filename, dir_file[1]->d_name);
				insert_bak(filename);
				sprintf(cmd, "mv %s %s", dir_file[1]->d_name, filename);
				system(cmd);
				bzero(filename, 128);
				strcpy(filename, dir_file[0]->d_name);
				free(*dir_file);
			}else{
				memset(cmd, 0, 128);
				strcpy(filename, dir_file[0]->d_name);
				insert_bak(filename);
				sprintf(cmd, "mv %s %s", dir_file[0]->d_name, filename); 
				system(cmd);
				bzero(filename, 128);
				strcpy(filename, dir_file[1]->d_name);
				free(*dir_file);
			}
		}else{
			if(total==1){
				memset(filename, 0, 128);
				strcpy(filename, dir_file[0]->d_name);
				free(*dir_file);
			}else{
				exit(1);
			}
		}
	}
			
	printf("filename is %s\n", filename);
	chdir("/tmp");
	memset(cmd, 0, 128);	
	sprintf(cmd, "tar -zxvf /usr/local/AOS/Upgrade/%s", filename);
	system(cmd);
	
	system("cp -f ./AOS/Bin/* /usr/local/AOS/Bin");
	if(chdir("/usr/local/AOS/Config")==-1){
		system("mkdir /usr/local/AOS/Config");
	}else{
		chdir("/tmp");
	}
	if(chdir("/usr/local/AOS/Config/Denypages")==-1){
		system("mkdir /usr/local/AOS/Config/Denypages");
	}else{
		chdir("/tmp");
	}
	if(chdir("/usr/local/AOS/Data")==-1){
		system("mkdir /usr/local/AOS/Data");
	}else{
		chdir("/tmp");
	}
	if(chdir("/usr/local/AOS/Data/Certificates")==-1){
		system("mkdir /usr/local/AOS/Data/Certificates");
	}else{
		chdir("/tmp");
	}
	if(chdir("/usr/local/AOS/www")==-1){
		system("mkdir /usr/local/AOS/www");
		system("mkdir /usr/local/AOS/www/html");
	}else{
		chdir("/tmp");
	}
	total=scandir("/boot", &dir_file, select_kernel_old, 0);
	if(total>0){
		for(i=0;i<total;i++){
			bzero(cmd, 128);
			sprintf(cmd, "rm -f /boot/%s", dir_file[i]->d_name);
			printf("%s\n", cmd);
			system(cmd);
		}
		free(*dir_file);
	}else{
		printf("no bak kernel\n");
	}

	total=scandir("/boot", &dir_file, select_initrd_old, 0);
	if(total>0){
		for(i=0;i<total;i++){
			bzero(cmd, 128);
			sprintf(cmd, "rm -f /boot/%s", dir_file[i]->d_name);
			printf("%s\n", cmd);
			system(cmd);
		}
		free(*dir_file);
	}else{
		printf("no bak initrd\n");
	}

	total=scandir("/boot", &dir_file, select_initrd, 0);
	if(total==1){
		printf("the oriange initrd is %s\n", (*dir_file)->d_name);
		memset(filename, 0, 128);
		strcpy(filename, (*dir_file)->d_name);
		free(*dir_file);
		memset(cmd, 0, 128);
		sprintf(cmd, "mv /boot/%s /boot/%s-bak", filename, filename);
		system(cmd);
	}else{
		printf("no oriange initrd\n");
	}
	

	total=scandir("/boot", &dir_file, select_kernel, 0);
	if(total==1){
		printf("the oriange kernel is %s\n", (*dir_file)->d_name);
		memset(filename, 0, 128);
		strcpy(filename, (*dir_file)->d_name);
		free(*dir_file);
	}else{
		printf("no oriange kernel\n");
	}
	
	memset(cmd, 0, 128);
	sprintf(cmd, "mv /boot/%s /boot/%s-bak", filename, filename);
	system(cmd);
	sscanf(filename, "vmlinuz-sslproxy-%d.%d.%d", &major_old, &minor_old, &min_old);
	printf("major_old=%d minor_old=%d min_old=%d\n");
	printf("filename is %s\n", filename);
	
	/*total=scandir("/boot", &dir_file, select_old_kernel, 0);
	if(total==1){
		printf("the old kernel is %s\n", (*dir_file)->d_name);
	}
	memset(cmd, 0, 128);
	sprintf(cmd, "rm -f %s", (*dir_file)->d_name);
	free(*dir_file);
	*/	
	total=scandir("/tmp/AOS/Kernel", &dir_file, select_kernel, 0);
	if(total==1){
		printf("filename in /tmp/AOS/Kernel is %s\n", (*dir_file)->d_name);
		memset(filename, 0, 128);
		strcpy(filename, (*dir_file)->d_name);
	}
	
	sscanf(filename, "vmlinuz-sslproxy-%d.%d.%d", &major, &minor, &min);
	printf("%d %d %d\n", major, minor, min);
	
	system("cp /tmp/AOS/Kernel/* /boot");
	fp=fopen("/boot/grub/grub.conf", "wt");
	if(fp==NULL){
		printf("open grub.conf error!\n");
		exit(1);
	}
	fprintf(fp, "default=0\n");
	fprintf(fp, "timeout=5\n");
	fprintf(fp, "splashimage=(hd0,0)/boot/grub/splash.xpm.gz\n");
	fprintf(fp, "hiddenmenu\n");
	fprintf(fp, "title AOSSSLPROXY (%d.%d.%d)\n", major, minor, min);
	fprintf(fp, "\troot (hd0,0)\n");
	fprintf(fp, "\tkernel /boot/%s ro root=LABEL=/ rhgb quiet\n", filename);
	fprintf(fp, "\tinitrd /boot/initrd-%d.%d.%d.img\n", major, minor, min);
	fprintf(fp, "title AOSSSLPROXY (%d.%d.%d) Second\n", major_old, minor_old, min_old);
	fprintf(fp, "\troot (hd0,0)\n");
	fprintf(fp, "\tkernel /boot/vmlinuz-sslproxy-%d.%d.%d-bak ro root=LABEL=/ rhgb quiet\n", major_old, minor_old, min_old);
	fprintf(fp, "\tinitrd /boot/initrd-%d.%d.%d-bak.img\n", major_old, minor_old, min_old);
	fclose(fp);

	
	system("cp -rf /tmp/AOS/GUI/html/* /usr/local/AOS/www/html");
	system("cp -f /tmp/AOS/GUI_startup_script/rc.local /etc/rc.d/");
	system("chmod +x /etc/rc.d/rc.local");
	system("cp -f /tmp/AOS/init_jmk_script/init_jmk /etc/init.d/");
	system("chmod +x /etc/rc.d/init.d/init_jmk");
	ret=remove("/etc/httpd/conf/httpd.conf");
	printf("ret == %d\n", ret);
	if(ret==-1){
		perror("remove error");
	}
	system("rm -f /etc/rc.d/rc3.d/S09init_jmk");
	system("ln -s /etc/rc.d/init.d/init_jmk /etc/rc.d/rc3.d/S09init_jmk");
	//system("cp -f /tmp/AOS/apache_conf/httpd.conf /etc/httpd/conf/");
	ret=copy("/tmp/AOS/apache_conf/httpd.conf", "/etc/httpd/conf/httpd.conf");
	printf("copy ret = %d\n", ret);
	system("cp -f /tmp/AOS/PHP_conf/php.ini /etc");
	
	system("chmod +x /usr/local/AOS/Bin/*");
	system("chmod +s /usr/local/AOS/Bin/mysudo");
	system("chmod 777 /usr/local/AOS/Bin/setxmlconfig");

	return 0;
}

	
	
		
