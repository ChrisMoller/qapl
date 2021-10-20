#ifndef ENUMS_H
#define ENUMS_H

typedef enum {
  MODE_BUTTON_UNSET,
  MODE_BUTTON_SPLINE,
  MODE_BUTTON_LINE,
  MODE_BUTTON_POLAR,
  MODE_BUTTON_PIE,
  MODE_BUTTON_SCATTER,
  MODE_BUTTON_AREA,
  MODE_BUTTON_BOX
} series_mode_e;

typedef enum {
  ASPECT_REAL,
  ASPECT_IMAG,
  ASPECT_MAGNITUDE,
  ASPECT_PHASE
} aspect_e;


#define SETTINGS_EDITOR		"Editor"
#define SETTINGS_FONT_FAMILY	"FontFamily"
#define SETTINGS_FONT_SIZE	"FontSize"
#define SETTINGS_HEIGHT		"Height"
#define SETTINGS_WIDTH		"Width"
#define SETTINGS_BG_COLOUR	"BG_Colour"
#define SETTINGS_FG_COLOUR	"FG_Colour"
#define SETTINGS_PLOT_THEME	"Plot_Theme"

#endif // ENUMS_H
