//13-nov-2021 1:03:10PM
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
#include <math.h>
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
typedef enum {FAILURE, SUCCESS, IN_PROGRESS} am_result;

#define AM_DYN_ARRAY_EMPTY_START_SLOTS 2

#define AM_MAX_KEYCODE_COUNT 512
#define AM_ROOT_WIN_CLASS "AM_ROOT"
#define AM_CHILD_WIN_CLASS "AM_CHILD"

#if defined(AM_LINUX)
    #define _CALL *
    #define amgl_get_proc_address(str) glXGetProcAddress((unsigned char*)(str))
#else
    #define _CALL __stdcall*
    #define amgl_get_proc_address(str) wglGetProcAddress((str))
#endif

//HACK: Temporary
am_bool temp_check = true;

//REVIEW: OpenGL functions

#if defined(AM_LINUX)
typedef void (_CALL PFNGLXSWAPINTERVALEXTPROC) (Display *dpy, GLXDrawable drawable, int interval);
#else
typedef BOOL (_CALL PFNWGLSWAPINTERVALEXTPROC) (int interval);
#endif
typedef GLuint (_CALL PFNGLCREATESHADERPROC) (GLenum type);
typedef void (_CALL PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (_CALL PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef void (_CALL PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (_CALL PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (_CALL PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (_CALL PFNGLDELETESHADERPROC) (GLuint shader);
typedef GLuint (_CALL PFNGLCREATEPROGRAMPROC) (void);
typedef void (_CALL PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (_CALL PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (_CALL PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (_CALL PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (_CALL PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (_CALL PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (_CALL PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (_CALL PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (_CALL PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (_CALL PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (_CALL PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef void (_CALL PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
typedef void (_CALL PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (_CALL PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (_CALL PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef GLint (_CALL PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (_CALL PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef GLint (_CALL PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (_CALL PFNGLGENERATEMIPMAPPROC) (GLenum target);
typedef void (_CALL PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
typedef void (_CALL PFNGLGENFRAMEBUFFERSPROC) (GLsizei n, GLuint *ids);
//FIX: Fix this, conflicting types
//typedef void (_CALL PFNGLDELETEFRAMEBUFFERSPROC) (GLsizei n, GLuint *framebuffers);
typedef void (_CALL PFNGLBINDFRAMEBUFFERPROC) (GLenum target, GLuint framebuffer);
typedef void (_CALL PFNGLFRAMEBUFFERTEXTURE2DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (_CALL PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (_CALL PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
typedef void (_CALL PFNGLGETUNIFORMFVPROC) (GLuint program, GLint location, GLfloat *params);


#if defined(AM_LINUX)
PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = NULL;
#else 
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
#endif
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


//----------------------------------------------------------------------------//
//                             END TYPES AND DEFS                             //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                               DYNAMIC  ARRAY                               //
//----------------------------------------------------------------------------//

typedef struct am_dyn_array {
    void *data;             //Pointer to data block
    size_t size;            //Size in bytes of occupied space
    size_t space;           //Size in bytes of available space
    size_t element_size;    //Size in bytes of a single element
} am_dyn_array;

void am_dyn_array_init(am_dyn_array *array, size_t element_size);
void am_dyn_array_clear(am_dyn_array *array);
void am_dyn_array_resize(am_dyn_array *array, size_t new_size);
void am_dyn_array_push(am_dyn_array *array, am_uint32 count, void *elements);
void am_dyn_array_pop(am_dyn_array *array, am_uint32 count);
void am_dyn_array_erase(am_dyn_array *array, am_uint32 pos, am_uint32 count);

#define am_dyn_array_data_length(array, count) ((array)->element_size * (count))
#define am_dyn_array_data_offset(array, index) ((array)->size + (array)->element_size * index)
#define am_dyn_array_data_retrieve(array, index, type) (*(type*)(am_dyn_array_data_offset((array), (index)))
#define am_dyn_array_data_retrieve_ptr(array, index, type) ((type*)(am_dyn_array_data_offset((array), (index)))

//----------------------------------------------------------------------------//
//                             END DYNAMIC  ARRAY                             //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                                PACKED ARRAY                                //
//----------------------------------------------------------------------------//

typedef struct am_packed_array {
    am_dyn_array elements;
    am_dyn_array indices;
    am_int32 next_id;
    am_uint32 num_elements;
} am_packed_array;

void am_packed_array_init(am_packed_array *pa, size_t element_size);
void am_packed_array_clear(am_packed_array *pa);
am_int32 am_packed_array_add(am_packed_array *pa, void* element);
void am_packed_array_erase(am_packed_array *pa, am_int32 id);


#define am_packed_array_get_val(array_ptr, index, type) (*(type*)((array_ptr)->elements.data + (array_ptr)->elements.element_size * (index)))
#define am_packed_array_get_idx(array_ptr, id, type) (*(type*)((array_ptr)->indices.data + (array_ptr)->indices.element_size * (id)))
#define am_packed_array_get_ptr(array_ptr, index, type) ((type*)((array_ptr)->elements.data + (array_ptr)->elements.element_size * (index)))
#define am_packed_array_lookup(array_ptr, id, type) ((type*)((array_ptr)->elements.data + (array_ptr)->elements.element_size * (*(am_uint32*)((array_ptr)->indices.data + (array_ptr)->indices.element_size * (id)))))
#define am_packed_array_length(array_ptr) ((array_ptr)->elements.size / (array_ptr)->elements.element_size)
#define am_packed_array_has(array_ptr, id) (((id) < (array_ptr)->next_id) && (am_packed_array_get_idx((array_ptr), (id), am_uint32) != 0xFFFFFFFF) ? 1:0)

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


#if defined(AM_LINUX) 
    #define AM_WINDOW_ROOT_PARENT DefaultRootWindow(am_engine_get_subsystem(platform)->display) 
#else 
    #define AM_WINDOW_ROOT_PARENT 0
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

typedef enum am_events {
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
} am_events;

typedef struct am_window_info {
    am_uint64 parent;
    const char* title;
    am_uint32 width;
    am_uint32 height; 
    am_uint32 x;
    am_uint32 y;
    //am_vec2i window_position; 
    am_bool is_fullscreen; //Useless for child windows
} am_window_info;

typedef struct am_window {
    am_uint64 handle;
    am_int32 am_id; 
    am_window_info info;
    am_window_info cache;
    
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
    void (*am_platform_key_callback)(am_int32, am_key_map, am_events);
    void (*am_platform_mouse_button_callback)(am_int32, am_mouse_map, am_events);
    void (*am_platform_mouse_motion_callback)(am_int32, am_int32, am_int32, am_events);
    void (*am_platform_mouse_scroll_callback)(am_int32, am_events);
    void (*am_platform_window_size_callback)(am_int32, am_uint32, am_uint32, am_events);
    void (*am_platform_window_motion_callback)(am_int32, am_uint32, am_uint32, am_events);
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
    am_packed_array windows; //of am_window
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
LRESULT CALLBACK am_platform_win32_event_handler(HWND handle, am_uint32 event, WPARAM wparam, LPARAM lparam);
#else
void am_platform_linux_event_handler(XEvent *xevent);
#endif
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
void am_platform_mouse_position(am_int32 *x, am_int32 *y); 
am_vec2 am_platform_mouse_positionv();
am_int32 am_platform_mouse_wheel_delta();
am_bool am_platform_mouse_moved();

//Platform default callbacks
void am_platform_key_callback_default(am_int32 id, am_key_map key, am_events event);
void am_platform_mouse_button_callback_default(am_int32 idow_handle, am_mouse_map button, am_events event);
void am_platform_mouse_motion_callback_default(am_int32 idow_handle, am_int32 x, am_int32 y, am_events event);
void am_platform_mouse_scroll_callback_default(am_int32 id, am_events event);
void am_platform_window_size_callback_default(am_int32 id, am_uint32 width, am_uint32 height, am_events event);
void am_platform_window_motion_callback_default(am_int32 id, am_uint32 x, am_uint32 y, am_events event);

#define am_platform_set_key_callback(platform, callback) platform->callbacks.am_platform_key_callback = callback
#define am_platform_set_mouse_button_callback(platform, callback) platform->callbacks.am_platform_mouse_button_callback = callback
#define am_platform_set_mouse_motion_callback(platform, callback) platform->callbacks.am_platform_mouse_motion_callback = callback
#define am_platform_set_mouse_scroll_callback(platform, callback) platform->callbacks.am_platform_mouse_scroll_callback = callback  
#define am_platform_set_window_size_callback(platform, callback) platform->callbacks.am_platform_window_size_callback = callback  
#define am_platform_set_window_motion_callback(platform, callback) platform->callbacks.am_platform_window_motion_callback = callback  

//Windows
am_int32 am_platform_window_create(am_window_info window_info);
am_window *am_platform_window_lookup(am_int32 id); 
am_window *am_platform_window_lookup_by_handle(am_uint64 handle);
am_window *am_platform_window_retrieve(am_uint32 index);
am_uint32 am_platform_window_index_lookup(am_int32 id);
am_bool am_platform_window_exists(am_int32 id);
void am_platform_window_resize(am_int32 id, am_uint32 width, am_uint32 height);
void am_platform_window_move(am_int32 id, am_uint32 x, am_uint32 y);
void am_platform_window_fullscreen(am_int32 id, am_bool state);
void am_platform_window_terminate(am_int32 id);

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

//https://www.khronos.org/opengl/wiki/OpenGL_Object
//BUFFERS shaders\|, textures \|, vertex \|, index \|, frame \|, uniform \|

//TODO: Compute Shaders
//Shader & program
//Removed compute shader for now
typedef enum amgl_shader_type {
    AMGL_SHADER_VERTEX,
    AMGL_SHADER_FRAGMENT
} amgl_shader_type;

typedef struct amgl_shader_source_info {
    amgl_shader_type type;
    char *source;
} amgl_shader_source_info;

typedef struct amgl_shader_info {
    //char *identifier;
    amgl_shader_source_info *sources;
    am_uint32 num_sources;
} amgl_shader_info;

typedef struct amgl_shader {
    am_uint32 handle;
    am_int32 am_id;
    amgl_shader_info info;
} amgl_shader;

typedef enum amgl_buffer_usage {
    AMGL_BUFFER_USAGE_DYNAMIC,
    AMGL_BUFFER_USAGE_STATIC,
    AMGL_BUFFER_USAGE_STREAM
} amgl_buffer_usage;

/*
typedef enum amgl_buffer_update_type {
    AMGL_BUFFER_UPDATE_SUBDATA,
    AMGL_BUFFER_UPDATE_RECREATE
} amgl_buffer_update_type;
*/

typedef enum amgl_vertex_buffer_attribute_format {
AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT4, 
AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT3,
AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT2,
AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT,
AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT4, 
AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT3, 
AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT2, 
AMGL_VERTEX_BUFFER_ATTRIBUTE_UINT,  
AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE, 
AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE2,
AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE3,
AMGL_VERTEX_BUFFER_ATTRIBUTE_BYTE4
} amgl_vertex_buffer_attribute_format;

typedef struct amgl_vertex_buffer_attribute {
    size_t stride;
    size_t offset;
    amgl_vertex_buffer_attribute_format format;
} amgl_vertex_buffer_attribute;

typedef struct amgl_vertex_buffer_layout {
    amgl_vertex_buffer_attribute *attributes;
    am_uint32 num_attribs;
} amgl_vertex_buffer_layout;

typedef struct amgl_vertex_buffer_info {
    void *data;
    size_t size;
    amgl_buffer_usage usage;
    amgl_vertex_buffer_layout layout;
} amgl_vertex_buffer_info;

typedef struct amgl_vertex_buffer {
    am_uint32 handle;
    am_int32 am_id;
    amgl_vertex_buffer_info info;
} amgl_vertex_buffer;

/*
typedef struct amgl_vertex_buffer_update_info {
    amgl_vertex_buffer_info info; 
    amgl_buffer_update_type update_type;
    size_t offset; 
} amgl_vertex_buffer_update_info;
*/

typedef struct amgl_index_buffer_info {
    void *data;
    size_t size;
    size_t offset;
    amgl_buffer_usage usage;
} amgl_index_buffer_info;

typedef struct amgl_index_buffer {
    am_int32 am_id;
    am_uint32 handle;
    amgl_index_buffer_info info;
} amgl_index_buffer;

//TODO: Support for more uniform types
typedef enum amgl_uniform_type {
    AMGL_UNIFORM_TYPE_FLOAT,
    AMGL_UNIFORM_TYPE_INT,
    AMGL_UNIFORM_TYPE_SAMPLER2D
} amgl_uniform_type;

typedef struct amgl_uniform_info {
    void* data;
    const char* name;
    size_t size;
    GLenum type;
    am_uint32 shader_id;
    //am_uint32 texture_slot; //NOTE: Ignore for now
} amgl_uniform_info;

typedef struct amgl_uniform {
    am_int32 am_id;
    am_uint32 location;
    amgl_uniform_info info;
} amgl_uniform;

//TODO: Textures: Change loading style, delete after upload to GPU
//Textures
/*
typedef enum amgl_texture_update_type {
    AMGL_TEXTURE_UPDATE_RECREATE,
    AMGL_TEXTURE_UPDATE_SUBDATA
    //TODO: PBO, FBO
} amgl_texture_update_type;
*/

//TODO: Specify formats in another enum
typedef struct amgl_texture_info {
    const char *name;
    void *data;
    am_uint32 width;
    am_uint32 height;
    GLenum format;
    GLenum min_filter;
    GLenum mag_filter;
    GLenum mip_filter;
    am_uint32 mip_num;
    GLenum wrap_s;
    GLenum wrap_t;
} amgl_texture_info;

typedef struct amgl_texture {
    am_uint32 handle;
    am_int32 am_id;
    //NOTE: Ignore for now
    //am_bool render_traget;
    amgl_texture_info info;
} amgl_texture;

//NOTE: Technically nothing is needed
//REVIEW
typedef struct amgl_frame_buffer_info {
    am_uint32 width;
    am_uint32 height;
} amgl_frame_buffer_info;

typedef struct amgl_frame_buffer {
    am_int32 am_id;
    am_uint32 handle;
    amgl_frame_buffer_info info;
} amgl_frame_buffer;

typedef struct amgl_draw_info {
    am_uint32 start;
    am_uint32 count;
} amgl_draw_info;

//IDEA: Lookup by handle could be helpful?

//Shaders
am_int32 amgl_shader_create(amgl_shader_info info);
amgl_shader *amgl_shader_lookup(am_int32 id);
am_uint32 amgl_shader_index_lookup(am_int32 id);
amgl_shader *amgl_shader_retrieve(am_uint32 index);
void amgl_shader_destroy(am_int32 id);

//Vertex buffer
am_int32 amgl_vertex_buffer_create(amgl_vertex_buffer_info info);
//void amgl_vertex_buffer_update(am_int32 id, amgl_vertex_buffer_update_info update);
amgl_vertex_buffer *amgl_vertex_buffer_lookup(am_int32 id);
am_uint32 amgl_vertex_buffer_index_lookup(am_int32 id);
amgl_vertex_buffer *amgl_vertex_buffer_retrieve(am_uint32 index);
void amgl_vertex_buffer_destroy(am_int32 id);

//Index buffer
am_int32 amgl_index_buffer_create(amgl_index_buffer_info info);
amgl_index_buffer *amgl_index_buffer_lookup(am_int32 id);
am_uint32 amgl_index_buffer_index_lookup(am_int32 id);
amgl_index_buffer *amgl_index_buffer_retrieve(am_uint32 index);
void amgl_index_buffer_destroy(am_int32 id);

//Uniform
am_int32 amgl_uniform_create(amgl_uniform_info info);
amgl_uniform *amgl_uniform_lookup(am_int32 id);
am_uint32 amgl_uniform_index_lookup(am_int32 id);
amgl_uniform *amgl_uniform_retrieve(am_uint32 index);
//NOTE: Ignore for now
//void amgl_uniform_update(am_int32 id, amgl_uniform_info info);
void amgl_uniform_destroy(am_int32 id);
//TODO: Merge into big bind function
//NOTE: Ignore for now
//void amgl_uniform_bind(amgl_uniform *uniforms, am_uint32 num);

//Texture
am_int32 amgl_texture_create(amgl_texture_info info);
//NOTE: Ignore for now
//void amgl_texture_update(am_int32 id, amgl_texture_info info, amgl_texture_update_type type);
void amgl_texture_load_from_file(const char *path, amgl_texture_info *info, am_bool flip);
void amgl_texture_load_from_memory(const void *memory, amgl_texture_info *info, size_t size, am_bool flip);
amgl_texture *amgl_texture_lookup(am_int32 id);
am_uint32 amgl_texture_index_lookup(am_int32 id);
amgl_texture *amgl_texture_retrieve(am_uint32 index);
void amgl_texture_destroy(am_int32 id);

//Framebuffer
am_int32 amgl_frame_buffer_create(amgl_frame_buffer_info info);
amgl_frame_buffer *amgl_frame_buffer_retrieve(am_int32 index);
amgl_frame_buffer *amgl_frame_buffer_lookup(am_int32 id);
am_uint32 amgl_frame_buffer_index_lookup(am_int32 id);
void amgl_frame_buffer_destroy(am_int32 id);

//Various OGL
void amgl_init(); //Create arrays for shaders, vertex b, index b, frame b etc, init gl addresses
void amgl_terminate();
void amgl_set_viewport(am_int32 x, am_int32 y, am_int32 width, am_int32 height);
void amgl_vsync(am_window *window, am_bool state);
void amgl_draw(amgl_draw_info info);

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
    //Variables marked with initial_ will not be updated during runtime
    char *initial_title;
    am_bool initial_fullscreen;
    am_uint32 initial_width;
    am_uint32 initial_height;
    am_uint32 initial_x;
    am_uint32 initial_y;
} am_engine_info;

typedef struct amgl_ctx_data {
    am_packed_array textures; //of amgl_texture
    am_packed_array shaders; //of amgl_shader
    am_packed_array vertex_buffers; //of amgl_vertex_buffer
    am_packed_array index_buffers; //of amgl_index_buffer
    am_packed_array frame_buffers; //of amgl_frame_buffer
    am_packed_array uniforms;  //of amgl_uniform

    //TODO: Rest of object arrays and other user data
} amgl_ctx_data;


typedef struct am_engine {
    am_engine_info info;
    am_platform *platform;
    amgl_ctx_data ctx_data;
    //TODO: Change cache to scene
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
/*

//FIX 
//FIX 
//FIX 
//FIX 
// REVIEW
//TODO: Remove vertex_buffer and index_buffer from amgl_draw_info and instead bind them here
amgl_apply_bindings(amgl_bind_info info);
amgl_bind_info {
    framebuffer (with subelements)
    uniform (with subelements)
    texture (with subelements)
    vertex 
    index
}

amgl_clear_bindings(FLAGS)
    eg  framebuffer 00000001
        uniform     00000010
        texture     00000100
    etc

ENGINE
   |
   - PLATFORM
   |    |
   |    - INPUT
   |    - TIME
   |    - WINDOW ARRAY
   |    - CALLBACKS
   |
   - AUDIO
   |
   - PFNGL* //Maybe not needed here
   |   |
   |   - Pointers to all OpenGL functions needed
   |
   - USER DATA POINTER
*/

//----------------------------------------------------------------------------//
//                             DYNAMIC  ARRAY IMPL                            //
//----------------------------------------------------------------------------//

//Initialized with TA_EMPTY_START_SLOTS empty slots, cuts down a few reallocs and doesn't occupy much memory
void am_dyn_array_init(am_dyn_array *array, size_t element_size) {
    array->data = malloc(AM_DYN_ARRAY_EMPTY_START_SLOTS * sizeof(element_size));
    assert(array->data);
    array->size = 0;
    array->space = AM_DYN_ARRAY_EMPTY_START_SLOTS * sizeof(element_size);
    array->element_size = element_size;
};

void am_dyn_array_clear(am_dyn_array *array) {
    free(array->data);
    memset((void*)array, 0, sizeof(am_dyn_array));
};

//Grows by the powers of 2, will optimize for memory usage if necessary in the future
//NOTE: Optimal growth rate is phi (1.618) ~= 1.5
void am_dyn_array_resize(am_dyn_array *array, size_t new_size) {
    if (new_size <= array->space) return;
    size_t sz_size = sizeof(size_t) * 8; //Size in bits of size_t

    //Calculates next power of 2 after new_size
    new_size--;
    for (size_t i = 1; i < sz_size; i *= 2) new_size |= new_size >> i;
    new_size++;

    void *buffer = realloc(array->data, new_size);
    assert(buffer);
    array->space = new_size;
    array->data = buffer;
};

void am_dyn_array_push(am_dyn_array *array, am_uint32 count, void *elements) {
    size_t needed_size = array->size + array->element_size * (size_t)count;
    //Resize if needed
    am_dyn_array_resize(array, needed_size);
    //Append element list to data array
    memcpy(array->data + array->size, elements, array->element_size * (size_t)count);
    array->size += array->element_size * (size_t)count;
};

void am_dyn_array_pop(am_dyn_array *array, am_uint32 count) {
    array->size -= array->element_size * (size_t)count;
};

void am_dyn_array_erase(am_dyn_array *array, am_uint32 pos, am_uint32 count) {
    size_t deletion_size = array->element_size * (size_t)count; //Number of bytes needed to be removed
    size_t offset = array->element_size * (size_t)pos; //Offset from the start of the array
    size_t move_size = array->size - offset - deletion_size;
    assert(move_size >= 0);
    //If deletion_size is smaller than move_size, it means I have to copy the last <count> elements and move them over
    //If move_size is smaller, I can shift whatever's after the deletion block to the left
    void* src = move_size >= deletion_size ? array->data + array->size - deletion_size: array->data + array->size - move_size;
    size_t n = move_size >= deletion_size ? deletion_size : move_size;
    memcpy(array->data + offset, src, n);
    array->size -= deletion_size;
};

//----------------------------------------------------------------------------//
//                           END DYNAMIC  ARRAY IMPL                          //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
//                             PACKED  ARRAY IMPL                             //
//----------------------------------------------------------------------------//

void am_packed_array_init(am_packed_array *parray, size_t element_size) {
    am_dyn_array_init(&parray->elements, element_size);
    am_dyn_array_init(&parray->indices, sizeof(am_uint32));
    parray->num_elements = 0;
};

void am_packed_array_clear(am_packed_array *parray) {
    am_dyn_array_clear(&parray->indices);
    am_dyn_array_clear(&parray->elements);
    memset(parray, 0, sizeof(am_packed_array));
};

am_int32 am_packed_array_add(am_packed_array *parray, void* element) {
    am_dyn_array_push(&parray->indices, 1, &parray->num_elements);
    am_dyn_array_push(&parray->elements, 1, element);
    parray->num_elements += 1;
    
    return parray->next_id++;
};

void am_packed_array_erase(am_packed_array *parray, am_int32 id) {
    am_uint32 index = am_packed_array_get_idx(parray, id, am_uint32);

    if (index == 0xFFFFFFFF) {
        printf("[WARN] Element with ID %d does not exist!\n", id);
        return;
    };

    if (!am_packed_array_has(parray, id)) {
        printf("[WARN] Invalid id %d!\n", id);
        return;
    };

    am_int32 last_element_id = -1;
    
    if (am_packed_array_get_idx(parray, id, am_uint32) == parray->num_elements - 1) {
        last_element_id = id;
    } else {
        for (am_int32 i = 0; i < parray->indices.size / parray->indices.element_size; i++) 
            if (am_packed_array_get_idx(parray, i, am_uint32) == parray->num_elements - 1) {
                last_element_id = i;
                break;
            };
    };

    //Remove element and replace with the last one
    am_dyn_array_erase(&parray->elements, index, 1);
    //Update the index of the element that moved
    am_packed_array_get_idx(parray, last_element_id, am_uint32) = index;
    //Set indices[id] to 0xFFFFFFFF
    am_packed_array_get_idx(parray, id, am_uint32) = 0xFFFFFFFF;
    --parray->num_elements;
};

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
    };
    return AM_MOUSE_BUTTON_INVALID;
};

am_platform *am_platform_create() {
    am_platform *platform = (am_platform*)am_malloc(sizeof(am_platform));
    if (platform == NULL) printf("[FAIL] Could not allocate memory for platform!\n");
    assert(platform != NULL);
    am_packed_array_init(&platform->windows, sizeof(am_window));

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
	window_class.lpfnWndProc = am_platform_win32_event_handler;
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
        am_platform_linux_event_handler(&xevent);  
    };
    #else
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
    };
    #endif 
};

#if defined(AM_LINUX) 
void am_platform_linux_event_handler(XEvent *xevent) {
    am_platform *platform = am_engine_get_subsystem(platform);
    am_uint64 handle = xevent->xany.window;
    am_int32 id = am_platform_window_lookup_by_handle(handle)->am_id;
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
            am_window *window = am_platform_window_lookup(id);
            if (window->info.height != xevent->xconfigure.height || window->info.width != xevent->xconfigure.width) {
                platform->callbacks.am_platform_window_size_callback(id, xevent->xconfigure.width, xevent->xconfigure.height, AM_EVENT_WINDOW_SIZE);
            };
            if (window->info.x != xevent->xconfigure.x || window->info.y != xevent->xconfigure.y) {
                platform->callbacks.am_platform_window_motion_callback(id, xevent->xconfigure.x, xevent->xconfigure.y, AM_EVENT_WINDOW_MOTION);
            };
            break;
        };
        case DestroyNotify: {
            printf("Destroying window %d!\n", id);
            am_packed_array_erase(&platform->windows, id);
            //for (am_int32 i = index; i < platform->windows.length; i++) --am_dyn_array_data_retrieve(&platform->windows, am_window, i)->am_id;
            //am_free(am_platform_window_lookup_by_handle(xevent->xclient.window));

            am_bool check_no_root = true;
            for (am_int32 i = 0; i < platform->windows.num_elements; i++) 
                if (am_platform_window_retrieve(i)->info.parent == AM_WINDOW_ROOT_PARENT) {
                    printf("check no root false");
                    check_no_root = false;
                    break;
                };

            if (check_no_root) am_engine_get_instance()->info.is_running = false;
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
LRESULT CALLBACK am_platform_win32_event_handler(HWND handle, am_uint32 event, WPARAM wparam, LPARAM lparam) {
    am_platform *platform = am_engine_get_subsystem(platform);
    am_uint64 window_handle = (am_uint64) handle;
    am_int32 id = am_platform_window_lookup_by_handle(handle)->am_id;
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
            //for (am_int32 i = index; i < platform->windows.length; i++) --am_dyn_array_data_retrieve(&platform->windows, am_window, index)->am_id;
            //am_free(am_platform_window_lookup(id));
            
            am_bool check_no_root = true;
            for (am_int32 i = 0; i < platform->windows.length; i++) 
                if (am_dyn_array_data_retrieve(&platform->windows, am_window, i)->info.parent == AM_WINDOW_ROOT_PARENT) {
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

//REVIEW: Passing pointer argument is probably unnecessary, only one platform instance should exist
void am_platform_update(am_platform *platform) {
    memcpy(platform->input.prev_mouse_map, platform->input.mouse_map, sizeof(platform->input.mouse_map));
    memcpy(platform->input.prev_keyboard_map, platform->input.keyboard_map, sizeof(platform->input.keyboard_map));
    platform->input.wheel_delta = 0;
    platform->input.mouse_moved = false;

    am_platform_poll_events();
};

void am_platform_terminate(am_platform *platform) {
    for (am_int32 i = 0; i <platform->windows.num_elements; i++) am_platform_window_terminate(am_platform_window_retrieve(i)->am_id);

    #if defined(AM_LINUX)
    //This sends the proper closing xevents
    am_platform_update(am_engine_get_subsystem(platform));
    #else
    UnregisterClass(AM_ROOT_WIN_CLASS, GetModuleHandle(NULL));
    UnregisterClass(AM_CHILD_WIN_CLASS, GetModuleHandle(NULL));
    #endif
    
    am_packed_array_clear(&platform->windows);
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

void am_platform_mouse_position(am_int32 *x, am_int32 *y) {
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

void am_platform_key_callback_default(am_int32 id, am_key_map key, am_events event) {
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

void am_platform_mouse_button_callback_default(am_int32 id, am_mouse_map button, am_events event) {
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

void am_platform_mouse_motion_callback_default(am_int32 id, am_int32 x, am_int32 y, am_events event) {
    am_platform *platform = am_engine_get_subsystem(platform);
    platform->input.mouse_moved = true;
    platform->input.mouse_x = x;
    platform->input.mouse_y = y;
};

void am_platform_mouse_scroll_callback_default(am_int32 id, am_events event) {
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

void am_platform_window_size_callback_default(am_int32 id, am_uint32 width, am_uint32 height, am_events event) {
    am_window *window = am_platform_window_lookup(id);

    window->cache.width = window->info.width;
    window->cache.height = window->info.height;
    window->info.width = width;
    window->info.height = height;
    printf("Window size callback: %d %d\n", width, height);
};

void am_platform_window_motion_callback_default(am_int32 id, am_uint32 x, am_uint32 y, am_events event) {
    am_window *window = am_platform_window_lookup(id);
    window->cache.x = window->info.x;
    window->cache.y = window->info.y;
    window->info.x = x;
    window->info.y = y;
    printf("Window pos callback: %d %d\n", x, y);
};

am_int32 am_platform_window_create(am_window_info window_info) {
    am_platform *platform = am_engine_get_subsystem(platform);

    am_window *new_window = (am_window*)am_malloc(sizeof(am_window)); //Alloc to push then free since it's stored dynamically now
    if (new_window == NULL) {
        printf("[FAIL] Could not allocate memory for window!\n");
        return -1;
    };

    am_int32 ret_id = am_packed_array_add(&platform->windows, new_window);
    am_free(new_window);
    new_window = am_platform_window_lookup(ret_id);
    new_window->am_id = ret_id;

    #if defined(AM_LINUX)
    XSetWindowAttributes window_attributes;
    am_int32 attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    new_window->visual_info = glXChooseVisual(platform->display, 0, attribs);
    new_window->colormap = XCreateColormap(platform->display, window_info.parent, new_window->visual_info->visual, AllocNone);
    window_attributes.colormap = new_window->colormap;
    window_attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask | EnterWindowMask | LeaveWindowMask;
    am_uint64 window = (am_uint64)XCreateWindow(platform->display, window_info.parent, window_info.x, window_info.y, window_info.width, window_info.height, 0, new_window->visual_info->depth, InputOutput, new_window->visual_info->visual, CWColormap | CWEventMask, &window_attributes);
    if (window == BadAlloc || window == BadColor || window == BadCursor || window == BadMatch || window == BadPixmap || window == BadValue || window == BadWindow) {
        printf("[FAIL] Could not create window (ID %d)!\n", ret_id);
        return -1;
    };
    new_window->handle = window;
    
    Atom wm_delete = XInternAtom(platform->display, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(platform->display, (Window)new_window->handle, &wm_delete, 1);
    XStoreName(platform->display, (Window)new_window->handle, window_info.title);
    XMapWindow(platform->display, (Window)new_window->handle);

    #else
	DWORD dwExStyle = WS_EX_LEFT; // 0
    DWORD dwStyle = WS_OVERLAPPED; // 0
    if (window_info.parent == AM_WINDOW_ROOT_PARENT) {
	    dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    };

    RECT window_rect = {
        .left = 0,
        .right = window_info.width,
        .top = 0,
        .bottom = window_info.height
    };
    AdjustWindowRectEx(&window_rect, dwStyle, false, dwExStyle);
    am_int32 rect_width = window_rect.right - window_rect.left;
    am_int32 rect_height = window_rect.bottom - window_rect.top;

    new_window->handle = (am_uint64)CreateWindowEx(dwExStyle, window_info.parent == AM_WINDOW_ROOT_PARENT ? AM_ROOT_WIN_CLASS:AM_CHILD_WIN_CLASS, window_info.title, dwStyle, window_info.x, window_info.y, rect_width, rect_height, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (new_window->handle == 0) {
        printf("[FAIL] Could not create window!\n");
        return -1;
    };
    if ((window_info.parent != AM_WINDOW_ROOT_PARENT)) {
        SetParent((HWND)new_window->handle, (HWND)window_info.parent);
        SetWindowLong((HWND)new_window->handle, GWL_STYLE, 0);
    };
    ShowWindow((HWND)new_window->handle, 1);
    #endif

    new_window->info.x = window_info.x;
    new_window->info.y = window_info.y;

    new_window->info.is_fullscreen = false;
    new_window->info.parent = window_info.parent;
    am_platform_window_fullscreen(new_window->am_id, window_info.is_fullscreen);
    new_window->info = window_info;


    //REVIEW: Once main window is deleted, cannot create new ones?
    am_window *main_window = am_platform_window_lookup(0);
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

    //TODO: VSYNC REMOVE COMMENT
    //amgl_vsync(new_window, am_engine_get_instance()->info.vsync_enabled);
    //TODO: REVIEW THIS
    #if defined(AM_LINUX)
        glXMakeCurrent(platform->display, 0, 0);
    #else
        wglMakeCurrent(0,0);
    #endif
    return ret_id;
};

am_window *am_platform_window_lookup(am_int32 id) {
    am_platform *platform = am_engine_get_subsystem(platform);
    if (am_packed_array_has(&platform->windows, id)) 
        return am_packed_array_lookup(&platform->windows, id, am_window);
    return NULL;
};

am_window *am_platform_window_lookup_by_handle(am_uint64 handle) {
    am_platform *platform = am_engine_get_subsystem(platform);
    for (am_int32 i = 0; i < platform->windows.num_elements; i++)
        if (am_packed_array_get_ptr(&platform->windows, i, am_window)->handle == handle) 
            return am_packed_array_get_ptr(&platform->windows, i, am_window);
    return NULL;
};

am_uint32 am_platform_window_index_lookup(am_int32 id) {
    am_platform *platform = am_engine_get_subsystem(platform);
    if (am_packed_array_has(&platform->windows, id)) 
        return am_packed_array_get_idx(&platform->windows, id, am_uint32);
    return 0xFFFFFFFF;
};

am_window *am_platform_window_retrieve(am_uint32 index) {
    am_platform *platform = am_engine_get_subsystem(platform);
    return am_packed_array_get_ptr(&platform->windows, index, am_window);
};

am_bool am_platform_window_exists(am_int32 id) {
    am_platform *platform = am_engine_get_subsystem(platform);
    return am_packed_array_has(&platform->windows, id);
};

void am_platform_window_resize(am_int32 id, am_uint32 width, am_uint32 height) {
    am_window *window = am_platform_window_lookup(id);
    window->cache.width = window->info.width;
    window->cache.height = window->info.height;
    #if defined(AM_LINUX)
    am_platform *platform = am_engine_get_subsystem(platform);
    XResizeWindow(platform->display, window->handle, width, height);
    #else
    RECT rect = {
        .left = 0,
        .top = 0,
        .bottom = height,
        .right = width
    };
    if (window->info.parent == AM_WINDOW_ROOT_PARENT) 
        AdjustWindowRectEx(&rect, WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME, false, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    else
        AdjustWindowRectEx(&rect, 0, false, 0);

    SetWindowPos((HWND)window->handle, 0, 0, 0 , rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);
    #endif
};

void am_platform_window_move(am_int32 id, am_uint32 x, am_uint32 y) {
    am_window *window = am_platform_window_lookup(id);

    window->cache.x = window->info.x;
    window->cache.y = window->info.y;

    #if defined(AM_LINUX)
    am_platform *platform = am_engine_get_subsystem(platform);
    XMoveWindow(platform->display, window->handle, x, y);
    #else
    RECT rect = {
        .left = x,
        .top = y,
        .bottom = window->info.height,
        .right = window->info.width
    };
    if (window->info.parent == AM_WINDOW_ROOT_PARENT) 
        AdjustWindowRectEx(&rect, WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME, false, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    else
        AdjustWindowRectEx(&rect, 0, false, 0);
    SetWindowPos((HWND)window->handle, 0, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_FRAMECHANGED);
    #endif  
};

//REVIEW: Child windows could go "fullscreen" by taking the parent's client dimensions
void am_platform_window_fullscreen(am_int32 id, am_bool state) {
    am_window *window = am_platform_window_lookup(id);
    printf("at fs %d\n", window->info.width);
    if (window->info.is_fullscreen == state || window->info.parent != AM_WINDOW_ROOT_PARENT) return;

    //From here the state has changed and it is a main window
    window->cache.is_fullscreen = window->info.is_fullscreen;
    window->info.is_fullscreen = state;
    am_window_info temp_info = window->info;
    //REVIEW: Currently not needed for linux
    #if defined(AM_WINDOWS)
    am_window_info temp_cache = window->cache;
    #endif

    #if defined(AM_LINUX)
    am_platform *platform = am_engine_get_subsystem(platform);
    Atom wm_state = XInternAtom(platform->display, "_NET_WM_STATE", false);
    Atom wm_fs = XInternAtom(platform->display, "_NET_WM_STATE_FULLSCREEN", false);
    XEvent xevent = {0};
    xevent.type = ClientMessage;
    xevent.xclient.window = window->handle;
    xevent.xclient.message_type = wm_state;
    xevent.xclient.format = 32;
    xevent.xclient.data.l[0] = state ? 1:0;
    xevent.xclient.data.l[1] = wm_fs;
    xevent.xclient.data.l[3] = 0l;
    XSendEvent(platform->display, AM_WINDOW_ROOT_PARENT, false, SubstructureRedirectMask | SubstructureNotifyMask, &xevent);
    XFlush(platform->display);
    XWindowAttributes window_attribs = {0};
    XGetWindowAttributes(platform->display, window->handle, &window_attribs);
    printf("Fullscreen toggle\n Pos: %d %d\n Size: %d %d\n Fullscreen toggle end\n\n", window_attribs.x, window_attribs.y, window_attribs.width, window_attribs.height);
    memcpy(&window->cache, &temp_info, sizeof(am_window_info));


    #else
    DWORD dw_style = GetWindowLong((HWND)window->handle, GWL_STYLE);
    if (window->info.is_fullscreen) {
        printf("Going fullscreen\n");
        MONITORINFO monitor_info = {sizeof(monitor_info)};
        GetMonitorInfo(MonitorFromWindow((HWND)window->handle, MONITOR_DEFAULTTONEAREST), &monitor_info);
        SetWindowLong((HWND)window->handle, GWL_STYLE, dw_style & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos((HWND)window->handle, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top, monitor_info.rcMonitor.right - monitor_info.rcMonitor.left, monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        memcpy(&window->cache, &temp_info, sizeof(am_window_info));
    } else {
        printf("Going not fullscreen\n");
        SetWindowLong((HWND)window->handle, GWL_STYLE, dw_style | WS_OVERLAPPEDWINDOW);
        am_platform_window_resize(id, temp_cache.width, temp_cache.height);
        am_platform_window_move(id, temp_cache.x, temp_cache.y);
        
    };
    #endif

};

//TODO: Uncomment glMake/Destroy
void am_platform_window_terminate(am_int32 id) {
    #if defined(AM_LINUX)
    am_platform *platform = am_engine_get_subsystem(platform);
    am_window *window = am_platform_window_lookup(id);
    XUnmapWindow(platform->display, window->handle);
    XFreeColormap(platform->display, window->colormap);
    XFree(window->visual_info);
    glXMakeCurrent(platform->display, window->handle, NULL);
    glXDestroyContext(platform->display, window->context);
    XDestroyWindow(platform->display, window->handle);
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
    usleep(ms*1000.0f);
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


am_int32 amgl_shader_create(amgl_shader_info info) {
    am_uint32 main_shader = glCreateProgram();
    //REVIEW: Is this a good option or would malloc be better?
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
            printf("[FAIL] Shader compilation failed for index %u:\n%s\n", i, err_log);
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
        printf("[FAIL] Failed to link program: \n%s\n", err_log);
        glDeleteProgram(main_shader);
        return -1;
    };

    for (am_int32 i = 0; i < info.num_sources; i++) { 
        glDetachShader(main_shader, shader_list[i]);
        glDeleteShader(shader_list[i]);
    };

    am_engine *engine = am_engine_get_instance();
    amgl_shader *ret = (amgl_shader*)am_malloc(sizeof(amgl_shader));
    if (ret == NULL) {
        printf("[FAIL] Could not allocate memory for shader!\n");
        return -1;  
    };

    ret->handle = main_shader;
    ret->info = info;
    ret->am_id = engine->ctx_data.shaders.next_id;
    am_int32 ret_id = am_packed_array_add(&engine->ctx_data.shaders, ret);
    am_free(ret);
    return ret_id;
};

amgl_shader *amgl_shader_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.shaders, id)) 
        return am_packed_array_lookup(&engine->ctx_data.shaders, id, amgl_shader);
    return NULL;
};

am_uint32 amgl_shader_index_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.shaders, id))
        return am_packed_array_get_idx(&engine->ctx_data.shaders, id, am_uint32);
    return 0xFFFFFFFF;
};

amgl_shader *amgl_shader_retrieve(am_uint32 index) {
    am_engine *engine = am_engine_get_instance();
    return am_packed_array_get_ptr(&engine->ctx_data.shaders, index, amgl_shader);
};

void amgl_shader_destroy(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    amgl_shader *shader = amgl_shader_lookup(id);
    glDeleteProgram(shader->handle);
    am_packed_array_erase(&engine->ctx_data.shaders, id);
};

am_int32 amgl_vertex_buffer_create(amgl_vertex_buffer_info info) {
    am_engine *engine = am_engine_get_instance();

    amgl_vertex_buffer *v_buffer = (amgl_vertex_buffer*)am_malloc(sizeof(amgl_vertex_buffer));
    if (v_buffer == NULL) {
        printf("[FAIL] Could not allocate memory for vertex buffer!\n");
        return -1;     
    };

    am_int32 usage = 0;
    switch (info.usage) {
        case AMGL_BUFFER_USAGE_STATIC: usage = GL_STATIC_DRAW; break;
        case AMGL_BUFFER_USAGE_STREAM: usage = GL_STREAM_DRAW; break;
        case AMGL_BUFFER_USAGE_DYNAMIC: usage = GL_DYNAMIC_DRAW; break;
    };

    glGenBuffers(1, &v_buffer->handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer->handle);
    glBufferData(GL_ARRAY_BUFFER, info.size, info.data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    v_buffer->info = info;
    v_buffer->am_id = engine->ctx_data.vertex_buffers.next_id;
    am_int32 ret_id = am_packed_array_add(&engine->ctx_data.vertex_buffers, v_buffer);
    am_free(v_buffer);
    return ret_id;
};

//TODO: WIll implement further down the line
/*
void amgl_vertex_buffer_update(am_int32 id, amgl_vertex_buffer_update_info update) {
    amgl_vertex_buffer *vbuffer = amgl_vertex_buffer_lookup(id);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer->handle);
    switch (update.update_type) {
        case AMGL_BUFFER_UPDATE_SUBDATA: {
            glBufferSubData(GL_ARRAY_BUFFER, update.offset, update.info.size, update.info.data);
            break;
        };
        case AMGL_BUFFER_UPDATE_RECREATE: {
            glBufferData(GL_ARRAY_BUFFER, update.info.size, update.info.data, update.info.usage);
            break;
        };
    };
    vbuffer->info = update.info;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
};*/

amgl_vertex_buffer *amgl_vertex_buffer_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.vertex_buffers, id)) 
        return am_packed_array_lookup(&engine->ctx_data.vertex_buffers, id, amgl_vertex_buffer);
    return NULL;
};

am_uint32 amgl_vertex_buffer_index_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.vertex_buffers, id))
        return am_packed_array_get_idx(&engine->ctx_data.vertex_buffers, id, am_uint32);
    return 0xFFFFFFFF;
}

amgl_vertex_buffer *amgl_vertex_buffer_retrieve(am_uint32 index) {
    am_engine *engine = am_engine_get_instance();
    return am_packed_array_get_ptr(&engine->ctx_data.vertex_buffers, index, amgl_vertex_buffer);
};

void amgl_vertex_buffer_destroy(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    amgl_vertex_buffer *vbuffer = amgl_vertex_buffer_lookup(id);
    glDeleteBuffers(1, &vbuffer->handle);
    am_packed_array_erase(&engine->ctx_data.vertex_buffers, id);
};

am_int32 amgl_index_buffer_create(amgl_index_buffer_info info) {
    am_engine *engine = am_engine_get_instance();
    
    amgl_index_buffer *index_bfr = (amgl_index_buffer*)malloc(sizeof(amgl_index_buffer));
    if (index_bfr == NULL) {
        printf("[FAIL] Could not allocate memory for index buffer!\n");
        return -1;
    };

    glGenBuffers(1, &index_bfr->handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_bfr->handle);
    switch (info.usage) {
        case AMGL_BUFFER_USAGE_STATIC: {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, info.size, info.data, GL_STATIC_DRAW);
            break;
        };
        case AMGL_BUFFER_USAGE_STREAM: {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, info.size, info.data, GL_STREAM_DRAW);
            break;
        };
        case AMGL_BUFFER_USAGE_DYNAMIC: {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, info.size, info.data, GL_DYNAMIC_DRAW);
            break;
        };
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    index_bfr->info = info;
    index_bfr->am_id = engine->ctx_data.index_buffers.next_id;
    am_int32 ret_id = am_packed_array_add(&engine->ctx_data.index_buffers, index_bfr);
    am_free(index_bfr);
    return ret_id;
};


amgl_index_buffer *amgl_index_buffer_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.index_buffers, id)) 
        return am_packed_array_lookup(&engine->ctx_data.index_buffers, id, amgl_index_buffer);
    return NULL;
};

am_uint32 amgl_index_buffer_index_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.index_buffers, id))
        return am_packed_array_get_idx(&engine->ctx_data.index_buffers, id, am_uint32);
    return 0xFFFFFFFF;
};

amgl_index_buffer *amgl_index_buffer_retrieve(am_uint32 index) {
    am_engine *engine = am_engine_get_instance();
    return am_packed_array_get_ptr(&engine->ctx_data.index_buffers, index, amgl_index_buffer);
};

void amgl_index_buffer_destroy(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    amgl_index_buffer *index_buffer = amgl_index_buffer_lookup(id);
    glDeleteBuffers(1, &index_buffer->handle);
    am_packed_array_erase(&engine->ctx_data.index_buffers, id);
};


am_int32 amgl_uniform_create(amgl_uniform_info info) {
    am_engine *engine = am_engine_get_instance();

    amgl_uniform *uniform = (amgl_uniform*)malloc(sizeof(amgl_uniform));
    if (uniform == NULL) {
        printf("[FAIL] Could not allocate memory for uniform!\n");
        return -1;
    };
    uniform->info = info;
    uniform->am_id = engine->ctx_data.uniforms.next_id;
    am_int32 ret_id = am_packed_array_add(&engine->ctx_data.uniforms, uniform);
    am_free(uniform);
    return ret_id;
};

amgl_uniform *amgl_uniform_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.uniforms, id)) 
        return am_packed_array_lookup(&engine->ctx_data.uniforms, id, amgl_uniform);
    return NULL;
};

am_uint32 amgl_uniform_index_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.uniforms, id))
        return am_packed_array_get_idx(&engine->ctx_data.uniforms, id, am_uint32);
    return 0xFFFFFFFF;
};

amgl_uniform *amgl_uniform_retrieve(am_uint32 index) {
    am_engine *engine = am_engine_get_instance();
    return am_packed_array_get_ptr(&engine->ctx_data.uniforms, index, amgl_uniform);
};

/*
void amgl_uniform_update(am_int32 id, amgl_uniform_info info) {
    amgl_uniform *uniform = amgl_uniform_lookup(id);
    uniform->info = info;
    uniform->location = glGetUniformLocation(uniform->info.shader_id, uniform->info.name);
    glUniform1fv(uniform->location, 1, (float*)uniform->info.data);
};
*/

void amgl_uniform_destroy(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    am_packed_array_erase(&engine->ctx_data.index_buffers, id);
};

/*
void amgl_uniform_bind(amgl_uniform *uniforms, am_uint32 num) {
    for (am_int32 i = 0; i < num; i++) {
        amgl_uniform uniform = uniforms[i];
        uniform.location = glGetUniformLocation(uniform.info.shader_id, uniform.info.name);

        switch (uniform.info.type) {
            case AMGL_UNIFORM_TYPE_FLOAT: {
                
                break;
            };
            case AMGL_UNIFORM_TYPE_INT: {
                glUniform1i(uniform.location, *(am_int32*)uniform.info.data);
                break;
            };
            case AMGL_UNIFORM_TYPE_SAMPLER2D: {
                amgl_texture *texture = amgl_texture_retrieve(*(am_uint32*)uniform.info.data);
                glActiveTexture(GL_TEXTURE0 + uniform.info.texture_slot);
                glBindTexture(GL_TEXTURE_2D, texture->handle);
                glUniform1i(uniform.location, uniform.info.texture_slot);
                break; 
            };
        };
    }; 
};
*/


am_int32 amgl_texture_create(amgl_texture_info info) {
    am_engine *engine = am_engine_get_instance();

    amgl_texture *texture = (amgl_texture*)am_malloc(sizeof(amgl_texture));
    if (texture == NULL) {
        printf("[FAIL] Could not allocate memory for texture!\n");
        return -1;
    };

    glGenTextures(1, &texture->handle);
    glBindTexture(GL_TEXTURE_2D, texture->handle);
    
    switch (info.format) {
        case GL_ALPHA: glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, info.width, info.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, info.data); break;
        case GL_RED: glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, info.width, info.height, 0, GL_RED, GL_UNSIGNED_BYTE, info.data); break;
        case GL_RGB8: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, info.width, info.height, 0, GL_RGB8, GL_UNSIGNED_BYTE, info.data); break;
        case GL_RGBA8: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, info.width, info.height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, info.data); break;
        case GL_RGBA16F: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, info.width, info.height, 0, GL_RGBA16, GL_FLOAT, info.data); break;
        case GL_RGBA: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, info.data); break;
        case GL_RGBA32F: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, info.width, info.height, 0, GL_RGBA32F, GL_FLOAT, info.data); break;
        case GL_DEPTH_COMPONENT: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, info.width, info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, info.data); break;
        case GL_DEPTH_COMPONENT16: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, info.width, info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, info.data); break;
        case GL_DEPTH_COMPONENT24: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, info.width, info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, info.data); break;
        case GL_DEPTH_COMPONENT32F: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, info.width, info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, info.data); break;
        case GL_DEPTH24_STENCIL8: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, info.width, info.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, info.data); break;
        case GL_DEPTH32F_STENCIL8: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, info.width, info.height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, info.data); break;
        default: break;
    }

    info.min_filter = info.min_filter == GL_NEAREST ? GL_NEAREST:GL_LINEAR;
    info.mag_filter = info.mag_filter == GL_NEAREST ? GL_NEAREST:GL_LINEAR;

    if (info.mip_num) {
        if (info.min_filter == GL_NEAREST) 
            info.min_filter = info.mip_filter == GL_NEAREST_MIPMAP_NEAREST ? GL_NEAREST_MIPMAP_NEAREST:GL_NEAREST_MIPMAP_LINEAR;
        else
            info.min_filter = info.mip_filter == GL_LINEAR_MIPMAP_NEAREST ? GL_LINEAR_MIPMAP_NEAREST:GL_LINEAR_MIPMAP_LINEAR;     

        glGenerateMipmap(GL_TEXTURE_2D);
    };

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, info.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, info.mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, info.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, info.wrap_t);

    //HACK
    //TODO: Research if this is needed for every texture, might not be needed if using shaders as far as I understand
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    texture->info = info;
    texture->am_id = engine->ctx_data.textures.next_id;
    am_int32 ret_id = am_packed_array_add(&engine->ctx_data.textures, texture);
    am_free(texture);
    return ret_id;
};

/*
void amgl_texture_update(am_int32 id, amgl_texture_info info, amgl_texture_update_type type) {
    amgl_texture *texture = amgl_texture_lookup(id);
    glBindTexture(GL_TEXTURE_2D, texture->handle);
    switch (type) {
        case AMGL_TEXTURE_UPDATE_SUBDATA: {
            //TODO: Have to update mipmaps as well, will implement later
            break;
        };
        case AMGL_TEXTURE_UPDATE_RECREATE: {
            am_engine *engine = am_engine_get_instance();
            am_int32 old_id = texture->am_id;
            amgl_texture_destroy(id);
            am_int32 new_tex = amgl_texture_create(info);
            amgl_texture *temp = amgl_texture_lookup(new_tex);
            temp->am_id = old_id;
            
            //am_engine *engine = am_engine_get_instance();
            //glDeleteTextures(1, &texture->handle);
            //am_int32 temp_id = amgl_texture_create(info);
            //amgl_texture *temp = amgl_texture_lookup(temp_id);
            //memmove((void*)texture, (void*)temp, sizeof(amgl_texture));
            //am_dyn_array_erase(&engine->ctx_data.textures, amgl_texture_index_lookup(id), 1);
            //texture->am_id = id;
            //texture->info = info;
            
            break;
        };
    };
    
    glBindTexture(GL_TEXTURE_2D, 0);
};
*/

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
    info->data = (void*)stbi_load_from_memory((const stbi_uc*)memory, size, (am_int32*)&info->width, (am_int32*)&info->height, &num_comps, 4);
    if (info->data == NULL) {
        printf("[FAIL] Unable to load texture!\n"); 
        return;
    };
};

amgl_texture *amgl_texture_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.textures, id)) 
        return am_packed_array_lookup(&engine->ctx_data.textures, id, amgl_texture);
    return NULL;
};

am_uint32 amgl_texture_index_lookup(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.textures, id))
        return am_packed_array_get_idx(&engine->ctx_data.textures, id, am_uint32);
    return 0xFFFFFFFF;
};

amgl_texture *amgl_texture_retrieve(am_uint32 index){ 
    am_engine *engine = am_engine_get_instance();
    return am_packed_array_get_ptr(&engine->ctx_data.textures, index, amgl_texture);
};

void amgl_texture_destroy(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    amgl_texture *texture = amgl_texture_lookup(id);
    glDeleteTextures(1, &texture->handle);
    am_packed_array_erase(&engine->ctx_data.textures, id);
};


am_int32 amgl_frame_buffer_create(amgl_frame_buffer_info info) {
    am_engine *engine = am_engine_get_instance();

    amgl_frame_buffer *framebuffer = (amgl_frame_buffer*)am_malloc(sizeof(amgl_frame_buffer));
    if (framebuffer == NULL) {
        printf("[FAIL] Could not allocate memory for framebuffer!\n");
        return -1;
    };

    glGenFramebuffers(1, &framebuffer->handle);
    framebuffer->info = info;
    framebuffer->am_id = engine->ctx_data.frame_buffers.next_id;
    am_int32 ret_id = am_packed_array_add(&engine->ctx_data.frame_buffers, framebuffer);
    am_free(framebuffer);
    return ret_id;
};

amgl_frame_buffer *amgl_frame_buffer_lookup(am_int32 id) {
     am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.frame_buffers, id)) 
        return am_packed_array_lookup(&engine->ctx_data.frame_buffers, id, amgl_frame_buffer);
    return NULL;
};

am_uint32 amgl_frame_buffer_index_lookup(am_int32 id) {
     am_engine *engine = am_engine_get_instance();
    if (am_packed_array_has(&engine->ctx_data.frame_buffers, id))
        return am_packed_array_get_idx(&engine->ctx_data.frame_buffers, id, am_uint32);
    return 0xFFFFFFFF;
};

amgl_frame_buffer *amgl_frame_buffer_retrieve(am_int32 index) {
    am_engine *engine = am_engine_get_instance();
    return am_packed_array_get_ptr(&engine->ctx_data.frame_buffers, index, amgl_frame_buffer);
};

void amgl_frame_buffer_destroy(am_int32 id) {
    am_engine *engine = am_engine_get_instance();
    amgl_frame_buffer *framebuffer = amgl_frame_buffer_lookup(id);
    glDeleteFramebuffers(1, &framebuffer->handle);
    am_packed_array_erase(&engine->ctx_data.frame_buffers, id);
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

    //NOTE: Adding default framebuffer since it is a framebuffer
    //REVIEW: Is this a good choice?
    am_engine *engine = am_engine_get_instance();
    am_packed_array_add(&engine->ctx_data.frame_buffers, &(amgl_frame_buffer){.handle = 0});
};

//TODO: Windows impl if necessary
void amgl_terminate() {

};

void amgl_set_viewport(am_int32 x, am_int32 y, am_int32 width, am_int32 height) {
    glViewport(x, y, width, height);
};

void amgl_vsync(am_window *window, am_bool state) {
    //REVIEW: Not sure if glFlush() is needed but I read it can help
    glFlush();
    //Have to load SwapInterval here because this is called for each window on creation, so before amgl_init()
    #if defined(AM_LINUX)
    am_platform *platform = am_engine_get_subsystem(platform);
    if (glXSwapIntervalEXT == NULL) glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)amgl_get_proc_address("glXSwapIntervalEXT");
        glXSwapIntervalEXT(platform->display, window->handle, state);
    #else
        if (wglSwapIntervalEXT == NULL) wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)amgl_get_proc_address("wglSwapIntervalEXT");
        wglSwapIntervalEXT(state == true ? 1:0);
    #endif
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
    engine->info = engine_info;

    engine->platform = am_platform_create();
    am_platform_timer_create();

    am_packed_array_init(&engine->ctx_data.textures, sizeof(amgl_texture));
    am_packed_array_init(&engine->ctx_data.shaders, sizeof(amgl_shader));
    am_packed_array_init(&engine->ctx_data.vertex_buffers, sizeof(amgl_vertex_buffer));
    am_packed_array_init(&engine->ctx_data.index_buffers, sizeof(amgl_index_buffer));
    am_packed_array_init(&engine->ctx_data.frame_buffers, sizeof(amgl_frame_buffer));
    am_packed_array_init(&engine->ctx_data.uniforms, sizeof(amgl_uniform));

    am_window_info main = {
        .height = engine->info.initial_height,
        .width = engine->info.initial_width,
        .title = engine->info.initial_title,
        .x = engine->info.initial_x,
        .y = engine->info.initial_y,
        .parent = AM_WINDOW_ROOT_PARENT,
        .is_fullscreen = engine->info.initial_fullscreen
    };

    printf("b win\n");
    am_platform_window_create(main);
    printf("a win\n");
    #if defined(AM_LINUX)
    //XSetWindowBackground(am_engine_get_subsystem(platform)->display, main_wind->handle, 0x0000FF);
    #endif
    amgl_init();
    amgl_set_viewport(0,0, (am_int32)main.width, (am_int32)main.height);
    //HACK: Temporary
    engine->info.is_running = true;
};


void am_engine_terminate(){
    am_engine *engine = am_engine_get_instance();
    am_platform_terminate(am_engine_get_subsystem(platform));

    for (am_int32 i = 0; i < engine->ctx_data.textures.num_elements; i++) amgl_texture_destroy(amgl_texture_lookup(i)->am_id);
    am_packed_array_clear(&engine->ctx_data.textures);
    for (am_int32 i = 0; i < engine->ctx_data.shaders.num_elements; i++) amgl_shader_destroy(amgl_shader_lookup(i)->am_id);
    am_packed_array_clear(&engine->ctx_data.shaders);
    for (am_int32 i = 0; i < engine->ctx_data.vertex_buffers.num_elements; i++) amgl_vertex_buffer_destroy(amgl_vertex_buffer_lookup(i)->am_id);
    am_packed_array_clear(&engine->ctx_data.vertex_buffers);
    for (am_int32 i = 0; i < engine->ctx_data.index_buffers.num_elements; i++) amgl_index_buffer_destroy(amgl_index_buffer_lookup(i)->am_id);
    am_packed_array_clear(&engine->ctx_data.index_buffers);
    for (am_int32 i = 0; i < engine->ctx_data.frame_buffers.num_elements; i++) amgl_frame_buffer_destroy(amgl_frame_buffer_lookup(i)->am_id);
    am_packed_array_clear(&engine->ctx_data.frame_buffers);
    for (am_int32 i = 0; i < engine->ctx_data.uniforms.num_elements; i++) amgl_uniform_destroy(amgl_uniform_lookup(i)->am_id);
    am_packed_array_clear(&engine->ctx_data.uniforms);

    amgl_terminate();
    //HACK: Temporary
    engine->info.is_running = false;
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
        .initial_fullscreen = false,
        .initial_height = 500,
        .initial_width = 500,
        .initial_title = "Test",
        .initial_x = 50,
        .initial_y = 50,
        .vsync_enabled = false,
        .is_running = true
    });
    XSetWindowBackground(am_engine_get_subsystem(platform)->display, am_platform_window_lookup(0)->handle, 0xFF0000);

    am_int32 child_id = am_platform_window_create((am_window_info) {
        .height = 100,
        .width = 100,
        .x = 50,
        .y = 50,
        .title = "Child",
        .is_fullscreen = false,
        .parent = am_platform_window_lookup(0)->handle
    });
    XSetWindowBackground(am_engine_get_subsystem(platform)->display, am_platform_window_lookup(1)->handle, 0xFFFF00);

    glXMakeCurrent(am_engine_get_subsystem(platform)->display, am_platform_window_lookup(0)->handle, am_platform_window_lookup(0)->context);

    am_bool run = 1;
    am_bool toggle = true;


    am_int32 shader_id = amgl_shader_create((amgl_shader_info) {
        .num_sources = 2,
        .sources = (amgl_shader_source_info[]) {
            { .type = AMGL_SHADER_VERTEX, .source = am_util_read_file("test_v.glsl") },
            { .type = AMGL_SHADER_FRAGMENT, .source = am_util_read_file("test_f.glsl") }
        }
    });
    
    am_int32 v1_id = amgl_vertex_buffer_create((amgl_vertex_buffer_info) {
        .data = (float[]) {
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f
        },
        .size = 10*sizeof(float),
        .usage = AMGL_BUFFER_USAGE_STATIC,
        .layout = (amgl_vertex_buffer_layout) {
            .num_attribs = 1,
            .attributes = &(amgl_vertex_buffer_attribute) {    
                .format = AMGL_VERTEX_BUFFER_ATTRIBUTE_FLOAT,
                .offset = 0,
                .stride = sizeof(float)     
            }
        }
    });

    am_int32 i1_id = amgl_index_buffer_create((amgl_index_buffer_info) {
        .data = (int[]){ 1,2,3,2,3,4 },
        .offset = 0,
        .size = 6*sizeof(int),
        .usage = AMGL_BUFFER_USAGE_STATIC
    });

    int cost = 5;
    am_int32 u1_id = amgl_uniform_create((amgl_uniform_info) {
        .data = &cost,
        .name = "costime",
        .shader_id = amgl_shader_lookup(shader_id)->handle,
        .size = sizeof(int),
        .type = AMGL_UNIFORM_TYPE_INT
    });

    amgl_texture_info tex_info = {
        .format = GL_RGBA,
        .mag_filter = GL_LINEAR,
        .min_filter = GL_LINEAR,
        .wrap_s = GL_REPEAT,
        .wrap_t = GL_REPEAT
    };
    amgl_texture_load_from_file("teo.png", &tex_info, true);
    am_int32 tex1_id = amgl_texture_create(tex_info);


    printf("Glerror %d\n", glGetError());
    while (run) {
        am_platform_update(am_engine_get_subsystem(platform));
    };
    return 0;
};

/*//REVIEW: for i := 1 to vertex_buffer_count glDrawElements; Start simple and make it more efficient if need be later on
//REVIEW: ONE DRAW CALL PER BUFFER*/
