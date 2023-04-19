// vim: ts=2 sw=2 expandtab
#pragma once
#include "TinyString.h"
#include <string>
#include <deque>
#include <Stream.h>
#define TINY_CONSOLE_AUTOSIZE 0

class TinyConsole
{
  public:
    using string = std::string;

    enum Color
    {
      black = 30,
      red = 31,
      green = 32,
      yellow = 33,
      blue = 34,
      magenta = 35,
      cyan = 36,
      white = 37,
      gray = 90,
      bright_red = 91,
      bright_green = 92,
      bright_yellow = 93,
      bright_blue = 94,
      bright_magenta = 95,
      bright_cyan = 96,
      bright_white = 97,
      none = 255
    };

    enum Operation
    {
      hide_cur,
      show_cur,
      erase_to_end,
      save_cursor,
      restore_cursor
    };

    using CallBack = void(*)(const string& command);
    using CallBackFnKey = void(*)(int fkey);
    using CallBackContolKey = void(*)(char ckey);

    TinyConsole();
    void begin(long baud);  // Init with Serial
    void begin(Stream&);
    void loop();

    void setPrompt(const char* prompt)
    { ps1 = prompt; }

    const TinyConsole& prompt() const;

    void setCallback(CallBack cb) { callback = cb; }
    void setCallbackFnKey(CallBackFnKey cb) { callback_fn = cb; }
    void setCallbackC0Key(CallBackContolKey cb) { callback_c0 = cb; }

    const TinyConsole& gotoxy(unsigned char x, unsigned char y) const;
    const TinyConsole& cursorVisible(bool visible) const;

    template<class Type>
    void printAt(unsigned char row, unsigned char col, const Type &what) const
    {
      saveCursor();
      cursorVisible(false);
      gotoxy(row, col);
      *serial << what;
      restoreCursor();
      cursorVisible(true);
    }

    const TinyConsole& cls() const;
    const TinyConsole& title(const char*) const;
    const TinyConsole& reset() const;
    const TinyConsole& saveCursor() const;
    const TinyConsole& restoreCursor() const;
    const TinyConsole& eraseEol() const;
    const TinyConsole& fg(enum Color c) const;
    const TinyConsole& bg(enum Color c) const { return fg(static_cast<enum Color>(static_cast<int>(c)+10)); }

    template<class Type>
    friend TinyConsole& operator << (TinyConsole& console, Type value)
    {
      *console.serial << value;
      return console;
    }

    friend TinyConsole& operator << (TinyConsole& console, Color color)
    {
      console.fg(color);
      return console;
    }

    friend TinyConsole& operator << (TinyConsole& console, Operation op)
    {
      if (op == erase_to_end)
        console.eraseEol();
      else if (op == save_cursor)
        console.saveCursor();
      else if (op == restore_cursor)
        console.restoreCursor();
      else
        console.cursorVisible(op == show_cur);
      return console;
    }

    bool isTerm() const { return term; }

  private:
    char waitChar();
    void handleEscape();
    string input;
    string ps1;
    string::size_type cursor=0; // column
    CallBack callback = nullptr;
    CallBackFnKey callback_fn = nullptr;
    CallBackContolKey callback_c0 = nullptr;
    Stream* serial = nullptr;
    bool term = false;
    bool csi6n = false;
    uint8_t histo_n = 0;
    uint8_t histo_max = 20;
    std::deque<string> history;

    bool getTermSize();
#if TINY_CONSOLE_AUTOSIZE
  public:
    unsigned char sx;
    unsigned char sy;
#endif
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
extern TinyConsole Console;
#endif
