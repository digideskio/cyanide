/**
  * GreenPois0n Cynanide - functions.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "lock.h"
#include "common.h"
#include "commands.h"
#include "functions.h"

static unsigned char push = 0xB5;
static unsigned char push_r7_lr[] = { 0x80, 0xB5 };
static unsigned char push_r4_r7_lr[] = { 0x90, 0xB5 };
static unsigned char push_r4_to_r7_lr[] = { 0xF0, 0xB5 };
static unsigned char push_r4_r5_r7_lr[] = { 0xB0, 0xB5 };

static unsigned char* functions[][3] = {
	{ "aes_crypto_cmd", "aes_crypto_cmd", push_r4_r7_lr },
	{ "free", "heap_panic",  push_r4_to_r7_lr },
	{ "fs_mount", "fs_mount", push_r4_to_r7_lr },
	{ "cmd_ramdisk", "Ramdisk too large", push_r4_r5_r7_lr },
	{ "cmd_go", "jumping into image", push_r7_lr },
	{ "image_load", "image validation failed but untrusted images are permitted", push_r4_to_r7_lr },
	{ "fsboot", "root filesystem mount failed", push_r4_to_r7_lr },
	{ "kernel_load", "rd=md0", push_r4_to_r7_lr },
	{ "task_yield", "task_yield", push_r4_r5_r7_lr },
	{ "default_block_write", "no reasonable default block write routine", push_r7_lr },
	{ "populate_images", "image %p: bdev %p type %c%c%c%c offset 0x%x", push_r4_r5_r7_lr },
	{ "uart_read", "uart_read", push_r4_to_r7_lr },
	{ "uart_write", "uart_write", push_r4_to_r7_lr },
	{ "task_create", "task_create", push_r4_to_r7_lr },
	{ "task_exit", "task_exit", push_r4_to_r7_lr },
	{ NULL, NULL, NULL }
};

unsigned int find_reference(unsigned int dataaddr, unsigned int base, unsigned int size, unsigned int address) {
	unsigned char* data = (unsigned char*)dataaddr;
	unsigned int i = 0;

	// Find where that offset is referenced
	unsigned int reference = 0;
	for(i = 0; i < size; i++) {
		if(!memcmp(&data[i], &address, 4)) {
			reference = i + base;
			break;
		}
	}
	return reference;
}

unsigned int find_top(unsigned int dataaddr, unsigned int base, unsigned int size, unsigned int offset) {
	unsigned char* data = (unsigned char *)dataaddr;
	// Find the top of that function
	int i;
	unsigned int function = 0;
	for(i=(offset&0xfffffffb)+1;i>0;i-=4) {
		if(data[i] == push) {
			function = base + i;
			break;
		}
	}
	return function;
}

unsigned int find_offset(unsigned int dataaddr, unsigned int base, unsigned int size, unsigned char** what) {
	unsigned char* data = (unsigned char *)dataaddr;
	int i = 0;
	unsigned char* top = what[2];
	unsigned char* name = what[0];
	unsigned char* signature = what[1];
	unsigned int dbase = dataaddr;

	// First find the string
	unsigned int address = find_string(dataaddr, base, size, signature);
	if(address == 0) return NULL;

	// Next find where that string is referenced
	unsigned int reference = find_reference(dataaddr, base, size, address);
	if(reference == 0) return NULL;
	reference -= base;

	// Finally find the top of that function
	unsigned int function = find_top(dataaddr, base, size, reference);

	return function;
}

unsigned int find_string(unsigned int dataaddr, unsigned int base, unsigned int size, const char* name) {
	unsigned char* data = (unsigned char *)dataaddr;
	// Find the string
	int i = 0;
	unsigned int address = 0;
	for(i = 0; i < size; i++) {
		if(!memcmp(&data[i], name, strlen(name))) {
			address = i + base;
			break;
		}
	}
	return address;
}

void* find_function(const char* name, unsigned int target, unsigned int base) {
	int i = 0;
	unsigned int found = 0;
	for(i = 0; i < sizeof(functions); i++) {
		if(!strcmp(functions[i][0], name)) {
			found = find_offset(target, base, 0x40000, functions[i]);
			break;
		}
	}

	return (void*) found;
}
