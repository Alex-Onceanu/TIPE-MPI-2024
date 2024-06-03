.PHONY: clean
.PHONY: launch

make: ./src/
	emcc \
		-o main.js $(shell find ./src -name '*.c') \
		-s EXPORTED_FUNCTIONS="['_main','_free_everything','_mouse_move','_mouse_up','_mouse_down','_w_up','_w_down','_a_up','_a_down','_s_up','_s_down','_d_up','_d_down','_e_up','_e_down','_space_up','_space_down','_shift_up','_shift_down','_c_gravity','_c_fluid_mu','_c_solid_mu','_c_rotation_mu','_c_ball_mass','_c_throw_speed','_c_throw_angle','_c_light_color_r','_c_light_color_g','_c_light_color_b','_c_ambient_intensity','_c_ball_reflectivity','_toggle_should_render_axis']" \
		-s EXPORTED_RUNTIME_METHODS=["ccall"] \
		-s MAX_WEBGL_VERSION=3 	\
		-s MIN_WEBGL_VERSION=3 	\
		-s ALLOW_MEMORY_GROWTH 	\
		--preload-file ./res/  	\
		-O3

clean:
	rm main.wasm main.js
