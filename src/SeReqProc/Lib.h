# ifndef LIB_H
# define LIB_H
/*
函数功能:	对指定的数据源文件进行解包，并对各类数据解完包后存储到对应文件
			此函数供外部调用接口
输入:	SorPath:	源数据文件的绝对路径		如:char sor[] = "D:\\HM\\DLL\\data\\data.ecg";
		DesPath:	目标数据文件存放的绝对路径	如:char des[] = "D:\\HM\\DLL\\data";
输出:	6500.ecg:	存储心电数据值
		6500.pres:	存储血压数据
		6500.resp:	存储呼吸率数据
		6500.tmp:	存储体温的值
		6500.rr:	存储血氧饱和度数据
返回值: -1:文件不存在或路径有误
		 1:数据转换成功
*/

#ifdef __cplusplus
extern "C"
{
#endif

extern int UnPackFile(char* SorPath, char* DesPath);

#ifdef __cplusplus
}
#endif

# endif
