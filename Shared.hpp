#pragma once

#define ONE_KB 1024
#define ONE_MB (1024 * ONE_KB)
#define ONE_GB (1024 * ONE_MB)

#define PAGE_SIZE (4 * ONE_KB)
#define PAGE_MASK (PAGE_SIZE - 1)

#define pagepad(expr) (((expr) & PAGE_MASK) ? ((expr) & ~PAGE_MASK) + PAGE_SIZE : (expr))

enum class FSFlags : int {
	READ       = 0, 
	WRITE      = 1, 
	CREATE     = 2, 
	OVERWRITE  = 4, 
	DIRECTORY  = 8
};

inline FSFlags operator|(FSFlags a, FSFlags b) {
	return static_cast<FSFlags>(static_cast<int>(a) | static_cast<int>(b));
}
inline FSFlags operator&(FSFlags a, FSFlags b) {
	return static_cast<FSFlags>(static_cast<int>(a) & static_cast<int>(b));
}

inline FSFlags& operator |=(FSFlags& a, FSFlags b) {
	return a = a | b;
}

#define FLAG(val, flag) (((val) & (flag)) == (flag))
