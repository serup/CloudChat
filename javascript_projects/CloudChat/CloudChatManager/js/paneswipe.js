var navibar_selection = 1;
var tip=8; // less than 8 means that a tip of the pane can be seen by user

//TODO: somehow move works, however panes are not staying in same place vertically - hmm!
//function animatepane(oldpage,oldpane,pane,strnavibar)
function animatepane(i)
{
        var strnavibar="nav_"+i;
        var strpage="page_"+i;
        var strpane="#"+strpage;
        var oldi = i-1;
        if(oldi <= 0) oldi=6;
        //var oldpage="page_"+oldi;
        var oldpane="#page_"+oldi;
    

//        // now move current pane left and at almost same time move new pane in from right
//	setTimeout(function(){
//                setTimeout (function(){// show page at start moving position - all the way right
//                    move(strpane).x(+((($(window).width())/8)*tip)).end();
//                }, .0700);
//                setTimeout(function(){// move current pane left 
//                    move(oldpane).x(-($(window).width())).end();
//                }, .0700); 
//                setTimeout(function(){// move next pane in from right to left
//                    document.getElementById(strpage).style.display = "inline"; 
//                    move(strpane).x('0').end();
//        	}, .0700); 
//		setTimeout(function(){// set the current navi pane and reset old pane displacement
//	  		NaviBar(strnavibar);
//      			move(oldpane).x('0').end(); // reset position
//		}, 800); 
//	}); 
     
// TEST with new jQuery    
//// NB! only works with jQuery UI 1.9.2 - or above        
// var $div1 = $('#div1'), 
//    $div2 = $('#div2'),
//    currentDiv = 'div1';
//
//$div2.hide();
//
//    $div1.toggle('slide', {direction: 'right'}, 'slow');
//    $div2.toggle('slide', {direction: 'left'}, 'slow');
//    
//    currentDiv = (currentDiv === 'div1') ? 'div2' : 'div1';
     
// this works with jQuery 1.10.4  - however still panes are moved strangely
var $div1 = $(oldpane), 
    $div2 = $(strpane);
    
    $div1.toggle('slide', {direction: 'left'}, 'slow');
    $div2.toggle('slide', {direction: 'right'}, 'slow');
}

var currentoffset=0;
function init_paneswipe()
{
    var i = navibar_selection;
    x$("#main_body").swipe(
	function(e, data){
	  	   i = navibar_selection;

		    if (data.direction == 'right') {
			    i=i-1;
			    if(i <= 0) i=6;

			    NaviBar("nav_"+ i); // circle backwards thru panes
			    navibar_selection = i; 
		    }
		    if (data.direction == 'left')
		    {
			    oldi = i;
			    i=i+1;
			    if(i > 6) i=1;
			    //animatepane(i);

			    // now set it -- only if above does not work
			    NaviBar("nav_"+i); // circle forward thru panes
		    }
/* disabled[uid:1234321] - due to poor performance, however it did make it possible for avoiding the scrolling bounce on iphone -- TODO: find a better way - suggestion, perhaps detect boundary area and only then preventDefault 

            if (data.direction == 'down')
            {
//                window.scrollBy(0,-50);
                var offset=window.scrollY+data.deltaY*100;
                $('html,body').animate({scrollTop: offset},200);
                // somehow chat part does not react to above, thus this
                var offset2=$('#chatarea').scrollTop()+data.deltaY*10;
                $('#chatarea').animate({scrollTop: offset2},200);
            }
            if (data.direction == 'up')
            {
//                window.scrollBy(0,50);
                var offset=window.scrollY+data.deltaY;
                $('html,body').animate({scrollTop: offset},200);
                // somehow chat part does not react to above, thus this
                var offset2=$('#chatarea').scrollTop()+data.deltaY*10;
                $('#chatarea').animate({scrollTop: offset2},200);
            }
 */
	}, {
        swipeCapture: true,
        longTapCapture: true,
        doubleTapCapture: true,
        simpleTapCapture: false,
        preventDefaultEvents: false	
    }
    );

}

