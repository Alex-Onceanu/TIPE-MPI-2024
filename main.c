#include <GLES2/gl2.h>
#include <emscripten/html5.h>

int main()
{
    // setting up EmscriptenWebGLContextAttributes
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;

    // target the canvas selector
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(ctx);

    // target infinite loop for emscripten !!!!1

    // this goes after you have activated the webgl context
    glClearColor(0.1, 0.4627, 0.382, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    return 1;
}