PWD	  = /home/mat/src-cvs/iiimtcf/iiimccf

INCLUDES  = -I. -I/usr/include/freetype2
LIBS      = -L$(PWD) -L$(PWD)/display -ldisplay -lfreetype

INCLUDES2 = -I. -I/usr/include/freetype2
LIBS2     = -liiimcf -L $(PWD) -lgraphdev -lfreetype -L$(PWD)/display -ldisplay

INCLUDES3 = -I. -I/usr/include/freetype2
LIBS3     = -liiimcf -L$(PWD) -liiimccf -lgraphdev -L$(PWD)/display -ldisplay 

run:
	./main.exe

all: driver graphdev main

clean: 
	rm main.exe libiiimccf.a libgraphdev.a display/libdisplay.a

main: 
	g++ -c $(INCLUDES3) console-app.cpp
	gcc -o main.exe  $(LIBS3) console-app.cpp 

iiimccf:
	g++ -c $(INCLUDES2) iiimccf.cpp iiimccf-core.cpp iiimccf-component.cpp iiimccf-util.cpp iiimccf-preedit.cpp iiimccf-lookup_choice.cpp
	gcc -static -o libiiimccf.so $(LIBS2)  libgraphdev.so ./display/libdisplay.so iiimccf.o iiimccf-core.o iiimccf-component.o iiimccf-util.o iiimccf-preedit.o iiimccf-lookup_choice.o


graphdev:
	g++ -c $(INCLUDES) graphdev.cpp font.cpp layer.cpp overspot.cpp
	gcc -shared -o libgraphdev.so  $(LIBS) graphdev.o font.o layer.o overspot.o

driver:
	cd ./display
	g++ -c -I.. *.cpp 
	cd ..
	gcc -shared -lstdc++ -o ./display/libdisplay.so ./display/*.o

