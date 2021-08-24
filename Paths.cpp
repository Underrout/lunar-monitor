#include "pch.h"
#include "Paths.h"

const char* Paths::getRomName()
{
	return reinterpret_cast<char*>(LM_CURR_ROM_NAME);
}

const char* Paths::getRomDir()
{
	return reinterpret_cast<char*>(LM_CURR_ROM_PATH);
}

const char* Paths::getLmExePath()
{
	return reinterpret_cast<char*>(LM_EXE_PATH);
}
