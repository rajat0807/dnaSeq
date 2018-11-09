<?php

$location="http://".$_SERVER['SERVER_NAME'].dirname($_SERVER['PHP_SELF'])."/";
$command="";
$commandresult="";
$timeout=3.0;
$elapsedtime=0.0;
$waiting=false;
$outputfile=null;
$outputfilename="";
$outputurl="";
$expirationdate=0;
$varsarray=null;

function PrintVars(){
	print("<table border='1'>\n");
	print("<tr><th colspan='2'>Variables</th></tr>\n");
	print("<tr><th>Name</th><th>Value</th></tr>\n");
	$formvars=$_POST;
	foreach($formvars as $key => $val) {
		if($val=="") $val="NULL";
		print("<tr><td>".$key."</td><td>".$val."</td></tr>\n");
	}
	print("</table>\n<br>\n");
}

function PrintOutput(){
	global $command;
	global $commandresult;
	print("<br><table border='1'>\n");
	print("<tr><td>command:</td><td>".$command."</td></tr>\n");
	print("<tr><td>result:</td><td>".$commandresult."</td></tr>\n");
	print("</table>\n");
}

function PrintWaitMessage(){
	print("<html>\n<head><title>Entropic Profiler Running</title></head>\n<body><center>\n");
	print("<table cellspacing='5' border='0' bgcolor='dodgerblue'>\n");
	print("<tr bgcolor='dodgerblue'><th><font color='white'>\n");
	print("Your request is being processed but it may take a while.<br>\n");
	print("The results will be sent to your e-mail as soon as they are completed.\n");
	print("</font></th></tr>\n");
	print("<tr><td align='center'>");
	print("<table border='0' bgcolor='white' cellpadding='15'><tr><td align='justify'><font color='blue'>\n");
	print("You may now go back to the main page or close your browser window.<br>\n");
	print("If you decide to wait on this page, you will be automatically redirected <br>\n");
	print("to the results page when they are ready. <br>\n");
	print("</font></td></tr></table>");
	print("</td></tr>\n</table><br>\n");
	print("<input type='button' value='Back' onclick=\"parent.location='".$_SERVER["HTTP_REFERER"]."'\">&nbsp\n");
	//print("</center></body>\n</html>");
	ob_flush();
	flush();
}

function PrintError($string, $output = false){
	global $location;
	print("<html>\n<head><title>Entropic Profiler Error</title></head>\n<body><center>\n");
	if($output) PrintOutput();
	print("<br><strong>Error: ".$string."</strong><br>\n");
	print("<input type='button' value='Back' onclick=\"parent.location='".$location."index.php'\"><br>\n");
	print("</center></body>\n</html>");
	ob_flush();
	flush();
	exit();
}

function CleanUp(){ exit(); }

function SendEmail(){
	global $outputurl;
	global $expirationdate;
	global $elapsedtime;
	global $varsarray;
	$varemail=$_POST["varemail"];
	$varjobname=$_POST["varjobname"];
	$link=$outputurl;
	$to=$varemail;
	//$varsarray=array($vartype,basename($filepath),$varjobname,$varstudy,$varl,$varphi,$vari,$varmotif,$varwindow);
	$subject="[EntropicProfiler] '".$varjobname."' results are ready!";
	$message="This is an automated response from the 'Entropic Profiler' application.\n";
	$message.="\nOn ".date("jS F Y")." at ".date("H:i")." you submitted a request with the following parameters:\n";
	$message.="Source  : ".$varsarray[0]." ('".$varsarray[1]."')\n";
	$message.="Name    : ".$varsarray[2]."\n";
	$message.="Study by: ".$varsarray[3]."\n";
	if($varsarray[3]=="motif") $message.="Motif   : ".$varsarray[7]."\n";
	else $message.="Phi     : ".$varsarray[5]."\nLength  : ".$varsarray[4]."\nPosition: ".$varsarray[6]."\n";
	$message.="Window  : ".$varsarray[8]."\n";
	$message.="\nYour request took ".number_format($elapsedtime, 2, '.', '')." seconds and is now completed.\n";
	$message.="Please use the link below to view the results:\n".$link."\n";
	$message.="\nNote: This page will only be available until ".date("l, jS \of F Y",$expirationdate).".\n";
	$headers="From: Entropic Profiler <entropicprofiler@kdbio.inesc-id.pt>\nContent-Type: text/plain; charset=\"iso-8859-1\"\n";
	if(!mail($to,$subject,$message,$headers)) print("<br><small>Failed to send e-mail!</small></br>");
}

function RedirectOutput($buffer){
	global $outputfile;
	//chdir(dirname($_SERVER['SCRIPT_FILENAME']));
	fwrite($outputfile,$buffer);
}

function StartRedirect(){
	global $outputfile;
	global $outputfilename;
	$outputfile=fopen($outputfilename,"w");
	if($outputfile===false) PrintError("Error opening file for output.");
	if(ob_start("RedirectOutput")===false) PrintError("Error redirecting output.");
}

function EndRedirect(){
	global $waiting;
	global $outputfile;
	global $outputurl;
	ob_end_flush();
	fclose($outputfile);
	if($waiting) {
		print("<input type='button' value='Results' onclick=\"parent.location='".$outputurl."'\"><br>\n");
		SendEmail();
		print("<script type=\"text/javascript\"> window.location.replace('".$outputurl."'); </script>");
	}
	else {
		//header("Location: ".$outputurl);
		print("<html><head></head><body>\n");
		print("<script type=\"text/javascript\"> window.location.replace('".$outputurl."'); </script>");
		print("</body></html>\n");
	}
}


function PreProcess(){
	while(list($key,$val)=each($_POST)) $$key=$val; // assign all POST variables
	
	if( !isset($vartype) || !isset($varstudy) ) {header("Location: ".$location."index.php");exit();} //PrintError("Please submit data through input form first.");
	if( !isset($varemail) || strlen($varemail)==0 ) PrintError("E-mail variable is not set.");
	if( $vartype=="text" && !isset($vartext) ) PrintError("Text parameter is not set.");
	if( $vartype=="example" && !isset($varexample) ) PrintError("Example parameter is not set.");
	if( $vartype=="load" && !isset($varload) ) PrintError("Load parameter is not set.");
	if( $varstudy=="position" && !isset($vari) ) PrintError("Position parameter not set.");
	if( $varstudy=="motif" && !isset($varmotif) ) PrintError("Motif parameter is not set.");
	
	$uploaddir="./uploads/".$varemail;
	if(!file_exists($uploaddir)){
		if(!mkdir($uploaddir)) PrintError("Can't create upload path.");
		chmod($uploaddir,0777);
	}
	$uploaddir=$uploaddir."/";
	$outputdir="./output/".$varemail;
	if(!file_exists($outputdir)){
		if(!mkdir($outputdir)) PrintError("Can't create output path.");
		chmod($outputdir,0777);
	}
	$outputdir=$outputdir."/";
	
	global $location;
	global $outputfilename;
	global $outputurl;
	$outputfilename=$outputdir."index.html";
	$outputurl=$location.substr($outputfilename,2);
	
	if($varstudy=="motif"){
		$findmax=$_POST["varallpos"];
		if($findmax!="on"){
			$vari=$_POST["varstartpos"];
			$varwindow=( intval($_POST["varendpos"]) - intval($_POST["varstartpos"]) );
		}
	} else $varmotif="";
	
	if($findmax=="on") {
		$findmax="1";
		$varl=5;
	}
	else $findmax="0";
	
	$varsource=$vartype;
	$vartreefile=0;
	
	if($vartype=="text"){
		$varfilename="textinput.seq";
		$filepath=$uploaddir.$varfilename;
		$inputfilehandle=fopen($filepath,'w');
		fwrite($inputfilehandle,$vartext);
		fclose($inputfilehandle);
	}
	if($vartype=="file"){
		$varfilename=basename($_FILES["varfile"]["name"]);
		$filepath=$uploaddir.$varfilename;
		if(file_exists($filepath)) unlink($filepath);
		if(!move_uploaded_file($_FILES["varfile"]["tmp_name"],$filepath)){
			switch($_FILES["varfile"]["error"]){
				case 1: PrintError("File '".$varfilename."' wasn't uploaded (too big for PHP).");break;
				case 2: PrintError("File '".$varfilename."' wasn't uploaded (too big for form).");break;
				case 3: PrintError("File '".$varfilename."' wasn't uploaded (partially uploaded).");break;
				case 4: PrintError("File '".$varfilename."' wasn't uploaded (no file).");break;
				case 6: PrintError("File '".$varfilename."' wasn't uploaded (missing temporary folder).");break;
				case 7: PrintError("File '".$varfilename."' wasn't uploaded (write failed).");break;
				case 8: PrintError("File '".$varfilename."' wasn't uploaded (stopped by extension).");break;
			}
		}
	}
	if($vartype=="example"){
		$varfilename=$varexample;
		$filepath="./examples/".$varfilename;
		$vartreefile=1;
	}
	if($vartype=="load"){
		$varfilename=basename($varload);
		$filepath=$varload;
		$vartreefile=1;
	}
	
	if( !file_exists("./ep") ) PrintError("Main executable is missing.");
	if( !isset($filepath) || !file_exists($filepath) ) PrintError("Sequence file is missing.");
	if( !isset($varl) ) PrintError("'L' parameter is missing.");
	if( !isset($varphi) ) PrintError("'Phi' parameter is missing.");
	if( !isset($vari) ) PrintError("Position parameter is missing.");
	if( !isset($findmax) ) PrintError("FindMax parameter is missing.");
	if( !isset($varwindow) ) PrintError("Window parameter is missing.");
	
	global $expirationdate;
	$expirationdate=time()+5*24*3600; // 5 days
	setcookie("email",$varemail,$expirationdate);
	setcookie("jobname",$varjobname,$expirationdate);
	setcookie("sequencedate",date("d-M-Y H:i:s"),$expirationdate);
	setcookie("sequencefile",$filepath,$expirationdate);
	setcookie("sequencesize",filesize($filepath),$expirationdate);
	setcookie("type",$vartype,$expirationdate);
	setcookie("study",$varstudy,$expirationdate);
	setcookie("window",$varwindow,$expirationdate);
	setcookie("phivalue",$varphi,$expirationdate);
	
	$commandarguments[0]="./ep";
	$commandarguments[1]="-tf";
	$commandarguments[2]="-f".$filepath;
	$commandarguments[3]="-l".$varl;
	$commandarguments[4]="-p".$varphi;
	$commandarguments[5]="-i".$vari;
	$commandarguments[6]="-m".$findmax;
	$commandarguments[7]="-w".$varwindow;
	$commandarguments[8]="-x".$vartreefile;
	$commandarguments[9]="-y".$varmotif;
	$command=implode(" ",$commandarguments);
	return $command;
}

$command=PreProcess();


$starttime=microtime();
//$commandresult=exec($command);
//$commandresult=passthru($command);


$cmdhandle=popen($command,"r");
stream_set_blocking($cmdhandle,0);
$time=0.0;
while(!feof($cmdhandle) && $time<$timeout){
	$commandresult.=fread($cmdhandle,255);
	$time+=0.20;
	usleep(200000);
}
//pclose($cmdhandle);

/*
$cmdhandle=popen($command,"r");
$readarray=array($cmdhandle);
$writearray=NULL;
$exceptarray=NULL;
if( stream_select($readarray,$writearray,$exceptarray,2) !== false ) {
	$endtime=microtime();
	$commandresult=fgets($cmdhandle,255);
}
pclose($cmdhandle);
*/

if($time>=$timeout){
	$waiting=true;
	PrintWaitMessage();
}
stream_set_blocking($cmdhandle,1);
$commandresult.=fread($cmdhandle,255);
pclose($cmdhandle);


$endtime=microtime();
$starttime=explode(" ",$starttime);
$starttime=$starttime[1]+$starttime[0];
$endtime=explode(" ",$endtime);
$endtime=$endtime[1]+$endtime[0];
$elapsedtime=$endtime-$starttime;
$elapsedtime=round($elapsedtime,5);
$output=explode(" ",$commandresult);
$outputcount=count($output);

if($outputcount==1) PrintError("Program execution failed.",true);
if($output[0]=="Error:") PrintError(substr($commandresult,7),true);

$varl=$output[0];
$varphi=$output[1];
$vari=$output[2];
$sequencelength=$output[3];
$limit=$output[4];
$numberofsteps=$output[5];
$numberofnodes=$output[6];
$numberofbytes=$output[7];
$varmotif=$output[8];
$sequencedescription=$output[9];

$varemail=$_POST["varemail"];
$varjobname=$_POST["varjobname"];
$uploaddir="./uploads/".$varemail."/";
$outputdir="./output/".$varemail."/";
$vartype=$_POST["vartype"];
$varstudy=$_POST["varstudy"];
$filepath="";
if($vartype=="text") $filepath=$uploaddir."textinput.seq";
else if($vartype=="file") $filepath=$uploaddir.$_FILES["varfile"]["name"];
else if($vartype=="example") $filepath="./examples/".$_POST["varexample"];
else if($vartype=="load") $filepath=$_POST["varload"];
$varwindow=0;
if($_POST["varstudy"]=="position") $varwindow=$_POST["varwindow"];
else if($_POST["varstudy"]=="motif"){
	if($_POST["varallpos"]=="on") $varwindow=$sequencelength;
	else $varwindow=( intval($_POST["varendpos"]) - intval($_POST["varstartpos"]) );
}
$treefilename=$filepath.".tree";

$sequencedescription="";
$filehandle=fopen($filepath,"r");
if($filehandle !== false){
	if(fgetc($filehandle)==">") $sequencedescription=trim(fgets($filehandle,100));
	fclose($filehandle);
}
if(strlen($sequencedescription)==0) $sequencedescription=$varjobname;

if(!$waiting){
	setcookie("sequencedescription",$sequencedescription,$expirationdate);
	setcookie("sequencelength",$sequencelength,$expirationdate);
	setcookie("lvalue",$varl,$expirationdate);
	setcookie("position",$vari,$expirationdate);
	setcookie("motif",$varmotif,$expirationdate);
} else {
	setcookie("sequencedescription","",-1);
	setcookie("sequencelength","",-1);
	setcookie("lvalue","",-1);
	setcookie("position","",-1);
	setcookie("motif","",-1);
}

$varsarray=array($vartype,basename($filepath),$varjobname,$varstudy,$varl,$varphi,$vari,$varmotif,$varwindow);

//global $location;
$framefilename="valuesframe.html";
$framefile=fopen($framefilename,"w");
fwrite($framefile,"<html>\n<head>\n");
fwrite($framefile,"<meta http-equiv='Cache-Control' content='no-cache'>\n");
fwrite($framefile,"<meta http-equiv='Pragma' content='no-cache'>\n");
fwrite($framefile,"<meta http-equiv='Expires' content='0'>\n");
fwrite($framefile,"<script type='text/javascript'>\n");
fwrite($framefile,"function submitPosition(x) { document.followlink.varstudy.value='position'; document.followlink.vari.value=x; document.followlink.submit(); } \n");
fwrite($framefile,"function submitMotif(x) { document.followlink.varstudy.value='motif'; document.followlink.varmotif.value=x; document.followlink.submit(); } \n");
fwrite($framefile,"</script>\n");
fwrite($framefile,"</head>\n<body>\n");
fwrite($framefile,"<form name='followlink' action='".$location."ep.php' method='post' target='_top'>\n");
fwrite($framefile,"<input type='hidden' name='varemail' value='".$varemail."'>\n");
fwrite($framefile,"<input type='hidden' name='varjobname' value='".$varjobname."'>\n");
fwrite($framefile,"<input type='hidden' name='vartype' value='load'>\n");
fwrite($framefile,"<input type='hidden' name='varload' value='".$filepath."'>\n");
fwrite($framefile,"<input type='hidden' name='varstudy' value='".$varstudy."'>\n");
fwrite($framefile,"<input type='hidden' name='varwindow' value=".$varwindow.">\n");
fwrite($framefile,"<input type='hidden' name='varallpos' value='on'>\n");
fwrite($framefile,"<input type='hidden' name='varphi' value=".$varphi.">\n");
fwrite($framefile,"<input type='hidden' name='varl' value=".$varl.">\n");
fwrite($framefile,"<input type='hidden' name='vari' value=".$vari.">\n");
fwrite($framefile,"<input type='hidden' name='varmotif' value='".$varmotif."'>\n");
fwrite($framefile,"<table border='0' cellspacing='3' width='100%' bgcolor='white'>\n");
fwrite($framefile,"<tr bgcolor='lightskyblue'>\n");
if($varstudy!="motif") fwrite($framefile,"<th>position</th>\n");
fwrite($framefile,"<th>string</th>\n<th>N</th>\n<th>EP(L,&Phi;,i)</th>\n<th>p-value</th>\n<th>z-score</th>\n</tr>\n");
$valuesfilename="values.txt";
if(file_exists($valuesfilename)){
	$valuesfile=fopen($valuesfilename,'r');
	$bgcolor="white";
	$i=0;
	while(!feof($valuesfile)){
		$valuesline=fgets($valuesfile);
		$valuesdata=explode(" ",$valuesline);
		if(count($valuesdata)<6) break;
		if(intval($valuesdata[0])<$varl) continue;
		$valuesdata[4]=sprintf("%.6f",$valuesdata[4]);
		$valuesdata[5]=sprintf("%.6f",$valuesdata[5]);
		fwrite($framefile,"<tr bgcolor='".$bgcolor."'>");
		if($varstudy!="motif") fwrite($framefile,"<td><a href=\"javascript:submitPosition(".$valuesdata[0].")\">".$valuesdata[0]."</a></td><td><a href=\"javascript:submitMotif('".$valuesdata[1]."')\">".$valuesdata[1]."</a></td>");
		else {
			if($i==0) fwrite($framefile,"<td><a href='positions.txt?".time()."' title='Right-click to save list of all positions' style='text-decoration:none;border-bottom-style:dotted;'>".$valuesdata[1]."</a></td>");
			else fwrite($framefile,"<td><a href=\"javascript:submitMotif('".$valuesdata[1]."')\">".$valuesdata[1]."</a></td>");
			$i++;
		}
		fwrite($framefile,"<td>".$valuesdata[2]."</td><td>".$valuesdata[3]."</td><td>".$valuesdata[4]."</td><td>".$valuesdata[5]."</td></tr>\n");
		if($bgcolor=="white") $bgcolor="lightcyan";
		else $bgcolor="white";
	}
	fclose($valuesfile);
	unlink($valuesfilename);
}
fwrite($framefile,"</form>\n</table>\n</body>\n</html>");
fclose($framefile);

$filestomove=array('3dplot.bmp','cgrmap.bmp','corrplot.bmp','distplot.bmp','lmaxplot.bmp','lplot.bmp','plot.bmp','positions.txt','valuesframe.html');
foreach($filestomove as $filename){
	$destfilename=$outputdir.$filename;
	if(file_exists($destfilename)) unlink($destfilename);
	if(file_exists($filename)) rename($filename,$destfilename);
}

StartRedirect();
?>

<html>
<head>
<title>
Entropic Profiler Results
</title>
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, max-age=0">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="0">
<script language="JavaScript" type="text/javascript" src="<?php print($location); ?>scripts.js"></script>
</head>
<body bgcolor="aliceblue" style="font-family:sans-serif;">
<center>

<?php
//global $waiting;
//global $expirationdate;
if($waiting){
	print("<script type=\"text/javascript\">\n");
	print("setCookie('sequencedescription','".$sequencedescription."',".$expirationdate.");\n");
	print("setCookie('sequencelength',".$sequencelength.",".$expirationdate.");\n");
	print("setCookie('lvalue',".$varl.",".$expirationdate.");\n");
	print("setCookie('position',".$vari.",".$expirationdate.");\n");
	print("setCookie('motif','".$varmotif."',".$expirationdate.");\n");
	print("</script>\n");
}
?>

<div id="debuginfoarea" style="display:none;">
<table border="0" cellspacing="3" bgcolor="white" width="100%">
<tr><td bgcolor="lightskyblue">input:</td><td><?php echo $command; ?></td></tr>
<tr><td bgcolor="lightskyblue">output:</td><td><?php echo $commandresult; ?></td></tr>
</table>
</div>

<table border="0" width="100%" height="100%" cellspacing="5" align="center" bgcolor="aliceblue">

<th colspan="2" bgcolor="dodgerblue" onclick="if(debuginfoarea.style.display=='none') debuginfoarea.style.display='block'; else debuginfoarea.style.display='none';">
<big><?php print($sequencedescription); ?></big>
</th>

<tr onclick="if(infoarea.style.display=='block') {infoarea.style.display='none';treeinfoarea.style.display='block';} else {infoarea.style.display='block';treeinfoarea.style.display='none';}">
<th bgcolor="deepskyblue" width="50%">Results</th>
<th bgcolor="deepskyblue" width="50%">Values</th>
</tr>

<tr bgcolor="white" align="center" valign="middle">
<td>
<div id="infoarea" style="display:block;">
<table border="0" cellspacing="3" bgcolor="white" width="100%" height="100%">
<tr><td bgcolor="lightskyblue">source file</td><td><?php print("&lt;".$vartype.":".basename($filepath)."&gt;"); ?></td></tr>
<tr><td bgcolor="lightskyblue">source size</td><td><?php if(file_exists($filepath)) print(number_format(filesize($filepath),0,",",".")); else print("0"); ?> bytes</td></tr>
<tr><td bgcolor="lightskyblue">running time</td><td><?php print($elapsedtime); ?> seconds</td></tr>
<tr><td bgcolor="lightskyblue">sequence (<i>s</i>) length</td><td><?php print(number_format($sequencelength,0,",",".")); ?> base pairs</td></tr>
<tr><td bgcolor="lightskyblue">motif (<i><small><?php echo "s<sub>".($vari-$varl+1)."</sub>...s<sub>".$vari."</sub>"; /* i-L+1...i */ ?></small></i>)</td><td><?php print($varmotif); ?></td></tr>
<tr><td bgcolor="lightskyblue">motif length (<i>L</i>)</td><td><?php print($varl); ?></td></tr>
<tr><td bgcolor="lightskyblue">smoothing parameter (<i>&Phi;</i>)</td><td><?php print($varphi); ?></td></tr>
<tr><td bgcolor="lightskyblue">position (<i>i</i>)</td><td><?php print($vari); ?></td></tr>
<tr><td bgcolor="lightskyblue">range window</td><td><?php print(number_format($varwindow,0,",",".")); ?> positions</td></tr>
</table>
</div>
<div id="treeinfoarea" style="display:none;">
<table border="0" cellspacing="3" bgcolor="white" width="100%" height="100%">
<tr><td bgcolor="lightskyblue">tree depth limit</td><td><?php print($limit); ?> characters</td></tr>
<tr><td bgcolor="lightskyblue">tree build work</td><td><?php print(number_format($numberofsteps,0,",",".")); ?> steps</td></tr>
<tr><td bgcolor="lightskyblue">tree size</td><td><?php print(number_format($numberofnodes,0,",",".")); ?> nodes</td></tr>
<tr><td bgcolor="lightskyblue">used memory</td><td><?php print(number_format($numberofbytes,0,",",".")); ?> bytes</td></tr>
<tr><td bgcolor="lightskyblue">tree file size</td><td><?php if(file_exists($treefilename)) print(number_format(filesize($treefilename),0,",",".")); else print("0"); ?> bytes</td></tr>
<tr><td bgcolor="lightskyblue">tree density</td><td><?php printf("%.2f",($numberofnodes/1398101)*100); ?>%</td></tr>
</table>
</div>
</td>
<td>
<iframe name="valuesframe" id="valuesframe" src="valuesframe.html" frameborder="0" scrolling="auto" width="100%" height="100%"><a href='valuesframe.html'>Values</a></iframe>
<script>document.frames['valuesframe'].location.reload();</script>
</td>
</tr>

<tr align="center"><td colspan="2"><small>Hold the mouse over the plots to view a more detailed description.</small></td></tr>

<?php if($varstudy=="motif") { ?>
<tr>
<th bgcolor="deepskyblue" width="50%">Motif Distribution</th>
<th bgcolor="deepskyblue" width="50%">Motif L Plot</th>
</tr>
<tr bgcolor="white" align="center" valign="middle">
<td>
<img src="distplot.bmp<?php print("?".time()); ?>" alt="Histogram of Motif distribution inside specified range" onmouseover="opentooltip(this);" onmouseout="closetooltip();">
</td>
<td>
<img src="lplot.bmp<?php print("?".time()); ?>" alt="E.P. values for variable Motif lenghts" onmouseover="opentooltip(this);" onmouseout="closetooltip();">
</td>
</tr>
<?php } else { ?>

<tr>
<th bgcolor="deepskyblue" width="50%">EP Plot</th>
<th bgcolor="deepskyblue" width="50%">3D Plot</th>
</tr>

<tr bgcolor="white" align="center" valign="middle">
<td>
<img src="plot.bmp<?php print("?".time()); ?>" alt="<?php echo "E.P. values for all motifs of length $varl around position $vari"; ?>" onmouseover="opentooltip(this);" onmouseout="closetooltip();">
</td>
<td>
<img src="3dplot.bmp<?php print("?".time()); ?>" alt="<?php echo "3D E.P. values for all motifs of lengths from 1 to 10 around position $vari"; ?>" onmouseover="opentooltip(this);" onmouseout="closetooltip();">
</td>
</tr>

<tr>
<th bgcolor="deepskyblue" width="50%">L Plot</th>
<th bgcolor="deepskyblue" width="50%">L Max Plot</th>
</tr>

<tr bgcolor="white" align="center" valign="middle">
<td>
<img src="lplot.bmp<?php print("?".time()); ?>" alt="<?php echo "E.P. values for motifs of lengths from 1 to 10 that end at position $vari"; ?>" onmouseover="opentooltip(this);" onmouseout="closetooltip();">
</td>
<td>
<img src="lmaxplot.bmp<?php print("?".time()); ?>" alt="<?php echo "Length values that maximize the E.P. value of each position around position $vari"; ?>" onmouseover="opentooltip(this);" onmouseout="closetooltip();">
</td>
</tr>

<tr><td colspan="2"><br></td></tr>

<tr>
<th bgcolor="deepskyblue" width="50%">CGR Map</th>
<th bgcolor="deepskyblue" width="50%">Correlation Plot</th>
</tr>

<tr bgcolor="white" align="center" valign="middle">
<td>
<img src="cgrmap.bmp<?php print("?".time()); ?>" alt="Chaos Game Representation Map" onmouseover="opentooltip(this);" onmouseout="closetooltip();">
</td>
<td>
<img src="corrplot.bmp<?php print("?".time()); ?>" alt="Correlation Plot for Entropic Profiles and p-values" onmouseover="opentooltip(this);" onmouseout="closetooltip();">
</td>
</tr>

<?php } ?>

<tr><td colspan="2"><br></td></tr>

<tr><td colspan="2" align="center" bgcolor="lightskyblue" onclick="backbutton.focus();backbutton.click();" onmouseover="this.bgColor='deepskyblue';" onmouseout="this.bgColor='lightskyblue';">
<input type="button" value="Back" name="backbutton" id="backbutton" onClick="window.location='<?php print($location."index.php"); ?>';" style="border:1px solid blue;width:100%;background-color:lightskyblue;" onmouseover="this.style.backgroundColor='deepskyblue';" onmouseout="this.style.backgroundColor='lightskyblue';">
</td></tr>

</table>

</center>
</body>
</html>

<?php
EndRedirect();
?>
