#include <wx/wx.h>

#define MAX_FILENAME_SIZE 5000

#define R_BRIGHTNESS_COEF 0.2126
#define G_BRIGHTNESS_COEF 0.7152
#define B_BRIGHTNESS_COEF 0.0722

#define NUM_COLORS 8

using namespace std;

class colorless_colors_app : public wxApp {
public:
	bool OnInit() override;
};

class colorless_colors_file_select_frame : public wxFrame {
public:
	colorless_colors_file_select_frame();
	char filename[MAX_FILENAME_SIZE] = "";
	void replace_panel(wxPanel* new_panel);
	wxPanel* get_current_panel() {
		return current_panel;
	}
private:
	wxPanel* current_panel;
	void on_select(wxCommandEvent& event);
	void on_exit(wxCommandEvent& event);
};

bool colorless_colors_app::OnInit() {
	wxInitAllImageHandlers();

	colorless_colors_file_select_frame* file_select_frame = new colorless_colors_file_select_frame();
	file_select_frame->Show();
	return true;
}

wxIMPLEMENT_APP(colorless_colors_app);