/*
	avr_spi.c

	Copyright 2008, 2009 Michel Pollet <buserror@gmail.com>
	Modified 2020 by VintagePC <https://github.com/vintagepc> to support clock divisors

 	This file is part of simavr.

	simavr is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	simavr is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with simavr.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "avr_spi.h"
#include "../../lcd/MySDL.h"


static avr_cycle_count_t avr_spi_raise(struct avr_t * avr, avr_cycle_count_t when, void * param)
{
	avr_spi_t * p = (avr_spi_t *)param;

	if (avr_regbit_get(avr, p->spe)) {
		// in master mode, any byte is sent as it comes..
		if (avr_regbit_get(avr, p->mstr)) {
			avr_raise_interrupt(avr, &p->spi);
			avr_raise_irq(p->io.irq + SPI_IRQ_OUTPUT, avr->data[p->r_spdr]);
		}
	}
	return 0;
}

static uint8_t avr_spi_read(struct avr_t * avr, avr_io_addr_t addr, void * param)
{
	avr_spi_t * p = (avr_spi_t *)param;
	uint8_t v = p->input_data_register;
	p->input_data_register = 0;
	avr_regbit_clear(avr, p->spi.raised);
//	printf("avr_spi_read = %02x\n", v);
	return v;
}


extern SDL_Surface *screen;


static void avr_spi_write(struct avr_t * avr, avr_io_addr_t addr, uint8_t v, void * param)
{

	static const uint8_t _avr_spi_clkdiv[4] = {4,16,64,128};
	static int x,y;
	static ssd1306_cmd_count = 0;
	avr_spi_t * p = (avr_spi_t *)param;

	if (addr == p->r_spdr) {
		/* Clear the SPIF bit. See ATmega164/324/644 manual, Section 18.5.2. */
		avr_regbit_clear(avr, p->spi.raised);
        //printf("SPI is writting %x  =================================================================\n",v);
		avr_core_watch_write(avr, addr, v);

        /*===================================== SSD1306显示模拟 =====================================================*/
		/* 如果是命令 */
		if((avr->data[0x2b] & (unsigned char)(1<<4)) == 0x00)
        {
            ssd1306_cmd_count++;
            if(ssd1306_cmd_count >=50 )  //过滤掉前50条初始化SSD1306的CMD
            {
                //printf("SPI is writting CMD%x  =================================================================\n",v);
                /* 获取X,Y */
                if( v <= 0xf && v>= 0x00)
                {
                    x = (v & 0x0f);
                    printf("x=%x   ",x);
                }
                else if((v & 0x10) == 0x10)
                {
                    x = x | ((v & 0x0f)<<4);
                }
                else if((v & 0xb0) == 0xb0)
                {
                    y = v & 0x0f;
                }
                else
                {

                }
            }
        }
        /* 如果是数据 */
        if((avr->data[0x2b] & (unsigned char)(1<<4)))
        {
            //printf("SPI is writting DATA%x  =================================================================\n",v);
            for(int i=0;i<8;i++)
            {
                if(v&(1<<i))
                    MySDLPutPixel24_nolockX4(screen,x,y*8+i,0xffffff);
                else
                    MySDLPutPixel24_nolockX4(screen,x,y*8+i,0x000000);
            }
            x++;
            if(x >= 128)
            {
                x = 0;
                y++;
            }
            if(y >= 8)
            {
                x=0;
                y=0;
                MySDL_UpdateScreen(screen);
            }
        }
        /*==========================================================================================*/



		uint16_t clock_shift = _avr_spi_clkdiv[avr->data[p->r_spcr]&0b11];
		// If master && 2X, double rate (half divisor)
		if (avr_regbit_get(avr, p->mstr) && avr_regbit_get(avr, p->spr[2]))
			clock_shift>>=1;

		// We can wait directly in clockshifts, it is a divisor, so /4 means 4 avr cycles to clock out one bit.
		avr_cycle_timer_register(avr, clock_shift<<3, avr_spi_raise, p); // *8 since 8 clocks to a byte.
	}
}

static void avr_spi_irq_input(struct avr_irq_t * irq, uint32_t value, void * param)
{
	avr_spi_t * p = (avr_spi_t *)param;
	avr_t * avr = p->io.avr;

	// check to see if receiver is enabled
	if (!avr_regbit_get(avr, p->spe))
		return;

	// double buffer the input.. ?
	p->input_data_register = value;
	avr_raise_interrupt(avr, &p->spi);

	// if in slave mode,
	// 'output' the byte only when we received one...
	if (!avr_regbit_get(avr, p->mstr)) {
		avr_raise_irq(p->io.irq + SPI_IRQ_OUTPUT, avr->data[p->r_spdr]);
	}
}


void avr_spi_reset(struct avr_io_t *io)
{
	avr_spi_t * p = (avr_spi_t *)io;
	avr_irq_register_notify(p->io.irq + SPI_IRQ_INPUT, avr_spi_irq_input, p);
}


static const char * irq_names[SPI_IRQ_COUNT] = {
	[SPI_IRQ_INPUT] = "8<in",
	[SPI_IRQ_OUTPUT] = "8<out",
};


static	avr_io_t	_io = {
	.kind = "spi",
	.reset = avr_spi_reset,
	.irq_names = irq_names,
};


void avr_spi_init(avr_t * avr, avr_spi_t * p)
{
	p->io = _io;

	avr_register_io(avr, &p->io);
	avr_register_vector(avr, &p->spi);
	// allocate this module's IRQ
	avr_io_setirqs(&p->io, AVR_IOCTL_SPI_GETIRQ(p->name), SPI_IRQ_COUNT, NULL);

	avr_register_io_write(avr, p->r_spdr, avr_spi_write, p);
	avr_register_io_read(avr, p->r_spdr, avr_spi_read, p);
}
