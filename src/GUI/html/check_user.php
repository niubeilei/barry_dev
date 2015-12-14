<?php
include "pmi_db.php";
@session_start();
session_register("manager_ID");
$manager_ID=$managername;
    //----------------20051201;by yy;用户注册方式由数据库改为文件读取
    //----------------20051213;by yy;新增用户文件注册方式；
    $filename = MANAGERFILE;

    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/
    	exec($cmd);
    	
        $handle = @fopen($filename, "r");
        $lginOK=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($user, $pass)=split(" ",$buffer,10);
            if (strcmp($managername,$user)==0 && strcmp($password,$pass)==0)
            {
                //session_register("managername");
                //session_unregister("$password");
                echo '<html>';
	     		echo '<head>';
	     		echo '<meta http-equiv="refresh" content="1;url=manager.php">';
	     		echo '</head>';
	     		echo '<body>';
		    	echo '</body>';
		     	echo '</html>';
                $lginOK=1;
                session_register("manager_ID");
                $manager_ID=$managername;
                //$manager_ID=$managername;
                break;
            }
            
          }
          fclose($handle);
          
    	//del write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = DELOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/    	
    	exec($cmd); 
    	       
          if ($lginOK == 0)
          {
              echo '<html><head><title>错误信息</title></head><body  background="image/pmiback.gif">';
              echo '<p><br><br><center><font face="楷体_GB2312"><b>管理员用户名或密码错误!<br>请检查后重新输入！</b></font>';
     		  echo '<p>';
              echo "<a href='login.html'><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>";
              echo '</center></body></html>';
         	  session_unregister("manager_ID");
              //session_unregister("$password");
     	      exit();
          }
        }
    } else {
        echo '<html><head><title>错误信息</title></head><body  background="image/pmiback.gif">';
		echo '<p><br><br><center><font face="楷体_GB2312"><b>文件未找到!</b></font>';
		echo '<p>';
   	   	echo "<a href='login.html'><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>";
     	echo '</center></body></html>';
     	session_unregister("manager_ID");

     	exit();

    }
    //----------------20051213;by yy;新增用户文件注册方式；
?>