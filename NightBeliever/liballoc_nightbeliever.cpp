#include "NightBeliever.hpp"

int liballoc_lock() {
	//pthread_mutex_lock( &mutex );
	return 0;
}

int liballoc_unlock() {
	//pthread_mutex_unlock( &mutex );
	return 0;
}

void* liballoc_alloc(int pages) {
	return map((void *) 0, pages);
}

int liballoc_free( void* ptr, int pages )
{
	unmap(ptr, pages);
	return 0;
}
