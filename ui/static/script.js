
function load_console_iframe(){
    document.getElementById("bottom").innerHTML = `<h4>Console Output</h4>
    <iframe class="console-iframe" id="console_iframe" src="/stream"></iframe>`
}

function window_onload(){
    load_console_iframe();
    console.log(document.getElementById("start-test"));
    showInfo(document.getElementById("start-test"), 'info_hello_world');
}

window.onload = window_onload;

var console_patterns = {
    "Failed to connect to ESP32: Timed out waiting for packet header":
        "Push the upload button while uploading the firmware.",
    "Failed to connect to ESP32: Invalid head of packet":
        "Push the upload button while uploading the firmware.",
    "Could not configure port":
        "Check if you have connected the USB correctly and that you are not using a charging only cable.",
    "could not open port":
        "Check if you are using the correct COM port. \nCurrent COM port is: {serial_port}",
    // automatic encoder linkage test#
    "check connection cable":
        "Check the cable which connects the dualpanto pcbs. It might be disconnected.",
    "start and end position are not aligning":
        "Check if the handles can move freely and if you moved the handles to the default position at the start. If the handles move freely and one of the handles didn't end up in the default start position there could be encoder drift",
    "handle didn't move far enough":
        "Handle didn't move far enough. Check if the handles can move freely. If they can there yould be a problem with the encoders",
    // sync
    "[Errno 16] Device or resource busy":
        "Com port: {serial_port} already in use. Check if another program already uses this com port or established a serial connection to the device."

}

function upload_firmware(firmware){
    load_console_iframe() //reload the iframe
    return fetch("/upload_firmware/" + firmware)
    .then(response => response.json())
    .then(data => {
        return new Promise((resolve, reject) => { setTimeout(function(){

        var console_out = document.getElementById("console_iframe").contentWindow.document.body.innerHTML;
        console_out = console_out.split("-- Dualpanto tester --").at(-1)

        for (const err in console_patterns){
            if (console_out.includes(err)){
                let msg = console_patterns[err];
                if (msg.includes("{serial_port}")){
                    // ! serial port has to be in the console output
                    var serial_port_location = console_out.indexOf("Serial port");
                    var serial_port = console_out.substring(serial_port_location, serial_port_location + 100).split(/\s+/)[2];
                    console.log(serial_port)
                    msg = msg.replace("{serial_port}", serial_port);
                }

                alert(msg);
            }
        }
        resolve(data);
    }, 500);});});
}

function do_test(button, test_name){
    //const infoBox = document.getElementById('info-box');
    button.innerHTML = "Loading...";

    upload_firmware(test_name).then((res) => {
        console.log(res);
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

function showInfo(button, test_name) {

    for (const list_item of document.getElementsByClassName("list-item")){
        list_item.style.backgroundColor = "#a5d6a7"
    }
    button.style.backgroundColor = "#4CAF50";

    for (const info_box_item of document.getElementsByClassName("info-box-item")){
        console.log(document.getElementsByClassName("info-box-item"), info_box_item)
        info_box_item.style.display = "none";
    }

    const infoBox = document.getElementById(test_name);
    infoBox.style.display = "block";
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
