#pragma once

struct import_data {
	uint32_t crc;
	uintptr_t address;
};

#define MAX_IMPORTS 64

void* find_import(uint32_t import_crc);

#define I(f) ((decltype(&f))find_import(HASH(#f)))
#define NT_IMPORT(f) ((f##_fn)find_import(HASH(#f)))