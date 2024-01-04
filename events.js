var finished_loading = false;

onload = function (e) {
    Module.onRuntimeInitialized = () => { finished_loading = true; };
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
        // case 'ArrowRight':
        //     Module.ccall('ar_down');
        //     break;
        // case 'ArrowLeft':
        //     Module.ccall('al_down');
        //     break;
        // case 'ArrowUp':
        //     Module.ccall('au_down');
        //     break;
        // case 'ArrowDown':
        //     Module.ccall('ad_down');
        //     break;
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
        // case 'ArrowRight':
        //     Module.ccall('ar_up');
        //     break;
        // case 'ArrowLeft':
        //     Module.ccall('al_up');
        //     break;
        // case 'ArrowUp':
        //     Module.ccall('au_up');
        //     break;
        // case 'ArrowDown':
        //     Module.ccall('ad_up');
        //     break;
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