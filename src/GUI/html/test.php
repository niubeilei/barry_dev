<html>

<head>
  <title></title>
</head>

<body>

<?php

echo "Hello!";

?>
<?php
  include "pmi_db.php";
  $filename = '/usr/SSLMPS/config/managers.conf';

  if (file_exists($filename)) {
    echo "The file $filename exists !";
    echo '<br>';
  } else {
    echo "The file $filename does not exist!";

  }

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
?>
</body>

</html>