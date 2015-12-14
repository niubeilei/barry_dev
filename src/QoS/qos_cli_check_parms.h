#ifndef _QOS_CLI_CHECK_PARMS_H
#define _QOS_CLI_CHECK_PARMS_H
extern int is_status_valid(char *status,int *set,char *errmsg);
extern int is_direction_valid(char *direction,int *value,char *errmsg);

extern int is_valid_proto(char *proto_type,u8 *proto);
extern int is_valid_ip_with_mask_bit(char* raw,u32 *src_ip,u8 *mask_bit);
extern int is_valid_port_with_wild(int raw_port,u16 *port);
extern int is_valid_ip_address(int raw,u32 *src_ip);
extern int is_valid_port_without_wild(int raw, u16 *port);
extern int is_valid_past_time(char *raw,u16 *past_time);
extern int is_valid_interval(int raw,u16 *interval);
extern int is_valid_dev_name(char *dev_name,int *devid);
extern int is_valid_dev_direction(char *raw, u8 *direction);
extern int is_valid_priority(char *raw,u8 *priority);
extern int is_valid_app(char *app);
extern int is_valid_direction(char *ifsending,u8 *direction);
extern int is_valid_vlantag(u16 tag);
extern int is_valid_if(char *dev_name,int *devid);
extern int is_valid_proto(char *proto_type,u8 *proto);
extern int is_valid_mac_address(char* str,u8 *mac);
#endif
