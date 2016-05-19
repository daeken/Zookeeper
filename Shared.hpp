#pragma once

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
