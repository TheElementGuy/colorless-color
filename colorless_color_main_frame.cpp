#include "colorless_color_main_frame.h"

void colorless_colors_file_select_frame::replace_panel(wxPanel* new_panel) {
	if (current_panel) {
		GetSizer()->Detach(current_panel);
		current_panel->Destroy();
	}

	current_panel = new_panel;
	GetSizer()->Add(current_panel, 1, wxEXPAND);
	Layout();
}

colorless_colors_file_select_frame::colorless_colors_file_select_frame() : wxFrame(nullptr, wxID_ANY, "Colorless Colors App") {
	wxMenu* menu_file = new wxMenu;
	menu_file->Append(wxID_ADD);
	menu_file->AppendSeparator();
	menu_file->Append(wxID_EXIT);

	wxMenuBar* menu_bar = new wxMenuBar;
	menu_bar->Append(menu_file, "&File");

	wxFrameBase::SetMenuBar(menu_bar);

	wxPanel* main_panel = new wxPanel(this, wxID_ANY);

	wxStaticText* welcome_text = new wxStaticText(main_panel, wxID_ANY, "Welcome to colorless color app. Click File > Add to get started.");

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	sizer->AddStretchSpacer(1);
	sizer->Add(welcome_text, 0, wxALIGN_CENTER_HORIZONTAL);
	sizer->AddStretchSpacer(1);

	main_panel->SetSizer(sizer);

	wxFrameBase::CreateStatusBar();
	wxFrameBase::SetStatusText("Awaiting file upload...");

	Bind(wxEVT_MENU, &colorless_colors_file_select_frame::on_select, this, wxID_ADD);
	Bind(wxEVT_MENU, &colorless_colors_file_select_frame::on_exit, this, wxID_EXIT);

	current_panel = main_panel;

	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	main_sizer->Add(current_panel, 1, wxEXPAND);
	SetSizer(main_sizer);

	wxImage raw_icon("auraful.ico", wxBITMAP_TYPE_ICO);

	raw_icon.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);

	wxIcon corner_icon;
	corner_icon.CopyFromBitmap(wxBitmap(raw_icon));

	wxIconBundle all_icons;
	all_icons.AddIcon(corner_icon);
	SetIcons(all_icons);
}

void colorless_colors_file_select_frame::on_select(wxCommandEvent& event) {
	cout << "Choosing file" << endl;
	wxFileDialog file_picker(this, _("Open image file"), "", "", "All Files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (file_picker.ShowModal() == wxID_CANCEL) {
		return;
	}

	strcpy(filename, file_picker.GetPath());

	SetStatusText("Running filters...");

	if (strlen(filename) == 0) {
		SetStatusText("File could not be read. Empty filename. Please try again.");
		return;
	}

	wxMessageDialog open_images(this, "Would you like to open the generated images? Note there are about 80 generated.", "Open generated images?", wxYES_NO);

	bool open_images_tf = false;

	switch (open_images.ShowModal()) {
		case wxID_YES: {
			open_images_tf = true;
			break;
		}
		case wxID_CANCEL: {
			wxFrameBase::SetStatusText("Awaiting file upload...");
			return;
		}
		default: {
			break;
		}
	}

	thread([this, open_images_tf]() {
		run_transform(this->filename, this, open_images_tf);
	}).detach();

}

void colorless_colors_file_select_frame::on_exit(wxCommandEvent& event) {
	Close(true);
}