<?php
$input = "Let us meet at 9 o' clock at the secret place.";
$hash = mhash(MHASH_SHA1, $input);
print "��ϡֵΪ ".bin2hex($hash)."\n";
?>