#include "Zookeeper.hpp"
#include <libgen.h>

retro_environment_t env;

uint32_t load_multiboot(uint32_t *header) {
	assert(header[1] & 0x10000);

	uint8_t *rel = (uint8_t *) header;

	uint32_t memsize = header[6] - KBASE;
	if(memsize & PAGE_MASK)
		memsize = (memsize & ~PAGE_MASK) + PAGE_SIZE;
	assert(memsize <= KRAM_SIZE);

	memcpy(box->cpu->kmem + (header[4] - KBASE), rel + (header[4] - header[3]), memsize);
	memset(box->cpu->kmem + (header[5] - KBASE), 0, header[6] - header[5]);
	box->cpu->map_pages(KBASE, KBASE, memsize / PAGE_SIZE);

	return header[7];
}

uint32_t load_kernel() {
	char *path;
	env(RETRO_ENVIRONMENT_GET_LIBRETRO_PATH, &path);
	auto temp = strdup(path);
	string full = dirname(temp);
	full += "/nightbeliever.krnl";
	FILE *fp = fopen(full.c_str(), "r");
	delete temp;
	fseek(fp, 0, SEEK_END);
	uint32_t size = ftell(fp);
	uint8_t *buf = new uint8_t[size];
	fseek(fp, 0, SEEK_SET);
	fread(buf, 1, size, fp);

	uint32_t *seek = (uint32_t *) buf;
	uint32_t *end = (uint32_t *) (buf + size);
	uint32_t entry;
	do {
		if(*seek == 0x1BADB002 && seek[2] == -(seek[0] + seek[1])) {
			if((entry = load_multiboot(seek)) != -1)
				break;
		}
	} while(++seek != end);

	if(seek == end) {
		cout << "Reached EOF before finding Multiboot header!" << endl;
	}

	delete[] buf;
	return entry;
}

RETRO_API unsigned retro_api_version() {
	return RETRO_API_VERSION;
}

void intHandler(int _) {
	break_in();
}

RETRO_API void retro_init() {
	// signal(SIGINT, intHandler);
}

RETRO_API void retro_deinit() {
}

RETRO_API void retro_get_system_info(struct retro_system_info *info) {
	info->library_name = "Zookeeper";
	info->library_version = "Pre-Alpha 1";
	info->valid_extensions = "xbe|iso";
	info->need_fullpath = true;
	info->block_extract = false;
}

RETRO_API bool retro_load_game(const struct retro_game_info *game) {
	new Box;

	box->kernel_entry = load_kernel();
	auto xbe = new Xbe((char *) game->path);
	xbe->LoadImage();
	return true;
}

RETRO_API bool retro_load_game_special(
  unsigned game_type,
  const struct retro_game_info *info, size_t num_info
) {
	return false;
}

RETRO_API void retro_unload_game() {
	delete box;
}

RETRO_API void retro_run() {
	box->cpu->run(box->kernel_entry);
	box->kernel_entry = -1; // Ensure we don't just keep going back here
}

RETRO_API void retro_get_system_av_info(struct retro_system_av_info *info) {
	info->geometry.base_width = 1280;
	info->geometry.base_height = 720;

	info->timing.fps = 30;
	info->timing.sample_rate = 44100;
}

RETRO_API void retro_set_environment(retro_environment_t cb) {
	env = cb;
}

RETRO_API void retro_set_video_refresh(retro_video_refresh_t cb) {

}

RETRO_API void retro_set_audio_sample(retro_audio_sample_t cb) {

}

RETRO_API void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {

}

RETRO_API void retro_set_input_poll(retro_input_poll_t cb) {

}

RETRO_API void retro_set_input_state(retro_input_state_t cb) {

}

RETRO_API void retro_set_controller_port_device(unsigned port, unsigned device) {
}

RETRO_API void retro_reset() {
}

RETRO_API size_t retro_serialize_size(void) {
	return 0;
}

RETRO_API bool retro_serialize(void *data, size_t size) {
	return false;
}
RETRO_API bool retro_unserialize(const void *data, size_t size) {
	return false;
}

RETRO_API void retro_cheat_reset() {
}
RETRO_API void retro_cheat_set(unsigned index, bool enabled, const char *code) {
}

/* Gets region of game. */
RETRO_API unsigned retro_get_region(void) {
	return 0;
}

/* Gets region of memory. */
RETRO_API void *retro_get_memory_data(unsigned id) {
	return NULL;
}
RETRO_API size_t retro_get_memory_size(unsigned id) {
	return 0;
}
