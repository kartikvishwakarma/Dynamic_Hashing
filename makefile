all: extendible_hashing.cpp linear_hashing.cpp dataset.cpp
	g++ extendible_hashing.cpp -o extend -std=c++11
	g++ linear_hashing.cpp -o linear -std=c++11
	g++ dataset.cpp -o dataset -std=c++11

extend:
	./extend 
linear:
	./linear
dataset:
	./dataset

clean:
	rm -f *.txt ./extend ./linear ./dataset

