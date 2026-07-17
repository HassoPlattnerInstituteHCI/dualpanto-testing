# BIS

This dualpanto-testing repo is used for the first few weekly assignments, and generally for uploading firmware to dualpanto.

If you have issue about installing all requirements and this testing tool, **please first ask on Moodle.**
Often other students have already solved issue you have. 


## Requirements
For setup and requirements, consult this repo's [READEM.md](./Readme.md).

Remeber: In PyCharm, unit test can be run easily by clicking on them in the markdown preview of this repo's [READEM.md](./Readme.md).
![markdown](./resources/execute.jpg)



## Assignment Week7: kinematics and rendering wall
Assignment for Week7 is to write code of god object and kinematics partially, and (mainly) get familiar with uploading firmware and this testing workflow since we use the test flow a lot from this week.

We have two coding assignments this week. First, open `config.py` and set your COM port to connect dualpanto before beginning.

### #1 kinematics
If you properly code it, IT handle follow Me handle

1. make sure install ESP32 driver, IDE, and all python library on requirements.txt
2. go to `dualpant-testing/firmware/haptics/BIS week7 Kinematics/firmware/src/hardware/panto.cpp`,find `TODO BIS` and write code
3. `python -m unittest test_firmware.Kinematics.test_BIS_week7_kinematics`
4. if you feel the device has issue, follow [test flow (Mechanics and Hardware)](Readme.md)
5. take the video that linkage is synced when you intearct with ME handle.
6. and upload to dokuwiki project page

### #2 rendering wall
If you properly code it, you will find a thin wall in the midle of dualpanto intearcitve region.

1. make sure install ESP32 driver, IDE, and all python library on requirements.txt
2. go to `dualpant-testing/firmware/haptics/BIS week7 God Object/firmware/src/physics/godObject.cpp`,find `TODO BIS` and write code
3. `python -m unittest test_firmware.Haptics.test_BIS_week7_go`
4. if you feel the device has issue, follow [test flow (Mechanics and Hardware)](Readme.md)
5. take the video that you interacts with rendered wall
6. and upload to dokuwiki project page

## Assignment Week10: 
1. upload the original firmware to your DualPanto (#5 in Readme.md, run `python -m unittest test_firmware.UploadDualPantoFramework`)
3. checkout master branch at https://github.com/HassoPlattnerInstituteHCI/PantoDraw-Web
4. search for the TODO BIS and fill them in with the things you learned today
5. run dualdraw and draw a nice picture (see readme.md for instructions)
6. upload a video of your drawing to dokuwiki

## Claibrating Firmware

### Calibration (Instructions)
Before uploading the regular firmware to the device, we can upload `calibrationFirmware` to store some calibration information permanently to the device. For this, bring the handles to the closed position (as far back as possible), and rotate the end effectors into a distinct/memorable position, e.g. facing **precisely** towards you.

1. **Absolute encoders** The linkage encoders (on the big motors) have absolute positioning. When flashing the `calibrationFirmware`, the encoder values are immediately read and permanently stored to non-volatile storage. The device now remembers this position as the closed position. This means that from now on, the handles can be in any position on devive startup, and tracking still works. If no calibration data is present, the regular firmware expects the handles to be in closed position on every start.
2. **Determine the end effector gear ratios**
Unfortunately, there are at least four different end effector gear boxes in circulation at the moment. The `calibrationFirmware` can distinguish between them, and permanently save their type to non-volatile storage. For this, some human input is needed.
For each end effector:
- Rotate the end effector **three times** clockwise (looking from the top). Try to be very precise! Since the difference between gear ratios is rather small, precision is key here. Since the linkage calibration is already done, you can pull the handles out a bit for easier rotation.
- Rotate the end effector counterclockwise for about half a revolution. This signals to the firmware that the calibration is done.

Uploading calibration firmware:
`python -m unittest test_firmware.UploadDualPantoFramework.perform_calibration`

*Remember: The end effector encoders are relative, meaning they still need to be in a known position on startup to register correctly. However, this calibration ensures that the handle rotation has the correct speed.*

When you have finished the calibration, proceed to uploading the regular `firmware`.

## Uploading DualpantoFramework
From Week9, we will develop dualpanto app. sometimes you want to upload dualpanto famework again and again when...

1. you update dualpanto framework (e.g. you tune PID)
2. you plug dualpanto device into different PC
    - we observe dualpanto has some wall rendering issue when you connect dualpanto to other PC. e.g, you uplaod firmware from mac and connect to windows pc.
    - this is not always happend. but You need to keep this in mind.
    - We haven't known why so. Welcome you to contribute here!

`python -m unittest test_firmware.UploadDualPantoFramework.test_upload_dp_firmware`
