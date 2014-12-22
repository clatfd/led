<?php
$mode=$_GET['mode'];
//echo $posy;
$modechar=substr($mode,0);
$fp = fopen("setdir.txt", "w"); 
fwrite($fp,$modechar."\n"); 
fclose($fp);

echo json_encode(array('msg'=>"Turning ".$mode));
?>
