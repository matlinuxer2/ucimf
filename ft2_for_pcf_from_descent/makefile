
ft2:ft2.o
	g++ -o $@ $< `freetype-config --libs`
ft2.o:ft2.cpp
	g++ -c `freetype-config --cflags` $<

clean:
	rm -rf *.o
