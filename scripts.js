
var fadeintimer;
var fadeouttimer;
var colorfadetimer;
var tooltip;

function RGB2Hex(rgbarray){
	var hexred=Number(rgbarray[0]).toString(16).toUpperCase();
	var hexgreen=Number(rgbarray[1]).toString(16).toUpperCase();
	var hexblue=Number(rgbarray[2]).toString(16).toUpperCase();
	if(hexred.length<2) hexred="0"+hexred;
	if(hexgreen.length<2) hexgreen="0"+hexgreen;
	if(hexblue.length<2) hexblue="0"+hexblue;
	return ("#"+hexred+hexgreen+hexblue);
}

function CSS2RGB(rgbstring){
	var rgb = rgbstring.substring(4,rgbstring.length-1);
	rgb = rgb.split(",",3);
	rgb[0]=parseInt(rgb[0]);
	rgb[1]=parseInt(rgb[1]);
	rgb[2]=parseInt(rgb[2]);
	return rgb;
}

function Hex2RGB(hex){
	var red=parseInt(hex.substr(1,2),16);
	var green=parseInt(hex.substr(3,2),16);
	var blue=parseInt(hex.substr(5,2),16);
	return [red,green,blue];
}

function changeColor(id,rgbadd,step){
	var color = CSS2RGB(document.getElementById(id).style.backgroundColor);
	color[0]+=rgbadd[0];
	color[1]+=rgbadd[1];
	color[2]+=rgbadd[2];
	if(color[0]<0) color[0]=0;
	if(color[1]<0) color[1]=0;
	if(color[2]<0) color[2]=0;
	if(color[0]>255) color[0]=255;
	if(color[1]>255) color[1]=255;
	if(color[2]>255) color[2]=255;
	document.getElementById(id).style.backgroundColor=RGB2Hex(color);
	if(step>0) colorfadetimer = window.setTimeout("changeColor('"+id+"',["+rgbadd.toString()+"],"+(step-1)+")",100);
}

function colorFade(object,color){
	window.clearTimeout(colorfadetimer);
	var oldcolor = CSS2RGB(object.style.backgroundColor);
	var newcolor = Hex2RGB(color);
	var addcolor = [ (newcolor[0]-oldcolor[0]) , (newcolor[1]-oldcolor[1]) , (newcolor[2]-oldcolor[2]) ];
	addcolor[0]=Math.floor(addcolor[0]/10);
	addcolor[1]=Math.floor(addcolor[1]/10);
	addcolor[2]=Math.floor(addcolor[2]/10);
	changeColor(object.id.toString(),addcolor,10);
}

function changeOpacity(object,value){
	object.style.opacity = (value/100);
	object.style.MozOpacity = (value/100);
	object.style.KhtmlOpacity = (value/100);
	//object.style.filters.alpha.opacity = value;
}

function fadeIn(object,value){
	changeOpacity(object,value);
	if(value<100) fadeintimer = window.setTimeout("fadeIn(" + object.id + "," + (value+5) + ")",50);
}

function fadeOut(object,value){
	changeOpacity(object,value);
	if(value>0) fadeouttimer = window.setTimeout("fadeOut(" + object.id + "," + (value-5) + ")",50);
}

function followMouseMove(e) {
	var posx = 0;
	var posy = 0;
	if (!e) var e = window.event;
	if (e.pageX || e.pageY) {
		posx = e.pageX;
		posy = e.pageY;
	}
	else if (e.clientX || e.clientY) {
		posx = e.clientX + document.body.scrollLeft + document.documentElement.scrollLeft;
		posy = e.clientY + document.body.scrollTop + document.documentElement.scrollTop;
	}
	tooltip.style.left=posx;
	tooltip.style.top=posy;
}

function opentooltip(image){
	tooltip = document.getElementById('tooltip');
	if( tooltip == null ){
		tooltip = document.createElement('div');
		tooltip.setAttribute('name','tooltip');
		tooltip.setAttribute('id','tooltip');
		tooltip.style.position = 'absolute';
		tooltip.style.zIndex = '10';
		tooltip.style.border = '1px dotted blue';
		tooltip.style.background = 'white';
		tooltip.style.margin = '20px 10px';
		tooltip.style.padding = '5px';
		document.body.appendChild(tooltip);
	}
	tooltip.innerHTML = image.getAttribute('alt');
	//window.clearTimeout(fadeouttimer);
	tooltip.style.visibility = 'visible';
	changeOpacity(tooltip,0);
	fadeintimer = window.setTimeout("fadeIn(tooltip,0)",1000);
	document.onmousemove = followMouseMove;
}

function closetooltip(){
	window.clearTimeout(fadeintimer);
	fadeOut(tooltip,0);
	tooltip.style.visibility = 'hidden';
	document.onmousemove = null;
}

function setCookie(name,value,expiredate){
	var date = new Date();
	date.setTime(expiredate*1000);
	document.cookie = name + "=" + value + "; expires=" + (date.toGMTString()) + "; path=/";
}
