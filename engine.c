#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <GL/gl.h>
#define WNDCLASSNAME "GLClass"
#define WNDNAME	"My app"

typedef enum {false,true} bool;

typedef struct vector2d {
    uint32_t X;
    uint32_t Y;
} vector2d;

typedef struct PixelStruct {
	union {
		uint32_t n;
		struct {uint8_t r;uint8_t g;uint8_t b;uint8_t a;};
	}; 
} PixelS;

typedef struct TextureStruct {
	uint32_t width;
	uint32_t height;
	GLuint id;
	PixelS TexData[];

} TextureS;

typedef struct ButtonInput {
	bool pressed;
	bool released;
	bool held;
} ButtonInput;

typedef enum KeyMapping {
	key_0 = 0x30, key_1 = 0x31, key_2 = 0x32, key_3 = 0x33, key_4 = 0x34, key_5 = 0x35, key_6 = 0x36, key_7 = 0x37, key_8 = 0x38, key_09 = 0x39, 
	key_A = 0x41, key_B = 0x42, key_C = 0x43, key_D = 0x44, key_E = 0x45, key_F = 0x46, key_G = 0x47, key_H = 0x48, key_I = 0x49, key_J = 0x4A,
	key_K = 0x4B, key_L = 0x4C, key_M = 0x4D, key_N = 0x4E, key_O = 0x4F, key_P = 0x50, key_Q = 0x51, key_R = 0x52, key_S = 0x53, key_T = 0x54,
	key_U = 0x55, key_V = 0x56, key_W = 0x57, key_X = 0x58, key_Y = 0x59, key_Z = 0x5A, key_Arrow_Left = 0x25, key_Arrow_Up = 0x26, key_Arrow_Right = 0x27,
	key_Arrow_Down = 0x28, key_Esc = 0x1B, key_Ctrl = 0x11, key_Shift = 0x10, key_Enter = 0x0D, key_Backspace = 0x08, key_Spacebar = 0x20, key_Delete = 0x2E
} KeyMapping;

HDC glDeviceContext;
HGLRC glRenderContext;
HWND windowHandle;
HANDLE engineThread;
bool applicationIsRunning=false;
bool fullscreenIsEnabled=false;
bool vsyncIsEnabled=false;
bool keyboardOldState[100];
bool keyboardNewState[100];
bool mouseOldState[3];
bool mouseNewState[3];
vector2d vScreenSize;
vector2d vViewSize;
vector2d vWindowSize;
vector2d vViewPosition;
uint32_t vScale = 1;
GLuint id=0;
vector2d vMousePosition;
ButtonInput vMouse[3]; 
ButtonInput vKeyboard[100];
TextureS *currentTarget=NULL;
TextureS *layers[32];
int32_t textureNumber=0;

TextureS *MakeTex(uint32_t width,uint32_t height, PixelS *imgData);
PixelS Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
LRESULT CALLBACK WindowEvent(HWND windowHandle, UINT uMessage, WPARAM wParameter, LPARAM lParameter);
bool StartSystemLoop(); 
bool CreateWindowPane(bool boolFullscreen, vector2d winPos, vector2d winSize);
bool CreateDevice(HWND windowHandle, bool boolFullscreen, bool boolVsync); 
bool CreateGraphics(bool boolFullscreen, bool boolVsync, vector2d viewPos, vector2d viewSize); 
void DisplayFrame(); 
void PrepareDrawing(); 
bool DestroyDevice(); 
bool ThreadCleanup(); 
void ClearBuffer(PixelS pixel,bool boolDepth);
void UpdateViewportEngine(uint32_t posx,uint32_t posy,uint32_t width,uint32_t height); 
void UpdateViewport(); 
uint32_t CreateTexture(uint32_t width, uint32_t height); 
void UpdateTexture(TextureS *texture);
void ApplyTexture(uint32_t id); 
uint32_t DeleteTexture(uint32_t id);
void DrawLayer(PixelS tint); 
void Terminate();
void CreateLayer(TextureS *texture);
bool WritePixel(uint32_t x, uint32_t y, PixelS pixel, TextureS *texture);
PixelS ReadPixel(uint32_t x, uint32_t y, TextureS *texture);
bool DrawPixel(uint32_t x, uint32_t y, PixelS pixel); // on default layer
PixelS CheckPixel(uint32_t x, uint32_t y); // on default layer
bool DrawTexture(uint32_t x, uint32_t y, TextureS *texture);
bool DrawPartialTexture(uint32_t x, uint32_t y, uint32_t top, uint32_t left, uint32_t right, uint32_t bottom, TextureS *texture);
void PrepareEngine(); 
DWORD WINAPI EngineThread(); 
bool Construct(uint32_t screenW, uint32_t screenH, uint32_t scale, bool boolFullscreen, bool boolVsync); 
bool Start(); 
void CoreUpdate();
bool OnInitU(); 
bool OnUpdateU(); 
void UpdateWinSize(uint32_t x, uint32_t y); 
uint32_t GetScreenWidth(); 
uint32_t GetScreenHeight(); 
void SetCurrentTargeti(uint32_t index);
void SetCurrentTargetp(TextureS *texture);
void UpdateMousePos(int32_t x,int32_t y);
void UpdateMouseState(bool state, int32_t button);
void SetKeyState(bool state, int32_t key);

PixelS Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha){
    PixelS pixel;
    PixelS *mem = &pixel;
	mem->n = red | (green << 8) | (blue << 16) | (alpha << 24);
    return pixel;
};

TextureS *MakeTex(uint32_t width,uint32_t height, PixelS *imgData){
    TextureS *texture = malloc(sizeof(TextureS)+sizeof(PixelS)*width*height);
	texture->id = CreateTexture(width, height);
    texture->height = height;
    texture->width = width;
	
    for (int i=0;i < width*height;i++){
        texture->TexData[i] = imgData[i];
	};
	
    printf("[DONE] MakeTex\n");
    return texture;  
};

LRESULT CALLBACK WindowEvent(HWND windowHandle, UINT uMessage, WPARAM wParameter, LPARAM lParameter) {
	switch(uMessage){
	case WM_SIZE: UpdateWinSize(lParameter & 0xFFFF, (lParameter >> 16) & 0xFFFF); break;
	case WM_MOUSEMOVE:{
		uint16_t x = lParameter & 0xFFFF; uint16_t y = (lParameter >> 16) & 0xFFFF;
		int16_t ix = *(int16_t*)&x;   int16_t iy = *(int16_t*)&y;
		UpdateMousePos(ix,iy);
		break; 
	};
	case WM_KEYDOWN: SetKeyState(true,wParameter); break;
	case WM_KEYUP: SetKeyState(false,wParameter); break;
	case WM_LBUTTONDOWN: UpdateMouseState(true,0); break;
	case WM_LBUTTONUP: UpdateMouseState(false,0); break;
	case WM_RBUTTONDOWN: UpdateMouseState(true,1); break;
	case WM_RBUTTONUP: UpdateMouseState(false,1); break;
	case WM_MBUTTONDOWN: UpdateMouseState(true,2); break;
	case WM_MBUTTONUP: UpdateMouseState(false,2); break;
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_CLOSE: Terminate(); break;
	};
	return DefWindowProc(windowHandle, uMessage, wParameter, lParameter);
};

bool StartSystemLoop(){
	MSG message;
	while (GetMessage(&message,NULL,0,0) > 0){
		TranslateMessage(&message);
		DispatchMessage(&message);
	};
	return true;
};

bool CreateWindowPane(bool boolFullscreen, vector2d windowPosition, vector2d windowSize){
	WNDCLASS windowClass;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpfnWndProc = WindowEvent;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.lpszMenuName = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszClassName = WNDCLASSNAME;
	RegisterClass(&windowClass);
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;

	vector2d vTopLeft;
	vTopLeft.X = windowPosition.X;
	vTopLeft.Y = windowPosition.Y;
	
	if (boolFullscreen){
		dwExStyle = 0;
		dwStyle = WS_VISIBLE | WS_POPUP;
		HMONITOR hmon = MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		if (!GetMonitorInfo(hmon, &mi)) return false;
		vWindowSize.X = mi.rcMonitor.right;
		vWindowSize.Y = mi.rcMonitor.bottom;
		vTopLeft.X = 0;
		vTopLeft.Y = 0;
	};

	RECT windowRectangle;
	windowRectangle.left = 0; windowRectangle.top = 0;
	windowRectangle.right = windowSize.X; windowRectangle.bottom = windowSize.Y;
	AdjustWindowRectEx(&windowRectangle, dwStyle, false, dwExStyle);
	int width = windowRectangle.right - windowRectangle.left;
	int height = windowRectangle.bottom - windowRectangle.top;

	windowHandle = CreateWindowEx(dwExStyle, WNDCLASSNAME, WNDNAME, dwStyle, vTopLeft.X, vTopLeft.Y, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
	return true;
};

bool CreateDevice(HWND windowHandle, bool boolFullscreen, bool boolVsync){
    //POSSIBLE TO REMOVE FRAME CAP
	glDeviceContext = GetDC(windowHandle);
	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); pfd.nVersion = 1; pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA; pfd.cColorBits = 32; pfd.cAccumBits = 0;
	pfd.cRedBits = 0; pfd.cRedShift = 0; pfd.cGreenBits = 0; pfd.cGreenShift = 0; pfd.cBlueBits = 0; pfd.cBlueShift = 0; pfd.cAlphaBits = 0; pfd.cAlphaShift = 0;
	pfd.cAccumRedBits = 0; pfd.cAccumGreenBits = 0; pfd.cAccumBlueBits = 0; pfd.cAccumAlphaBits = 0; pfd.cDepthBits = 0; pfd.cStencilBits = 0; pfd.cAuxBuffers = 0;
	pfd.iLayerType = PFD_MAIN_PLANE; pfd.bReserved = 0; pfd.dwLayerMask = 0; pfd.dwVisibleMask = 0; pfd.dwDamageMask = 0;

	int pf = 0;
	if (!(pf = ChoosePixelFormat(glDeviceContext,&pfd))) {
		return false;
	};
	SetPixelFormat(glDeviceContext,pf,&pfd);
	if (!(glRenderContext = wglCreateContext(glDeviceContext))){
		return false;
	};
	wglMakeCurrent(glDeviceContext, glRenderContext);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	return true;
};

bool CreateGraphics(bool boolFullscreen, bool boolVsync, vector2d viewPosition, vector2d viewSize){
	if (CreateDevice(windowHandle,fullscreenIsEnabled,vsyncIsEnabled)==true){
		UpdateViewportEngine(viewPosition.X,viewPosition.Y,viewSize.X,viewSize.Y);
		return true;
	} else {
		return false;
	};
};

void DisplayFrame(){
	SwapBuffers(glDeviceContext);
};

void PrepareDrawing(){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
};

bool DestroyDevice(){
	wglDeleteContext(glRenderContext);
	return true;
};

bool ThreadCleanup(){
	DestroyDevice();
	PostMessage(windowHandle,WM_DESTROY,0,0);
	return true;
};

void ClearBuffer(PixelS pixel,bool boolDepth){
	glClearColor(((float)pixel.r)/255.0f,((float)pixel.g)/255.0f,((float)pixel.b)/255.0f,((float)pixel.a)/255.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	if (boolDepth){
		glClear(GL_DEPTH_BUFFER_BIT);
	};
};

void UpdateViewportEngine(uint32_t positionX,uint32_t positionY,uint32_t width,uint32_t height){
	glViewport(positionX,positionY,width,height);
};

void UpdateViewport(){
	uint32_t windowWidth = vScreenSize.X * vScale;
	uint32_t windowHeight = vScreenSize.Y * vScale;
	float ratio = (float)windowWidth / (float)windowHeight;

	vViewSize.X = (uint32_t)vWindowSize.X;
	vViewSize.Y = (uint32_t)((float)vViewSize.X/ratio);

	if (vViewSize.Y > vWindowSize.Y){
		vViewSize.Y = vWindowSize.Y;
		vViewSize.X = (uint32_t)((float)vViewSize.Y * ratio);
	};
	vViewPosition.X = (vWindowSize.X-vViewSize.X)/2;
	vViewPosition.Y = (vWindowSize.Y-vViewSize.Y)/2;
};

uint32_t CreateTexture(uint32_t width, uint32_t height){
	glGenTextures(1,&id);
	glBindTexture(GL_TEXTURE_2D,id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	return id;
};

void ApplyTexture(uint32_t id){
	glBindTexture(GL_TEXTURE_2D,id);
};

void UpdateTexture(TextureS *texture){
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture->TexData);
};

uint32_t DeleteTexture(uint32_t id){
	glDeleteTextures(1,&id);
	return id;
};

void DrawLayer(PixelS tint){
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();
};

void Terminate(){
	for (int i=0;i<textureNumber;i++){
		free(&layers[i]);
	};
	applicationIsRunning = false;
	printf("[DONE] Memory cleanup\n");
};

void CreateLayer(TextureS *texture){
    layers[textureNumber] = texture;
    textureNumber++;
};

bool WritePixel(uint32_t x, uint32_t y, PixelS pixel, TextureS *texture){
	if ((x<texture->width) && (y<texture->height)){
		texture->TexData[(texture->width)*y+x] = pixel;
		return true;
	};
	return false;
};
PixelS ReadPixel(uint32_t x, uint32_t y, TextureS *texture){
	if ((x<texture->width) && (y<texture->height)){
		return texture->TexData[(texture->width)*y+x];
	};
	return Pixel(0,0,0,0);
};

bool DrawPixel(uint32_t x, uint32_t y, PixelS pixel){
	x = x*vScale;
	y = y*vScale;
	if ((x<layers[0]->width) && (y<layers[0]->height)){
		if (vScale>1){
			for (int i = 0;i < vScale;i++){
				for (int j = 0;j < vScale;j++){
					WritePixel(x+i,y+j,pixel,layers[0]);
				};
			};
		} else {
			WritePixel(x,y,pixel,layers[0]);
		};
		return true;
	};
	return false;
};

PixelS CheckPixel(uint32_t x, uint32_t y){
	x = x*vScale;
	y = y*vScale;
	if ((x<layers[0]->width) && (y<layers[0]->height)){
		return layers[0]->TexData[(layers[0]->width)*y+x];
	};
	return Pixel(0,0,0,0);
};

bool DrawTexture(uint32_t x, uint32_t y, TextureS *texture){
	x = x*vScale;
	y = y*vScale;

    if (vScale > 1){
        for (uint32_t i = 0; i < texture->width; i++){
            for (uint32_t j = 0; j < texture->height; j++){
                for (uint32_t is = 0; is < vScale; is++){
                    for (uint32_t js = 0; js < vScale; js++){
						WritePixel(x + (i*vScale) + is, y + (j*vScale) + js,ReadPixel(i,j,texture),layers[0]);
					};
				};
			};	
        };
    } else {
        for (int32_t i = 0; i < texture->width; i++){
            for (int32_t j = 0; j < texture->height; j++){
				WritePixel(x+i, y+j,ReadPixel(i,j,texture),layers[0]);
			};
        };
    };
	return true;
};

bool DrawPartialTexture(uint32_t x, uint32_t y, uint32_t top, uint32_t left, uint32_t right, uint32_t bottom, TextureS *texture){
	x = x*vScale;
	y = y*vScale;
TEST STARTING POINT 0,0
    if (vScale > 1){
        for (uint32_t i = left; i < right+1; i++){
            for (uint32_t j = top; j < bottom+1; j++){
                for (uint32_t is = 0; is < vScale; is++){
                    for (uint32_t js = 0; js < vScale; js++){
						WritePixel(x + ((i-left)*vScale) + is, y + ((j-top)*vScale) + js, ReadPixel(i,j,texture), layers[0]);
					};
				};
			};	
        };
    } else {
        for (int32_t i = left; i < right+1; i++){
            for (int32_t j = top; j < bottom+1; j++){
				WritePixel(x + i - left, y + j - top, ReadPixel(i,j,texture), layers[0]);
			};
        };
    };
	return true;
};

void PrepareEngine(){
	PixelS *data = malloc(sizeof(PixelS)*vWindowSize.X*vWindowSize.Y);
	for (uint32_t i = 0; i < vWindowSize.X*vWindowSize.Y;i++){
		data[i] = Pixel(255,255,255,255);
	};
	CreateLayer(MakeTex(vWindowSize.X,vWindowSize.Y, data));
	free(data);
	SetCurrentTargeti(0);
	UpdateTexture(currentTarget);
    printf("[DONE] Default texture created\n");
};

DWORD WINAPI EngineThread(){
	printf("[INIT] Engine thread\n");
	CreateGraphics(fullscreenIsEnabled,vsyncIsEnabled,vViewPosition,vViewSize);
	PrepareEngine();
	printf("[DONE] Engine prep\n");
	if (!OnInitU()){
		applicationIsRunning = false;
	};
	while (applicationIsRunning){
		CoreUpdate();
	};
	ThreadCleanup();
	return 1;
};

bool Construct(uint32_t screenW, uint32_t screenH, uint32_t scale, bool boolFullscreen, bool boolVsync){
	vScreenSize.X = screenW;
	vScreenSize.Y = screenH;
	vScale = scale;
	vWindowSize.X = vScreenSize.X * scale;
	vWindowSize.Y = vScreenSize.Y * scale;
	fullscreenIsEnabled = boolFullscreen;
	vsyncIsEnabled = boolVsync;
    printf("[DONE] Construct finished\n");
	return true;
};

bool Start(){
	for (int i = 0; i < 3; i++){
		vMouse[i].released = false;
		vMouse[i].pressed = false;
		vMouse[i].held = false;
	};
	for (int i = 0; i < 100; i++){
		vKeyboard[i].held = false;
		vKeyboard[i].pressed = false;
		vKeyboard[i].released = false;
	};
	vMousePosition.X = 0; vMousePosition.Y = 0; 
    vector2d windowPosition;
    windowPosition.X = 50;
    windowPosition.Y = 50;
	CreateWindowPane(fullscreenIsEnabled,windowPosition,vWindowSize);
	UpdateWinSize(vWindowSize.X,vWindowSize.Y);
	applicationIsRunning = true;
	engineThread = CreateThread(NULL,0,*EngineThread,NULL,0,NULL);
	StartSystemLoop();
	if (engineThread != NULL){
		WaitForSingleObject(engineThread,INFINITE);
	};
	printf("[DONE] Start ended\n");
	return true;
};

void CoreUpdate(){
	ClearBuffer(Pixel(0,0,0,255),true);
	if (!OnUpdateU()){
		applicationIsRunning = false;
	};
	for (int i = 0; i < 3; i++){
		vMouse[i].pressed = false;
		vMouse[i].released = false;
		if (mouseOldState[i] != mouseNewState[i]){
			if (mouseNewState[i]){
				vMouse[i].pressed = !vMouse[i].held;
				vMouse[i].held = true;
			} else {
				vMouse[i].released = true;
				vMouse[i].held = false;
			};
		};
		mouseOldState[i] = mouseNewState[i];
	};
	for (int i = 0; i < 100; i++){
		vKeyboard[i].pressed = false;
		vKeyboard[i].released = false;
		if (keyboardOldState[i] != keyboardNewState[i]){
			if (keyboardNewState[i]){
				vKeyboard[i].pressed = !vKeyboard[i].held;
				vKeyboard[i].held = true;
			} else {
				vKeyboard[i].released = true;
				vKeyboard[i].held = false;
			};
		};
		keyboardOldState[i] = keyboardNewState[i];
	};

	UpdateViewportEngine(vViewPosition.X,vViewPosition.Y,vViewSize.X,vViewSize.Y);
	ClearBuffer(Pixel(0,0,0,255),true);
	PrepareDrawing();
	ApplyTexture(currentTarget->id);
	UpdateTexture(currentTarget);
	DrawLayer(Pixel(0,0,0,0));
	DisplayFrame();
};

void UpdateWinSize(uint32_t x, uint32_t y){
	vWindowSize.X = x;
	vWindowSize.Y = y;
	UpdateViewport();
};

uint32_t GetScreenWidth(){
	return vScreenSize.X;
};

uint32_t GetScreenHeight(){
	return vScreenSize.Y;
};

void SetCurrentTargeti(uint32_t index){
	currentTarget = layers[index];
};

void SetCurrentTargetp(TextureS *texture){
	currentTarget = texture;
};

void UpdateMousePos(int32_t x,int32_t y){
	vMousePosition.X = x/vScale;
	vMousePosition.Y = y/vScale;
};

void UpdateMouseState(bool state, int32_t button){
	mouseNewState[button] = state;
};

void SetKeyState(bool state, int32_t key){
	keyboardNewState[key] = state;
};

bool OnInitU(){
	PixelS *data1 = malloc(sizeof(PixelS)*20*20);
	for (uint32_t i = 0; i < 20*20;i++){
		data1[i] = Pixel(255,100,255,255);
	};
	CreateLayer(MakeTex(20, 20, data1));
	free(data1);
	DrawPartialTexture(1,1,3,3,8,8,layers[1]);
	DrawPixel(2,2,Pixel(255,255,90,255));
	DrawPixel(2,3,Pixel(255,99,90,255));
	return true;
}; 

bool OnUpdateU(){
	if (vKeyboard[key_W].pressed){
		printf("W pressed ");
	};
	if (vMouse[0].pressed){
		printf("Click");
	};
	return true;
};

int main(){
    Construct(300,250,2,false,false);
	Start();
}

