/* 
    Source borrowed from: https://github.com/AndrewBelt/osdialog
    Heavily modified
*/

#include "choosepngfile.hpp"

#ifdef _WIN32

#include <iostream>

static char* wchar_to_utf8(const wchar_t* s) 
{
	if (!s) return NULL;
	int len = WideCharToMultiByte(CP_UTF8, 0, s, -1, NULL, 0, NULL, NULL);
	if (!len) return NULL;
	char* r = new char[len];
	return r;
}

std::string choose_file(std::string title) 
{
	/* open or save file dialog */
	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.hwndOwner = GetActiveWindow();
	ofn.lStructSize = sizeof(ofn);
	ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	/* The file to choose */
	wchar_t strFile[MAX_PATH] = L"";
	std::wstring dialog_title = std::wstring(title.begin(), title.end());
	ofn.lpstrTitle = dialog_title.c_str();
	ofn.lpstrFile = strFile;
	ofn.nMaxFile = MAX_PATH;

	/* Filters */
	wchar_t strFilter[] = L"*.png\0*.png";
	ofn.lpstrFilter = strFilter;
	ofn.nFilterIndex = 1;

	std::string file_str = "";

	if (GetOpenFileNameW(&ofn)) 
	{
		std::wstring wfile_str(strFile);
		file_str = std::string(wfile_str.begin(), wfile_str.end());
	}

	return file_str;
}

#else

std::string choose_file(std::string title) 
{
	assert(gtk_init_check(NULL, NULL));

	GtkFileChooserAction gtkAction;

	GtkWidget* dialog = gtk_file_chooser_dialog_new(
	    title.c_str(), NULL, GTK_FILE_CHOOSER_ACTION_OPEN, 
        "_Cancel", GTK_RESPONSE_CANCEL,
	    "Open", GTK_RESPONSE_ACCEPT, NULL);

	GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name (filter, "*.png");
    gtk_file_filter_add_pattern (filter, "*.png");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	char* filename = NULL;
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) 
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

	gtk_widget_destroy(dialog);

	std::string filename_str = "";
	if(filename) 
    {
		filename_str = filename;
		g_free(filename);
	}

	while (gtk_events_pending())
		gtk_main_iteration();

	return filename_str;
}

#endif

