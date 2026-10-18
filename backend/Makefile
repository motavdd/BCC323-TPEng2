all: build_unit

build_lib:
	mkdir -p bin
	g++ -fPIC -shared -DBUILD_DLL src/*.cpp -I./src -o bin/libmodel.so

build_unit:
	g++ test/Unit/*.cpp -I./src -L./bin -lmodel -o bin/unit_tests 


run_unit:
	LD_LIBRARY_PATH=./bin ./bin/unit_tests

clean:
	rm -rf bin/