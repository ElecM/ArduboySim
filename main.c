#ifdef __cplusplus
    //#include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <SDL/SDL.h>
#include "lcd/MySDL.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "run_avr.c"
#include "sim_mega32u4.h"
#include "avr_ioport.h"
#include "heap.h"


unsigned short int avr_flash[32767+4]={0};  /* atmega32u4的flash是 32K */
unsigned char      avr_sram[3*1024]={0};    /* atmega32u4的sram是 3K */

SDL_Surface *screen = NULL;


SDL_Event event;

int main ( int argc, char** argv )
{
   MemAllocInit();
   screen = MySDL_Init(128*4,64*4,0x00ff);
   MySDL_UpdateScreen(screen);

   avr_t *avr=NULL;
    unsigned long filesize;

	FILE* fp = fopen("game.bin", "rb");

	if(!fp)
    {
        printf("game open fail");
    }
    fseek(fp,0,SEEK_END);//定位到文件的最后面
    filesize = ftell(fp);
    fseek(fp,0,SEEK_SET);//定位到文件的最开始
    fread(avr_flash, filesize, 1, fp);
    printf("%d  ",filesize);
	fclose(fp);


    /*
    printf("game[]=");
    for(int i=0;i<filesize;i++)
    {
        printf("0x%x,",*((uint8_t*)avr_flash+i));
        if(i%100==0)
            printf("\n");
    }

    printf("\n\n\n");
    */


	avr = mega32u4.make();


	avr_init(avr,avr_flash,avr_sram);
	avr->pc    = 0;


	printf("mem used=%d Bytes   ",MemGetUsedBytes());


    int game_state;
    // program main loop
    bool done = false;
    uint32_t instructions = 20;
    avr_core_watch_write(avr, 0x23, (1<<4));
    avr_core_watch_write(avr, 0x2C, (1<<6));
    avr_core_watch_write(avr, 0x2F, 0);
    long i=0;
    while (!done)
    {
        instructions = 20;
        while(instructions --)
        {

            game_state = avr_run(avr);
            if (game_state == cpu_Done || game_state == cpu_Crashed)
			   done = true;
        }



        // message processing loop
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    if (event.key.keysym.sym == SDLK_DOWN)
                    {
                       // printf("DOWN-key before:%x",avr->data[0x31]);
                        //avr->io[0x0E].w.c(avr,0x2E,(1<<6),avr->io[0x0E].w.param);
                        uint8_t dat = avr_core_watch_read(avr,0x2F);
                        avr_core_watch_write(avr, 0x2F, dat&(~(1<<7)));
                    }

                    if (event.key.keysym.sym == SDLK_UP)
                    {
                       // printf("DOWN-key before:%x",avr->data[0x31]);
                        //avr->io[0x0E].w.c(avr,0x2E,(1<<6),avr->io[0x0E].w.param);
                        uint8_t dat = avr_core_watch_read(avr,0x2F);
                        avr_core_watch_write(avr, 0x2F, dat&(~(1<<4)));
                    }
                    if (event.key.keysym.sym == SDLK_RIGHT)
                    {
                       // printf("DOWN-key before:%x",avr->data[0x31]);
                        //avr->io[0x0E].w.c(avr,0x2E,(1<<6),avr->io[0x0E].w.param);
                        avr_core_watch_write(avr, 0x2F, 1<<5);
                    }
                    if (event.key.keysym.sym == SDLK_LEFT)
                    {
                       // printf("DOWN-key before:%x",avr->data[0x31]);
                        //avr->io[0x0E].w.c(avr,0x2E,(1<<6),avr->io[0x0E].w.param);
                        uint8_t dat = avr_core_watch_read(avr,0x2F);
                        avr_core_watch_write(avr, 0x2F, dat&(~(1<<6)));
                    }
                    if (event.key.keysym.sym == SDLK_1 || event.key.keysym.sym == SDLK_a)
                    {
                       // printf("DOWN-key before:%x",avr->data[0x31]);
                        //avr->io[0x0E].w.c(avr,0x2E,(1<<6),avr->io[0x0E].w.param);
                        uint8_t dat = avr_core_watch_read(avr,0x2c);
                        avr_core_watch_write(avr, 0x2C, dat&(~(1<<6)));
                    }
                    if (event.key.keysym.sym == SDLK_2 || event.key.keysym.sym == SDLK_d)
                    {
                       // printf("DOWN-key before:%x",avr->data[0x31]);
                        //avr->io[0x0E].w.c(avr,0x2E,(1<<6),avr->io[0x0E].w.param);
                        uint8_t dat = avr_core_watch_read(avr,0x23);
                        avr_core_watch_write(avr, 0x23, dat&(~(1<<4)));
                    }

                    break;
                }

            case SDL_KEYUP:
                {
                    if (event.key.keysym.sym == SDLK_1 || event.key.keysym.sym == SDLK_a)
                    {
                        avr_core_watch_write(avr, 0x2C, (1<<6));
                    }
                    if (event.key.keysym.sym == SDLK_2 || event.key.keysym.sym == SDLK_d)
                    {
                       avr_core_watch_write(avr, 0x23, (1<<4));
                    }
                    if(event.key.keysym.sym == SDLK_DOWN ||
                       event.key.keysym.sym == SDLK_UP ||
                       event.key.keysym.sym == SDLK_RIGHT ||
                       event.key.keysym.sym == SDLK_LEFT)
                    {
                        avr_core_watch_write(avr, 0x2F, 0xff);
                    }

                    break;
                }

            } // end switch
        } // end of message processing

        // finally, update the screen :)
    } // end main loop
    avr_terminate(avr);
    // free loaded bitmap


    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
