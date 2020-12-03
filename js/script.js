window.onload = function(){
    document.getElementById("bt1").onclick = led_on;
    document.getElementById("bt2").onclick = led_off;
}

function draw(){
    var canvas = document.getElementById('LED_canvas');
    var ctx = canvas.getContext('2d');
    ctx.fillRect(25, 25, 100, 100);
}

function led_on(){
    xhr.open('GET', '/', true);
    xhr.setRequestHeader("Content-Type", "text/plain");    
    xhr.send('led_on')
}

function led_off(){
    alert("LED OFF")
}