all:
	./build-packages.sh $$(lsb_release -c -s) 2>&1 | tee build.log

lucid:
	./build-packages.sh lucid 2>&1 | tee build.log

karmic:
	./build-packages.sh karmic 2>&1 | tee build.log

jaunty:
	./build-packages.sh jaunty 2>&1 | tee build.log

clean:
	rm -r __debian* incoming/ build.log 2>/dev/null

