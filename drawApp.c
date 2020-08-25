#include <byMe/graphicsEngine.h>
#include <winuser.h>

PixelType pp;
bool OnUserInitialize(){
    PixelType pp = Pixel(80,0,0,255);
    LoadImageFromPath("C:\\Users\\dfgdf\\Desktop\\test1.jpg");
    TextureDraw(1,1,tTexturePack[0]);
    LineDraw(20,20,200,175,pp);
    return true;
};
bool OnUserUpdate(){
    if (biKeyboard[key_W].pressed) pp = Pixel(0,0,250,255);  
    if (biKeyboard[key_S].pressed){
        printf("S");
        pp = Pixel(0,0,0,255);
    };
    if (biMouse[0].held) PixelWrite(vMousePosition.X, vMousePosition.Y, pp, tCurrentDrawTarget);
    return true;
};
int main(){
    Construct(500,600,3,false,false);
    Start();
};