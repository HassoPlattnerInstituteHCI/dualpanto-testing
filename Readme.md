# Dualpanto: Install Firmware & Testing

The purposes of this repository are 

1. to install the dualpanto framework firmware to the dualpanto devices, which is needed for running & developing apps/games (follow the setup section. the actual firmware installation is in step 5)
2. to provide semi-automatic testing for the dualpanto device. This involves basic functionality checks of the hardware, the haptic rendering, communication protocol and the unity integration. A test will install a different testing firmware, which overwrites the dualpanto framework firmware. (follow "If something doesn't work, run these tests")

This project is work-in-progress. Welcome to contribute.
### For BIS participants
Please check [BIS.md](BIS.md) first.


# Setup
## 1. Install the ESP32 driver (so your computer can communicate with the dualpanto)
- [Download](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads) the latest installer of the driver for your OS-Version
- Unzip and run the installer
  * Tips for windows: download "CP210x Universal Windows Driver," right-click on the silabser.inf file, and select _Install_
  * Tips for mac: mount the DMG file and double-click on the	Silicon Labs VCP Driver. If the installation is blocked, go to the _System Preferences Security & Privacy_ pane to	unblock the system extension

## 2. Setup C++ environment (so that C++ code of this repo can be compiled)

### macOS
 - Go to Appstore and install Xcode
 - Run `xcode-select –install` to install the compilers
 - Run `sudo xcode-select -s /Applications/Xcode.app/Contents/Developer`

### windows
 - Install Visual Studio 2019 or 2017 (you won't use this as your IDE, this is just to install the C++ dependency in the next step)
 - Select at least the workload “Desktopentwicklung mit C++”

## 3. Install IDE (this is where you will run the Dualpanto tests)
We highly recommend to use PyCharm to run this test flow, but you can also use VSCode or the command line.
### Alternative A: PyCharm (recommended, because your environment is guaranteed to work)
1. download [PyCharm](https://www.jetbrains.com/community/education/#students). As a student, you get a free license (click the "Apply now" button and complete the form).
2. clone this repo **(on Windows: dont clone into the WSL subsystem, it wont work!)** and open it with pycharm
3. [create virtual environment (we use python 3.7)](https://www.jetbrains.com/help/pycharm/creating-virtual-environment.html) and [install packages using requirements.txt](https://www.jetbrains.com/help/pycharm/managing-dependencies.html)
4. install [Mermaid](https://plugins.jetbrains.com/plugin/20146-mermaid) plugin for flowchart vis

if you chose PyCharm, you can run unittest from Markdown preview 

<img src="/resources/execute.jpg" width="50%" />

alternatively, you can still [execute unittest from scripts](https://www.jetbrains.com/help/pycharm/testing-your-first-python-application.html#create-test)

**Use the Terminal in PyCharm in further steps. Because only in the PyCharm terminal you will get your correct python virtual environment.**

### Alternative B: VScode (can work, but if you already configured your environment in a weird way it might be hard for us to debug)

1. download [VScode](https://code.visualstudio.com/)
2. install the Python extension for VSCode (https://marketplace.visualstudio.com/items?itemName=ms-python.python)
3. go into the extensions menu on the left -> press the settings gear on the Python extension -> select "Settings" -> type "terminal" into the search bar -> make sure that "Activate env in current terminal" and "Activate environment" are both checked. This will ensure that you are always in the correct environment.
4. **On Windows**, use the PowerShell or Command Prompt terminal inside VSCode in all following steps. Don't use the WSL subsystem.
5. clone this repo (**on Windows**: dont clone into the WSL subsystem, it wont work!) and open it with VScode
6. shift + command + P -> >Python: Create Environment -> Venv
7. **On windows:** if it says "No python installed", install python from the Microsoft Store (you can get there by clicking the "no python installed" warning). Then go back to VSCode and repeat from step 5.
8. select your python version (e.g. Python 3.12.6) -> select requirements.txt -> confirm
9. it will install for a while (as shown by a message window on the bottom right). When it's done, **open a new terminal**.
10. Check that it worked: Run `pip list` in the terminal, it should show you (among others) `pyserial` and `platformio` in the list.
11. If they dont show up, or you get python errors later such as "no module named serial": press shift + command + P -> Python: Select Interpreter -> select the interpreter which includes venv, e.g. `Python 3.12.6 ('.venv': venv)`. Then open a new terminal. Then try again. **You might have to redo step 10 every time you open VSCode.** If you don't see this interpreter in the list, redo the installation steps above from step 5.

### Alternative C: Command Line (hard, because you get no help with setup from the IDE and do everything yourself from the terminal)
you can also run all unittest from command line interface if struggle with setting up IDE. Install python library in requirements.txt, then execute script.

## 4. Install submodules (installs dependencies that are needed to run the tests)
- in a terminal, navigate to the dualpanto-testing folder which you cloned this repo into
- run `git submodule init`, then `git submodule update`

## 5. Uploading the firmware to the Dualpanto

You need to have the dualpanto connected to your computer to do this. Here is how it works:

1. plug the USB-A end of the USB cable into your computer. If you don't have USB-A (e.g. new laptop), use a USB-C to USB-A adapter.
2. plug the micro-USB-B end of the cable into the Dualpanto. The port is in the top left of the back of the device.
3. Find out your COM port (the COM port is basically the name that your computer gave to the connection to the dualpanto). How to find your COM port: **Windows** : go to device manager / geräte-manager → ports/anschüsse (COM & LPT) and check the com port (e.g. "//.//COM3"). **Mac**: use command `ls /dev/cu.*` in the terminal and look for e.g. "/dev/cu.SLAB_USBtoUART"
4. open `config.py` in the dualpanto-testing folder and put your COM port between the quotes in `COM_PORT = ""`. (@Shohei: is this the only file to be modified?)
5. **Start pressing this button** on the back of the device (image below).

![swith](./resources/dualpanto_switch.jpg)

7. In the terminal of your IDE (either PyCharm or VSCode with Python virtual environment), navigate to the dualpanto-testing folder and run this command: `python -m unittest test_firmware.UploadDualPantoFramework.test_upload_dp_firmware`. **Keep pressing the button** while this is running.
8. After the install is finished (terminal says OK), you can **release the button**.

If all of this worked, you did successfully setup the Dualpanto. If not, see the next section.

# If something doesn't work, run these tests

## Whenever you plug Dualpanto into a new PC do this:

Repeat step 5 (uploading the firmware).

known issues when you plug dualpanto device into different PC:
    - we observe dualpanto has some wall rendering issue when you connect dualpanto to other PC. e.g, you uplaod firmware from mac and connect to windows pc.
    - this is not always happend. but You need to keep this in mind.
    - We haven't known why so. Welcome you to contribute here!
    
Most of the test is not semi-automated since they are related to haptics and mechanical issue. Check `How to test?`.

**When you need to ask question on discord, please tell us which test you failed, and describe error or issue you have precise as possible.**

## Configurate test flow
2. All test upload a firmware for each unit-test. You probably need to push button back of dualpantowhen you upload a firmware (it depends on OS).
![swith](./resources/dualpanto_switch.jpg)
   
## 1. Mechanical and Hardware
(For BIS studetns: There are code to figure out whether encoder and motor works, but no good evaluation process yet. We will update asap. You can also modify code to test your purpose.)

0.  [check mechanical configuration](physical_test/checklist.md)
1. `python -m unittest test_firmware.Basic`
2. `python -m unittest test_hardware.Linkage.test_encoder`**WIP**
3. `python -m unittest test_hardware.Linkage.test_sync`**WIP**
4. `python -m unittest test_hardware.EndEffector.test_encoder` **WIP**
5. `python -m unittest test_hardware.EndEffector.test_motor` **WIP**

- *1.Basic* check whether you could compile firmware and upload firmaere with platfiormIO
- *2.Linkage.test_encoder* check the four encoders on top of big motors works fine
- *3.Linkage.test_sync* check the four big motors works fine
- *4.EndEffector.test_encoder* check the two encoders on end effectors works fine
- *5.EndEffector.test_sync* check the two motors on end effectors works fine

```mermaid
%%{init: {'theme': 'neutral' } }%%
flowchart TD;
    ST((start)) --> MC[0.mechanical];
    MC -- checked --> FB;
    
    subgraph  test_firmware
    FB[1.Basic];
    FB -- not work --> CH(check USB connection, <br> not using power-only cable, <br> COM port, <br> platformIO  properly installed,  <br> and pushing button back of dp)
    
    end
    
    FB -- work -->  LTE[2.test_Encoder]

    subgraph test_hardware.Linkage
    LTE -- work -->LTS[3.test_sync];
    LTE-- not work -->  MC1[check mechanical again]
    LTS -- not work --> MC1
    
    end
    
    subgraph test_hardware.EndEffector
    LTS -- work --> EETE[4.test_encoder]
    EETE -- work --> EETM[5.test_motor]
    EETM -- not work -->MC2
    EETE -- not work --> MC2[check mechanical again]
    EETM -- work --> DONE((done))
   
    
    end
    
    CH -- no idea --> TA((Ask on discord <br> #general channel))
    MC1 -- no idea --> TA
    MC2 -- no idea --> TA
    
    style TA fill:#FF9B00
    style DONE fill:#99CC00
    
    click LE "https://github.com/HassoPlattnerInstituteHCI/unity-dualpanto-toolkit/blob/develop/README.md"
    click TA "https://discord.com"
    click MC "https://discord.com"
```

## 2. Rendering Haptics = DualPanto Firmware

0. [check mechanical configuration](physical_test/checklist.md)
1. `python -m unittest test_firmware.Haptics.test_line_wall` **WIP**
2. `python -m unittest test_firmware.Kinematics.test_kinematics_sync` **WIP**
3. `python -m unittest test_firmware.Haptics.test_force_field` **WIP**
4. `python -m unittest test_firmware.Haptics.test_rail` **WIP**
5. `python -m unittest test_firmware.Haptics.test_rectangle_obstacle` **WIP**
6. `python -m unittest test_firmware.Haptics.test_diamond_obstacle` **WIP**
7. `python -m unittest test_firmware.Haptics.test_moving_obstacle` **WIP**

- *1.Haptics.test_line_wall* check dualpanto firmware can render simplest wall = line or not
- *2.Kinematics.test_kiematics_sync* check forward and inverse kinematics of dualpanto firmware works fine or not


## 3. Communication Protocol

**WIP**

## 4. Unity

**WIP**



# Feedback
This project is currenty under developement. 
For questions, please reach out to martin.taraz@hpi.de, shohei.katakura@hpi.de

For BIS students, please reach out shohei.katakura@hpi.de
