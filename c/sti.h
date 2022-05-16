#ifndef STI_H
#define STI_H

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
} sti_Vec2;


typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} sti_Color;


typedef struct {
  char* data;
  uint32_t count;
  uint32_t capacity;
} sti_Buffer;


typedef enum {
  STI_D_UP = 'A',
  STI_D_DOWN = 'B',
  STI_D_LEFT = 'D',
  STI_D_RIGHT = 'C',
} sti_Direction;

typedef enum {

  STI_KC_UNKNOWN = 0,

  STI_KC_0 = '0',
  STI_KC_1 = '1',
  STI_KC_2 = '2',
  STI_KC_3 = '3',
  STI_KC_4 = '4',
  STI_KC_5 = '5',
  STI_KC_6 = '6',
  STI_KC_7 = '7',
  STI_KC_8 = '8',
  STI_KC_9 = '9',

  STI_KC_A = 'A',
  STI_KC_B = 'B',
  STI_KC_C = 'C',
  STI_KC_D = 'D',
  STI_KC_E = 'E',
  STI_KC_F = 'F',
  STI_KC_G = 'G',
  STI_KC_H = 'H',
  STI_KC_I = 'I',
  STI_KC_J = 'J',
  STI_KC_K = 'K',
  STI_KC_L = 'L',
  STI_KC_M = 'M',
  STI_KC_N = 'N',
  STI_KC_O = 'O',
  STI_KC_P = 'P',
  STI_KC_Q = 'Q',
  STI_KC_R = 'R',
  STI_KC_S = 'S',
  STI_KC_T = 'T',
  STI_KC_U = 'U',
  STI_KC_V = 'V',
  STI_KC_W = 'W',
  STI_KC_X = 'X',
  STI_KC_Y = 'Y',
  STI_KC_Z = 'Z',

  STI_KC_ESC,
  STI_KC_ENTER,
  STI_KC_SPACE,
  STI_KC_HOME,
  STI_KC_END,
  STI_KC_PAGEUP,
  STI_KC_PAGEDOWN,
  STI_KC_LEFT,
  STI_KC_UP,
  STI_KC_RIGHT,
  STI_KC_DOWN,
  STI_KC_INSERT,
  STI_KC_DELETE,
  STI_KC_BACKSPACE,
  STI_KC_TAB,

  STI_KC_F1,
  STI_KC_F2,
  STI_KC_F3,
  STI_KC_F4,
  STI_KC_F5,
  STI_KC_F6,
  STI_KC_F7,
  STI_KC_F8,
  STI_KC_F9,
  STI_KC_F10,
  STI_KC_F11,
  STI_KC_F12,

} sti_KeyCode;


typedef enum {
  STI_MD_NONE = 0,
  STI_MD_CTRL = 0b001,
  STI_MD_ALT = 0b010,
  STI_MD_SHIFT = 0b100,
} sti_Modifiers;


typedef enum {
  STI_MB_UNKNOWN = 0,
  STI_MB_LEFT = 1,
  STI_MB_MIDDLE = 2,
  STI_MB_RIGHT = 3,
} sti_MouseBtn;


typedef enum {
  STI_ET_UNKNOWN = 0,
  STI_ET_KEY_DOWN,
  STI_ET_RESIZE,
  STI_ET_DOUBLE_CLICK,
  STI_ET_MOUSE_DOWN,
  STI_ET_MOUSE_UP,
  STI_ET_MOUSE_MOVE,
  STI_ET_MOUSE_DRAG,
  STI_ET_MOUSE_SCROLL,
} sti_EventType;


typedef struct {
  sti_KeyCode keycode;
  char ascii;
  sti_Modifiers modifiers;
} sti_EventKey;


typedef struct {
  sti_MouseBtn button;
  int x, y;
  sti_Direction scroll;
  sti_Modifiers modifiers;
} sti_EventMouse;


typedef struct {
  sti_EventType type;
  union {
    sti_EventKey key;
    sti_EventMouse mouse;
    sti_Vec2 resize;
  };
} sti_Event;


typedef void (*sti_eventCallback) (sti_Event*);
typedef void (*sti_callback) ();


typedef struct {
  const char* title;
  void* user_data;
  bool use_alternative_buffer;
  bool hide_cursor;
  bool capture_input_events;
  uint32_t fps;

  sti_eventCallback event_callback;
  sti_callback frame_callback;
  sti_callback init_callback;
  sti_callback clean_callback;

} sti_Config;


/*****************************************************************************/
/* STI PUBLIC API                                                            */
/*****************************************************************************/

sti_Config sti_newConfig();
void sti_run(sti_Config* config);
void sti_endLoop();

void* sti_getUserData();
void sti_setUserData(void* user_data);

sti_Color sti_colorHex(const char* color);
#define sti_vec2(x, y) (sti_Vec2) { (x), (y) }
#define sti_color(r, g, b) (sti_Color) { (r), (g), (b) }

void sti_bufferInit(sti_Buffer* buff);
void sti_bufferClean(sti_Buffer* buff);
void sti_write(sti_Buffer* buff, const char* data, uint32_t count);

void sti_char(char c);
void sti_charLen(char c, uint32_t len);
void sti_boxChar(char c);
void sti_boxCharLen(char c, uint32_t len);
void sti_string(const char* str);
void sti_stringLen(const char* str, uint32_t length);
void sti_int(int num);
void sti_fmt(const char* fmt, ...);

void sti_clear();
void sti_eraseTillEol();
void sti_eraseTillEof();
void sti_flush();

bool sti_isatty();
void sti_setTitle(const char* fmt, ...);
sti_Vec2 sti_getSize();
sti_Vec2 sti_getPosition();

void sti_setPosition(sti_Vec2 pos);
void sti_move(int n, sti_Direction direction);
void sti_hideCursor();
void sti_showCursor();

void sti_boldBegin();
void sti_boldEnd();
void sti_dimBegin();
void sti_dimEnd();
void sti_italicBegin();
void sti_italicEnd();
void sti_underlineBegin();
void sti_underlineEnd();
void sti_hiddenBegin();
void sti_hiddenEnd();
void sti_strikethroughBegin();
void sti_strikethroughEnd();

void sti_colorPush(sti_Color color);
void sti_colorPop();
void sti_colorBgPush(sti_Color color);
void sti_colorBgPop();

void sti_colorBegin(sti_Color col);
void sti_colorEnd();
void sti_colorBgBegin(sti_Color col);
void sti_colorBgEnd();

void sti_black();
void sti_blackBg();
void sti_red();
void sti_redBg();
void sti_green();
void sti_greenBg();
void sti_yellow();
void sti_yellowBg();
void sti_blue();
void sti_blueBg();
void sti_magenta();
void sti_magentaBg();
void sti_cyan();
void sti_cyanBg();
void sti_white();
void sti_whiteBg();
void sti_default();
void sti_defaultBg();

/*****************************************************************************/
/* STI IMPLEMENTATION                                                        */
/*****************************************************************************/

#if defined(STI_IMPL)

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
  #define STI_SYS_WINDOWS
#else
  #define STI_SYS_NIX
#endif


// Platform specific includes.
#if defined(STI_SYS_WINDOWS)
  #include <Windows.h>

#elif defined(STI_SYS_NIX)
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

#define STI_REALLOC(ptr, size) realloc(ptr, size)
#define STI_FREE(ptr) free(ptr)

// Maximum buffer size for sprintf to write an integer to it.
// INT_MAX = 10 digits, '+/-' = 1 digit, '\0' = 1 digit.
#define STI_INTBUFF_MAX 12

#define STI_EVENT_BUFF_SIZE 64
#define STI_INPUT_BUFF_SIZE 128
#define STI_COLOR_STACK_SIZE 256

#define STI_MAX(a, b) ((a) > (b) ? (a) : (b))
#define STI_MIN(a, b) ((a) < (b) ? (a) : (b))

#define _BETWEEN(a, c, b) ((a) <= (c) && (c) <= (b))

typedef struct {

#if defined(STI_SYS_WINDOWS)
  DWORD outmode, inmode; // Initial modes to restore.
  HANDLE h_stdout, h_stdin; // Handles.


#elif defined(STI_SYS_NIX)
  struct termios tio;
  sti_Buffer input_buff;
#endif

  sti_Vec2 screensize;
  sti_Vec2 mouse_pos;
  sti_Buffer screenbuff;

  int color_sp_fg, color_sp_bg; // Color stack pointers.
  sti_Color color_stack_fg[STI_COLOR_STACK_SIZE];
  sti_Color color_stack_bg[STI_COLOR_STACK_SIZE];

  bool initialized, isdone;
  sti_Config config;
  const char* err; // Contain error message if any.
} sti_Ctx;

static sti_Ctx _ctx;

static inline void _newScreenBuffer();
static inline void _restoreScreenBuffer();
static void _initTerminal();
static void _restoreTerminal();
static sti_Vec2 _getSize();

sti_Config sti_newConfig() {
  sti_Config config;
  memset(&config, 0, sizeof(config));
  config.use_alternative_buffer = false;
  config.fps = 60;
  return config;
}


void* sti_getUserData() {
  return _ctx.config.user_data;
}


void sti_setUserData(void* user_data) {
  _ctx.config.user_data = user_data;
}


static void _finalize(void) {
  if (_ctx.config.clean_callback) _ctx.config.clean_callback();

  sti_bufferClean(&_ctx.screenbuff);
  if (_ctx.config.hide_cursor) sti_showCursor();
  if (_ctx.config.use_alternative_buffer) _restoreScreenBuffer();
  if (_ctx.config.hide_cursor) sti_showCursor();
  _restoreTerminal();
  _ctx.initialized = false;
}


// Should be running in a tty otherwise it'll fail.
static void _initialize(sti_Config* config) {
  assert(sti_isatty());
  assert(!_ctx.initialized);

  memset(&_ctx, 0, sizeof(_ctx));
  _ctx.config = (config != NULL) ? *config : sti_newConfig();
  _ctx.isdone = false;

  _initTerminal();
  _ctx.screensize = _getSize();
  sti_bufferInit(&_ctx.screenbuff);

  if (_ctx.config.use_alternative_buffer) _newScreenBuffer();
  if (_ctx.config.hide_cursor) sti_hideCursor();

  atexit(_finalize);

  if (_ctx.config.init_callback) _ctx.config.init_callback();

  _ctx.initialized = true;
}


sti_Color sti_colorHex(const char* color) {
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

  return (sti_Color) {
    c[0] << 4 | c[1], // r
    c[2] << 4 | c[3], // g
    c[4] << 4 | c[5], // b
  };
}


void sti_bufferInit(sti_Buffer* buff) {
  assert(buff != NULL);
  buff->data = NULL;
  buff->count = 0;
  buff->capacity = 0;
}


void sti_bufferClean(sti_Buffer* buff) {
  assert(buff != NULL);
  buff->data = STI_REALLOC(buff->data, 0);
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


static void _bufferReserve(sti_Buffer* buff, uint32_t count) {
  assert(buff != NULL);
  if (buff->capacity >= count) return;
  count = _powerOf2Ceil(count);
  count = STI_MAX(8, count); // Minimum size = 8.
  buff->data = STI_REALLOC(buff->data, count);
  buff->capacity = count;
}


void sti_write(sti_Buffer* buff, const char* data, uint32_t count) {
  assert(buff != NULL);
  _bufferReserve(buff, buff->count + count);
  memcpy(buff->data + buff->count, data, (size_t)count);
  buff->count += count;
}


void sti_char(char c) {
  sti_Buffer* buff = &_ctx.screenbuff;
  _bufferReserve(buff, buff->count + 1);
  buff->data[buff->count++] = c;
}


void sti_charLen(char c, uint32_t len) {
  sti_Buffer* buff = &_ctx.screenbuff;
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
void sti_boxChar(char c) {
  sti_Buffer* buff = &_ctx.screenbuff;
  sti_string("\x1b(0");
  sti_char(c);
  sti_string("\x1b(B");
}


void sti_boxCharLen(char c, uint32_t len) {
  sti_Buffer* buff = &_ctx.screenbuff;
  sti_string("\x1b(0");
  sti_charLen(c, len);
  sti_string("\x1b(B");
}


void sti_string(const char* str) {
  sti_Buffer* buff = &_ctx.screenbuff;
  sti_write(buff, str, (uint32_t) strlen(str));
}


void sti_stringLen(const char* str, uint32_t length) {
  sti_Buffer* buff = &_ctx.screenbuff;
  sti_write(buff, str, length);
}


void sti_int(int num) {
  char int_buff[STI_INTBUFF_MAX];
  sprintf(int_buff, "%d", num);
  sti_string(int_buff);
}


void sti_fmt(const char* fmt, ...) {
  sti_Buffer* buff = &_ctx.screenbuff;
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


void sti_clear() {
  sti_string("\x1b[2J\x1b[H");
}


void sti_eraseTillEol() {
  sti_string("\x1b[K");
}


void sti_eraseTillEof() {
  sti_string("\x1b[0J");
}


void sti_flush() {
  sti_char('\0');
  sti_Buffer* buff = &_ctx.screenbuff;
  fprintf(stdout, "%s", buff->data);
  buff->count = 0;
}


// "\x1b[{escape}m"
static inline void _escape(const char* escape) {
  sti_fmt("\x1b[%sm", escape);
}


// "\x1b[{escape};2;{r};{g};{b}m" Note: %hhu = unsigned char.
static inline void _escapeCol(sti_Color col, const char* escape) {
  sti_fmt("\x1b[%s;2;%hhu;%hhu;%hhum", escape, col.r, col.g, col.b);
}


void sti_reset() {
  _escape("0"); // "\x1b[0m"
}


void sti_colorPush(sti_Color color) {
  assert(_ctx.color_sp_fg + 1 < STI_COLOR_STACK_SIZE);
  _ctx.color_stack_fg[++_ctx.color_sp_fg] = color;
  sti_colorBegin(color);
}


void sti_colorPop() {
  assert(_ctx.color_sp_fg > 0);
  --_ctx.color_sp_fg;
  if (_ctx.color_sp_fg == 0) sti_colorEnd();
  else sti_colorBegin(_ctx.color_stack_fg[_ctx.color_sp_fg]);
}


void sti_colorBgPush(sti_Color color) {
  assert(_ctx.color_sp_bg + 1 < STI_COLOR_STACK_SIZE);
  _ctx.color_stack_bg[++_ctx.color_sp_bg] = color;
  sti_colorBgBegin(color);
}


void sti_colorBgPop() {
  assert(_ctx.color_sp_bg > 0);
  --_ctx.color_sp_bg;
  if (_ctx.color_sp_bg == 0) sti_colorEnd();
  else sti_colorBgBegin(_ctx.color_stack_bg[_ctx.color_sp_bg]);
}


void sti_colorBegin(sti_Color col) {
  _escapeCol(col, "38");
}


void sti_colorEnd() {
  _escape("39");
}


void sti_colorBgBegin(sti_Color col) {
  _escapeCol(col, "48");
}


void sti_colorBgEnd() {
  _escape("49");
}


#define _STI_GRAPHICS_FN(name, esc_begin, esc_end) \
  void sti_##name##Begin() {                       \
    _escape(#esc_begin);                           \
  }                                                \
                                                   \
  void sti_##name##End() {                         \
    _escape(#esc_end);                             \
  }

_STI_GRAPHICS_FN(bold, 1, 22)
_STI_GRAPHICS_FN(dim, 2, 22)
_STI_GRAPHICS_FN(italic, 3, 23)
_STI_GRAPHICS_FN(underline, 4, 24)
_STI_GRAPHICS_FN(hidden, 8, 28)
_STI_GRAPHICS_FN(strikethrough, 9, 29)

#undef _STI_GRAPHICS_FN

#define _STI_COLOR_FN(name, fg, bg)       \
  void sti_##name() {                     \
    _escape(#fg);                         \
  }                                       \
                                          \
  void sti_##name##Bg() {                 \
    _escape(#bg);                         \
  }


_STI_COLOR_FN(black, 30, 40)
_STI_COLOR_FN(red, 31, 41)
_STI_COLOR_FN(green, 32, 42)
_STI_COLOR_FN(yellow, 33, 43)
_STI_COLOR_FN(blue, 34, 44)
_STI_COLOR_FN(magenta, 35, 45)
_STI_COLOR_FN(cyan, 36, 46)
_STI_COLOR_FN(white, 37, 47)
_STI_COLOR_FN(default, 39, 49)

#undef _STI_COLOR_FN


bool sti_isatty() {
  return (!!isatty(fileno(stdout))) && (!!isatty(fileno(stdin)));
}


// "\x1b]0;<title>\x07"
void sti_setTitle(const char* fmt, ...) {
  assert(fmt != NULL);
  va_list args;
  va_start(args, fmt);
  fprintf(stdout, "\x1b]0;");
  vfprintf(stdout, fmt, args);
  fprintf(stdout, "\x07");
  va_end(args);
}


static sti_Vec2 _getSize() {
  sti_Vec2 size;

#if defined(STI_SYS_WINDOWS)
  CONSOLE_SCREEN_BUFFER_INFO binfo;
  GetConsoleScreenBufferInfo(_ctx.h_stdout, &binfo);
  size.w = binfo.srWindow.Right - binfo.srWindow.Left + 1;
  size.y = binfo.srWindow.Bottom - binfo.srWindow.Top + 1;

#elif defined(STI_SYS_NIX)
  struct winsize wsize;
  ioctl(fileno(stdout), TIOCGWINSZ, &wsize);
  size.w = wsize.ws_col;
  size.h = wsize.ws_row;
#endif

  return size;
}


sti_Vec2 sti_getSize() {
  return _ctx.screensize;
}


sti_Vec2 sti_getPosition() {
  sti_Vec2 pos;

#if defined(STI_SYS_WINDOWS)
  CONSOLE_SCREEN_BUFFER_INFO binfo;
  GetConsoleScreenBufferInfo(_ctx.h_stdout, &binfo);
  pos.x = binfo.dwCursorPosition.X;
  pos.y = binfo.dwCursorPosition.Y;
  return pos;

#elif defined(STI_SYS_NIX)

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


void sti_setPosition(sti_Vec2 pos) {
  sti_fmt("\x1b[%i;%iH", pos.y + 1, pos.x + 1);
}


void sti_move(int n, sti_Direction direction) {
  sti_fmt("\x1b[%i%c", n, (char) direction);
}


void sti_hideCursor() {
  fprintf(stdout, "\x1b[?25l");
}


void sti_showCursor() {
  fprintf(stdout, "\x1b[?25h");
}


static inline void _newScreenBuffer() {
  fprintf(stdout, "\x1b[?1049h");
}


static inline void _restoreScreenBuffer() {
  fprintf(stdout, "\x1b[H\x1b[J"); // Clear screen and go to (0, 0).
  fprintf(stdout, "\x1b[?1049l");
}


// Platform specific _initTerminal() implementation.
#if defined(STI_SYS_WINDOWS)


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

#elif defined(STI_SYS_NIX)

static void _resizeHandler(int sig) {
  sti_Vec2 newsize = sti_getSize();
  if (_ctx.screensize.x == newsize.x && _ctx.screensize.y == newsize.y) return;
  _ctx.screensize = newsize;

  if (_ctx.config.event_callback != NULL) {
    sti_Event event;
    memset(&event, 0, sizeof(sti_Event));
    event.type = STI_ET_RESIZE;
    event.resize = _ctx.screensize;
    _ctx.config.event_callback(&event);
  }
}

static void _initTerminal() {

  // Backup the older termios.
  tcgetattr(fileno(stdin), &(_ctx.tio));

  sti_bufferInit(&_ctx.input_buff);
  _bufferReserve(&_ctx.input_buff, STI_INPUT_BUFF_SIZE);

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


#if defined(STI_SYS_WINDOWS)

static void _restoreTerminal() {
  SetConsoleMode(_ctx.h_stdout, _ctx.outmode);
  SetConsoleMode(_ctx.h_stdin, _ctx.inmode);
}

#elif defined(STI_SYS_NIX)

static void _restoreTerminal() {
  sti_bufferClean(&_ctx.input_buff);
  tcsetattr(fileno(stdin), TCSAFLUSH, &(_ctx.tio));
  fprintf(stdout, "\x1b[?1003l\x1b[?1006l\x1b[?25h");
}

#endif


#if defined(STI_SYS_WINDOWS)

// Returns false if the key should be ignored as an event (ex: shift, ctrl, ...).
static bool _toStiKeyCode(WORD vk, sti_KeyCode * kc) {

  if (0x30 <= vk && vk <= 0x39) { *kc = (sti_KeyCode)STI_KC_0 + (vk - 0x30); return true; }
  if (0x60 <= vk && vk <= 0x69) { *kc = (sti_KeyCode)STI_KC_0 + (vk - 0x60); return true; }
  if (0x41 <= vk && vk <= 0x5a) { *kc = (sti_KeyCode)STI_KC_A + (vk - 0x41); return true; }
  if (0x70 <= vk && vk <= 0x7b) { *kc = (sti_KeyCode)STI_KC_F1 + (vk - 0x70); return true; }

  // Note that shift, ctrl, alt keys are returned as STI_KC_UNKNOWN since *nix systems
  // don't support them.
  switch (vk) {
  case VK_BACK: *kc = STI_KC_BACKSPACE; break;
  case VK_TAB: *kc = STI_KC_TAB; break;
  case VK_RETURN: *kc = STI_KC_ENTER; break;
  case VK_ESCAPE: *kc = STI_KC_ESC; break;
  case VK_SPACE: *kc = STI_KC_SPACE; break;
  case VK_PRIOR: *kc = STI_KC_PAGEUP; break;
  case VK_NEXT: *kc = STI_KC_PAGEDOWN; break;
  case VK_END: *kc = STI_KC_END; break;
  case VK_HOME: *kc = STI_KC_HOME; break;
  case VK_LEFT: *kc = STI_KC_LEFT; break;
  case VK_RIGHT: *kc = STI_KC_RIGHT; break;
  case VK_UP: *kc = STI_KC_UP; break;
  case VK_DOWN: *kc = STI_KC_DOWN; break;
  case VK_INSERT: *kc = STI_KC_INSERT; break;
  case VK_DELETE: *kc = STI_KC_DELETE; break;

  case VK_SHIFT:
  case VK_CONTROL:
  case VK_MENU:
  case VK_PAUSE:
  case VK_CAPITAL: //< Capslock.
    return false;
  }

  return true;
}

static bool _readEvent(sti_Event* event) {

  memset(event, 0, sizeof(sti_Event));
  event->type = STI_ET_UNKNOWN;

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

      if (!_toStiKeyCode(ker->wVirtualKeyCode, &event->key.keycode)) return false;

      event->type = STI_ET_KEY_DOWN;
      event->key.ascii = ker->uChar.AsciiChar;

      if ((ker->dwControlKeyState & LEFT_ALT_PRESSED) || (ker->dwControlKeyState & RIGHT_ALT_PRESSED))
        event->key.modifiers |= STI_MD_ALT;
      if ((ker->dwControlKeyState & LEFT_CTRL_PRESSED) || (ker->dwControlKeyState & RIGHT_CTRL_PRESSED))
        event->key.modifiers |= STI_MD_CTRL;
      if (ker->dwControlKeyState & SHIFT_PRESSED)
        event->key.modifiers |= STI_MD_SHIFT;

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
          event->mouse.button = STI_MB_LEFT;

        } else if (change & RIGHTMOST_BUTTON_PRESSED) {
          pressed = mer->dwButtonState & RIGHTMOST_BUTTON_PRESSED;
          event->mouse.button = STI_MB_RIGHT;

        } else if (change & FROM_LEFT_2ND_BUTTON_PRESSED) {
          pressed = mer->dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED;
          event->mouse.button = STI_MB_MIDDLE;
        }
      }
      last_state = mer->dwButtonState;

      event->mouse.x = mer->dwMousePosition.X;
      event->mouse.y = mer->dwMousePosition.Y;

      if (mer->dwEventFlags == 0) {
        event->type = (pressed) ? STI_ET_MOUSE_DOWN : STI_ET_MOUSE_UP;

      } else if (mer->dwEventFlags & MOUSE_MOVED) {
        if (_ctx.mouse_pos.x == event->mouse.x && _ctx.mouse_pos.y == event->mouse.y) {
          return false;
        }
        event->type = (pressed) ? STI_ET_MOUSE_DRAG : STI_ET_MOUSE_MOVE;

      } else if (mer->dwEventFlags & MOUSE_WHEELED) {
        event->type = STI_ET_MOUSE_SCROLL;
        event->mouse.scroll = (mer->dwButtonState & 0xFF000000) ? STI_D_DOWN : STI_D_UP;

      } else if (mer->dwEventFlags & DOUBLE_CLICK) {
        event->type = STI_ET_DOUBLE_CLICK;
      }

      _ctx.mouse_pos.x = event->mouse.x;
      _ctx.mouse_pos.y = event->mouse.y;

      if ((mer->dwControlKeyState & LEFT_ALT_PRESSED) || (mer->dwControlKeyState & RIGHT_ALT_PRESSED))
        event->mouse.modifiers |= STI_MD_ALT;
      if ((mer->dwControlKeyState & LEFT_CTRL_PRESSED) || (mer->dwControlKeyState & RIGHT_CTRL_PRESSED))
        event->mouse.modifiers |= STI_MD_CTRL;
      if (mer->dwControlKeyState & SHIFT_PRESSED)
        event->mouse.modifiers |= STI_MD_SHIFT;


    } break;

    case WINDOW_BUFFER_SIZE_EVENT: {
      WINDOW_BUFFER_SIZE_RECORD* wbs = &ir.Event.WindowBufferSizeEvent;
      event->type = STI_ET_RESIZE;
      sti_Vec2 newsize = sti_vec2(wbs->dwSize.X, wbs->dwSize.Y);
      if (_ctx.screensize.x == newsize.x && _ctx.screensize.y == newsize.y) return false;
      _ctx.screensize = newsize;
      event->resize = _ctx.screensize;
    } break;

    // Not handling as it's not available in *nix.
    case MENU_EVENT:
    case FOCUS_EVENT:
      return false;
  }

  return event->type != STI_ET_UNKNOWN;
}

#elif defined(STI_SYS_NIX)

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


static void _keyEvent(char c, sti_Event * event) {
  event->type = STI_ET_KEY_DOWN;
  event->key.ascii = c;

  // Note: Ctrl+M and <enter> both reads as '\r'.
  if (c == '\r') { event->key.keycode = STI_KC_ENTER; return; }
  if (c == 127) { event->key.keycode = STI_KC_BACKSPACE; return; }
  if (c == 9) { event->key.keycode = STI_KC_TAB; return; }
  if (c == 32) { event->key.keycode = STI_KC_SPACE; return; }

  event->key.keycode = (sti_KeyCode)c;

  // Ctrl + key
  if (1 <= c && c <= 26) {
    event->key.modifiers |= STI_MD_CTRL;
    event->key.keycode = (sti_KeyCode)('A' + (c - 1));

  } else if (_BETWEEN('a', c, 'z') || _BETWEEN('A', c, 'Z') || _BETWEEN('0', c, '9')) {
    event->key.keycode = (sti_KeyCode)c;
    if (_BETWEEN('A', c, 'Z')) event->key.modifiers |= STI_MD_SHIFT;
  }
}


// Reference: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h2-Mouse-Tracking
static void _mouseEvent(const char* buff, uint32_t count, sti_Event * event) {

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
  if (high & 0b001) event->mouse.modifiers |= STI_MD_SHIFT;
  if (high & 0b010) event->mouse.modifiers |= STI_MD_ALT;
  if (high & 0b100) event->mouse.modifiers |= STI_MD_CTRL;

  event->mouse.x = cx - 1;
  event->mouse.y = cy - 1;

  switch (type) {
    case 0: {
      event->type = (m == 'M') ? STI_ET_MOUSE_DOWN : STI_ET_MOUSE_UP;
      event->mouse.button = (sti_MouseBtn)(low + 1);
    } break;

    case 1: {
      if (low == 0b11) {  // Mouse move.
        event->type = STI_ET_MOUSE_MOVE;

      } else { // Drag.
        event->type = STI_ET_MOUSE_DRAG;
        event->mouse.button = (sti_MouseBtn)(low + 1);
      }
    } break;

    case 2: { // Scroll.
      event->type = STI_ET_MOUSE_SCROLL;

      if (low == 0) event->mouse.scroll = STI_D_UP;
      else if (low == 1) event->mouse.scroll = STI_D_DOWN;

    } break;
  }


}

void _parseEscapeSequence(const char* buff, uint32_t count, sti_Event * event) {
  assert(buff[0] == '\x1b');

  if (count == 1) {
    event->type = STI_ET_KEY_DOWN;
    event->key.ascii = *buff;
    event->key.keycode = STI_KC_ESC;
    return;
  }

  if (count == 2) {
    _keyEvent(buff[1], event);
    event->key.modifiers |= STI_MD_ALT;
    return;
  }


#define _SET_KEY(code)              \
  do {                              \
    event->type = STI_ET_KEY_DOWN;  \
    event->key.keycode = code;      \
  } while (false)


#define _MATCH(str) ((count >= strlen(str) + 1) && (strncmp(buff + 1, (str), strlen(str)) == 0))

  if (_MATCH("[<")) _mouseEvent(buff + 3, count - 3, event);

  else if (_MATCH("[A") || _MATCH("OA")) _SET_KEY(STI_KC_UP);
  else if (_MATCH("[B") || _MATCH("OB")) _SET_KEY(STI_KC_DOWN);
  else if (_MATCH("[C") || _MATCH("OC")) _SET_KEY(STI_KC_RIGHT);
  else if (_MATCH("[D") || _MATCH("OD")) _SET_KEY(STI_KC_LEFT);
  else if (_MATCH("[5~") || _MATCH("[[5~")) _SET_KEY(STI_KC_PAGEUP);
  else if (_MATCH("[6~") || _MATCH("[[6~")) _SET_KEY(STI_KC_PAGEDOWN);

  else if (_MATCH("[H") || _MATCH("OH") || _MATCH("[1~") || _MATCH("[[7~")) _SET_KEY(STI_KC_HOME);
  else if (_MATCH("[F") || _MATCH("OF") || _MATCH("[4~") || _MATCH("[[8~")) _SET_KEY(STI_KC_END);

  else if (_MATCH("[2~")) _SET_KEY(STI_KC_INSERT);
  else if (_MATCH("[3~")) _SET_KEY(STI_KC_DELETE);

  else if (_MATCH("OP") || _MATCH("[11~")) _SET_KEY(STI_KC_F1);
  else if (_MATCH("OQ") || _MATCH("[12~")) _SET_KEY(STI_KC_F2);
  else if (_MATCH("OR") || _MATCH("[13~")) _SET_KEY(STI_KC_F3);
  else if (_MATCH("OS") || _MATCH("[14~")) _SET_KEY(STI_KC_F4);

  else if (_MATCH("[15~")) _SET_KEY(STI_KC_F5);
  else if (_MATCH("[17~")) _SET_KEY(STI_KC_F6);
  else if (_MATCH("[18~")) _SET_KEY(STI_KC_F7);
  else if (_MATCH("[19~")) _SET_KEY(STI_KC_F8);
  else if (_MATCH("[20~")) _SET_KEY(STI_KC_F9);
  else if (_MATCH("[21~")) _SET_KEY(STI_KC_F10);
  else if (_MATCH("[23~")) _SET_KEY(STI_KC_F11);
  else if (_MATCH("[24~")) _SET_KEY(STI_KC_F12);

#undef _MATCH
#undef _SET_KEY

}


static void _buffShfit(sti_Buffer* buff, uint32_t count) {
  assert(buff != NULL);
  if (count < buff->count) {
    memmove(buff->data, buff->data + count, buff->count - count);
    buff->count -= count;
  } else {
    buff->count = 0;
  }
}


static bool _readEvent(sti_Event * event) {

  memset(event, 0, sizeof(sti_Event));
  event->type = STI_ET_UNKNOWN;
  sti_Buffer* buff = &_ctx.input_buff;

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
    if (event->type == STI_ET_MOUSE_MOVE) {
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

  return event->type != STI_ET_UNKNOWN;

}
#endif


void sti_endLoop() {
  _ctx.isdone = true;
}


#if defined(STI_SYS_NIX)

#define _MAX_DBL_CLICK_WAIT .5 // 500 ms

typedef struct {

  // 0: initial state.
  // 1: left button down.
  // 2: left button up.
  int state;

  // time since state 2.
  clock_t dt;

} _dblClickStateMachine;

static inline bool _dcsmUpdateState(_dblClickStateMachine* dcsm, sti_Event* event) {
  switch (dcsm->state) {
    case 0: { // 0: initial state.
      if (event->type == STI_ET_MOUSE_DOWN && event->mouse.button == STI_MB_LEFT) {
        dcsm->state = 1;
        dcsm->dt = clock();
      }
    } break;

    case 1: { // 1: left button down, waiting for the button up.
      if (event->type == STI_ET_MOUSE_UP && event->mouse.button == STI_MB_LEFT) {
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
      if (event->type == STI_ET_MOUSE_DOWN && event->mouse.button == STI_MB_LEFT) {
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

void sti_run(sti_Config* config) {

  _initialize(config);

  if (_ctx.config.fps == 0) _ctx.config.fps = 60;
  if (_ctx.config.title) sti_setTitle("%s", _ctx.config.title);

  if (_ctx.config.event_callback == NULL && _ctx.config.frame_callback == NULL) {
    return;
  }

#if defined(STI_SYS_NIX)
  _dblClickStateMachine dcsm = { 0 };
#endif

  while (!_ctx.isdone) {

    clock_t start = clock();

    sti_Event event;
    if (_ctx.config.event_callback && _readEvent(&event)) {

#if defined(STI_SYS_NIX)
      if (_dcsmUpdateState(&dcsm, &event)) {
        event.type = STI_ET_DOUBLE_CLICK;
      }
#endif

#if defined(STI_SYS_WINDOWS)
      // FIXME: On windows resizeing the window makes the cursor re appear.
      if (_ctx.config.hide_cursor) sti_hideCursor();
#endif

      _ctx.config.event_callback(&event);
    }

    if (_ctx.config.frame_callback) _ctx.config.frame_callback();

    clock_t et = ((clock() - start) * 1000) / CLOCKS_PER_SEC; // elapsed time in ms.
    clock_t ft = (1000 / _ctx.config.fps); // frame time in ms.

    int fps = (et > 0) ? (int) (1000.f / (float) et) : INT_MAX;
    if (fps > (int) _ctx.config.fps) fps = (_ctx.config.fps);
    if (_ctx.config.title) sti_setTitle("%s | %i fps", _ctx.config.title, fps);
    else sti_setTitle("%i fps", fps);

    if (ft > et) {
#if defined(_MSC_VER) || defined(__TINYC__) // Sleep(milli seconds)
      Sleep(ft - et);

#else // usleep(micro seconds)
      usleep((ft - et) * 1000);
#endif
    }

  }

  _finalize();
}

#endif // STI_IMPL

#endif // STI_H
