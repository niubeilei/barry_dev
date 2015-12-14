#!/bin/sh
WORK_DIR=~/tmp
MY_PATCH_O_MATIC=patch-o-matic-ng-20061223
MY_KERN=linux-2.6.15.5
MY_MADWIFI=madwifi-0.9.2.1
MY_IPTABLES=iptables-1.3.5
MY_TIME=time
MY_RHC=mystro
RHC_SOURCE=prodea
if [ ! -d $WORK_DIR ]; then
mkdir -p ~/tmp
fi
if [ ! -d $WORK_DIR/kernel ]; then
mkdir -p $WORK_DIR/kernel
fi
export KERNEL_PATCH_DIR=${WORK_DIR}/kernel 
export KERNEL_DIR=${WORK_DIR}/kernel/${MY_KERN} 
export KERNELHOME=${KERNEL_DIR} 
RHC_DIR=${WORK_DIR}/rhc 
NJOBS=2 

chmod 755 ../prodea/copy-dist
cd  ../prodea/
./copy-dist
# check rhc source code is not exist!
if [ ! -d $WORK_DIR\/$RHC_SOURCE ]; then
echo "===================================================================================
The vesion is not correct, please check it!
==================================================================================="
exit
fi
echo "===================================================================================
Do you want download all kernel source code(yes/no)?
==================================================================================="
read ANSWER
cd $KERNEL_PATCH_DIR
if [ "$ANSWER" = yes -o "$ANSWER" = YES ] ; then
wget ftp://172.22.0.1/pub/kernels/linux-2.6.15.5.tar.bz2
wget ftp://172.22.0.1/pub/kernel-patch/madwifi-0.9.2.1.tar.bz2
wget ftp://172.22.0.1/pub/kernel-patch/iptables-1.3.5.tar.bz2
wget ftp://172.22.0.1/pub/kernel-patch/patch-o-matic-ng-20061223.tar.bz2
wget ftp://172.22.0.1/pub/kernel-patch/time.tar.gz
fi
bzip_list=" 
${MY_IPTABLES} 
${MY_KERN} 
${MY_PATCH_O_MATIC} 
${MY_MADWIFI} 
" 
zip_list=" 
${MY_TIME} 
" 
if [ ! -d $KERNEL_PATCH_DIR ]; then 
echo " 
============================================================== 
Error, patch directory not found $KERNEL_PATCH_DIR 
Have you checked out the kernel and kernel patches 
============================================================== 
" 
exit 1 
fi 

echo " 
============================================================== 
WORKDIR          $WORK_DIR 
KERNEL_PATCH_DIR $KERNEL_PATCH_DIR 
KERNEL_DIR       $KERNEL_DIR 
KERNELHOME       $KERNELHOME 
PATCH_O_MATIC    $MY_PATCH_O_MATIC 
RHC_DIR          $RHC_DIR 
NJOBS            $NJOBS 
============================================================== 
" 


pushd . 

echo " ============================================================== 
Untarring the patches and sources if they do not exist  
============================================================== " 
rm -rf $KERNEL_DIR 
cd ${KERNEL_PATCH_DIR} 

for i in ${bzip_list}; do 
if [ ! -d $i ]; then 
echo " ============================================================== 
Untarring ${i}.tar.bz2 
============================================================== " 
tar xjf ${i}.tar.bz2 
if [ $? -ne 0 ]; then 
echo " ============================================================== 
Error while untarring ${i}.tar.bz2 
============================================================== " 
exit 1 
fi 
else 
echo " ============================================================== 
File ${i}.tar.bz2 already untarred 
============================================================== " 
fi 
done 

for i in ${zip_list}; do 
if [ ! -d $i ]; then 
echo " ============================================================== 
Untarring ${i}.tar.gz 
============================================================== " 
tar xzf ${i}.tar.gz 
if [ $? -ne 0 ]; then 
echo " 
============================================================== 
Error while untarring ${i}.tar.gz 
============================================================== 
" 
exit 1 
fi 
echo " ============================================================== 
File ${i}.tar.gz already untarred 
============================================================== " 
fi 
done 

if [ ! -d $KERNEL_DIR ]; then 
echo " 
============================================================== 
Error, directory not found $KERNEL_DIR 
You need to checkout the kernel source into  
$KERNEL_DIR 
============================================================== 
" 
exit 1 
fi 

echo " 
============================================================== 
Patching the kernel 
============================================================== 
" 
cd ${KERNEL_PATCH_DIR} 
echo " 
============================================================== 
Copying time patch  
============================================================== 
" 
# Copy the time patch in if it does not exist 
if [ -d ${MY_PATCH_O_MATIC} ]; then 
cp -rp time ${MY_PATCH_O_MATIC}/patchlets/ 
else 
echo "Error dir ${MY_PATCH_O_MATIC} not found" 
exit 1 
fi 

# Patch kernel with madwifi: 
echo " 
============================================================== 
Applying madwifi patch  
============================================================== 
" 
cd madwifi-0.9.2.1/patches/ 
./install.sh ${KERNEL_DIR} 
cd ${KERNEL_PATCH_DIR} 

# Patch iptable: 
cd ${KERNEL_PATCH_DIR}/${MY_PATCH_O_MATIC} 
echo " 
============================================================== 
Applying iptables time patch  
============================================================== 
" 
IPTABLES_DIR=${KERNEL_PATCH_DIR}/${MY_IPTABLES} ./runme --batch time  
IPTABLES_DIR=${KERNEL_PATCH_DIR}/${MY_IPTABLES} ./runme --batch set 
cd ${KERNEL_PATCH_DIR} 

#When a question appears, select 'y' 

# Install the kernel changes cd ${WORKDIR}/mystro/src/Install/ 
cd ${WORK_DIR}/prodea/src/Install/ 
LINUXHOME=${KERNEL_DIR}  make installRhc
echo ${WORK_DIR}/prodea/src/Install/
# Clean up the installation directory if it exists 
echo " 
============================================================== 
Create install tree 
============================================================== 
"
cd ${WORK_DIR}
#del the last rhc dir
[ -d ${RHC_DIR} ] && rm -rf ${RHC_DIR}

# Create our install tree 
cdirlist=" 
${RHC_DIR}/bin 
${RHC_DIR}/lib 
${RHC_DIR}/boot 
${RHC_DIR}/rhcConf 
${RHC_DIR}/aos_secure 
" 

for i in ${cdirlist}; do 
if [ ! -d $i ]; then 
echo " 
============================================================= 
Creating directory $i 
============================================================== 
" 

mkdir -p $i 
	if [ $? -ne 0 ]; then 
	echo " 
	============================================================= 
	Creation of $i failed 
	=============================================================" 
	exit 1 
	fi 
fi 
done 

# Compile the kernel 
echo " 
============================================================== 
Compiling the kernel 
============================================================== 
" 
cd $KERNEL_DIR 
echo " 
============================================================== 
Compiling the bzImage 
============================================================== 
" 
AOSHOME=$WORK_DIR/prodea make  bzImage
echo " 
============================================================== 
Compiling the modules 
============================================================== 
"
AOSHOME=$WORK_DIR/prodea make  modules 
AOSHOME=$WORK_DIR/prodea INSTALL_MOD_PATH=${RHC_DIR}/boot/ make modules_install

# Compile the rest of the source
# add version
echo "
==============================================================
Add version, please input Version(eg: 1_0_3)
=============================================================="
read VER
sed -i "20c #define VERSION \"$VER\" " ${WORK_DIR}/${RHC_SOURCE}/src/*exe/version.h
echo "
==============================================================
Add version, please input buildnumber(eg: 3)
=============================================================="
read BUILDNUMBER
sed -i "21c #define BUILDNUMBER \"$BUILDNUMBER\" " ${WORK_DIR}/${RHC_SOURCE}/src/*exe/version.h 
echo "
==============================================================
Compiling the rest of the source
==============================================================
"

cd ${WORK_DIR}/prodea
compile_src(){
while read line; do
if [ -z "${line/##*/}" ] || [ -z "$line" ]; then
continue
fi
echo "
=============================================================
Compiling $line
=============================================================="
#make -j ${NJOBS} -C $i
make -C $line
if [ $? -ne 0 ]; then
echo "
=============================================================
Compile of $line failed
============================================================="
#exit 1
fi
done
}

compile_src<<EOF
src
src/am
src/KernelInterface
src/KernelInterface_exe
src/CliUtil
src/rhcUtil/
src/CommandMan
src/ParentCtrl
src/KernelConfMgr clean
src/KernelConfMgr
EOF


for i in ${dirlist}; do
echo "
=============================================================
Compiling $i
==============================================================
"
#make -j ${NJOBS} -C $i
make -C $i
if [ $? -ne 0 ]; then
echo "
=============================================================
Compile of $i failed
=============================================================
"
exit 1
fi
done

# Install the source
echo "
==============================================================
Installing the compiled source
==============================================================
"
process_commands(){
	while read line; do
		if [ -z "${line/##*/}" ] || [ -z "$line" ]; then
			continue
		fi
		echo $line
		$line
		if [ $? -ne 0 ]; then
		echo "Error, Failed: $line"
		exit 1
		fi
		done
}

# This routine will run the following commands and set an
# error code if it fails
process_commands<<EOF
cp src/KernelInterface_exe/Cli.exe ${RHC_DIR}/bin
cp src/KernelConfMgr/KernelConfMgr ${RHC_DIR}/bin
cp src/KernelConfMgr/config.txt ${RHC_DIR}/bin
cp src/tools/rhcinit ${RHC_DIR}/bin
cp src/am/am.conf ${RHC_DIR}/bin
cp src/am/am ${RHC_DIR}/bin
cp -a  ThirdPartyLibs/* ${RHC_DIR}/lib/
cp -a  src/rhcConf/*.template ${RHC_DIR}/rhcConf/
cp -a  src/aos_secure/securecmd.db ${RHC_DIR}/aos_secure/
cp -a  Data/cmd.txt ${RHC_DIR}/bin/
cp -a  ${KERNEL_DIR}/arch/i386/boot/bzImage ${RHC_DIR}/boot/rhcImage
mv  ${RHC_DIR}/boot/lib/modules/2.6.15.5rhc ${RHC_DIR}/boot/2.6.15.5rhc
rm -rf ${RHC_DIR}/boot/lib/
#copy webwall binary
cp src/ParentCtrl/install/webwall.tar.gz    ${RHC_DIR}/bin/

EOF

echo "==============================================================
Tarring up distribution
=============================================================="
pushd .
VERSION=`${RHC_DIR}/bin/Cli.exe -r`
cd ${WORK_DIR}
tar zcf rel_rhc_china$VERSION.tar.gz rhc
popd

echo "
==============================================================
Finished compile in $SECONDS
==============================================================
"
cp $AOSHOME/src/Install/install-rhc.sh $WORK_DIR/
LINES=`cat $WORK_DIR/install-rhc.sh|wc -l`
LINES=`expr 3 + $LINES`
echo '#!/bin/bash' > $WORK_DIR/install-rhc-china-$VERSION.bin
echo "LINES=$LINES" >>$WORK_DIR/install-rhc-china-$VERSION.bin
cat $WORK_DIR/install-rhc.sh >> $WORK_DIR/install-rhc-china-$VERSION.bin
cat $WORK_DIR/rel_rhc_china$VERSION.tar.gz >>$WORK_DIR/install-rhc-china-$VERSION.bin
chmod 755 $WORK_DIR/install-rhc-china-$VERSION.bin
echo "==================================================================
SUCCESS! The install file is $WORK_DIR/install-rhc-china-$VERSION.bin
To install, please run: sudo $WORK_DIR/install-rhc-china-$VERSION.bin
=================================================================="

exit
