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
日期:2006-1-8
作者:Stephen
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
	
	echo '<html><head><title>反向应用服务配置删除</title></head><body background="image/pmiback.gif"><center><font face="楷体_GB2312"><b>' ;
	$apptype = "backward";
	$proxyservsarray=GetAppSvrList($apptype);
  $nr=count($proxyservsarray);
  $selproxyname=$proxyservsarray[$RadioGroupID-1][2];
  if ($delall==1)
  {    	  
      $ret=ModiServs($RadioGroupID,"",$apptype,"","","","","","","","",9);
      if ($ret)
      {
          echo "反向应用服务已成功全部删除！"."<br><br>";              
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
          echo "反向应用服务删除没能成功，读写文件可能失败！"."<br>";
      }
  }
  if (($RadioGroupID<1) && ($delall==0))
  {
      echo "请选择一条反向应用服务后再删除！"."<br>";
  } 
  else if($delall==0)
  {
    $ret=ModiServs($RadioGroupID,$selproxyname,"","","","","","","","","",2);
    if ($ret)
    {
    	echo "第 $RadioGroupID 条反向应用服务已成功删除！"."<br><br>";                
      
      //exec the Cli.exe commands -- add by ytao
      
      $cmd=MYSUDO.MYCLI."\\\"app proxy set status $selproxyname off\\\"";
      //echo $cmd."<br>";
      exec($cmd,$a);                
      $cmd=MYSUDO.MYCLI."\\\"app proxy remove $selproxyname\\\"";
      //echo $cmd."<br>";
      exec($cmd,$a);
    } 
    else
       echo "反向应用服务删除没能成功，读写文件可能失败！"."<br>";
  }
	echo "<p></b></font><a href=\"revserver.php?numb=&numb&apptype=$apptype\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
?>