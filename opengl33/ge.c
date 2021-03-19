#ifdef _WIN32
    #define WINDOWS
#else
    #define LINUX
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <GL/gl.h>


#ifdef LINUX
    #include <X11/Xatom.h>
    #include <X11/Xlib.h>
    #include <GL/glx.h>
    #include <pthread.h>
#else
    #include <windows.h>
    #include <GL/glext.h>
#endif



#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"


#define NO_MOUSE_BUTTONS 4
#define NO_KEYBOARD_BUTTONS 256


typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int  int32;
typedef unsigned int  uint32;
typedef signed long int longint;
typedef enum {FAIL = 0, OK = 1, NO_FILE = -1} return_code;
typedef enum {false, true} bool;

#ifdef LINUX
    typedef GLXContext GLDeviceContext;
    typedef GLXContext GLRenderContext;
    typedef int(GLSwapInterval)(Display* dpy, GLXDrawable drawable, int interval);
#else   
    typedef HDC GLDeviceContext;
    typedef HGLRC GLRenderContext;
    typedef bool(WINAPI GLSwapInterval) (int interval);
#endif


#ifdef LINUX
    #define _CALL *
    
#else
    #define _CALL __stdcall*
#endif

#ifdef LINUX
    #define getProcAddr(str) glXGetProcAddress((str))
    #define _THR void *
#else
    #define getProcAddr(str) wglGetProcAddress((str))
    #define _THR DWORD WINAPI
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
typedef void (_CALL PFNGLACTIVETEXTUREPROC) (GLenum texture); 


static GLSwapInterval *glSwapIntervalEXT;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;;
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
#ifdef WINDOWS 
    PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
#endif

typedef struct v2i {
    int32 X;
    int32 Y;
} v2i;


typedef struct PixelT {
    union {
        uint32 rgba;
        struct {
            uint8 r; uint8 g; uint8 b; uint8 a;
        };
    };
} PixelT;

typedef struct ImageT {
    int32 width;
    int32 height;
    PixelT *data;
} ImageT;

typedef struct ArrayT {
    uint8 *data;
    uint32 length;
    uint32 allocLength;
    uint32 elementSize;
} ArrayT;

typedef struct TextureT {
    uint32 handle;
    ImageT *image;
    uint32 width;
    uint32 height;
} TextureT;

typedef struct AtlasT {
    ImageT *image;
    uint32 subWidth;
    uint32 subHeight;
} AtlasT;

/*
typedef struct LayerT { 
    uint32 glid;
    uint32 index;
    v2i offset;
    bool visible;
    bool update;
    ImageT *sprite;
} LayerT;*/

typedef struct ButtonState {
    bool pressed;
    bool released;
    bool held;
} ButtonState;

typedef enum MouseMap {
    LClick = 1, MClick = 2, RClick = 3, ScrollUp = 4, ScrollDown = 5
} MouseMap;

typedef struct VBO {
    uint32 handle;
    int32 type;
    bool dynamic;
} VBO;

typedef struct VAO {
    uint32 handle;
} VAO;

typedef struct input {
    ButtonState keyboard[256];
    ButtonState mouse[4];
    bool currentKeyboardState[256];
    bool previousKeyboardState[256];
    bool currentMouseState[4];
    bool previousMouseState[4];
    int32 wheelDelta;
    v2i mousePosition;
} Input;


Input input = {false};
int32 screenScale = 1;
v2i screenSize = {0,0};
v2i viewportSize = {0,0};
v2i viewportPosition = {0,0};
v2i windowSize = {0,0};
v2i windowPosition = {0,0};
bool fullscreenEnabled = false;
bool vsyncEnaled = false;
bool appRunning = true;
longint window;
GLDeviceContext glDeviceContext;
GLRenderContext glRenderContext;
uint32 mainProgram = 0;

ArrayT programs;
ArrayT VAOs;
ArrayT VBOs;

GLfloat vertices[] = {
   0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.0f, 0.0f, 0.0f
};

unsigned int indices[] = { 0, 1, 2 };

float pixels[] = {
  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
};

VAO vao;
VBO vbo;
VBO idx;
GLuint tex;



void CreateWindowP(uint32 windowPositionX, uint32 windowPositionY, uint32 windowWidth, uint32 windowHeight, bool fullscreenEnabled);
void EventHandler(); //Linux event handler
void StartEventLoop(); //Windows event handler
return_code SetWindowTitle(char *title);

void ArrayInit(ArrayT *array, uint32 elementSize);
bool ArrayResize(ArrayT *array, uint32 newsize);
bool ArrayPush(ArrayT *array, void *data, uint32 length);
bool ArrayInsert(ArrayT *array, void *data, uint32 length, uint32 index);
bool ArrayPop(ArrayT *array, uint32 length);
bool ArrayRemove(ArrayT *array, uint32 index, uint32 length);
void ArrayCleanup(ArrayT *array);

#define arrayDataLength(array, length) ((array)->elementSize * (length))
#define arrayDataPos(array, index) ((array)->data + arrayDataLength((array), (index)))
#define arrayRetrieve(array, type, index) (((type*) (void*) (array)->data)[(index)])

void glInitialize();
char* CopyShaderToBuffer(char* path);
void FreeAllocatedMemory(void* ptr);
uint32 ShaderCompile(char* path, uint32 type);
void ShaderDelete(uint32 id);
uint32 ProgramCompile(char* vertexShaderSource, char* fragmentShaderSource);
void ProgramDelete(uint32 program);
void VBOCreate(VBO *vbo, int32 type, bool dynamic);
void VBODelete(VBO *vbo);
void VBOBind(VBO *vbo);
void VBOBuffer(VBO *vbo, void *data, size_t offset, size_t count);
void VAOCreate(VAO *vao);
void VAODelete(VAO *vao);
void VAOBind(VAO *vao);
void VAOAttribs(VAO *vao, VBO *vbo, uint32 index, int32 size, GLenum type, GLsizei stride, size_t offset);
void BufferClear(bool colorBuffer, bool depthBuffer);

void WindowSizeUpdate(uint32 width, uint32 height);
void ViewportSizeUpdate();

return_code EventUserInit();
return_code EventUserUpdate();
void EventUserTerminate();

PixelT Pixel(uint8 red, uint8 green, uint8 blue, uint8 alpha);
bool ImageCreate(ImageT *image, uint32 width, uint32 height, PixelT *data);
bool ImageLoad(ImageT *image, char *path);
void ImageReset(ImageT *image);
void ImageDelete(ImageT *image);

//TODO:Implement
bool TextureCreate(TextureT *texture, uint32 width, uint32 height, ImageT *source);
bool TextureLoad(TextureT *texture, char *path);
void TextureBind(TextureT *texture);
void TextureDelete(TextureT *texture);
bool AtlasCreate(AtlasT *atlas, uint32 subWidth, uint32 subHeight, ImageT *image);
bool AtlasLoad(AtlasT *atlas, char *path);
//AtlasRetrieve(AtlasT *atlas, uint32 row, uint32 column); Think of a good solution
void AtlasDestroy(AtlasT *atlas);
//TODO:END

void StartEngine(uint32 width, uint32 height, uint32 scale, bool vsync, bool fullscreen); 

_THR RenderThread();
void FrameRender();
void TerminateCleanup();

/*
void Construct(uint32 width, uint32 height, uint32 scale, bool vsync, bool fullscreen);
void Start();
void *Thread();
void FrameRender();
void TerminateCleanup();
void BufferClear(bool colorBuffer, bool depthBuffer);


PixelT Pixel(uint8 red, uint8 green, uint8 blue, uint8 alpha);
PixelT PixelRetrieve(uint32 x, uint32 y,  ImageT *sprite); 
return_code PixelDraw(PixelT pixel, uint32 x, uint32 y, ImageT *sprite); 
ImageT *SpriteCreate(uint32 width, uint32 height, PixelT *buffer);
ImageT *SpriteLoadFromImage(char path[]);
void SpriteClear(ImageT *sprite, PixelT pixel);
void SpriteDelete(ImageT *sprite);
void SpriteDraw(ImageT *sprite, uint32 x, uint32 y, uint32 scale, ImageT *target);
void SpriteDrawPartial(ImageT *sprite, uint32 left, uint32 top, uint32 right, uint32 bottom, uint32 scale,  ImageT *target, uint32 x, uint32 y);
LayerT *LayerCreate(bool visible, bool update);          
void LayerDelete(uint32 id);


return_code UserInitEvent();
return_code UserUpdateEvent();
void UserTerminateEvent();


void ViewportSizeUpdate();
void WindowSizeUpdate(uint32 x, uint32 y);
void LayerOffsetUpdate(LayerT *layer, uint32 x, uint32 y);

*/

//-------------------------------------------------------------//
//                           START UTIL                        //
//-------------------------------------------------------------//

//#define MAX(x,y) (x>y?x:y)
//#define MIN(x,y) (x>y?y:x)
//#define ABS(x) (x>0?x:-(x))

//-------------------------------------------------------------//
//                            END UTIL                         //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                        START PLATFORM                       //
//-------------------------------------------------------------//

#if defined(LINUX)

Display* display;
Window rootWindow;
XVisualInfo* visualInfo;
Colormap colourmap;

typedef enum KeyboardMap {
    key_F1 = (uint8)XK_F1, key_F2 = (uint8)XK_F2, key_F3 = (uint8)XK_F3, key_F4 = (uint8)XK_F4, key_F5 = (uint8)XK_F5, key_F6 = (uint8)XK_F6, key_F7 = (uint8)XK_F7, key_F8 = (uint8)XK_F8, key_F9 = (uint8)XK_F9, key_F10 = (uint8)XK_F10, key_F11 = (uint8)XK_F11, key_F12 = (uint8)XK_F12,
    key_A = 0x61, key_B = 0x62, key_C = 0x63, key_D = 0x64, key_E = 0x65, key_F = 0x66, key_G = 0x67, key_H = 0x68, key_I = 0x69, key_J = 0x6a, key_K = 0x6b, key_L = 0x6c, key_M = 0x6d, key_N = 0x6e, key_O = 0x6f, key_P = 0x70, key_Q = 0x71, key_R = 0x72, key_S = 0x73, key_T = 0x74, key_U = 0x75, key_V = 0x76, key_W = 0x77, key_X = 0x78, key_Y = 0x79, key_Z = 0x7a,
    key_0 = 0x30, key_1 = 0x31, key_2 = 0x32, key_3 = 0x33, key_4 = 0x34, key_5 = 0x35, key_6 = 0x36, key_7 = 0x37, key_8 = 0x38, key_9 = 0x39,
    key_Esc = (uint8)XK_Escape, key_Tab = (uint8)XK_Tab, key_CapsLock = (uint8)XK_Caps_Lock, key_ShiftL = (uint8)XK_Shift_L, key_ControlL = (uint8)XK_Control_L, key_AltL = (uint8)XK_Alt_L,
    key_Backspace = (uint8)XK_BackSpace, key_Backslash = (uint8)XK_backslash, key_Enter = (uint8)XK_Return, key_ShiftR = (uint8)XK_Shift_R, key_AltR = (uint8)XK_Alt_R, key_ControlR = (uint8)XK_Control_R,
    key_Minus = (uint8)XK_minus, key_Equals = (uint8)XK_equal, key_LeftBracket = (uint8)XK_bracketleft, key_RightBracket = (uint8)XK_bracketright, key_Apostrophe = (uint8)XK_apostrophe, key_Semicolon = (uint8)XK_semicolon,
    key_Comma = (uint8)XK_comma, key_Dot = 0x2e, key_Space = (uint8)XK_space, key_Slash = (uint8)XK_slash
} KeyboardMap;

void CreateWindowP(uint32 windowPositionX, uint32 windowPositionY, uint32 windowWidth, uint32 windowHeight, bool fullscreenEnabled) {
    XInitThreads();
    display = XOpenDisplay(NULL);
    rootWindow = DefaultRootWindow(display);
    XSetWindowAttributes windowAttributes;
    GLint attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    visualInfo = glXChooseVisual(display, 0, attribs);
    colourmap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);
    windowAttributes.colormap = colourmap;
    windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask;
    window = (longint)XCreateWindow(display, rootWindow, windowPositionX, windowPositionY, windowWidth, windowHeight, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &windowAttributes);
    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(display, (Window)window, &wmDelete, 1);
    XMapWindow(display, (Window)window);
    XStoreName(display, (Window)window, "Engine");

    if (fullscreenEnabled) {
        Atom wmState = XInternAtom(display, "_NET_WM_STATE", true);
        Atom wmFs = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", true);

        XChangeProperty(display, window, wmState, XA_ATOM, 32, PropModeReplace, (unsigned char*)&wmFs, 1);
    };
};

void StartEventLoop() {return;};

void EventHandler() {
    XEvent event;
    while (XPending(display)) {
        XNextEvent(display, &event);
        switch(event.type) {
            case Expose: {
                XWindowAttributes windowAttribs;
                XGetWindowAttributes(display, (Window)window, &windowAttribs);
                WindowSizeUpdate(windowAttribs.width, windowAttribs.height);
                break;
            };
            case KeyPress: {
                KeySym keysim = XLookupKeysym(&event.xkey, 0);
                XKeyEvent *e =(XKeyEvent*)&event;
                XLookupString(e, NULL, 0, &keysim, NULL);
                input.currentKeyboardState[(uint8)keysim] = true;
                break;
            };
            case KeyRelease: {
                KeySym keysim = XLookupKeysym(&event.xkey, 0);
                XKeyEvent *e =(XKeyEvent*)&event;
                XLookupString(e, NULL, 0, &keysim, NULL);
                input.currentKeyboardState[(uint8)keysim] = false;
                break;
            };
            case ButtonPress: {
                if (event.xbutton.button < 4) input.currentMouseState[event.xbutton.button] = true; else if (event.xbutton.button == ScrollUp) input.wheelDelta += 120; else if (event.xbutton.button == ScrollDown) input.wheelDelta -= 120;
                break;
            };
            case ButtonRelease: {
                if (event.xbutton.button < 4) input.currentMouseState[event.xbutton.button] = false;
                break;
            };
            case MotionNotify: {
                input.mousePosition.X = event.xbutton.x;
                input.mousePosition.Y = event.xbutton.y;
                break;
            };
            case ClientMessage: {
                appRunning = false; 
                TerminateCleanup();
                break;  
            };
        };
    };
};

return_code SetWindowTitle(char* title) {
    if (XStoreName(display, (Window)window, title) < 3) return OK; else return FAIL;
};

#else
LRESULT CALLBACK EventLoop(HWND windowHandle, uint32 uMessage, WPARAM wParameter, LPARAM lParameter);

typedef enum KeyboardMap {
    key_F1 = VK_F1, key_F2 = VK_F2, key_F3 = VK_F3, key_F4 = VK_F4, key_F5 = VK_F5, key_F6 = VK_F6, key_F7 = VK_F7, key_F8 = VK_F8, key_F9 = VK_F9, key_F10 = VK_F10, key_F11 = VK_F11, key_F12 = VK_F12,
    key_A = 0x41, key_B = 0x42, key_C = 0x43, key_D = 0x44, key_E = 0x45, key_F = 0x46, key_G = 0x47, key_H = 0x48, key_I = 0x49, key_J = 0x4a, key_K = 0x4b, key_L = 0x4c, key_M = 0x4d, key_N = 0x4e, key_O = 0x4f, key_P = 0x50, key_Q = 0x51, key_R = 0x52, key_S = 0x53, key_T = 0x54, key_U = 0x55, key_V = 0x56, key_W = 0x57, key_X = 0x58, key_Y = 0x59, key_Z = 0x5a,
    key_0 = 0x30, key_1 = 0x31, key_2 = 0x32, key_3 = 0x33, key_4 = 0x34, key_5 = 0x35, key_6 = 0x36, key_7 = 0x37, key_8 = 0x38, key_9 = 0x39,
    key_Esc = VK_ESCAPE, key_Tab = VK_TAB, key_CapsLock = VK_CAPITAL, key_ShiftL = VK_LSHIFT, key_ControlL = VK_LCONTROL, key_AltL = VK_LMENU,
    key_Backspace = VK_BACK, key_Backslash = 0xdc, key_Enter = VK_RETURN, key_ShiftR = VK_RSHIFT, key_AltR = VK_RMENU, key_ControlR = VK_RCONTROL,
    key_Minus = 0xbd, key_Equals = 0xbb, key_LeftBracket = 0xdb, key_RightBracket = 0xdd, key_Apostrophe = 0xde, key_Semicolon = 0xba,
    key_Comma = 0xbc, key_Dot = 0xbe, key_Space = VK_SPACE, key_Slash = 0xbf
} KeyboardMap;

void CreateWindowP(uint32 windowPositionX, uint32 windowPositionY, uint32 windowWidth, uint32 windowHeight, bool fullscreenEnabled) {
    WNDCLASS windowClass;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpfnWndProc = EventLoop;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.lpszMenuName = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszClassName = "Engine";
	RegisterClass(&windowClass);
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;
    
	v2i vTopLeft;
	vTopLeft.X = windowPositionX;
	vTopLeft.Y = windowPositionY;
	
	if (fullscreenEnabled){
		dwExStyle = 0;
		dwStyle = WS_VISIBLE | WS_POPUP;
		HMONITOR hmon = MonitorFromWindow((HWND)window, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		if (!GetMonitorInfo(hmon, &mi)) {
            return;
        };
		windowWidth = mi.rcMonitor.right;
		windowHeight = mi.rcMonitor.bottom;
		vTopLeft.X = 0;
		vTopLeft.Y = 0;
	};
    
	RECT windowRectangle;
	windowRectangle.left = 0; windowRectangle.top = 0;
	windowRectangle.right = windowWidth; windowRectangle.bottom = windowHeight;
	AdjustWindowRectEx(&windowRectangle, dwStyle, false, dwExStyle);
	int width = windowRectangle.right - windowRectangle.left;
	int height = windowRectangle.bottom - windowRectangle.top;
    
	window = (longint)CreateWindowEx(dwExStyle, "Engine", "Engine", dwStyle, vTopLeft.X, vTopLeft.Y, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
};

void EventHandler() {return;};

void StartEventLoop() {
    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
};
//TODO:Will need to update this because of the new input struct
LRESULT CALLBACK EventLoop(HWND windowHandle, uint32 uMessage, WPARAM wParameter, LPARAM lParameter){
    switch(uMessage){
        case WM_SIZE: { WindowSizeUpdate(lParameter & 0xFFFF, (lParameter >> 16) & 0xFFFF); break; };
        case WM_MOUSEMOVE: {
            uint16 x = lParameter & 0xFFFF;
            uint16 y = (lParameter >> 16) & 0xFFFF;
            input.mousePosition.X = *(int16*)&x;
            input.mousePosition.Y = *(int16*)&y;
            break;
        };
        case WM_KEYDOWN: { input.currentKeyboardState[wParameter] = true; break; };
        case WM_KEYUP: { input.currentKeyboardState[wParameter] = false; break; };
        case WM_LBUTTONDOWN: { input.currentMouseState[LClick] = true; break; };
        case WM_LBUTTONUP: { input.currentMouseState[LClick] = false; break; };
        case WM_RBUTTONDOWN: { input.currentMouseState[RClick] = true; break; };
        case WM_RBUTTONUP: { input.currentMouseState[RClick] = false; break; };
        case WM_MBUTTONDOWN: { input.currentMouseState[MClick] = true; break; };
        case WM_MBUTTONUP: { input.currentMouseState[MClick] = false; break; };
        case WM_MOUSEWHEEL: { input.wheelDelta += GET_WHEEL_DELTA_WPARAM(wParameter); break; };
        case WM_DESTROY: { PostQuitMessage(0); break; };
        case WM_CLOSE: { appRunning = false; TerminateCleanup(); break; };
    };
    return DefWindowProc(windowHandle, uMessage, wParameter, lParameter);
};

return_code SetWindowTitle(char* title) {
    if (SetWindowText((HWND)window, title) != 0) return OK; else return FAIL;
};

#endif

//-------------------------------------------------------------//
//                          END PLATFORM                       //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                          START VECTOR                       //
//-------------------------------------------------------------//

void ArrayInit(ArrayT *array, uint32 elementSize) {
    array->data = NULL;
    array->length = 0;
    array->allocLength = 0;
    array->elementSize = elementSize;
};

bool ArrayResize(ArrayT *array, uint32 newLength) {
    if (newLength > array->allocLength) {
        void *buffer = NULL;
        buffer = realloc(array->data, (newLength + (array->allocLength >> 1u))*arrayDataLength(array, 1));
        if (buffer) {
            array->allocLength = newLength + (array->allocLength >> 1u);
            array->data = (uint8*) buffer;
        } else return false;
    };
    return true;
};

bool ArrayPush(ArrayT *array, void *data, uint32 length) {
    if (length >=0) { //Useless check, just keeping the same function type for prettiness
        ArrayResize(array, array->length + length);
        memcpy(arrayDataPos(array, array->length), data, arrayDataLength(array, length));
        array->length += length;
        return true;
    };
    return false;
};

bool ArrayInsert(ArrayT *array, void *data, uint32 length, uint32 index) {
    if (index <= array->length) {
        ArrayResize(array, array->length + length);
        memcpy(arrayDataPos(array, index+length), arrayDataPos(array, index), arrayDataLength(array, length));
        memcpy(arrayDataPos(array, index), data, arrayDataLength(array, length));
        array->length += length;
        return true;
    };
    return false;
};

bool ArrayPop(ArrayT *array, uint32 length) {
    if (length <= array->length) {
        array->length -= length;
        return true;
    };
    return false;
};

bool ArrayRemove(ArrayT *array, uint32 index, uint32 length) {
    if (index + length > array->length) return false;
    memcpy(arrayDataPos(array, index), arrayDataPos(array, index+length), arrayDataLength(array, array->length - length));
    array->length -= length;
    return true;
};

void ArrayCleanup(ArrayT *array) {
    array->length = 0;
    array->elementSize = 0;
    array->allocLength = 0;
    FreeAllocatedMemory((void*)array->data);
    array->data = NULL;
};
//-------------------------------------------------------------//
//                           END VECTOR                        //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                          START OPENGL                       //
//-------------------------------------------------------------//

void glInitialize() {
    #ifdef LINUX
        glDeviceContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
        glXMakeCurrent(display, window, glDeviceContext);
        glSwapIntervalEXT = NULL;
        glSwapIntervalEXT = (GLSwapInterval*)getProcAddr((unsigned char*)"glXSwapIntervalEXT");
        if (glSwapIntervalEXT == NULL && !vsyncEnaled) printf("Unable to disable vsync, will run at monitor's default refresh rate\n");
        if (glSwapIntervalEXT != NULL && !vsyncEnaled) glSwapIntervalEXT(display, window, 0);
    #else
        glDeviceContext = GetDC((HWND)window);
        PIXELFORMATDESCRIPTOR DesiredPixelFormat = {};
        DesiredPixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        DesiredPixelFormat.nVersion = 1;
        DesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
        DesiredPixelFormat.cColorBits = 32;
        DesiredPixelFormat.cAlphaBits = 8;
        DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
        
        int SuggestedPixelFormatIndex = ChoosePixelFormat(glDeviceContext, &DesiredPixelFormat);
        PIXELFORMATDESCRIPTOR SuggestedPixelFormat;
        DescribePixelFormat(glDeviceContext, SuggestedPixelFormatIndex, sizeof(SuggestedPixelFormat), &SuggestedPixelFormat);
        SetPixelFormat(glDeviceContext, SuggestedPixelFormatIndex, &SuggestedPixelFormat);

        glRenderContext = wglCreateContext(glDeviceContext);
        wglMakeCurrent(glDeviceContext, glRenderContext);

        glSwapIntervalEXT = NULL;
        glSwapIntervalEXT = (GLSwapInterval*)wglGetProcAddress("wglSwapIntervalEXT");
        if (glSwapIntervalEXT == NULL && !vsyncEnaled) printf("Unable to disable vsync");
        if (glSwapIntervalEXT != NULL && !vsyncEnaled) glSwapIntervalEXT(0);
    #endif
    glViewport(viewportPosition.X, viewportPosition.Y, viewportSize.X, viewportSize.Y);
    
    //TODO: GLX, needs WGL version
    glCreateShader = (PFNGLCREATESHADERPROC)getProcAddr("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)getProcAddr("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)getProcAddr("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)getProcAddr("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)getProcAddr("glGetShaderInfoLog");
    glDetachShader = (PFNGLDETACHSHADERPROC)getProcAddr("glDetachShader");
    glDeleteShader = (PFNGLDELETESHADERPROC)getProcAddr("glDeleteShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)getProcAddr("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)getProcAddr("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)getProcAddr("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)getProcAddr("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)getProcAddr("glGetProgramInfoLog");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)getProcAddr("glDeleteProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)getProcAddr("glUseProgram");
    glGenBuffers = (PFNGLGENBUFFERSPROC)getProcAddr("glGenBuffers");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)getProcAddr("glDeleteBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)getProcAddr("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)getProcAddr("glBufferData");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)getProcAddr("glGenVertexArrays");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)getProcAddr("glDeleteVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)getProcAddr("glBindVertexArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)getProcAddr("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)getProcAddr("glEnableVertexAttribArray");
    glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)getProcAddr("glGetAttribLocation");

    glUniform1i = (PFNGLUNIFORM1IPROC)getProcAddr("glUniform1i");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)getProcAddr("glGetUniformLocation");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)getProcAddr("glGenerateMipmap");
    #ifdef WINDOWS 
        glActiveTexture = (PFNGLACTIVETEXTUREPROC)getProcAddr("glActiveTexture");
    #endif

};

void FreeAllocatedMemory(void* ptr) {
    free(ptr);
};

char* CopyShaderToBuffer(char* path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        printf("Failed to open file!\n");
        return NULL;
    };
    uint32 cCount = 0;
    while (!feof(f)) {
        ++cCount;
        fgetc(f);
    };
    char *source = malloc(sizeof(char)*(cCount+1));
    if (source == NULL) {
        fclose(f);
        printf("Failed to allocate memory for the shader!\n");
        return NULL;
    };
    rewind(f);
    fread(source, 1, sizeof(char)*cCount, f);
    if (ferror(f)) {
        FreeAllocatedMemory(source);
        fclose(f);
        printf("Error reading file!\n");
        return NULL;
    };
    fclose(f);
    source[cCount] = '\0';
    return source;
};

    //---------//
    // SHADER  //
    //---------//

uint32 ShaderCompile(char* path, uint32 type) {
    char *source = CopyShaderToBuffer(path);

    uint32 shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, NULL);
    glCompileShader(shader);
    FreeAllocatedMemory((void*)source);
    int32 isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if(!isCompiled) {
        int32 length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char errorLog[length];
        glGetShaderInfoLog(shader, length, &length, errorLog);
        printf("Shader compilation failed: %s\n", errorLog);
        glDeleteShader(shader); 
        return 0;
    };
    return shader;
};

void ShaderDelete(uint32 id) {
    glDeleteShader(id);
};

    //---------//
    // PROGRAM //
    //---------//

uint32 ProgramCompile(char* vertexShaderSource, char* fragmentShaderSource) {
    uint32 vShader = ShaderCompile(vertexShaderSource, GL_VERTEX_SHADER);
    uint32 fShader = ShaderCompile(fragmentShaderSource, GL_FRAGMENT_SHADER);

    uint32 program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    glDetachShader(program, vShader);
    glDetachShader(program, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    int32 isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        int32 length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char errorLog[length];
        glGetProgramInfoLog(program, length, &length, errorLog);
        printf("Program linking failed: %s\n", errorLog);
        glDeleteProgram(program);
        return 0;
    };
    printf("Success!\n");
    return program;
};

void ProgramDelete(uint32 program) {
    glDeleteProgram(program);
};

    //---------//
    //   VBO   //
    //---------//


void VBOCreate(VBO *vbo, int32 type, bool dynamic) {
    vbo->type = type;
    vbo->dynamic = dynamic;
    glGenBuffers(1, &vbo->handle);
};

void VBODelete(VBO *vbo) {
    glDeleteBuffers(1, &vbo->handle);
};

void VBOBind(VBO *vbo) {
    glBindBuffer(vbo->type, vbo->handle);
};

void VBOBuffer(VBO *vbo, void *data, size_t offset, size_t count) {
    VBOBind(vbo);
    glBufferData(vbo->type, count-offset, data, vbo->dynamic ? GL_DYNAMIC_DRAW:GL_STATIC_DRAW);
};

    //---------//
    //   VAO   //
    //---------//

void VAOCreate(VAO *vao) {
    glGenVertexArrays(1, &vao->handle);
};

void VAODelete(VAO *vao) {
    glDeleteVertexArrays(1, &vao->handle);
};

void VAOBind(VAO *vao) {
    glBindVertexArray(vao->handle);
};

void VAOAttribs(VAO *vao, VBO *vbo, uint32 index, int32 size, GLenum type, GLsizei stride, size_t offset) {
    VAOBind(vao);
    VBOBind(vbo);
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *)offset);
    glEnableVertexAttribArray(index);
};

void BufferClear(bool colorBuffer, bool depthBuffer) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    if (colorBuffer) glClear(GL_COLOR_BUFFER_BIT);
    if (depthBuffer) glClear(GL_DEPTH_BUFFER_BIT);
};

//-------------------------------------------------------------//
//                           END OPENGL                        //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                         START UPDATES                       //
//-------------------------------------------------------------//

void WindowSizeUpdate(uint32 width, uint32 height) {
    windowSize.X = width;
    windowSize.Y = height;
    ViewportSizeUpdate();
};

void ViewportSizeUpdate() {
    float ratio = (float)screenSize.X/ (float)screenSize.Y;
    viewportSize.X = windowSize.X;
    viewportSize.Y = (uint32) ((float)viewportSize.X / ratio);
    if (viewportSize.Y > windowSize.Y) {
        viewportSize.Y = windowSize.Y;
        viewportSize.X = (uint32) ((float)viewportSize.Y * ratio); 
    } 
    viewportPosition.X = (windowSize.X - viewportSize.X) / 2;
    viewportPosition.X = (windowSize.Y - viewportSize.Y) / 2;
};

//-------------------------------------------------------------//
//                          END UPDATES                        //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                         START EVENTS                        //
//-------------------------------------------------------------//

return_code EventUserInit() {

    mainProgram = ProgramCompile("test.vs", "test.fs");
    VAOCreate(&vao);
    VAOBind(&vao);

    VBOCreate(&vbo, GL_ARRAY_BUFFER, false);
    VBOBind(&vbo);
    VBOBuffer(&vbo, vertices, 0, sizeof(vertices));

    VBOCreate(&idx, GL_ELEMENT_ARRAY_BUFFER, false);
    VBOBind(&idx);
    VBOBuffer(&idx, indices, 0, sizeof(indices));
    
    glUseProgram(mainProgram);

    VAOAttribs(&vao, &vbo, glGetAttribLocation(mainProgram, "point"), 3, GL_FLOAT, 5 * sizeof(float), 0);
    VAOAttribs(&vao, &vbo, glGetAttribLocation(mainProgram, "texcoord"), 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));


    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(mainProgram, "tex"), 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_BGR, GL_FLOAT, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    return OK;
};

return_code EventUserUpdate() {
    return OK;
};

void EventUserTerminate();

//-------------------------------------------------------------//
//                          END EVENTS                         //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                       START DATA MANIP                      //
//-------------------------------------------------------------//

PixelT Pixel(uint8 red, uint8 green, uint8 blue, uint8 alpha) {
    PixelT pixel;
    PixelT *mem = &pixel;
    mem->rgba = red | (green << 8) | (blue << 16) | (alpha << 24);
    return pixel;
};

bool ImageCreate(ImageT *image, uint32 width, uint32 height, PixelT *data) {
    image->width = width;
    image->height = height;
    void *bfr = NULL;
    bfr = malloc(sizeof(PixelT)*width*height);
    if (!bfr) return false;
    image->data = (PixelT*) bfr;
    if (data == NULL) { 
        for (uint32 i = 0; i < width*height; i++) image->data[i] = Pixel(0,100,0,255); //TODO: FOR DEBUG; REPLACE WITH 0,0,0,0
    } else {
        memcpy(image->data, data, sizeof(PixelT)*width*height);
    };
    return true;
};

bool ImageLoad(ImageT *image, char *path) {
    uint32 width, height, noc;
    unsigned char *buffer = NULL;
    buffer = stbi_load(path, &width, &height, &noc, 4);
    if (ImageCreate(image, width, height, (PixelT*)buffer)) {
        stbi_image_free(buffer);
        return true;
    };
    stbi_image_free(buffer);
    return false;
};

void ImageReset(ImageT *image) {
    for (uint32 i = 0; i < image->height*image->height; i++) image->data[i] = Pixel(0,0,0,0);
    image->width = 0;
    image->height = 0;
};

void ImageDelete(ImageT *image) {
    FreeAllocatedMemory((void*)image->data);
    image->width = 0;
    image->height = 0;
};

//-------------------------------------------------------------//
//                         END DATA MANIP                      //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                          START MAIN                         //
//-------------------------------------------------------------//

void StartEngine(uint32 width, uint32 height, uint32 scale, bool vsync, bool fullscreen) {
    screenScale  = scale;
    screenSize.X = width;
    screenSize.Y = height;
    windowSize.X = width*scale;
    windowSize.Y = height*scale;
    fullscreenEnabled = fullscreen;
    vsyncEnaled = vsync;
    appRunning = true;
    ArrayInit(&programs, sizeof(uint32));
    ArrayInit(&VAOs, sizeof(VAO));
    ArrayInit(&VBOs, sizeof(VBO));
    CreateWindowP(windowPosition.X, windowPosition.Y, windowSize.X, windowSize.Y, fullscreenEnabled);
    WindowSizeUpdate(windowSize.X, windowSize.Y);
    #ifdef STBI_INCLUDE_STB_IMAGE_H 
        stbi_set_flip_vertically_on_load(true);
    #endif
    
    #ifdef LINUX
        pthread_t tid;  
        //TODO:Possible (very likely) memory leak, read up on this
        pthread_create(&tid, NULL, &RenderThread, NULL);
        StartEventLoop();
        pthread_join(tid, NULL);
    #else
        HANDLE tid = CreateThread(NULL, 0, RenderThread, NULL, 0, NULL);
        StartEventLoop();

        if (tid != NULL) WaitForSingleObject(tid, 0xFFFFFFFF);

        //TODO:IMPLEMENT WINDOWS THREADING
    #endif 
};

_THR RenderThread() {
    glInitialize();
    if (!EventUserInit()) appRunning = false;
    while (appRunning) FrameRender();
    
    #ifdef LINUX
        //TODO:Send proper close message
        XEvent event;
        XSendEvent(display, window, false, 0, &event);
        pthread_exit(0);
    #else
        wglDeleteContext(glRenderContext);
        PostMessage((HWND)window,WM_DESTROY,0,0);
        //TODO:EXIT THREAD HERE WINDOWS
    #endif
    
};

void FrameRender() {
    EventHandler();
    BufferClear(true, false);
    glUseProgram(mainProgram);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
    #ifdef LINUX
        glXSwapBuffers(display, window);
    #else
        SwapBuffers(glDeviceContext);
    #endif
};

void TerminateCleanup() {
    //TODO:Make an optimized cleaning step for windows as well
    /*
    vectorCleanup(&programs);
    vectorCleanup(&VAOs);
    vectorCleanup(&VBOs);
    */
   ArrayCleanup(&programs);
   ArrayCleanup(&VAOs);
   ArrayCleanup(&VBOs);
   //EventUserTerminate();
    #ifdef LINUX
        XUnmapWindow(display, window);
        glXMakeCurrent(display, 0, NULL);
        XFreeColormap(display, colourmap);
        XDestroyWindow(display, window);
        glXDestroyContext(display, glDeviceContext);
        XFree(visualInfo);
        XCloseDisplay(display);
    #else
        wglDeleteContext(glRenderContext);
    #endif
};

//-------------------------------------------------------------//
//                           END MAIN                          //
//-------------------------------------------------------------//


int main() {
    StartEngine(200, 200, 1, true, false);
    return 0;
}
