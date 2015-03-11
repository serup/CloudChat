/* ===================================================
 * changeimage.js v1.0.0
 * http://www.scanva.com
 * Read more: http://forum.codecall.net/topic/36057-javascripttutorial-mouseover-image-change/#ixzz2Crfmc8Q9
 * ===================================================
 * Copyright 2012 SCANVA, ApS.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================== */

if (document.images)
{
         taxion= new Image(100,100);
         taxion.src="../img/IconTaxi_on.png";  
         taxioff= new Image(100,100);
	 taxioff.src="../img/IconTaxi_off.png";

         dispatcheron= new Image(100,100);
         dispatcheron.src="../img/IconDispatcher_on.png";  
         dispatcheroff= new Image(100,100);
         dispatcheroff.src="../img/IconDispatcher_off.png";

         dispatcherproon= new Image(100,100);
         dispatcherproon.src="../img/IconDispatcherpro_on.png";  
         dispatcherprooff= new Image(100,100);
	 dispatcherprooff.src="../img/IconDispatcherpro_off.png";

}


function change(imgName)
{
   if (document.images)
   {
         imgOn=eval(imgName + "on.src");
         document[imgName].src= imgOn;
   }
}


function changeback(imgName)
{
   if (document.images)
   {
                imgOff=eval(imgName + "off.src");
                document[imgName].src= imgOff;
   }
//   document.getElementById("iconinfo").innerHTML="<h1 class='offset2' style='position:fixed;width:60%;margin-top:220px;margin-bottom:0px;text-align:center'><small>Handling your resources with high-performance hardware <br> and easy-to-use software service solutions</small></h1>";
//   document.getElementById("iconinfo").innerHTML="<h1 class='span7' style='margin-top:40px;margin-bottom:0px;text-align:center'><small>Handling your resources with high-performance hardware and easy-to-use software service solutions</small></h1>";
   document.getElementById("iconinfo").innerHTML="<h1 class='offset1' style='position:relative;text-align:center'><small>Handling your resources with high-performance hardware and easy-to-use software service solutions</small></h1>";

}

$("#product-taxi-salary").mousemove(function(event) {
  var msg = "<p style='color:green'>SALARY<b style='color:black'>PLAN</b></p>Unlimited, automatic vehicle data retrieval for all your drivers. Creating salary notes based on driver activity ";
  //document.getElementById("iconinfo").innerHTML="<div class='offset3'><h1 style='position:fixed;width:10%;margin-top:220px;margin-bottom:0px;text-align:center'><small style='position:fixed;text-align:center'>" + "  " + msg + "</small></h1></div>";
  //document.getElementById("iconinfo").innerHTML="<h1 class='span8' style='margin-top:40px;text-align:center'><small style='text-align:center'>" + "  " + msg + "</small></h1>";
  document.getElementById("iconinfo").innerHTML="<h1 class='offset1' style='position:relative;text-align:center'><small style='text-align:center'>" + "  " + msg + "</small></h1>";
});

$("#product-dispatcher").mousemove(function(event) {
   var msg = "<p style='color:green'>DISPATCHER<b style='color:black'>PLAN</b></p>Secure dispatcher service solution for small and medium-sized dispatcher centrals ";
  //document.getElementById("iconinfo").innerHTML="<div class='offset3'><h1 style='position:fixed;width:10%;margin-top:220px;margin-bottom:0px;text-align:center'><small style='position:fixed;text-align:center'>" + "  " + msg + "</small></h1></div>";
  //document.getElementById("iconinfo").innerHTML="<h1 class='span7' style='margin-top:40px;text-align:center'><small style='text-align:center'>" + "  " + msg + "</small></h1>";
  document.getElementById("iconinfo").innerHTML="<h1 class='offset1' style='position:relative;text-align:center'><small style='text-align:center'>" + "  " + msg + "</small></h1>";
});

$("#product-dispatcher-pro").mousemove(function(event) {
   var msg = "<p style='color:green'>DISPATCHER<b style='color:black'>PLAN</b>pro</p>Multiple central dispatcher service, with software and cloud backup solutions ";
  //document.getElementById("iconinfo").innerHTML="<div class='offset3'><h1 style='position:fixed;width:10%;margin-top:220px;margin-bottom:0px;text-align:center'><small style='position:fixed;text-align:center'>" + "  " + msg + "</small></h1></div>";
  //document.getElementById("iconinfo").innerHTML="<h1 class='span7' style='margin-top:40px;text-align:center'><small style='text-align:center'>" + "  " + msg + "</small></h1>";
  document.getElementById("iconinfo").innerHTML="<h1 class='offset1' style='position:relative;text-align:center'><small style='text-align:center'>" + "  " + msg + "</small></h1>";
});

