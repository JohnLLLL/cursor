#include "cursor.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/limits.h>

/**** global data ****/
static struct Cursor_t cursor_;
static struct winsize output_terminal_winsize_;
static char buffer_[BUFSIZ];
static size_t pbl_; /* preoccupied buffer length - const */

/**** local functions declaration ****/
static void apply_color_attr(void);

/**** local functions definitions ****/
static void apply_color_attr(void)
{
  int n, k;
  int attr = cursor_.font_attr;
  sprintf(buffer_ + pbl_, "%d;%d%n", cursor_.fg_color, cursor_.bg_color, &n);

  if (attr & a_bold) {
    sprintf(buffer_ + pbl_ + n, "%d%n", 1, &k);
    n += k;
  }

  if (attr & a_faint) {
    sprintf(buffer_ + pbl_ + n, "%d%n", 2, &k);
    n += k;
  }

  if (attr & a_italic) {
#if 0
    sprintf(buffer_ + pbl_ + n, "%d%n", 1, &k);
    n += k;
#endif
  }

  if (attr & a_underline) {
    sprintf(buffer_ + pbl_ + n, "%d%n", 4, &k);
    n += k;
  }

  sprintf(buffer_ + pbl_ + n, "m%n", &k);
  n += k;

  write(STDOUT_FILENO, buffer_, n);
}

/**** functions definitions ****/
int cursor_init(void)
{
  memset(&cursor_, 0, sizeof(cursor_));
  if (!isatty(STDOUT_FILENO)) {
    return -errno;
  }

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &output_terminal_winsize_)) {
    return -errno;
  }

  cursor_.fd = 0;
  cursor_.col_max = output_terminal_winsize_.ws_col;
  cursor_.row_max = output_terminal_winsize_.ws_row;

  reset_font_attributes();

  buffer_[0] = 0x1b;
  buffer_[1] = '[';
  pbl_ = 2;

  return 1;
}

void cursor_set_position(int col, int row)
{
  int n;

  sprintf(buffer_ + pbl_, "%d;%dH%n", col, row, &n);
  write(STDOUT_FILENO, buffer_, pbl_ + n);
}

void cursor_move_by(int col, int row)
{
  static const char MOVES_V[2] = {'A', 'B'};
  static const char MOVES_H[2] = {'C', 'D'};
  int n;

  sprintf(buffer_ + pbl_, "%d%c%n", abs(col), MOVES_H[col <= 0], &n);
  write(STDOUT_FILENO, buffer_, pbl_ + n);

  sprintf(buffer_ + pbl_, "%d%c%n", abs(row), MOVES_V[row <= 0], &n);
  write(STDOUT_FILENO, buffer_, pbl_ + n);
}

void set_bg_color(enum color_enum_t color)
{
  cursor_.bg_color = 40 + color;

  apply_color_attr();
}

void set_fg_color(enum color_enum_t color)
{
  cursor_.fg_color = 30 + color;

  apply_color_attr();
}

void set_font_attributes(int attr)
{
  cursor_.font_attr = attr;

  apply_color_attr();
}

void add_font_attributes(int attr)
{
  cursor_.font_attr |= attr;

  apply_color_attr();
}

void remove_font_attributes(int attr)
{
  cursor_.font_attr &= ~attr;

  apply_color_attr();
}

void reset_font_attributes(void)
{
  cursor_.bg_color = 40 + c_dflt;
  cursor_.fg_color = 30 + c_dflt;
  cursor_.font_attr = 0;

  apply_color_attr();
}

