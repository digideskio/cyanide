/**
  * GreenPois0n Cynanide - framebuffer.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "commands.h"

#define COLOR_WHITE 0xffffff
#define COLOR_BLACK 0x0

typedef struct Font {
	unsigned int width;
	unsigned int height;
	unsigned char data[];
} Font;

typedef struct fb_info {
	unsigned int fbuffer;
	unsigned int width;
	unsigned int height;
	unsigned int linelen;
	unsigned int unk1;
	unsigned int unk2;
	unsigned int unk3;
	void (*plot)(struct fb_info* fbdata, unsigned int x, unsigned int y, unsigned int color);
	unsigned int (*get_pixel)(struct fb_info* fbdata, unsigned int x, unsigned int y);
	void (*hline)(struct fb_info* fbdata, unsigned int x, unsigned int y, unsigned int length, unsigned int color);
	void (*vline)(struct fb_info* fbdata, unsigned int x, unsigned int y, unsigned int length, unsigned int color);
} fb_info;
	

extern Bool gFbHasInit;

int fb_init();
int fb_cmd(int argc, CmdArg* argv);
int fbimg_cmd(int argc, CmdArg* argv);
void fb_clear();
void fb_display_text(Bool option);
void fb_set_loc(unsigned int x, unsigned int y);
fb_info* find_fbinfo();
unsigned int fb_get_x();
unsigned int fb_get_y();
unsigned int fb_get_width();
unsigned int fb_get_height();
void fb_putc(int c);
void fb_println(const char* str);
void fb_print(const char* str);
void fb_print_force(const char* str);
void fb_draw_image(unsigned int* image, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
void fb_capture_image(unsigned int* image, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
unsigned int* fb_load_image(const char* data, unsigned int len, unsigned int* width, unsigned int* height, unsigned int alpha);

#endif /* FRAMEBUFFER_H */
