#ifndef _CURSOR_H_
#define _CURSOR_H_

#define ESC_PREFIX            "\x1b["
#define ESC_DELIMITER         ";"

#define COLOR_SUFFIX          "m"
#define CURSOR_POSITION_SUFIX "f"
#define CURSOR_FORWARD_SUFFIX "C"
#define CURSOR_DOWN_SUFFIX    "B"
#define CURSOR_UP_SUFFIX      "A"
#define CURSOR_BACK_SUFFIX    "D"

#define SAVE_CURSOR_POSITION  "\x1b[s"
#define REST_CURSOR_POSITION  "\x1b[u"

#define CURSOR_TO_UL          "\x1b[1;1f"
#define CURSOR_TO_LINE_START  "\x1b[1G"

#define CLEAR_SCREEN          "\x1b[2J"
#define CLEAR_LINE            "\x1b[2K"
#define CLEAR_LINE_BEFORE     "\x1b[1K"


/** color enumeration */
enum color_enum_t {
  c_red         = 1,
  c_green       = 2,
  c_yellow      = 3,
  c_blue        = 4,
  c_magenta     = 5,
  c_cyan        = 6,
  c_white       = 7,
  c_dflt        = 9
};

/** font attributes enumeration */
enum font_attr_enum_t {
  a_none        = 0,
  a_bold        = 0x01,
  a_faint       = 0x02,
  a_italic      = 0x04,
  a_underline   = 0x08
};

/** Cursor describing struct */
struct Cursor_t {
  int col_max;      ///< maximum x-position
  int row_max;      ///< maximum y-position
  int fd;           ///< terminal fd

  int fg_color;     ///< foreground color
  int bg_color;     ///< background color
  int font_attr;    ///< attributes: blink, bold, faint, etc. = bitwise OR
  int col_pos;      ///< current x-position
  int row_pos;      ///< current y-position
};

/**** functions ****/
void gotoxy(int col, int row);
void set_bg_color(color_enum_t color);
void set_fg_color(color_enum_t color);
void set_font_attributes(int attr);
void add_font_attributes(int attr);
void remove_font_attributes(int attr);

#endif
