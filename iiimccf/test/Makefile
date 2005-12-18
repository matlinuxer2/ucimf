CFLAGS=`freetype-config --cflags` -I../display -I..
LDFLAGS=-L../display -lfreetype -I/usr/include/iiim
#test-layer: 
#	g++ -g $(CFLAGS) $(LDFLAGS) test-layer.cpp ../layer.cpp ../font.cpp
#test-font2: 
#	g++ -g $(CFLAGS) $(LDFLAGS) test-font2.cpp ../font.cpp
#test-font: 
#	g++ -g $(CFLAGS) $(LDFLAGS) test-font.cpp
#test-freetype: 
#	g++ -g $(CFLAGS) $(LDFLAGS) test-freetype.cpp

graphiz:
	g++ -g -shared -o libgraphiz.a $(CFLAGS) $(LDFLAGS) ../graphdev.cpp ../font.cpp 


test-basefont: libdisplay.a libgraphdev.a
	g++ -g $(CFLAGS) $(LDFLAGS) test-basefont.cpp
test-graphdev: libdisplay.a libgraphdev.a
	g++ -g $(CFLAGS) $(LDFLAGS) test-graphdev.cpp -ldisplay -lgraphdev
main:
	gcc -g -shared iiimccf.c -o libiiimccf.so
	cp libiiimccf.so /tmp/iterm/lib/
libdisplay.a:
	gcc -g -shared -I.  ./display/*cpp -o ./display/libdisplay.a
libgraphdev.a: libdisplay.a
	g++ -g -shared $(CFLAGS) $(LDFLAGS) graphdev.cpp basefont.cpp 
