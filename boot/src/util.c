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

#include "common.h"

uint32_t crc32(const char *buf, int len) {
	uint32_t *val = (uint32_t *)buf;
	len >>= 2;
	CRC_CR = CRC_CR_RESET | CRC_CR_REV_IN_WORD | CRC_CR_REV_OUT;
	while (len--) CRC_DR = *val++;
	return ~CRC_DR;
}

__attribute__((__section__(".ramtext")))
int writeflash(char *dst, const char *src, int len) {
	FLASH_KEYR = FLASH_KEYR_KEY1;
	FLASH_KEYR = FLASH_KEYR_KEY2;
	FLASH_CR = FLASH_CR_PER;
	uint32_t ofs = ((uint32_t)dst + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1); // Align upward to page boundary
	for (int pos = 0; pos < len; pos += PAGE_SIZE) { // Erase pages
#ifdef STM32G0
		FLASH_CR = FLASH_CR_PER | FLASH_CR_STRT | ((ofs + pos - (uint32_t)_rom) / PAGE_SIZE) << FLASH_CR_PNB_SHIFT;
#else
		FLASH_AR = ofs + pos;
		FLASH_CR = FLASH_CR_PER | FLASH_CR_STRT;
#endif
		while (FLASH_SR & FLASH_SR_BSY);
	}
	FLASH_CR = FLASH_CR_PG;
#ifdef STM32G0
	for (int pos = 0; pos < len; pos += 8) { // Write data
		*(uint32_t *)(dst + pos) = *(uint32_t *)(src + pos);
		*(uint32_t *)(dst + pos + 4) = *(uint32_t *)(src + pos + 4);
#else
	for (int pos = 0; pos < len; pos += 2) { // Write data
		*(uint16_t *)(dst + pos) = *(uint16_t *)(src + pos);
#endif
		while (FLASH_SR & FLASH_SR_BSY);
	}
	FLASH_CR = FLASH_CR_LOCK;
	for (int pos = 0; pos < len; pos += 4) { // Check written data
		if (*(uint32_t *)(dst + pos) != *(uint32_t *)(src + pos)) return 0;
	}
	return 1;
}

__attribute__((__section__(".ramtext")))
void updateflash(char *dst, const char *src, int len) {
	writeflash(dst, src, len); // Can't really do anything in case of error
	SCB_AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ; // Reboot
	for (;;); // Never return
}