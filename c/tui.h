
#error "WIP"

#include <ctype.h>

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <locale.h>
#include <wchar.h>


// Compiler warnings.
#ifdef _MSC_VER
  #pragma warning(disable:6308)
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
#endif


// We define TUI_CC_VC_TCC for MSVC, TCC running on windows. They does't
// provide some POSIX compliant headres like <unistd.h>, <dirent.h>, etc.
#if defined(_MSC_VER) || (defined(_WIN32) && defined(__TINYC__))
  #define TUI_CC_VC_TCC
#endif


#if defined(TUI_CC_VC_TCC)
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

#define TUI_MAX(a, b) ((a) > (b) ? (a) : (b))
#define TUI_MIN(a, b) ((a) < (b) ? (a) : (b))

#define tui_vec2(x, y) (tui_Vec2) { (x), (y) }

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

  TUI_KC_ENTER = '\r',

  TUI_KC_0 = '0',
  TUI_KC_1,
  TUI_KC_2,
  TUI_KC_3,
  TUI_KC_4,
  TUI_KC_5,
  TUI_KC_6,
  TUI_KC_7,
  TUI_KC_8,
  TUI_KC_9,

  TUI_KC_A = 'A',
  TUI_KC_B,
  TUI_KC_C,
  TUI_KC_D,
  TUI_KC_E,
  TUI_KC_F,
  TUI_KC_G,
  TUI_KC_H,
  TUI_KC_I,
  TUI_KC_J,
  TUI_KC_K,
  TUI_KC_L,
  TUI_KC_M,
  TUI_KC_N,
  TUI_KC_O,
  TUI_KC_P,
  TUI_KC_Q,
  TUI_KC_R,
  TUI_KC_S,
  TUI_KC_T,
  TUI_KC_U,
  TUI_KC_V,
  TUI_KC_W,
  TUI_KC_X,
  TUI_KC_Y,
  TUI_KC_Z,

  TUI_KC_END = 0x23,
  TUI_KC_HOME = 0x24,
  TUI_KC_LEFT = 0x25,
  TUI_KC_UP = 0x26,
  TUI_KC_RIGHT = 0x27,
  TUI_KC_DOWN = 0x28,
  TUI_KC_INSERT = 0x2D,
  TUI_KC_DELETE = 0x2E,

  TUI_KC_F1 = 0x70,
  TUI_KC_F2 = 0x71,
  TUI_KC_F3 = 0x72,
  TUI_KC_F4 = 0x73,
  TUI_KC_F5 = 0x74,
  TUI_KC_F6 = 0x75,
  TUI_KC_F7 = 0x76,
  TUI_KC_F8 = 0x77,
  TUI_KC_F9 = 0x78,
  TUI_KC_F10 = 0x79,
  TUI_KC_F11 = 0x7A,
  TUI_KC_F12 = 0x7B,
  TUI_KC_F13 = 0x7C,
  TUI_KC_F14 = 0x7D,
  TUI_KC_F15 = 0x7E,
  TUI_KC_F16 = 0x7F,
  TUI_KC_F17 = 0x80,
  TUI_KC_F18 = 0x81,
  TUI_KC_F19 = 0x82,
  TUI_KC_F20 = 0x83,
  TUI_KC_F21 = 0x84,
  TUI_KC_F22 = 0x85,
  TUI_KC_F23 = 0x86,
  TUI_KC_F24 = 0x87,

} tui_KeyCode;


typedef enum {
  TUI_KM_NONE  = 0,
  TUI_KM_CTRL  = 1 << 0,
  TUI_KM_ALT   = 1 << 1,
  TUI_KM_SHIFT = 1 << 2,
} tui_KeyModifiers;


typedef enum {
  TUI_MB_UNKNOWN = 0,
  TUI_MB_LEFT,
  TUI_MB_RIGHT,
  TUI_MB_MIDDLE,
} tui_MouseBtn;


typedef enum {
  TUI_ET_UNKNOWN = 0,
  TUI_ET_KEY_DOWN,
  TUI_ET_RESIZE,
  TUI_ET_MOUSE_DOWN,
  TUI_ET_MOUSE_UP,
  TUI_ET_MOUSE_MOVE,
  TUI_ET_MOUSE_DRAG,
  TUI_ET_MOUSE_SCROLL,
} tui_EventType;


typedef struct {
  tui_KeyCode keycode;
  char ascii;
  tui_KeyModifiers modifiers;
} tui_EventKey;


typedef struct {
  tui_MouseBtn button;
  tui_Vec2 position;
  tui_Direction scroll;
} tui_EventMouse;


typedef struct {
  tui_EventType type;
  union {
    tui_EventKey key;
    tui_EventMouse mouse;
    tui_Vec2 resize;
  };
} tui_Event;


typedef struct {
  void* user_data;
  bool use_alternative_buffer;
  bool hide_cursor;
  bool capture_input_events;
} tui_Config;


typedef struct {

#if defined(TUI_SYS_WINDOWS)
  DWORD outmode, inmode; // Initial modes to restore.
  HANDLE h_stdout, h_stdin; // Handles.

#elif defined(TUI_SYS_NIX)
  struct termios tio;
  tui_Buffer input_buff;
#endif

  int event_buff_count;
  tui_Event event_buff[TUI_EVENT_BUFF_SIZE];

  tui_Config config;
  const char* err; // Contain error message if any.
} tui_Ctx;


/*****************************************************************************/
/* TUI PUBLIC API                                                            */
/*****************************************************************************/


void tui_bufferInit(tui_Buffer* buff);
void tui_bufferClean(tui_Buffer* buff);
void tui_write(tui_Buffer* buff, const char* data, uint32_t count);
void tui_char(tui_Buffer* buff, char c);
void tui_boxChar(tui_Buffer* buff, char c);
void tui_string(tui_Buffer* buff, const char* str);
void tui_int(tui_Buffer* buff, int num);
void tui_fmt(tui_Buffer* buff, const char* fmt, ...);

void tui_clear(tui_Buffer* buff);
void tui_eraseTillEol(tui_Buffer* buff);

tui_Config tui_newConfig();
void tui_initialize(tui_Config* config);
void tui_finalize(void);

bool tui_isatty();
void tui_setTitle(const char* title);
tui_Vec2 tui_getSize();
tui_Vec2 tui_getPosition();
void tui_setPosition(tui_Vec2 pos);
void tui_move(int n, tui_Direction direction);
void tui_hideCursor();
void tui_showCursor();


/*****************************************************************************/
/* TUI IMPLEMENTATION                                                        */
/*****************************************************************************/

// _CTRL_KEY('a') or _CTRL_KEY('A') == Ctrl+A
#define _CTRL_KEY(k) ((k) & 0x1f)

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


// Should be running in a tty otherwise it'll fail.
void tui_initialize(tui_Config* config) {
  assert(tui_isatty(NULL));
  memset(&_ctx, 0, sizeof(_ctx));
  _ctx.config = (config != NULL) ? *config : tui_newConfig();
  _initTerminal();
  if (_ctx.config.use_alternative_buffer) _newScreenBuffer();
  if (_ctx.config.hide_cursor) tui_hideCursor();
  atexit(tui_finalize);
}


void tui_finalize(void) {
  _restoreTerminal();
  if (_ctx.config.use_alternative_buffer) _restoreScreenBuffer();
  if (_ctx.config.hide_cursor) tui_showCursor();
}


void tui_bufferInit(tui_Buffer* buff) {
  buff->data = NULL;
  buff->count = 0;
  buff->capacity = 0;
}


void tui_bufferClean(tui_Buffer* buff) {
  buff->data = TUI_REALLOC(buff->data, 0);
  buff->count = 0;
  buff->capacity = 0;
}


static void _buffSfhit(tui_Buffer* buff, uint32_t count) {
  if (count < buff->count) {
    memmove(buff->data, buff->data + count, buff->count - count);
    buff->count -= count;
  } else {
    buff->count = 0;
  }
}


// FIXME: remove this.
static void _printEvent(tui_Event* event) {
  switch (event->type) {
    case TUI_ET_UNKNOWN:
      printf("event.unknown\n");
      return;

    case TUI_ET_KEY_DOWN: {
      printf("event.key_down\n");
      printf("event.keycode = %i\n", event->key.keycode);
      if (!iscntrl(event->key.keycode)) {
        printf("event.ascii = %c\n", event->key.ascii);
      }
      return;
    }

    case TUI_ET_RESIZE: {
      printf("event.resize\n");
      printf("event.size = (%i, %i)\n", event->resize.w, event->resize.h);
      return;
    }

    case TUI_ET_MOUSE_DOWN:
      printf("event.mouse_down\n");
      return;
    case TUI_ET_MOUSE_UP:
      printf("event.mouse_up\n");
      return;
    case TUI_ET_MOUSE_MOVE:
      printf("event.mouse_move\n");
      return;
    case TUI_ET_MOUSE_DRAG:
      printf("event.drag\n");
      return;
    case TUI_ET_MOUSE_SCROLL:
      printf("event.scroll\n");
      return;
  }
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
  if (buff->capacity >= count) return;
  count = _powerOf2Ceil(count);
  count = TUI_MAX(8, count); // Minimum size = 8.
  buff->data = TUI_REALLOC(buff->data, count);
  buff->capacity = count;
}


void tui_write(tui_Buffer* buff, const char* data, uint32_t count) {
  _bufferReserve(buff, buff->count + count);
  memcpy(buff->data + buff->count, data, (size_t)count);
  buff->count += count;
}


void tui_char(tui_Buffer* buff, char c) {
  _bufferReserve(buff, buff->count + 1);
  buff->data[buff->count++] = c;
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
void tui_boxChar(tui_Buffer* buff, char c) {
  tui_string(buff, "\x1b(0");
  tui_char(buff, c);
  tui_string(buff, "\x1b(B");
}


void tui_string(tui_Buffer* buff, const char* str) {
  tui_write(buff, str, (uint32_t)strlen(str));
}


void tui_int(tui_Buffer* buff, int num) {
  char int_buff[TUI_INTBUFF_MAX];
  sprintf(int_buff, "%d", num);
  tui_string(buff, int_buff);
}


void tui_fmt(tui_Buffer* buff, const char* fmt, ...) {
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


void tui_clear(tui_Buffer* buff) {
  tui_string(buff, "\x1b[H\x1b[J");
}


void tui_eraseTillEol(tui_Buffer* buff) {
  tui_string(buff, "\x1b[K");
}


// "\x1b[{escape}m"
static inline void _escape(tui_Buffer* buff, const char* escape) {
  tui_fmt(buff, "\x1b[%sm", escape);
}


// "\x1b[{escape};2;{r};{g};{b}m" Note: %hhu = unsigned char.
static inline void _escapeCol(tui_Buffer* buff, tui_Color col, const char* escape) {
  tui_fmt(buff, "\x1b[%s;2;%hhu;%hhu;%hhum", escape, col.r, col.g, col.b);
}


void tui_reset(tui_Buffer* buff) {
  _escape(buff, "0"); // "\x1b[0m"
}


void tui_colorBegin(tui_Buffer* buff, tui_Color col) {
  _escapeCol(buff, col, "38");
}


void tui_colorEnd(tui_Buffer* buff) {
  _escape(buff, "39");
}


void tui_colorBgBegin(tui_Buffer* buff, tui_Color col) {
  _escapeCol(buff, col, "48");
}


void tui_colorBgEnd(tui_Buffer* buff) {
  _escape(buff, "49");
}


#define _TUI_GRAPHICS_FN(name, esc_begin, esc_end) \
  void tui_##name##Begin(tui_Buffer* buff) {       \
    _escape(buff, #esc_begin);                     \
  }                                                \
                                                   \
  void tui_##name##End(tui_Buffer* buff) {         \
    _escape(buff, #esc_end);                       \
  }

_TUI_GRAPHICS_FN(bold, 1, 22)
_TUI_GRAPHICS_FN(dim, 2, 22)
_TUI_GRAPHICS_FN(italic, 3, 23)
_TUI_GRAPHICS_FN(underline, 4, 24)
_TUI_GRAPHICS_FN(hidden, 8, 28)
_TUI_GRAPHICS_FN(strikethrough, 9, 29)

#undef _TUI_GRAPHICS_FN

#define _TUI_COLOR_FN(name, fg, bg)       \
  void tui_##name(tui_Buffer* buff) {     \
    _escape(buff, #fg);                   \
  }                                       \
                                          \
  void tui_##name##Bg(tui_Buffer* buff) { \
    _escape(buff, #bg);                   \
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
  size.x = binfo.dwSize.X;
  size.y = binfo.dwSize.Y;

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

    if ('0' <= c && c <= '9') {
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
  fprintf(stdout, "\x1b[%i;%iH", pos.y + 1, pos.x + 1);
}


void tui_move(int n, tui_Direction direction) {
  fprintf(stdout, "\x1b[%i%c", n, (char) direction);
}


void tui_hideCursor() {
  fprintf(stdout, "\x1b[?25l");
}


void tui_showCursor() {
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
static void _initTerminal() {

  // Backup the older termios.
  tcgetattr(fileno(stdin), &(_ctx.tio));

  tui_bufferInit(&_ctx.input_buff);
  _bufferReserve(&_ctx.input_buff, TUI_INPUT_BUFF_SIZE);

  struct termios tio;
  tcgetattr(fileno(stdin), &tio);


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


// Returns the length of an escape sequence.
static int _escapeLength(const char* buff, uint32_t size) {
  int length = 0;

  while (length < size) {
    char c = buff[length++];

    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '~')) {
      printf("[el] -- %c\n", c);
      if (c == 'O' && length < size) {
        c = buff[length];
        if ((c >= 'A' && c <= 'D') || (c >= 'P' && c <= 'S') || c == 'F' || c == 'H') {
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

#if defined(TUI_SYS_WINDOWS)

static void _ReadEvent(tui_Event* event) {

  memset(event, 0, sizeof(tui_Event));
  event->type = TUI_ET_UNKNOWN;

  // Dispatch prevous events first to make room for the newer ones.
  if (_ctx.event_buff_count >= TUI_EVENT_BUFF_SIZE) {
    return;
  }

  //tui_Event* event = &_ctx.event_buff[_ctx.event_buff_count];
  //event->type = TUI_ET_UNKNOWN;

  DWORD count;
  if (!GetNumberOfConsoleInputEvents(_ctx.h_stdin, &count)) {
    // ERROR:
  }

  INPUT_RECORD buff[64];

  if (count >= 1) {
    ReadConsoleInput(_ctx.h_stdin, buff, 64, &count);
    for (int i = 0; i < (int) count; i++) {

      switch (buff[i].EventType) {
      case KEY_EVENT: // keyboard input
        printf("keyevent\n");
        break;

      case MOUSE_EVENT: // mouse input
        printf("mouse ev\n");
        break;

      case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
        printf("buff ev\n");
        break;

        //case FOCUS_EVENT:  // disregard focus events
        //case MENU_EVENT:   // disregard menu events

      }

    }

  }

}

#elif defined(TUI_SYS_NIX)

static void _readEvent(tui_Event* event) {
#define match(p, c) ((*(p) == (c)) && ((p)++, true))
#define next(p) (*((p)++))

  memset(event, 0, sizeof(tui_Event));
  event->type = TUI_ET_UNKNOWN;
  tui_Buffer* buff = &_ctx.input_buff;

  buff->count += read(fileno(stdin), buff->data + buff->count, buff->capacity - buff->count);
  if (buff->count == 0) return;

  int event_length = 1; // Num of character for the event in the buffer.

  if (buff->data[0] == '\x1b') {
    event_length = _escapeLength(buff->data + 1, buff->count - 1);


  } else {

  }

  
  _buffSfhit(buff, event_length);



  const char* c = buff->data;

  if (match(c, '\x1b')) {
    _escapeLength(c, )
  }

  if (match(c, '\x1b')) {
    if (match(c, '[')) {
      switch (next(c)) {
        case 'A':{
          event->type = TUI_ET_KEY_DOWN;
          event->key.keycode = TUI_KC_UP;
        } break;

        case 'B': {
          event->type = TUI_ET_KEY_DOWN;
          event->key.keycode = TUI_KC_DOWN;
        } break;

        case 'C':{
          event->type = TUI_ET_KEY_DOWN;
          event->key.keycode = TUI_KC_RIGHT;
        } break;

        case 'D':{
          event->type = TUI_ET_KEY_DOWN;
          event->key.keycode = TUI_KC_LEFT;
        } break;

        default:
          break;
      }
    }

    // Since Ctrl+M and <enter> both reads as '\r'.
  } else if (match(c, '\r')) {
      event->type = TUI_ET_KEY_DOWN;
      event->key.keycode = TUI_KC_ENTER;

  } else {

    event->type = TUI_ET_KEY_DOWN;
    event->key.ascii = *c;

    // 
    if (1 <= *c && *c <= 26) {
      event->key.modifiers |= TUI_KM_CTRL;
      event->key.keycode = TUI_KC_A + *c - 1;
    }

  }

#undef match
#undef next

}
#endif


void foobar () {
  tui_Event* event;

  
  
  tui_Buffer* buff;

  

  
  
  
}

















void _drawBox(tui_Buffer* buff) {

  tui_Vec2 size = tui_getSize();
  int l = (int) (size.x - strlen("test") - 2);
  printf("x = %i\n", size.x);

  tui_boxChar(buff, 'l');
  for (int i = 0; i < size.x - 2; i++) {
    tui_boxChar(buff, 'q');
  }
  tui_boxChar(buff, 'k');
  
  tui_char(buff, '\n');
  
  tui_boxChar(buff, 'x');
  {
    tui_fmt(buff, "%*.*s", l, l, " ");
    tui_cyanBg(buff);
    tui_string(buff, "test");
    tui_colorBgEnd(buff);
  }
  tui_boxChar(buff, 'x');
  
  tui_char(buff, '\n');
  
  tui_boxChar(buff, 'm');
  for (int i = 0; i < size.x - 2; i++) {
    tui_boxChar(buff, 'q');
  }
  tui_boxChar(buff, 'j');
}

void test() {

  tui_Buffer _buff, * buff;
  buff = &_buff;
  tui_bufferInit(buff);
  {

    tui_setPosition(tui_vec2(0, 0));
    tui_hideCursor();

    _drawBox(buff);

    tui_red(buff);
    tui_underlineBegin(buff);
    tui_string(buff, "\nhello");
    tui_underlineEnd(buff);
    tui_colorEnd(buff);

    tui_green(buff);
    tui_string(buff, " world");
    tui_colorEnd(buff);
    tui_char(buff, '\0');
    fprintf(stdout, "%s\n", buff->data);

  }
  tui_bufferClean(buff);

}

int main() {

  setlocale(LC_ALL, "");

  tui_Config config = tui_newConfig();
  config.use_alternative_buffer = false;
  config.capture_input_events = true;

  tui_initialize(&config);

  /*
  //tui_Event event;
  while (1) {
    tui_Event event;
    _ReadEvent(&event);
    //_printEvent();
  }
  //*/

  /*
  while (1) {
    char c = getchar();
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == _CTRL_KEY('q')) break;
  }
  //*/

  /*
  while (1) {
    char buff[100];
    int l = read(fileno(stdin), buff, 100);
    if (l < 1) continue;
    if (buff[0] == _CTRL_KEY('q')) break;
    
    for (int i = 0; i < l; i++) {
      char c = buff[i];
      if (iscntrl(c)) {
        printf("%d\r\n", c);
      } else {
        printf("%d ('%c')\r\n", c, c);
      }
    }

    if (buff[0] == '\x1b') {
      printf("escapelength = %i\n", _escapeLength(buff+1, 99));
    }
  }
  //*/

  tui_finalize();



  return 0;
}
