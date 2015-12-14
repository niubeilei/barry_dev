<?php
	include ("constant.php");
    	//header('Cache-control: private, must-revalidate');
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
	echo '<html><head>';
    echo '<title>Denypage管理删除</title></head><body background="image/pmiback.gif">';
    echo '<form name="DenypageDel" method="post"><center><font face="楷体_GB2312"><b>';
    //echo '<table><tr><td>';
    if ($RadioGroupID<1)
    {
        echo "请选择一条Denypage后再删除！"."<br>";
    } else
    {
        if (!isset($deldp))
        {
            //echo "<input type=\"hidden\" name=\"RadioGroupID\" value=$RadioGroupID><br>";
            //echo "<input type=\"hidden\" name=\"numb\" value=$numb><br>";
            //echo "<input type=\"hidden\" name=\"denypname\" value=$denypname><br>";
            echo "删除该Denypage所关联的应用服务的Denypage项吗？&nbsp;&nbsp;&nbsp;&nbsp;";
            echo "<a href=\"denypage_del.php?RadioGroupID=$RadioGroupID&numb=$numb&deldp=0\">删除关联</a><br><br>";
            echo "或是<br><br>只删除该Denypage，不删除应用服务对应Denypage项？&nbsp;&nbsp;";
            echo "<a href=\"denypage_del.php?RadioGroupID=$RadioGroupID&numb=$numb&deldp=1\">不删除关联</a><br><br><br>";
        } else
        {   
        		$denynames=GetDenysList();
	    			$denypname=$denynames[$RadioGroupID-1];
	    			
            $denytable=GetDenypagesList();
	    			$denyfile=DENYPAGEFILEPATH.$denytable[$RadioGroupID-1][2];
	    			//echo $denyfile."<br>";
	    			
            if ($deldp==0)
            {
                $ret=ModiDenyPages($RadioGroupID,"","",2);
                /*
                if ($ret)
                {
                    $ret=ModiServsPara($denypname,"-",0);
                    echo $denypname;
                }
                */
            } else
            {
                $ret=ModiDenyPages($RadioGroupID,"","",0);
            }
            
            if ($ret)
            {
                echo "第 $RadioGroupID 条应用服务已成功删除！"."<br><br>";  
                //---------------20060101;by yt;增加“删除Denypage规则接口”；
                //exec the Cli.exe commands -- add by ytao
                $cmd=MYSUDO.MYCLI."\\\"deny page remove $denypname\\\"";
                 //echo $cmd."<br>";
                 exec($cmd,$a);  
                $cmd=MYSUDO."rm -f $denyfile";
                 //echo $cmd."<br>";
                 exec($cmd,$a);   
                 //---------------20060101;by yt;增加“删除Denypage规则接口”；            

            } else
            {
                echo "应用服务删除没能成功，读写文件可能失败！"."<br>";
            }
        }
    }
    //echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";

    //echo '</td></tr></table>';
	echo "<p></b></font><a href=\"denypage.php?numb=$numb\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></form></body></html>";
?>