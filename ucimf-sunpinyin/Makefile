a.out:
	g++ -o a.out main.cpp $$(pkg-config --libs --cflags sunpinyin-2.0)

run: a.out
	./a.out

clean:
	rm a.out
