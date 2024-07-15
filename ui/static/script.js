
function load_console_iframe(){
    document.getElementById("bottom").innerHTML = `<h4>Console Output</h4>
    <iframe class="console-iframe" id="console_iframe" src="/stream"></iframe>`
}

window.onload = load_console_iframe;

var console_patterns = {
    "Failed to connect to ESP32: Timed out waiting for packet header":
        "Push the upload button while uploading the firmware.",
    "Failed to connect to ESP32: Invalid head of packet":
        "Push the upload button while uploading the firmware.",
    "Could not configure port":
        "Check if you have connected the USB correctly and that you are not using a charging only cable.",
    "could not open port":
        "Check if you are using the correct COM port. \nCurrent COM port is: {serial_port}",
    // automatic encoder linkage test
    "start and end position are not aligning":
        "Check if the handles can move freely. If the handles move freely and one of the handles didn't end up in the default start position ask TA"
}

function upload_firmware(firmware){
    return fetch("/upload_firmware/" + firmware)
    .then(response => response.json())
    .then(data => {
        console.log(data);
        var console_out = document.getElementById("console_iframe").contentWindow.document.body.innerHTML;
        console_out = console_out.split("-- Dualpanto tester --").at(-1)

        for (const err in console_patterns){
            if (console_out.includes(err)){
                var msg = console_patterns[err];

                if (err.includes("{serial_port}")){
                    // ! serial port has to be in the console output
                    var serial_port_location = console_out.indexOf("Serial port");
                    var serial_port = console_out.substring(serial_port_location, serial_port_location + 100).split(/\s+/)[2];
                    console.log(serial_port)
                    msg.replace("{serial_port}", serial_port);
                }

                alert(msg);
            }
        }
        return data;
    });
}

function do_test(button, test_name){
    //const infoBox = document.getElementById('info-box');
    button.innerHTML = "Loading...";

    upload_firmware(test_name).then((res) => {
        if (res == null) {
            alert("Check if localhost server is running.");
        }

        if (res.status != "ok"){
            button.innerHTML = "Failed - Retry";
            button.style.backgroundColor = "rgb(187, 86, 86)";
        }
        else {
            button.innerHTML = "Success - Retry";
            button.style.backgroundColor = "#1c7520";
        }
    });



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
//    infoBox.innerHTML += '<h4>Console Output</h4><iframe class="console-iframe" id="console_iframe" src="/stream"></iframe>'
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
