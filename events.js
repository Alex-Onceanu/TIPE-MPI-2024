var finished_loading = false;
console.log("Chargement en cours, veuillez patienter...\n");

onload = function (e) {
    console.log("Chargement terminé !\n");
    Module.onRuntimeInitialized = () => { finished_loading = true; };
}

onexit = function (e) {
    Module.ccall('free_everything');
}

function clamp(pos, min_x, max_x, min_y, max_y) {
    if (pos.x < min_x) {
        pos.x = min_x;
    }
    else if (pos.x > max_x) {
        pos.x = max_x;
    }
    if (pos.y < min_y) {
        pos.y = min_y;
    }
    else if (pos.y > max_y) {
        pos.y = max_y;
    }
}

onmousedown = function (e) {
    if (!finished_loading) return;
    Module.ccall('mouse_down');
}

onmouseup = function (e) {
    if (!finished_loading) return;
    Module.ccall('mouse_up');
}

onmousemove = function (e) {
    if (!finished_loading) return;
    var rect = document.getElementById("canvas").getBoundingClientRect();
    var max_x = rect.right - rect.left;
    var max_y = rect.bottom - rect.top;

    // Position de la souris relative au canvas, normalisée
    var pos = {
        x: 2.0 * (-0.5 + (e.clientX - rect.left) / max_x),
        y: -2.0 * (-0.5 + (e.clientY - rect.top) / max_y)
    };

    clamp(pos, -1.0, 1.0, -1.0, 1.0);
    Module.ccall('mouse_move', 'number', 'number', [pos.x, pos.y]);
}

document.addEventListener('keydown', (event) => {
    if (!finished_loading) return;
    var code = event.code;
    switch (code) {
        case 'KeyW':
            Module.ccall('w_down');
            break;
        case 'KeyA':
            Module.ccall('a_down');
            break;
        case 'KeyS':
            Module.ccall('s_down');
            break;
        case 'KeyD':
            Module.ccall('d_down');
            break;
        case 'KeyE':
            Module.ccall('e_down');
            break;
        case 'ArrowRight':
            Module.ccall('d_down');
            break;
        case 'ArrowLeft':
            Module.ccall('a_down');
            break;
        case 'ArrowUp':
            Module.ccall('w_down');
            break;
        case 'ArrowDown':
            Module.ccall('s_down');
            break;
        case 'Space':
            Module.ccall('space_down');
            break;
        case 'ShiftLeft':
            Module.ccall('shift_down');
            break;
        case 'ShiftRight':
            Module.ccall('shift_down');
            break
        default:
            break;
    }
}, false);

document.addEventListener('keyup', (event) => {
    if (!finished_loading) return;
    var code = event.code;
    switch (code) {
        case 'KeyW':
            Module.ccall('w_up');
            break;
        case 'KeyA':
            Module.ccall('a_up');
            break;
        case 'KeyS':
            Module.ccall('s_up');
            break;
        case 'KeyD':
            Module.ccall('d_up');
            break;
        case 'KeyE':
            Module.ccall('e_up');
            break;
        case 'ArrowRight':
            Module.ccall('d_up');
            break;
        case 'ArrowLeft':
            Module.ccall('a_up');
            break;
        case 'ArrowUp':
            Module.ccall('w_up');
            break;
        case 'ArrowDown':
            Module.ccall('s_up');
            break;
        case 'Space':
            Module.ccall('space_up');
            break;
        case 'ShiftLeft':
            Module.ccall('shift_up');
            break;
        case 'ShiftRight':
            Module.ccall('shift_up');
            break
        default:
            break;
    }
}, false);


function c_gravity(e) {
    if (!finished_loading) return;
    console.log("g = ", e / 600.0);
    Module.ccall('c_gravity', null, ['number'], [e / 600.0])
}

function c_fluid_mu(e) {
    if (!finished_loading) return;
    Module.ccall('c_fluid_mu', null, ['number'], [e / 6000.0])
}

function c_solid_mu(e) {
    if (!finished_loading) return;
    Module.ccall('c_solid_mu', null, ['number'], [e / 60.0])
}

function c_rotation_mu(e) {
    if (!finished_loading) return;
    Module.ccall('c_rotation_mu', null, ['number'], [e / 600.0])
}

function c_ball_mass(e) {
    if (!finished_loading) return;
    Module.ccall('c_ball_mass', null, ['number'], [e / 100.0])
}

function c_throw_speed(e) {
    if (!finished_loading) return;
    Module.ccall('c_throw_speed', null, ['number'], [e / 10.0])
}

function c_throw_angle(e) {
    if (!finished_loading) return;
    Module.ccall('c_throw_angle', null, ['number'], [e * 3.14159 / 180.0])
}

function c_light_color_r(e) {
    if (!finished_loading) return;
    Module.ccall('c_light_color_r', null, ['number'], [e / 255.0])
}

function c_light_color_g(e) {
    if (!finished_loading) return;
    Module.ccall('c_light_color_g', null, ['number'], [e / 255.0])
}

function c_light_color_b(e) {
    if (!finished_loading) return;
    Module.ccall('c_light_color_b', null, ['number'], [e / 255.0])
}

function c_ambient_intensity(e) {
    if (!finished_loading) return;
    Module.ccall('c_ambient_intensity', null, ['number'], [e / 100.0])
}

function c_reflectivity(e) {
    if (!finished_loading) return;
    Module.ccall('c_ball_reflectivity', null, ['number'], [e / 100.0])
}

function toggle_should_render_axis() {
    if (!finished_loading) return;
    Module.ccall('toggle_should_render_axis', null, null, [])
}