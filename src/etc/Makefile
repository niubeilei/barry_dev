
#//
#// Copyright (C) 2005
#// Packet Engineering, Inc. All rights reserved.
#//
#// Redistribution and use in source and binary forms, with or without
#// modification is not permitted unless authorized in writing by a duly
#// appointed officer of Packet Engineering, Inc. or its derivatives
#//
#// Description:
#//
#// Modification History:
#// 2014/10/19: modified by Phil
#//
#////////////////////////////////////////////////////////////////////////////
INSTARGET = run.sh importData.sh ImportData.java 
INSTARGET_VPD = ../../vpd/jimoIDE
INSTARGET_LIB = ../../obj/*
INSTARGET_SYSDOC = ../SvrProxyExe/systemDocs/*

INSDIR = ../../install
INSDIR_VPD = ${INSDIR}/vpd
INSDIR_LIB = ${INSDIR}/obj
INSDIR_SYSDOC = ${INSDIR}/systemDocs

#
# Rules for normal comile and link 
#
all:: install

install: 
	mkdir -p ../../install;
	@for i in $(INSTARGET) ;do cp -rf $$i $(INSDIR)/.; done
	rm -rf $(INSDIR_VPD); mkdir -p $(INSDIR_VPD)
	@for i in $(INSTARGET_VPD) ;do cp -rf $$i $(INSDIR_VPD)/.; done
	rm -rf $(INSDIR_LIB); mkdir -p $(INSDIR_LIB)
	@for i in $(INSTARGET_LIB) ;do cp -rf $$i $(INSDIR_LIB)/.; done
	rm -rf $(INSDIR_SYSDOC); mkdir -p $(INSDIR_SYSDOC)
	@for i in $(INSTARGET_SYSDOC) ;do cp -rf $$i $(INSDIR_SYSDOC)/.; done
