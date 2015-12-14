start_time=`date`
libdir=../WebSite/WEB-INF/lib
jimovpd_dir=../vpd/jimo_vpd
systemvpd_dir=../vpd/systemvpd
if [ -f ImportData.class ] ; then
	java -classpath ${libdir}/socketding.jar:${libdir}/commons-lang-2.4.jar:. ImportData 127.0.0.1 2000 ${systemvpd_dir}
	java -classpath ${libdir}/socketding.jar:${libdir}/commons-lang-2.4.jar:. ImportData 127.0.0.1 2000 ${jimovpd_dir}
else
	javac ImportData.java -classpath ${libdir}/socketding.jar
	java -classpath ${libdir}/socketding.jar:${libdir}/commons-lang-2.4.jar:. ImportData 127.0.0.1 2000 ${systemvpd_dir}
	java -classpath ${libdir}/socketding.jar:${libdir}/commons-lang-2.4.jar:. ImportData 127.0.0.1 2000 ${jimovpd_dir}
fi


echo "---------------------------------"
echo "start time: $start_time" >> time.log
echo "end time: `date`" >> time.log
