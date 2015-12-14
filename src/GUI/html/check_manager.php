<?
include "pmi_db.php";
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();
	}
	switch($islogin)
	{
		case '0':
                //---------------20051213;by yy;修改原数据库读写注册为文件方式
                if (GetManagerExist($mananame1)===false)
                {
                    AddManager($mananame1,$password1);
                    echo '<center><font face="楷体_GB2312"><b><br>注册成功！</b></font></center>';
	            } else
                {
		            echo '<center><font face="楷体_GB2312"><b><br>该管理员已注册过！</b></font></center>';
                }
                //---------------20051213;by yy;修改原数据库读写注册为文件方式

				echo '<html><head><title>管理员注册</title></head><body  background="image/pmiback.gif">';
				echo '<p>';
   	   			echo "<center><a href='manager.php'><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>";
     	   			echo '</center></body></html>';
     	   			exit();

		case '1':

                //del_manager($mananame2,$password3);
                //---------------20051213;by yy;修改原数据库读写注册为文件方式
                $usercount=GetManagerList();

                $CC=count($usercount);
                if ($CC < 2)
                {
                    echo '<center><font face="楷体_GB2312"><b><br>不能删除最后一个管理员</b></font></center>';
                }
                else if ((GetManagerLgin($mananame2,$password3)) && (ModiManager($mananame2,$password3,0)===true))
                {
                    echo '<center><font face="楷体_GB2312"><b><br>注销成功</b></font></center>';
             	} else
                {
            		echo '<center><font face="楷体_GB2312"><b><br>管理员密码错误或保存错误！</b></font></center>';
            	}
                //---------------20051213;by yy;修改原数据库读写注册为文件方式

     	   		echo '<html><head><title>删除管理员</title></head><body  background="image/pmiback.gif">';
				echo '<p>';
   	   			echo "<center><a href='manager.php'><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>";
     	   			echo '</center></body></html>';
     	   			exit();

		case '2':
     	   	    //update_manager($mananame3,$password4,$password5);
                //---------------20051213;by yy;修改原数据库读写注册为文件方式
                if (GetManagerLgin($mananame3,$password4))
                {
                    if (ModiManager($mananame3,$password5,1)===true)
                    {
                        echo '<center><font face="楷体_GB2312"><b><br>修改密码成功</b></font></center>';
                    } else
                    {
                 	      echo '<center><font face="楷体_<font color="#000000"></font>GB2312"><b><br>管理员密码错误或保存错误！</b></font></center>';
                    }
                } else
                {
                    echo '<center><font face="楷体_GB2312"><b><br>管理员密码错误！</b></font></center>';	
                }
                //---------------20051213;by yy;修改原数据库读写注册为文件方式

                echo '<html><head><title>修改管理员密码</title></head><body  background="image/pmiback.gif">';
				echo '<p>';
   	   			echo "<center><a href='manager.php'><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>";
     	   			echo '</center></body></html>';
     	   			exit();

	}
?>