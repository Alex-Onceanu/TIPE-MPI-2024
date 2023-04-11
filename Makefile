.PHONY: clean
.PHONY: launch

make: ./src/
	emcc -o main.js ./src/main.c ./src/init.c --preload-file ./res/shaders/ -s "EXPORTED_RUNTIME_METHODS=['ccall']" 

clean:
	rm main.wasm main.js
