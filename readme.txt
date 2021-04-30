This project was created using WSL ubuntu 2018 and python 3.6.
Using mbed compiler with ubuntu 20.04 and ubuntu 18.94 python 2.7 caused errors.

This program requires the mbed-os version 5 works with the provided cadmium.json and mbed-app.json file.
However mbed-os version 6 breaks these files and mbed compile will not work.

To get mbed-os version 5. 
on bash terminal run:

git clone https://github.com/ARMmbed/mbed-os.git
cd ./mbed-os
git submodule update --init --recursive
git checkout 3801d4a1c3


To install the cadmium dependencies use the install.sh script provided.
To run the script in project root:

1- Open Bash terminal
2- Type ./install.sh


FOLDER STRUCTURE:

.
├── CAN_Controller
│   ├── atomics
│   │   ├── CAN_controller.hpp
│   │   └── float_to_internal.hpp
│   ├── build
│   ├── cadmium.json
│   ├── mbed_app.json
│   ├── mbed_settings.py
│   ├── mbed_settings.pyc
│   ├── test
│   │   ├── can_output.txt
│   │   ├── test.txt
│   │   ├── test1.txt
│   │   └── test_main.cpp
│   └── top_model
│       ├── main.cpp
│       └── makefile
├── Display
│   ├── atomics
│   │   ├── CAN_controller.hpp
│   │   ├── LCD_display.hpp
│   │   └── can_input.hpp
│   ├── build
│   ├── cadmium.json
│   ├── mbed_app.json
│   ├── test
│   │   ├── inputs
│   │   │   └── test.txt
│   │   ├── output
│   │   │   └── lcd_out.txt
│   │   └── test_main.cpp
│   └── top_model
│       ├── main.cpp
│       └── makefile
├── Makefile
├── Motor
│   ├── atomics
│   │   ├── CAN_controller.hpp
│   │   └── Motor.hpp
│   ├── build
│   ├── cadmium.json
│   ├── mbed_app.json
│   ├── test
│   │   ├── input
│   │   │   └── test.txt
│   │   ├── output
│   │   │   └── motor_out.txt
│   │   └── test_main.cpp
│   └── top_model
│       ├── inputs
│       ├── main.cpp
│       ├── makefile
│       └── output
│           └── can_out.txt
├── Project_report.docx
├── data_structures
│   ├── can_structure.hpp
│   ├── internal_structure.hpp
│   └── test_input.hpp
├── drivers
│   ├── CAN_driver.hpp
│   └── LCD.hpp
├── install.sh
├── lib
│   └── TextLCD
│       ├── TextLCD.cpp
│       ├── TextLCD.h
│       ├── TextLCD_Config.h
│       ├── TextLCD_UDC.h
│       ├── TextLCD_UDC.inc
│       └── TextLCD_UTF8.inc
├── logs
│   ├── LCDlog.txt
│   ├── Motor_log.txt
│   └── potentiometer_out log.txt
└── readme.txt


The CAN_Controller, Motor and Display directories containt the model for the respective node.
    
    -The CAN_Controller directory contains the model for the analog input and CAN controller code
    and testing. 
    -The Motor directory contains the model for the Motor and Can transmission and recieving code.
    -The Display directory contains the model for the LCd display and reading from CAN bus. 

The logs directory contains the logs obtained from actual hardware simulations.

The software simulations code is contained in the test folder for each of the nodes. The test folder contains
some aditional code to allow the models to recieve inputs as they would in the proper hardware simulation.
Hence they are in a seperate folder and file.

The cadmium.json and mbed.json file are used by the mbed compiler to configure compilation settings. The 
configuration is set such that building the code will create a BUILD folder under the Motor,Display or CAN_Controller
containing the compiled code and binary files as required.

The data_structures directory contains data_structures used by the models. The can_structure.hpp is used as a stand in 
for the internal CANMessage structure used in mved-os and should only be used for simulations.

The drivers directory contains wrappers for the mbed-os api and is used to allow the libraries to be used in the models.

The Makefile in root folder calls all the Makefile in the subdirectories and can be used to make the projects.



COMPILING THE PROGRAM:

Missing from the folder structure above is the mbed-os folder. The mbed-os folder must be located in root folder to allow 
the .hpp, .cpp and Makefile to properly link the mbed-os libraries.

To compile the simulations:

    1- For Motor simulation type 
        make Motor
    
    2- For Display simulation type
        make Display
    3- For CAN_Controller simulation type
        make CAN
    
    These will create binary files in the respective test folder which can be run using:
        /(Directory Name of Path Compiled)/test/NUCLEO_F207Z

To compile the Harware binary files:

     1- For Motor simulation type 
        make MotorE
    
    2- For Display simulation type
        make DisplayE
    3- For CAN_Controller simulation type
        make CANE

As mentioned above the bin files will be located in
    BUILD/NUCLEO_F207Z/*.bin

To clean the folders run:

    make clean

To clean the BUILD folders run:

    make eclean


Note:
In case the install.sh does not execute due to it not being an executable.
In bash terminal type:

chmod +x install.sh



