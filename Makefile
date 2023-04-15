.PHONY: clean
.PHONY: launch

make: ./src/
	emcc -o main.js ./src/lib/stb_image/stb_image.c ./src/main.c ./src/init.c ./src/maths.c --preload-file ./res/ -s "EXPORTED_RUNTIME_METHODS=['ccall']" 

clean:
	rm main.wasm main.js
