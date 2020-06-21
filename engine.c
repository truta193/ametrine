#define _CRT_RAND_S
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <GL/gl.h>
#define WNDCLASSNAME "GLClass"
#define WNDNAME	"My app"
//FUNCTION TO SET WINDOW TITLE
//FRAME TIMER
//% positioning, e.g 0.5 away from X origin = X axis middle (0.0 - 1.0)
//Work on fullscreen
//Will need to rework mouse/keyboard input 

typedef struct XandY {
    uint32_t X;
    uint32_t Y;
} vector2d;

HDC glDeviceContext;
HGLRC glRenderContext;
HWND hWnd;
HANDLE engineThread;
BOOL isRunning;
BOOL isFullscreen=FALSE;
BOOL isVsync=FALSE;
BOOL isFocused=FALSE;

vector2d vScreenSize;
vector2d vViewSize;
vector2d vWinSize;
vector2d vViewPos;
uint32_t vScale = 1;
GLuint id=0;
vector2d vMousePos;
int  vKBCache;

typedef enum rcode {FAIL = 0, OK = 1} rcode;

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

typedef struct MState {
	BOOL LButton;
	BOOL RButton;
	BOOL MButton;
} MState;

MState vMouseState; 



typedef enum keyMapping {
	key_0 = 0x30, key_1 = 0x31, key_2 = 0x32, key_3 = 0x33, key_4 = 0x34, key_5 = 0x35, key_6 = 0x36, key_7 = 0x37, key_8 = 0x38, key_09 = 0x39, 
	key_A = 0x41, key_B = 0x42, key_C = 0x43, key_D = 0x44, key_E = 0x45, key_F = 0x46, key_G = 0x47, key_H = 0x48, key_I = 0x49, key_J = 0x4A,
	key_K = 0x4B, key_L = 0x4C, key_M = 0x4D, key_N = 0x4E, key_O = 0x4F, key_P = 0x50, key_Q = 0x51, key_R = 0x52, key_S = 0x53, key_T = 0x54,
	key_U = 0x55, key_V = 0x56, key_W = 0x57, key_X = 0x58, key_Y = 0x59, key_Z = 0x5A, key_Arrow_Left = 0x25, key_Arrow_Up = 0x26, key_Arrow_Right = 0x27,
	key_Arrow_Down = 0x28, key_Esc = 0x1B, key_Ctrl = 0x11, key_Shift = 0x10, key_Enter = 0x0D, key_Backspace = 0x08, key_Spacebar = 0x20, key_Delete = 0x2E
} keyMapping;

TextureS *currentTarget=NULL;
TextureS *layers[32];
int32_t textureNumber=0;

TextureS *MakeTex(uint32_t width,uint32_t height, PixelS *imgData);
PixelS Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
LRESULT CALLBACK WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
rcode StartSystemLoop(); 
rcode CreateWindowPane(BOOL boolFullscreen, vector2d winPos, vector2d winSize);
rcode CreateDevice(HWND hWnd, BOOL boolFullscreen, BOOL boolVsync); 
rcode CreateGraphics(BOOL boolFullscreen, BOOL boolVsync, vector2d viewPos, vector2d viewSize); 
void DisplayFrame(); 
void PrepareDrawing(); 
rcode DestroyDevice(); 
rcode ThreadCleanup(); 
void ClearBuffer(PixelS pixel,BOOL boolDepth);
void UpdateViewportEngine(uint32_t posx,uint32_t posy,uint32_t width,uint32_t height); 
void UpdateViewport(); 
uint32_t CreateTexture(uint32_t width, uint32_t height); 
void UpdateTexture(TextureS *texture);
void ApplyTexture(uint32_t id); 
uint32_t DeleteTexture(uint32_t id);
void DrawLayer(PixelS tint); 
void Terminate();
void CreateLayer(TextureS *texture);
rcode WritePixel(uint32_t x, uint32_t y, PixelS pixel, TextureS *texture);
PixelS ReadPixel(uint32_t x, uint32_t y, TextureS *texture);
rcode DrawPixel(uint32_t x, uint32_t y, PixelS pixel); // on default layer
PixelS CheckPixel(uint32_t x, uint32_t y); // on default layer
rcode DrawTexture(uint32_t x, uint32_t y, TextureS *texture);
void PrepareEngine(); 
DWORD WINAPI EngineThread(); 
rcode Construct(uint32_t screenW, uint32_t screenH, uint32_t scale, BOOL boolFullscreen, BOOL boolVsync); 
rcode Start(); 
void CoreUpdate();
BOOL OnInitU(); 
BOOL OnUpdateU(); 

void UpdateWinSize(uint32_t x, uint32_t y); 
void SetScale(uint32_t scale);
uint32_t ScreenWidth(); 
uint32_t ScreenHeight(); 
void SetCurrentTargeti(uint32_t index);
void SetCurrentTargetp(TextureS *texture);
void MouseUpdate(uint32_t button,BOOL state);
void MouseUpdatePos(int32_t x,int32_t y);
void Set_Focus(BOOL state);
void SetKeyState(BOOL state, int key);


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

LRESULT CALLBACK WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg){
	case WM_SIZE: UpdateWinSize(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF); break;
	case WM_MOUSEMOVE:{
	uint16_t x = lParam & 0xFFFF; uint16_t y = (lParam >> 16) & 0xFFFF;
	int16_t ix = *(int16_t*)&x;   int16_t iy = *(int16_t*)&y;
	MouseUpdatePos(ix,iy);
	break; 
	};
	case WM_SETFOCUS: Set_Focus(TRUE); break;
	case WM_KILLFOCUS: Set_Focus(FALSE); break;
	case WM_KEYDOWN: SetKeyState(TRUE,wParam); break;
	case WM_KEYUP: break; //SetKeyState(FALSE,wParam); 
	case WM_LBUTTONDOWN: MouseUpdate(0,TRUE); break;
	case WM_LBUTTONUP: MouseUpdate(0,FALSE); break;
	case WM_RBUTTONDOWN: MouseUpdate(1,TRUE); break;
	case WM_RBUTTONUP: MouseUpdate(1,FALSE); break;
	case WM_MBUTTONDOWN: MouseUpdate(2,TRUE); break;
	case WM_MBUTTONUP: MouseUpdate(2,FALSE); break;
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_CLOSE: Terminate(); break;
	};
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
};

rcode StartSystemLoop(){
	MSG msg;
	while (GetMessage(&msg,NULL,0,0) > 0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	};
	return OK;
};

rcode CreateWindowPane(BOOL boolFullscreen, vector2d winPos, vector2d winSize){
    //IMPLEMENT FULLSCREEN 
	WNDCLASS winC;
	winC.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winC.hCursor = LoadCursor(NULL, IDC_ARROW);
	winC.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	winC.hInstance = GetModuleHandle(NULL);
	winC.lpfnWndProc = WindowEvent;
	winC.cbClsExtra = 0;
	winC.cbWndExtra = 0;
	winC.lpszMenuName = NULL;
	winC.hbrBackground = NULL;
	winC.lpszClassName = WNDCLASSNAME;
	RegisterClass(&winC);
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;

	vector2d vTopLeft;
	vTopLeft.X = winPos.X;
	vTopLeft.Y = winPos.Y;
	/*
	if (boolFullscreen){
		dwExStyle = 0;
		dwStyle = WS_VISIBLE | WS_POPUP;
		HMONITOR hmon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		if (!GetMonitorInfo(hmon, &mi)) return FAIL;
		vWinSize.X = mi.rcMonitor.right;
		vWinSize.Y = mi.rcMonitor.bottom;
		vTopLeft.X = 0;
		vTopLeft.Y = 0;
	}*/

	RECT rWndRect;
	rWndRect.left = 0; rWndRect.top = 0;
	rWndRect.right = winSize.X; rWndRect.bottom = winSize.Y;
	AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
	int width = rWndRect.right - rWndRect.left;
	int height = rWndRect.bottom - rWndRect.top;

	hWnd = CreateWindowEx(dwExStyle, WNDCLASSNAME, WNDNAME, dwStyle, vTopLeft.X, vTopLeft.Y, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
	return OK;
};

rcode CreateDevice(HWND hWnd, BOOL boolFullscreen, BOOL boolVsync){
    //POSSIBLE TO REMOVE FRAME CAP
	glDeviceContext = GetDC(hWnd);
	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); pfd.nVersion = 1; pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA; pfd.cColorBits = 32; pfd.cAccumBits = 0;
	pfd.cRedBits = 0; pfd.cRedShift = 0; pfd.cGreenBits = 0; pfd.cGreenShift = 0; pfd.cBlueBits = 0; pfd.cBlueShift = 0; pfd.cAlphaBits = 0; pfd.cAlphaShift = 0;
	pfd.cAccumRedBits = 0; pfd.cAccumGreenBits = 0; pfd.cAccumBlueBits = 0; pfd.cAccumAlphaBits = 0; pfd.cDepthBits = 0; pfd.cStencilBits = 0; pfd.cAuxBuffers = 0;
	pfd.iLayerType = PFD_MAIN_PLANE; pfd.bReserved = 0; pfd.dwLayerMask = 0; pfd.dwVisibleMask = 0; pfd.dwDamageMask = 0;

	int pf = 0;
	if (!(pf = ChoosePixelFormat(glDeviceContext,&pfd))) {
		return FAIL;
	};
	SetPixelFormat(glDeviceContext,pf,&pfd);
	if (!(glRenderContext = wglCreateContext(glDeviceContext))){
		return FAIL;
	};
	wglMakeCurrent(glDeviceContext, glRenderContext);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	return OK;
};

rcode CreateGraphics(BOOL boolFullscreen, BOOL boolVsync, vector2d viewPos, vector2d viewSize){
	if (CreateDevice(hWnd,isFullscreen,isVsync)==OK){
		UpdateViewportEngine(viewPos.X,viewPos.Y,viewSize.X,viewSize.Y);
		return OK;
	} else {
		return FAIL;
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

rcode DestroyDevice(){
	wglDeleteContext(glRenderContext);
	return OK;
};

rcode ThreadCleanup(){
	DestroyDevice();
	PostMessage(hWnd,WM_DESTROY,0,0);
	return OK;
};

void ClearBuffer(PixelS pixel,BOOL boolDepth){
	glClearColor(((float)pixel.r)/255.0f,((float)pixel.g)/255.0f,((float)pixel.b)/255.0f,((float)pixel.a)/255.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	if (boolDepth){
		glClear(GL_DEPTH_BUFFER_BIT);
	};
};

void UpdateViewportEngine(uint32_t posx,uint32_t posy,uint32_t width,uint32_t height){
	glViewport(posx,posy,width,height);
};

void UpdateViewport(){
	uint32_t ww = vScreenSize.X * vScale;
	uint32_t wh = vScreenSize.Y * vScale;
	float ratio = (float)ww / (float)wh;

	vViewSize.X = (uint32_t)vWinSize.X;
	vViewSize.Y = (uint32_t)((float)vViewSize.X/ratio);

	if (vViewSize.Y > vWinSize.Y){
		vViewSize.Y = vWinSize.Y;
		vViewSize.X = (uint32_t)((float)vViewSize.Y * ratio);
	};
	vViewPos.X = (vWinSize.X-vViewSize.X)/2;
	vViewPos.Y = (vWinSize.Y-vViewSize.Y)/2;
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
	isRunning = FALSE;
};

void CreateLayer(TextureS *texture){
    layers[textureNumber] = texture; //&texture;
    textureNumber++;
};

rcode WritePixel(uint32_t x, uint32_t y, PixelS pixel, TextureS *texture){
	if ((x<texture->width) && (y<texture->height)){
		texture->TexData[(texture->width)*y+x] = pixel;
		return OK;
	};
	return FAIL;
};
PixelS ReadPixel(uint32_t x, uint32_t y, TextureS *texture){
	if ((x<texture->width) && (y<texture->height)){
		return texture->TexData[(texture->width)*y+x];
	};
	return Pixel(0,0,0,0);
};

rcode DrawPixel(uint32_t x, uint32_t y, PixelS pixel){
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
		return OK;
	};
	return FAIL;
};

PixelS CheckPixel(uint32_t x, uint32_t y){
	x = x*vScale;
	y = y*vScale;
	if ((x<layers[0]->width) && (y<layers[0]->height)){
		return layers[0]->TexData[(layers[0]->width)*y+x];
	};
	return Pixel(0,0,0,0);
};

rcode DrawTexture(uint32_t x, uint32_t y, TextureS *texture){
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
	return OK;
};

void PrepareEngine(){
	PixelS *data = malloc(sizeof(PixelS)*vWinSize.X*vWinSize.Y);
	for (uint32_t i = 0; i < vWinSize.X*vWinSize.Y;i++){
		data[i] = Pixel(255,255,255,255);
	};
	CreateLayer(MakeTex(vWinSize.X,vWinSize.Y, data));
	free(data);
	SetCurrentTargeti(0);
	UpdateTexture(currentTarget);
    printf("[DONE]Default texture created\n");
};

DWORD WINAPI EngineThread(){
	printf("[INIT]Engine thread\n");
	CreateGraphics(isFullscreen,isVsync,vViewPos,vViewSize);
	printf("[DONE]Create graphics\n");
	PrepareEngine();
	printf("[DONE]Engine prep\n");
	if (!OnInitU()){
		isRunning = FALSE;
	};
	while (isRunning){
		CoreUpdate();
	};
	ThreadCleanup();
	return 1;
};

rcode Construct(uint32_t screenW, uint32_t screenH, uint32_t scale, BOOL boolFullscreen, BOOL boolVsync){
	vScreenSize.X = screenW;
	vScreenSize.Y = screenH;
	SetScale(scale);
	vWinSize.X = vScreenSize.X * scale;
	vWinSize.Y = vScreenSize.Y * scale;
	isFullscreen = boolFullscreen;
	isVsync = boolVsync;
    printf("[DONE]Construct finished\n");
	return OK;
};

rcode Start(){
	vMouseState.LButton = FALSE; vMouseState.RButton = FALSE; vMouseState.MButton = FALSE; 
	vMousePos.X = 0; vMousePos.Y = 0;
    vector2d wp;
    wp.X = 50;
    wp.Y = 50;
	CreateWindowPane(FALSE,wp,vWinSize);
	UpdateWinSize(vWinSize.X,vWinSize.Y);
	isRunning = TRUE;
	engineThread = CreateThread(NULL,0,*EngineThread,NULL,0,NULL);
	StartSystemLoop();
	if (engineThread != NULL) {WaitForSingleObject(engineThread,INFINITE);};
	printf("[DONE]Start ended\n");
	return OK;
};

void CoreUpdate(){
	ClearBuffer(Pixel(0,0,0,255),TRUE);
	if (!OnUpdateU()){
		isRunning = FALSE;
	};
	UpdateViewportEngine(vViewPos.X,vViewPos.Y,vViewSize.X,vViewSize.Y);
	ClearBuffer(Pixel(0,0,0,255),TRUE);
	PrepareDrawing();
	ApplyTexture(currentTarget->id);
	UpdateTexture(currentTarget);
	DrawLayer(Pixel(255,255,255,255));
	DisplayFrame();

};

void UpdateWinSize(uint32_t x, uint32_t y){
	vWinSize.X = x;
	vWinSize.Y = y;
	UpdateViewport();
};

void SetScale(uint32_t scale){
	vScale = scale;
};

uint32_t ScreenWidth(){
	return vScreenSize.X;
};

uint32_t ScreenHeight(){
	return vScreenSize.Y;
};

void SetCurrentTargeti(uint32_t index){
	currentTarget = layers[index];
};

void SetCurrentTargetp(TextureS *texture){
	currentTarget = texture;
};

void MouseUpdate(uint32_t button,BOOL state){
	switch (button){
	case 0: vMouseState.LButton = state; break;
	case 1: vMouseState.RButton = state; break;
	case 2: vMouseState.MButton = state; break;
	default: break;
	};
};

void MouseUpdatePos(int32_t x,int32_t y){
	vMousePos.X = x/vScale;
	vMousePos.Y = y/vScale;
};

void Set_Focus(BOOL state){
	isFocused = state;
};

void SetKeyState(BOOL state, int key){
	vKBCache = key;
};

BOOL OnInitU(){
	PixelS *data1 = malloc(sizeof(PixelS)*20*20);
	for (uint32_t i = 0; i < 20*20;i++){
		data1[i] = Pixel(255,100,255,255);
	};
	CreateLayer(MakeTex(20, 20, data1));
	free(data1);
	DrawTexture(1,1,layers[1]);
	DrawPixel(2,2,Pixel(255,255,90,255));
	DrawPixel(2,3,Pixel(255,99,90,255));
	return TRUE;
}; 
BOOL OnUpdateU(){
	if (vKBCache==key_W){
		printf("Pressed W");
		vKBCache = 0;
	}
	return TRUE;
};
int main(){
    Construct(1929,1080,1,TRUE,FALSE);
	Start();
}


