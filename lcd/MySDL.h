#ifndef  _MYSDL_H_
#define  _MYSDL_H_

#ifdef __cplusplus
    //#include <cstdlib>
#else
    #include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


#include <SDL/SDL.h>


SDL_Surface *  MySDL_Init( Uint16 width, Uint16 height ,Uint32 color);
void  MySDLSetTitleAndIcon(const char *title, const char *icon);
void MySDL_UpdateScreen(SDL_Surface * screen);
void MySDLPutPixel24_nolock(SDL_Surface * surface, int x, int y, Uint32 color);
Uint32  MySDLGetPixel24_nolock(SDL_Surface * surface, int x, int y);
SDL_Surface* MySDLLoadBmp(SDL_Surface* screen, const char * BmpDirectory);
void MySDLDrawBmp(SDL_Surface* screen,SDL_Surface* bmp, Uint16 x, Uint16 y);
void MySDLFreeSurface(SDL_Surface *bmp);
void MySDLDrawLine(SDL_Surface * screen,int x1,int y1,int x2,int y2,unsigned long LiColor);
void MySDLDrawCir(SDL_Surface * screen,Uint16 x,Uint16 y,Uint16 R,Uint16 Color,Uint16 Width);
void DisWin(SDL_Surface * screen,int x,int y,int width,int heigh,const char *s);
void DisCharAntiAis(unsigned short int A[64][64],unsigned short int B[64][64],SDL_Surface*s,uint16_t x,uint16_t y,uint16_t height,uint16_t width,uint16_t wColor,uint16_t bColor,const uint8_t *pChar);
void DisChar(SDL_Surface*s,uint16_t x,uint16_t y,uint16_t height,uint16_t width,uint16_t wColor,uint16_t bColor,const uint8_t *pChar);
void MySDLPutPixel24_nolockX4(SDL_Surface * surface, int x, int y, uint32_t color);
void MySDLPutPixel16_nolockX4(SDL_Surface * surface, int x, int y, unsigned short color);
void MySDLDisWin(SDL_Surface *s,int w,int h,uint32_t color);
void MySDLPutPixel16_nolock(SDL_Surface * surface, int x, int y, unsigned short color);

#ifdef __cplusplus
};
#endif

#endif // _MYSDL_H_
