# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = aos_core

KERNELHOME = /usr/src/linux

AOSCORE_DIR = $(KERNELHOME)/net/AosCore

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -Wall $(KERNEL_VERSION) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE

INCDIRS = -I../

OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = (if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../obj

LIBS = -lnsl 

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

TARGETLIB = lib$(LIBNAME).a

#
# Rules for normal comile and link 
#
all:: lib

lib: $(OBJECTS)
	ar rvu $(TARGETLIB) $^
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 


uninstall:
	rm -f $(AOSCORE_DIR)/Makefile
	rm -f $(AOSCORE_DIR)/TcpSockBridge.c
	rm -f $(AOSCORE_DIR)/aosKernelAlarm.c
	rm -f $(AOSCORE_DIR)/MgdObj.c
	rm -f $(AOSCORE_DIR)/aosBridge.c
	rm -f $(AOSCORE_DIR)/aosBuiltinRules.c
	rm -f $(AOSCORE_DIR)/aosConfig.c
	rm -f $(AOSCORE_DIR)/aosCoreComm.c
	rm -f $(AOSCORE_DIR)/aosCorePcp.c
	rm -f $(AOSCORE_DIR)/aosDev.c
	rm -f $(AOSCORE_DIR)/aosFwdDev.c
	rm -f $(AOSCORE_DIR)/aosFwder.c
	rm -f $(AOSCORE_DIR)/aosKernelApi.c
	rm -f $(AOSCORE_DIR)/aosMacCtlr.c
	rm -f $(AOSCORE_DIR)/aosMacRule.c
	rm -f $(AOSCORE_DIR)/aosMultiCaster.c
	rm -f $(AOSCORE_DIR)/aosPktStat.c
	rm -f $(AOSCORE_DIR)/aosRule.c
	rm -f $(AOSCORE_DIR)/aosRuleHeadSwapper.c
	rm -f $(AOSCORE_DIR)/aosRuleNetInput.c
	rm -f $(AOSCORE_DIR)/aosSkbRecv.c
	rm -f $(AOSCORE_DIR)/aosSock.c
	rm -f $(AOSCORE_DIR)/aosSocket.c
	rm -f $(AOSCORE_DIR)/aosSockSSL.c
	rm -f $(AOSCORE_DIR)/aosSockCtl.c
	rm -f $(AOSCORE_DIR)/aosTimer.c
	rm -f $(AOSCORE_DIR)/TcpSockBridge.c
	rm -f $(AOSCORE_DIR)/SockFirewall.c
	rm -f $(KERNELHOME)/net/ipv4/aosArp.c
	rm -f $(AOSCORE_DIR)/aosArp.c
	rm -f $(AOSCORE_DIR)/AppProxy.c
	rm -f $(AOSCORE_DIR)/CharPtree.c
	rm -f $(AOSCORE_DIR)/aosSockBridge.c
	rm -f $(AOSCORE_DIR)/KernelLock.c
	rm -f $(AOSCORE_DIR)/KernelStr.c
	rm -f $(AOSCORE_DIR)/Slab.c
	rm -f $(AOSCORE_DIR)/TArray.c
	rm -f $(AOSCORE_DIR)/AppServer.c
	rm -f $(AOSCORE_DIR)/ServerGroup.c
	rm -f $(AOSCORE_DIR)/AaaServer.c
	rm -f $(AOSCORE_DIR)/aostcpapi.c
	rm -f $(AOSCORD_DIR)/TestAosApi.c    
	rm -f $(AOSCORD_DIR)/xiaoqingapi.c    

CREATE_AOSCORE_DIR = $(shell if [ -d $(AOSCORE_DIR) ]; then echo ""; else mkdir $(AOSCORE_DIR); fi)

install:
	$(CREATE_AOSCORE_DIR)
	cp ../KernelMakefiles/Makefile-2.6.7 /usr/src/linux/Makefile
	cp ../KernelMakefiles/Kconfig-net-2.6.7 /usr/src/linux/net/Kconfig
	cp ../KernelMakefiles/Makefile-net-2.6.7 /usr/src/linux/net/Makefile
	ln -s -f $(AOSHOME)/src/aos_core/Makefile-AosCore $(AOSCORE_DIR)/Makefile
	ln -s -f $(AOSHOME)/src/aos_core/TcpSockBridge.cpp $(AOSCORE_DIR)/TcpSockBridge.c
	ln -s -f $(AOSHOME)/src/aos_core/aosBridge.cpp $(AOSCORE_DIR)/aosBridge.c
	ln -s -f $(AOSHOME)/src/aos_core/aosBuiltinRules.cpp $(AOSCORE_DIR)/aosBuiltinRules.c
	ln -s -f $(AOSHOME)/src/aos_core/aosConfig.cpp $(AOSCORE_DIR)/aosConfig.c
	ln -s -f $(AOSHOME)/src/aos_core/aosCoreComm.cpp $(AOSCORE_DIR)/aosCoreComm.c
	ln -s -f $(AOSHOME)/src/aos_core/aosCorePcp.cpp $(AOSCORE_DIR)/aosCorePcp.c
	ln -s -f $(AOSHOME)/src/aos_core/aosDev.cpp $(AOSCORE_DIR)/aosDev.c
	ln -s -f $(AOSHOME)/src/aos_core/aosSockCtl.cpp $(AOSCORE_DIR)/aosSockCtl.c
	ln -s -f $(AOSHOME)/src/aos_core/aosFwdDev.cpp $(AOSCORE_DIR)/aosFwdDev.c
	ln -s -f $(AOSHOME)/src/aos_core/aosFwder.cpp $(AOSCORE_DIR)/aosFwder.c
	ln -s -f $(AOSHOME)/src/aos_core/aosKernelApi.cpp $(AOSCORE_DIR)/aosKernelApi.c
	ln -s -f $(AOSHOME)/src/aos_core/aosMacCtlr.cpp $(AOSCORE_DIR)/aosMacCtlr.c
	ln -s -f $(AOSHOME)/src/aos_core/aosMacRule.cpp $(AOSCORE_DIR)/aosMacRule.c
	ln -s -f $(AOSHOME)/src/aos_core/aosMultiCaster.cpp $(AOSCORE_DIR)/aosMultiCaster.c
	ln -s -f $(AOSHOME)/src/aos_core/aosPktStat.cpp $(AOSCORE_DIR)/aosPktStat.c
	ln -s -f $(AOSHOME)/src/aos_core/aosRule.cpp $(AOSCORE_DIR)/aosRule.c
	ln -s -f $(AOSHOME)/src/aos_core/aosRuleHeadSwapper.cpp $(AOSCORE_DIR)/aosRuleHeadSwapper.c
	ln -s -f $(AOSHOME)/src/aos_core/aosRuleNetInput.cpp $(AOSCORE_DIR)/aosRuleNetInput.c
	ln -s -f $(AOSHOME)/src/aos_core/aosSkbRecv.cpp $(AOSCORE_DIR)/aosSkbRecv.c
	ln -s -f $(AOSHOME)/src/aos_core/aosSockBridge.cpp $(AOSCORE_DIR)/aosSockBridge.c
	ln -s -f $(AOSHOME)/src/aos_core/aosSock.cpp $(AOSCORE_DIR)/aosSock.c
	ln -s -f $(AOSHOME)/src/aos_core/aosSocket.cpp $(AOSCORE_DIR)/aosSocket.c
	ln -s -f $(AOSHOME)/src/aos_core/aosSockSSL.cpp $(AOSCORE_DIR)/aosSockSSL.c
	ln -s -f $(AOSHOME)/src/aos_core/aosTimer.cpp $(AOSCORE_DIR)/aosTimer.c
	ln -s -f $(AOSHOME)/src/aos_core/TcpSockBridge.cpp $(AOSCORE_DIR)/TcpSockBridge.c
	ln -s -f $(AOSHOME)/src/aos_core/SockFirewall.cpp $(AOSCORE_DIR)/SockFirewall.c
	ln -s -f $(AOSHOME)/src/aos_ipv4/aosArp.cpp $(AOSCORE_DIR)/aosArp.c
	ln -s -f $(AOSHOME)/src/KernelUtil/aosKernelAlarm.cpp $(AOSCORE_DIR)/aosKernelAlarm.c
	ln -s -f $(AOSHOME)/src/KernelUtil/MgdObj.cpp $(AOSCORE_DIR)/MgdObj.c
	ln -s -f $(AOSHOME)/src/KernelUtil/CharPtree.cpp $(AOSCORE_DIR)/CharPtree.c
	ln -s -f $(AOSHOME)/src/KernelUtil/KernelLock.cpp $(AOSCORE_DIR)/KernelLock.c
	ln -s -f $(AOSHOME)/src/KernelUtil/KernelStr.cpp $(AOSCORE_DIR)/KernelStr.c
	ln -s -f $(AOSHOME)/src/KernelUtil/Slab.cpp $(AOSCORE_DIR)/Slab.c
	ln -s -f $(AOSHOME)/src/KernelUtil/TArray.cpp $(AOSCORE_DIR)/TArray.c
	ln -s -f $(AOSHOME)/src/KernelUtil/aostcpapi.c $(AOSCORE_DIR)/aostcpapi.c
	ln -s -f $(AOSHOME)/src/KernelAPI/KernelWrapper/TestAosApi.c $(AOSCORE_DIR)/TestAosApi.c
	ln -s -f $(AOSHOME)/src/AppProxy/AppProxy.cpp $(AOSCORE_DIR)/AppProxy.c
	ln -s -f $(AOSHOME)/src/Servers/AppServer.cpp $(AOSCORE_DIR)/AppServer.c
	ln -s -f $(AOSHOME)/src/Servers/ServerGroup.cpp $(AOSCORE_DIR)/ServerGroup.c
	ln -s -f $(AOSHOME)/src/AAA/AaaServer.cpp $(AOSCORE_DIR)/AaaServer.c
	ln -s -f $(AOSHOME)/src/KernelAPI/KernelWrapper/xiaoqingapi.c $(AOSCORE_DIR)/xiaoqingapi.c
	ln -s -f $(AOSHOME)/src/AosAlarm/AosAlarm.cpp $(AOSCORE_DIR)/AosAlarm.c
	ln -s -f $(AOSHOME)/src/AppProxy/DenyPage.cpp $(AOSCORE_DIR)/DenyPage.c
	ln -s -f $(AOSHOME)/src/AppProxy/usbkey.cpp $(AOSCORE_DIR)/usbkey.c
