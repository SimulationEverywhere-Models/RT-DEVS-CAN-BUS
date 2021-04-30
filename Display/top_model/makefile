CC=g++
CFLAGS=-std=c++17
COMPILE_TARGET=NUCLEO_F207ZG
EXECUTABLE_NAME=NUCLEO_F207ZG

INCLUDECADMIUM=-I ../../../cadmium/include
INCLUDEDESTIMES=-I ../../../cadmium/DESTimes/include

embedded:
	mbed compile --target $(COMPILE_TARGET) --toolchain GCC_ARM --profile ../cadmium.json --build ../BUILD  --source ../top_model --source ../../mbed-os --source ../../lib/TextLCD

flash:
	st-flash write ../BUILD/$(COMPILE_TARGET)/GCC_ARM-CADMIUM/*.bin 0x8000000
	$(info *** FLASH WILL TAKE ~15 Seconds! DO NOT RESET WHILE COM PORT LED IS FLASHING! ***)

all: main.o 
	$(CC) -g -o $(EXECUTABLE_NAME) ../build/main.o 

test: ../test/test_main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) ../test/test_main.cpp -o ../test/test_main.o
	$(CC) -g -o ../test/$(EXECUTABLE_NAME) ../test/test_main.o 



main.o: main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) main.cpp -o ../build/main.o




clean:
	rm -f $(EXECUTABLE_NAME) *.o *~
	rm -f ../test/$(EXECUTABLE_NAME) *.o *~
	rm -f ./*.o
	rm -f ../build/*.o
	rm -f ../test/*.o

eclean:
	rm -rf ../BUILD