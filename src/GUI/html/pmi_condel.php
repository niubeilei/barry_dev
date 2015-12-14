<?php
	include ("constant.php");

//echo "opcode=".$opcode."<br>";
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
            echo '</b></body></html>';
            exit();
	}
	echo '<html><head><title>PMI访问控制配置规则删除</title></head><body background="image/pmiback.gif"><center><font face="楷体_GB2312"><b>' ;
    if ($RadioGroupID<1)
    {
        echo "请选择一条规则后再删除！"."<br>";
    } else
    {
        $pminames=GetPMIRules();
        $appname=$pminames[$RadioGroupID-1][1];
        $ret=ModiPMIRules($RadioGroupID,"","","","",0);
        if ($ret)
        {
            echo "第 $RadioGroupID 条规则已成功删除！"."<br>";
            //---------------20060101;by yt;增加“删除规则接口”；
            //exec the Cli.exe commands -- add by ytao 
            $cmd=MYSUDO.MYCLI."\\\"app proxy set pmi $appname off\\\"";
            //echo $cmd."<br>";
            exec($cmd,$a);
            //---------------20060101;by yt;增加“删除规则接口”；
        } else
        {
            echo "规则删除没能成功，读写文件失败！"."<br>";
        }
    }
    //echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";


	echo "<p></b></font><a href=\"pmi_control.php?numb=$numb\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
?>