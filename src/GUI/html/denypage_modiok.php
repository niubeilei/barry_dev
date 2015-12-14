<?php
	include("constant.php");

@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
            echo '</b></body></html>';
            exit();
	}
	echo '<html><head><title>Denypage修改</title></head><body background="image/pmiback.gif"><center><font face="楷体_GB2312"><b>' ;
    //************************************************************************************
    if (!VarIsValid($denypname,1))
    {
        echo "Denypage名称中不允许空格存在，请修改后再操作！<br>";
    } else if(!VarIsValid($denyppath,1))
    {
        echo "Denypage文件名中不允许空格存在，请修改后再操作！<br>";
    } else
    {
      
      //-------------20051221;by cw;原路径检查使用,现取消，改固定路径DENYPAGEFILEPATH
      if ($opcode==1)
      {
          //------------------------20060101;by cw;Denypage总个数限制在256个以内；
          $dcount=count(GetDenysList());
          if ($dcount>=256)
          {
              echo "超出Denypage个数限制（≤256），添加失败！";
              echo "<p></b></font><a href=\"denypage.php?numb=$numb\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";  
              exit();
          }
          //------------------------20060101;by cw;Denypage总个数限制在256个以内；
          
           if($denypfile_size > 5000)
							{
								echo "<center><font face=\"楷体_GB2312\"><b>文件不能大于5K,上传失败！！！</b></font></center>";
								echo "<p></b></font><a href=\"denypage.php?numb=$numb\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>"; 
								exit();
							}
							
          if (!DenypageExists($denypname,$denyppath))
          {
              $Mdenyppath=DENYPAGEFILEPATH.$denyppath;              
              //echo "path=   ".$_FILES['denypfile']['tmp_name'];
              
        
              
        //add write option to the file   --by ytao
	    	$mysudo = MYSUDO;
	    	$chmodaddow = ADDOW;
	    	$cmd = 0;
	    	$cmd = $mysudo.$chmodaddow.DENYPAGEFILEPATH;
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
              
              if (copy($denypfile,$Mdenyppath))
              //if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile))
              //if (move_uploaded_file($_FILES['denypfile']['tmp_name'],"denyTTT.zip"))
              {
		
		    	//del write option to the file   --by ytao
		    	$mysudo = MYSUDO;
		    	$chmodaddow = DELOW;
		    	$cmd = 0;
		    	$cmd = $mysudo.$chmodaddow.DENYPAGEFILEPATH;
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
		
                  $ret=AddDenyPage($denypname,$denyppath);
                  if ($ret)
                  {
                      echo "Denypage已添加成功！"."<br>";
                      //---------------20060101;by yt;增加“添加Denypage规则接口”；
                      //exec the Cli.exe commands -- add by ytao
                      $cmd=MYSUDO.MYCLI."\\\"deny page import $denypname /usr/local/AOS/Config/Denypages/$denyppath\\\"";
                      //echo $cmd."<br>";
                      exec($cmd,$a);  
                      //---------------20060101;by yt;增加“添加Denypage规则接口”；
                  } else
                  {
                      echo "Denypage添加没能成功，读写文件可能失败！"."<br>";
                  }
              } else
              {
                  echo "Denypage添加没能成功，上传文件失败！"."<br>";
              }
          } else
          {
              echo "名称为<font color=\"#0000FF\"> $denypname </font>的Denypage名称或文件已经存在，<br>请重新填写Denypage名称或文件名！"."<br>";
          }
      } else if($opcode==2)
      {
          $df=DENYPAGEFILEPATH.$denyppath;
          //echo $df;
              
          //$cmd=MYSUDU."mv ".$denypfile." ".DENYPAGEFILEPATH.$denyppath;
          //echo $cmd;
          //exec($cmd);         
          
              if($denypfile_size > 5000)
							{
								echo "<center><font face=\"楷体_GB2312\"><b>文件不能大于5K,上传失败！！！</b></font></center>";
								echo "<p></b></font><a href=\"denypage.php?numb=$numb\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>"; 
								exit();
							}
							
        //add write option to the file   --by ytao
	    	$mysudo = MYSUDO;
	    	$chmodaddow = ADDOW;
	    	$cmd = 0;
	    	$cmd = $mysudo.$chmodaddow.DENYPAGEFILEPATH;
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
          
          if (copy($denypfile,$df))
          {
              echo "Denypage修改已成功！"."<br>";
              //---------------20060101;by yt;增加“修改Denypage规则接口”；
              //exec the Cli.exe commands -- add by ytao
              $cmd=MYSUDO.MYCLI."\\\"deny page import $denypname /usr/local/AOS/Config/Denypages/$denyppath\\\"";
              //echo $cmd."<br>";
              exec($cmd,$a);  
              //---------------20060101;by yt;增加“修改Denypage规则接口”；
              
          } else
          {
              echo "Denypage修改没能成功，上传文件失败！"."<br>";
          }
          
          		//del write option to the file   --by ytao
		    	$mysudo = MYSUDO;
		    	$chmodaddow = DELOW;
		    	$cmd = 0;
		    	$cmd = $mysudo.$chmodaddow.DENYPAGEFILEPATH;
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
		    //	exec($cmd);
          

      }
      //-------------20051221;by cw;原路径检查使用,现取消，改固定路径DENYPAGEFILEPATH
    } 
    //************************************************************************************
    echo "<p></b></font><a href=\"denypage.php?numb=$numb\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
?>