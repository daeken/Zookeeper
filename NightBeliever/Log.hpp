#pragma once

void log(const char *fmt, ...);

#define bailout(fmt, args...) do { log(__FILE__ " (%i): " fmt, __LINE__, ##args); halt(); } while(0)
