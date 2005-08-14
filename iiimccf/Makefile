INCLUDES  = -I. -I/usr/include/freetype2
LIBS      = -L. -Ldisplay -ldisplay -lfreetype

INCLUDES2 = -I. -I/usr/include/freetype2
LIBS2     = -liiimcf -L. -lgraphdev -lfreetype -Ldisplay -ldisplay

INCLUDES3 = -I. -I/usr/include/freetype2
LIBS3     = -liiimcf -L. -liiimccf -lgraphdev -Ldisplay -ldisplay 

run:
	./main.exe

all: driver graphdev main

clean: 
	rm main.exe libiiimccf.a libgraphdev.a display/libdisplay.a

main: 
	g++ -g  -o main.exe $(INCLUDES3) $(LIBS3) console-app.cpp 

iiimccf:
	g++ -g -fPIC -shared -o libiiimccf.a $(INCLUDES2) $(LIBS2) iiimccf.cpp iiimccf-core.cpp iiimccf-component.cpp iiimccf-util.cpp iiimccf-preedit.cpp iiimccf-lookup_choice.cpp

graphdev:
	g++ -g -fPIC -shared -o libgraphdev.a  $(INCLUDES) $(LIBS) graphdev.cpp font.cpp layer.cpp overspot.cpp

driver:
	g++ -g -fPIC -shared -o ./display/libdisplay.a -I. ./display/*cpp 
