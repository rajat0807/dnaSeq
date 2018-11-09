<html>
<head>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=ISO-8859-1">
<title>
Entropic Profiler
</title>
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, max-age=0">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="0">
<script language="JavaScript" type="text/javascript" src="scripts.js"></script>

<?php
$hascookies=isset($_COOKIE["type"]);
if($hascookies){
	$cookiedescription=$_COOKIE["sequencedescription"];
	$cookielength=$_COOKIE["sequencelength"];
	$cookiedate=$_COOKIE["sequencedate"];
	$cookiefile=$_COOKIE["sequencefile"];
	$cookiesize=$_COOKIE["sequencesize"];
	$cookiel=$_COOKIE["lvalue"];
	$cookiephi=$_COOKIE["phivalue"];
	$cookiei=$_COOKIE["position"];
	$cookiewindow=$_COOKIE["window"];
	$cookiemotif=$_COOKIE["motif"];
	$cookiestudy=$_COOKIE["study"];
	$cookieemail=$_COOKIE["email"];
	$cookiejobname=$_COOKIE["jobname"];
	if( !file_exists($cookiefile) || !file_exists($cookiefile.".tree")
	|| !file_exists($cookiefile.".data") || !file_exists($cookiefile.".desc") )
		$hascookies=FALSE;
}
?>

<script type="text/javascript">

function show(e){
	e.style.display="block";
}

function hide(e){
	e.style.display="none";
}

function markerror(e){
	e.style.borderStyle="dashed";
	e.style.borderColor="red";
}

function clearerror(e){
	e.style.borderStyle="";
	e.style.borderColor="";
}

function numbersonly(e){
	var keycode=(e.charCode ? e.charCode : e.keyCode);
	if ( (keycode<48 || keycode>57) /* 0...9 */
		&& keycode!=35 && keycode!=36 && keycode!=37 && keycode!=39 /* end , home , left , right */
		&& keycode!=8 && keycode!=45 && keycode!=46 ) return false; /* backspace , insert , delete */
}

function basesonly(e){
	var keycode=(e.charCode ? e.charCode : e.keyCode);
	if (keycode!=65 && keycode!=67 && keycode!=71 && keycode!=84 /* A , C , G , T */
		&& keycode!=97 && keycode!=99 && keycode!=103 && keycode!=116 /* a , c , g , t */
		&& keycode!=35 && keycode!=36 && keycode!=37 && keycode!=39 /* end , home , left , right */
		&& keycode!=8 && keycode!=45 && keycode!=46) return false; /* backspace , insert , delete */
}

function validchar(a){
	var c=a.charCodeAt(0);
	if( (c>=48 && c<=57) || (c>=65 && c<=90) || (c>=97 && c<=122) ) /* 0...9 , A...Z , a...z */
		return true;
	return false;
}

function emailchar(a){
	var c=a.charCodeAt(0);
	if( (c>=48 && c<=57) || (c>=65 && c<=90) || (c>=97 && c<=122) /* 0...9 , A...Z , a...z */
		|| c==46 || c==45 || c==95 ) return true; /* '.' , '-' , '_' */ /* 32==' ' , 64=='@' */
	return false;
}

function cutCharAt(s,i){
	return ( s.substring(0,i) + s.substring(i+1) );
}

function changeType(e){
	var f=document.forminput;
	var type=e.value;
	hide(document.getElementById("textarea"));
	hide(document.getElementById("filearea"));
	hide(document.getElementById("examplearea"));
	hide(document.getElementById("loadarea"));
	show(document.getElementById(type+"area"));
	f.newseq.value='1';
	if(type=="load"){
		f.newseq.value='0';
		populateLoad();
	}
	varValidate(f.vari);
	varValidate(f.varmotif);
	varValidate(f.varemail);
	if(type!="load") document.getElementById("var"+type).focus();
}

function changeStudy(e){
	var f=document.forminput;
	var type=e.value;
	varValidate(f.vartext);
	if(type=="position"){
		show(document.getElementById("positionarea"));
		show(document.getElementById("larea"));
		show(document.getElementById("phiarea"));
		hide(document.getElementById("motifarea"));
		hide(document.getElementById("acgtarea"));
		hide(document.getElementById("empty1"));
		show(document.getElementById("positionoptionsarea"));
		hide(document.getElementById("motifoptionsarea"));
		varValidate(f.vari);
		f.vari.focus();
	}
	if(type=="motif"){
		hide(document.getElementById("positionarea"));
		hide(document.getElementById("larea"));
		hide(document.getElementById("phiarea"));
		show(document.getElementById("motifarea"));
		show(document.getElementById("acgtarea"));
		show(document.getElementById("empty1"));
		hide(document.getElementById("positionoptionsarea"));
		show(document.getElementById("motifoptionsarea"));
		varValidate(f.varmotif);
		f.varmotif.focus();
	}
}

function populateExample(){
	var f=document.forminput;
	if(f.varexample.selectedIndex=='0'){
		f.varl.value='8';
		f.varphi.value='10';
		f.varwindow.value='100';
		f.vari.value='35840';
		f.varmotif.value='GCTGGTGG';
		f.varstartpos.value='35800';
		f.varendpos.value='35900';
		f.varseqlength.value='4639675';
		f.varjobname.value='Example1';
	}
	if(f.varexample.selectedIndex=='1'){
		f.varl.value='8';
		f.varphi.value='10';
		f.varwindow.value='100';
		f.vari.value='36532';
		f.varmotif.value='AAGTGCGGT';
		f.varstartpos.value='36500';
		f.varendpos.value='36600';
		f.varseqlength.value='1830023';
		f.varjobname.value='Example2';
	}
	if(f.varexample.selectedIndex=='2'){
		f.varl.value='6';
		f.varphi.value='10';
		f.varwindow.value='100';
		f.vari.value='276';
		f.varmotif.value='TATAAT';
		f.varstartpos.value='300';
		f.varendpos.value='400';
		f.varseqlength.value='2000';
		f.varjobname.value='Example3';
	}
}

function populateLoad(){
	var f=document.forminput;
	f.vari.value='<?php if($hascookies) print($cookiei); ?>';
	f.varwindow.value='<?php if($hascookies) print($cookiewindow); ?>';
	f.varl.value='<?php if($hascookies) print($cookiel); ?>';
	f.varphi.value='<?php if($hascookies) print($cookiephi); ?>';
	f.varmotif.value='<?php if($hascookies) print($cookiemotif); ?>';
	f.varseqlength.value='<?php if($hascookies) print($cookielength); ?>';
	f.varemail.value='<?php if($hascookies) print($cookieemail); ?>';
	f.varjobname.value='<?php if($hascookies) print($cookiejobname); ?>';
}

function varValidate(v){
	var f=document.forminput;
	var seqlength=parseInt(f.varseqlength.value);
	var error=false;
	switch(v.name){
		case 'varemail': // ok
			n=v.value.length;
			if(n<8 || v.value.indexOf("@")==-1 || v.value.indexOf(".")==-1) { error=true; break; }
			temp=v.value.split("@");
			for(k=0;k<2;k++){
				n=temp[k].length;
				for(i=0;i<n;i++){
					c=temp[k].charAt(i);
					if(!emailchar(c)){
						temp[k]=cutCharAt(temp[k],i);
						i--;
						n--;
					}
				}
			}
			v.value=( temp[0] + "@" + temp[1] );
			if( temp[0].length<2 || temp[1].length<5 ) { error=true; break; }
			k=temp[1].lastIndexOf(".");
			n=temp[1].length;
			if( k==-1 || k<2 || (n-k-1)<2 || (n-k-1)>4 ) error=true;
			break;
		case 'varjobname': // ok
			n=v.value.length;
			if(n==0) { error=true; break; }
			for(i=0;i<n;i++){
				c=v.value.charAt(i);
				if(!validchar(c)){
					v.value=cutCharAt(v.value,i);
					i--;
					n--;
				}
			}
			break;
		case 'vartext': // ok
			if(!f.vartype[0].checked) break;
			seqlength=v.value.length;
			i=0;
			if(v.value.charAt(0)=='>') { while(v.value.charAt(i)!='\n' && i<seqlength) {i++;} i++; }
			desclength=i;
			for(;i<seqlength;i++){
				c=v.value.charCodeAt(i);
				if( c==65 || c==67 || c==71 || c==84 ) continue;	/* 'A' , 'C' , 'G' , 'T' */
				//else if( c==10 || c==13 || c==9 || c==32 ) continue;	/* '\n' , '\r' , tab , space */
				else if( c==97 || c==99 || c==103 || c==116 )		/* 'a' , 'c' , 'g' , 't' */
					v.value=v.value.substring(0,i)+String.fromCharCode(c-32)+v.value.substring(i+1,seqlength);
				else {
					v.value=(v.value.substring(0,i)+v.value.substring(i+1,seqlength));
					i--;
					seqlength--;
				}
			}
			seqlength-=desclength;
			if(seqlength<10) { error=true; break;}
			if(f.varstudy[0].checked && seqlength<f.vari.value) f.vari.value=seqlength;
			if(f.varstudy[0].checked && seqlength<f.varwindow.value)
				f.varwindow.value=Math.min(9999,seqlength);
			if(f.varstartpos.value>seqlength) f.varstartpos.value=1;
			if(f.varendpos.value>seqlength) f.varendpos.value=seqlength;
			f.varseqlength.value=seqlength;
			break;
		case 'varfile': // ok
			if(f.vartype[1].checked && v.value.length==0) error=true;
			break;
		case 'vari': // ok
			if(!f.varstudy[0].checked) break;
			for(i=0;i<v.value.length;i++){
				c=v.value.charCodeAt(i);
				if(c<48 || c>57){ /* 0...9 */
					v.value=(v.value.substring(0,i)+v.value.substring(i+1,v.value.length));
					i--;
				}
			}
			if(v.value.length==0 || v.value.length>10 || isNaN(v.value)) { error=true; break; }
			if(v.value<3) v.value=3;
			if(v.value<parseInt(f.varl.value)) f.varl.value=v.value;
			if(!f.vartype[1].checked && v.value>seqlength) v.value=seqlength;
			break;
		case 'varl': // ok
			if(!f.varstudy[0].checked) break;
			if(parseInt(v.value)<3 || parseInt(v.value)>10) v.value=5;
			if(parseInt(v.value)>f.vari.value) f.vari.value=parseInt(v.value);
			break;
		case 'varwindow': // ok
			if(!f.varstudy[0].checked) break;
			if(v.value.length==0 || isNaN(v.value)) v.value=Math.min(100,seqlength);
			if(v.value<3) v.value=3;
			if(v.value.length>4 || v.value>9999) v.value=9999;
			if(!f.vartype[1].checked && v.value>seqlength) v.value=Math.min(9999,seqlength);
			break;
		case 'varmotif': // ok
			if(!f.varstudy[1].checked) break;
			v.value=v.value.toUpperCase();
			for(i=0;i<v.value.length;i++){
				c=v.value.charCodeAt(i);
				if(c!=65 && c!=67 && c!=71 && c!=84){
					v.value=(v.value.substring(0,i)+v.value.substring(i+1,v.value.length));
					i--;
				}
			}
			if(v.value.length<3 || v.value.length>10) error=true;
			break;
		case 'varstartpos': // ok
			if(!f.varstudy[1].checked || f.varallpos.checked) break;
			v.value=parseInt(v.value);
			if(v.value.length==0 || v.value<1 || isNaN(v.value)) v.value=1;
			if(!f.vartype[1].checked && (parseInt(v.value) + f.varmotif.value.length)>seqlength)
				v.value=(seqlength - f.varmotif.value.length + 1);
			if((parseInt(v.value) + f.varmotif.value.length)>f.varendpos.value)
				f.varendpos.value=(parseInt(v.value) + f.varmotif.value.length - 1);
			break;
		case 'varendpos': // ok
			if(!f.varstudy[1].checked || f.varallpos.checked) break;
			v.value=parseInt(v.value);
			if(v.value.length==0 || v.value<1 || isNaN(v.value)) v.value=seqlength;
			if(!f.vartype[1].checked && v.value>seqlength) v.value=seqlength;
			if((v.value - f.varmotif.value.length)<f.varstartpos.value)
				f.varstartpos.value=(v.value - f.varmotif.value.length + 1);
			break;
		default:
			break;
	}
	if(error==true) markerror(v);
	else clearerror(v);
	return (!error);
}

function formValidate(){
	var f=document.forminput;
	var result=true;
	result=(result && varValidate(f.varemail));
	result=(result && varValidate(f.varjobname));
	result=(result && varValidate(f.vartext));
	result=(result && varValidate(f.varfile));
	result=(result && varValidate(f.vari));
	result=(result && varValidate(f.varl));
	result=(result && varValidate(f.varwindow));
	result=(result && varValidate(f.varmotif));
	result=(result && varValidate(f.varstartpos));
	result=(result && varValidate(f.varendpos));
	if(!result) return false;
	hide(document.getElementById("textarea"));
	hide(document.getElementById("filearea"));
	hide(document.getElementById("examplearea"));
	hide(document.getElementById("loadarea"));
	show(document.getElementById("waitarea"));
	f.submit.disabled='true';
	f.reset.disabled='true';
	f.submit.value='Sending information...';
	n=f.elements.length;
	for(i=0;i<n;i++){
		if(f.elements[i].type!='hidden') f.elements[i].readOnly='true';
	}
	return true;
}

function formReset(){
	var f=document.forminput;
	clearerror(f.vartext);
	clearerror(f.varfile);
	clearerror(f.vari);
	clearerror(f.varl);
	clearerror(f.varwindow);
	clearerror(f.varstartpos);
	f.varstartpos.disabled='true';
	clearerror(f.varendpos);
	f.varendpos.disabled='true';
	clearerror(f.varmotif);
	clearerror(f.varemail);
	clearerror(f.varjobname);
	f.varstudy[0].click();
	f.vartype[0].click();
	hide(document.getElementById("waitarea"));
	show(document.getElementById("positionoptionsarea"));
	hide(document.getElementById("motifoptionsarea"));
	hide(document.getElementById("optionsarea"));
	show(document.getElementById("optionslabelarea"));
	f.submit.disabled='';
}

</script>

</head>

<body bgcolor="aliceblue" style="font-family:sans-serif;" onload="document.forminput.varstudy[<?php if($hascookies && $cookiestudy=="motif") print("1"); else print("0"); ?>].click();document.forminput.submit.disabled='';document.forminput.vartype[<?php if($hascookies) print(3); else print(0); ?>].click();">
<center>

<div name="logo" id="logo" style="position:absolute;top:0px;right:0px;z-index:2;">
<a href="http://kdbio.inesc-id.pt/"><img style="border:none;" src="logo.gif" alt="INESC-ID KDBIO"></a>
</div>
<!-- -moz-opacity:0.4;opacity:0.4;filter:alpha(opacity=40); -->

<form name="forminput" action="ep.php" method="post" enctype="multipart/form-data" onsubmit="return formValidate();" onreset="formReset();">

<table width="75%" border="0">
<tr><td align="center" valign="middle">

<table border="0">
<tr><td>
<fieldset style="border: 1px dotted rgb(0,255,0); background-color: rgb(245,255,245);">
<legend style="font: bold large cursive; color: green; background-color: white; border: 2px double rgb(0,255,0); padding: 5px;">Entropic Profiler</legend>
<p align="justify"><font face="Verdana">
<b>Entropic profiles</b> of DNA sequences are local information plots of the relative over and
under-expression of motifs per position. They are calculated based on Chaos Game
Representation (CGR) using a recently proposed fractal kernel and Parzen's window density
estimation method. They allow the visualization of motif densities for two different
parameters: resolution <em>L</em> and smoothing parameter <em>&Phi;</em>. This method detects biological
significant regions of DNA, here exemplified for the genomes of <em>E.coli</em> and <em>H.influenzae</em> and
promoter regions in <em>B.subtilis</em>.
An important simplification allows its calculation using segment counts, explored in this
application through suffix trees.<br>
<small>References:<br><em>
Vinga, S. and Almeida, J.S. <a href="http://www.biomedcentral.com/1471-2105/8/393/" style="cursor:help;">
<b>Local Renyi entropic profiles of DNA sequences.</b></a> BMC Bioinformatics 2007, 8:393 (Oct 16).<br>
Fernandes F., Vinga S., Freitas A.T. (2007), <a href="http://www.inesc-id.pt/ficheiros/publicacoes/4737.pdf" style="cursor:help;">
<b>Detection of conserved regions in genomes using entropic profiles</b></a>, INESC-ID Tec. Rep. 33/2007
</em>
<br><br>For a more detailed description of the parameters in this application, please hold your mouse over each one of the options.<br>
<br><a href="mailto:entropicprofiler@kdbio.inesc-id.pt?subject=[EntropicProfiler]">Support and suggestions</a>
<br><a href="http://www.inesc-id.pt/ficheiros/publicacoes/4737.pdf">Supplementary material</a>
</small>
</font></p>
</fieldset>
</td>
<td>
</td></tr>
</table>

</td></tr>
<tr><td align="center" valign="middle">

<a name="options"></a>
<table border="0" cellspacing="4">

<tr bgcolor="lightskyblue">
<td width="10%" height="10%" align="center" bgcolor="deepskyblue"><br></td>
<td width="20%" bgcolor="skyblue"></td>
<td width="70%" nowrap colspan="2" onmouseover="this.bgColor='lightblue';" onmouseout="this.bgColor='lightskyblue';">
<label for="varemail">&bull; E-mail: </label><input type="text" name="varemail" id="varemail" value='<?php /* print("user@".gethostbyaddr($_SERVER['REMOTE_ADDR'])); */ ?>' onblur="varValidate(this);">
&nbsp;&nbsp;&nbsp;
<label for="varjobname">&bull; Job name: </label><input type="text" name="varjobname" id="varjobname" value='Test' onblur="varValidate(this);">
</td>
</tr>

<tr bgcolor="lightskyblue">
<td height="50%" align="center" bgcolor="deepskyblue">Sequence</td>
<td nowrap bgcolor="skyblue" onmouseover="this.bgColor='powderblue';" onmouseout="this.bgColor='skyblue';">
<br>
<input type="radio" name="vartype" value="text" id="text" checked onclick="changeType(this);">
<label for="text">FASTA Text:</label><br>
<br>
<input type="radio" name="vartype" value="file" id="file" onclick="changeType(this);">
<label for="file">FASTA File:</label><br>
<br>
<input type="radio" name="vartype" value="example" id="example" onclick="changeType(this);">
<label for="example">Load Example:</label><br>
<br>
<input type="radio" name="vartype" value="load" id="load" <?php if(!$hascookies) print("disabled"); ?> onclick="changeType(this);">
<label for="load">Load Last Work:</label><br>
<br>
</td>

<td align="center" valign="middle" colspan="2">
<div id="textarea" style="display:block;">
<textarea name="vartext" id="vartext" rows="10" cols="50" wrap="virtual" style="height:100%;width:100%;" onblur="varValidate(this);"></textarea>
<!-- height:100%;width:100%;white-space:-moz-pre-wrap;overflow:auto;word-wrap:break-word;white-space:pre-wrap; -->
</div>
<div id="filearea" style="display:none;">
<input type="hidden" name="MAX_FILE_SIZE" value="5000000" onblur="varValidate(this);">
<input type="file" name="varfile" id="varfile">
</div>
<div id="examplearea" style="display:none;">
<select name="varexample" id="varexample" onfocus="populateExample();" onchange="populateExample();">
<option value="example1.seq" selected>Escherichia coli K12</option>
<option value="example2.seq">Haemophilus influenzae Rd KW20</option>
<option value="example3.seq">Promoter regions in B.subtilis</option>
</select>
</div>
<div id="loadarea" style="display:none;">
<table width="90%" border="0" cellspacing="3">
<tr><td bgcolor="deepskyblue" nowrap><b>Job name</b></td><td bgcolor="white"><?php if($hascookies) print($cookiejobname); ?></td></tr>
<tr><td bgcolor="deepskyblue" nowrap><b>Description</b></td><td bgcolor="white"><small><?php if($hascookies) print($cookiedescription); ?></small></td></tr>
<tr><td bgcolor="deepskyblue" nowrap><b>Sequence length</b></td><td bgcolor="white"><?php if($hascookies) print(number_format($cookielength,0,",",".")); ?> basepairs</td></tr>
<tr><td bgcolor="deepskyblue" nowrap><b>Submitted on</b></td><td bgcolor="white"><?php if($hascookies) print($cookiedate); ?></td></tr>
<tr><td bgcolor="deepskyblue" nowrap><b>File name</b></td><td bgcolor="white">&lt;<?php if($hascookies) print(basename($cookiefile)); ?>&gt;</td></tr>
<tr><td bgcolor="deepskyblue" nowrap><b>File size</b></td><td bgcolor="white"><?php if($hascookies) print(number_format($cookiesize,0,",",".")); ?> bytes</td></tr>
</table>
<input type="hidden" name="varload" value="<?php if($hascookies) print($cookiefile); ?>" >
</div>
<div id="waitarea" style="display:none;cursor:wait;">
<table border="0" bgcolor="white">
<tr><td align="center" valign="middle">
<font color="blue"><big><strong>Sending data and performing calculations!<br>Please wait...</strong></big></font>
</td></tr>
</table>
</div>
</td>
</tr>

<tr bgcolor="lightskyblue">
<td width="10%" height="30%" align="center" rowspan="3" bgcolor="deepskyblue">
Study by
</td>
<td width="20%" align="left" rowspan="3" bgcolor="skyblue" onmouseover="this.bgColor='powderblue';" onmouseout="this.bgColor='skyblue';">
<input type="radio" name="varstudy" value="position" id="position" checked onclick="changeStudy(this);">
<label for="position">Position:</label><br><br>
<input type="radio" name="varstudy" value="motif" id="motif" onclick="changeStudy(this);">
<label for="motif">Motif:</label>
</td>

<td width="20%" align="center" valign="middle" rowspan="3" onclick="if(varstudy[0].checked) vari.focus(); else varmotif.focus();" onmouseover="this.bgColor='lightblue';" onmouseout="this.bgColor='lightskyblue';">
<div id="positionarea" style="display:block;">
<input type="text" name="vari" value="100" id="vari" size="13" maxlength="10" align="middle" onkeypress="return numbersonly(event);" onblur="varValidate(this);">
</div>
<div id="motifarea" style="display:none;">
<input type="text" name="varmotif" value="ACGT" id="varmotif" size="13" maxlength="10" align="middle" onkeypress="return basesonly(event);" onblur="varValidate(this);">
</div>
</td>

<td width="50%" align="left" onclick="if(varl.style.display=='block') varl.focus();" onmouseover="this.bgColor='lightblue';" onmouseout="this.bgColor='lightskyblue';">
<div id="larea" style="display:block;">
<!-- <label for="varl"><b>L</b>= </label> -->
<select name="varl" id="varl" onchange="varValidate(this);">
<?php
for($i=3;$i<=10;$i++){
	print("<option value='".$i."'");
	if($i==5) print(" selected");
	print(">".$i."</option>\n");
}
?>
</select>
<label for="varl" style="cursor:help;" title="Length of the motif that ends in the specified position.">Resolution Length</label>
</div>
<div id="empty1" style="display:none;"><br></div>
</td>
</tr>

<tr bgcolor="lightskyblue">
<td align="left" onclick="if(varphi.style.display=='block') varphi.focus();" onmouseover="this.bgColor='lightblue';" onmouseout="this.bgColor='lightskyblue';">
<div id="phiarea" style="display:block;">
<!-- <label for="varphi"><b>&Phi;= </b></label> -->
<select name="varphi" id="varphi">
<?php
for($i=1;$i<=10;$i++){
	print("<option value='".$i."'");
	if($i==5) print(" selected");
	print(">".$i."</option>\n");
}
?>
</select>
<label for="varphi" style="cursor:help;" title="The higher the &Phi; value, the higher is the weight given to larger motifs.">Smoothing Parameter</label>
</div>
<div id="acgtarea" style="display:none;">
<input type="button" value=" A " onclick="if(varmotif.value.length<10) varmotif.value=varmotif.value+'A'; varValidate(varmotif); varmotif.focus();">
<input type="button" value=" C " onclick="if(varmotif.value.length<10) varmotif.value=varmotif.value+'C'; varValidate(varmotif); varmotif.focus();">
<input type="button" value=" G " onclick="if(varmotif.value.length<10) varmotif.value=varmotif.value+'G'; varValidate(varmotif); varmotif.focus();">
<input type="button" value=" T " onclick="if(varmotif.value.length<10) varmotif.value=varmotif.value+'T'; varValidate(varmotif); varmotif.focus();">
<input type="button" value=" &larr;" onclick="if(varmotif.value.length>0) varmotif.value=varmotif.value.slice(0,varmotif.value.length-1); varValidate(varmotif); varmotif.focus();">
</div>
</td>
</tr>

<tr bgcolor="lightskyblue">
<td align="left" onclick="if(optionslabelarea.style.display=='block') optionslink.click();" onmouseover="this.bgColor='lightblue';" onmouseout="this.bgColor='lightskyblue';">
<div id="optionslabelarea" style="display:block;">
<a href="#options" id="optionslink" onclick="optionsarea.style.display='block'; optionslabelarea.style.display='none';">Options</a>
</div>
<div id="optionsarea" style="display:none;">
<div id="positionoptionsarea" style="display:block;">
<input type="text" name="varwindow" id="varwindow" value="100" size="4" maxlength="4" onkeypress="return numbersonly(event);" onblur="varValidate(this);">
<label for="varwindow" style="cursor:help;" title="Range of positions to study around the specified position.">Window Length</label>
&nbsp;&nbsp;&nbsp;&nbsp;
<input type="checkbox" name="findmax" id="findmax" onclick="if(this.checked) varl.disabled=true; else varl.disabled=false;">
<label for="findmax" style="cursor:help;" title="Automatically detect the length of the motif that maximizes the E.P. value of the specified position.">Find Max L</label>
</div>
<div id="motifoptionsarea" style="display:none;">
<small>
<input type="checkbox" name="varallpos" id="varallpos" checked onclick="if(varallpos.checked) {varstartpos.disabled='true';varendpos.disabled='true';} else {varstartpos.disabled='';varendpos.disabled='';}">
<label for="varallpos" style="cursor:help;" title="Search for the motif on all positions of the sequence.">Search entire sequence</label>
&nbsp;&nbsp;
<label for="varstartpos" style="cursor:help;" title="Position in the sequence to start the search.">From:</label>
<input type="text" name="varstartpos" id="varstartpos" value="1" size="5" maxlength="10" disabled onkeypress="return numbersonly(event);" onblur="varValidate(this);">
<label for="varendpos" style="cursor:help;" title="Position in the sequence to end the search.">To:</label>
<input type="text" name="varendpos" id="varendpos" value="1" size="5" maxlength="10" disabled onkeypress="return numbersonly(event);" onblur="varValidate(this);">
</small>
</div>
</div>
</td>
</tr>

<tr bgcolor="lightskyblue">
<td height="10%" bgcolor="deepskyblue" align="center" onclick="window.location='admin.php';">
</td>
<td align="center" bgcolor="skyblue" onclick="reset.click();" onmouseover="this.bgColor='powderblue';" onmouseout="this.bgColor='skyblue';">
<input type="reset" name="reset" style="border:3px double blue;width:100%;background-color:skyblue;" onmouseover="this.style.backgroundColor='powderblue';" onmouseout="this.style.backgroundColor='skyblue';" value="Reset">
</td>
<td align="center" colspan="2" onclick="submit.click();" onmouseover="this.bgColor='dodgerblue';" onmouseout="this.bgColor='lightskyblue';">
<input type="hidden" name="varseqlength" value="0">
<input type="hidden" name="newseq" value="<?php if($hascookies) print("0"); else print("1"); ?>" >
<input type="submit" name="submit" id="submit" style="border:3px double blue;font-weight:bold;width:100%;color:blue;background-color:dodgerblue;" onmouseover="this.style.backgroundColor='blue';this.style.color='white';" onmouseout="this.style.backgroundColor='dodgerblue';this.style.color='blue';" value="Get Entropic Profiles" disabled>
</td>
</tr>

</table>

</td>
</tr>
</table>
</form>

</center>
</body>
</html>
