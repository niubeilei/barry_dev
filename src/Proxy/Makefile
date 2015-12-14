#
# Makefile for the Linux X.25 Packet layer.
#

obj-$(CONFIG_AOS_PROXY) += proxy.o
proxy-y			:= proxy_main.o proxy_proc.o proxy_cli.o proxy_app.o proxy_service.o proxy_wrapper.o proxy_func.o proxy_scheduler.o
obj-$(CONFIG_AOS_PROXY_SCHEDULER_RR) += sched_rr.o
obj-$(CONFIG_AOS_PROXY_SCHEDULER_IP) += sched_ip.o
obj-$(CONFIG_AOS_PROXY_SCHEDULER_WRR) += sched_wrr.o
obj-$(CONFIG_AOS_PROXY_DUMMY) += app_dummy.o
obj-$(CONFIG_AOS_PROXY_HTTP) += app_http.o
obj-$(CONFIG_AOS_PROXY_FTP) += app_ftp.o
obj-$(CONFIG_AOS_PROXY_PLAIN_SW) += wrapper_plain.o 
obj-$(CONFIG_AOS_PROXY_SSL_SW) += wrapper_ssl.o 

