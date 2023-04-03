.PHONY: clean
.PHONY: launch

make: main.c
	emcc -o main.js main.c -s "EXPORTED_RUNTIME_METHODS=['ccall']" 

clean:
	rm main.wasm main.js
