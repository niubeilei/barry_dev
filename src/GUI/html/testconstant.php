<html>

<head>
  <title></title>
</head>

<body>

<?php

echo "Hello!";

?>
<?php
  //include "pmi_db.php";
  include "constant.php";
    /*
    $handle = @fopen($filename, "r");
    $nr=0;
    $na=array();
    if ($handle) {
      while (!feof($handle)) {
        $buffer = fgetss($handle, 4096);
        $buffer = trim($buffer);
        list($user, $pass)=split(" ",$buffer,10);
        echo 'line'.$nr.':'.$buffer.';<br>';
        echo 'user:'.$user.';'.'password:'.$pass.';<br>';
        $na[]=$user;
        $nr+=1;
      }
      fclose($handle);
      //$nr=mysql_num_rows($result);
    }
    $ua=GetManagerList();
    print_r($ua);
    //AddManager("abb","abc");
    //print_r($na);
    echo '<br>userfileinfos:<br>';
    ModiManager("manager","123",1);
    */
    /*
    echo "PMI规则记录表！<br>";
    $pa=GetPMIRules();
    print_r($pa);
    echo "<br><br>";
    $count=count($pa);
    echo "<br>array coun is:$count";
    */
    //$ret=ModiPMIRules(0,"App4","","","",2);  //删除Appname=app4的所有记录
    //$ret=ModiPMIRulesAPP("电雷办","综合治理办公室",0);
    //$ret=ModiPMIRulesAPP("App1","FTPApp",1);
    //echo "$ret";
    /*
    $ret=GetAppSvrList();
    $coun=count($ret);
    echo "<br>Countis:$coun";
    print_r($ret);
    if (AddPMIRules("App100","sys3000","保卫处","off"))
      echo "PMIRules 添加陈公！";
    else
      echo "PMIRules 添加失败！";
    */
    echo date('l dS \of F Y h:i:s A');
    echo "<br>";
    echo date('Y/m/d--H:i:s');
    $adenyp=GetDenypagesList();
    print_r($adenyp);
    /*
    if (mkdir("/teeemp"))
      echo "创建目录成功！";

    $strc=strrpos("/usr/temp/ssl/abc.text","/");
    echo "/found at $strc <br>";
    $sbstr=substr("/usr/temp/ssl/abc.text",$strc+1);
    echo "sbustr is $sbstr";
    */
    echo "configfilepath=".CFGFILEPATH."<br>$PROTOCOLFILE";
    echo DENYPAGEFILE;
?>
</body>

</html>