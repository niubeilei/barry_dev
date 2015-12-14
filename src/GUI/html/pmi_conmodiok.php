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
	echo '<html><head><title>PMI访问控制配置规则修改</title></head><body background="image/pmiback.gif"><center><font face="楷体_GB2312"><b>' ;
    if (VarIsValid($sysname,1))
    {
      if ($opcode==1)
      {
          if (!PMIRuleExists($appselect,0,3))
          {
              $ret=AddPMIRules($appselect,$orgname,$sysname,$swselect);
              if ($ret)
              {
                  echo "规则已添加成功！"."<br>";
                  
                  //---------------20060101;by yt;增加“添加PMI规则”；
                  //exec the Cli.exe commands -- add by ytao
                  $cmd=MYSUDO.MYCLI."\\\"app proxy pmi sysname $appselect $sysname\\\"";
                  //echo $cmd."<br>";
                  exec($cmd,$a);
                  
                  //if (($swselect=="on") or ($swselect=="ON"))
                  //{
                  	$cmd=MYSUDO.MYCLI."\\\"app proxy set pmi $appselect $swselect\\\"";
                  	//echo $cmd."<br>";
                  	exec($cmd,$a);
                  //}

                  //---------------20060101;by yt;增加“添加PMI规则”；
              } else
              {
                  echo "规则添加没能成功，读写文件可能失败！"."<br>";
              }
          } else
          {
              echo "应用服务名称为<font color=\"#0000FF\"> $appselect </font>的PMI规则已经存在，";
              echo "<br>请重新选择应用服务名称！"."<br>";
          }
      } else if($opcode==2)
      {
          if (!PMIRuleExists($appselect,$RuleID,3))
          {
              //echo "$appselect,$sysname,$orgname,$swselect";
              $ret=ModiPMIRules($RuleID,$appselect,$orgname,$sysname,$swselect,1);
              if ($ret)
              {
              	 
                  echo "第 $RuleID 条规则已修改成功！"."<br>";
                  //---------------20060101;by yt;增加“修改PMI规则”；
                  //exec the Cli.exe commands -- add by ytao
                  $cmd=MYSUDO.MYCLI."\\\"app proxy pmi sysname $appselect $sysname\\\"";
                  //echo $cmd."<br>";
                  exec($cmd,$a);
                  
                  //if (($swselect=="on") or ($swselect=="ON"))
                  //{
                  	$cmd=MYSUDO.MYCLI."\\\"app proxy set pmi $appselect $swselect\\\"";
                  	//echo $cmd."<br>";
                  	exec($cmd,$a);
                  //}                  
                  
                  //---------------20060101;by yt;增加“修改PMI规则”；
              } else
              {
                  echo "规则修改没能成功，读写文件可能失败！"."<br>";
              }
          } else
          {
              echo "应用服务名称为<font color=\"#0000FF\"> $appselect </font>与其他已经存在PMI规则发生冲突，";
              echo "<br>请重新选择应用服务名称！"."<br>";
          }
      }
    }
    else
    {
      echo "应用系统名称不允许空格存在，请修改后再操作！";
    }
    //echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";


	echo "<p></b></font><a href=\"pmi_control.php?numb=$numb\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
?>