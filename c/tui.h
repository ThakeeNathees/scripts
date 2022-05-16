#ifndef TUI_H
#define TUI_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  union {
    int x;
    int w;
  };
  union {
    int y;
    int h;
  };
} tui_Vec2;


typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} tui_Color;


typedef struct {
  char* data;
  uint32_t count;
  uint32_t capacity;
} tui_Buffer;


typedef enum {
  TUI_D_UP = 'A',
  TUI_D_DOWN = 'B',
  TUI_D_LEFT = 'D',
  TUI_D_RIGHT = 'C',
} tui_Direction;

typedef enum {

  TUI_KC_UNKNOWN = 0,

  TUI_KC_0 = '0',
  TUI_KC_1 = '1',
  TUI_KC_2 = '2',
  TUI_KC_3 = '3',
  TUI_KC_4 = '4',
  TUI_KC_5 = '5',
  TUI_KC_6 = '6',
  TUI_KC_7 = '7',
  TUI_KC_8 = '8',
  TUI_KC_9 = '9',

  TUI_KC_A = 'A',
  TUI_KC_B = 'B',
  TUI_KC_C = 'C',
  TUI_KC_D = 'D',
  TUI_KC_E = 'E',
  TUI_KC_F = 'F',
  TUI_KC_G = 'G',
  TUI_KC_H = 'H',
  TUI_KC_I = 'I',
  TUI_KC_J = 'J',
  TUI_KC_K = 'K',
  TUI_KC_L = 'L',
  TUI_KC_M = 'M',
  TUI_KC_N = 'N',
  TUI_KC_O = 'O',
  TUI_KC_P = 'P',
  TUI_KC_Q = 'Q',
  TUI_KC_R = 'R',
  TUI_KC_S = 'S',
  TUI_KC_T = 'T',
  TUI_KC_U = 'U',
  TUI_KC_V = 'V',
  TUI_KC_W = 'W',
  TUI_KC_X = 'X',
  TUI_KC_Y = 'Y',
  TUI_KC_Z = 'Z',

  TUI_KC_ESC,
  TUI_KC_ENTER,
  TUI_KC_SPACE,
  TUI_KC_HOME,
  TUI_KC_END,
  TUI_KC_PAGEUP,
  TUI_KC_PAGEDOWN,
  TUI_KC_LEFT,
  TUI_KC_UP,
  TUI_KC_RIGHT,
  TUI_KC_DOWN,
  TUI_KC_INSERT,
  TUI_KC_DELETE,
  TUI_KC_BACKSPACE,
  TUI_KC_TAB,

  TUI_KC_F1,
  TUI_KC_F2,
  TUI_KC_F3,
  TUI_KC_F4,
  TUI_KC_F5,
  TUI_KC_F6,
  TUI_KC_F7,
  TUI_KC_F8,
  TUI_KC_F9,
  TUI_KC_F10,
  TUI_KC_F11,
  TUI_KC_F12,

} tui_KeyCode;


typedef enum {
  TUI_MD_NONE = 0,
  TUI_MD_CTRL = 0b001,
  TUI_MD_ALT = 0b010,
  TUI_MD_SHIFT = 0b100,
} tui_Modifiers;


typedef enum {
  TUI_MB_UNKNOWN = 0,
  TUI_MB_LEFT = 1,
  TUI_MB_MIDDLE = 2,
  TUI_MB_RIGHT = 3,
} tui_MouseBtn;


typedef enum {
  TUI_ET_UNKNOWN = 0,
  TUI_ET_KEY_DOWN,
  TUI_ET_RESIZE,
  TUI_ET_DOUBLE_CLICK,
  TUI_ET_MOUSE_DOWN,
  TUI_ET_MOUSE_UP,
  TUI_ET_MOUSE_MOVE,
  TUI_ET_MOUSE_DRAG,
  TUI_ET_MOUSE_SCROLL,
} tui_EventType;


typedef struct {
  tui_KeyCode keycode;
  char ascii;
  tui_Modifiers modifiers;
} tui_EventKey;


typedef struct {
  tui_MouseBtn button;
  int x, y;
  tui_Direction scroll;
  tui_Modifiers modifiers;
} tui_EventMouse;


typedef struct {
  tui_EventType type;
  union {
    tui_EventKey key;
    tui_EventMouse mouse;
    tui_Vec2 resize;
  };
} tui_Event;


typedef void (*tui_eventCallback) (tui_Event*);
typedef void (*tui_callback) ();


typedef struct {
  void* user_data;
  bool use_alternative_buffer;
  bool hide_cursor;
  bool capture_input_events;
  tui_eventCallback event_callback;
  tui_callback init_callback;
  tui_callback clean_callback;
} tui_Config;


/*****************************************************************************/
/* TUI PUBLIC API                                                            */
/*****************************************************************************/

tui_Config tui_newConfig();
void tui_run(tui_Config* config);
void tui_endLoop();

tui_Color tui_colorHex(const char* color);
#define tui_vec2(x, y) (tui_Vec2) { (x), (y) }
#define tui_color(r, g, b) (tui_Color) { (r), (g), (b) }

void tui_bufferInit(tui_Buffer* buff);
void tui_bufferClean(tui_Buffer* buff);
void tui_write(tui_Buffer* buff, const char* data, uint32_t count);

void tui_char(char c);
void tui_charLen(char c, uint32_t len);
void tui_boxChar(char c);
void tui_boxCharLen(char c, uint32_t len);
void tui_string(const char* str);
void tui_stringLen(const char* str, uint32_t length);
void tui_int(int num);
void tui_fmt(const char* fmt, ...);

void tui_clear();
void tui_eraseTillEol();
void tui_eraseTillEof();
void tui_flush();

bool tui_isatty();
void tui_setTitle(const char* title);
tui_Vec2 tui_getSize();
tui_Vec2 tui_getPosition();

void tui_setPosition(tui_Vec2 pos);
void tui_move(int n, tui_Direction direction);
void tui_hideCursor();
void tui_showCursor();

void tui_boldBegin();
void tui_boldEnd();
void tui_dimBegin();
void tui_dimEnd();
void tui_italicBegin();
void tui_italicEnd();
void tui_underlineBegin();
void tui_underlineEnd();
void tui_hiddenBegin();
void tui_hiddenEnd();
void tui_strikethroughBegin();
void tui_strikethroughEnd();

void tui_colorPush(tui_Color color);
void tui_colorPop();
void tui_colorBgPush(tui_Color color);
void tui_colorBgPop();

void tui_colorBegin(tui_Color col);
void tui_colorEnd();
void tui_colorBgBegin(tui_Color col);
void tui_colorBgEnd();

void tui_black();
void tui_blackBg();
void tui_red();
void tui_redBg();
void tui_green();
void tui_greenBg();
void tui_yellow();
void tui_yellowBg();
void tui_blue();
void tui_blueBg();
void tui_magenta();
void tui_magentaBg();
void tui_cyan();
void tui_cyanBg();
void tui_white();
void tui_whiteBg();
void tui_default();
void tui_defaultBg();

/*****************************************************************************/
/* TUI IMPLEMENTATION                                                        */
/*****************************************************************************/

#if defined(TUI_IMPL)

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Compiler warnings.
#ifdef _MSC_VER
  #pragma warning(disable:6308)
#elif defined(__GNUC__)
  // Bug 53119 - -Wmissing-braces wrongly warns about universal zero initializer {0}.
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
  #pragma GCC diagnostic ignored "-Wmissing-braces"
#endif

// Platform detection macros.
#ifdef _WIN32
  #define TUI_SYS_WINDOWS
#else
  #define TUI_SYS_NIX
#endif


// Platform specific includes.
#if defined(TUI_SYS_WINDOWS)
  #include <Windows.h>

#elif defined(TUI_SYS_NIX)
  #include <termios.h>
  #include <sys/ioctl.h>
  #include <signal.h>
#endif

#if defined(_MSC_VER) || (defined(_WIN32) && defined(__TINYC__))
  #include <io.h>
  #define isatty _isatty
  #define fileno _fileno
#else
  #include <unistd.h>
#endif

#define TUI_REALLOC(ptr, size) realloc(ptr, size)
#define TUI_FREE(ptr) free(ptr)

// Maximum buffer size for sprintf to write an integer to it.
// INT_MAX = 10 digits, '+/-' = 1 digit, '\0' = 1 digit.
#define TUI_INTBUFF_MAX 12

#define TUI_EVENT_BUFF_SIZE 64
#define TUI_INPUT_BUFF_SIZE 128
#define TUI_COLOR_STACK_SIZE 256

#define TUI_MAX(a, b) ((a) > (b) ? (a) : (b))
#define TUI_MIN(a, b) ((a) < (b) ? (a) : (b))

#define _BETWEEN(a, c, b) ((a) <= (c) && (c) <= (b))

typedef struct {

#if defined(TUI_SYS_WINDOWS)
  DWORD outmode, inmode; // Initial modes to restore.
  HANDLE h_stdout, h_stdin; // Handles.


#elif defined(TUI_SYS_NIX)
  struct termios tio;
  tui_Buffer input_buff;
#endif

  tui_Vec2 screensize;
  tui_Vec2 mouse_pos;
  tui_Buffer screenbuff;

  int color_sp_fg, color_sp_bg; // Color stack pointers.
  tui_Color color_stack_fg[TUI_COLOR_STACK_SIZE];
  tui_Color color_stack_bg[TUI_COLOR_STACK_SIZE];

  tui_Config config;
  bool isdone;
  const char* err; // Contain error message if any.
} tui_Ctx;

static tui_Ctx _ctx;

static inline void _newScreenBuffer();
static inline void _restoreScreenBuffer();

static void _initTerminal();
static void _restoreTerminal();


tui_Config tui_newConfig() {
  tui_Config config;
  memset(&config, 0, sizeof(config));
  config.use_alternative_buffer = false;
  return config;
}


static void _finalize(void) {
  if (_ctx.config.clean_callback) _ctx.config.clean_callback();

  tui_bufferClean(&_ctx.screenbuff);
  if (_ctx.config.hide_cursor) tui_showCursor(NULL);
  if (_ctx.config.use_alternative_buffer) _restoreScreenBuffer();
  if (_ctx.config.hide_cursor) tui_showCursor(NULL);
  _restoreTerminal();
}


// Should be running in a tty otherwise it'll fail.
static void _initialize(tui_Config* config) {
  assert(tui_isatty(NULL));

  memset(&_ctx, 0, sizeof(_ctx));
  _ctx.config = (config != NULL) ? *config : tui_newConfig();
  _ctx.isdone = false;

  _initTerminal();
  _ctx.screensize = tui_getSize();
  tui_bufferInit(&_ctx.screenbuff);

  if (_ctx.config.use_alternative_buffer) _newScreenBuffer();
  if (_ctx.config.hide_cursor) tui_hideCursor(NULL);

  atexit(_finalize);

  if (_ctx.config.init_callback) _ctx.config.init_callback();
}


tui_Color tui_colorHex(const char* color) {
  assert(color != NULL);
  if (*color == '#') color++;

  int c[6];

  for (int i = 0; i < 6; i++) {
    if (_BETWEEN('0', *color, '9')) {
      c[i] = (*color++) - '0';
    } else if (_BETWEEN('a', *color, 'f')) {
      c[i] = ((*color++) - 'a') + 10;
    } else if (_BETWEEN('A', *color, 'F')) {
      c[i] = ((*color++) - 'A') + 10;
    } else {
      assert(false); // Invalid color string.
    }
  }

  return (tui_Color) {
    c[0] << 4 | c[1], // r
    c[2] << 4 | c[3], // g
    c[4] << 4 | c[5], // b
  };
}


void tui_bufferInit(tui_Buffer* buff) {
  assert(buff != NULL);
  buff->data = NULL;
  buff->count = 0;
  buff->capacity = 0;
}


void tui_bufferClean(tui_Buffer* buff) {
  assert(buff != NULL);
  buff->data = TUI_REALLOC(buff->data, 0);
  buff->count = 0;
  buff->capacity = 0;
}


// Returns the smallest power of two that is equal to or greater than [n].
// Reference: https://github.com/wren-lang/wren/blob/main/src/vm/wren_utils.h
static inline int _powerOf2Ceil(int n) {
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;

  return n;
}


static void _bufferReserve(tui_Buffer* buff, uint32_t count) {
  assert(buff != NULL);
  if (buff->capacity >= count) return;
  count = _powerOf2Ceil(count);
  count = TUI_MAX(8, count); // Minimum size = 8.
  buff->data = TUI_REALLOC(buff->data, count);
  buff->capacity = count;
}


void tui_write(tui_Buffer* buff, const char* data, uint32_t count) {
  assert(buff != NULL);
  _bufferReserve(buff, buff->count + count);
  memcpy(buff->data + buff->count, data, (size_t)count);
  buff->count += count;
}


void tui_char(char c) {
  tui_Buffer* buff = &_ctx.screenbuff;
  _bufferReserve(buff, buff->count + 1);
  buff->data[buff->count++] = c;
}


void tui_charLen(char c, uint32_t len) {
  tui_Buffer* buff = &_ctx.screenbuff;
  _bufferReserve(buff, buff->count + len);
  memset(buff->data + buff->count, c, (size_t) len);
  buff->count += len;
}


/*
* Hex    ASCII  DEC-Line-Drawing
* ------------------------------
*  0x6a    j        ┘
*  0x6b    k        ┐
*  0x6c    l        ┌
*  0x6d    m        └
*  0x6e    n        ┼
*  0x71    q        ─
*  0x74    t        ├
*  0x75    u        ┤
*  0x76    v        ┴
*  0x77    w        ┬
*  0x78    x        │
*/
void tui_boxChar(char c) {
  tui_Buffer* buff = &_ctx.screenbuff;
  tui_string("\x1b(0");
  tui_char(c);
  tui_string("\x1b(B");
}


void tui_boxCharLen(char c, uint32_t len) {
  tui_Buffer* buff = &_ctx.screenbuff;
  tui_string("\x1b(0");
  tui_charLen(c, len);
  tui_string("\x1b(B");
}


void tui_string(const char* str) {
  tui_Buffer* buff = &_ctx.screenbuff;
  tui_write(buff, str, (uint32_t) strlen(str));
}


void tui_stringLen(const char* str, uint32_t length) {
  tui_Buffer* buff = &_ctx.screenbuff;
  tui_write(buff, str, length);
}


void tui_int(int num) {
  char int_buff[TUI_INTBUFF_MAX];
  sprintf(int_buff, "%d", num);
  tui_string(int_buff);
}


void tui_fmt(const char* fmt, ...) {
  tui_Buffer* buff = &_ctx.screenbuff;
  va_list args;
  va_start(args, fmt);

  va_list args_copy;
  va_copy(args_copy, args);
  int size = vsnprintf(NULL, 0, fmt, args_copy);
  va_end(args_copy);

  // vnsprintf will only copy n-1 characters (so we're using size + 1).
  _bufferReserve(buff, buff->count + size + 1);
  vsnprintf(buff->data + buff->count, (size_t)size + 1, fmt, args);
  buff->count += size;

  va_end(args);
}


void tui_clear() {
  tui_string("\x1b[2J\x1b[H");
}


void tui_eraseTillEol() {
  tui_string("\x1b[K");
}


void tui_eraseTillEof() {
  tui_string("\x1b[0J");
}


void tui_flush() {
  tui_char('\0');
  tui_Buffer* buff = &_ctx.screenbuff;
  fprintf(stdout, "%s", buff->data);
  buff->count = 0;
}


// "\x1b[{escape}m"
static inline void _escape(const char* escape) {
  tui_fmt("\x1b[%sm", escape);
}


// "\x1b[{escape};2;{r};{g};{b}m" Note: %hhu = unsigned char.
static inline void _escapeCol(tui_Color col, const char* escape) {
  tui_fmt("\x1b[%s;2;%hhu;%hhu;%hhum", escape, col.r, col.g, col.b);
}


void tui_reset() {
  _escape("0"); // "\x1b[0m"
}


void tui_colorPush(tui_Color color) {
  assert(_ctx.color_sp_fg + 1 < TUI_COLOR_STACK_SIZE);
  _ctx.color_stack_fg[++_ctx.color_sp_fg] = color;
  tui_colorBegin(color);
}


void tui_colorPop() {
  assert(_ctx.color_sp_fg > 0);
  --_ctx.color_sp_fg;
  if (_ctx.color_sp_fg == 0) tui_colorEnd();
  else tui_colorBegin(_ctx.color_stack_fg[_ctx.color_sp_fg]);
}


void tui_colorBgPush(tui_Color color) {
  assert(_ctx.color_sp_bg + 1 < TUI_COLOR_STACK_SIZE);
  _ctx.color_stack_bg[++_ctx.color_sp_bg] = color;
  tui_colorBgBegin(color);
}


void tui_colorBgPop() {
  assert(_ctx.color_sp_bg > 0);
  --_ctx.color_sp_bg;
  if (_ctx.color_sp_bg == 0) tui_colorEnd();
  else tui_colorBgBegin(_ctx.color_stack_bg[_ctx.color_sp_bg]);
}


void tui_colorBegin(tui_Color col) {
  _escapeCol(col, "38");
}


void tui_colorEnd() {
  _escape("39");
}


void tui_colorBgBegin(tui_Color col) {
  _escapeCol(col, "48");
}


void tui_colorBgEnd() {
  _escape("49");
}


#define _TUI_GRAPHICS_FN(name, esc_begin, esc_end) \
  void tui_##name##Begin() {                       \
    _escape(#esc_begin);                           \
  }                                                \
                                                   \
  void tui_##name##End() {                         \
    _escape(#esc_end);                             \
  }

_TUI_GRAPHICS_FN(bold, 1, 22)
_TUI_GRAPHICS_FN(dim, 2, 22)
_TUI_GRAPHICS_FN(italic, 3, 23)
_TUI_GRAPHICS_FN(underline, 4, 24)
_TUI_GRAPHICS_FN(hidden, 8, 28)
_TUI_GRAPHICS_FN(strikethrough, 9, 29)

#undef _TUI_GRAPHICS_FN

#define _TUI_COLOR_FN(name, fg, bg)       \
  void tui_##name() {                     \
    _escape(#fg);                         \
  }                                       \
                                          \
  void tui_##name##Bg() {                 \
    _escape(#bg);                         \
  }


_TUI_COLOR_FN(black, 30, 40)
_TUI_COLOR_FN(red, 31, 41)
_TUI_COLOR_FN(green, 32, 42)
_TUI_COLOR_FN(yellow, 33, 43)
_TUI_COLOR_FN(blue, 34, 44)
_TUI_COLOR_FN(magenta, 35, 45)
_TUI_COLOR_FN(cyan, 36, 46)
_TUI_COLOR_FN(white, 37, 47)
_TUI_COLOR_FN(default, 39, 49)

#undef _TUI_COLOR_FN


bool tui_isatty() {
  return (!!isatty(fileno(stdout))) && (!!isatty(fileno(stdin)));
}


void tui_setTitle(const char* title) {
  assert(title != NULL);
  fprintf(stdout, "\x1b]0;%s\x07", title);
}


tui_Vec2 tui_getSize() {
  tui_Vec2 size;

#if defined(TUI_SYS_WINDOWS)
  CONSOLE_SCREEN_BUFFER_INFO binfo;
  GetConsoleScreenBufferInfo(_ctx.h_stdout, &binfo);
  size.w = binfo.srWindow.Right - binfo.srWindow.Left + 1;
  size.y = binfo.srWindow.Bottom - binfo.srWindow.Top + 1;

#elif defined(TUI_SYS_NIX)
  struct winsize wsize;
  ioctl(fileno(stdout), TIOCGWINSZ, &wsize);
  size.w = wsize.ws_col;
  size.h = wsize.ws_row;
#endif

  return size;
}


tui_Vec2 tui_getPosition() {
  tui_Vec2 pos;

#if defined(TUI_SYS_WINDOWS)
  CONSOLE_SCREEN_BUFFER_INFO binfo;
  GetConsoleScreenBufferInfo(_ctx.h_stdout, &binfo);
  pos.x = binfo.dwCursorPosition.X;
  pos.y = binfo.dwCursorPosition.Y;
  return pos;

#elif defined(TUI_SYS_NIX)

  struct termios old_state, new_state;
  if (tcgetattr(fileno(stdin), &old_state) != 0) {
    _ctx.err = "tcgetattr(stdin) failed.";
    goto L_done;
  }

  new_state = old_state;
  new_state.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(fileno(stdin), TCSANOW, &new_state);

  // Request cursor position. stdin will be in the for of ESC[n;mR
  // here where n is the row and m is the column. (1 based).
  fprintf(stdout, "\x1b[6n");

  if (getchar() != '\x1b' || getchar() != '[') {
    _ctx.err = "Request for cursor failed.";
    goto L_done;
  }

  pos.x = 0; pos.y = 0;
  int* p = &pos.y;

  while (true) {
    int c = getchar();

    if (c == EOF) {
      _ctx.err = "getchar() failed.";
      goto L_done;
    }

    if (c == ';') p = &pos.x;
    if (c == 'R') break;

    
    if (_BETWEEN('0', c, '9')) {
      *p = *p * 10 + (c - '0');
    }
  }

  // Since column, row numbers are 1 based substract 1 for 0 based.
  pos.x--; pos.y--;

L_done:
  // Restore the old state.
  tcsetattr(fileno(stdin), TCSANOW, &old_state);
#endif

  return pos;
}


void tui_setPosition(tui_Vec2 pos) {
  tui_fmt("\x1b[%i;%iH", pos.y + 1, pos.x + 1);
}


void tui_move(int n, tui_Direction direction) {
  tui_fmt("\x1b[%i%c", n, (char) direction);
}


void tui_hideCursor() {
  tui_string("\x1b[?25l");
}


void tui_showCursor() {
  tui_string("\x1b[?25h");
}


static inline void _newScreenBuffer() {
  fprintf(stdout, "\x1b[?1049h");
}


static inline void _restoreScreenBuffer() {
  fprintf(stdout, "\x1b[H\x1b[J"); // Clear screen and go to (0, 0).
  fprintf(stdout, "\x1b[?1049l");
}


// Platform specific _initTerminal() implementation.
#if defined(TUI_SYS_WINDOWS)


static void _initTerminal() {
  _ctx.h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(_ctx.h_stdout, &_ctx.outmode);

  _ctx.h_stdin = GetStdHandle(STD_INPUT_HANDLE);
  GetConsoleMode(_ctx.h_stdin, &_ctx.inmode);

  DWORD outmode = (_ctx.outmode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  SetConsoleMode(_ctx.h_stdout, outmode);

  if (_ctx.config.capture_input_events) {
    DWORD inmode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    SetConsoleMode(_ctx.h_stdin, inmode);
  }
}

#elif defined(TUI_SYS_NIX)

static void _resizeHandler(int sig) {
  tui_Vec2 newsize = tui_getSize();
  if (_ctx.screensize.x == newsize.x && _ctx.screensize.y == newsize.y) return;
  _ctx.screensize = newsize;

  if (_ctx.config.event_callback != NULL) {
    tui_Event event;
    memset(&event, 0, sizeof(tui_Event));
    event.type = TUI_ET_RESIZE;
    event.resize = _ctx.screensize;
    _ctx.config.event_callback(&event);
  }
}

static void _initTerminal() {

  // Backup the older termios.
  tcgetattr(fileno(stdin), &(_ctx.tio));

  tui_bufferInit(&_ctx.input_buff);
  _bufferReserve(&_ctx.input_buff, TUI_INPUT_BUFF_SIZE);

  struct termios tio;
  tcgetattr(fileno(stdin), &tio);

  signal(SIGWINCH, _resizeHandler);

  // References:
  // https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html#turn-off-ctrl-c-and-ctrl-z-signals
  // https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html#disable-ctrl-s-and-ctrl-q
  if (_ctx.config.capture_input_events) {
    tio.c_iflag &= ~(ICRNL | IXON | IGNBRK | BRKINT | PARMRK);
    tio.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN | TOSTOP);
  } else {
    tio.c_lflag &= ~(ECHO | ICANON);
  }

  // Non blocking stdin (read time = 0). However WSL (windows subsystem for linux) is always
  // blocking.
  tio.c_cc[VTIME] = 0;
  tio.c_cc[VMIN] = 0;
  tcsetattr(fileno(stdin), TCSAFLUSH, &tio);

  // Enable mouse events.
  fprintf(stdout, "\x1b[?1003h\x1b[?1006h");

}
#endif


#if defined(TUI_SYS_WINDOWS)

static void _restoreTerminal() {
  SetConsoleMode(_ctx.h_stdout, _ctx.outmode);
  SetConsoleMode(_ctx.h_stdin, _ctx.inmode);
}

#elif defined(TUI_SYS_NIX)

static void _restoreTerminal() {
  tui_bufferClean(&_ctx.input_buff);
  tcsetattr(fileno(stdin), TCSAFLUSH, &(_ctx.tio));
  fprintf(stdout, "\x1b[?1003l\x1b[?1006l\x1b[?25h");
}

#endif


#if defined(TUI_SYS_WINDOWS)

// Returns false if the key should be ignored as an event (ex: shift, ctrl, ...).
static bool _toTuiKeyCode(WORD vk, tui_KeyCode * kc) {

  if (0x30 <= vk && vk <= 0x39) { *kc = (tui_KeyCode)TUI_KC_0 + (vk - 0x30); return true; }
  if (0x60 <= vk && vk <= 0x69) { *kc = (tui_KeyCode)TUI_KC_0 + (vk - 0x60); return true; }
  if (0x41 <= vk && vk <= 0x5a) { *kc = (tui_KeyCode)TUI_KC_A + (vk - 0x41); return true; }
  if (0x70 <= vk && vk <= 0x7b) { *kc = (tui_KeyCode)TUI_KC_F1 + (vk - 0x70); return true; }

  // Note that shift, ctrl, alt keys are returned as TUI_KC_UNKNOWN since *nix systems
  // don't support them.
  switch (vk) {
  case VK_BACK: *kc = TUI_KC_BACKSPACE; break;
  case VK_TAB: *kc = TUI_KC_TAB; break;
  case VK_RETURN: *kc = TUI_KC_ENTER; break;
  case VK_ESCAPE: *kc = TUI_KC_ESC; break;
  case VK_SPACE: *kc = TUI_KC_SPACE; break;
  case VK_PRIOR: *kc = TUI_KC_PAGEUP; break;
  case VK_NEXT: *kc = TUI_KC_PAGEDOWN; break;
  case VK_END: *kc = TUI_KC_END; break;
  case VK_HOME: *kc = TUI_KC_HOME; break;
  case VK_LEFT: *kc = TUI_KC_LEFT; break;
  case VK_RIGHT: *kc = TUI_KC_RIGHT; break;
  case VK_UP: *kc = TUI_KC_UP; break;
  case VK_DOWN: *kc = TUI_KC_DOWN; break;
  case VK_INSERT: *kc = TUI_KC_INSERT; break;
  case VK_DELETE: *kc = TUI_KC_DELETE; break;

  case VK_SHIFT:
  case VK_CONTROL:
  case VK_MENU:
  case VK_PAUSE:
  case VK_CAPITAL: //< Capslock.
    return false;
  }

  return true;
}

static bool _readEvent(tui_Event* event) {

  memset(event, 0, sizeof(tui_Event));
  event->type = TUI_ET_UNKNOWN;

  DWORD count;
  if (!GetNumberOfConsoleInputEvents(_ctx.h_stdin, &count)) {
    _ctx.err = "GetNumberOfConsoleInputEvents() failed.";
    return false;
  }

  if (count == 0) return false;

  INPUT_RECORD ir;
  if (!ReadConsoleInput(_ctx.h_stdin, &ir, 1, &count)) {
    _ctx.err = "ReadConsoleInput() failed.";
    return false;
  }

  switch (ir.EventType) {
    case KEY_EVENT: {
      KEY_EVENT_RECORD* ker = &ir.Event.KeyEvent;

      // Key up event not available in *nix systems. So we're ignoring here as well.
      if (!ker->bKeyDown) return false;

      if (!_toTuiKeyCode(ker->wVirtualKeyCode, &event->key.keycode)) return false;

      event->type = TUI_ET_KEY_DOWN;
      event->key.ascii = ker->uChar.AsciiChar;

      if ((ker->dwControlKeyState & LEFT_ALT_PRESSED) || (ker->dwControlKeyState & RIGHT_ALT_PRESSED))
        event->key.modifiers |= TUI_MD_ALT;
      if ((ker->dwControlKeyState & LEFT_CTRL_PRESSED) || (ker->dwControlKeyState & RIGHT_CTRL_PRESSED))
        event->key.modifiers |= TUI_MD_CTRL;
      if (ker->dwControlKeyState & SHIFT_PRESSED)
        event->key.modifiers |= TUI_MD_SHIFT;

    } break;

    case MOUSE_EVENT: {

      MOUSE_EVENT_RECORD* mer = &ir.Event.MouseEvent;

      static DWORD last_state = 0; // < Last state to compare if a new button pressed.
      bool pressed = mer->dwButtonState; // If any state is on pressed will be true.

      // Xor will give != 0 if any button changed.
      DWORD change = last_state ^ mer->dwButtonState;

      if (change != 0) {
        // What if the mouse doesn't have the middle button and right button is the second button?.
        if (change & FROM_LEFT_1ST_BUTTON_PRESSED) {
          pressed = mer->dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED;
          event->mouse.button = TUI_MB_LEFT;

        } else if (change & RIGHTMOST_BUTTON_PRESSED) {
          pressed = mer->dwButtonState & RIGHTMOST_BUTTON_PRESSED;
          event->mouse.button = TUI_MB_RIGHT;

        } else if (change & FROM_LEFT_2ND_BUTTON_PRESSED) {
          pressed = mer->dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED;
          event->mouse.button = TUI_MB_MIDDLE;
        }
      }
      last_state = mer->dwButtonState;

      event->mouse.x = mer->dwMousePosition.X;
      event->mouse.y = mer->dwMousePosition.Y;

      if (mer->dwEventFlags == 0) {
        event->type = (pressed) ? TUI_ET_MOUSE_DOWN : TUI_ET_MOUSE_UP;

      } else if (mer->dwEventFlags & MOUSE_MOVED) {
        if (_ctx.mouse_pos.x == event->mouse.x && _ctx.mouse_pos.y == event->mouse.y) {
          return false;
        }
        event->type = (pressed) ? TUI_ET_MOUSE_DRAG : TUI_ET_MOUSE_MOVE;

      } else if (mer->dwEventFlags & MOUSE_WHEELED) {
        event->type = TUI_ET_MOUSE_SCROLL;
        event->mouse.scroll = (mer->dwButtonState & 0xFF000000) ? TUI_D_DOWN : TUI_D_UP;

      } else if (mer->dwEventFlags & DOUBLE_CLICK) {
        event->type = TUI_ET_DOUBLE_CLICK;
      }

      _ctx.mouse_pos.x = event->mouse.x;
      _ctx.mouse_pos.y = event->mouse.y;

      if ((mer->dwControlKeyState & LEFT_ALT_PRESSED) || (mer->dwControlKeyState & RIGHT_ALT_PRESSED))
        event->mouse.modifiers |= TUI_MD_ALT;
      if ((mer->dwControlKeyState & LEFT_CTRL_PRESSED) || (mer->dwControlKeyState & RIGHT_CTRL_PRESSED))
        event->mouse.modifiers |= TUI_MD_CTRL;
      if (mer->dwControlKeyState & SHIFT_PRESSED)
        event->mouse.modifiers |= TUI_MD_SHIFT;


    } break;

    case WINDOW_BUFFER_SIZE_EVENT: {
      WINDOW_BUFFER_SIZE_RECORD* wbs = &ir.Event.WindowBufferSizeEvent;
      event->type = TUI_ET_RESIZE;
      tui_Vec2 newsize = tui_vec2(wbs->dwSize.X, wbs->dwSize.Y);
      if (_ctx.screensize.x == newsize.x && _ctx.screensize.y == newsize.y) return false;
      _ctx.screensize = newsize;
      event->resize = _ctx.screensize;
    } break;

    // Not handling as it's not available in *nix.
    case MENU_EVENT:
    case FOCUS_EVENT:
      return false;
  }

  return event->type != TUI_ET_UNKNOWN;
}

#elif defined(TUI_SYS_NIX)

// Returns the length of an escape sequence.
static int _escapeLength(const char* buff, uint32_t size) {
  int length = 0;

  while (length < size) {
    char c = buff[length++];

    if (_BETWEEN('a', c, 'z') || _BETWEEN('A', c, 'Z') || (c == '~')) {
      if (c == 'O' && length < size) {
        c = buff[length];
        if (_BETWEEN('A', c, 'D') || _BETWEEN('P', c, 'S') || c == 'F' || c == 'H') {
          return length + 1;
        }
      }

      return length;

    } else if (c == '\x1b') {
      return length;
    }
  }

  return length;
}


static void _keyEvent(char c, tui_Event * event) {
  event->type = TUI_ET_KEY_DOWN;
  event->key.ascii = c;

  // Note: Ctrl+M and <enter> both reads as '\r'.
  if (c == '\r') { event->key.keycode = TUI_KC_ENTER; return; }
  if (c == 127) { event->key.keycode = TUI_KC_BACKSPACE; return; }
  if (c == 9) { event->key.keycode = TUI_KC_TAB; return; }
  if (c == 32) { event->key.keycode = TUI_KC_SPACE; return; }

  event->key.keycode = (tui_KeyCode)c;

  // Ctrl + key
  if (1 <= c && c <= 26) {
    event->key.modifiers |= TUI_MD_CTRL;
    event->key.keycode = (tui_KeyCode)('A' + (c - 1));

  } else if (_BETWEEN('a', c, 'z') || _BETWEEN('A', c, 'Z') || _BETWEEN('0', c, '9')) {
    event->key.keycode = (tui_KeyCode)c;
    if (_BETWEEN('A', c, 'Z')) event->key.modifiers |= TUI_MD_SHIFT;
  }
}


// Reference: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h2-Mouse-Tracking
static void _mouseEvent(const char* buff, uint32_t count, tui_Event * event) {

  // buff = cb ; cx ; cy m|M

  const char* c = buff;
  if (!(_BETWEEN('0', *c, '9'))) return;

  int cb = 0, cx = 0, cy = 0;
  char m;

  while (_BETWEEN('0', *c, '9')) cb = cb * 10 + (*c++ - '0');
  if (*c++ != ';') return;
  while (_BETWEEN('0', *c, '9')) cx = cx * 10 + (*c++ - '0');
  if (*c++ != ';') return;
  while (_BETWEEN('0', *c, '9')) cy = cy * 10 + (*c++ - '0');

  m = *c++;
  if (m != 'm' && m != 'M') return;

  // low two bits = button information.
  // next three bits = modifiers.
  // next bits = event type.
  int low = cb & 0b11;
  int high = (cb & 0b11100) >> 2;
  int type = cb >> 5;

  // Note that the modifiers won't work/incorrect in WSL.
  if (high & 0b001) event->mouse.modifiers |= TUI_MD_SHIFT;
  if (high & 0b010) event->mouse.modifiers |= TUI_MD_ALT;
  if (high & 0b100) event->mouse.modifiers |= TUI_MD_CTRL;

  event->mouse.x = cx - 1;
  event->mouse.y = cy - 1;

  switch (type) {
    case 0: {
      event->type = (m == 'M') ? TUI_ET_MOUSE_DOWN : TUI_ET_MOUSE_UP;
      event->mouse.button = (tui_MouseBtn)(low + 1);
    } break;

    case 1: {
      if (low == 0b11) {  // Mouse move.
        event->type = TUI_ET_MOUSE_MOVE;

      } else { // Drag.
        event->type = TUI_ET_MOUSE_DRAG;
        event->mouse.button = (tui_MouseBtn)(low + 1);
      }
    } break;

    case 2: { // Scroll.
      event->type = TUI_ET_MOUSE_SCROLL;

      if (low == 0) event->mouse.scroll = TUI_D_UP;
      else if (low == 1) event->mouse.scroll = TUI_D_DOWN;

    } break;
  }


}

void _parseEscapeSequence(const char* buff, uint32_t count, tui_Event * event) {
  assert(buff[0] == '\x1b');

  if (count == 1) {
    event->type = TUI_ET_KEY_DOWN;
    event->key.ascii = *buff;
    event->key.keycode = TUI_KC_ESC;
    return;
  }

  if (count == 2) {
    _keyEvent(buff[1], event);
    event->key.modifiers |= TUI_MD_ALT;
    return;
  }


#define _SET_KEY(code)              \
  do {                              \
    event->type = TUI_ET_KEY_DOWN;  \
    event->key.keycode = code;      \
  } while (false)


#define _MATCH(str) ((count >= strlen(str) + 1) && (strncmp(buff + 1, (str), strlen(str)) == 0))

  if (_MATCH("[<")) _mouseEvent(buff + 3, count - 3, event);

  else if (_MATCH("[A") || _MATCH("OA")) _SET_KEY(TUI_KC_UP);
  else if (_MATCH("[B") || _MATCH("OB")) _SET_KEY(TUI_KC_DOWN);
  else if (_MATCH("[C") || _MATCH("OC")) _SET_KEY(TUI_KC_RIGHT);
  else if (_MATCH("[D") || _MATCH("OD")) _SET_KEY(TUI_KC_LEFT);
  else if (_MATCH("[5~") || _MATCH("[[5~")) _SET_KEY(TUI_KC_PAGEUP);
  else if (_MATCH("[6~") || _MATCH("[[6~")) _SET_KEY(TUI_KC_PAGEDOWN);

  else if (_MATCH("[H") || _MATCH("OH") || _MATCH("[1~") || _MATCH("[[7~")) _SET_KEY(TUI_KC_HOME);
  else if (_MATCH("[F") || _MATCH("OF") || _MATCH("[4~") || _MATCH("[[8~")) _SET_KEY(TUI_KC_END);

  else if (_MATCH("[2~")) _SET_KEY(TUI_KC_INSERT);
  else if (_MATCH("[3~")) _SET_KEY(TUI_KC_DELETE);

  else if (_MATCH("OP") || _MATCH("[11~")) _SET_KEY(TUI_KC_F1);
  else if (_MATCH("OQ") || _MATCH("[12~")) _SET_KEY(TUI_KC_F2);
  else if (_MATCH("OR") || _MATCH("[13~")) _SET_KEY(TUI_KC_F3);
  else if (_MATCH("OS") || _MATCH("[14~")) _SET_KEY(TUI_KC_F4);

  else if (_MATCH("[15~")) _SET_KEY(TUI_KC_F5);
  else if (_MATCH("[17~")) _SET_KEY(TUI_KC_F6);
  else if (_MATCH("[18~")) _SET_KEY(TUI_KC_F7);
  else if (_MATCH("[19~")) _SET_KEY(TUI_KC_F8);
  else if (_MATCH("[20~")) _SET_KEY(TUI_KC_F9);
  else if (_MATCH("[21~")) _SET_KEY(TUI_KC_F10);
  else if (_MATCH("[23~")) _SET_KEY(TUI_KC_F11);
  else if (_MATCH("[24~")) _SET_KEY(TUI_KC_F12);

#undef _MATCH
#undef _SET_KEY

}


static void _buffShfit(tui_Buffer* buff, uint32_t count) {
  assert(buff != NULL);
  if (count < buff->count) {
    memmove(buff->data, buff->data + count, buff->count - count);
    buff->count -= count;
  } else {
    buff->count = 0;
  }
}


static bool _readEvent(tui_Event * event) {

  memset(event, 0, sizeof(tui_Event));
  event->type = TUI_ET_UNKNOWN;
  tui_Buffer* buff = &_ctx.input_buff;

  int count = read(fileno(stdin), buff->data + buff->count, buff->capacity - buff->count);
  if (count == 0) return false;

  buff->count += count;

  // FIXME: Remove it. Temproary code to print the input.
  // const char* c = buff->data + buff->count - count;
  // for (int i = 0; i < count; i++, c++) {
  //   if (!iscntrl(*c)) {
  //     printf("%i (%c)\n", *c, *c);
  //   } else {
  //     printf("%i\n", *c);
  //   }
  // }

  int event_length = 1; // Num of character for the event in the buffer.

  if (buff->data[0] == '\x1b') {
    event_length = _escapeLength(buff->data + 1, buff->count - 1) + 1;
    _parseEscapeSequence(buff->data, event_length, event);
    if (event->type == TUI_ET_MOUSE_MOVE) {
      if (_ctx.mouse_pos.x == event->mouse.x && _ctx.mouse_pos.y == event->mouse.y) {
        _buffShfit(buff, event_length); // Flush the event.
        return false;
      }
      _ctx.mouse_pos.x = event->mouse.x;
      _ctx.mouse_pos.y = event->mouse.y;
    }

  } else {
    _keyEvent(buff->data[0], event);
  }

  _buffShfit(buff, event_length);

  return event->type != TUI_ET_UNKNOWN;

}
#endif


void tui_endLoop() {
  _ctx.isdone = true;
}


#if defined(TUI_SYS_NIX)

#define _MAX_DBL_CLICK_WAIT .5 // 500 ms

typedef struct {

  // 0: initial state.
  // 1: left button down.
  // 2: left button up.
  int state;

  // time since state 2.
  clock_t dt;

} _dblClickStateMachine;

static inline bool _dcsmUpdateState(_dblClickStateMachine* dcsm, tui_Event* event) {
  switch (dcsm->state) {
    case 0: { // 0: initial state.
      if (event->type == TUI_ET_MOUSE_DOWN && event->mouse.button == TUI_MB_LEFT) {
        dcsm->state = 1;
        dcsm->dt = clock();
      }
    } break;

    case 1: { // 1: left button down, waiting for the button up.
      if (event->type == TUI_ET_MOUSE_UP && event->mouse.button == TUI_MB_LEFT) {
        if (((double)(clock() - dcsm->dt) / CLOCKS_PER_SEC) > _MAX_DBL_CLICK_WAIT) {
          dcsm->state = 0;
        } else {
          dcsm->state = 2;
          dcsm->dt = clock();
        }
      } else {
        dcsm->state = 0;
      }
    } break;

    case 2: { // 2: left button up, waiting for the second click.
      if (event->type == TUI_ET_MOUSE_DOWN && event->mouse.button == TUI_MB_LEFT) {
        dcsm->state = 1;
        if (((double) (clock() - dcsm->dt) / CLOCKS_PER_SEC) <= _MAX_DBL_CLICK_WAIT) {
          dcsm->state = 0;
          return true;
        }
      } else {
        dcsm->state = 0;
      }

    } break;
  }

  return false;
}
#undef _MAX_DBL_CLICK_WAIT

#endif // Double click state machine.

void tui_run(tui_Config* config) {
  _initialize(config);

  if (_ctx.config.event_callback == NULL) return;

#if defined(TUI_SYS_NIX)
  _dblClickStateMachine dcsm = { 0 };
#endif

  while (!_ctx.isdone) {
    tui_Event event;
    if (_readEvent(&event)) {

#if defined(TUI_SYS_NIX)
      if (_dcsmUpdateState(&dcsm, &event)) {
        event.type = TUI_ET_DOUBLE_CLICK;
      }
#endif

#if defined(TUI_SYS_WINDOWS)
      // On windows resizeing the window makes the cursor re appear.
      if (_ctx.config.hide_cursor) tui_hideCursor(NULL);
#endif

      _ctx.config.event_callback(&event);
    }
  }

  _finalize();
}

#endif // TUI_IMPL

#endif // TUI_H
