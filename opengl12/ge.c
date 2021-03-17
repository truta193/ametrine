#ifdef _WIN32
    #define WINDOWS
#else
    #define LINUX
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <GL/gl.h>
#include <pthread.h>


#ifdef LINUX
    #include <X11/Xatom.h>
    #include <X11/Xlib.h>
    #include <GL/glx.h>
#else
    #include <windows.h>
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

typedef struct v2 {
    int32 X;
    int32 Y;
} v2;

typedef struct PixelT {
    union {
        uint32 rgba;
        struct {
            uint8 r; uint8 g; uint8 b; uint8 a;
        };
    };
} PixelT;

typedef struct SpriteT {
    int32 width;
    int32 height;
    PixelT buffer[];
} SpriteT;

typedef struct LayerT { 
    uint32 glid;
    uint32 index;
    v2 offset;
    bool visible;
    bool update;
    SpriteT *sprite;
} LayerT;

typedef struct ButtonState {
    bool pressed;
    bool released;
    bool held;
} ButtonState;

typedef enum MouseMap {
    LClick = 1, MClick = 2, RClick = 3, ScrollUp = 4, ScrollDown = 5
} MouseMap;

int32 screenScale = 1;
int32 wheelDelta = 0;
int32 layerCount = 0;
v2 viewportSize = {0,0};
v2 viewportPosition = {0,0};
v2 screenSize = {0,0};
v2 windowSize = {0,0};
v2 windowPosition = {0,0};
v2 mousePosition = {0,0};
bool fullscreenEnabled = false;
bool vsyncEnaled = false;
bool appRunning = true;
longint window;
GLDeviceContext glDeviceContext;
GLRenderContext glRenderContext;
static GLSwapInterval *glSwapIntervalEXT;
ButtonState keyboard[256] = {false,false,false};
ButtonState mouse[4] = {false,false,false};
bool currentKeyboardState[256] = {false};
bool previousKeyboardState[256] = {false};
bool currentMouseState[4] = {false};
bool previousMouseState[4] = {false};
LayerT *layers[16];


void CreateWindowP(v2 windowPosition, v2 windowSize, bool fullscreen);           /**/
void EventHandler(); /*Linux event handler                                       PLATFORM*/
void StartEventLoop(); /*Windows event handler                                   SPECIFIC*/
return_code SetWindowTitle(char *title);                                         /**/

void Construct(uint32 width, uint32 height, uint32 scale, bool vsync, bool fullscreen);
void Start();
void GLInitialize();
void *Thread();
void FrameRender();
void TerminateCleanup();
void BufferClear(bool colorBuffer, bool depthBuffer);

PixelT Pixel(uint8 red, uint8 green, uint8 blue, uint8 alpha);
PixelT PixelRetrieve(uint32 x, uint32 y,  SpriteT *sprite); 
return_code PixelDraw(PixelT pixel, uint32 x, uint32 y, SpriteT *sprite); 
SpriteT *SpriteCreate(uint32 width, uint32 height, PixelT *buffer);
SpriteT *SpriteLoadFromImage(char path[]);
void SpriteClear(SpriteT *sprite, PixelT pixel);
void SpriteDelete(SpriteT *sprite);
void SpriteDraw(SpriteT *sprite, uint32 x, uint32 y, uint32 scale, SpriteT *target);
void SpriteDrawPartial(SpriteT *sprite, uint32 left, uint32 top, uint32 right, uint32 bottom, uint32 scale,  SpriteT *target, uint32 x, uint32 y);
LayerT *LayerCreate(bool visible, bool update);          
void LayerDelete(uint32 id);

return_code UserInitEvent();
return_code UserUpdateEvent();
void UserTerminateEvent();

void ViewportSizeUpdate();
void WindowSizeUpdate(uint32 x, uint32 y);
void LayerOffsetUpdate(LayerT *layer, uint32 x, uint32 y);

//-------------------------------------------------------------//
//                           START UTIL                        //
//-------------------------------------------------------------//

#define MAX(x,y) (x>y?x:y)
#define ABS(x) (x>0?x:-(x))

//-------------------------------------------------------------//
//                            END UTIL                         //
//-------------------------------------------------------------//

SpriteT *test;

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

void CreateWindowP(v2 windowPosition, v2 windowSize, bool fullscreen) {
    XInitThreads();
    display = XOpenDisplay(NULL);
    rootWindow = DefaultRootWindow(display);
    XSetWindowAttributes windowAttributes;
    GLint attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    visualInfo = glXChooseVisual(display, 0, attribs);
    colourmap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);
    windowAttributes.colormap = colourmap;
    windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask;
    window = (longint)XCreateWindow(display, rootWindow, windowPosition.X, windowPosition.Y, windowSize.X, windowSize.Y, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &windowAttributes);
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
                currentKeyboardState[(uint8)keysim] = true;
                break;
            };
            case KeyRelease: {
                KeySym keysim = XLookupKeysym(&event.xkey, 0);
                XKeyEvent *e =(XKeyEvent*)&event;
                XLookupString(e, NULL, 0, &keysim, NULL);
                currentKeyboardState[(uint8)keysim] = false;
                break;
            };
            case ButtonPress: {
                if (event.xbutton.button < 4) currentMouseState[event.xbutton.button] = true; else if (event.xbutton.button == ScrollUp) wheelDelta += 120; else if (event.xbutton.button == ScrollDown) wheelDelta -= 120;
                break;
            };
            case ButtonRelease: {
                if (event.xbutton.button < 4) currentMouseState[event.xbutton.button] = false;
                break;
            };
            case MotionNotify: {
                mousePosition.X = event.xbutton.x;
                mousePosition.Y = event.xbutton.y;
                break;
            };
            case ClientMessage: { TerminateCleanup(); printf("Reached here"); break; }; /*Call to terminate function for memory cleanup*/
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

void CreateWindowP(v2 windowPosition, v2 windowSize, bool fullscreen) {
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
    
	v2 vTopLeft;
	vTopLeft.X = windowPosition.X;
	vTopLeft.Y = windowPosition.Y;
	
	if (fullscreen){
		dwExStyle = 0;
		dwStyle = WS_VISIBLE | WS_POPUP;
		HMONITOR hmon = MonitorFromWindow((HWND)window, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		if (!GetMonitorInfo(hmon, &mi)) {
            return;
        };
		windowSize.X = mi.rcMonitor.right;
		windowSize.Y = mi.rcMonitor.bottom;
		vTopLeft.X = 0;
		vTopLeft.Y = 0;
	};
    
	RECT windowRectangle;
	windowRectangle.left = 0; windowRectangle.top = 0;
	windowRectangle.right = windowSize.X; windowRectangle.bottom = windowSize.Y;
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

LRESULT CALLBACK EventLoop(HWND windowHandle, uint32 uMessage, WPARAM wParameter, LPARAM lParameter){
    switch(uMessage){
        case WM_SIZE: { WindowSizeUpdate(lParameter & 0xFFFF, (lParameter >> 16) & 0xFFFF); break; };
        case WM_MOUSEMOVE: {
            uint16 x = lParameter & 0xFFFF;
            uint16 y = (lParameter >> 16) & 0xFFFF;
            mousePosition.X = *(int16*)&x;
            mousePosition.Y = *(int16*)&y;
            break;
        };
        case WM_KEYDOWN: { currentKeyboardState[wParameter] = true; break; };
        case WM_KEYUP: { currentKeyboardState[wParameter] = false; break; };
        case WM_LBUTTONDOWN: { currentMouseState[LClick] = true; break; };
        case WM_LBUTTONUP: { currentMouseState[LClick] = false; break; };
        case WM_RBUTTONDOWN: { currentMouseState[RClick] = true; break; };
        case WM_RBUTTONUP: { currentMouseState[RClick] = false; break; };
        case WM_MBUTTONDOWN: { currentMouseState[MClick] = true; break; };
        case WM_MBUTTONUP: { currentMouseState[MClick] = false; break; };
        case WM_MOUSEWHEEL: { wheelDelta += GET_WHEEL_DELTA_WPARAM(wParameter); break; };
        case WM_DESTROY: { PostQuitMessage(0); break; };
        case WM_CLOSE:{ TerminateCleanup(); break; };
    };
    return DefWindowProc(windowHandle, uMessage, wParameter, lParameter);
};

return_code SetWindowTitle(char* title) {
    if (SetWindowText((HWND)window, title) != 0) return OK; else return FAIL;
};

#endif

//-------------------------------------------------------------//
//                         END PLATFORM                        //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                          START INIT                         //
//-------------------------------------------------------------//

void Construct(uint32 width, uint32 height, uint32 scale, bool vsync, bool fullscreen) {
    screenScale  = scale;
    screenSize.X = width;
    screenSize.Y = height;
    windowSize.X = width*scale;
    windowSize.Y = height*scale;
    fullscreenEnabled = fullscreen;
    vsyncEnaled = vsync;
    appRunning = true;
};

void Start() {
    CreateWindowP(windowPosition, windowSize, fullscreenEnabled);
    WindowSizeUpdate(windowSize.X, windowSize.Y);
    #ifdef STBI_INCLUDE_STB_IMAGE_H 
        stbi_set_flip_vertically_on_load(true);
    #endif
    pthread_t tid;  
    pthread_create(&tid, NULL, &Thread, NULL);
    StartEventLoop();
    pthread_join(tid, NULL);
};

void GLInitialize() {
    #ifdef LINUX
        glDeviceContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
        glXMakeCurrent(display, window, glDeviceContext);
        glSwapIntervalEXT = NULL;
        glSwapIntervalEXT = (GLSwapInterval*)glXGetProcAddress((unsigned char*)"glXSwapIntervalEXT");
        if (glSwapIntervalEXT == NULL && !vsyncEnaled) printf("Unable to disable vsync, will run at monitor's default refresh rate");
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
};

void *Thread() {
    /*init clock*/ //TODO:Implement clock for fps
    GLInitialize();
    LayerCreate(true, true);

    if (!UserInitEvent()) appRunning = false;
    while (appRunning) FrameRender();
    
    #ifdef LINUX
        glXDestroyContext(display, glDeviceContext);
        XEvent event;
        XSendEvent(display, window, false, 0, &event);
    #else
        wglDeleteContext(glRenderContext);
        PostMessage((HWND)window,WM_DESTROY,0,0);
    #endif
    pthread_exit(0);
};

void FrameRender() {
    EventHandler();
    if (!UserUpdateEvent()) appRunning = false;
    for (int32 i = 1; i < NO_MOUSE_BUTTONS; i++) {
        mouse[i].pressed = false;
        mouse[i].released = false;
        if (previousMouseState[i] != currentMouseState[i])
            if (currentMouseState[i]) {
                mouse[i].pressed = !mouse[i].held;
                mouse[i].held = true;
            } else {
                mouse[i].released = true;
                mouse[i].held = false;
            };
        previousMouseState[i] = currentMouseState[i];
    };
    for (int32 i = 0; i < NO_KEYBOARD_BUTTONS; i++) {
        keyboard[i].pressed = false;
        keyboard[i].released = false;
        if (previousKeyboardState[i] != currentKeyboardState[i])
            if (currentKeyboardState[i]) {
                keyboard[i].pressed = !keyboard[i].held;
                keyboard[i].held = true;
            } else {
                keyboard[i].released = true;
                keyboard[i].held = false;
            };
        previousKeyboardState[i] = currentKeyboardState[i];
    };
    
     glViewport(viewportPosition.X, viewportPosition.Y, viewportSize.X, viewportSize.Y);
    BufferClear(true, false); //TODO: Global variable for these
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    for (int32 i = 0; i < layerCount; i++) {
        if (layers[i]->visible) {
            glBindTexture(GL_TEXTURE_2D, layers[i]->glid);
            if (layers[i]->update) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, layers[i]->sprite->width, layers[i]->sprite->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &layers[i]->sprite->buffer);           
            glBegin(GL_TRIANGLES);
            glTexCoord2f(0.0f + layers[0]->offset.X, 0.0f + layers[0]->offset.Y);
            glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(1.0f + layers[0]->offset.X, 0.0f + layers[0]->offset.Y);
            glVertex2f(1.0f, -1.0f);
            glTexCoord2f(1.0f + layers[0]->offset.X, 1.0f + layers[0]->offset.Y);
            glVertex2f(1.0f, 1.0f);
            glTexCoord2f(0.0f + layers[0]->offset.X, 0.0f + layers[0]->offset.Y);
            glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(1.0f + layers[0]->offset.X, 1.0f + layers[0]->offset.Y);
            glVertex2f(1.0f, 1.0f);
            glTexCoord2f(0.0f + layers[0]->offset.X, 1.0f + layers[0]->offset.Y);
            glVertex2f(-1.0f, 1.0f);
            glEnd();
        };
    };
    
    #ifdef LINUX
        glXSwapBuffers(display, window);  
    #else
        SwapBuffers(glDeviceContext);
    #endif
    
};

void TerminateCleanup(){
    for (int32 i = 0; i < layerCount; i++) {
        free(layers[i]->sprite->buffer);
        free(layers[i]->sprite);
        free(layers[i]);
    };
    UserTerminateEvent();
    appRunning = false;
};

void BufferClear(bool colorBuffer, bool depthBuffer) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    if (colorBuffer) glClear(GL_COLOR_BUFFER_BIT);
    if (depthBuffer) glClear(GL_DEPTH_BUFFER_BIT);
};

//-------------------------------------------------------------//
//                           END INIT                          //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                         START RENDER                        //
//-------------------------------------------------------------//

PixelT Pixel(uint8 red, uint8 green, uint8 blue, uint8 alpha) {
    PixelT pixel;
    PixelT *mem = &pixel;
    mem->rgba = red | (green << 8) | (blue << 16) | (alpha << 24);
    return pixel;
};

PixelT PixelRetrieve(uint32 x, uint32 y, SpriteT *sprite) {
    if (x < sprite->width && y < sprite->height) {
        return sprite->buffer[(sprite->width)*y + x];
    };
    return Pixel(0,0,0,0);
};

return_code PixelDraw(PixelT pixel, uint32 x, uint32 y, SpriteT *sprite) {
    if (x < sprite->width && y < sprite->height) {
        sprite->buffer[(sprite->width)*y + x] = pixel;
        return OK;
    }
    return FAIL;
};

SpriteT *SpriteCreate(uint32 width, uint32 height, PixelT *buffer) {
    SpriteT *sprite = (SpriteT*) malloc(sizeof(SpriteT) + sizeof(PixelT)*width*height);
    sprite->width = width;
    sprite->height = height;
    if (buffer == NULL) 
        for (uint32 i = 0; i < width*height; i++) sprite->buffer[i] = Pixel(0,100,0,255); //TODO: FOR DEBUG, REPLACE WITH 0,0,0,0
    else 
        for (uint32 i = 0; i < width*height; i++) sprite->buffer[i] = buffer[i];
    return sprite;
};

SpriteT *SpriteLoadFromImage(char path[]) {
    uint32 width, height, noc;
    unsigned char *buffer = stbi_load(path, &width, &height, &noc, 4);
    if (buffer == NULL) printf("Unable to load image!\n");
    stbi_image_free(buffer);
    return SpriteCreate(width, height, (PixelT*)buffer);
};

void SpriteClear(SpriteT *sprite, PixelT pixel) {
    for (uint32 i = 0; i < sprite->width*sprite->height; i++) sprite->buffer[i] = pixel;
};

void SpriteDelete(SpriteT *sprite) {
    free(sprite);
};

void SpriteDraw(SpriteT *sprite, uint32 x, uint32 y, uint32 scale, SpriteT *target) { 
    uint32 boundX = sprite->width * scale;
    uint32 boundY = sprite->height * scale;
    if (target->width - x < boundX) boundX += target->width - x - boundX;
    if (target->height - y < boundY) boundY += target->height - y - boundY;

    if (scale > 1) {
        for (uint32 i = 0; i < sprite->width; i++)
            for (uint32 j = 0; j < sprite->height; j++)
                for (uint32 k = 0; k < scale; k++)
                    for (uint32 l = 0; l < scale; l++)
                        PixelDraw(PixelRetrieve(i, j, sprite), x + i*scale + k, y + j*scale + l, target);
    } else {
        for (uint32 i = 0; i < boundX; i++)
            for (uint32 j = 0; j < boundY; j++)
                PixelDraw(PixelRetrieve(i, j, sprite), x + i, y + j, target);
    };
};

void SpriteDrawPartial(SpriteT *sprite, uint32 left, uint32 top, uint32 right, uint32 bottom, uint32 scale,  SpriteT *target, uint32 x, uint32 y) {
    uint32 boundX = (right - left + 1) * scale;
    uint32 boundY = (bottom - top + 1) * scale;
    if (target->width - x < boundX) boundX += target->width - x - boundX;
    if (target->height - y < boundY) boundY += target->height - y - boundY;

    if (scale > 1) {
        for (uint32 i = left; i < right + 1; i++) 
            for (uint32 j = top; j < bottom + 1; j++)
                for (uint32 k = 0; k < scale; k++)
                    for (uint32 l = 0; l < scale; l++)
                        PixelDraw(PixelRetrieve(i, j, sprite), x + (i - left)*scale + k, y + (j - top)*scale + l, target);
    } else {
        for (uint32 i = left; i < right + 1; i++)
            for (uint32 j = top; j < bottom + 1; j++)
                PixelDraw(PixelRetrieve(i, j, sprite), x + i - left, y + j - top, target);
    };
};

LayerT *LayerCreate(bool visible, bool update){ 
    GLuint id;
    LayerT *layer = (LayerT*) malloc(sizeof(LayerT));
    SpriteT *sprite = SpriteCreate(screenSize.X, screenSize.Y, NULL);
    layer->visible = visible;
    layer->update = update;
    layer->index = layerCount;
    layer->sprite = sprite;
    
    glGenTextures(1, &id);
    layer->glid = id;
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenSize.X, screenSize.Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, layer->sprite->buffer);
    layers[layerCount++] = layer;
    return layer;
};

void LayerDelete(uint32 index) {
    if (layerCount > 0 && index < layerCount) {
        free(layers[index]->sprite);
        free(layers[index]);
        for (uint32 i = index; i < layerCount - 1; i++) {
            layers[i] = layers[i+1];
            layers[i]->index -= 1;
        };
        if (layerCount > 1) free(layers[layerCount--]);
    };
}; 

//-------------------------------------------------------------//
//                           END RENDER                        //
//-------------------------------------------------------------//

//-------------------------------------------------------------//
//                          EVENTS BEGIN                       //
//-------------------------------------------------------------//

return_code UserInitEvent() { 
    PixelT tt[900];
    for (uint32 i = 0; i < 900; i++) tt[i] = Pixel(0, 255, 255, 255);
    test = SpriteCreate(30, 30, tt);
    SpriteDraw(test, 0, 0 , 1, layers[0]->sprite);
    PixelDraw(Pixel(255,0,0,255), 100, 100, layers[0]->sprite);
    return OK; 
};

return_code UserUpdateEvent() { return OK; };

void UserTerminateEvent() { return; };

//-------------------------------------------------------------//
//                          EVENTS END                         //
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//                        UPDATES BEGIN                        //
//-------------------------------------------------------------//

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

void WindowSizeUpdate(uint32 x, uint32 y) {
    windowSize.X = x;
    windowSize.Y = y;
    ViewportSizeUpdate();
};

void LayerOffsetUpdate(LayerT *layer, uint32 x, uint32 y) {
    layer->offset.X = x;
    layer->offset.Y = y;
};

//-------------------------------------------------------------//
//                          UPDATE END                         //
//-------------------------------------------------------------//

int main() {
    Construct(200,200, 3, true, false);
    Start();
    return 0;
};