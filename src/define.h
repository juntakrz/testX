#pragma once

// 'fileworks' flags
#define CHECK_IF_EXE	0x01	// check if loaded file is a Windows executable
#define CHECK_IF_ICO	0x02	// check if loaded file is an icon file

typedef enum { none = 0, exec = 1, icon = 2} bufType;