#include "cursor.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/limits.h>

#if 0
#define LogPrint(fmt,...) do { sprintf(stderr, fmt, ##__VA_ARGS__); } while(0)
#else
#define LogPrint(fmt, ...)
#endif

/**** global data ****/
static struct Cursor_t cursor_;
static struct winsize output_terminal_winsize_;
static char buffer_[BUFSIZ];
static size_t pbl_; /* preoccupied buffer length - const */

/**** local functions declaration ****/
static void apply_color_attr(void);
static void do_changes(int n);

/**** local functions definitions ****/
void do_changes(int n) {
  write(cursor_.fd, buffer_, n);
#if 0
  int i;
  LogPrint("(%08x) ", n);
  for (i = 0; i < n; ++i) {
    LogPrint("%02x ", buffer_[i]);
  }
  LogPrint("\n");
#endif
}

static void apply_color_attr(void)
{
  int n;
  int attr = cursor_.font_attr;

  LogPrint("%s: %d / %d", __func__, cursor_.fg_color, cursor_.bg_color);

  /* XXX attr reset is a spike! */
#warning "Font attr reset is a spike!"
  sprintf(buffer_ + pbl_, "0m");
  do_changes(pbl_ + 2);
  /* end XXX */

  n = sprintf(buffer_ + pbl_, "%d;%d", cursor_.fg_color, cursor_.bg_color);

  if (attr & a_bold) {
    LogPrint(" bold");
    n += sprintf(buffer_ + pbl_ + n, ";%d", 1);
  }

  if (attr & a_faint) {
    LogPrint(" faint");
    n += sprintf(buffer_ + pbl_ + n, ";%d", 2);
  }

  if (attr & a_italic) {
    LogPrint(" italic");
#if 0
    n + =sprintf(buffer_ + pbl_ + n, "%d", 1);
#endif
  }

  if (attr & a_underline) {
    LogPrint(" underline");
    n += sprintf(buffer_ + pbl_ + n, ";%d", 4);
  }

  n += sprintf(buffer_ + pbl_ + n, "m");

  LogPrint(": %d\n", n);

  do_changes(pbl_ + n);
}

/**** functions definitions ****/
int cursor_init(int fd)
{
  memset(&cursor_, 0, sizeof(cursor_));

  cursor_.fd = fd;

  if (!isatty(cursor_.fd)) {
    return -errno;
  }

  if (ioctl(cursor_.fd, TIOCGWINSZ, &output_terminal_winsize_)) {
    return -errno;
  }

  cursor_.col_max = output_terminal_winsize_.ws_col;
  cursor_.row_max = output_terminal_winsize_.ws_row;

  buffer_[0] = '\x1b';
  buffer_[1] = '[';
  pbl_ = 2;

  reset_font_attributes();

  return 1;
}

void cursor_set_position(int col, int row)
{
  LogPrint("%s: %d, %d\n", __func__, col, row);
  int n;

  n = sprintf(buffer_ + pbl_, "%d;%dH", row, col);
  do_changes(pbl_ + n);
}

void cursor_move_by(int col, int row)
{
  static const char MOVES_V[2] = {'A', 'B'};
  static const char MOVES_H[2] = {'C', 'D'};
  int n;

  LogPrint("%s: %d, %d\n", __func__, col, row);
  if (col != 0) {
    n = sprintf(buffer_ + pbl_, "%d%c", abs(col), MOVES_H[col < 0]);
    do_changes(pbl_ + n);
  }

  if (row != 0) {
    n = sprintf(buffer_ + pbl_, "%d%c", abs(row), MOVES_V[row > 0]);
    do_changes(pbl_ + n);
  }
}

void set_bg_color(enum color_enum_t color)
{
  LogPrint("%s: %d\n", __func__, color);
  cursor_.bg_color = 40 + color;

  apply_color_attr();
}

void set_fg_color(enum color_enum_t color)
{
  LogPrint("%s: %d\n", __func__, color);
  cursor_.fg_color = 30 + color;

  apply_color_attr();
}

void set_font_attributes(int attr)
{
  LogPrint("%s: %d\n", __func__, attr);
  cursor_.font_attr = attr;

  apply_color_attr();
}

void add_font_attributes(int attr)
{
  LogPrint("%s: %d\n", __func__, attr);
  cursor_.font_attr |= attr;

  apply_color_attr();
}

void remove_font_attributes(int attr)
{
  LogPrint("%s: %d\n", __func__, attr);
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

void cursor_save_position(void)
{
  sprintf(buffer_ + pbl_, "s");
  do_changes(pbl_ + 1);
}

void cursor_restore_position(void)
{
  sprintf(buffer_ + pbl_, "u");
  do_changes(pbl_ + 1);
}

void cursor_to_line_start(void)
{
  sprintf(buffer_ + pbl_, "1G");
  do_changes(pbl_ + 2);
}

void clear_screen(void)
{
  sprintf(buffer_ + pbl_, "2J");
  do_changes(pbl_ + 2);
}

void clear_entire_line(void)
{
  sprintf(buffer_ + pbl_, "2K");
  do_changes(pbl_ + 2);
}

void clear_line_before(void)
{
  sprintf(buffer_ + pbl_, "1K");
  do_changes(pbl_ + 2);
}
