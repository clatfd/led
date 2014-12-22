<?php
$posx=$_GET['x'];
$posy=$_GET['y'];
//echo $posy;
$fp = fopen("setpos.txt", "w"); 
fwrite($fp,$posx.",".$posy."\n"); 
fclose($fp);

echo json_encode(array('msg'=>"set to (".$posx.",".$posy.")"));
?>
