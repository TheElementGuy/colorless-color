#ifndef COLORLESS_COLOR_COLORLESS_COLOR_MAIN_FRAME_H
#define COLORLESS_COLOR_COLORLESS_COLOR_MAIN_FRAME_H

#include <wx/wx.h>

#include "transform_runner.h"

#define FRAME_MAX_FILENAME_SIZE 500

class colorless_colors_file_select_frame : public wxFrame {
public:
	colorless_colors_file_select_frame();
	char filename[FRAME_MAX_FILENAME_SIZE] = "";
	void replace_panel(wxPanel* new_panel);
	[[nodiscard]] wxPanel* get_current_panel() const {
		return current_panel;
	}
	wxStaticBitmap* current_preview = nullptr;
private:
	wxPanel* current_panel;
	void on_select(wxCommandEvent& event);
	void on_exit(wxCommandEvent& event);
};

#endif //COLORLESS_COLOR_COLORLESS_COLOR_MAIN_FRAME_H
