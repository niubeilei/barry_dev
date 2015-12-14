<?php
	include ("constant.php");
	include ("mysql.lib.php");

//----------------20051213;by yy;新增用户文件注册方式；
function GetManagerList()
{
    $filename = MANAGERFILE;
    $MANNAME=array();
    //$MANNAME[]="uuuu";
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
    	
        $handle = fopen($filename, "r");
        $ni=0;
        if ($handle)
        {
          //$MANNAME[]="sss";
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($user, $pass)=split(" ",$buffer,10);
            if ($user>" ")
            {
                $MANNAME[]=$user;
                $ni+=1;
            }
          }
          fclose($handle);
        }
    }
    else
    {
        $MANNAME[]="用户文件不存在!";
    }
    //print_r($MANNAME);
    return $MANNAME;
}

function GetManagerExist($MUser)
{
    $users=GetManagerList();
    //print_r($users);

    foreach ($users as $name)
    {
        if (strcmp($name,$MUser)==0)
        {
            return true;
            break;
        }
    }
    return false;
}

function GetManagerLgin($MUser,$MPass)
{
    $filename = MANAGERFILE;
    $lginOK=0;
    if (file_exists($filename))
    {
        $handle = @fopen($filename, "r");

        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($user, $pass)=split(" ",$buffer,10);
            if (strcmp($MUser,$user)==0 && strcmp($MPass,$pass)==0)
            {
                //session_unregister("$password");
                echo '<html>';
	     		echo '<head>';
	     		echo '<meta http-equiv="refresh" content="1;url=manager.php">';
	     		echo '</head>';
	     		echo '<body>';
		    	echo '</body>';
		     	echo '</html>';
                $lginOK=1;
                break;
            }
          }
          fclose($handle);
        }
    }
    if ($lginOK==1)
    {
        return true;
    }
}

function AddManager($MUser,$MPass)
{
    $filename = MANAGERFILE;
    $AddOK=0;
    if (file_exists($filename))
    {
                
    	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
   	exec($cmd);
    	
        $handle = @fopen($filename, "a");
        $MCode="\r\n".$MUser.' '.$MPass;
        if ($handle)
        {
            if (fwrite($handle, $MCode) === true)
            {
              $AddOK=1;
	      //write_ed();
            }
            fclose($handle);
        }
        
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

    }
    if ($AddOK==1)
    {
        return true;
    }
}

//**** $MType用以指明删除该用户还是修改该用户密码
//**** =0:删除该用户；1:修改密码；
function ModiManager($MUser,$MPass,$MType)
{
    $filename = MANAGERFILE;
    $ModiOK=0;
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
        $Users=array();
        $i=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($user, $pass)=split(" ",$buffer,10);
            if ((strcmp($MUser,$user)==0) && (strcmp($MPass,$pass)==0) && ($MType==0))
            {
                continue;
            }
            else if (strcmp($MUser,$user)==0 && ($MType==1))
            {
                $pass=$MPass;
            }
            $Users[$i]=array("$user","$pass");
            $i+=1;
          }
          fclose($handle);
        }
   
        
        $handle = @fopen($filename, "w");

        $buffer="";
        if ($handle)
        {
          for ($j = 0; $j < $i; $j++)
          {
            if ($j != 0)
            {
                $buffer="\r\n";
            } else
            {
                $buffer="";
            }

            $buffer .= $Users[$j][0]." ".$Users[$j][1];
            fwrite($handle,$buffer);

          }
          fclose($handle);
          $ModiOK=1;
	  //write_ed();
        }
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
    	
    }
    //print_r($Users);
    if ($ModiOK==1)
    {
        return true;
    }
}
//----------------20051213;by yy;新增用户文件注册方式；

?>
