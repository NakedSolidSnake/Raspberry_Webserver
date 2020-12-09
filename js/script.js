function drawCircle(color){
    var canvas = document.getElementById('LED_canvas');
    var context = canvas.getContext('2d');
    var centerX = canvas.width / 2;
    var centerY = canvas.height / 2;
    var radius = 70;

    context.beginPath();
    context.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
    context.fillStyle = color;
    context.fill();
    context.lineWidth = 5;
    context.strokeStyle = '#888888';
    context.stroke();
}

function print(message){
    // console.log("Message: " + message);
    var i;
    for (i = 0; i < message.length; i++) {
        console.log(message.elements[i].value);
    }
}

function sendLED(endpoint){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
       // Typical action to be performed when the document is ready:
            let color;
            console.log(this.responseText);
            if(this.responseText === '1'){
                color = 'red';
            }else if(this.responseText === '0'){
                color = 'gray';
            }
            
            drawCircle(color);
            // document.getElementById("demo").innerHTML = xhttp.responseText;            
        }
    };
    xhttp.open("GET", endpoint, true);
    xhttp.send();
}





