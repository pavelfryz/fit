var bdosData = null;
var BDOS_MIN_WIDTH = 30; 
var BDOS_MIN_HEIGHT = 20; 

function initBDOS(){ //browser desktop operation system
	document.addEventListener('mousemove', mouseMove);
	document.addEventListener('mouseup', mouseUp);
	
	var i;
	var desktops = document.querySelectorAll(".desktop");
	for (i = 0; i < desktops.length; i++) {
		desktops[i].innerHTML = "<div class='workplace' data-z-index='0' data-fs='false'>" + desktops[i].innerHTML + "</div>";
	}
	
	var windows = document.querySelectorAll(".workplace>.window");
	for (i = 0; i < windows.length; i++) {
		//fill initial values
		windows[i].setAttribute("data-x",windows[i].offsetLeft);
		windows[i].setAttribute("data-y",windows[i].offsetTop);
		windows[i].setAttribute("data-width",windows[i].offsetWidth);
		windows[i].setAttribute("data-height",windows[i].offsetHeight);
		
		//wrap content of window
		windows[i].innerHTML = "<div class='content'>" + windows[i].innerHTML + "</div>";
		//create title of window, allowing moving of window
		var title = document.createElement('div');
		title.innerHTML = windows[i].getAttribute("data-title");
		title.setAttribute('class', 'title');
		windows[i].appendChild(title);
		//add minimize maximize button
		var minmax = document.createElement('div');
		minmax.setAttribute('class', 'minmax');
		windows[i].appendChild(minmax);
		//add resize controls
		var top = document.createElement('div');
		var bottom = document.createElement('div');
		var left = document.createElement('div');
		var right = document.createElement('div');
		var topleft = document.createElement('div');
		var topright = document.createElement('div');
		var bottomleft = document.createElement('div');
		var bottomright = document.createElement('div');
		top.setAttribute('class', 'top');
		bottom.setAttribute('class', 'bottom');
		left.setAttribute('class', 'left');
		right.setAttribute('class', 'right');
		topleft.setAttribute('class', 'topleft');
		topright.setAttribute('class', 'topright');
		bottomleft.setAttribute('class', 'bottomleft');
		bottomright.setAttribute('class', 'bottomright');
		windows[i].appendChild(top);
		windows[i].appendChild(bottom);
		windows[i].appendChild(left);
		windows[i].appendChild(right);
		windows[i].appendChild(topleft);
		windows[i].appendChild(topright);
		windows[i].appendChild(bottomleft);
		windows[i].appendChild(bottomright);
		title.addEventListener('mousedown', mouseDown);
		minmax.addEventListener('click',mouseClick);
		top.addEventListener('mousedown', mouseDown);
		bottom.addEventListener('mousedown', mouseDown);
		left.addEventListener('mousedown', mouseDown);
		right.addEventListener('mousedown', mouseDown);
		topleft.addEventListener('mousedown', mouseDown);
		topright.addEventListener('mousedown', mouseDown);
		bottomleft.addEventListener('mousedown', mouseDown);
		bottomright.addEventListener('mousedown', mouseDown);
	}
}
	
function mouseMove(e){
	if(bdosData){
		//moving of window
		if(bdosData.action == "move"){
			if(bdosData.window.parentNode.getAttribute("data-fs")=="true"){
				bdosData.window.parentNode.setAttribute("data-fs",false);
				bdosData.window.style.top = bdosData.window.getAttribute("data-y")+"px";
				bdosData.window.style.left = bdosData.window.getAttribute("data-x")+"px";
				bdosData.window.style.right = "unset";
				bdosData.window.style.bottom = "unset";
				bdosData.window.style.width = bdosData.window.getAttribute("data-width")+"px";
				bdosData.window.style.height = bdosData.window.getAttribute("data-height")+"px";
			}
			var x = Number(bdosData.window.getAttribute("data-x"))-bdosData.x+e.clientX;
			var y = Number(bdosData.window.getAttribute("data-y"))-bdosData.y+e.clientY;
			x = Math.max(x,0);
			x = Math.min(x,bdosData.window.parentNode.offsetWidth-bdosData.window.getAttribute("data-width"));
			y = Math.max(y,0);
			y = Math.min(y,bdosData.window.parentNode.offsetHeight-bdosData.window.getAttribute("data-height"));
			bdosData.window.style.top = y + "px";
			bdosData.window.style.left = x + "px";
		//resizing of window
		}else if(bdosData.action == "resize"){
			if(bdosData.window.parentNode.getAttribute("data-fs")=="true"){
				bdosData.window.parentNode.setAttribute("data-fs",false);
				bdosData.window.setAttribute("data-x",bdosData.window.offsetLeft);
				bdosData.window.setAttribute("data-y",bdosData.window.offsetTop);
				bdosData.window.setAttribute("data-width",bdosData.window.offsetWidth);
				bdosData.window.setAttribute("data-height",bdosData.window.offsetHeight);
				
				bdosData.window.style.top = 0;
				bdosData.window.style.left = 0;
				bdosData.window.style.right = "unset";
				bdosData.window.style.bottom = "unset";
				bdosData.window.style.width = bdosData.window.getAttribute("data-width")+"px";
				bdosData.window.style.height = bdosData.window.getAttribute("data-height")+"px";
			}
			var resize = bdosData.resize;
			if(resize=="right"||resize=="topright"||resize=="bottomright"){
				var width = Number(bdosData.window.getAttribute("data-width"))-bdosData.x+e.clientX;
				width = Math.max(width,BDOS_MIN_WIDTH);
				width = Math.min(width,bdosData.window.parentNode.offsetWidth-bdosData.window.getAttribute("data-x"));
				bdosData.window.style.width = width+"px";
			}
			if(resize=="bottom"||resize=="bottomleft"||resize=="bottomright"){
				var height = Number(bdosData.window.getAttribute("data-height"))-bdosData.y+e.clientY;
				height = Math.max(height,BDOS_MIN_HEIGHT);
				height = Math.min(height,bdosData.window.parentNode.offsetHeight-bdosData.window.getAttribute("data-y"));
				bdosData.window.style.height = height+"px";
			}
			if(resize=="left"||resize=="topleft"||resize=="bottomleft"){
				var x = Number(bdosData.window.getAttribute("data-x"))-bdosData.x+e.clientX;
				x = Math.max(x,0);
				x = Math.min(x,Number(bdosData.window.getAttribute("data-x"))+Number(bdosData.window.getAttribute("data-width"))-BDOS_MIN_WIDTH);
				var width =Number(bdosData.window.getAttribute("data-x"))+Number(bdosData.window.getAttribute("data-width"))-x;
				bdosData.window.style.width = width+"px";
				bdosData.window.style.left = x+"px";
			}
			if(resize=="top"||resize=="topleft"||resize=="topright"){
				var y = Number(bdosData.window.getAttribute("data-y"))-bdosData.y+e.clientY;
				y = Math.max(y,0);
				y = Math.min(y,Number(bdosData.window.getAttribute("data-y"))+Number(bdosData.window.getAttribute("data-height"))-BDOS_MIN_HEIGHT);
				var height =Number(bdosData.window.getAttribute("data-y"))+Number(bdosData.window.getAttribute("data-height"))-y;
				bdosData.window.style.height = height+"px";
				bdosData.window.style.top = y+"px";
			}
			
		}
	}
	e.preventDefault();
}

function mouseClick(e){
	var window = this.parentNode;
	var desktop = window.parentNode;
	var z = desktop.getAttribute("data-z-index");
	desktop.setAttribute("data-z-index",++z);
	var fs = desktop.getAttribute("data-fs");
	window.style.zIndex = z;
	// maximize/minimize functionality
	if(fs=="false"){
		window.style.top = 0;
		window.style.left = 0;
		window.style.right = 0;
		window.style.bottom = 0;
		window.style.width = "unset";
		window.style.height = "unset";
		desktop.setAttribute("data-fs",true);
	}else{
		window.style.top = window.getAttribute("data-y")+"px";
		window.style.left = window.getAttribute("data-x")+"px";
		window.style.right = "unset";
		window.style.bottom = "unset";
		window.style.width = window.getAttribute("data-width")+"px";
		window.style.height = window.getAttribute("data-height")+"px";
		desktop.setAttribute("data-fs",false);
	}
	e.preventDefault();
}

function mouseDown(e){
	var window = this.parentNode;
	var desktop = window.parentNode;
	var z = desktop.getAttribute("data-z-index");
	desktop.setAttribute("data-z-index",++z);
	window.style.zIndex = z;
	//initialize moving
	if(this.getAttribute("class")=="title"){
		bdosData = {};
		bdosData.action = "move";
		bdosData.window = window;
		bdosData.x = e.clientX;
		bdosData.y = e.clientY;
	//initialize resizing
	} else {
		bdosData = {};
		bdosData.action = "resize";
		bdosData.resize = this.getAttribute("class");
		bdosData.window = window;
		bdosData.x = e.clientX;
		bdosData.y = e.clientY;
	}
	e.preventDefault();
}

function mouseUp(e){
	if(bdosData){
		bdosData.window.setAttribute("data-x",bdosData.window.offsetLeft);
		bdosData.window.setAttribute("data-y",bdosData.window.offsetTop);
		bdosData.window.setAttribute("data-width",bdosData.window.offsetWidth);
		bdosData.window.setAttribute("data-height",bdosData.window.offsetHeight);
	}
	bdosData = null;
	e.preventDefault();
}
