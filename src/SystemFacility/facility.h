#ifndef SYSTEMFACILITY_FACILITY_H
#define SYSTEMFACILITY_FACILITY_H

#define BUFLEN 1024
struct ros_hardware_info
{
	char * version;
};

int ros_retrieve_system_id(char *data);
int ros_retrieve_motherboard_sn(char *data);
int ros_retrieve_hardware_info(struct ros_hardware_info *data);
int ros_retrieve_firmware_ver(char *version);
int ros_retrieve_sub_features(char **features);

#endif

