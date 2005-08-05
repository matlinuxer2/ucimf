INCLUDES  = -I. -I/usr/include/freetype2
LIBS      = -L. -Ldisplay -ldisplay -lfreetype

INCLUDES2 = -I. -I/usr/include/freetype2
LIBS2     = -L. -lgraphdev -lfreetype

INCLUDES3 = -I. -I/usr/include/freetype2
LIBS3     = -L. -liiimccf -lgraphdev -ldriver

run: 
	g++ -g -o test.exe $(INCLUDE3) $(LIB3) console-app.cpp 

iiimccf:
	g++ -g -shared -o libiiimccf.a $(INCLUDES2) $(LIBS2) iiimccf.cpp iiimccf-core.cpp iiimccf-component.cpp iiimccf-util.cpp

graphdev:
	g++ -g -shared -o libgraphdev.a  $(INCLUDES) $(LIBS) graphdev.cpp font.cpp layer.cpp overspot.cpp

driver:
	g++ -g -shared -o ./display/libdisplay.a -I. ./display/*cpp 
