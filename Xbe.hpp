#pragma once

#include "Zookeeper.hpp"
#include <cstdint>
#include <cstdio>

class Xbe {
public:
	FILE *fp;
	uint32_t file_size;
	uint8_t *file_data;
	uint8_t *full_header;
	XbeHeader_t *header;
	XbeSection_t *sections;
	Xbe(char *fn);
	uint32_t LoadImage();
};
