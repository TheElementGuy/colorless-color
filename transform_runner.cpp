#include "transform_runner.h"

#include "colorless_color_main_frame.h"

#define MAX_FILENAME_SIZE 500

#define R_BRIGHTNESS_COEF 0.2126
#define G_BRIGHTNESS_COEF 0.7152
#define B_BRIGHTNESS_COEF 0.0722

#define NUM_COLORS 8

#define IMG_WINDOW_PADDING 10

void parse_filename(const char* filename, char* clean) {
	char next_filename[MAX_FILENAME_SIZE];
	const char* final = strrchr(filename, '\\');
	if (!final) {
		return;
	}
	final++;
	int i = 0;
	while (*final) {
		next_filename[i] = *final;
		i++;
		final++;
	}
	next_filename[i] = '\0';
	strcpy(clean, next_filename);
}

int get_new_dimension(int orig, int window, int other_orig, int other_window) {
	double scale_factor = 1.0;
	double real_window = window - IMG_WINDOW_PADDING;
	double real_other_window = other_window - IMG_WINDOW_PADDING;
	if (orig < real_window && other_orig < real_other_window) {
		return orig;
	}
	if (orig > real_window && other_orig < real_other_window) {
		scale_factor = real_window / orig;
	} else if (orig < real_window && other_orig > real_other_window) {
		scale_factor = real_other_window / other_orig;
	} else if (orig > real_window && other_orig > real_other_window) {
		double sf1 = real_window / orig;
		double sf2 = real_other_window / other_orig;
		scale_factor = min(sf1, sf2);
	}
	return orig * scale_factor;
}

void redimension(wxImage* to_resize, colorless_colors_file_select_frame* frame) {
	int resize_width = get_new_dimension(to_resize->GetWidth(), frame->GetClientSize().GetWidth(), to_resize->GetHeight(), frame->GetClientSize().GetHeight());
	int resize_height = get_new_dimension(to_resize->GetHeight(), frame->GetClientSize().GetHeight(), to_resize->GetWidth(), frame->GetClientSize().GetWidth());
	to_resize->Rescale(resize_width, resize_height, wxIMAGE_QUALITY_NEAREST);
}

void run_transform(char* filename, colorless_colors_file_select_frame* frame, const bool open_images_flag) {

	char foldername[MAX_FILENAME_SIZE] = "";
	strncpy(foldername, filename, strlen(filename) - 4);
	foldername[strlen(filename) - 4] = '\0';

	char parsed_filename[MAX_FILENAME_SIZE] = "";
	parse_filename(filename, parsed_filename);

	wxFileName::Mkdir(foldername);

	int load_success = 0;

	const img working = img(filename, &load_success);

	wxImage sample_image(filename, wxBITMAP_TYPE_ANY);

	redimension(&sample_image, frame);

	frame->CallAfter([frame, sample_image] {
		wxPanel* image_panel = new wxPanel(frame, wxID_ANY);

		frame->current_preview = new wxStaticBitmap(image_panel, wxID_ANY, sample_image);

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

		sizer->AddStretchSpacer(1);
		sizer->Add(frame->current_preview, 0, wxALIGN_CENTER_HORIZONTAL);
		sizer->AddStretchSpacer(1);

		image_panel->SetSizer(sizer);

		frame->replace_panel(image_panel);

		image_panel->Layout();
	});

	if (load_success) {

		named_img_transform transforms[NUM_COLORS * (NUM_COLORS + 2)] = {};

		int index = 0;
		for (int i = 0; i < NUM_COLORS; i++) {
			for (int j = 0; j < NUM_COLORS; j++) {
				if (i == j) {
					transforms[index] = nit_of_compression(ALL_COLORS[i]);
					index++;
					transforms[index] = nit_of_translucent(ALL_COLORS[i]);
					index++;
					transforms[index] = nit_of_transparent(ALL_COLORS[i]);
				} else {
					transforms[index] = nit_of_adjusted_lands(ALL_COLORS[i], ALL_COLORS[j]);
				}
				index++;
			}
		}

		for (int i = 0; i < sizeof(transforms) / sizeof(named_img_transform); i++) {
			char message[100] = "Running: ";
			strcat(message, transforms[i].name);
			wxString message_string(message);
			frame->CallAfter([frame, message_string]() {
				frame->SetStatusText(message_string);
			});
			img to_convert(working);
			to_convert.transform(transforms[i].func);
			char new_filename[MAX_FILENAME_SIZE + 10];
			strcpy(new_filename, foldername);
			strcat(new_filename, "\\");
			strcat(new_filename, transforms[i].name);
			strcat(new_filename, "_");
			strcat(new_filename, parsed_filename);
			strcpy(message, "Writing: ");
			strcat(message, transforms[i].name);
			strcat(message, "...");
			message_string = wxString(message);
			frame->CallAfter([frame, message_string]() {
				frame->SetStatusText(message_string);
			});
			if (to_convert.write(new_filename) != 1) {
				frame->CallAfter([frame]() {
					frame->SetStatusText("Error when writing image");
				});
			}

			wxImage new_preview(new_filename, wxBITMAP_TYPE_ANY);

			redimension(&new_preview, frame);

			frame->CallAfter([frame, new_preview]() {
				frame->current_preview->SetBitmap(new_preview);
				frame->current_preview->GetParent()->Layout();
				frame->current_preview->GetParent()->Refresh();
			});

			wxString path_string(new_filename);
			wxLaunchDefaultApplication(path_string);
		}

		wxString folder_path_string(foldername);
		wxLaunchDefaultApplication(folder_path_string);
	} else {
		frame->CallAfter([frame]() {
				frame->SetStatusText("Error loading image. Please try again.");
			});
	}
}