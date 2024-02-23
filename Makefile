lib/Catch2/README.md:
	git submodule update --init --recursive

MultiProcOpenCL: 
	mkdir -p build
	cd build && cmake .. && make
