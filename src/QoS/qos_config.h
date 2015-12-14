#ifndef  _QOS_CONFIG_H
#define  _QOS_CONFIG_H

#define QOS_TRAFFIC_STATUS_BUFF_SIZE  8
#define QOS_TRAFFIC_PROTO_BUFF_SIZE   8
#define QOS_TRAFFIC_PRI_BUFF_SIZE   8
#define QOS_TRAFFIC_DIRECTION_BUFF_SIZE   16

#define QOS_CONFIG_RET_SIZE          1024 
extern int aos_qos_traffic_facility_status;
extern int AosQos_initConfModule(void);
extern void AosQos_exitConfModule(void);

#endif
