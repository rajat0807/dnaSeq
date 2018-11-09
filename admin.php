<html>
<head>
<title>
Entropic Profiler Administration Screen
</title>
<style type="text/css">
   body { background-color: whitesmoke; text-align: center; font-family: sans-serif; }
   table { border: 1px solid black; background-color: white; text-align: center; }
   th { background-color: deepskyblue; }
   tr { background-color: palegreen; }
   tr.dirname { background-color: lightskyblue; text-align: left; }
   tr.expfile { background-color: lightpink; }
   tr.totalsize { background-color: lightcyan; text-align: right; }
</style>
</head>
<body>
<center>

<table>
<tr><th>Name</th><th>Timestamp</th><th>Age</th><th>Size</th></tr>
<?php
$filespath="./examples/";
$totalsize=0;
for($i=1;$i<=2;$i++){
print("<tr class='dirname'><td colspan=4>".$filespath."</td></tr>");
$foundfiles=glob($filespath."*.seq");
 $expiredate=time()-3*24*3600; // 3 days lifetime
foreach($foundfiles as $file){
  $fullfilename=realpath($file);
  $filename=basename($fullfilename,".seq");
  $filesize=filesize($fullfilename);
  $filedate=filemtime($fullfilename);
  $fileage=(time()-$filedate)/(24*3600);
  if( $i==2 && isset($_POST['submit']) && $filedate<$expiredate ){
      unlink($fullfilename);
      unlink($fullfilename.".tree");
      unlink($fullfilename.".data");
      unlink($fullfilename.".desc");
  } else {
    if(file_exists($fullfilename.".tree")) $filesize+=filesize($fullfilename.".tree");
    if(file_exists($fullfilename.".data")) $filesize+=filesize($fullfilename.".data");
    if(file_exists($fullfilename.".desc")) $filesize+=filesize($fullfilename.".desc");
    $totalsize+=$filesize;
    $rowclass="";
    if($filedate<$expiredate) $rowclass=" class='expfile'";
    print("<tr".$rowclass."><td>".$filename."</td><td>".date("d-m-Y H:i:s",$filedate)."</td><td>".round($fileage,2)."</td><td>".number_format($filesize,0,",",".")."</td></tr>");
  }
}
$filespath="./uploads/";
}
print("<tr class='totalsize'><td colspan=3></td><td>".number_format($totalsize,0,",",".")."</td></tr>");
?>
</table>

<br>
<?php
if(!isset($_POST['submit'])) print("<form method='post' action='".$PHP_SELF."'><input type='submit' name='submit' value='Clean'></form>");
?>
<input type='button' value='Done' onclick="window.open('index.php','_self');">

</center>
</body>
</html>
