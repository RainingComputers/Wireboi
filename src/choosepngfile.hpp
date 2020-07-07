#ifndef CHOOSEPNGFILE
#define CHOOSEPNGFILE

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <cassert>
#include <gtk/gtk.h>
#endif

std::string choose_file(std::string title);

#endif