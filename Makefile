.PHONY: clean
.PHONY: launch

make: ./src/
	emcc \
		-o main.js $(shell find ./src -name '*.c') \
		-sEXPORTED_FUNCTIONS=_main,_mouse_move,_mouse_up,_mouse_down,_w_up,_w_down,_a_up,_a_down,_s_up,_s_down,_d_up,_d_down,_au_up,_au_down,_ad_up,_ad_down,_ar_up,_ar_down,_al_up,_al_down,_space_up,_space_down,_shift_up,_shift_down\
		--preload-file ./res/ -s "EXPORTED_RUNTIME_METHODS=['ccall']" 

clean:
	rm main.wasm main.js
