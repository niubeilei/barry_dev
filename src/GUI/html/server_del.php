<!--
**************************************************************************
                  《 PLW517服务器密码机管理系统Web站点》
                        版权所有 (C) 2005,2006
**************************************************************************
******************************************************
项目:《 PLW517服务器密码机管理系统Web站点》
模块:AppProxy代理服务删除处理页面
描述:
版本:1.0.1.0
日期:2005-12-22
作者:Rechard
更新:
	1.2005-12-24
        增加全部清空功能；由GET参数delall判断------
            =0，只删除当前条目；=1，全部删除
TODO:
*******************************************************
-->
<?php
	include ("constant.php");
	@session_start();
	if($manager_ID==NULL)
	{
  	echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	  echo '请先登陆！<p>';
	  echo '<a href="login.html">返回</a>';
    echo '</b></body></html>';
    exit();
	}
	
	if ($apptype == "forward")
		echo '<html><head><title>应用服务配置删除</title></head><body background="image/pmiback.gif"><center><font face="楷体_GB2312"><b>' ;
	else
		echo '<html><head><title>反向应用服务配置删除</title></head><body background="image/pmiback.gif"><center><font face="楷体_GB2312"><b>' ;

	$proxyservsarray=GetAppSvrList($apptype);
  $nr=count($proxyservsarray);
  $selproxyname=$proxyservsarray[$RadioGroupID-1][2];
  if ($delall==1)
  {    	  
      $ret=ModiServs($RadioGroupID,"",$apptype,"","","","","","","","",9);
      if ($ret)
      {
          echo "应用服务已成功全部删除！"."<br><br>";              
          //exec the Cli.exe commands -- add by ytao
          for($lineapp=0;$lineapp<$nr;$lineapp++)
          {
          		$appnametmp=$proxyservsarray[$lineapp][2];
          		//echo "Now del ".$appnametmp."<br>";
          	  $cmd=MYSUDO.MYCLI."\\\"app proxy set status $appnametmp off\\\"";
              //echo $cmd."<br>";
              exec($cmd,$a);                
              $cmd=MYSUDO.MYCLI."\\\"app proxy remove $appnametmp\\\"";
              //echo $cmd."<br>";
              exec($cmd,$a);            	
          }        
      } else
      {
          echo "应用服务删除没能成功，读写文件可能失败！"."<br>";
      }
  }
    if (($RadioGroupID<1) && ($delall==0))
    {
        echo "请选择一条应用服务后再删除！"."<br>";
    } else if($delall==0)
    {
        if (!isset($delpmi))
        {
            echo "<input type=\"hidden\" name=\"RadioGroupID\" value=$RadioGroupID><br>";
            echo "删除应用服务所关联的PMI访问规则吗？&nbsp;&nbsp;&nbsp;&nbsp;";
            echo "<a href=\"server_del.php?RadioGroupID=$RadioGroupID&delpmi=0&apptype=$apptype\">删除关联</a><br><br>";
            echo "或是<br><br>只删除应用服务，不删除PMI访问规则？&nbsp;&nbsp;";
            echo "<a href=\"server_del.php?RadioGroupID=$RadioGroupID&delpmi=1&apptype=$apptype\">不删除关联</a><br><br><br>";
        } else
        {
            if ($delpmi==0)
            {
                $ret=ModiServs($RadioGroupID,$selproxyname,"","","","","","","","","",2);
            } else
            {
                $ret=ModiServs($RadioGroupID,$selproxyname,"","","","","","","","","",0);
            }

            if ($ret)
            {
                echo "第 $RadioGroupID 条应用服务已成功删除！"."<br><br>";                
                
                //exec the Cli.exe commands -- add by ytao
                
                $cmd=MYSUDO.MYCLI."\\\"app proxy set status $selproxyname off\\\"";
                //echo $cmd."<br>";
                exec($cmd,$a);                
                $cmd=MYSUDO.MYCLI."\\\"app proxy remove $selproxyname\\\"";
                //echo $cmd."<br>";
                exec($cmd,$a);
                

            } else
            {
                echo "应用服务删除没能成功，读写文件可能失败！"."<br>";
            }
        }
    }
	echo "<p></b></font><a href=\"server.php?numb=&numb&apptype=$apptype\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
?>