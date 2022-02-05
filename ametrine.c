//REVIEW: Could change int num_* property of some structs to size_t size, so it matches with the other ones

//TODO: 0 is the default value for filling structs, should start array indexing at 1 for proper warn/error messages
//TODO: Should create object entry first thing upon creation; in case of failure to create, free the spot
//TODO: Lines 209, 559, 678, 700, 738, 779, 1009, 2777, 2902, 2934, 3045, 3093, 3333

//----------------------------------------------------------------------------//
//                                  INCLUDES                                  //
//----------------------------------------------------------------------------//

#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
//#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#if (defined _WIN32 || defined _WIN64)
    #define OEMRESOURCE
    #define AM_WINDOWS
    #include <windows.h>
    #include <GL/glext.h>
#else
    #define AM_LINUX
    #include <X11/Xatom.h>
    #include <X11/Xlib.h>
    #include <GL/glx.h>
    #include <sys/stat.h>
#endif

//----------------------------------------------------------------------------//
//                                END INCLUDES                                //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                               TYPES AND DEFS                               //
//----------------------------------------------------------------------------//

#define am_malloc(size) malloc(size)
#define am_free(mem) free((void*)mem)
#define am_realloc(mem, size) realloc(mem, size)
#define am_calloc(mem, size) calloc(mem, size)

#define AM_MAX_NAME_LENGTH 32

typedef signed char am_int8;
typedef unsigned char am_uint8;
typedef signed short am_int16;
typedef unsigned short am_uint16;
typedef signed int  am_int32;
typedef unsigned int  am_uint32;
typedef signed long long int am_int64; //Can be 4 bytes on 32bit OSes
typedef unsigned long long int am_uint64;
typedef float am_float32;
typedef double am_float64;
typedef enum {false, true} am_bool;
typedef am_uint32 am_id;

#if defined(AM_LINUX)
    #define AM_CALL *
    #define amgl_get_proc_address(str) glXGetProcAddress((unsigned char*)(str))
#else
    #define AM_CALL __stdcall*
    #define amgl_get_proc_address(str) wglGetProcAddress((str))
#endif

//HACK: Temporary
am_bool temp_check = true;

//REVIEW: OpenGL functions

#if defined(AM_LINUX)
typedef void (AM_CALL PFNGLSWAPINTERVALEXTPROC) (Display *dpy, GLXDrawable drawable, int interval);
#else
typedef BOOL (AM_CALL PFNGLSWAPINTERVALEXTPROC) (int interval);
#endif
typedef GLuint (AM_CALL PFNGLCREATESHADERPROC) (GLenum type);
typedef void (AM_CALL PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (AM_CALL PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef void (AM_CALL PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (AM_CALL PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (AM_CALL PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (AM_CALL PFNGLDELETESHADERPROC) (GLuint shader);
typedef GLuint (AM_CALL PFNGLCREATEPROGRAMPROC) (void);
typedef void (AM_CALL PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (AM_CALL PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (AM_CALL PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (AM_CALL PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (AM_CALL PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (AM_CALL PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (AM_CALL PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (AM_CALL PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (AM_CALL PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (AM_CALL PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (AM_CALL PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef void (AM_CALL PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
typedef void (AM_CALL PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (AM_CALL PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (AM_CALL PFNGLVERTEXATTRIBIPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const void * pointer);
typedef void (AM_CALL PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef GLint (AM_CALL PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (AM_CALL PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef GLint (AM_CALL PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (AM_CALL PFNGLGENERATEMIPMAPPROC) (GLenum target);
typedef void (AM_CALL PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
typedef void (AM_CALL PFNGLGENFRAMEBUFFERSPROC) (GLsizei n, GLuint *ids);
//FIX: Fix this, conflicting types
//typedef void (AM_CALL PFNGLDELETEFRAMEBUFFERSPROC) (GLsizei n, GLuint *framebuffers);
typedef void (AM_CALL PFNGLBINDFRAMEBUFFERPROC) (GLenum target, GLuint framebuffer);
typedef void (AM_CALL PFNGLFRAMEBUFFERTEXTURE2DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (AM_CALL PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (AM_CALL PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
typedef void (AM_CALL PFNGLGETUNIFORMFVPROC) (GLuint program, GLint location, GLfloat *params);
typedef void (AM_CALL PFNGLBINDIMAGETEXTUREPROC) (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

PFNGLSWAPINTERVALEXTPROC glSwapInterval = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLDETACHSHADERPROC glDetachShader = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;
PFNGLUNIFORM1FVPROC glUniform1fv = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLGETUNIFORMFVPROC glGetUniformfv = NULL;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture = NULL;

//----------------------------------------------------------------------------//
//                             END TYPES AND DEFS                             //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                               DYNAMIC  ARRAY                               //
//----------------------------------------------------------------------------//

#define AM_DYN_ARRAY_EMPTY_START_SLOTS 2

typedef struct am_dyn_array_header {
    size_t size; // In bytes
    size_t capacity; // In bytes
} am_dyn_array_header;

#define am_dyn_array(type) type*
#define am_dyn_array_get_header(array) ((am_dyn_array_header*)((size_t*)(array) - 2))
#define am_dyn_array_get_size(array) am_dyn_array_get_header(array)->size
#define am_dyn_array_get_count(array) am_dyn_array_get_size(array)/sizeof((array)[0])
#define am_dyn_array_get_capacity(array) am_dyn_array_get_header(array)->capacity
#define am_dyn_array_push(array, value) (am_dyn_array_resize((void**)&(array), sizeof((array)[0])), (array)[am_dyn_array_get_count(array)] = (value), am_dyn_array_get_header(array)->size += sizeof((array)[0]))
#define am_dyn_array_pop(array) am_dyn_array_get_header(array)->size -= sizeof((array)[0])
#define am_dyn_array_clear(array) (am_dyn_array_get_header(array)->size = 0)

void am_dyn_array_init(void **array, size_t value_size);
void am_dyn_array_resize(void **array, size_t add_size);
void am_dyn_array_replace(void *array, void *values, size_t offset, size_t size);
void am_dyn_array_destroy(void *array);

//----------------------------------------------------------------------------//
//                             END DYNAMIC  ARRAY                             //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                PACKED ARRAY                                //
//----------------------------------------------------------------------------//

#define AM_PA_INVALID_ID 0xFFFFFFFF
#define AM_PA_INVALID_INDEX 0xFFFFFFFF

#define am_packed_array(type)\
    struct {\
        am_dyn_array(am_uint32) indices;\
        am_dyn_array(type) elements;\
        am_uint32 next_id;\
    }*

#define am_packed_array_get_idx(array, id) ((id) < (array)->next_id ? (array)->indices[(id)] : AM_PA_INVALID_INDEX)
//TODO: Some kind of guard for get_val needed
#define am_packed_array_get_val(array, id) ((array)->elements[(array)->indices[(id)]])
#define am_packed_array_get_ptr(array, id) (((id) < (array)->next_id) && ((array)->indices[(id)] != AM_PA_INVALID_INDEX) ? &((array)->elements[(array)->indices[(id)]]) : NULL)
#define am_packed_array_get_count(array) (am_dyn_array_get_count((array)->elements))
#define am_packed_array_has(array, id) (((id) < (array)->next_id) && (am_packed_array_get_idx((array), (id)) != AM_PA_INVALID_INDEX) ? 1 : 0)
#define am_packed_array_init(array, value_size) (am_packed_array_alloc((void**)&(array), sizeof(*(array))), (array)->next_id = 1, am_dyn_array_init((void**)&((array)->indices), sizeof(am_uint32)*2), am_dyn_array_get_header((array)->indices)->size += sizeof(am_uint32), am_dyn_array_init((void**)&((array)->elements), (value_size)))
#define am_packed_array_clear(array) (am_dyn_array_clear((array)->indices), am_dyn_array_clear((array)->elements))
#define am_packed_array_destroy(array) (am_dyn_array_destroy((array)->indices), am_dyn_array_destroy((array)->elements), am_free(array))
#define am_packed_array_add(array, value) (am_dyn_array_push((array)->indices, am_dyn_array_get_count((array)->indices)-1), am_dyn_array_push((array)->elements, value), (array)->next_id++)

//----------------------------------------------------------------------------//
//                              END PACKED ARRAY                              //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                    MATH                                    //
//----------------------------------------------------------------------------//


typedef struct am_vec2 {
    union {
        am_float32 xy[2];
        struct {
            am_float32 x, y;
        };
    };
} am_vec2;

static inline am_vec2 am_vec2_add(am_vec2 vec0, am_vec2 vec1);
static inline am_vec2 am_vec2_sub(am_vec2 vec0, am_vec2 vec1);
static inline am_vec2 am_vec2_mul(am_vec2 vec0, am_vec2 vec1);
static inline am_vec2 am_vec2_div(am_vec2 vec0, am_vec2 vec1);
static inline am_vec2 am_vec2_scale(am_float32 scalar, am_vec2 vec);

//----------------------------------------------------------------------------//
//                                  END MATH                                  //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                  PLATFORM                                  //
//----------------------------------------------------------------------------//

#define AM_MAX_KEYCODE_COUNT 512
#define AM_ROOT_WIN_CLASS "AM_ROOT"
#define AM_CHILD_WIN_CLASS "AM_CHILD"
#define AM_WINDOW_DEFAULT_X 500
#define AM_WINDOW_DEFAULT_Y 500
#define AM_WINDOW_DEFAULT_WIDTH 500
#define AM_WINDOW_DEFAULT_HEIGHT 500
#define AM_WINDOW_DEFAULT_NAME "Ametrine"
#define AM_WINDOW_DEFAULT_NAME_WITH_ID(id) AM_WINDOW_DEFAULT_NAME #id
#if defined(AM_LINUX) 
    #define AM_WINDOW_DEFAULT_PARENT DefaultRootWindow(am_engine_get_subsystem(platform)->display)
#else 
    #define AM_WINDOW_DEFAULT_PARENT 0
#endif

typedef enum am_key_map {
    AM_KEYCODE_INVALID,
    AM_KEYCODE_ESCAPE,
    AM_KEYCODE_F1,
    AM_KEYCODE_F2,
    AM_KEYCODE_F3,
    AM_KEYCODE_F4,
    AM_KEYCODE_F5,
    AM_KEYCODE_F6,
    AM_KEYCODE_F7,
    AM_KEYCODE_F8,
    AM_KEYCODE_F9,
    AM_KEYCODE_F10,
    AM_KEYCODE_F11,
    AM_KEYCODE_F12,
    AM_KEYCODE_F13,
    AM_KEYCODE_F14,
    AM_KEYCODE_F15,
    AM_KEYCODE_F16,
    AM_KEYCODE_F17,
    AM_KEYCODE_F18,
    AM_KEYCODE_F19,
    AM_KEYCODE_F20,
    AM_KEYCODE_F21,
    AM_KEYCODE_F22,
    AM_KEYCODE_F23,
    AM_KEYCODE_F24,
    AM_KEYCODE_F25,
    AM_KEYCODE_PRINT_SCREEN,
    AM_KEYCODE_SCROLL_LOCK,
    AM_KEYCODE_PAUSE,
    AM_KEYCODE_ACCENT_GRAVE,
    AM_KEYCODE_1,
    AM_KEYCODE_2,
    AM_KEYCODE_3,
    AM_KEYCODE_4,
    AM_KEYCODE_5,
    AM_KEYCODE_6,
    AM_KEYCODE_7,
    AM_KEYCODE_8,
    AM_KEYCODE_9,
    AM_KEYCODE_0,
    AM_KEYCODE_MINUS,
    AM_KEYCODE_EQUAL,
    AM_KEYCODE_BACKSPACE,
    AM_KEYCODE_INSERT,
    AM_KEYCODE_HOME,
    AM_KEYCODE_PAGE_UP,
    AM_KEYCODE_NUMPAD_NUM,
    AM_KEYCODE_NUMPAD_DIVIDE,
    AM_KEYCODE_NUMPAD_MULTIPLY,
    AM_KEYCODE_NUMPAD_SUBTRACT,
    AM_KEYCODE_TAB,
    AM_KEYCODE_Q,
    AM_KEYCODE_W,
    AM_KEYCODE_E,
    AM_KEYCODE_R,
    AM_KEYCODE_T,
    AM_KEYCODE_Y,
    AM_KEYCODE_U,
    AM_KEYCODE_I,
    AM_KEYCODE_O,
    AM_KEYCODE_P,
    AM_KEYCODE_LEFT_SQUARE_BRACKET,
    AM_KEYCODE_RIGHT_SQUARE_BRACKET,
    AM_KEYCODE_BACKSLASH,
    AM_KEYCODE_DELETE,
    AM_KEYCODE_END,
    AM_KEYCODE_PAGE_DOWN,
    AM_KEYCODE_NUMPAD_7,
    AM_KEYCODE_NUMPAD_8,
    AM_KEYCODE_NUMPAD_9,
    AM_KEYCODE_CAPS_LOCK,
    AM_KEYCODE_A,
    AM_KEYCODE_S,
    AM_KEYCODE_D,
    AM_KEYCODE_F,
    AM_KEYCODE_G,
    AM_KEYCODE_H,
    AM_KEYCODE_J,
    AM_KEYCODE_K,
    AM_KEYCODE_L,
    AM_KEYCODE_SEMICOLON,
    AM_KEYCODE_APOSTROPHE,
    AM_KEYCODE_ENTER,
    AM_KEYCODE_NUMPAD_4,
    AM_KEYCODE_NUMPAD_5,
    AM_KEYCODE_NUMPAD_6,
    AM_KEYCODE_NUMPAD_ADD,
    AM_KEYCODE_LEFT_SHIFT,
    AM_KEYCODE_Z,
    AM_KEYCODE_X,
    AM_KEYCODE_C,
    AM_KEYCODE_V,
    AM_KEYCODE_B,
    AM_KEYCODE_N,
    AM_KEYCODE_M,
    AM_KEYCODE_COMMA,
    AM_KEYCODE_PERIOD,
    AM_KEYCODE_SLASH,
    AM_KEYCODE_RIGHT_SHIFT,
    AM_KEYCODE_UP_ARROW,
    AM_KEYCODE_NUMPAD_1,
    AM_KEYCODE_NUMPAD_2,
    AM_KEYCODE_NUMPAD_3,
    AM_KEYCODE_LEFT_CONTROL,
    AM_KEYCODE_LEFT_META,
    AM_KEYCODE_LEFT_ALT,
    AM_KEYCODE_SPACE, 
    AM_KEYCODE_RIGHT_ALT,
    AM_KEYCODE_RIGHT_META,
    AM_KEYCODE_MENU,
    AM_KEYCODE_RIGHT_CONTROL,
    AM_KEYCODE_LEFT_ARROW,
    AM_KEYCODE_DOWN_ARROW,
    AM_KEYCODE_RIGHT_ARROW, 
    AM_KEYCODE_NUMPAD_0,
    AM_KEYCODE_NUMPAD_DECIMAL,
    AM_KEYCODE_NUMPAD_EQUAL,
    AM_KEYCODE_NUMPAD_ENTER,
    AM_KEYCODE_COUNT
} am_key_map;

typedef enum am_mouse_map {
    AM_MOUSE_BUTTON_INVALID,
    AM_MOUSE_BUTTON_LEFT,
    AM_MOUSE_BUTTON_RIGHT,
    AM_MOUSE_BUTTON_MIDDLE,
    AM_MOUSE_BUTTON_COUNT
} am_mouse_map;

typedef enum am_platform_events {
    AM_EVENT_INVALID,
    AM_EVENT_KEY_PRESS,
    AM_EVENT_KEY_RELEASE,
    AM_EVENT_MOUSE_MOTION,
    AM_EVENT_MOUSE_BUTTON_PRESS,
    AM_EVENT_MOUSE_BUTTON_RELEASE,
    AM_EVENT_MOUSE_SCROLL_UP,
    AM_EVENT_MOUSE_SCROLL_DOWN,
    AM_EVENT_WINDOW_SIZE,
    AM_EVENT_WINDOW_MOTION,
    AM_EVENT_COUNT
} am_platform_events;

typedef struct am_window_info {
    am_uint64 parent;
    char name[AM_MAX_NAME_LENGTH];
    am_uint32 width;
    am_uint32 height; 
    am_uint32 x;
    am_uint32 y;
    am_bool is_fullscreen; //Useless for child windows, for now
} am_window_info;

typedef am_window_info am_window_cache;

typedef struct am_window {
    am_uint64 handle;
    am_id id;
    am_uint64 parent;
    char name[AM_MAX_NAME_LENGTH];
    am_uint32 width;
    am_uint32 height;
    am_uint32 x;
    am_uint32 y;
    am_bool is_fullscreen; //Useless for child windows
    am_window_cache cache;

    #if defined(AM_LINUX)
        Colormap colormap;
        XVisualInfo *visual_info;
        GLXContext context;
    #else
        HDC hdc;
        HGLRC context;
        //Context windows
    #endif
} am_window;

typedef struct am_platform_callbacks {
    void (*am_platform_key_callback)(am_id, am_key_map, am_platform_events);
    void (*am_platform_mouse_button_callback)(am_id, am_mouse_map, am_platform_events);
    void (*am_platform_mouse_motion_callback)(am_id, am_int32, am_int32, am_platform_events);
    void (*am_platform_mouse_scroll_callback)(am_id, am_platform_events);
    void (*am_platform_window_size_callback)(am_id, am_uint32, am_uint32, am_platform_events);
    void (*am_platform_window_motion_callback)(am_id, am_uint32, am_uint32, am_platform_events);
} am_platform_callbacks;

typedef struct am_platform_input {
    am_key_map keycodes[AM_MAX_KEYCODE_COUNT]; //LUT
    am_bool keyboard_map[AM_KEYCODE_COUNT]; 
    am_bool prev_keyboard_map[AM_KEYCODE_COUNT];
    am_bool mouse_map[AM_MOUSE_BUTTON_COUNT];
    am_bool prev_mouse_map[AM_MOUSE_BUTTON_COUNT];
    am_int32 wheel_delta;
    am_uint32 mouse_x;
    am_uint32 mouse_y;
    am_bool mouse_moved;
} am_platform_input;

typedef struct am_platform_time {
    am_uint64 offset;
    am_uint64 frequency;
} am_platform_time;

typedef struct am_platform {
    #if defined(AM_LINUX)
        Display *display;
    #endif
    am_packed_array(am_window) windows;
    am_platform_input input;
    am_platform_time time;
    am_platform_callbacks callbacks;
} am_platform;


//Platform 
#if defined(AM_LINUX)
am_key_map am_platform_translate_keysym(KeySym *key_syms, am_int32 width);
#endif
am_mouse_map am_platform_translate_button(am_uint32 button);
am_platform *am_platform_create();
void am_platform_poll_events();
#if defined(AM_WINDOWS) 
LRESULT CALLBACK am_platform_event_handler(HWND handle, am_uint32 event, WPARAM wparam, LPARAM lparam);
#else
void am_platform_event_handler(XEvent *xevent);
#endif
//REVIEW: Passing pointer argument is probably unnecessary, only one platform instance should exist
void am_platform_update(am_platform *platform);
void am_platform_terminate(am_platform *platform);

//Keyboard input
void am_platform_key_press(am_key_map key); 
void am_platform_key_release(am_key_map key); 
am_bool am_platform_key_pressed(am_key_map key); 
am_bool am_platform_key_down(am_key_map key);
am_bool am_platform_key_released(am_key_map key); 
am_bool am_platform_key_up(am_key_map key);

//Mouse input
void am_platform_mouse_button_press(am_mouse_map button); 
void am_platform_mouse_button_release(am_mouse_map button); 
am_bool am_platform_mouse_button_pressed(am_mouse_map button); 
am_bool am_platform_mouse_button_down(am_mouse_map button); 
am_bool am_platform_mouse_button_released(am_mouse_map button); 
am_bool am_platform_mouse_button_up(am_mouse_map button);
void am_platform_mouse_position(am_uint32 *x, am_uint32 *y);
am_vec2 am_platform_mouse_positionv();
am_int32 am_platform_mouse_wheel_delta();
am_bool am_platform_mouse_moved();

//Platform default callbacks
void am_platform_key_callback_default(am_id id, am_key_map key, am_platform_events event);
void am_platform_mouse_button_callback_default(am_id id, am_mouse_map button, am_platform_events event);
void am_platform_mouse_motion_callback_default(am_id id, am_int32 x, am_int32 y, am_platform_events event);
void am_platform_mouse_scroll_callback_default(am_id id, am_platform_events event);
void am_platform_window_size_callback_default(am_id id, am_uint32 width, am_uint32 height, am_platform_events event);
void am_platform_window_motion_callback_default(am_id id, am_uint32 x, am_uint32 y, am_platform_events event);

#define am_platform_set_key_callback(platform, callback) platform->callbacks.am_platform_key_callback = callback
#define am_platform_set_mouse_button_callback(platform, callback) platform->callbacks.am_platform_mouse_button_callback = callback
#define am_platform_set_mouse_motion_callback(platform, callback) platform->callbacks.am_platform_mouse_motion_callback = callback
#define am_platform_set_mouse_scroll_callback(platform, callback) platform->callbacks.am_platform_mouse_scroll_callback = callback  
#define am_platform_set_window_size_callback(platform, callback) platform->callbacks.am_platform_window_size_callback = callback  
#define am_platform_set_window_motion_callback(platform, callback) platform->callbacks.am_platform_window_motion_callback = callback  

//Windows
am_id am_platform_window_create(am_window_info window_info);
void am_platform_window_resize(am_id id, am_uint32 width, am_uint32 height);
void am_platform_window_move(am_id id, am_uint32 x, am_uint32 y);
void am_platform_window_fullscreen(am_id id, am_bool state);
void am_platform_window_destroy(am_id id);

//Time
void am_platform_timer_create();
void am_platform_timer_sleep(am_float32 ms);
am_uint64 am_platform_timer_value();
am_uint64 am_platform_elapsed_time();

//----------------------------------------------------------------------------//
//                                END PLATFORM                                //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                  START GL                                  //
//----------------------------------------------------------------------------//

//TODO: Compute Shaders
//Shader & program

#define AMGL_SHADER_DEFAULT_NAME "amgl_shader"

#define AMGL_VERTEX_BUFFER_DEFAULT_NAME "amgl_vertex_buffer"
#define AMGL_INDEX_BUFFER_DEFAULT_NAME "amgl_index_buffer"
#define AMGL_UNIFORM_DEFAULT_NAME "amgl_uniform"

#define AMGL_TEXTURE_DEFAULT_NAME "amgl_texture"
#define AMGL_TEXTURE_DEFAULT_WIDTH 500
#define AMGL_TEXTURE_DEFAULT_HEIGHT 500
#define AMGL_TEXTURE_DEFAULT_WRAP AMGL_TEXTURE_WRAP_REPEAT
#define AMGL_TEXTURE_DEFAULT_FORMAT AMGL_TEXTURE_FORMAT_RGBA8

#define AMGL_FRAME_BUFFER_DEFAULT_NAME "amgl_frame_buffer"
#define AMGL_RENDER_PASS_DEFAULT_NAME "amgl_render_pass"
#define AMGL_PIPELINE_DEFAULT_NAME "amgl_pipeline"


typedef enum amgl_shader_type {
    AMGL_SHADER_VERTEX,
    AMGL_SHADER_FRAGMENT
    //compute
} amgl_shader_type;

typedef struct amgl_shader_source_info {
    //REVIEW: Would a name here have a use?
    amgl_shader_type type;
    char *source;
} amgl_shader_source_info;

typedef struct amgl_shader_info {
    char name[AM_MAX_NAME_LENGTH];
    amgl_shader_source_info *sources;
    am_uint32 num_sources;
} amgl_shader_info;

typedef struct amgl_shader {
    char name[AM_MAX_NAME_LENGTH];
    am_uint32 handle;
    am_id id;
} amgl_shader;

typedef enum amgl_buffer_usage {
    AMGL_BUFFER_USAGE_STATIC,
    AMGL_BUFFER_USAGE_DYNAMIC,
    AMGL_BUFFER_USAGE_STREAM
} amgl_buffer_usage;

/*
typedef enum amgl_buffer_update_type {
    AMGL_BUFFER_UPDATE_SUBDATA,
    AMGL_BUFFER_UPDATE_RECREATE
} amgl_buffer_update_type;
*/

typedef enum amgl_vertex_buffer_attribute_format {
    AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT2,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT3,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT4,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT2,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT3,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT4,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE2,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE3,
    AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE4
} amgl_vertex_buffer_attribute_format;

typedef struct amgl_vertex_buffer_attribute {
    size_t stride;
    size_t offset;
    amgl_vertex_buffer_attribute_format format;
    am_uint32 buffer_index; 
} amgl_vertex_buffer_attribute;

typedef struct amgl_vertex_buffer_layout {
    amgl_vertex_buffer_attribute *attributes;
    am_uint32 num_attribs;
} amgl_vertex_buffer_layout;

typedef struct amgl_vertex_buffer_info {
    char name[AM_MAX_NAME_LENGTH];
    void *data;
    size_t size;
    amgl_buffer_usage usage;
} amgl_vertex_buffer_info;

typedef struct amgl_vertex_buffer {
    char name[AM_MAX_NAME_LENGTH];
    am_uint32 handle;
    am_id id;
} amgl_vertex_buffer;

/*
typedef struct amgl_vertex_buffer_update_info {
    amgl_vertex_buffer_info info; 
    amgl_buffer_update_type update_type;
    size_t offset; 
} amgl_vertex_buffer_update_info;
*/

typedef struct amgl_index_buffer_info {
    char name[AM_MAX_NAME_LENGTH];
    void *data;
    size_t size;
    size_t offset;
    amgl_buffer_usage usage;
} amgl_index_buffer_info;

typedef struct amgl_index_buffer {
    char name[AM_MAX_NAME_LENGTH];
    am_id id;
    am_uint32 handle;
} amgl_index_buffer;

//TODO: Support for more uniform types
typedef enum amgl_uniform_type {
    AMGL_UNIFORM_TYPE_FLOAT,
    AMGL_UNIFORM_TYPE_INT,
    AMGL_UNIFORM_TYPE_SAMPLER2D
} amgl_uniform_type;

typedef struct amgl_uniform_info {
    char name[AM_MAX_NAME_LENGTH];
    amgl_uniform_type type;
} amgl_uniform_info;

typedef struct amgl_uniform {
    char name[AM_MAX_NAME_LENGTH];
    am_id id;
    am_uint32 location;
    void* data;
    size_t size;
    amgl_uniform_type type;
    am_id shader_id;
} amgl_uniform;

//TODO: Textures: Change loading style, delete after upload to GPU
//Textures
/*
typedef enum amgl_texture_update_type {
    AMGL_TEXTURE_UPDATE_RECREATE,
    AMGL_TEXTURE_UPDATE_SUBDATA
} amgl_texture_update_type;
*/

typedef enum amgl_texture_format {
    AMGL_TEXTURE_FORMAT_RGBA8,
    AMGL_TEXTURE_FORMAT_RGB8,
    AMGL_TEXTURE_FORMAT_RGBA16F,
    AMGL_TEXTURE_FORMAT_RGBA32F,
    AMGL_TEXTURE_FORMAT_RGBA,
    AMGL_TEXTURE_FORMAT_A8,
    AMGL_TEXTURE_FORMAT_R8,
    AMGL_TEXTURE_FORMAT_DEPTH8,
    AMGL_TEXTURE_FORMAT_DEPTH16,
    AMGL_TEXTURE_FORMAT_DEPTH24,
    AMGL_TEXTURE_FORMAT_DEPTH32F,
    AMGL_TEXTURE_FORMAT_DEPTH24_STENCIL8,
    AMGL_TEXTURE_FORMAT_DEPTH32F_STENCIL8,
    AMGL_TEXTURE_FORMAT_STENCIL8
} amgl_texture_format;

typedef enum amgl_texture_wrap {
    AMGL_TEXTURE_WRAP_REPEAT,
    AMGL_TEXTURE_WRAP_MIRRORED_REPEAT,
    AMGL_TEXTURE_WRAP_CLAMP_TO_EDGE,
    AMGL_TEXTURE_WRAP_CLAMP_TO_BORDER
} amgl_texture_wrap;

typedef enum amgl_texture_filter {
    AMGL_TEXTURE_FILTER_NEAREST,
    AMGL_TEXTURE_FILTER_LINEAR
} amgl_texture_filter;

//TODO: Specify formats in another enum
typedef struct amgl_texture_info {
    char name[AM_MAX_NAME_LENGTH];
    void *data;
    am_uint32 width;
    am_uint32 height;
    amgl_texture_format format;
    amgl_texture_filter min_filter;
    amgl_texture_filter mag_filter;
    amgl_texture_filter mip_filter;
    am_uint32 mip_num;
    amgl_texture_wrap wrap_s;
    amgl_texture_wrap wrap_t;
} amgl_texture_info;

typedef struct amgl_texture {
    char name[AM_MAX_NAME_LENGTH];
    am_uint32 handle;
    am_id id;
    amgl_texture_format format;
    //NOTE: Ignore for now
    //am_bool render_target;
} amgl_texture;

//NOTE: Technically nothing is needed for now since no update function
//REVIEW
typedef struct amgl_frame_buffer_info {
    char name[AM_MAX_NAME_LENGTH];
    am_uint32 width;
    am_uint32 height;
} amgl_frame_buffer_info;

typedef struct amgl_frame_buffer {
    char name[AM_MAX_NAME_LENGTH];
    am_id id;
    am_uint32 handle;
} amgl_frame_buffer;

// Pipeline Description: vert-layout, shader, bindable, render states
// Pass Description: render pass, action on render targets (clear, set viewport, etc.)

//TODO: Make enums for most of these
typedef struct amgl_blend_info {
    am_int32 func;
    am_int32 src;
    am_int32 dst;
} amgl_blend_info;

typedef struct amgl_depth_info {
    am_int32 func;
} amgl_depth_info;

typedef struct amgl_stencil_info {
    am_int32 func;
    am_int32 ref;
    am_int32 comp_mask;
    am_int32 write_mask;
    am_int32 sfail;
    am_int32 dpfail;
    am_int32 dppass;
} amgl_stencil_info;

typedef struct amgl_raster_info {
    am_int32 face_culling;
    am_int32 winding_order;
    am_int32 primitive;
    am_int32 shader_id; //?
    am_int32 index_buffer_element_size; //?
} amgl_raster_info;

typedef struct amgl_pipeline_info {
    char name[AM_MAX_NAME_LENGTH];
    amgl_blend_info blend;
    amgl_depth_info depth;
    amgl_raster_info raster;
    amgl_stencil_info stencil;
    amgl_vertex_buffer_layout layout;
} amgl_pipeline_info;

typedef struct amgl_pipeline {
    char name[AM_MAX_NAME_LENGTH];
    am_id id;
    amgl_blend_info blend;
    amgl_depth_info depth;
    amgl_raster_info raster;
    amgl_stencil_info stencil;
    amgl_vertex_buffer_layout layout;
} amgl_pipeline;

typedef struct amgl_render_pass_info {
    char name[AM_MAX_NAME_LENGTH];
    am_id framebuffer_id;
    am_id *color_texture_ids;
    am_uint32 num_colors;
    am_id depth_texture_id;
    am_id stencil_texture_id;
} amgl_render_pass_info;

typedef struct amgl_render_pass {
    char name[AM_MAX_NAME_LENGTH];
    am_id id;
    am_id framebuffer_id;
    am_id *color_texture_ids;
    am_uint32 num_colors;
    am_id depth_texture_id;
    am_id stencil_texture_id;
} amgl_render_pass;

typedef struct amgl_vertex_buffer_bind_info {
    am_id vertex_buffer_id;
    //size_t offset;
} amgl_vertex_buffer_bind_info;

typedef struct amgl_index_buffer_bind_info {
    am_id index_buffer_id;
} amgl_index_buffer_bind_info;

typedef struct amgl_texture_bind_info {
    am_id texture_id;
    am_uint32 binding;
} amgl_texture_bind_info;

typedef struct amgl_uniform_bind_info {
    am_id uniform_id;
    void *data;
    am_uint32 binding;
} amgl_uniform_bind_info;

typedef struct amgl_bindings_info {
    struct {
        amgl_vertex_buffer_bind_info *info;
        size_t size;
    } vertex_buffers;

    struct {
        amgl_index_buffer_bind_info *info;
        size_t size;
    } index_buffers;

    struct {
        amgl_uniform_bind_info *info;
        size_t size;
    } uniforms;

    struct {
        amgl_texture_bind_info *info;
        size_t size;
    } textures;
} amgl_bindings_info;

//REVIEW: Should perhaps hold IDs instead?
typedef struct amgl_frame_cache {
    amgl_index_buffer index_buffer;
    size_t index_element_size;
    am_dyn_array(amgl_vertex_buffer) vertex_buffers;
    amgl_pipeline pipeline;
} amgl_frame_cache;

typedef struct amgl_draw_info {
    am_uint32 start;
    am_uint32 count;
} amgl_draw_info;

//Shaders
am_id amgl_shader_create(amgl_shader_info info);
void amgl_shader_destroy(am_id id);

//Vertex buffer
am_id amgl_vertex_buffer_create(amgl_vertex_buffer_info info);
//void amgl_vertex_buffer_update(am_int32 id, amgl_vertex_buffer_update_info update);
void amgl_vertex_buffer_destroy(am_id id);

//Index buffer
am_id amgl_index_buffer_create(amgl_index_buffer_info info);
void amgl_index_buffer_destroy(am_id id);

//Uniform
am_id amgl_uniform_create(amgl_uniform_info info);
//NOTE: Ignore for now
//void amgl_uniform_update(am_int32 id, amgl_uniform_info info);
void amgl_uniform_destroy(am_id id);
//NOTE: Ignore for now
//void amgl_uniform_bind(amgl_uniform *uniforms, am_uint32 num);

//Texture
am_id amgl_texture_create(amgl_texture_info info);
//NOTE: Ignore for now
//void amgl_texture_update(am_int32 id, amgl_texture_info info, amgl_texture_update_type type);

am_int32 amgl_texture_translate_format(amgl_texture_format format);
am_int32 amgl_texture_translate_wrap(amgl_texture_wrap wrap);
GLenum amgl_texture_translate_filter(amgl_texture_filter filter);
void amgl_texture_load_from_file(const char *path, amgl_texture_info *info, am_bool flip);
void amgl_texture_load_from_memory(const void *memory, amgl_texture_info *info, size_t size, am_bool flip);
void amgl_texture_destroy(am_id id);

//Framebuffer
am_id amgl_frame_buffer_create(amgl_frame_buffer_info info);
void amgl_frame_buffer_destroy(am_id id);

//Render pass
am_id amgl_render_pass_create(amgl_render_pass_info info);
void amgl_render_pass_destroy(am_id id);

//Pipeline
am_id amgl_pipeline_create(amgl_pipeline_info info);
void amgl_pipeline_destroy(am_int32 id);

//Various OGL
void amgl_init(); //Create arrays for shaders, vertex b, index b, frame b etc., init gl addresses
void amgl_terminate();
void amgl_set_viewport(am_int32 x, am_int32 y, am_int32 width, am_int32 height);
void amgl_vsync(am_id window_id, am_bool state);
void amgl_start_render_pass(am_int32 render_pass_id);
void amgl_end_render_pass(am_int32 render_pass_id);
void amgl_bind_pipeline(am_int32 pipeline_id);
void amgl_set_bindings(amgl_bindings_info *info);
void amgl_draw(amgl_draw_info *info);

//----------------------------------------------------------------------------//
//                                   END GL                                   //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                   ENGINE                                   //
//----------------------------------------------------------------------------//

typedef struct am_engine_info {
    void (*init)();
    void (*update)();
    void (*shutdown)();
    am_bool is_running;
    am_bool vsync_enabled;
    char win_name[AM_MAX_NAME_LENGTH];
    am_bool win_fullscreen;
    am_uint32 win_width;
    am_uint32 win_height;
    am_uint32 win_x;
    am_uint32 win_y;
} am_engine_info;

typedef struct amgl_ctx_data {
    am_packed_array(amgl_texture) textures; 
    am_packed_array(amgl_shader) shaders; 
    am_packed_array(amgl_vertex_buffer) vertex_buffers;
    am_packed_array(amgl_index_buffer) index_buffers; 
    am_packed_array(amgl_frame_buffer) frame_buffers; 
    am_packed_array(amgl_uniform) uniforms;
    am_packed_array(amgl_render_pass) render_passes;
    am_packed_array(amgl_pipeline) pipelines;
    amgl_frame_cache frame_cache;
} amgl_ctx_data;


typedef struct am_engine {
    void (*init)();
    void (*update)();
    void (*shutdown)();
    am_bool is_running;
    am_bool vsync_enabled;
    am_platform *platform;
    amgl_ctx_data ctx_data;
    //IDEA: A scene structure, maybe scenegraph?
    //am_audio audio; TODO: Implement
    //am_pfngl pfngl; TODO: Implement
} am_engine;

//The only one that should exist
am_engine *_am_engine_instance;

//TODO: Implement engine part
#define am_engine_get_instance() _am_engine_instance
#define am_engine_get_subsystem(sys) am_engine_get_instance()->sys

void am_engine_create(am_engine_info engine_info);
void am_engine_terminate();


//----------------------------------------------------------------------------//
//                                 END ENGINE                                 //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                    UTIL                                    //
//----------------------------------------------------------------------------//

char* am_util_read_file(const char *path);

//----------------------------------------------------------------------------//
//                                  END UTIL                                  //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                             DYNAMIC  ARRAY IMPL                            //
//----------------------------------------------------------------------------//

void am_dyn_array_init(void **array, size_t value_size) {
    if (*array == NULL) {
        am_dyn_array_header *data = (am_dyn_array_header*)malloc(value_size + sizeof(am_dyn_array_header));
        data->capacity = value_size;
        data->size = 0;
        *array = ((size_t*)data + 2);
    };
};

void am_dyn_array_resize(void **array, size_t add_size) {
    am_dyn_array_header *header;
    size_t new_capacity = 0, alloc_size = 0;
    if (am_dyn_array_get_capacity(*array) < am_dyn_array_get_size(*array) + add_size) {
        new_capacity = 2 * am_dyn_array_get_capacity(*array);
        alloc_size = new_capacity + sizeof(am_dyn_array_header);
        header = (am_dyn_array_header*)am_realloc(am_dyn_array_get_header(*array), alloc_size);
        if (!header) {
            printf("[FAIL] am_dyn_array_resize: Failed to allocate memory for array!\n");
            return;
        };
        header->capacity = new_capacity;
        *array = (size_t*)header + 2;
    };
};

void am_dyn_array_replace(void *array, void *values, size_t offset, size_t size) {
    assert(offset + size <= am_dyn_array_get_size(array));
    memcpy(array + offset, values, size);
};

void am_dyn_array_destroy(void *array) {
    am_dyn_array_header *header = am_dyn_array_get_header(array);
    header->capacity = AM_DYN_ARRAY_EMPTY_START_SLOTS;
    header->size = 0;
    am_free(header);
};

//----------------------------------------------------------------------------//
//                           END DYNAMIC  ARRAY IMPL                          //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                             PACKED  ARRAY IMPL                             //
//----------------------------------------------------------------------------//

void am_packed_array_alloc(void **array, size_t size) {
    if (*array == NULL) {
        *array = am_malloc(size);
        memset(*array, 0, size);

    };
};

//Not *technically* a function but it fits better here
#define am_packed_array_erase(array, id)\
    do {\
        am_uint32 index = am_packed_array_get_idx(array, id);\
        if (index == AM_PA_INVALID_INDEX) {\
            printf("[WARN] am_packed_array_erase: Element with ID %d does not exist!\n", id);\
            break;\
        };\
        if (!am_packed_array_has((array), (id))) {\
            printf("[WARN] am_packed_array_erase: Invalid id (%d)!\n", id);\
            break;\
        };\
        am_int32 last_element_id = -1;\
        if (am_packed_array_get_idx((array), (id)) == am_dyn_array_get_count((array)->elements) - 1) last_element_id = id;\
        else for (am_int32 i = 0; i < am_dyn_array_get_size(array) / sizeof(am_uint32); i++)\
            if (am_packed_array_get_idx((array), i) == am_dyn_array_get_count((array)->elements) - 1) {\
                last_element_id = i;\
                break;\
            };\
        am_dyn_array_replace((array)->elements, &((array)->elements[am_dyn_array_get_count((array)->elements) - 1]), index*sizeof((array)->elements[0]), sizeof((array)->elements[0]));\
        (array)->indices[last_element_id] = index;\
        (array)->indices[id] = AM_PA_INVALID_INDEX;\
        am_dyn_array_get_size((array)->elements) -= sizeof((array)->elements[0]);\
    } while(0);

//----------------------------------------------------------------------------//
//                           END PACKED  ARRAY IMPL                           //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                  MATH IMPL                                 //
//----------------------------------------------------------------------------//

static inline am_vec2 am_vec2_add(am_vec2 vec0, am_vec2 vec1) {
    am_vec2 v;
    v.x = vec0.x + vec1.x;
    v.y = vec0.y + vec1.y;
    return v;
};

static inline am_vec2 am_vec2_sub(am_vec2 vec0, am_vec2 vec1) {
    am_vec2 v;
    v.x = vec0.x - vec1.x;
    v.y = vec0.y - vec1.y;
    return v;
};

static inline am_vec2 am_vec2_mul(am_vec2 vec0, am_vec2 vec1) {
    am_vec2 v;
    v.x = vec0.x * vec1.x;
    v.y = vec0.y * vec1.y;
    return v;
};

static inline am_vec2 am_vec2_div(am_vec2 vec0, am_vec2 vec1){
    am_vec2 v;
    v.x = vec0.x / vec1.x;
    v.y = vec0.y / vec1.y;
    return v;
};

static inline am_vec2 am_vec2_scale(am_float32 scalar, am_vec2 vec){
    am_vec2 v;
    v.x = scalar * vec.x;
    v.y = scalar * vec.y;
    return v;
};

//----------------------------------------------------------------------------//
//                               END MATH IMPL                                //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                PLATFORM IMPL                               //
//----------------------------------------------------------------------------//


#if defined(AM_LINUX)
am_key_map am_platform_translate_keysym(KeySym *key_syms, am_int32 width) {
    if (width > 1) {
        switch (key_syms[1]) {
            case XK_KP_0:           return AM_KEYCODE_NUMPAD_0;
            case XK_KP_1:           return AM_KEYCODE_NUMPAD_1;
            case XK_KP_2:           return AM_KEYCODE_NUMPAD_2;
            case XK_KP_3:           return AM_KEYCODE_NUMPAD_3;
            case XK_KP_4:           return AM_KEYCODE_NUMPAD_4;
            case XK_KP_5:           return AM_KEYCODE_NUMPAD_5;
            case XK_KP_6:           return AM_KEYCODE_NUMPAD_6;
            case XK_KP_7:           return AM_KEYCODE_NUMPAD_7;
            case XK_KP_8:           return AM_KEYCODE_NUMPAD_8;
            case XK_KP_9:           return AM_KEYCODE_NUMPAD_9;
            case XK_KP_Separator:
            case XK_KP_Decimal:     return AM_KEYCODE_NUMPAD_DECIMAL;
            case XK_KP_Equal:       return AM_KEYCODE_NUMPAD_EQUAL;
            case XK_KP_Enter:       return AM_KEYCODE_NUMPAD_ENTER;
            default:                break;
        };
    };

    switch (key_syms[0]) {
        case XK_Escape:         return AM_KEYCODE_ESCAPE;
        case XK_Tab:            return AM_KEYCODE_TAB;
        case XK_Shift_L:        return AM_KEYCODE_LEFT_SHIFT;
        case XK_Shift_R:        return AM_KEYCODE_RIGHT_SHIFT;
        case XK_Control_L:      return AM_KEYCODE_LEFT_CONTROL;
        case XK_Control_R:      return AM_KEYCODE_RIGHT_CONTROL;
        case XK_Alt_L:          return AM_KEYCODE_LEFT_ALT;
        case XK_Alt_R:          return AM_KEYCODE_RIGHT_ALT;
        case XK_Super_L:        return AM_KEYCODE_LEFT_META;
        case XK_Super_R:        return AM_KEYCODE_RIGHT_META;
        case XK_Menu:           return AM_KEYCODE_MENU;
        case XK_Num_Lock:       return AM_KEYCODE_NUMPAD_NUM;
        case XK_Caps_Lock:      return AM_KEYCODE_CAPS_LOCK;
        case XK_Print:          return AM_KEYCODE_PRINT_SCREEN;
        case XK_Scroll_Lock:    return AM_KEYCODE_SCROLL_LOCK;
        case XK_Pause:          return AM_KEYCODE_PAUSE;
        case XK_Delete:         return AM_KEYCODE_DELETE;
        case XK_BackSpace:      return AM_KEYCODE_BACKSPACE;
        case XK_Return:         return AM_KEYCODE_ENTER;
        case XK_Home:           return AM_KEYCODE_HOME;
        case XK_End:            return AM_KEYCODE_END;
        case XK_Page_Up:        return AM_KEYCODE_PAGE_UP;
        case XK_Page_Down:      return AM_KEYCODE_PAGE_DOWN;
        case XK_Insert:         return AM_KEYCODE_INSERT;
        case XK_Left:           return AM_KEYCODE_LEFT_ARROW;
        case XK_Right:          return AM_KEYCODE_RIGHT_ARROW;
        case XK_Down:           return AM_KEYCODE_DOWN_ARROW;
        case XK_Up:             return AM_KEYCODE_UP_ARROW;
        case XK_F1:             return AM_KEYCODE_F1;
        case XK_F2:             return AM_KEYCODE_F2;
        case XK_F3:             return AM_KEYCODE_F3;
        case XK_F4:             return AM_KEYCODE_F4;
        case XK_F5:             return AM_KEYCODE_F5;
        case XK_F6:             return AM_KEYCODE_F6;
        case XK_F7:             return AM_KEYCODE_F7;
        case XK_F8:             return AM_KEYCODE_F8;
        case XK_F9:             return AM_KEYCODE_F9;
        case XK_F10:            return AM_KEYCODE_F10;
        case XK_F11:            return AM_KEYCODE_F11;
        case XK_F12:            return AM_KEYCODE_F12;
        case XK_F13:            return AM_KEYCODE_F13;
        case XK_F14:            return AM_KEYCODE_F14;
        case XK_F15:            return AM_KEYCODE_F15;
        case XK_F16:            return AM_KEYCODE_F16;
        case XK_F17:            return AM_KEYCODE_F17;
        case XK_F18:            return AM_KEYCODE_F18;
        case XK_F19:            return AM_KEYCODE_F19;
        case XK_F20:            return AM_KEYCODE_F20;
        case XK_F21:            return AM_KEYCODE_F21;
        case XK_F22:            return AM_KEYCODE_F22;
        case XK_F23:            return AM_KEYCODE_F23;
        case XK_F24:            return AM_KEYCODE_F24;
        case XK_F25:            return AM_KEYCODE_F25;
        case XK_KP_Divide:      return AM_KEYCODE_NUMPAD_DIVIDE;
        case XK_KP_Multiply:    return AM_KEYCODE_NUMPAD_MULTIPLY;
        case XK_KP_Subtract:    return AM_KEYCODE_NUMPAD_SUBTRACT;
        case XK_KP_Add:         return AM_KEYCODE_NUMPAD_ADD;
        case XK_KP_Insert:      return AM_KEYCODE_NUMPAD_0;
        case XK_KP_End:         return AM_KEYCODE_NUMPAD_1;
        case XK_KP_Down:        return AM_KEYCODE_NUMPAD_2;
        case XK_KP_Page_Down:   return AM_KEYCODE_NUMPAD_3;
        case XK_KP_Left:        return AM_KEYCODE_NUMPAD_4;
        case XK_KP_Right:       return AM_KEYCODE_NUMPAD_6;
        case XK_KP_Home:        return AM_KEYCODE_NUMPAD_7;
        case XK_KP_Up:          return AM_KEYCODE_NUMPAD_8;
        case XK_KP_Page_Up:     return AM_KEYCODE_NUMPAD_9;
        case XK_KP_Delete:      return AM_KEYCODE_NUMPAD_DECIMAL;
        case XK_KP_Equal:       return AM_KEYCODE_NUMPAD_EQUAL;
        case XK_KP_Enter:       return AM_KEYCODE_NUMPAD_ENTER;
        case XK_a:              return AM_KEYCODE_A;
        case XK_b:              return AM_KEYCODE_B;
        case XK_c:              return AM_KEYCODE_C;
        case XK_d:              return AM_KEYCODE_D;
        case XK_e:              return AM_KEYCODE_E;
        case XK_f:              return AM_KEYCODE_F;
        case XK_g:              return AM_KEYCODE_G;
        case XK_h:              return AM_KEYCODE_H;
        case XK_i:              return AM_KEYCODE_I;
        case XK_j:              return AM_KEYCODE_J;
        case XK_k:              return AM_KEYCODE_K;
        case XK_l:              return AM_KEYCODE_L;
        case XK_m:              return AM_KEYCODE_M;
        case XK_n:              return AM_KEYCODE_N;
        case XK_o:              return AM_KEYCODE_O;
        case XK_p:              return AM_KEYCODE_P;
        case XK_q:              return AM_KEYCODE_Q;
        case XK_r:              return AM_KEYCODE_R;
        case XK_s:              return AM_KEYCODE_S;
        case XK_t:              return AM_KEYCODE_T;
        case XK_u:              return AM_KEYCODE_U;
        case XK_v:              return AM_KEYCODE_V;
        case XK_w:              return AM_KEYCODE_W;
        case XK_x:              return AM_KEYCODE_X;
        case XK_y:              return AM_KEYCODE_Y;
        case XK_z:              return AM_KEYCODE_Z;
        case XK_1:              return AM_KEYCODE_1;
        case XK_2:              return AM_KEYCODE_2;
        case XK_3:              return AM_KEYCODE_3;
        case XK_4:              return AM_KEYCODE_4;
        case XK_5:              return AM_KEYCODE_5;
        case XK_6:              return AM_KEYCODE_6;
        case XK_7:              return AM_KEYCODE_7;
        case XK_8:              return AM_KEYCODE_8;
        case XK_9:              return AM_KEYCODE_9;
        case XK_0:              return AM_KEYCODE_0;
        case XK_space:          return AM_KEYCODE_SPACE;
        case XK_minus:          return AM_KEYCODE_MINUS;
        case XK_equal:          return AM_KEYCODE_EQUAL;
        case XK_bracketleft:    return AM_KEYCODE_LEFT_SQUARE_BRACKET;
        case XK_bracketright:   return AM_KEYCODE_RIGHT_SQUARE_BRACKET;
        case XK_backslash:      return AM_KEYCODE_BACKSLASH;
        case XK_semicolon:      return AM_KEYCODE_SEMICOLON;
        case XK_apostrophe:     return AM_KEYCODE_APOSTROPHE;
        case XK_grave:          return AM_KEYCODE_ACCENT_GRAVE;
        case XK_comma:          return AM_KEYCODE_COMMA;
        case XK_period:         return AM_KEYCODE_PERIOD;
        case XK_slash:          return AM_KEYCODE_SLASH;
        default:                return AM_KEYCODE_INVALID;
    };
};
#endif

am_mouse_map am_platform_translate_button(am_uint32 button) {
    switch (button) {
        case 1: return AM_MOUSE_BUTTON_LEFT;
        case 2: return AM_MOUSE_BUTTON_MIDDLE;
        case 3: return AM_MOUSE_BUTTON_RIGHT;
        default: return AM_MOUSE_BUTTON_INVALID;
    };
};

am_platform *am_platform_create() {
    am_platform *platform = (am_platform*)am_malloc(sizeof(am_platform));
    if (platform == NULL) printf("[FAIL] am_platform_create: Could not allocate memory!\n");
    //REVIEW
    assert(platform != NULL);
    platform->windows = NULL;
    am_packed_array_init(platform->windows, sizeof(am_window));

    #if defined(AM_LINUX)
    platform->display = XOpenDisplay(NULL);
    memset(platform->input.keycodes, -1, sizeof(platform->input.keycodes));
    am_int32 min, max;
    XDisplayKeycodes(platform->display, &min, &max);
    am_int32 width;
    KeySym *key_syms = XGetKeyboardMapping(platform->display, min, max - min + 1, &width);
    for (am_int32 i = min; i < max; i++) platform->input.keycodes[i] = am_platform_translate_keysym(&key_syms[(i-min)*width], width);
    XFree(key_syms);
    #else
    platform->input.keycodes[0x00B] = AM_KEYCODE_0;
    platform->input.keycodes[0x002] = AM_KEYCODE_1;
    platform->input.keycodes[0x003] = AM_KEYCODE_2;
    platform->input.keycodes[0x004] = AM_KEYCODE_3;
    platform->input.keycodes[0x005] = AM_KEYCODE_4;
    platform->input.keycodes[0x006] = AM_KEYCODE_5;
    platform->input.keycodes[0x007] = AM_KEYCODE_6;
    platform->input.keycodes[0x008] = AM_KEYCODE_7;
    platform->input.keycodes[0x009] = AM_KEYCODE_8;
    platform->input.keycodes[0x00A] = AM_KEYCODE_9;
    platform->input.keycodes[0x01E] = AM_KEYCODE_A;
    platform->input.keycodes[0x030] = AM_KEYCODE_B;
    platform->input.keycodes[0x02E] = AM_KEYCODE_C;
    platform->input.keycodes[0x020] = AM_KEYCODE_D;
    platform->input.keycodes[0x012] = AM_KEYCODE_E;
    platform->input.keycodes[0x021] = AM_KEYCODE_F;
    platform->input.keycodes[0x022] = AM_KEYCODE_G;
    platform->input.keycodes[0x023] = AM_KEYCODE_H;
    platform->input.keycodes[0x017] = AM_KEYCODE_I;
    platform->input.keycodes[0x024] = AM_KEYCODE_J;
    platform->input.keycodes[0x025] = AM_KEYCODE_K;
    platform->input.keycodes[0x026] = AM_KEYCODE_L;
    platform->input.keycodes[0x032] = AM_KEYCODE_M;
    platform->input.keycodes[0x031] = AM_KEYCODE_N;
    platform->input.keycodes[0x018] = AM_KEYCODE_O;
    platform->input.keycodes[0x019] = AM_KEYCODE_P;
    platform->input.keycodes[0x010] = AM_KEYCODE_Q;
    platform->input.keycodes[0x013] = AM_KEYCODE_R;
    platform->input.keycodes[0x01F] = AM_KEYCODE_S;
    platform->input.keycodes[0x014] = AM_KEYCODE_T;
    platform->input.keycodes[0x016] = AM_KEYCODE_U;
    platform->input.keycodes[0x02F] = AM_KEYCODE_V;
    platform->input.keycodes[0x011] = AM_KEYCODE_W;
    platform->input.keycodes[0x02D] = AM_KEYCODE_X;
    platform->input.keycodes[0x015] = AM_KEYCODE_Y;
    platform->input.keycodes[0x02C] = AM_KEYCODE_Z;
    platform->input.keycodes[0x028] = AM_KEYCODE_APOSTROPHE;
    platform->input.keycodes[0x02B] = AM_KEYCODE_BACKSLASH;
    platform->input.keycodes[0x033] = AM_KEYCODE_COMMA;
    platform->input.keycodes[0x00D] = AM_KEYCODE_EQUAL;
    platform->input.keycodes[0x029] = AM_KEYCODE_ACCENT_GRAVE;
    platform->input.keycodes[0x01A] = AM_KEYCODE_LEFT_SQUARE_BRACKET;
    platform->input.keycodes[0x00C] = AM_KEYCODE_MINUS;
    platform->input.keycodes[0x034] = AM_KEYCODE_PERIOD;
    platform->input.keycodes[0x01B] = AM_KEYCODE_RIGHT_SQUARE_BRACKET;
    platform->input.keycodes[0x027] = AM_KEYCODE_SEMICOLON;
    platform->input.keycodes[0x035] = AM_KEYCODE_SLASH;
    platform->input.keycodes[0x00E] = AM_KEYCODE_BACKSPACE;
    platform->input.keycodes[0x153] = AM_KEYCODE_DELETE;
    platform->input.keycodes[0x14F] = AM_KEYCODE_END;
    platform->input.keycodes[0x01C] = AM_KEYCODE_ENTER;
    platform->input.keycodes[0x001] = AM_KEYCODE_ESCAPE;
    platform->input.keycodes[0x147] = AM_KEYCODE_HOME;
    platform->input.keycodes[0x152] = AM_KEYCODE_INSERT;
    platform->input.keycodes[0x15D] = AM_KEYCODE_MENU;
    platform->input.keycodes[0x151] = AM_KEYCODE_PAGE_DOWN;
    platform->input.keycodes[0x149] = AM_KEYCODE_PAGE_UP;
    platform->input.keycodes[0x045] = AM_KEYCODE_PAUSE;
    platform->input.keycodes[0x146] = AM_KEYCODE_PAUSE;
    platform->input.keycodes[0x039] = AM_KEYCODE_SPACE;
    platform->input.keycodes[0x00F] = AM_KEYCODE_TAB;
    platform->input.keycodes[0x03A] = AM_KEYCODE_CAPS_LOCK;
    platform->input.keycodes[0x145] = AM_KEYCODE_NUMPAD_NUM;
    platform->input.keycodes[0x046] = AM_KEYCODE_SCROLL_LOCK;
    platform->input.keycodes[0x03B] = AM_KEYCODE_F1;
    platform->input.keycodes[0x03C] = AM_KEYCODE_F2;
    platform->input.keycodes[0x03D] = AM_KEYCODE_F3;
    platform->input.keycodes[0x03E] = AM_KEYCODE_F4;
    platform->input.keycodes[0x03F] = AM_KEYCODE_F5;
    platform->input.keycodes[0x040] = AM_KEYCODE_F6;
    platform->input.keycodes[0x041] = AM_KEYCODE_F7;
    platform->input.keycodes[0x042] = AM_KEYCODE_F8;
    platform->input.keycodes[0x043] = AM_KEYCODE_F9;
    platform->input.keycodes[0x044] = AM_KEYCODE_F10;
    platform->input.keycodes[0x057] = AM_KEYCODE_F11;
    platform->input.keycodes[0x058] = AM_KEYCODE_F12;
    platform->input.keycodes[0x064] = AM_KEYCODE_F13;
    platform->input.keycodes[0x065] = AM_KEYCODE_F14;
    platform->input.keycodes[0x066] = AM_KEYCODE_F15;
    platform->input.keycodes[0x067] = AM_KEYCODE_F16;
    platform->input.keycodes[0x068] = AM_KEYCODE_F17;
    platform->input.keycodes[0x069] = AM_KEYCODE_F18;
    platform->input.keycodes[0x06A] = AM_KEYCODE_F19;
    platform->input.keycodes[0x06B] = AM_KEYCODE_F20;
    platform->input.keycodes[0x06C] = AM_KEYCODE_F21;
    platform->input.keycodes[0x06D] = AM_KEYCODE_F22;
    platform->input.keycodes[0x06E] = AM_KEYCODE_F23;
    platform->input.keycodes[0x076] = AM_KEYCODE_F24;
    platform->input.keycodes[0x038] = AM_KEYCODE_LEFT_ALT;
    platform->input.keycodes[0x01D] = AM_KEYCODE_LEFT_CONTROL;
    platform->input.keycodes[0x02A] = AM_KEYCODE_LEFT_SHIFT;
    platform->input.keycodes[0x15B] = AM_KEYCODE_LEFT_META;
    platform->input.keycodes[0x137] = AM_KEYCODE_PRINT_SCREEN;
    platform->input.keycodes[0x138] = AM_KEYCODE_RIGHT_ALT;
    platform->input.keycodes[0x11D] = AM_KEYCODE_RIGHT_CONTROL;
    platform->input.keycodes[0x036] = AM_KEYCODE_RIGHT_SHIFT;
    platform->input.keycodes[0x15C] = AM_KEYCODE_RIGHT_META;
    platform->input.keycodes[0x150] = AM_KEYCODE_DOWN_ARROW;
    platform->input.keycodes[0x14B] = AM_KEYCODE_LEFT_ARROW;
    platform->input.keycodes[0x14D] = AM_KEYCODE_RIGHT_ARROW;
    platform->input.keycodes[0x148] = AM_KEYCODE_UP_ARROW;
    platform->input.keycodes[0x052] = AM_KEYCODE_NUMPAD_0;
    platform->input.keycodes[0x04F] = AM_KEYCODE_NUMPAD_1;
    platform->input.keycodes[0x050] = AM_KEYCODE_NUMPAD_2;
    platform->input.keycodes[0x051] = AM_KEYCODE_NUMPAD_3;
    platform->input.keycodes[0x04B] = AM_KEYCODE_NUMPAD_4;
    platform->input.keycodes[0x04C] = AM_KEYCODE_NUMPAD_5;
    platform->input.keycodes[0x04D] = AM_KEYCODE_NUMPAD_6;
    platform->input.keycodes[0x047] = AM_KEYCODE_NUMPAD_7;
    platform->input.keycodes[0x048] = AM_KEYCODE_NUMPAD_8;
    platform->input.keycodes[0x049] = AM_KEYCODE_NUMPAD_9;
    platform->input.keycodes[0x04E] = AM_KEYCODE_NUMPAD_ADD;
    platform->input.keycodes[0x053] = AM_KEYCODE_NUMPAD_DECIMAL;
    platform->input.keycodes[0x135] = AM_KEYCODE_NUMPAD_DIVIDE;
    platform->input.keycodes[0x11C] = AM_KEYCODE_NUMPAD_ENTER;
    platform->input.keycodes[0x059] = AM_KEYCODE_NUMPAD_EQUAL;
    platform->input.keycodes[0x037] = AM_KEYCODE_NUMPAD_MULTIPLY;
    platform->input.keycodes[0x04A] = AM_KEYCODE_NUMPAD_SUBTRACT;

    WNDCLASS window_class = {0};
	window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    //REVIEW: Might not be necessary, come back after OpenGL is implemented
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.hInstance = GetModuleHandle(NULL);
	window_class.lpfnWndProc = am_platform_event_handler;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.lpszMenuName = NULL;
	window_class.hbrBackground = NULL;
    //HACK: Colored here just for visuals
    window_class.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));
	window_class.lpszClassName = "AM_ROOT";
	if (!RegisterClass(&window_class)) {
        printf("[FAIL] Failed to register root class!\n");
        return NULL;
    };
    //HACK: Colored here just for visuals
    window_class.hbrBackground = CreateSolidBrush(RGB(0, 0, 255));
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_PARENTDC;
    window_class.lpszClassName = "AM_CHILD";
    if (!RegisterClass(&window_class)) {
        printf("[FAIL] Failed to register child class!\n");
        return NULL;
    };
    #endif

    platform->input.wheel_delta = 0;
    platform->input.mouse_x = 0;
    platform->input.mouse_y = 0;

    memset(platform->input.keyboard_map, 0, sizeof(platform->input.keyboard_map));
    memset(platform->input.prev_keyboard_map, 0, sizeof(platform->input.prev_keyboard_map));
    memset(platform->input.mouse_map, 0, sizeof(platform->input.mouse_map));
    memset(platform->input.mouse_map, 0, sizeof(platform->input.prev_mouse_map));

    am_platform_set_key_callback(platform, am_platform_key_callback_default);
    am_platform_set_mouse_button_callback(platform, am_platform_mouse_button_callback_default);
    am_platform_set_mouse_motion_callback(platform, am_platform_mouse_motion_callback_default);
    am_platform_set_mouse_scroll_callback(platform, am_platform_mouse_scroll_callback_default);
    am_platform_set_window_motion_callback(platform, am_platform_window_motion_callback_default);
    am_platform_set_window_size_callback(platform, am_platform_window_size_callback_default);
    printf("[OK] Platform init successful!\n");
    return platform;
};

void am_platform_poll_events() {
    #if defined(AM_LINUX)
    am_platform *platform = am_engine_get_subsystem(platform);
    XEvent xevent;
    while (XPending(platform->display)) {
        XNextEvent(platform->display, &xevent);
        am_platform_event_handler(&xevent);  
    };
    #else
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
    };
    #endif 
};

//REVIEW: ONLY LINUX SIDE TESTED
#if defined(AM_LINUX) 
void am_platform_event_handler(XEvent *xevent) {
    am_platform *platform = am_engine_get_subsystem(platform);
    am_uint64 handle = xevent->xany.window;
    am_id id = -1;

    for (am_uint32 i = 0; i < am_packed_array_get_count(platform->windows); i++)
        if (platform->windows->elements[i].handle == handle) 
            id = platform->windows->elements[i].id;

    switch (xevent->type) {
        case KeyPress: {
            am_key_map key = platform->input.keycodes[xevent->xkey.keycode];
            platform->callbacks.am_platform_key_callback(id, key, AM_EVENT_KEY_PRESS);  
            break;
        };
        case KeyRelease: {
            am_key_map key = platform->input.keycodes[xevent->xkey.keycode];
            platform->callbacks.am_platform_key_callback(id, key, AM_EVENT_KEY_RELEASE);  
            break;
        };
        case ButtonPress: {
            am_mouse_map button = am_platform_translate_button(xevent->xbutton.button);
            if (button == AM_MOUSE_BUTTON_INVALID) {
                if (xevent->xbutton.button == 4) {
                    platform->callbacks.am_platform_mouse_scroll_callback(id, AM_EVENT_MOUSE_SCROLL_UP);
                    break;
                };
                if (xevent->xbutton.button == 5) {
                    platform->callbacks.am_platform_mouse_scroll_callback(id, AM_EVENT_MOUSE_SCROLL_DOWN);
                    break;
                };
            };
            platform->callbacks.am_platform_mouse_button_callback(id, button, AM_EVENT_MOUSE_BUTTON_PRESS);
            break;
        };
        case ButtonRelease: {
            am_mouse_map button = am_platform_translate_button(xevent->xbutton.button);
            platform->callbacks.am_platform_mouse_button_callback(id, button, AM_EVENT_MOUSE_BUTTON_RELEASE);
            break;
        };
        case MotionNotify: {
            platform->callbacks.am_platform_mouse_motion_callback(id, xevent->xbutton.x, xevent->xbutton.y, AM_EVENT_MOUSE_MOTION);
            break;
        };  
        case ConfigureNotify: {
            am_window *window = am_packed_array_get_ptr(platform->windows, id);
            if (window->height != xevent->xconfigure.height || window->width != xevent->xconfigure.width) {
                platform->callbacks.am_platform_window_size_callback(id, xevent->xconfigure.width, xevent->xconfigure.height, AM_EVENT_WINDOW_SIZE);
            };
            if (window->x != xevent->xconfigure.x || window->y != xevent->xconfigure.y) {
                platform->callbacks.am_platform_window_motion_callback(id, xevent->xconfigure.x, xevent->xconfigure.y, AM_EVENT_WINDOW_MOTION);
            };
            break;
        };
        case DestroyNotify: {
            printf("Destroying window %d!\n", id);
            am_packed_array_erase(platform->windows, id);
            //for (am_int32 i = index; i < platform->windows.length; i++) --am_dyn_array_data_retrieve(&platform->windows, am_window, i)->id;
            //am_free(am_platform_window_lookup_by_handle(xevent->xclient.window));

            am_bool check_no_root = true;
            for (am_int32 i = 0; i < am_packed_array_get_count(platform->windows); i++) 
                if (platform->windows->elements[i].parent == AM_WINDOW_DEFAULT_PARENT) {
                    printf("check no root false");
                    check_no_root = false;
                    break;
                };

            if (check_no_root) am_engine_get_instance()->is_running = false;
            break;
        };
        case ClientMessage: {
            if (xevent->xclient.data.l[0] == XInternAtom(platform->display, "WM_DELETE_WINDOW", false)) {
                XUnmapWindow(platform->display, handle);
                XDestroyWindow(platform->display, handle);
            };
            break;
        };
        default: break;
    };
};
#else
LRESULT CALLBACK am_platform_event_handler(HWND handle, am_uint32 event, WPARAM wparam, LPARAM lparam) {
    am_platform *platform = am_engine_get_subsystem(platform);
    am_uint64 window_handle = (am_uint64) handle;
    am_int32 id = am_platform_window_lookup_by_handle(handle)->id;
    switch (event) {
        case WM_KEYDOWN: {
            am_key_map key = platform->input.keycodes[(HIWORD(lparam) & (KF_EXTENDED | 0xff))];
            platform->callbacks.am_platform_key_callback(window_handle, key, AM_EVENT_KEY_PRESS);
            break;
        };
        case WM_KEYUP: {
            am_key_map key = platform->input.keycodes[(HIWORD(lparam) & (KF_EXTENDED | 0xff))];
            platform->callbacks.am_platform_key_callback(window_handle, key, AM_EVENT_KEY_RELEASE);
            break;
        };
        case WM_LBUTTONDOWN: {
            platform->callbacks.am_platform_mouse_button_callback(window_handle, AM_MOUSE_BUTTON_LEFT, AM_EVENT_MOUSE_BUTTON_PRESS);
            break;
        };
        case WM_LBUTTONUP: {
            platform->callbacks.am_platform_mouse_button_callback(window_handle, AM_MOUSE_BUTTON_LEFT, AM_EVENT_MOUSE_BUTTON_RELEASE);
            break;  
        };
        case WM_MBUTTONDOWN: {
            platform->callbacks.am_platform_mouse_button_callback(window_handle, AM_MOUSE_BUTTON_MIDDLE, AM_EVENT_MOUSE_BUTTON_PRESS);
            break;
        };
        case WM_MBUTTONUP: {
            platform->callbacks.am_platform_mouse_button_callback(window_handle, AM_MOUSE_BUTTON_MIDDLE, AM_EVENT_MOUSE_BUTTON_RELEASE);
            break;  
        };
        case WM_RBUTTONDOWN: {
            platform->callbacks.am_platform_mouse_button_callback(window_handle, AM_MOUSE_BUTTON_RIGHT, AM_EVENT_MOUSE_BUTTON_PRESS);
            break;
        };
        case WM_RBUTTONUP: {
            platform->callbacks.am_platform_mouse_button_callback(window_handle, AM_MOUSE_BUTTON_RIGHT, AM_EVENT_MOUSE_BUTTON_RELEASE);
            break;  
        };
        case WM_MOUSEWHEEL: {
            ++platform->input.wheel_delta;
            break;
        };
        case WM_MOUSEMOVE: {
            platform->callbacks.am_platform_mouse_motion_callback(window_handle, LOWORD(lparam), HIWORD(lparam), AM_EVENT_MOUSE_MOTION);
            break;
        };
        case WM_GETMINMAXINFO: {
            MINMAXINFO *info = (MINMAXINFO*)lparam;
            info->ptMinTrackSize.x = 1;
            info->ptMinTrackSize.y = 1;
            break;
        };
        case WM_SIZE: {
            if (am_platform_window_exists(id)) 
                platform->callbacks.am_platform_window_size_callback(window_handle, LOWORD(lparam), HIWORD(lparam), AM_EVENT_WINDOW_SIZE);
            break;
        };
        case WM_MOVE: {
            if (am_platform_window_exists(id)) 
                platform->callbacks.am_platform_window_motion_callback(window_handle, LOWORD(lparam), HIWORD(lparam), AM_EVENT_WINDOW_MOTION);

            break;
        };
        case WM_DESTROY: {
            am_int32 index = am_platform_window_index_lookup(id);
            am_dyn_array_erase(&platform->windows, index, 1);
            //for (am_int32 i = index; i < platform->windows.length; i++) --am_dyn_array_data_retrieve(&platform->windows, am_window, index)->id;
            //am_free(am_platform_window_lookup(id));
            
            am_bool check_no_root = true;
            for (am_int32 i = 0; i < platform->windows.length; i++) 
                if (am_dyn_array_data_retrieve(&platform->windows, am_window, i)->info.parent == AM_WINDOW_DEFAULT_PARENT) {
                    check_no_root = false;
                    break;
                };

            //HACK: Temporary var. to quit loop, should send proper closing signal
            if (check_no_root) temp_check = false;
            break;
        };
        default: break;
    };
    return DefWindowProc(handle, event, wparam, lparam);
};
#endif

void am_platform_update(am_platform *platform) {
    memcpy(platform->input.prev_mouse_map, platform->input.mouse_map, sizeof(platform->input.mouse_map));
    memcpy(platform->input.prev_keyboard_map, platform->input.keyboard_map, sizeof(platform->input.keyboard_map));
    platform->input.wheel_delta = 0;
    platform->input.mouse_moved = false;

    am_platform_poll_events();
};

void am_platform_terminate(am_platform *platform) {
    for (am_int32 i = 0; i < am_packed_array_get_count(platform->windows); i++) am_platform_window_destroy(platform->windows->elements[i].id);

    #if defined(AM_LINUX)
    //This sends the proper closing xevents
    am_platform_update(am_engine_get_subsystem(platform));
    #else
    UnregisterClass(AM_ROOT_WIN_CLASS, GetModuleHandle(NULL));
    UnregisterClass(AM_CHILD_WIN_CLASS, GetModuleHandle(NULL));
    #endif
    
    am_packed_array_destroy(platform->windows);
    am_free(platform);
};

void am_platform_key_press(am_key_map key) {
    if (key >= AM_KEYCODE_COUNT) return;
    am_platform *platform = am_engine_get_subsystem(platform);
    platform->input.keyboard_map[key] = true;
};

void am_platform_key_release(am_key_map key) {
    if (key >= AM_KEYCODE_COUNT) return;
    am_platform *platform = am_engine_get_subsystem(platform);
    platform->input.keyboard_map[key] = false; 
}; 

am_bool am_platform_key_pressed(am_key_map key) {
    if (key >= AM_KEYCODE_COUNT) return false;
    am_platform *platform = am_engine_get_subsystem(platform);
    return platform->input.keyboard_map[key] && !platform->input.prev_keyboard_map[key]; 
}; 

am_bool am_platform_key_down(am_key_map key) {
    if (key >= AM_KEYCODE_COUNT) return false;
    am_platform *platform = am_engine_get_subsystem(platform);
    return platform->input.keyboard_map[key] && platform->input.prev_keyboard_map[key];
}; 

am_bool am_platform_key_released(am_key_map key) {
    if (key >= AM_KEYCODE_COUNT) return false;
    am_platform *platform = am_engine_get_subsystem(platform);
    return !platform->input.keyboard_map[key] && platform->input.prev_keyboard_map[key]; 
}; 

am_bool am_platform_key_up(am_key_map key) {
    if (key >= AM_KEYCODE_COUNT) return false;
    am_platform *platform = am_engine_get_subsystem(platform);
    return !platform->input.keyboard_map[key];
};

void am_platform_mouse_button_press(am_mouse_map button) {
    if (button >= AM_MOUSE_BUTTON_COUNT) return;
    am_platform *platform = am_engine_get_subsystem(platform);
    platform->input.mouse_map[button] = true;
}; 

void am_platform_mouse_button_release(am_mouse_map button) {
    if (button >= AM_MOUSE_BUTTON_COUNT) return;
    am_platform *platform = am_engine_get_subsystem(platform);
    platform->input.mouse_map[button] = false;

};

am_bool am_platform_mouse_button_pressed(am_mouse_map button) {
    if (button >= AM_MOUSE_BUTTON_COUNT) return false;
    am_platform *platform = am_engine_get_subsystem(platform);
    return platform->input.mouse_map[button] && !platform->input.prev_mouse_map[button];
};

am_bool am_platform_mouse_button_down(am_mouse_map button) {
    if (button >= AM_MOUSE_BUTTON_COUNT) return false;
    am_platform *platform = am_engine_get_subsystem(platform);
    return platform->input.mouse_map[button] && platform->input.prev_mouse_map[button];
};

am_bool am_platform_mouse_button_released(am_mouse_map button) {
    if (button >= AM_MOUSE_BUTTON_COUNT) return false;
    am_platform *platform = am_engine_get_subsystem(platform);
    return !platform->input.mouse_map[button] && platform->input.prev_mouse_map[button];
};

am_bool am_platform_mouse_button_up(am_mouse_map button) {
    if (button >= AM_MOUSE_BUTTON_COUNT) return false;
    am_platform *platform = am_engine_get_subsystem(platform);
    return !platform->input.mouse_map[button];  
};

void am_platform_mouse_position(am_uint32 *x, am_uint32 *y) {
    am_platform *platform = am_engine_get_subsystem(platform);
    *x = platform->input.mouse_x;
    *y = platform->input.mouse_y;
};

am_vec2 am_platform_mouse_positionv() {
    am_platform *platform = am_engine_get_subsystem(platform);
    am_vec2 posv;
    posv.x = (am_float32)platform->input.mouse_x;
    posv.y = (am_float32)platform->input.mouse_y;
    return posv;
};

am_int32 am_platform_mouse_wheel_delta() {
    am_platform *platform = am_engine_get_subsystem(platform);
    return platform->input.wheel_delta;
};

am_bool am_platform_mouse_moved() {
    am_platform *platform = am_engine_get_subsystem(platform);
    return platform->input.mouse_moved;
};

void am_platform_key_callback_default(am_id id, am_key_map key, am_platform_events event) {
    am_platform *platform = am_engine_get_subsystem(platform);
    switch (event) {
        case AM_EVENT_KEY_PRESS: {
            platform->input.keyboard_map[key] = true;
            break;
        };
        case AM_EVENT_KEY_RELEASE: {
            platform->input.keyboard_map[key] = false;
            break;
        };
        default: break;
    };
};

void am_platform_mouse_button_callback_default(am_id id, am_mouse_map button, am_platform_events event) {
    am_platform *platform = am_engine_get_subsystem(platform);
    switch (event) {
        case AM_EVENT_MOUSE_BUTTON_PRESS: {
            platform->input.mouse_map[button] = true;
            break;
        };
        case AM_EVENT_MOUSE_BUTTON_RELEASE: {
            platform->input.mouse_map[button] = false;
            break;
        };
        default: break;
    };
};

void am_platform_mouse_motion_callback_default(am_id id, am_int32 x, am_int32 y, am_platform_events event) {
    am_platform *platform = am_engine_get_subsystem(platform);
    platform->input.mouse_moved = true;
    platform->input.mouse_x = x;
    platform->input.mouse_y = y;
};

void am_platform_mouse_scroll_callback_default(am_id id, am_platform_events event) {
    am_platform *platform = am_engine_get_subsystem(platform);
    switch (event) {
        case AM_EVENT_MOUSE_SCROLL_UP: {
            ++platform->input.wheel_delta;
            break;
        };
        case AM_EVENT_MOUSE_SCROLL_DOWN: {
            --platform->input.wheel_delta;
            break;
        };
        default: break;
    };
};

void am_platform_window_size_callback_default(am_id id, am_uint32 width, am_uint32 height, am_platform_events event) {
    am_platform *platform = am_engine_get_subsystem(platform);
    am_window *window = am_packed_array_get_ptr(platform->windows, id);
    if (!window) {
        printf("[FAIL] am_platform_window_size_callback_default: Window not found on size callback!\n");
        return;
    };

    window->cache.width = window->width;
    window->cache.height = window->height;
    window->width = width;
    window->height = height;
    printf("Window size callback: %d %d\n", width, height);
};

void am_platform_window_motion_callback_default(am_id id, am_uint32 x, am_uint32 y, am_platform_events event) {
    am_platform *platform = am_engine_get_subsystem(platform);
    am_window *window = am_packed_array_get_ptr(platform->windows, id);
    if (!window) {
        printf("[FAIL] am_platform_window_motion_callback_default: Window not found on motion callback!\n");
        return;
    };
    window->cache.x = window->x;
    window->cache.y = window->y;
    window->x = x;
    window->y = y;
    printf("Window pos callback: %d %d\n", x, y);
};

am_id am_platform_window_create(am_window_info window_info) {
    am_platform *platform = am_engine_get_subsystem(platform);

    am_window *new_window = (am_window*)am_malloc(sizeof(am_window));
    if (new_window == NULL) {
        printf("[FAIL] am_platform_window_create: Could not allocate memory for window!\n");
        return AM_PA_INVALID_ID;
    };

    am_id ret_id = am_packed_array_add(platform->windows, *new_window);
    am_free(new_window);
    new_window = am_packed_array_get_ptr(platform->windows, ret_id);
    new_window->id = ret_id;

    //Defaults
    if (!window_info.x) {
        window_info.x = AM_WINDOW_DEFAULT_X;
        printf("[WARN] am_platform_window_create: info.x is 0, setting to default (%d)! (id: %u)\n", AM_WINDOW_DEFAULT_X, ret_id);
    };
    if (!window_info.y) {
        window_info.y = AM_WINDOW_DEFAULT_Y;
        printf("[WARN] am_platform_window_create: info.y is 0, setting to default (%d)! (id: %u)\n", AM_WINDOW_DEFAULT_Y, ret_id);
    }
    if (!window_info.width) {
        window_info.width = AM_WINDOW_DEFAULT_WIDTH;
        printf("[WARN] am_platform_window_create: info.width is 0, setting to default (%d)! (id: %u)\n", AM_WINDOW_DEFAULT_WIDTH, ret_id);
    }
    if (!window_info.height) {
        window_info.height = AM_WINDOW_DEFAULT_HEIGHT;
        printf("[WARN] am_platform_window_create: info.height is 0, setting to default (%d)! (id: %u)\n", AM_WINDOW_DEFAULT_HEIGHT, ret_id);
    }
    if (!strlen(window_info.name)) {
        snprintf(window_info.name, AM_MAX_NAME_LENGTH, "%s%d", AM_WINDOW_DEFAULT_NAME, ret_id);
        printf("[WARN] am_platform_window_create: info.name is empty, setting default name (%s)! (id: %u)\n", window_info.name, ret_id);
    };
    if (!window_info.parent) {
        window_info.parent = AM_WINDOW_DEFAULT_PARENT;
        printf("[WARN] am_platform_window_create: info.parent is 0, setting default parent (%lu)! (id: %u)\n", AM_WINDOW_DEFAULT_PARENT, ret_id);
    };
    if (!window_info.is_fullscreen) {
        window_info.is_fullscreen = false;
    };


#if defined(AM_LINUX)
    XSetWindowAttributes window_attributes;
    am_int32 attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    new_window->visual_info = glXChooseVisual(platform->display, 0, attribs);
    new_window->colormap = XCreateColormap(platform->display, window_info.parent, new_window->visual_info->visual, AllocNone);
    window_attributes.colormap = new_window->colormap;
    window_attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask | EnterWindowMask | LeaveWindowMask;
    am_uint64 window = (am_uint64)XCreateWindow(platform->display, window_info.parent,
                                                (am_int32)window_info.x, (am_int32)window_info.y,
                                                window_info.width, window_info.height, 0,
                                                new_window->visual_info->depth, InputOutput,
                                                new_window->visual_info->visual, CWColormap | CWEventMask,
                                                &window_attributes
                                                );
    if (window == BadAlloc || window == BadColor || window == BadCursor || window == BadMatch || window == BadPixmap || window == BadValue || window == BadWindow) {
        printf("[FAIL] am_platform_window_create: Could not create window! (id: %u)\n", ret_id);
        am_packed_array_erase(platform->windows, ret_id);
        return AM_PA_INVALID_ID;
    };
    new_window->handle = window;

    Atom wm_delete = XInternAtom(platform->display, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(platform->display, (Window)new_window->handle, &wm_delete, 1);
    XStoreName(platform->display, (Window)new_window->handle, window_info.name);
    XMapWindow(platform->display, (Window)new_window->handle);

    #else
	DWORD dwExStyle = WS_EX_LEFT; // 0
    DWORD dwStyle = WS_OVERLAPPED; // 0
    if (window_info.parent == AM_WINDOW_DEFAULT_PARENT) {
	    dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    };

    RECT window_rect = {
        .left = 0,
        .right = window_info.win_width,
        .top = 0,
        .bottom = window_info.win_height
    };
    AdjustWindowRectEx(&window_rect, dwStyle, false, dwExStyle);
    am_int32 rect_width = window_rect.right - window_rect.left;
    am_int32 rect_height = window_rect.bottom - window_rect.top;

    new_window->handle = (am_uint64)CreateWindowEx(dwExStyle, window_info.parent == AM_WINDOW_DEFAULT_PARENT ? AM_ROOT_WIN_CLASS:AM_CHILD_WIN_CLASS, window_info.win_name, dwStyle, window_info.x, window_info.y, rect_width, rect_height, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (new_window->handle == 0) {
        printf("[FAIL] Could not create window!\n");
        return -1;
    };
    if ((window_info.parent != AM_WINDOW_DEFAULT_PARENT)) {
        SetParent((HWND)new_window->handle, (HWND)window_info.parent);
        SetWindowLong((HWND)new_window->handle, GWL_STYLE, 0);
    };
    ShowWindow((HWND)new_window->handle, 1);
    #endif

    new_window->x = window_info.x;
    new_window->y = window_info.y;
    //This is set to false due to how the is_fullscreen toggle works, it will be correctly assigned after am_platform_window_fullscreen
    new_window->is_fullscreen = false;
    new_window->parent = window_info.parent;
    new_window->height = window_info.height;
    new_window->width = window_info.width;
    snprintf(new_window->name, AM_MAX_NAME_LENGTH, "%s", window_info.name);
    am_platform_window_fullscreen(new_window->id, window_info.is_fullscreen);


    //REVIEW: Once main window is deleted, cannot create new ones?
    //REVIEW: Should all windows share contexts?
    am_window *main_window = am_packed_array_get_ptr(platform->windows, 1);
    #if defined(AM_LINUX)
    new_window->context = NULL;
    new_window->context = glXCreateContext(platform->display, new_window->visual_info, main_window->context, GL_TRUE);
    glXMakeCurrent(platform->display, new_window->handle, new_window->context);
    #else
    new_window->hdc = GetDC((HWND)new_window->handle);
    PIXELFORMATDESCRIPTOR pixel_format_desc = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,        
        32,                   
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        24,                   
        8,                    
        0,                    
        PFD_MAIN_PLANE,
        0, 0, 0, 0
    };

    am_int32 suggested_pf_index = ChoosePixelFormat(new_window->hdc, &pixel_format_desc);
    PIXELFORMATDESCRIPTOR suggested_pf;
    DescribePixelFormat(new_window->hdc, suggested_pf_index, sizeof(suggested_pf), &suggested_pf);
    SetPixelFormat(new_window->hdc, suggested_pf_index, &suggested_pf);

    new_window->context = wglCreateContext(new_window->hdc);
    wglMakeCurrent(new_window->hdc, new_window->context);
    wglShareLists(new_window->context, main_window->context);
    #endif

    amgl_vsync(new_window->id, am_engine_get_instance()->vsync_enabled);

    #if defined(AM_LINUX)
        glXMakeCurrent(platform->display, 0, 0);
    #else
        wglMakeCurrent(0,0);
    #endif
    return ret_id;
};

void am_platform_window_resize(am_id id, am_uint32 width, am_uint32 height) {
    am_platform *platform  = am_engine_get_subsystem(platform);
    am_window *window = am_packed_array_get_ptr(platform->windows, id);
    if (!window) {
        printf("[FAIL] am_platform_window_resize: Window id is invalid! (id: %u)\n", id);
        return;
    };
    window->cache.width = window->width;
    window->cache.height = window->height;
    #if defined(AM_LINUX)
    XResizeWindow(platform->display, window->handle, width, height);
    #else
    RECT rect = {
        .left = 0,
        .top = 0,
        .bottom = win_height,
        .right = win_width
    };
    if (window->info.parent == AM_WINDOW_DEFAULT_PARENT)
        AdjustWindowRectEx(&rect, WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME, false, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    else
        AdjustWindowRectEx(&rect, 0, false, 0);

    SetWindowPos((HWND)window->handle, 0, 0, 0 , rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);
    #endif
};

void am_platform_window_move(am_id id, am_uint32 x, am_uint32 y) {
    am_platform *platform = am_engine_get_subsystem(platform);
    am_window *window = am_packed_array_get_ptr(platform->windows, id);
    if (!window) {
        printf("[FAIL] am_platform_window_move: Window id is invalid (id: %u)\n", id);
        return;
    };
    window->cache.x = window->x;
    window->cache.y = window->y;

    #if defined(AM_LINUX)
    XMoveWindow(platform->display, window->handle, (am_int32)x, (am_int32)y);
    #else
    RECT rect = {
        .left = x,
        .top = y,
        .bottom = window->info.win_height,
        .right = window->info.win_width
    };
    if (window->info.parent == AM_WINDOW_DEFAULT_PARENT)
        AdjustWindowRectEx(&rect, WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME, false, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    else
        AdjustWindowRectEx(&rect, 0, false, 0);
    SetWindowPos((HWND)window->handle, 0, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_FRAMECHANGED);
    #endif  
};

//REVIEW: Child windows could go "is_fullscreen" by taking the parent's client dimensions
void am_platform_window_fullscreen(am_id id, am_bool state) {
    am_platform *platform = am_engine_get_subsystem(platform);
    am_window *window = am_packed_array_get_ptr(platform->windows, id);
    printf("at fs %d\n", window->width);
    if (!window) {
        printf("[FAIL] am_platform_window_fullscreen: Window id is invalid! (id: %u)\n", id);
        return;
    };
    if (window->is_fullscreen == state || window->parent != AM_WINDOW_DEFAULT_PARENT) return;

    //From here the state has changed, and it is a main window
    window->cache.is_fullscreen = window->is_fullscreen;
    window->is_fullscreen = state;
    am_window_info temp_info = {
            .width = window->width,
            .height = window->height,
            .x = window->x,
            .y = window->y,
            .is_fullscreen = window->is_fullscreen,
            .parent = window->parent,
    };
    snprintf(temp_info.name, AM_MAX_NAME_LENGTH, "%s", window->name);
    //REVIEW: Currently not needed for linux
    #if defined(AM_WINDOWS)
    am_window_cache temp_cache = window->cache;
    #endif

    #if defined(AM_LINUX)
    Atom wm_state = XInternAtom(platform->display, "_NET_WM_STATE", false);
    Atom wm_fs = XInternAtom(platform->display, "_NET_WM_STATE_FULLSCREEN", false);
    XEvent xevent = {0};
    xevent.type = ClientMessage;
    xevent.xclient.window = window->handle;
    xevent.xclient.message_type = wm_state;
    xevent.xclient.format = 32;
    xevent.xclient.data.l[0] = state ? 1:0;
    xevent.xclient.data.l[1] = (long)wm_fs;
    xevent.xclient.data.l[3] = 0l;
    XSendEvent(platform->display, AM_WINDOW_DEFAULT_PARENT, false, SubstructureRedirectMask | SubstructureNotifyMask, &xevent);
    XFlush(platform->display);
    XWindowAttributes window_attribs = {0};
    XGetWindowAttributes(platform->display, window->handle, &window_attribs);
    printf("Fullscreen toggle\n Pos: %d %d\n Size: %d %d\nFullscreen toggle end\n\n", window_attribs.x, window_attribs.y, window_attribs.width, window_attribs.height);
    memcpy(&window->cache, &temp_info, sizeof(am_window_info));


    #else
    DWORD dw_style = GetWindowLong((HWND)window->handle, GWL_STYLE);
    if (window->info.is_fullscreen) {
        printf("Going is_fullscreen\n");
        MONITORINFO monitor_info = {sizeof(monitor_info)};
        GetMonitorInfo(MonitorFromWindow((HWND)window->handle, MONITOR_DEFAULTTONEAREST), &monitor_info);
        SetWindowLong((HWND)window->handle, GWL_STYLE, dw_style & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos((HWND)window->handle, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top, monitor_info.rcMonitor.right - monitor_info.rcMonitor.left, monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        memcpy(&window->cache, &temp_info, sizeof(am_window_info));
    } else {
        printf("Going not is_fullscreen\n");
        SetWindowLong((HWND)window->handle, GWL_STYLE, dw_style | WS_OVERLAPPEDWINDOW);
        am_platform_window_resize(id, temp_cache.win_width, temp_cache.win_height);
        am_platform_window_move(id, temp_cache.x, temp_cache.y);
        
    };
    #endif

};

void am_platform_window_destroy(am_id id) {
    #if defined(AM_LINUX)
    am_platform *platform = am_engine_get_subsystem(platform);
    am_window *window = am_packed_array_get_ptr(platform->windows, id);
    if (!window) {
        printf("[FAIL] am_platform_window_destroy: Window id is invalid! (id: %u)\n", id);
        return;
    };
    glXDestroyContext(platform->display, window->context);
    XUnmapWindow(platform->display, window->handle);
    XDestroyWindow(platform->display, window->handle);
    XFreeColormap(platform->display, window->colormap);
    XFree(window->visual_info);
    XFlush(platform->display);
    #else
    DestroyWindow((HWND)(window->handle));
    #endif
};

void am_platform_timer_create() {
    am_platform *platform = am_engine_get_subsystem(platform);
    #if defined(AM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    platform->time.offset = (am_uint64)ts.tv_sec * (am_uint64)1000000000 + (am_uint64)ts.tv_nsec;

    platform->time.frequency = 1000000000;
    #else
    QueryPerformanceFrequency((LARGE_INTEGER*)&platform->time.frequency);
    QueryPerformanceCounter((LARGE_INTEGER*)&platform->time.offset);
    #endif
};

void am_platform_timer_sleep(am_float32 ms) {
    #if defined(AM_LINUX)
    usleep((__useconds_t)(ms*1000.0f));
    #else
    timeBeginPeriod(1);
    Sleep((uint64_t)ms);
    timeEndPeriod(1);
    #endif
};

am_uint64 am_platform_timer_value() {
    #if defined(AM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (am_uint64)ts.tv_sec * (am_uint64)1000000000 + (am_uint64)ts.tv_nsec;
    #else
    am_uint64 value;
    QueryPerformanceCounter((LARGE_INTEGER*)&value);
    return value;
    #endif
};

am_uint64 am_platform_elapsed_time() {
    am_platform *platform = am_engine_get_subsystem(platform);
    return (am_platform_timer_value() - platform->time.offset);
};

//----------------------------------------------------------------------------//
//                              END PLATFORM IMPL                             //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                START GL IMPL                               //
//----------------------------------------------------------------------------//


am_id amgl_shader_create(amgl_shader_info info) {
    if (!info.num_sources || info.sources == NULL) {
        printf("[FAIL] amgl_shader_create: No shader sources provided!\n");
        return AM_PA_INVALID_ID;
    };

    am_engine *engine = am_engine_get_instance();
    amgl_shader *new_shader = (amgl_shader*)am_malloc(sizeof(amgl_shader));
    if (new_shader == NULL) {
        printf("[FAIL] amgl_shader_create: Could not allocate memory for shader!\n");
        return AM_PA_INVALID_ID;
    };

    am_int32 ret_id = am_packed_array_add(engine->ctx_data.shaders, *new_shader);
    am_free(new_shader);
    new_shader = am_packed_array_get_ptr(engine->ctx_data.shaders, ret_id);
    new_shader->id = ret_id;

    am_uint32 main_shader = glCreateProgram();
    am_uint32 shader_list[info.num_sources]; 
    for (am_int32 i = 0; i < info.num_sources; i++) {
        am_uint32 shader = 0;
        switch (info.sources[i].type) {
            case AMGL_SHADER_FRAGMENT: {
                shader = glCreateShader(GL_FRAGMENT_SHADER);
                break;
            };
            case AMGL_SHADER_VERTEX: {
                shader = glCreateShader(GL_VERTEX_SHADER);
                break;
            };
            default: break;
        };
        glShaderSource(shader, 1, (const GLchar**)&info.sources[i].source, NULL);
        glCompileShader(shader);
        shader_list[i] = shader;// For detaching after linking

        am_int32 compile_result = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);
        if (!compile_result) {
            am_int32 length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            char err_log[length];
            glGetShaderInfoLog(shader, length, &length, err_log);
            printf("[FAIL] amgl_shader_create: Shader compilation failed for index %u:\n%s\n", i, err_log);
            glDeleteShader(shader);
        };
        glAttachShader(main_shader, shader);
    };
    glLinkProgram(main_shader);

    am_int32 is_linked = 0;
    glGetProgramiv(main_shader, GL_LINK_STATUS, &is_linked);
    if (!is_linked) {
        am_int32 length = 0;
        glGetProgramiv(main_shader, GL_INFO_LOG_LENGTH, &length);
        char err_log[length];
        glGetProgramInfoLog(main_shader, length, &length, err_log);
        printf("[FAIL] amgl_shader_create: Failed to link shader (id: %u): \n%s\n", ret_id, err_log);
        am_packed_array_erase(engine->ctx_data.shaders, ret_id);
        glDeleteProgram(main_shader);
        return AM_PA_INVALID_ID;
    };

    for (am_int32 i = 0; i < info.num_sources; i++) { 
        glDetachShader(main_shader, shader_list[i]);
        glDeleteShader(shader_list[i]);
        am_free(info.sources[i].source);
    };

    new_shader->handle = main_shader;
    //REVIEW: Could simplify
    if (!strlen(info.name)) {
        snprintf(info.name, AM_MAX_NAME_LENGTH, "%s%d", AMGL_SHADER_DEFAULT_NAME, ret_id);
        printf("[WARN] amgl_shader_create: info.name is empty, choosing default name (%s)! (id: %u)\n", info.name, ret_id);
    };
    snprintf(new_shader->name, AM_MAX_NAME_LENGTH, "%s", info.name);
    return ret_id;
};

void amgl_shader_destroy(am_id id) {
    am_engine *engine = am_engine_get_instance();
    amgl_shader *shader = am_packed_array_get_ptr(engine->ctx_data.shaders, id);
    if (!shader) {
        printf("[FAIL] amgl_shader_destroy: Invalid shader id! (id: %u)\n", id);
        return;
    };
    glDeleteProgram(shader->handle);
    am_packed_array_erase(engine->ctx_data.shaders, id);
};

am_id amgl_vertex_buffer_create(amgl_vertex_buffer_info info) {
    am_engine *engine = am_engine_get_instance();

    if (info.data == NULL) printf("[WARN] amgl_vertex_buffer_create: Data pointer is NULL!\n");
    if (info.size == 0) printf("[WARN] amgl_vertex_buffer_create: Size is not specified!\n");

    amgl_vertex_buffer *v_buffer = (amgl_vertex_buffer*)am_malloc(sizeof(amgl_vertex_buffer));
    if (v_buffer == NULL) {
        printf("[FAIL] amgl_vertex_buffer_create: Could not allocate memory for vertex buffer!\n");
        return AM_PA_INVALID_ID;
    };

    am_id ret_id = am_packed_array_add(engine->ctx_data.vertex_buffers, *v_buffer);
    am_free(v_buffer);
    v_buffer = am_packed_array_get_ptr(engine->ctx_data.vertex_buffers, ret_id);
    v_buffer->id = ret_id;

    am_int32 usage = 0;
    switch (info.usage) {
        case AMGL_BUFFER_USAGE_STATIC: usage = GL_STATIC_DRAW; break;
        case AMGL_BUFFER_USAGE_STREAM: usage = GL_STREAM_DRAW; break;
        case AMGL_BUFFER_USAGE_DYNAMIC: usage = GL_DYNAMIC_DRAW; break;
        default: {
            printf("[FAIL] amgl_vertex_buffer_create: Invalid usage value!\n");
            am_packed_array_erase(engine->ctx_data.vertex_buffers, ret_id);
            return AM_PA_INVALID_ID;
        };
    };

    glGenBuffers(1, &v_buffer->handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer->handle);
    glBufferData(GL_ARRAY_BUFFER, (long int)info.size, info.data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //REVIEW: Could simplify
    if (!strlen(info.name)) {
        snprintf(info.name, AM_MAX_NAME_LENGTH, "%s%d", AMGL_VERTEX_BUFFER_DEFAULT_NAME, v_buffer->id);
        printf("[WARN] amgl_vertex_buffer_create: Choosing default name! (id: %u)\n", v_buffer->id);
    };
    snprintf(v_buffer->name, AM_MAX_NAME_LENGTH, "%s", info.name);

    return ret_id;
};

void amgl_vertex_buffer_destroy(am_id id) {
    am_engine *engine = am_engine_get_instance();
    amgl_vertex_buffer *vbuffer = am_packed_array_get_ptr(engine->ctx_data.vertex_buffers, id);
    if (!vbuffer) {
        printf("[FAIL] amgl_vertex_buffer_destroy: Invalid vertex buffer id! (id: %u)\n", id);
        return;
    }
    glDeleteBuffers(1, &vbuffer->handle);
    am_packed_array_erase(engine->ctx_data.vertex_buffers, id);
};

am_id amgl_index_buffer_create(amgl_index_buffer_info info) {
    am_engine *engine = am_engine_get_instance();

    if (info.data == NULL) printf("[WARN] amgl_vertex_index_create: Data pointer is NULL!\n");
    if (info.size == 0) printf("[WARN] amgl_vertex_index_create: Size is not specified!\n");
    
    amgl_index_buffer *index_bfr = (amgl_index_buffer*)malloc(sizeof(amgl_index_buffer));
    if (index_bfr == NULL) {
        printf("[FAIL] amgl_vertex_index_create: Could not allocate memory for index buffer!\n");
        return -1;
    };

    am_id ret_id = am_packed_array_add(engine->ctx_data.index_buffers, *index_bfr);
    am_free(index_bfr);
    index_bfr = am_packed_array_get_ptr(engine->ctx_data.index_buffers, ret_id);
    index_bfr->id = ret_id;

    glGenBuffers(1, &index_bfr->handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_bfr->handle);
    switch (info.usage) {
        case AMGL_BUFFER_USAGE_STATIC: {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)info.size, info.data, GL_STATIC_DRAW);
            break;
        };
        case AMGL_BUFFER_USAGE_STREAM: {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)info.size, info.data, GL_STREAM_DRAW);
            break;
        };
        case AMGL_BUFFER_USAGE_DYNAMIC: {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)info.size, info.data, GL_DYNAMIC_DRAW);
            break;
        };
        default: {
            printf("[FAIL] amgl_vertex_index_create: Invalid usage!\n");
            am_packed_array_erase(engine->ctx_data.index_buffers, ret_id);
            return AM_PA_INVALID_ID;
        };
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //REVIEW: Could simplify
    if (!strlen(info.name)) {
        snprintf(info.name, AM_MAX_NAME_LENGTH, "%s%d", AMGL_INDEX_BUFFER_DEFAULT_NAME, index_bfr->id);
        printf("[WARN] amgl_index_buffer_create: Choosing default name! (id: %u)\n", index_bfr->id);
    };
    snprintf(index_bfr->name, AM_MAX_NAME_LENGTH, "%s", info.name);

    return ret_id;
};

void amgl_index_buffer_destroy(am_id id) {
    am_engine *engine = am_engine_get_instance();
    amgl_index_buffer *index_buffer = am_packed_array_get_ptr(engine->ctx_data.index_buffers, id);
    if (!index_buffer) {
        printf("[FAIL] amgl_index_buffer_destroy: Invalid index buffer id! (id: %u)\n", id);
        return;
    };
    glDeleteBuffers(1, &index_buffer->handle);
    am_packed_array_erase(engine->ctx_data.index_buffers, id);
};


am_id amgl_uniform_create(amgl_uniform_info info) {
    am_engine *engine = am_engine_get_instance();

    amgl_uniform *uniform = (amgl_uniform*)malloc(sizeof(amgl_uniform));
    if (uniform == NULL) {
        printf("[FAIL] amgl_uniform_create: Could not allocate memory for uniform!\n");
        return -1;
    };

    uniform->location = 0xFFFFFFFF;
    uniform->id = engine->ctx_data.uniforms->next_id;
    uniform->type = info.type;
    //REVIEW: Could simplify
    if (!strlen(info.name)) {
        snprintf(info.name, AM_MAX_NAME_LENGTH, "%s%d", AMGL_UNIFORM_DEFAULT_NAME, uniform->id);
        printf("[WARN] amgl_uniform_create: Choosing default name! (id: %u)\n", uniform->id);
    };
    snprintf(uniform->name, AM_MAX_NAME_LENGTH, "%s", info.name);
    am_int32 ret_id = am_packed_array_add(engine->ctx_data.uniforms, *uniform);
    am_free(uniform);
    return ret_id;
};

/*
void amgl_uniform_update(am_int32 id, amgl_uniform_info info) {
    amgl_uniform *uniform = amgl_uniform_lookup(id);
    uniform->info = info;
    uniform->location = glGetUniformLocation(uniform->info.shader_id, uniform->info.win_name);
    glUniform1fv(uniform->location, 1, (float*)uniform->info.data);
};
*/

void amgl_uniform_destroy(am_id id) {
    am_engine *engine = am_engine_get_instance();
    am_packed_array_erase(engine->ctx_data.index_buffers, id);
};

am_id amgl_texture_create(amgl_texture_info info) {
    am_engine *engine = am_engine_get_instance();

    amgl_texture *texture = (amgl_texture*)am_malloc(sizeof(amgl_texture));
    if (texture == NULL) {
        printf("[FAIL] amgl_texture_create: Could not allocate memory for texture!\n");
        return -1;
    };

    am_id ret_id = am_packed_array_add(engine->ctx_data.textures, *texture);
    am_free(texture);
    texture = am_packed_array_get_ptr(engine->ctx_data.textures, ret_id);
    texture->id = ret_id;

    if (!info.wrap_s) {
        info.wrap_s = AMGL_TEXTURE_DEFAULT_WRAP;
        printf("[WARN] amgl_texture_create: Choosing default wrap_s value!\n");
    };
    if (!info.wrap_t) {
        info.wrap_t = AMGL_TEXTURE_DEFAULT_WRAP;
        printf("[WARN] amgl_texture_create: Choosing default wrap_t value!\n");
    };
    if (!info.height) {
        info.height = AMGL_TEXTURE_DEFAULT_HEIGHT;
        printf("[WARN] amgl_texture_create: Choosing default height value!\n");
    };
    if (!info.width) {
        info.width = AMGL_TEXTURE_DEFAULT_WIDTH;
        printf("[WARN] amgl_texture_create: Choosing default width value!\n");
    };
    if (!info.format) {
        info.format = AMGL_TEXTURE_DEFAULT_FORMAT;
        printf("[WARN] amgl_texture_create: Choosing default format value!\n");
    };

    glGenTextures(1, &texture->handle);
    glBindTexture(GL_TEXTURE_2D, texture->handle);

    switch (info.format) {
        case AMGL_TEXTURE_FORMAT_A8: glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (am_int32)info.width, (am_int32)info.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, info.data); break;
        case AMGL_TEXTURE_FORMAT_R8: glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (am_int32)info.width, (am_int32)info.height, 0, GL_RED, GL_UNSIGNED_BYTE, info.data); break;
        case AMGL_TEXTURE_FORMAT_RGB8: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (am_int32)info.width, (am_int32)info.height, 0, GL_RGB8, GL_UNSIGNED_BYTE, info.data); break;
        case AMGL_TEXTURE_FORMAT_RGBA8: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (am_int32)info.width, (am_int32)info.height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, info.data); break;
        case AMGL_TEXTURE_FORMAT_RGBA16F: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, (am_int32)info.width, (am_int32)info.height, 0, GL_RGBA16, GL_FLOAT, info.data); break;
        case AMGL_TEXTURE_FORMAT_RGBA32F: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (am_int32)info.width, (am_int32)info.height, 0, GL_RGBA32F, GL_FLOAT, info.data); break;
        case AMGL_TEXTURE_FORMAT_RGBA: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (am_int32)info.width, (am_int32)info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, info.data); break;
        case AMGL_TEXTURE_FORMAT_DEPTH8: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (am_int32)info.width, (am_int32)info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, info.data); break;
        case AMGL_TEXTURE_FORMAT_DEPTH16: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, (am_int32)info.width, (am_int32)info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, info.data); break;
        case AMGL_TEXTURE_FORMAT_DEPTH24: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, (am_int32)info.width, (am_int32)info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, info.data); break;
        case AMGL_TEXTURE_FORMAT_DEPTH32F: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, (am_int32)info.width, (am_int32)info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, info.data); break;
        case AMGL_TEXTURE_FORMAT_DEPTH24_STENCIL8: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, (am_int32)info.width, (am_int32)info.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, info.data); break;
        case AMGL_TEXTURE_FORMAT_DEPTH32F_STENCIL8: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, (am_int32)info.width, (am_int32)info.height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, info.data); break;
        default: {
            printf("[FAIL] amgl_texture_create: Invalid texture format!\n");
            am_packed_array_erase(engine->ctx_data.textures, ret_id);
            return AM_PA_INVALID_ID;
        };
    };

    am_int32 min_filter = info.min_filter == AMGL_TEXTURE_FILTER_NEAREST ? GL_NEAREST:GL_LINEAR;
    am_int32 mag_filter = info.mag_filter == AMGL_TEXTURE_FILTER_NEAREST ? GL_NEAREST:GL_LINEAR;

    if (info.mip_num) {
        if (info.min_filter == AMGL_TEXTURE_FILTER_NEAREST)
            min_filter = info.mip_filter == AMGL_TEXTURE_FILTER_NEAREST ? GL_NEAREST_MIPMAP_NEAREST:GL_NEAREST_MIPMAP_LINEAR;
        else
            min_filter = info.mip_filter == AMGL_TEXTURE_FILTER_NEAREST ? GL_LINEAR_MIPMAP_NEAREST:GL_LINEAR_MIPMAP_LINEAR;

        glGenerateMipmap(GL_TEXTURE_2D);
    };

    am_int32 wrap_s = amgl_texture_translate_wrap(info.wrap_s);
    am_int32 wrap_t = amgl_texture_translate_wrap(info.wrap_t);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

    //TODO: Research if this is needed for every texture, might not be needed if using shaders as far as I understand
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    //REVIEW: Could simplify
    if (!strlen(info.name)) {
        snprintf(info.name, AM_MAX_NAME_LENGTH, "%s%d", AMGL_TEXTURE_DEFAULT_NAME, texture->id);
        printf("[WARN] amgl_texture_create: Choosing default name! (id: %u)\n", texture->id);
    };
    snprintf(texture->name, AM_MAX_NAME_LENGTH, "%s", info.name);

    return ret_id;
};

am_int32 amgl_texture_translate_format(amgl_texture_format format) {
    switch (format) {
        case AMGL_TEXTURE_FORMAT_RGBA8: return GL_RGBA8;
        case AMGL_TEXTURE_FORMAT_RGB8: return  GL_RGB8;
        case AMGL_TEXTURE_FORMAT_RGBA16F: return GL_RGBA16F;
        case AMGL_TEXTURE_FORMAT_RGBA32F: return  GL_RGBA32F;
        case AMGL_TEXTURE_FORMAT_RGBA: return GL_RGBA;
        case AMGL_TEXTURE_FORMAT_A8: return GL_ALPHA;
        case AMGL_TEXTURE_FORMAT_R8: return GL_RED;
        case AMGL_TEXTURE_FORMAT_DEPTH8: return GL_DEPTH_COMPONENT;
        case AMGL_TEXTURE_FORMAT_DEPTH16: return GL_DEPTH_COMPONENT16;
        case AMGL_TEXTURE_FORMAT_DEPTH24: return  GL_DEPTH_COMPONENT24;
        case AMGL_TEXTURE_FORMAT_DEPTH32F: return  GL_DEPTH_COMPONENT32F;
        case AMGL_TEXTURE_FORMAT_DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
        case AMGL_TEXTURE_FORMAT_DEPTH32F_STENCIL8: return GL_DEPTH32F_STENCIL8;
        default: {
            printf("[WARN] amgl_texture_translate_format: Unknown texture format!\n");
            return GL_UNSIGNED_BYTE;
        };
    };
};

am_int32 amgl_texture_translate_wrap(amgl_texture_wrap wrap) {
    switch (wrap) {
        case AMGL_TEXTURE_WRAP_REPEAT: return GL_REPEAT;
        case AMGL_TEXTURE_WRAP_MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
        case AMGL_TEXTURE_WRAP_CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
        case AMGL_TEXTURE_WRAP_CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
        default: {
            printf("[WARN] amgl_texture_translate_wrap: Unknown texture wrap!\n");
            return GL_REPEAT;
        };
    };
};

void amgl_texture_load_from_file(const char *path, amgl_texture_info *info, am_bool flip) {
    //Not using am_util_read_file because size is needed
    FILE *file = fopen(path, "rb");
    char* buffer = NULL;
    size_t rd_size = 0;

    if (file) {
        #if defined(AM_LINUX)
        struct stat st;
        stat(path, &st);
        rd_size = st.st_size;
        #else
        HANDLE file_hwnd = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        LARGE_INTEGER size;
        GetFileSizeEx(file_hwnd, &size);
        rd_size = (am_int32) size.QuadPart;
        CloseHandle(file_hwnd);
        #endif
        buffer = (char*)am_malloc(rd_size+1);
        if (buffer) fread(buffer, 1, rd_size, file);
        buffer[rd_size] = '\0'; 

    };
    fclose(file);
    amgl_texture_load_from_memory(buffer, info, rd_size, flip);
};

//REVIEW: Needed? Could merge with above
void amgl_texture_load_from_memory(const void *memory, amgl_texture_info *info, size_t size, am_bool flip) {
    am_int32 num_comps = 0;
    stbi_set_flip_vertically_on_load(flip);
    info->data = NULL;
    info->data = (void*)stbi_load_from_memory((const stbi_uc*)memory, (am_int32)size, (am_int32*)&info->width, (am_int32*)&info->height, &num_comps, 4);
    if (info->data == NULL) {
        printf("[FAIL] amgl_texture_load_from_memory: Unable to load texture!\n");
        return;
    };
};

void amgl_texture_destroy(am_id id) {
    am_engine *engine = am_engine_get_instance();
    amgl_texture *texture = am_packed_array_get_ptr(engine->ctx_data.textures, id);
    if (!texture) {
        printf("[FAIL] amgl_texture_destroy: Invalid texture id! (id: %u)\n", id);
        return;
    }
    glDeleteTextures(1, &texture->handle);
    am_packed_array_erase(engine->ctx_data.textures, id);
};


am_id amgl_frame_buffer_create(amgl_frame_buffer_info info) {
    am_engine *engine = am_engine_get_instance();

    amgl_frame_buffer *framebuffer = (amgl_frame_buffer*)am_malloc(sizeof(amgl_frame_buffer));
    if (framebuffer == NULL) {
        printf("[FAIL] amgl_frame_buffer_create: Could not allocate memory for framebuffer!\n");
        return AM_PA_INVALID_ID;
    };

    glGenFramebuffers(1, &framebuffer->handle);
    framebuffer->id = engine->ctx_data.frame_buffers->next_id;
    //REVIEW: Could simplify
    if (!strlen(info.name)) {
        snprintf(info.name, AM_MAX_NAME_LENGTH, "%s%d", AMGL_FRAME_BUFFER_DEFAULT_NAME, framebuffer->id);
        printf("[WARN] amgl_frame_buffer_create: Choosing default name! (id: %u)\n", framebuffer->id);
    };
    snprintf(framebuffer->name, AM_MAX_NAME_LENGTH, "%s", info.name);
    am_id ret_id = am_packed_array_add(engine->ctx_data.frame_buffers, *framebuffer);
    am_free(framebuffer);
    return ret_id;
};

void amgl_frame_buffer_destroy(am_id id) {
    am_engine *engine = am_engine_get_instance();
    amgl_frame_buffer *framebuffer = am_packed_array_get_ptr(engine->ctx_data.frame_buffers, id);
    if (!framebuffer) {
        printf("[FAIL] amgl_frame_buffer_destroy: Invalid frame buffer id! (id: %u)\n", framebuffer->id);
        return;
    };
    glDeleteFramebuffers(1, &framebuffer->handle);
    am_packed_array_erase(engine->ctx_data.frame_buffers, id);
};

am_id amgl_render_pass_create(amgl_render_pass_info info) {
    am_engine *engine = am_engine_get_instance();

    amgl_render_pass *render_pass = (amgl_render_pass*)am_malloc(sizeof(amgl_render_pass));
    if (render_pass == NULL) {
        printf("[FAIL] amgl_render_pass_create: Could not allocate memory for render pass!\n");
        return -1;
    };

    am_id ret_id = am_packed_array_add(engine->ctx_data.render_passes, *render_pass);
    am_free(render_pass);
    render_pass = am_packed_array_get_ptr(engine->ctx_data.render_passes, ret_id);
    render_pass->id = ret_id;

    //render_pass->info = info;
    if (!info.depth_texture_id) printf("[WARN] amgl_render_pass_create: No depth texture specified!\n");
    render_pass->depth_texture_id = info.depth_texture_id;
    if (!info.framebuffer_id) {
        printf("[WARN] amgl_render_pass_create: No framebuffer id specified, choosing id 1!\n");
        render_pass->framebuffer_id = 1;
    } else {
        render_pass->framebuffer_id = info.framebuffer_id;
    };

    if (!info.stencil_texture_id) printf("[WARN] amgl_render_pass_create: No stencil texture id specified!\n");
    render_pass->stencil_texture_id = info.stencil_texture_id;
    if ((!info.num_colors) || info.color_texture_ids == NULL) printf("[WARN] amgl_render_pass_create: No color attachments passed!\n");
    render_pass->num_colors = info.num_colors;
    render_pass->color_texture_ids = info.color_texture_ids;

    //REVIEW: Could simplify
    if (!strlen(info.name)) {
        snprintf(info.name, AM_MAX_NAME_LENGTH, "%s%d", AMGL_RENDER_PASS_DEFAULT_NAME, render_pass->id);
        printf("[WARN] amgl_render_pass_create: Choosing default name! (id: %u)\n", render_pass->id);
    };
    snprintf(render_pass->name, AM_MAX_NAME_LENGTH, "%s", info.name);
    return ret_id;
};

void amgl_render_pass_destroy(am_id id) {
    am_engine *engine = am_engine_get_instance();
    am_packed_array_erase(engine->ctx_data.render_passes, id);
};

am_id amgl_pipeline_create(amgl_pipeline_info info) {
    am_engine *engine = am_engine_get_instance();

    amgl_pipeline *pipeline = (amgl_pipeline*)am_malloc(sizeof(amgl_pipeline));
    if (pipeline == NULL) {
        printf("[FAIL] Could not allocate memory for framebuffer!\n");
        return -1;
    };

    pipeline->blend = info.blend;
    pipeline->depth = info.depth;
    pipeline->stencil = info.stencil;
    pipeline->raster = info.raster;
    pipeline->layout = info.layout;

    pipeline->id = engine->ctx_data.pipelines->next_id;
    //REVIEW: Could simplify
    if (!strlen(info.name)) {
        snprintf(info.name, AM_MAX_NAME_LENGTH, "%s%d", AMGL_PIPELINE_DEFAULT_NAME, pipeline->id);
        printf("[WARN] amgl_pipeline_create: Choosing default name! (id: %u)\n", pipeline->id);
    };
    snprintf(pipeline->name, AM_MAX_NAME_LENGTH, "%s", info.name);
    am_int32 ret_id = am_packed_array_add(engine->ctx_data.pipelines, *pipeline);
    am_free(pipeline);
    return ret_id;  
};

void amgl_pipeline_destroy(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    am_packed_array_erase(engine->ctx_data.pipelines, id);  
};

void amgl_init() {
    //TODO: Init all gl procedures and functions
    glEnable(GL_TEXTURE_2D);
    glCreateShader = (PFNGLCREATESHADERPROC)amgl_get_proc_address("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)amgl_get_proc_address("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)amgl_get_proc_address("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)amgl_get_proc_address("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)amgl_get_proc_address("glGetShaderInfoLog");
    glDetachShader = (PFNGLDETACHSHADERPROC)amgl_get_proc_address("glDetachShader");
    glDeleteShader = (PFNGLDELETESHADERPROC)amgl_get_proc_address("glDeleteShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)amgl_get_proc_address("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)amgl_get_proc_address("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)amgl_get_proc_address("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)amgl_get_proc_address("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)amgl_get_proc_address("glGetProgramInfoLog");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)amgl_get_proc_address("glDeleteProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)amgl_get_proc_address("glUseProgram");
    glGenBuffers = (PFNGLGENBUFFERSPROC)amgl_get_proc_address("glGenBuffers");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)amgl_get_proc_address("glDeleteBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)amgl_get_proc_address("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)amgl_get_proc_address("glBufferData");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)amgl_get_proc_address("glGenVertexArrays");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)amgl_get_proc_address("glDeleteVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)amgl_get_proc_address("glBindVertexArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)amgl_get_proc_address("glVertexAttribPointer");
    glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)amgl_get_proc_address("glVertexAttribIPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)amgl_get_proc_address("glEnableVertexAttribArray");
    glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)amgl_get_proc_address("glGetAttribLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)amgl_get_proc_address("glUniform1i");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)amgl_get_proc_address("glGetUniformLocation");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)amgl_get_proc_address("glGenerateMipmap");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)amgl_get_proc_address("glBufferSubData");
    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)amgl_get_proc_address("glGenFramebuffers");
    glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)amgl_get_proc_address("glDeleteFramebuffers");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)amgl_get_proc_address("glBindFramebuffer");
    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)amgl_get_proc_address("glFramebufferTexture2D");
    glUniform1fv = (PFNGLUNIFORM1FVPROC)amgl_get_proc_address("glUniform1fv");
    glUniform1f = (PFNGLUNIFORM1FPROC)amgl_get_proc_address("glUniform1f");
    glGetUniformfv = (PFNGLGETUNIFORMFVPROC)amgl_get_proc_address("glGetUniformfv");
    glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)amgl_get_proc_address("glBindImageTexture");

    am_engine *engine = am_engine_get_instance();
    amgl_frame_buffer base_fbo = {
        .handle = 0, //OpenGL default
        .id = 1
    };
    am_packed_array_add(engine->ctx_data.frame_buffers, base_fbo);
};

//TODO: Windows impl if necessary
void amgl_terminate() {

};

void amgl_set_viewport(am_int32 x, am_int32 y, am_int32 width, am_int32 height) {
    glViewport(x, y, width, height);
};

void amgl_vsync(am_id window_id, am_bool state) {
    //REVIEW: Not sure if glFlush() is needed but I read it can help
    glFlush();
    //Have to load SwapInterval here because this is called for each window on creation, so before amgl_init()
    #if defined(AM_LINUX)
    am_platform *platform = am_engine_get_subsystem(platform);
    am_window *window = am_packed_array_get_ptr(platform->windows, window_id);
    if (glSwapInterval == NULL) glSwapInterval = (PFNGLSWAPINTERVALEXTPROC)amgl_get_proc_address("glXSwapIntervalEXT");
        glSwapInterval(platform->display, window->handle, state);
    #else
        if (glSwapInterval == NULL) glSwapInterval = (PFNGLSWAPINTERVALEXTPROC)amgl_get_proc_address("wglSwapIntervalEXT");
        glSwapInterval(state == true ? 1:0);
    #endif
};

void amgl_start_render_pass(am_int32 render_pass_id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(engine->ctx_data.render_passes, render_pass_id)) {
        amgl_render_pass *render_pass = am_packed_array_get_ptr(engine->ctx_data.render_passes, render_pass_id);
        if (am_packed_array_has(engine->ctx_data.frame_buffers, render_pass->framebuffer_id)) {
            glBindFramebuffer(GL_FRAMEBUFFER, am_packed_array_get_val(engine->ctx_data.frame_buffers, render_pass->framebuffer_id).handle);
            for (am_int32 i = 0; i < render_pass->num_colors; i++) {
                if (am_packed_array_has(engine->ctx_data.textures, render_pass->color_texture_ids[i])) {
                    amgl_texture *color = am_packed_array_get_ptr(engine->ctx_data.textures, render_pass->color_texture_ids[i]);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color->handle, 0);
                };
            };
        };
    };
};

void amgl_end_render_pass(am_int32 render_pass_id) {
    am_engine* engine = am_engine_get_instance();

    engine->ctx_data.frame_cache.index_buffer = (amgl_index_buffer){.id = AM_PA_INVALID_ID};
    engine->ctx_data.frame_cache.index_element_size = 0;
    engine->ctx_data.frame_cache.pipeline = (amgl_pipeline){.id = AM_PA_INVALID_ID};
    am_dyn_array_clear(engine->ctx_data.frame_cache.vertex_buffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //glDisable(GL_SCISSOR_TEST); //Read more on this
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
};

void amgl_bind_pipeline(am_int32 pipeline_id) {
    am_engine* engine = am_engine_get_instance();
    if (am_packed_array_has(engine->ctx_data.pipelines, pipeline_id)) {
        
        engine->ctx_data.frame_cache.index_buffer = (amgl_index_buffer){.id = AM_PA_INVALID_ID};
        engine->ctx_data.frame_cache.index_element_size = 0;
        engine->ctx_data.frame_cache.pipeline = (amgl_pipeline){.id = AM_PA_INVALID_ID};
        am_dyn_array_clear(engine->ctx_data.frame_cache.vertex_buffers);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        amgl_pipeline *pipeline = am_packed_array_get_ptr(engine->ctx_data.pipelines, pipeline_id);
        engine->ctx_data.frame_cache.pipeline = *pipeline;

        
        if (!pipeline->depth.func) {
            glDisable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
            //TODO: Will have to change this if I add enums
            glDepthFunc(pipeline->depth.func);
        };

        if (!pipeline->stencil.func) {
            glDisable(GL_STENCIL_TEST);
        } else {
            glEnable(GL_STENCIL_TEST);
            am_uint32 func = pipeline->stencil.func;
            am_uint32 sfail = pipeline->stencil.sfail;
            am_uint32 dpfail = pipeline->stencil.dpfail;
            am_uint32 dppass = pipeline->stencil.dppass;
            glStencilFunc(func, pipeline->stencil.ref, pipeline->stencil.comp_mask);
            glStencilMask(pipeline->stencil.write_mask);
            glStencilOp(sfail, dpfail, dppass);
        };

        if (!pipeline->blend.func) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
            glBlendEquation(pipeline->blend.func);
            glBlendFunc(pipeline->blend.src, pipeline->blend.dst);
        };

        if (!pipeline->raster.face_culling) {
            glDisable(GL_CULL_FACE);
        } else {
            glEnable(GL_CULL_FACE);
            glCullFace(pipeline->raster.face_culling);
        };

        //TODO below fix
        //FIX add enum and converter where default (0) is GL_CW
        glFrontFace(pipeline->raster.winding_order == 0 ? GL_CW : GL_CCW);

        if (am_packed_array_has(engine->ctx_data.shaders, pipeline->raster.shader_id)){
            glUseProgram(am_packed_array_get_ptr(engine->ctx_data.shaders, pipeline->raster.shader_id)->handle);
        };
    };
};

void amgl_set_bindings(amgl_bindings_info *info) {
    am_engine *engine = am_engine_get_instance();
    am_uint32 vertex_count = info->vertex_buffers.info ? info->vertex_buffers.size ? info->vertex_buffers.size / sizeof(amgl_vertex_buffer_bind_info) : 1 : 0;
    am_uint32 index_count = info->index_buffers.info ? info->index_buffers.size ? info->index_buffers.size / sizeof(amgl_index_buffer_bind_info) : 1 : 0;
    am_uint32 uniform_count = info->uniforms.info ? info->uniforms.size ? info->uniforms.size / sizeof(amgl_uniform_bind_info) : 1 : 0;
    am_uint32 texture_count = info->textures.info ? info->textures.size ? info->textures.size / sizeof(amgl_texture_bind_info) : 1 : 0;
    
    
    if (vertex_count) {
        am_dyn_array_clear(engine->ctx_data.frame_cache.vertex_buffers);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    };

    for (am_uint32 i = 0; i < vertex_count; i++) {
        am_int32 id = info->vertex_buffers.info[i].vertex_buffer_id;
        if (!am_packed_array_has(engine->ctx_data.vertex_buffers, id)) {
            printf("[FAIL] Vertex buffer %d could not be found!\n", id);
            break;
        };
        amgl_vertex_buffer vertex_buffer = am_packed_array_get_val(engine->ctx_data.vertex_buffers, id);
        am_dyn_array_push(engine->ctx_data.frame_cache.vertex_buffers, vertex_buffer);
    };

    for (am_uint32 i = 0; i < index_count; i++) {
        am_int32 id = info->index_buffers.info[i].index_buffer_id;;
        if (!am_packed_array_has(engine->ctx_data.index_buffers, id)) {
            printf("[FAIL] Index buffer %d could not be found!\n", id);
            break;
        };
        amgl_index_buffer index_buffer = am_packed_array_get_val(engine->ctx_data.index_buffers, id);
        engine->ctx_data.frame_cache.index_buffer = index_buffer;
    };

    for (am_uint32 i = 0; i < uniform_count; i++) {
        am_int32 id = info->uniforms.info[i].uniform_id;
        size_t size = am_packed_array_get_ptr(engine->ctx_data.uniforms, id)->size;
        am_uint32 binding = info->uniforms.info[i].binding;

        if (!am_packed_array_has(engine->ctx_data.uniforms, id)) {
            printf("[FAIL] Uniform %d could not be found!\n", id);
            break;
        };

        if (!am_packed_array_has(engine->ctx_data.pipelines, engine->ctx_data.frame_cache.pipeline.id)) {
            printf("[FAIL] Cannot bind uniform %d since pipeline %d could not be found!\n", id, engine->ctx_data.frame_cache.pipeline.id);
            break;
        }
        amgl_pipeline *pipeline = am_packed_array_get_ptr(engine->ctx_data.pipelines, engine->ctx_data.frame_cache.pipeline.id);
        amgl_uniform *uniform = am_packed_array_get_ptr(engine->ctx_data.uniforms, id);

        am_int32 shader_id = pipeline->raster.shader_id;

        if (uniform->location == 0xFFFFFFFF || uniform->shader_id != pipeline->raster.shader_id) {
            if (!am_packed_array_has(engine->ctx_data.shaders, shader_id)) {
                printf("[FAIL] Cannot bind uniform %d since shader %d could not be found!\n", id, shader_id);
                break;
            };
            amgl_shader shader = am_packed_array_get_val(engine->ctx_data.shaders, shader_id);
            uniform->location = glGetUniformLocation(shader.handle, strlen(uniform->name) ? uniform->name : "AM_UNIFORM_EMPTY_NAME");
            if (uniform->location >= 0xFFFFFFFF) {
                printf("[FAIL] Uniform %d not found by win_name!\n", uniform->id);
                break;
            };
            uniform->shader_id = shader_id;
            uniform->data = info->uniforms.info->data;

            //TODO: Expand
            switch (uniform->type) {
                case AMGL_UNIFORM_TYPE_FLOAT: {
                    glUniform1f((am_int32)uniform->location, *((float*)uniform->data));
                    break;
                };
                case AMGL_UNIFORM_TYPE_INT: {
                    glUniform1i((am_int32)uniform->location, *((int*)uniform->data));
                    break;
                };
                case AMGL_UNIFORM_TYPE_SAMPLER2D: {
                    amgl_texture *texture = am_packed_array_get_ptr(engine->ctx_data.textures, *((am_int32*)(uniform->data)));
                    glActiveTexture(GL_TEXTURE0 + binding);
                    glBindTexture(GL_TEXTURE_2D, texture->handle);
                    glUniform1i((am_int32)uniform->location, (am_int32)binding); //binding++ for when I add uniform list here
                    break;
                };
                default: {
                    printf("[FAIL] Invalid uniform type!\n");
                    exit(1);
                };
            };
        };
    };

    for (am_uint32 i = 0; i < texture_count; i++) {
        am_int32 id = info->textures.info[i].texture_id;

        if (!am_packed_array_has(engine->ctx_data.textures, id)) {
            printf("[FAIL] Texture %d could not be found!\n", id);
            break;       
        };
        amgl_texture *texture = am_packed_array_get_ptr(engine->ctx_data.textures, id);
        am_int32 format = amgl_texture_translate_format(texture->format);

        //TODO: READ_WRITE for now, will have to implement some kind of image layer on top of a texture
        glBindImageTexture(0, texture->handle, 0, GL_FALSE, 0, GL_READ_WRITE, format);
    };
};

void amgl_draw(amgl_draw_info *info) {
    am_engine *engine = am_engine_get_instance();
    amgl_pipeline *pipeline = am_packed_array_get_ptr(engine->ctx_data.pipelines, engine->ctx_data.frame_cache.pipeline.id);

    if (am_dyn_array_get_count(engine->ctx_data.frame_cache.vertex_buffers) == 0) {
        printf("[FAIL] No vertex buffer bound at draw time!\n");
        exit(0);
    };

    for (am_uint32 i = 0; i < pipeline->layout.num_attribs; i++) {
        am_uint32 vertex_idx = pipeline->layout.attributes[i].buffer_index;
        amgl_vertex_buffer vertex_buffer = (vertex_idx < am_dyn_array_get_count(engine->ctx_data.frame_cache.vertex_buffers)) ? engine->ctx_data.frame_cache.vertex_buffers[vertex_idx] : engine->ctx_data.frame_cache.vertex_buffers[0];

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.handle);
        size_t stride = pipeline->layout.attributes[i].stride;
        size_t offset = pipeline->layout.attributes[i].offset;
        glEnableVertexAttribArray(i);
        switch (pipeline->layout.attributes[i].format) {
                
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT4: glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT3: glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT2: glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT:  glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT4:  glVertexAttribIPointer(i, 4, GL_UNSIGNED_INT, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT3:  glVertexAttribIPointer(i, 3, GL_UNSIGNED_INT, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT2:  glVertexAttribIPointer(i, 2, GL_UNSIGNED_INT, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT:   glVertexAttribIPointer(i, 1, GL_UNSIGNED_INT, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE:   glVertexAttribPointer(i, 1, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE2:  glVertexAttribPointer(i, 2, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE3:  glVertexAttribPointer(i, 3, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(uintptr_t)offset); break;
            case AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE4:  glVertexAttribPointer(i, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(uintptr_t)offset); break;
            default: {
                printf("[FAIL] Invalid layout attribute format!\n");
                exit(0);
            };
        };
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    };

    if (am_packed_array_has(engine->ctx_data.index_buffers, engine->ctx_data.frame_cache.index_buffer.id)) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, engine->ctx_data.frame_cache.index_buffer.handle);
    };

    //TODO below
    //FIX SAME AS AMGL_PIPELINE_BIND WINDING PROBLEM
    am_uint32 primitive = pipeline->raster.primitive == 0 ? GL_TRIANGLES : GL_LINES;
    am_uint32 idx_buf_elem_size = pipeline->raster.index_buffer_element_size;

    if (am_packed_array_has(engine->ctx_data.index_buffers, engine->ctx_data.frame_cache.index_buffer.id)) {
        glDrawElements(primitive, info->count, GL_UNSIGNED_INT, (void*)(intptr_t)info->start);
    } else {
        glDrawArrays(primitive, info->start, info->count);
    };

};


//----------------------------------------------------------------------------//
//                                 END GL IMPL                                //
//----------------------------------------------------------------------------//



//----------------------------------------------------------------------------//
//                              START ENGINE IMPL                             //
//----------------------------------------------------------------------------//

void am_engine_create(am_engine_info engine_info){
    am_engine_get_instance() = (am_engine*)am_malloc(sizeof(am_engine));
    am_engine *engine = am_engine_get_instance();
    engine->init = engine_info.init;
    engine->update = engine_info.update;
    engine->shutdown = engine->shutdown;
    //engine->is_running = true;
    engine->vsync_enabled = engine_info.vsync_enabled;

    engine->platform = am_platform_create();
    am_platform_timer_create();

    engine->ctx_data.textures = NULL;
    am_packed_array_init(engine->ctx_data.textures, sizeof(amgl_texture)*AM_DYN_ARRAY_EMPTY_START_SLOTS);
    engine->ctx_data.shaders = NULL;
    am_packed_array_init(engine->ctx_data.shaders, sizeof(amgl_shader)*AM_DYN_ARRAY_EMPTY_START_SLOTS);
    engine->ctx_data.vertex_buffers = NULL;
    am_packed_array_init(engine->ctx_data.vertex_buffers, sizeof(amgl_vertex_buffer)*AM_DYN_ARRAY_EMPTY_START_SLOTS);
    engine->ctx_data.index_buffers = NULL;
    am_packed_array_init(engine->ctx_data.index_buffers, sizeof(amgl_index_buffer)*AM_DYN_ARRAY_EMPTY_START_SLOTS);
    engine->ctx_data.frame_buffers = NULL;
    am_packed_array_init(engine->ctx_data.frame_buffers, sizeof(amgl_frame_buffer)*AM_DYN_ARRAY_EMPTY_START_SLOTS);
    engine->ctx_data.uniforms = NULL;
    am_packed_array_init(engine->ctx_data.uniforms, sizeof(amgl_uniform)*AM_DYN_ARRAY_EMPTY_START_SLOTS);
    engine->ctx_data.render_passes = NULL;
    am_packed_array_init(engine->ctx_data.render_passes, sizeof(amgl_render_pass)*AM_DYN_ARRAY_EMPTY_START_SLOTS);
    engine->ctx_data.pipelines = NULL;
    am_packed_array_init(engine->ctx_data.pipelines, sizeof(amgl_pipeline)*AM_DYN_ARRAY_EMPTY_START_SLOTS);
    engine->ctx_data.frame_cache.vertex_buffers = NULL;
    am_dyn_array_init((void**)&(engine->ctx_data.frame_cache.vertex_buffers), sizeof(amgl_vertex_buffer));

    am_window_info main = {
        .height = engine_info.win_height,
        .width = engine_info.win_width,
        .x = engine_info.win_x,
        .y = engine_info.win_y,
        .parent = AM_WINDOW_DEFAULT_PARENT,
        .is_fullscreen = engine_info.win_fullscreen
    };
    printf("sl ec %lu\n", strlen(engine_info.win_name));
    if (!strlen(engine_info.win_name)) snprintf(main.name, AM_MAX_NAME_LENGTH, "%s", engine_info.win_name);

    printf("b win\n");
    am_platform_window_create(main);
    printf("a win\n");
    #if defined(AM_LINUX)
    //XSetWindowBackground(am_engine_get_subsystem(platform)->display, main_wind->handle, 0x0000FF);
    #endif
    amgl_init();
    amgl_set_viewport(0,0, (am_int32)main.width, (am_int32)main.height);
    //HACK: Temporary
    engine_info.is_running = true;
};

void am_engine_terminate(){
    am_engine *engine = am_engine_get_instance();
    am_platform_terminate(am_engine_get_subsystem(platform));

    for (am_int32 i = 0; i < am_packed_array_get_count(engine->ctx_data.textures); i++) amgl_texture_destroy(engine->ctx_data.textures->elements[i].id);
    am_packed_array_destroy(engine->ctx_data.textures);
    for (am_int32 i = 0; i < am_packed_array_get_count(engine->ctx_data.vertex_buffers); i++) amgl_vertex_buffer_destroy(engine->ctx_data.vertex_buffers->elements[i].id);
    am_packed_array_destroy(engine->ctx_data.vertex_buffers);
    for (am_int32 i = 0; i < am_packed_array_get_count(engine->ctx_data.index_buffers); i++) amgl_index_buffer_destroy(engine->ctx_data.index_buffers->elements[i].id);
    am_packed_array_destroy(engine->ctx_data.index_buffers);
    for (am_int32 i = 0; i < am_packed_array_get_count(engine->ctx_data.frame_buffers); i++) amgl_frame_buffer_destroy(engine->ctx_data.frame_buffers->elements[i].id);
    am_packed_array_destroy(engine->ctx_data.frame_buffers);
    for (am_int32 i = 0; i < am_packed_array_get_count(engine->ctx_data.uniforms); i++) amgl_uniform_destroy(engine->ctx_data.uniforms->elements[i].id);
    am_packed_array_destroy(engine->ctx_data.uniforms);
    for (am_int32 i = 0; i < am_packed_array_get_count(engine->ctx_data.render_passes); i++) amgl_render_pass_destroy(engine->ctx_data.render_passes->elements[i].id);
    am_packed_array_destroy(engine->ctx_data.render_passes);
    for (am_int32 i = 0; i < am_packed_array_get_count(engine->ctx_data.pipelines); i++) amgl_pipeline_destroy(engine->ctx_data.pipelines->elements[i].id);
    am_packed_array_destroy(engine->ctx_data.pipelines);
    am_dyn_array_destroy(engine->ctx_data.frame_cache.vertex_buffers);


    amgl_terminate();
    //HACK: Temporary
    engine->is_running = false;
    am_free(engine);
};


//----------------------------------------------------------------------------//
//                               END ENGINE IMPL                              //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                               START UTIL IMPL                              //
//----------------------------------------------------------------------------//

char* am_util_read_file(const char *path) {
    FILE *source = fopen(path, "rb");
    am_int32 rd_size = 0;
    char* buffer = NULL;
    if (source) {
        #if defined(AM_LINUX)
        struct stat st;
        stat(path, &st);
        rd_size = st.st_size;
        #else
        HANDLE file_hwnd = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        LARGE_INTEGER size;
        GetFileSizeEx(file_hwnd, &size);
        rd_size = (am_int32) size.QuadPart;
        CloseHandle(file_hwnd);
        #endif
        buffer = (char*)am_malloc(rd_size+1);
        if (buffer) fread(buffer, 1, rd_size, source);
        buffer[rd_size] = '\0';
    };
    fclose(source);
    return buffer;
};

//----------------------------------------------------------------------------//
//                               END  UTIL IMPL                               //
//----------------------------------------------------------------------------//

int main() {
    am_engine_create((am_engine_info){
        .win_fullscreen = false,
        .win_height = 500,
        .win_width = 500,
        .win_x = 50,
        .win_y = 50,
        .vsync_enabled = true,
        .is_running = true
        //.win_name = "Testing"
    });

    am_platform *platform = am_engine_get_subsystem(platform);
    glXMakeCurrent(am_engine_get_subsystem(platform)->display, am_packed_array_get_ptr(platform->windows, 0)->handle, am_packed_array_get_ptr(platform->windows, 0)->context);
    XSetWindowBackground(am_engine_get_subsystem(platform)->display, am_packed_array_get_ptr(platform->windows, 0)->handle, 0xFF0000);

    am_id shader_id = amgl_shader_create((amgl_shader_info) {
        .num_sources = 2,
        .sources = (amgl_shader_source_info[]) {
            { .type = AMGL_SHADER_VERTEX, .source = am_util_read_file("/home/truta/CLionProjects/ametrine/test_v.glsl") },
            { .type = AMGL_SHADER_FRAGMENT, .source = am_util_read_file("/home/truta/CLionProjects/ametrine/test_f.glsl") }
        }
    });
    
    
    amgl_texture_info tex_info = {
        .format = AMGL_TEXTURE_FORMAT_RGBA,
        .mag_filter = AMGL_TEXTURE_FILTER_LINEAR,
        .min_filter = AMGL_TEXTURE_FILTER_LINEAR,
        .wrap_s = AMGL_TEXTURE_WRAP_REPEAT,
        .wrap_t = AMGL_TEXTURE_WRAP_REPEAT
    };
    amgl_texture_load_from_file("/home/truta/CLionProjects/ametrine/pics/t3.png", &tex_info, true);
    am_id tex_id = amgl_texture_create(tex_info);

    
    am_id uni_id = amgl_uniform_create((amgl_uniform_info){
        .name = "u_tex",
        .type = AMGL_UNIFORM_TYPE_SAMPLER2D
    });

    am_id vbo_id = amgl_vertex_buffer_create((amgl_vertex_buffer_info){
        .data = (float[]) {
            -0.5f, -0.5f,  0.0f, 0.0f,  // Top Left
            0.5f, -0.5f,  1.0f, 0.0f,  // Top Right 
            -0.5f,  0.5f,  0.0f, 1.0f,  // Bottom Left
            0.5f,  0.5f,  1.0f, 1.0f   // Bottom Right
        },
        .size = sizeof(float)*16,
        .usage = AMGL_BUFFER_USAGE_STATIC,
    });

    
    am_id idx_id = amgl_index_buffer_create((amgl_index_buffer_info){
        .data = (am_uint32[]){
            0, 3, 2,    // First Triangle
            0, 1, 3     // Second Triangle
        },
        .size = sizeof(am_uint32)*6,
        .offset = 0,
        .usage = AMGL_BUFFER_USAGE_STATIC
    });

    am_id pipeline_id = amgl_pipeline_create((amgl_pipeline_info){
        .raster = {
            .shader_id = shader_id,
        },
        .layout = {
            .num_attribs = 2,
            .attributes = (amgl_vertex_buffer_attribute[]){
                {.format = AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT2, .offset = 0, .stride = 2*sizeof(float)},
                {.format = AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT2, .offset = 2*sizeof(float), .stride = 2*sizeof(float)}
            }
        }
    });

    am_id rp_id = amgl_render_pass_create((amgl_render_pass_info){0});

    printf("IDS: %u %u %u\n", rp_id, idx_id, pipeline_id);

    am_bool run = true;
    am_engine *engine = am_engine_get_instance();
    while (run) {
        am_platform_update(am_engine_get_subsystem(platform));
        amgl_bindings_info binds = {
            .vertex_buffers = {.info = &(amgl_vertex_buffer_bind_info){.vertex_buffer_id = vbo_id}, .size = sizeof(amgl_vertex_buffer_bind_info)},
            .index_buffers = {.info = &(amgl_index_buffer_bind_info){.index_buffer_id = idx_id}},
            .uniforms = {.info = &(amgl_uniform_bind_info){.uniform_id = uni_id, .data = &tex_id, .binding = 0}}
        };
        amgl_start_render_pass(rp_id);
            glEnable(GL_BLEND);
            glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            amgl_bind_pipeline(pipeline_id);
            amgl_set_bindings(&binds);
            amgl_draw(&(amgl_draw_info){.start = 0, .count = 6});
        amgl_end_render_pass(rp_id);
        //TODO: Will have to swap buffers for all existing windows
        glXSwapBuffers(platform->display, am_packed_array_get_ptr(platform->windows, 1)->handle);
        if (am_platform_key_pressed(AM_KEYCODE_X)) {
            am_engine_terminate();
            run = !run;
        };
    };

    return 0;
};
