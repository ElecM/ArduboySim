#include "MySDL.h"




SDL_Surface *  MySDL_Init( Uint16 width, Uint16 height ,Uint32 color)
{
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return  NULL;
    }
    atexit(SDL_Quit);

    SDL_Surface* screen = SDL_SetVideoMode(width, height, 24,SDL_HWSURFACE|SDL_DOUBLEBUF);

    if ( !screen )
    {
        printf("Unable to set 640x480 video: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Rect rec;
    rec.x = 0;
    rec.y = 0;
    rec.w = width;
    rec.h = height;
    SDL_FillRect(screen, &rec, color);
    return screen;
}



void  MySDLSetTitleAndIcon(const char *title, const char *icon)
{
    SDL_WM_SetCaption(title,NULL);
    SDL_WM_SetIcon(SDL_LoadBMP(icon),NULL);
}



void MySDL_UpdateScreen(SDL_Surface * screen)
{
    SDL_Flip(screen);
}







void MySDLPutPixel24_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
    if(x >= 0 && x <= surface->w && y>=0 && y<= surface->h)
    {
        Uint8 * pixel = (Uint8*)surface->pixels;
        pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 3);
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        pixel[2] = (color >> 24) & 0xFF;
        pixel[1] = (color >> 16)  & 0xFF;
        pixel[0] = (color >> 8)  & 0xFF;
        #else
        pixel[0] = color & 0xFF;
        pixel[1] = (color >> 8) & 0xFF;
        pixel[2] = (color >> 16) & 0xFF;
        #endif
    }

}


void MySDLPutPixel16_nolock(SDL_Surface * surface, int x, int y, unsigned short color)
{

    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 3);

    Uint8 b = ((color & 0xF800)>>11);
    Uint8 g = ((color & 0x07E0)>>5);
    Uint8 r = (color & 0x001F);

    *(pixel+2)=b*255/31;
    *(pixel+1)=g*255/63;
    *(pixel+0)=r*255/31;
}





Uint32  MySDLGetPixel24_nolock(SDL_Surface * surface, int x, int y)
{
    Uint32 color=0;
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 3);
    color = pixel[0] +  (pixel[1]<<8) + (pixel[2]<<16);
    return color;
}

SDL_Surface* MySDLLoadBmp(SDL_Surface* screen, const char *  BmpDirectory)
{
    SDL_Surface* bmp = SDL_LoadBMP(BmpDirectory);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        return NULL;
    }
    return bmp;
}

void MySDLDrawBmp(SDL_Surface* screen,SDL_Surface* bmp, Uint16 x, Uint16 y)
{
   SDL_Rect dstrect;
   dstrect.x = x;
   dstrect.y = y;
   SDL_BlitSurface(bmp, 0, screen, &dstrect);
}

void MySDLFreeSurface(SDL_Surface *bmp)
{
    SDL_FreeSurface(bmp);
}


void MySDLDrawLine(SDL_Surface * screen,int x1,int y1,int x2,int y2,unsigned long LiColor)
{
	int t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向
	else if(delta_x==0)incx=0;//垂直线
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//水平线
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//画线输出
	{

	    if(uRow >=0 && uRow <= screen->w && uCol>=0 && uCol<= screen->h)
        {
           MySDLPutPixel16_nolock(screen, uRow, uCol, LiColor);
        }
        xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}



void MySDLDrawCir(SDL_Surface * screen,Uint16 x,Uint16 y,Uint16 R,Uint16 Color,Uint16 Width)
{
   int D=3-2*R;
   int xc=0,yc=R,ys=0,Wi;
   while(xc<=yc)
   {
       Wi=yc-Width;
	   for(ys=Wi;ys<=yc;ys++)
	  {
	   MySDLPutPixel24_nolock(screen,xc+x,ys+y,Color);
       MySDLPutPixel24_nolock(screen,xc+x,-ys+y,Color);
	   MySDLPutPixel24_nolock(screen,-xc+x,ys+y,Color);
       MySDLPutPixel24_nolock(screen,-xc+x,-ys+y,Color);
	   MySDLPutPixel24_nolock(screen,ys+x,xc+y,Color);
       MySDLPutPixel24_nolock(screen,ys+x,-xc+y,Color);
	   MySDLPutPixel24_nolock(screen,-ys+x,xc+y,Color);
       MySDLPutPixel24_nolock(screen,-ys+x,-xc+y,Color);
	   }
	 if(D>=0)
	 {
	    yc--;
		D=D+4*(xc-yc)+10;
	 }
	 else
	    D=D+4*xc+6;
	    xc++;
   }

}



void DisWin(SDL_Surface * screen,int x,int y,int width,int heigh,const char *s)
{
    for(int i=0;i<20;i++)
    {
      MySDLDrawLine(screen,x,y+i,x+width,y+i,0xff00);
    }

    for(int i=20;i<heigh;i++)
    {
      MySDLDrawLine(screen,x,y+i,x+width,y+i,0xffff);
    }

    MySDLDrawLine(screen,x,y,x+width,y,0x00);
    MySDLDrawLine(screen,x,y+heigh,x+width,y+heigh,0x00);
    MySDLDrawLine(screen,x,y,x,y+heigh,0x00);
    MySDLDrawLine(screen,x+width,y,x+width,y+heigh,0x00);

    MySDLDrawLine(screen,x+width-15,y+2,x+width,y+17,0x00);
    MySDLDrawLine(screen,x+width-15,y+17,x+width,y+2,0x00);
}




void DisCharAntiAis(unsigned short int A[64][64],unsigned short int B[64][64],SDL_Surface*s,uint16_t x,uint16_t y,uint16_t height,uint16_t width,uint16_t wColor,uint16_t bColor,const uint8_t *pChar)
{
	uint16_t  i;
    uint16_t j,m;
     for(i=0;i<(height*width);i+=8)
	  { m=*pChar;
	    for(j=0;j<8;j++)
	     { if(((m&0x80)>>7)==1)
			    {
                //MySDLPutPixel16_nolock(s,x+i%width+j,y+i/width,wColor);
                 B[((i%width)+j)][(i/width)]=wColor;
			    }
			    else
                {
                //MySDLPutPixel16_nolock(s,x+i%width+j,y+i/width,bColor);
                  B[((i%width)+j)][(i/width)]=bColor;
                }
			    m<<=1;
	      }
       pChar++;
	  }

     for(i=0;i<32;i++)
      for(j=0;j<32;j++)
      {
        A[i*2][j*2] = B[i][j];
        A[i*2+1][j*2] = B[i][j];
        A[i*2][j*2+1] = B[i][j];
        A[i*2+1][j*2+1] = B[i][j];
      }

    for(i=1;i<63;i++)
     for(j=1;j<63;j++)
     {
        B[i][j] =  (A[i-1][j]+A[i+1][j]+A[i][j+1]+A[i][j-1]+A[i][j]+
                   A[i+1][j+1]+A[i-1][j-1]+A[i-1][j+1]+A[i+1][j-1])/9;

     }


      for(i=0;i<32;i++)
      for(j=0;j<32;j++)
      {
        A[i][j] = (B[i*2][j*2]+A[i*2+1][j*2]+A[i*2][j*2+1]+A[i*2+1][j*2+1])/4;
      }


     for(i=0;i<32;i++)
      for(j=0;j<32;j++)
      {
        MySDLPutPixel16_nolockX4(s,x+i,y+j,A[i][j]);
      }
}



void DisChar(SDL_Surface*s,uint16_t x,uint16_t y,uint16_t height,uint16_t width,uint16_t wColor,uint16_t bColor,const uint8_t *pChar)
{
	uint16_t  i;
    uint16_t j,m;
     for(i=0;i<(height*width);i+=8)
	  { m=*pChar;
	    for(j=0;j<8;j++)
	     { if(((m&0x80)>>7)==1)
			    {
                    MySDLPutPixel16_nolockX4(s,x+i%width+j,y+i/width,wColor);
			    }
			    else
                {
                    MySDLPutPixel16_nolockX4(s,x+i%width+j,y+i/width,bColor);
                }
			    m<<=1;
	      }
       pChar++;
	  }

}


void MySDLPutPixel16_nolockX4(SDL_Surface * surface, int x, int y, unsigned short color)
{
    unsigned int i,j;
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
    {
        MySDLPutPixel16_nolock(surface,x*4+i,y*4+j,color);
    }
}

void MySDLPutPixel24_nolockX4(SDL_Surface * surface, int x, int y, uint32_t color)
{
    unsigned int i,j;
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
    {
        if(x >= 0 && x <= surface->w && y>=0 && y<= surface->h)
        {
           MySDLPutPixel24_nolock(surface,x*4+i,y*4+j,color);
        }
    }
}

void MySDLDisWin(SDL_Surface *s,int w,int h,uint32_t color)
{
    for(int i=0;i<w;i++)
     for(int j=0;j<h;j++)
    {
        MySDLPutPixel24_nolock(s,i,j,color);
    }
}

