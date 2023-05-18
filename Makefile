.PHONY: clean
.PHONY: launch

make: ./src/
	emcc -o main.js $(shell find ./src -name '*.c') --preload-file ./res/ -s "EXPORTED_RUNTIME_METHODS=['ccall']" 

clean:
	rm main.wasm main.js
