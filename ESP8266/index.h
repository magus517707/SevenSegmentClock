const char webpage[] = R"=====(
<!DOCTYPE html>
<html>

<head>

  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <!-- PAGE settings -->
  <link rel="icon" href="https://cloud.unambitio.us/apps/sharingpath/magus517707/USF/Spring'21/Make%20Course/clock/wifi/page/favicon.ico">
  <title>Kinetic 7-Segment Clock</title>

  <!-- CSS dependencies -->
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css" type="text/css">
  <link rel="stylesheet" href="https://cloud.unambitio.us/apps/sharingpath/magus517707/USF/Spring'21/Make%20Course/clock/wifi/page/wireframe.css">
</head>
<script>
function toggle(id){
    var div = document.getElementById(id);
    div.style.display = div.style.display == "none" ? "block" : "none";
    var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
    }
  };
  xhttp.open("GET", "toggle", true);
  xhttp.send();
}

function send(q){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
    }
  };
  var val =99;
  switch(q){
    case 0:
      val = document.getElementById("Hval").value;
      break;
    case 1:
      val = document.getElementById("Mval").value;
      break;
    case 2:
      val = document.getElementById("Sval").value;
      break;
    case 3:
      var h = document.getElementById("Hval").value;
      var m = document.getElementById("Mval").value;
      var s = document.getElementById("Sval").value;
      xhttp.open("GET", "set_all?h="+h+"&m="+m+"&s="+s, true);
      xhttp.send();
      return;
  }
  if(val != 99){
    xhttp.open("GET", "set_time?state="+q+"&val="+val, true);
    xhttp.send();
  }
}

function reset(){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
    }
  };
  xhttp.open("GET", "reset", true);
  xhttp.send();
}

setInterval(function(){getData();}, 1000);

function getData(){
  var state = document.getElementById('tuning').style.display;
  if(state == 'none'){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("clock").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "get_time", true);
  xhttp.send();
}
}

function get_angle(){
  var MIN_ANGLE = 0.0;
  var MAX_ANGLE = 180.0;
  var SERVO_MAX = 500;
  var SERVO_MIN = 110;
  var angle = document.getElementById('rangeInput').value;
  var value = (angle - MIN_ANGLE) * (SERVO_MAX - SERVO_MIN) / (MAX_ANGLE - MIN_ANGLE) + SERVO_MIN;
  return Math.round(value);
}

function consolidate(){
var bt = [0,0,0,0];
var cbh10= document.getElementsByName('H10');
var cbh1= document.getElementsByName('H1');
var cbm10= document.getElementsByName('M10');
var cbm1= document.getElementsByName('M1');
var digits = [cbh10,cbh1,cbm10,cbm1];
  for (var i = 0; i < 4; i++){
		for(var cb of digits[i]){
    if(cb.checked){
    //console.log(parseInt(cb.value));
    	bt[i] += parseInt(cb.value);
      }
    }
    //console.log(bt[i]);
  }
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
    }
  };
  xhttp.open("GET", "set_servo?h10="+bt[0]+"&h1="+bt[1]+"&m10="+bt[2]+"&m1="+bt[3]+"&ang="+get_angle(), true);
  xhttp.send();
}
function check_all(v){
  switch(v){
    case 0:
      var checkboxes = document.getElementsByName('H10');
      break;
    case 1:
      var checkboxes = document.getElementsByName('H1');
      break;
    case 2:
      var checkboxes = document.getElementsByName('M10');
      break;
    case 3:
      var checkboxes = document.getElementsByName('M1');
      break;
  }
  for (var checkbox of checkboxes) {
    checkbox.checked = !checkbox.checked
  }
}
</script>
<body class="text-center bg-dark" >
  <div class="p-5 d-flex flex-column h-25" style="">
    <div class="container mb-auto">
      <div class="row">
        <div class="mx-auto col-md-6">
          <nav class="navbar navbar-expand-md navbar-dark text-center">
            <div class="container">
              <a class="navbar-brand" onclick="reset()"><b class="text-center">Kinetic 7-Segment Clock</b></a>
            </div>
          </nav>
        </div>
        <div class="col-md-3" style=""><button type="button" class="btn btn-warning text-body btn-lg border border-dark d-flex justify-content-center align-items-center flex-grow-0" onclick="toggle('tuning')" style="">Toggle Clock (Tuning Mode)</button></div>
      </div>
      </div>
      <div class="row">
        <div class="col-md-12">
          <h5 class="">Current Time on Clock:</h5>
          <h1 class="display-4"><span id = "clock">00:00:00</span></h1>
        </div>
      </div>
    </div>
    <div class="container">
      <div class="row">
        <div class="col-md-8 mx-auto ">
          <br>
          <br>
          <h1 class="cover-heading"><b>Set The Time Here:</b></h1>
        </div>
      </div>
      <div class="row">
        <div class="col-md-4" style="">
          <form class="" id="Hour">
            <div class="form-group" style=""> <label>Hour</label> <input type="number" class="form-control" id = Hval> <small class="form-text text-muted"></small> </div>
            <button type="button" class="btn btn-dark border border-top-0 border-bottom-0 border-warning text-white btn-lg" id="Hour_sub" onclick="send(0)">Submit</button>
          </form>
        </div>
        <div class="col-md-4" style="">
          <form class="" id="Minute">
            <div class="form-group" style=""> <label>Minute</label> <input type="number" class="form-control" id = Mval> <small class="form-text text-muted"></small> </div>
            <button type="button" class="btn btn-dark border border-top-0 border-bottom-0 border-warning text-white btn-lg" id="Min_sub" onclick="send(1)">Submit</button>
          </form>
        </div>
        <div class="col-md-4" style="">
          <form class="" id="Second">
            <div class="form-group" style=""> <label>Second</label> <input type="number" class="form-control" id = Sval> <small class="form-text text-muted"></small> </div>
            <button type="button" class="btn btn-dark border border-top-0 border-bottom-0 border-warning text-white btn-lg" id="Sec_sub" onclick="send(2)">Submit</button>
          </form>
        </div>
      </div>
      <div class="row">
        <div class="col-md-12"><button type="button" class="btn btn-dark border border-warning text-white btn-block my-2" id="All_sub" onclick="send(3)">Submit All</button></div>
      </div>
    </div>
    <div class="p-3 d-flex flex-column h-75" style="">
    <div class="container" id="tuning" style="display:none">
      <div class="col-md-8 mx-auto">
        <h6 class="cover-heading"><b>Servo Angle Control in Degrees</b></h6>
        <form>
          <div>
            <input id="rangeInput" value="0" type="range" min="0" max="180" step="0.1" oninput="amount.value=rangeInput.value"/>
            <input id="amount" type="number" value="0" min="0" max="180" step="0.1" oninput="rangeInput.value=amount.value" />
            <button type="button"  class="btn btn-dark border border-warning text-white btn-block my-2" onclick="consolidate()">Send Angle</button>
          </div>
        </form>
        </div>
      <div class="row">
        <div class="col-md-3" style="">
          <div class="well">Hr (Tens)
            <br>
            <div>
              <div>
                <input type="checkbox" id="H10_A" name="H10" value=64>
                <label>A</label>
              </div>
              <div>
                <input type="checkbox" id="H10_B" name="H10" value=32>
                <label>B</label>
              </div>
              <div>
                <input type="checkbox" id="H10_C" name="H10" value=16>
                <label>C</label>
              </div>
              <div>
                <input type="checkbox" id="H10_D" name="H10" value=8>
                <label>D</label>
              </div>
              <div>
                <input type="checkbox" id="H10_E" name="H10" value=4>
                <label>E</label>
              </div>
              <div>
                <input type="checkbox" id="H10_F" name="H10" value=2>
                <label>F</label>
              </div>
              <div>
                <input type="checkbox" id="H10_G" name="H10" value=1>
                <label>G</label>
              </div>  
              <div>
                <input type="checkbox" id="select-all-H10" onclick="check_all(0)" on>
                <label>Select All</label>
              </div>
            </div>
          </div> 
        </div>
        <div class="col-md-3" style="">
          <div class="well">Hr (Ones)
            <br>
            <div>
              <div>
                <input type="checkbox" id="H1_A" name="H1" value=64>
                <label>A</label>
              </div>
              <div>
                <input type="checkbox" id="H1_B" name="H1" value=32>
                <label>B</label>
              </div>
              <div>
                <input type="checkbox" id="H1_C" name="H1" value=16>
                <label>C</label>
              </div>
              <div>
                <input type="checkbox" id="H1_D" name="H1" value=8>
                <label>D</label>
              </div>
              <div>
                <input type="checkbox" id="H1_E" name="H1" value=4>
                <label>E</label>
              </div>
              <div>
                <input type="checkbox" id="H1_F" name="H1" value=2>
                <label>F</label>
              </div>
              <div>
                <input type="checkbox" id="H1_G" name="H1" value=1>
                <label>G</label>
              </div>  
              <div>
                <input type="checkbox" id="select-all-H1" onclick="check_all(1)">
                <label>Select All</label>
              </div>
            </div>
          </div> 
        </div>
        <div class="col-md-3" style="">
          <div class="well">Min (Tens)
            <br>
            <div>
              <div>
                <input type="checkbox" id="M10_A" name="M10" value=64>
                <label>A</label>
              </div>
              <div>
                <input type="checkbox" id="M10_B" name="M10" value=32>
                <label>B</label>
              </div>
              <div>
                <input type="checkbox" id="M10_C" name="M10" value=16>
                <label>C</label>
              </div>
              <div>
                <input type="checkbox" id="M10_D" name="M10" value=8>
                <label>D</label>
              </div>
              <div>
                <input type="checkbox" id="M10_E" name="M10" value=4>
                <label>E</label>
              </div>
              <div>
                <input type="checkbox" id="M10_F" name="M10" value=2>
                <label>F</label>
              </div>
              <div>
                <input type="checkbox" id="M10_G" name="M10" value=1>
                <label>G</label>
              </div>  
              <div>
                <input type="checkbox" id="select-all-M10" onclick="check_all(2)">
                <label>Select All</label>
              </div>
            </div>
          </div> 
        </div>
        <div class="col-md-3" style="">
          <div class="well">Min (Ones)
            <br>
            <div>
              <div>
                <input type="checkbox" id="M1_A" name="M1" value=64>
                <label>A</label>
              </div>
              <div>
                <input type="checkbox" id="M1_B" name="M1" value=32>
                <label>B</label>
              </div>
              <div>
                <input type="checkbox" id="M1_C" name="M1" value=16>
                <label>C</label>
              </div>
              <div>
                <input type="checkbox" id="M1_D" name="M1" value=8>
                <label>D</label>
              </div>
              <div>
                <input type="checkbox" id="M1_E" name="M1" value=4>
                <label>E</label>
              </div>
              <div>
                <input type="checkbox" id="M1_F" name="M1" value=2>
                <label>F</label>
              </div>
              <div>
                <input type="checkbox" id="M1_G" name="M1" value=1>
                <label>G</label>
              </div>  
              <div>
                <input type="checkbox" id="select-all-M1" onclick="check_all(3)">
                <label>Select All</label>
              </div>
            </div>
          </div> 
        </div>
        </div>
      </div>
      </div>
    </div>
  </div>
  <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js" integrity="sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49" crossorigin="anonymous"></script>
  <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js" integrity="sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy" crossorigin="anonymous" style=""></script>
</body>

</html>
)=====";