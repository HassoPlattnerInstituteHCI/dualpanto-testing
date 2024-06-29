
function test_upload_firmware(){
    return fetch('/upload_firmware')
    .then(response => response.json())
    .then(data => {
        console.log(data);
        var console_out = document.getElementById("console_iframe").contentWindow.document.body.innerHTML;
        console_out = console_out.split("-- Dualpanto tester --").at(-1)
        if (console_out.includes("Failed to connect to ESP32: Timed out waiting for packet header")){
            alert("Check if you pushed the upload button. ");
        }
        if (console_out.includes("Could not configure port")){
            alert("Check if you have connected the USB correctly and that you are not using a charging only cable.");
        }
        if (console_out.includes("could not open port")){
            var serial_port_location = console_out.indexOf("Serial port");
            var serial_port = console_out.substring(serial_port_location, serial_port_location + 100).split(/\s+/)[2];
            console.log(serial_port)
            alert("Check if you are using the correct COM port. \nCurrent COM port is: " + serial_port);
        }
        return data;
    });
}

function do_test(button, test_name){
    //const infoBox = document.getElementById('info-box');
    button.innerHTML = "Loading...";

    switch (test_name){
        case "upload_firmware":
            test_upload_firmware()
            .then((res) => {
            if (res.status != "ok"){
                button.innerHTML = "Failed - Retry";
                button.style.backgroundColor = "rgb(187, 86, 86)";
            }
            else {
                button.innerHTML = "Success - Retry";
                button.style.backgroundColor = "#1c7520";
            }
            });
        break;
    }

    //infoBox.innerHTML = `<h2>${item}</h2><p>More detailed information about ${item}.</p>`;
}

function showInfo(test_name) {
    const infoBox = document.getElementById('info-box');
    infoBox.innerHTML = "";
     switch (test_name){
        case "upload_firmware":
            infoBox.innerHTML = "<h2>Upload firmware test</h2>"
            infoBox.innerHTML +=`<h4>Troubleshooting steps</h4><p> 1. Check if USB is connected<br>
            2. Check if the correct COM Port is set <(more info)> <br>
            3. Are you pushing the correct button of the dualpanto? </p>`
        break;
    }
    infoBox.innerHTML += '<h4>Console Output</h4><iframe class="console-iframe" id="console_iframe" src="/stream"></iframe>'
}

function simple_hash(s) {
  return s.split("").reduce(function(a, b) {
    a = ((a << 5) - a) + b.charCodeAt(0);
    return a & a;
  }, 0);
}


var last_content_hash = "";

setInterval(function(){
    try {
         var iframe = document.getElementById("console_iframe");
         var new_content_hash = simple_hash(iframe.contentWindow.document.body.innerHTML);
         if (last_content_hash != new_content_hash) {
            iframe.contentWindow.scrollTo(0, iframe.contentDocument.body.scrollHeight);
         }
         last_content_hash = new_content_hash;
         } catch { };
} ,100);
