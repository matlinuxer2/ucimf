
ft2:ft2.o
	g++ -g -o $@ $< `freetype-config --libs`
ft2.o:font_loading.cpp
	g++ -g -c -o ft2.o `freetype-config --cflags` $<

clean:
	rm -rf *.o
