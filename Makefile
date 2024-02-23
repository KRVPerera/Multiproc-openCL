clone:
	git submodule update --init --recursive

create_build_folder:
	mkdir -p build

build:
	cd build && cmake .. && make
