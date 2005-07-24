test-font2: 
	g++ -g -lfreetype -I. -I/usr/include/freetype2 -L. -Ldisplay -ldisplay -lgraphdev test-font2.cpp font.cpp
test-font: 
	g++ -g -lfreetype -I. -I/usr/include/freetype2 -L. -Ldisplay -ldisplay -lgraphdev test-font.cpp
test-freetype: 
	g++ -g -lfreetype -I/usr/include/freetype2 -L. -Ldisplay -ldisplay -lgraphdev test-freetype.cpp

graphiz:
	g++ -g -I. -I/usr/include/freetype2 -L. -Ldisplay -ldisplay -lfreetype basefont.cpp graphdev.cpp font.cpp 


test-basefont: libdisplay.a libgraphdev.a
	g++ -g -L. -Ldisplay -ldisplay -lgraphdev test-basefont.cpp
test-graphdev: libdisplay.a libgraphdev.a
	g++ -g -I. -Ldisplay -L. test-graphdev.cpp -ldisplay -lgraphdev
main:
	gcc -g -shared iiimccf.c -o libiiimccf.so
	cp libiiimccf.so /tmp/iterm/lib/
libdisplay.a:
	gcc -g -shared -I.  ./display/*cpp -o ./display/libdisplay.a
libgraphdev.a: libdisplay.a
	g++ -g -shared -I. -Ldisplay -ldisplay graphdev.cpp basefont.cpp 
