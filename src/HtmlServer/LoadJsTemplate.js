////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2011/12/22	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////

var gAosDwdFiles = new Array();
var gAosOrderedFiles = new Array();
var gAosVpdJSFiles = new Array();
var gAosLastStartedIdx = new Array();
var gAosTotalJSToLoad = new Array();
var gAosTotalDwds = loadFileNum;
var gAosLastFileToStart = 0;

function AosDwdFileInfo(src, fname, elem, fileid)
{
	this.src = src;
	this.fname = fname;
	this.loaded = false;
	this.started = false;
	this.elem = elem;
	this.fileid = fileid;
}

function AosLoadJs(src, fname, fileid)
{
	var reloadJs = false;
	if (gAosDwdFiles[fileid])
	{
		if(gAosDwdFiles[fileid].reload)
		{
			reloadJs = true;
		}
		else
		{
			return;
		}
	}
	var scriptElem = document.createElement('script');
	scriptElem.src = src;
	if(reloadJs)
	{
		var ff = gAosDwdFiles[fileid];
		ff.order = gAosDwdFiles[fileid].reloadOrder;
		ff.elem = scriptElem;
		gAosOrderedFiles[ff.order] = ff;
	}
	else
	{
		var ff = new AosDwdFileInfo(src, fname, scriptElem, fileid);
		gAosDwdFiles[fileid] = ff;
		ff.order = gAosOrderedFiles.length;
		gAosOrderedFiles.push(ff);
	}
	document.getElementsByTagName('head')[0].appendChild(scriptElem);
	if(document.all)
	{
		scriptElem.onreadystatechange = function()
		{
			if(this.readyState == 4 ||
			this.readyState == 'complete' || 
			this.readyState == 'loaded')
			{
				AosCallback(fileid, fname);
			}
		}
	}
	else
	{
		scriptElem.onload = function()
		{
			AosCallback(fileid, fname);
		}
	}
}

function AosCallback(fileid, fname)
{
	var fileinfo = gAosDwdFiles[fileid];
	if (!fileinfo)
	{
		return;
	}
	fileinfo.loaded = true;
	var idx = fileinfo.order;
	if(!window[fname]) 
	{
		fileinfo.loaded = false;
		gAosDwdFiles[fileid].reload = true;
		gAosDwdFiles[fileid].reloadOrder = idx;
		gAosDwdFiles[fileid].reloadCount = gAosDwdFiles[fileid].reloadCount||0;
		if(gAosDwdFiles[fileid].reloadCount++ > 3)
		{
			if(window.console)console.log('****'+fname+'Js file download error!!!***');
			return;
		}
		AosLoadJs(fileinfo.src ,fileinfo.fname, fileid);
		return;
	}

	while (gAosLastFileToStart<idx)
	{
		var ff = gAosOrderedFiles[gAosLastFileToStart];
		if (!ff || !ff.loaded){return;}
		if (!ff.started)
		{
			window[ff.fname].call();
			ff.started = true;
		}
		gAosLastFileToStart++;
	}
	window[fname].call();
	fileinfo.started = true;
	gAosLastFileToStart = idx+1;
	while (gAosLastFileToStart < gAosTotalDwds)
	{
		var ff = gAosOrderedFiles[gAosLastFileToStart];
		if (!ff || !ff.loaded){return;}
		if (!ff.started)
		{
			window[ff.fname].call();
			ff.started = true;
		}
		gAosLastFileToStart++;
	}
	if (gAosLastFileToStart >= gAosTotalDwds)
	{
		AosStartApp();
	}
	if (fileinfo.listeners)
	{
		for (var i=0; i<fileinfo.listeners.length; i++){
			AosCheckFileLoaded(fileid, fileinfo.fname, 
			fileinfo.listeners[i], 
			fileinfo.orders[i]);
		}
	}
}

function AosAreAllJSLoaded(transid)
{
	var files = gAosVpdJSFiles[transid];
	if (!files){return;}
	for (var i=0; i<files.length; i++)
	{
		var ff = gAosDwdFiles[files[i]];
		if (!ff || !ff.started){return false;} 
	}
	AosStartApp_vpd(transid);
	gAosVpdJSFiles[transid] = [];
	return true;
}


function AosLoadJs_vpd(src, fname, fileid)
{
	var transid = aos_vpdtransid;
	var scriptElem = document.createElement('script');
	scriptElem.src = src;
	var ff = gAosDwdFiles[fileid];
	if(ff && ff[transid] && ff[transid].reload)
	{
		var order = gAosDwdFiles[fileid][transid].reloadOrder;
		ff.elem = scriptElem;
		gAosDwdFiles[fileid] = ff;
		gAosVpdJSFiles[transid][order] = fileid;
	}
	else
	{
		var order = gAosVpdJSFiles[transid].length;
		gAosVpdJSFiles[transid].push(fileid);
		if(ff)
		{
			if(!ff.listeners)
			{
				ff.listeners = new Array();
				ff.orders = new Array();
			}
			ff.listeners.push(transid);
			ff.orders.push(order);
			return;
		}
		var ff = new AosDwdFileInfo(src, fname, scriptElem);
		gAosDwdFiles[fileid] = ff;
	}

	document.getElementsByTagName('head')[0].appendChild(scriptElem);
	scriptElem.onreadystatechange = function()
	{
		if(this.readyState == 4 || 
		   this.readyState == 'complete' || 
		   this.readyState == 'loaded')
		{
			AosCallback_vpd(fileid, fname, transid, order);
		}
	}

	scriptElem.onload = function()
	{
		AosCallback_vpd(fileid, fname, transid, order);
	}
}

function AosCallback_vpd(fileid, fname, transid, order)
{
	var crt_ff = gAosDwdFiles[fileid];
	if (!crt_ff){return;}
	AosCheckFileLoaded(fileid, fname, transid, order);
	if (crt_ff.listeners)
	{
		for (var i=0; i<crt_ff.listeners.length; i++)
		{
			AosCheckFileLoaded(fileid, fname, crt_ff.listeners[i], crt_ff.orders[i]);
		}
	}
}

function AosCheckFileLoaded(fileid, fname, transid, order)
{
	var crt_ff = gAosDwdFiles[fileid];
	crt_ff.loaded = true;
	var crt_idx = order;
	var last_idx = gAosLastStartedIdx[transid];
	if(!window[fname])
	{
		crt_ff[transid] = {};
		gAosDwdFiles[fileid].loaded = false;
		crt_ff[transid].reload = true;
		crt_ff[transid].reloadOrder = order;
		gAosDwdFiles[fileid].reloadCount = gAosDwdFiles[fileid].reloadCount||0;
		if(gAosDwdFiles[fileid].reloadCount++>3)
		{
			if(window.console)
			{
				console.log('***********'+fname+'download error************');
			}
			return;
		}
		AosLoadJs_vpd(crt_ff.src ,crt_ff.fname, fileid);
		return;
	}

	while (last_idx < crt_idx)
	{
		var pos = gAosVpdJSFiles[transid][last_idx];
		var ff = gAosDwdFiles[pos];
		if (!ff || !ff.loaded)
		{
			gAosLastStartedIdx[transid] = last_idx;
			return;
		}
		if (!ff.started)
		{
			window[ff.fname].call();
			ff.started = true;
		}
		last_idx++;
	}

	window[fname].call();
	crt_ff.started = true;
	last_idx = crt_idx+1;
	var total = gAosTotalJSToLoad[transid];
	while (last_idx < total)
	{
		var pos = gAosVpdJSFiles[transid][last_idx];
		var ff = gAosDwdFiles[pos];
		if (!ff || !ff.loaded)
		{
			gAosLastStartedIdx[transid] = last_idx;
			return;
		}
		if (!ff.started)
		{
			window[ff.fname].call();
			ff.started = true;
		}
		last_idx++;
	}
	gAosLastStartedIdx[transid] = last_idx;
	if (last_idx >= total)
	{
		AosStartApp_vpd(transid);
		gAosVpdJSFiles[transid] = [];
	}
}

