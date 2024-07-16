import time
import os

from flask import Flask, render_template, send_from_directory
import time
import test_hardware
import test_firmware

app = Flask(__name__)

import logging

log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)

os.chdir("../")
dir = os.path.dirname(os.path.realpath(__file__))


def print_console_seperator():
    print(" -- Dualpanto tester -- ")


@app.route('/')
def index():
    return render_template('index.html')


@app.route("/upload_firmware/<firmware>")
def upload_firmware(firmware):
    print_console_seperator()
    try:
        if firmware == "hello_world":
            t = test_firmware.Basic()
            t.test_upload_firmware()
            yield '{"status": "ok"}'
        if firmware == "encoder_motor_automatic":
            t = test_hardware.Linkage()
            t.test_motor_encoder_combination()
            yield '{"status": "ok"}'
        if firmware == "sync":
            t = test_hardware.Linkage()
            t.test_sync()
            yield '{"status": "ok"}'
        if firmware == "line_wall":
            t = test_firmware.Haptics()
            t.test_line_wall()
            yield '{"status": "ok"}'
    except Exception as e:
        print(e)
        yield '{"status": "error"}'


@app.route('/static/<path:path>')
def send_static(path):
    return send_from_directory('static', path)


@app.route('/stream')
def generate_large_csv():
    def generate():
        with open("ui/server_output", "r+") as stdfile:
            while not stdfile.closed:
                line = stdfile.readline()
                if len(line) == 0:
                    time.sleep(0.01)
                    continue
                time.sleep(0.001)
                yield f"{line}\n"

    return app.response_class(generate(), mimetype='text/plain')


if __name__ == '__main__':
    app.run("localhost", 8080)
