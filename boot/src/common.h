/*
** Copyright (C) 2022-2023 Arseny Vakhrushev <arseny.vakhrushev@me.com>
**
** This firmware is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This firmware is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this firmware. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/wwdg.h>
#include <libopencm3/stm32/crc.h>
#include "config.h"

#define CNT(clk, rate) (((clk) + ((rate) >> 1)) / (rate)) // Rate-based clock count

extern char _rom[], _rom_end[], _ram_end[]; // Linker exports

void init(void);
void initio(void);

int recv(char *buf, int len);
void send(const char *buf, int len);
int recvval(void);
void sendval(int val);
int recvdata(char *buf);
void senddata(const char *buf, int len);

uint32_t crc32(const char *buf, int len);
int writeflash(char *dst, const char *src, int len) __attribute__((__long_call__));
void updateflash(char *dst, const char *src, int len) __attribute__((__long_call__, __noreturn__));