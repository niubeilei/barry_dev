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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define MAX_LEN 128
#define BAK_TAR "/usr/local/AOS/Upgrade/bak/sslproxy-rel-bak.tar"
#define UPGRADE_TAR "/usr/local/AOS/Upgrade/upgrade/sslproxy-rel.tar"
#define CURR_TAR "/usr/local/AOS/Upgrade/curr/sslproxy-rel.tar"


struct kernel_ver{
	int major;
	int minor;
	int min;
};

int select_kernel(const struct dirent *dir){
	if(memcmp(dir->d_name, "vmlinuz-sslproxy", strlen("vmlinuz-sslproxy"))==0){
		if(strstr(dir->d_name, "bak")==NULL){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

int select_kernel_old(const struct dirent *dir){
	if(memcmp(dir->d_name, "vmlinuz-sslproxy", strlen("vmlinuz-sslproxy"))==0){
		if(strstr(dir->d_name, "bak")!=NULL){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

int select_initrd_old(const struct dirent *dir){
	if(memcmp(dir->d_name, "initrd-sslproxy", strlen("initrd-sslproxy"))==0){
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
	if(memcmp(dir->d_name, "initrd-sslproxy", strlen("initrd-sslproxy"))==0){
		if(strstr(dir->d_name, "bak")==NULL){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

int get_kernel_version(char *filename, struct kernel_ver k_version){
	sscanf(filename, "vmlinuz-sslproxy-%d.%d.%d", &k_version.major, &k_version.minor, &k_version.min);
	return 0;
}

int rename_initrd_to_bak(char *filename){
	char *point;

	point=strstr(filename, ".img");
	memcpy(point, "-bak", 4);
	memcpy(point+4, ".img", 4);

	return 0;
}

int main(int argc, char *argv[]){
	char filename[MAX_LEN];
	char cmd[MAX_LEN];
	int total;
	int update_flag=0;
	struct dirent **dir_file;
	struct kernel_ver new_ver, old_ver;
	int i;
	FILE *fp;
	int resume_flag=0;
	int ret;

	if(argc>=2){
		bzero(filename, MAX_LEN);
		strcpy(filename, BAK_TAR);
		resume_flag=1;
	}else{
		bzero(filename, MAX_LEN);
		strcpy(filename, UPGRADE_TAR);
		resume_flag=0;
	}

	ret=chdir("/tmp");
	if(ret==-1){
		fprintf(stderr, "no /tmp directory\n");
		exit(1);
	}
	bzero(cmd, MAX_LEN);
	sprintf(cmd, "tar -zxf %s ./", filename);
	system(cmd);

	total=scandir("./AOS/Kerenl", &dir_file, select_kernel, 0);
	if(total==1){
		update_flag=1;		
		bzero(filename, MAX_LEN);
		strcpy(filename, (*dir_file)->d_name);
		free(*dir_file);
	}
	
	if(update_flag){
		get_kernel_version(filename, new_ver);
		total=scandir("/boot", &dir_file, select_kernel, 0);
		if(total==1){
			get_kernel_version((*dir_file)->d_name, old_ver);
			bzero(cmd, MAX_LEN);

			system(cmd);
			free(*dir_file);
		}
		total=scandir("/boot", &dir_file, select_kernel_old, 0);
		if(total>0){
			for(i=0;i<total;i++){
				bzero(cmd, MAX_LEN);
				sprintf(cmd, "rm -f /boot/%s", dir_file[i]->d_name);
				system(cmd);
			}
			free(*dir_file);
		}
		total=scandir("/boot", &dir_file, select_initrd_old, 0);
		if(total>0){
			for(i=0;i<total;i++){
				bzero(cmd, MAX_LEN);
				sprintf(cmd, "rm -f /boot/%s", dir_file[i]->d_name);
				system(cmd);
			}
			free(*dir_file);
		}
		total=scandir("/boot", &dir_file, select_initrd, 0);
		if(total==1){
			bzero(cmd, MAX_LEN);
			bzero(filename, MAX_LEN);
			strcpy(filename, (*dir_file)->d_name);
			rename_initrd_to_bak(filename);
			sprintf(cmd, "mv -f %s %s", (*dir_file)->d_name, filename);
			system(cmd);
			free(*dir_file);			
		}
		bzero(cmd, MAX_LEN);
		sprintf(cmd, "cp -f /tmp/AOS/Kernel/* /boot");
		system(cmd);
		
		fp=fopen("/boot/grub/grub.conf", "wt");
		if(fp==NULL){
			fprintf(stderr, "open grub.conf error!\n");
			exit(1);
		}
		fprintf(fp, "default=0\n");
		fprintf(fp, "timeout=5\n");
		fprintf(fp, "splashimage=(hd0,0)/boot/grub/splash.xpm.gz\n");
		fprintf(fp, "hiddenmenu\n");
		fprintf(fp, "title AOSSSLPROXY (%d.%d.%d)\n", new_ver.major, new_ver.minor, new_ver.min);
		fprintf(fp, "\troot (hd0,0)\n");
		fprintf(fp, "\tkernel /boot/vmlinuz-sslproxy-%d.%d.%d ro root=LABEL=/ rhgb quiet\n", new_ver.major, new_ver.minor, new_ver.min);
		fprintf(fp, "\tinitrd /boot/initrd-%d.%d.%d.img\n", new_ver.major, new_ver.minor, new_ver.min);
		fprintf(fp, "title AOSSSLPROXY (%d.%d.%d) Second\n", old_ver.major, old_ver.minor, old_ver.min);
		fprintf(fp, "\troot (hd0,0)\n");
		fprintf(fp, "\tkernel /boot/vmlinuz-sslproxy-%d.%d.%d-bak ro root=LABEL=/ rhgb quiet\n", old_ver.major, old_ver.minor, old_ver.min);
		fprintf(fp, "\tinitrd /boot/initrd-%d.%d.%d-bak.img\n", old_ver.major, old_ver.minor, old_ver.min);
		fclose(fp);		
	}
	if(chdir("/usr/local/AOS/Data")==-1){
		system("mkdir /usr/local/AOS/Data");
	}
	if(chdir("/usr/local/AOS/Data/Certificates")==-1){
		system("mkdir /usr/local/AOS/Data/Certificates");
	}
	if(chdir("/usr/local/AOS/Config")==-1){
		system("mkdir /usr/local/AOS/Config");
	}
	if(chdir("/usr/local/AOS/www")==-1){
		system("mkdir /usr/local/AOS/www");
	}
	if(chdir("/usr/local/AOS/Config/Denypages")==-1){
		system("mkdir /usr/local/AOS/Config/Denypages");
	}
	if(chdir("/usr/local/AOS/www/html")==-1){
		system("mkdir /usr/local/AOS/www/html");
	}
	chdir("/tmp");

	system("cp -f /tmp/AOS/Bin/* /usr/local/AOS/Bin");
	system("chmod +x /usr/local/AOS/Bin/*");
	system("chmod +s /usr/local/AOS/Bin/mysudo");
	system("chmod 777 /usr/local/AOS/Bin/setxmlconfig");
	system("cp -f /tmp/AOS/GUI_startup_script/rc.local /etc/rc.d/rc.local");
	system("chmod +x /etc/rc.d/rc.local");
	system("cp -f /tmp/AOS/apache_conf/httpd.conf /etc/httpd/conf/httpd.conf");
	system("cp -f /tmp/AOS/PHP_conf/php.ini /etc/php.ini");
	system("cp -f /tmp/AOS/init_jmk_script/init_jmk /etc/rc.d/init.d/init_jmk");
	system("chmod +x /etc/rc.d/init.d/init_jmk");
	system("rm -f /etc/rc.d/rc3.d/S09init_jmk");
	system("ln -s /etc/rc.d/rc3.d/S09init_jmk /etc/rc.d/init.d/init_jmk");
	
	if(chdir("/usr/local/AOS/Upgrade/curr")==-1){
		system("mkdir /usr/local/AOS/Upgrade/curr");
	}
	if(chdir("/usr/local/AOS/Upgrade/bak")==-1){
		system("mkdir /usr/local/AOS/Upgrade/bak");
	}

	if(resume_flag){
		system("mv -f /usr/local/AOS/Upgrade/bak/sslproxy-rel-bak.tar /usr/local/AOS/Upgrade/upgrade/sslproxy-rel.tar");
		system("mv -f /usr/local/AOS/Upgrade/curr/sslproxy-rel.tar /usr/local/AOS/Upgrade/curr/sslproxy-rel-bak.tar");
		system("mv -f /usr/local/AOS/Upgrade/upgrade/sslproxy-rel.tar /usr/local/AOS/Upgrade/curr/sslproxy-rel.tar");
		system("rm -f /usr/local/AOS/Upgrade/upgrade/sslproxy-rel.tar");
	}else{
		system("rm -f /usr/local/AOS/Upgrade/bak/sslproxy-rel-bak.tar");
		system("mv -f /usr/local/AOS/Upgrade/curr/sslproxy-rel.tar /usr/local/AOS/Upgrade/bak/sslproxy-rel-bak.tar");
		system("mv -f /usr/local/AOS/Upgrage/upgrade/sslproxy-rel.tar /usr/local/AOS/Upgrade/curr/sslproxy-rel.tar");
	}



	return 0;
}
