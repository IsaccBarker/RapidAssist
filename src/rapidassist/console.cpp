/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "rapidassist/console.h"
#include "rapidassist/time_.h"
#include "rapidassist/environment.h"
#include <vector>

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif /* WIN32_LEAN_AND_MEAN */
#include <Windows.h>

#elif __linux__

#include <cstdio>       // fileno()
#include <unistd.h>     // isatty()
#include <sys/ioctl.h>  // ioctl()
#include <unistd.h>

//for getCursorPos()
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#endif

namespace ra
{
  namespace console
  {
    struct CursorCoordinate
    {
      int x;
      int y;
    };

    std::vector<CursorCoordinate> gCursorPositionStack;

#ifdef __linux__
    //Support functions for getCursorPos()
    //from https://www.linuxquestions.org/questions/programming-9/get-cursor-position-in-c-947833/

    #define   RD_EOF   -1
    #define   RD_EIO   -2

    static inline int rd(const int fd)
    {
      unsigned char   buffer[4];
      ssize_t         n;
      while (1) {
        n = read(fd, buffer, 1);
        if (n > (ssize_t)0)
          return buffer[0];
        else
          if (n == (ssize_t)0)
            return RD_EOF;
          else
            if (n != (ssize_t)-1)
              return RD_EIO;
            else
              if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
                return RD_EIO;
      }
    }

    static inline int wr(const int fd, const char *const data, const size_t bytes)
    {
      const char       *head = data;
      const char *const tail = data + bytes;
      ssize_t           n;
      while (head < tail) {
        n = write(fd, head, (size_t)(tail - head));
        if (n > (ssize_t)0)
          head += n;
        else
          if (n != (ssize_t)-1)
            return EIO;
          else
            if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
              return errno;
      }
      return 0;
    }

    /* Return a new file descriptor to the current TTY.
    */
    int current_tty(void)
    {
      const char *dev;
      int         fd;

      dev = ttyname(STDIN_FILENO);
      if (!dev)
        dev = ttyname(STDOUT_FILENO);
      if (!dev)
        dev = ttyname(STDERR_FILENO);
      if (!dev) {
        errno = ENOTTY;
        return -1;
      }

      do {
        fd = open(dev, O_RDWR | O_NOCTTY);
      } while (fd == -1 && errno == EINTR);
      if (fd == -1)
        return -1;

      return fd;
    }

    /* As the tty for current cursor position.
    * This function returns 0 if success, errno code otherwise.
    * Actual errno will be unchanged.
    */
    int cursor_position(const int tty, int *const rowptr, int *const colptr)
    {
      struct termios  saved, temporary;
      int             retval, result, rows, cols, saved_errno;

      /* Bad tty? */
      if (tty == -1)
        return ENOTTY;

      saved_errno = errno;

      /* Save current terminal settings. */
      do {
        result = tcgetattr(tty, &saved);
      } while (result == -1 && errno == EINTR);
      if (result == -1) {
        retval = errno;
        errno = saved_errno;
        return retval;
      }

      /* Get current terminal settings for basis, too. */
      do {
        result = tcgetattr(tty, &temporary);
      } while (result == -1 && errno == EINTR);
      if (result == -1) {
        retval = errno;
        errno = saved_errno;
        return retval;
      }

      /* Disable ICANON, ECHO, and CREAD. */
      temporary.c_lflag &= ~ICANON;
      temporary.c_lflag &= ~ECHO;
      temporary.c_cflag &= ~CREAD;

      /* This loop is only executed once. When broken out,
      * the terminal settings will be restored, and the function
      * will return retval to caller. It's better than goto.
      */
      do {

        /* Set modified settings. */
        do {
          result = tcsetattr(tty, TCSANOW, &temporary);
        } while (result == -1 && errno == EINTR);
        if (result == -1) {
          retval = errno;
          break;
        }

        /* Request cursor coordinates from the terminal. */
        retval = wr(tty, "\033[6n", 4);
        if (retval)
          break;

        /* Assume coordinate reponse parsing fails. */
        retval = EIO;

        /* Expect an ESC. */
        result = rd(tty);
        if (result != 27)
          break;

        /* Expect [ after the ESC. */
        result = rd(tty);
        if (result != '[')
          break;

        /* Parse rows. */
        rows = 0;
        result = rd(tty);
        while (result >= '0' && result <= '9') {
          rows = 10 * rows + result - '0';
          result = rd(tty);
        }

        if (result != ';')
          break;

        /* Parse cols. */
        cols = 0;
        result = rd(tty);
        while (result >= '0' && result <= '9') {
          cols = 10 * cols + result - '0';
          result = rd(tty);
        }

        if (result != 'R')
          break;

        /* Success! */

        if (rowptr)
          *rowptr = rows;

        if (colptr)
          *colptr = cols;

        retval = 0;

      } while (0);

      /* Restore saved terminal settings. */
      do {
        result = tcsetattr(tty, TCSANOW, &saved);
      } while (result == -1 && errno == EINTR);
      if (result == -1 && !retval)
        retval = errno;

      /* Done. */
      return retval;
    }
#endif

    void getCursorPos(int & x, int & y)
    {
#ifdef _WIN32
		  CONSOLE_SCREEN_BUFFER_INFO info;
		  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
		  x = info.dwCursorPosition.X;
		  y = info.dwCursorPosition.Y;
#elif __linux__
      x = 0;
      y = 0;

      //required to properly read cursor position using ANSI commands.
      fflush(stdout);

      //from https://www.linuxquestions.org/questions/programming-9/get-cursor-position-in-c-947833/
      int fd = current_tty();
      if (fd == -1)
        return; //failure

      int row = 0;
      int col = 0;
      if (cursor_position(fd, &row, &col))
        return; //failure

      x = col - 1; //convert from ANSI 1-based to 0-based
      y = row;
#endif
    }

    void setCursorPos(const int & x, const int & y)
    {
#ifdef _WIN32
		  COORD coord;
		  coord.X = x;
		  coord.Y = y;
		  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#elif __linux__
      printf("\033[%d;%dH", y, x);
#endif
    }

    void getDimension(int & width, int & height)
    {
#ifdef _WIN32
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
      width = (int)csbi.dwMaximumWindowSize.X;
      height = (int)csbi.dwMaximumWindowSize.Y;
#elif __linux__
      struct winsize ws;
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
      width = (int)ws.ws_col;
      height = (int)ws.ws_row;
#endif
    }

    void clearScreen()
    {
		  //system("cls");

#ifdef _WIN32
      COORD coordScreen = { 0, 0 };
      DWORD cCharsWritten;
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      DWORD dwConSize;
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

      GetConsoleScreenBufferInfo(hConsole, &csbi);
      dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
      FillConsoleOutputCharacter(hConsole, TEXT(' '),
                                dwConSize,
                                coordScreen,
                                &cCharsWritten);
      GetConsoleScreenBufferInfo(hConsole, &csbi);
      FillConsoleOutputAttribute(hConsole,
                                csbi.wAttributes,
                                dwConSize,
                                coordScreen,
                                &cCharsWritten);
      SetConsoleCursorPosition(hConsole, coordScreen);
#elif __linux__
      //not implemented yet
      printf("\033[2J");
#endif
    }

    void pushCursorPos()
    {
      CursorCoordinate coord;
      getCursorPos(coord.x, coord.y);

      gCursorPositionStack.push_back(coord);
    }

    void popCursorPos()
    {
      if (!gCursorPositionStack.empty())
      {
        size_t offset = gCursorPositionStack.size() - 1;
        const CursorCoordinate & last = gCursorPositionStack[offset];
        setCursorPos(last.x, last.y);

        //remove the last entry
        gCursorPositionStack.erase(gCursorPositionStack.begin() + offset);
      }
    }

    char getAnimationSprite(double iRefreshRate)
    {
      static const char gAnimationSprites[] = {'-', '\\', '|', '/'};
      static const int gNumAnimationSprites = sizeof(gAnimationSprites)/sizeof(gAnimationSprites[0]);
      double seconds = ra::time::getMillisecondsTimer(); //already seconds
      int spriteIndex = (int)(seconds/iRefreshRate);
      spriteIndex = spriteIndex % gNumAnimationSprites;
      char sprite = gAnimationSprites[spriteIndex];
      return sprite;
    }
 
    void printAnimationCursor()
    {
      pushCursorPos();
      printf("%c", getAnimationSprite(0.15));
      popCursorPos();
    }

    const char * getTextColorName(const TextColor & color)
    {
      switch(color)
      {
      case Black:
        return "Black";
        break;
      case White:
        return "White";
        break;
      case DarkBlue:
        return "DarkBlue";
        break;
      case DarkGreen:
        return "DarkGreen";
        break;
      case DarkCyan:
        return "DarkCyan";
        break;
      case DarkRed:
        return "DarkRed";
        break;
      case DarkMagenta:
        return "DarkMagenta";
        break;
      case DarkYellow:
        return "DarkYellow";
        break;
      case DarkGray:
        return "DarkGray";
        break;
      case Blue:
        return "Blue";
        break;
      case Green:
        return "Green";
        break;
      case Cyan:
        return "Cyan";
        break;
      case Red:
        return "Red";
        break;
      case Magenta:
        return "Magenta";
        break;
      case Yellow:
        return "Yellow";
        break;
      case Gray:
        return "Gray";
        break;
      default:
        return "";
      };
    }

    namespace ansi
    {
      //https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
      //https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal

      namespace FormatAttribute
      {
        enum Attr
        {
          Default = 0,
          Bold = 1,
          Dim = 2,
          Underlined = 3,
          Blink = 5,
          Reverse = 7,
          Hidden = 8
        };
        const char * toString(const Attr & attr)
        {
          switch(attr)
          {
            case Default    : return "Default"   ; break;
            case Bold       : return "Bold"      ; break;
            case Dim        : return "Dim"       ; break;
            case Underlined : return "Underlined"; break;
            case Blink      : return "Blink"     ; break;
            case Reverse    : return "Reverse"   ; break;
            case Hidden     : return "Hidden"    ; break;
          };
          return "Unknown";
        }
        static const int NUM_COLOR_ATTR = 7;
      };
      namespace ForegroundColor
      {
        enum Color
        {
          Default = 39,
          Black = 30,
          Red,
          Green,
          Yellow,
          Blue,
          Magenta,
          Cyan,
          LightGray,
          DarkGray = 90,
          LightRed,
          LightGreen,
          LightYellow,
          LightBlue,
          LightMagenta,
          LightCyan,
          White,
        };
        const char * toString(const Color & color)
        {
          switch(color)
          {
            case Default      : return "Default"      ; break;
            case Black        : return "Black"        ; break;
            case Red          : return "Red"          ; break;
            case Green        : return "Green"        ; break;
            case Yellow       : return "Yellow"       ; break;
            case Blue         : return "Blue"         ; break;
            case Magenta      : return "Magenta"      ; break;
            case Cyan         : return "Cyan"         ; break;
            case LightGray    : return "LightGray"    ; break;
            case DarkGray     : return "DarkGray"     ; break;
            case LightRed     : return "LightRed"     ; break;
            case LightGreen   : return "LightGreen"   ; break;
            case LightYellow  : return "LightYellow"  ; break;
            case LightBlue    : return "LightBlue"    ; break;
            case LightMagenta : return "LightMagenta" ; break;
            case LightCyan    : return "LightCyan"    ; break;
            case White        : return "White"        ; break;
          };
          return "Unknown";
        }
        static const int NUM_FOREGROUND_COLOR = 17;
      }; //namespace ForegroundColor
      namespace BackgroundColor
      {
        enum Color
        {
          Default = 49,
          Black = 40,
          Red,
          Green,
          Yellow,
          Blue,
          Magenta,
          Cyan,
          LightGray,
          DarkGray = 100,
          LightRed,
          LightGreen,
          LightYellow,
          LightBlue,
          LightMagenta,
          LightCyan,
          White,
        };
        const char * toString(const Color & color)
        {
          switch(color)
          {
            case Default      : return "Default"      ; break;
            case Black        : return "Black"        ; break;
            case Red          : return "Red"          ; break;
            case Green        : return "Green"        ; break;
            case Yellow       : return "Yellow"       ; break;
            case Blue         : return "Blue"         ; break;
            case Magenta      : return "Magenta"      ; break;
            case Cyan         : return "Cyan"         ; break;
            case LightGray    : return "LightGray"    ; break;
            case DarkGray     : return "DarkGray"     ; break;
            case LightRed     : return "LightRed"     ; break;
            case LightGreen   : return "LightGreen"   ; break;
            case LightYellow  : return "LightYellow"  ; break;
            case LightBlue    : return "LightBlue"    ; break;
            case LightMagenta : return "LightMagenta" ; break;
            case LightCyan    : return "LightCyan"    ; break;
            case White        : return "White"        ; break;
          };
          return "Unknown";
        }
        static const int NUM_BACKGROUND_COLOR = 17;
      }; //namespace BackgroundColor
    }; //namespace ansi
    
    void setTextColor(const TextColor & iForeground, const TextColor & iBackground)
    {
#ifdef _WIN32
      WORD foregroundAttribute = 0;
      switch(iForeground)
      {
      case Black:
        foregroundAttribute = 0;
        break;
      case White:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case DarkBlue:
        foregroundAttribute = FOREGROUND_BLUE;
        break;
      case DarkGreen:
        foregroundAttribute = FOREGROUND_GREEN;
        break;
      case DarkCyan:
        foregroundAttribute = FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case DarkRed:
        foregroundAttribute = FOREGROUND_RED;
        break;
      case DarkMagenta:
        foregroundAttribute = FOREGROUND_RED | FOREGROUND_BLUE;
        break;
      case DarkYellow:
        foregroundAttribute = FOREGROUND_RED | FOREGROUND_GREEN;
        break;
      case DarkGray:
        foregroundAttribute = FOREGROUND_INTENSITY;
        break;
      case Blue:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
        break;
      case Green:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
        break;
      case Cyan:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case Red:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_RED;
        break;
      case Magenta:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
        break;
      case Yellow:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
        break;
      case Gray:
        foregroundAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      };

      WORD backgroundAttribute = 0;
      switch(iBackground)
      {
      case Black:
        backgroundAttribute = 0;
        break;
      case White:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      case DarkBlue:
        backgroundAttribute = BACKGROUND_BLUE;
        break;
      case DarkGreen:
        backgroundAttribute = BACKGROUND_GREEN;
        break;
      case DarkCyan:
        backgroundAttribute = BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      case DarkRed:
        backgroundAttribute = BACKGROUND_RED;
        break;
      case DarkMagenta:
        backgroundAttribute = BACKGROUND_RED | BACKGROUND_BLUE;
        break;
      case DarkYellow:
        backgroundAttribute = BACKGROUND_RED | BACKGROUND_GREEN;
        break;
      case DarkGray:
        backgroundAttribute = BACKGROUND_INTENSITY;
        break;
      case Blue:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_BLUE;
        break;
      case Green:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_GREEN;
        break;
      case Cyan:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      case Red:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_RED;
        break;
      case Magenta:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE;
        break;
      case Yellow:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN;
        break;
      case Gray:
        backgroundAttribute = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      };

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), foregroundAttribute | backgroundAttribute);
#elif __linux__
      ansi::FormatAttribute::Attr ansi_attr = ansi::FormatAttribute::Default;
      ansi::ForegroundColor::Color ansi_foreground = ansi::ForegroundColor::Default;
      ansi::BackgroundColor::Color ansi_background = ansi::BackgroundColor::Default;

      //foreground
      switch(iForeground)
      {
      case Black:
        ansi_foreground = ansi::ForegroundColor::Black;
        break;
      case White:
        ansi_foreground = ansi::ForegroundColor::White;
        break;
      case DarkBlue:
        ansi_foreground = ansi::ForegroundColor::Blue;
        break;
      case DarkGreen:
        ansi_foreground = ansi::ForegroundColor::Green;
        break;
      case DarkCyan:
        ansi_foreground = ansi::ForegroundColor::Cyan;
        break;
      case DarkRed:
        ansi_foreground = ansi::ForegroundColor::Red;
        break;
      case DarkMagenta:
        ansi_foreground = ansi::ForegroundColor::Magenta;
        break;
      case DarkYellow:
        ansi_foreground = ansi::ForegroundColor::Yellow;
        break;
      case DarkGray:
        ansi_foreground = ansi::ForegroundColor::DarkGray;
        break;
      case Blue:
        ansi_foreground = ansi::ForegroundColor::LightBlue;
        break;
      case Green:
        ansi_foreground = ansi::ForegroundColor::LightGreen;
        break;
      case Cyan:
        ansi_foreground = ansi::ForegroundColor::LightCyan;
        break;
      case Red:
        ansi_foreground = ansi::ForegroundColor::LightRed;
        break;
      case Magenta:
        ansi_foreground = ansi::ForegroundColor::LightMagenta;
        break;
      case Yellow:
        ansi_foreground = ansi::ForegroundColor::LightYellow;
        break;
      case Gray:
        ansi_foreground = ansi::ForegroundColor::LightGray;
        break;
      };

      //background
      switch(iBackground)
      {
      case Black:
        ansi_background = ansi::BackgroundColor::Black;
        break;
      case White:
        ansi_background = ansi::BackgroundColor::White;
        break;
      case DarkBlue:
        ansi_background = ansi::BackgroundColor::Blue;
        break;
      case DarkGreen:
        ansi_background = ansi::BackgroundColor::Green;
        break;
      case DarkCyan:
        ansi_background = ansi::BackgroundColor::Cyan;
        break;
      case DarkRed:
        ansi_background = ansi::BackgroundColor::Red;
        break;
      case DarkMagenta:
        ansi_background = ansi::BackgroundColor::Magenta;
        break;
      case DarkYellow:
        ansi_background = ansi::BackgroundColor::Yellow;
        break;
      case DarkGray:
        ansi_background = ansi::BackgroundColor::DarkGray;
        break;
      case Blue:
        ansi_background = ansi::BackgroundColor::LightBlue;
        break;
      case Green:
        ansi_background = ansi::BackgroundColor::LightGreen;
        break;
      case Cyan:
        ansi_background = ansi::BackgroundColor::LightCyan;
        break;
      case Red:
        ansi_background = ansi::BackgroundColor::LightRed;
        break;
      case Magenta:
        ansi_background = ansi::BackgroundColor::LightMagenta;
        break;
      case Yellow:
        ansi_background = ansi::BackgroundColor::LightYellow;
        break;
      case Gray:
        ansi_background = ansi::BackgroundColor::LightGray;
        break;
      };

      printf("\033[%d;%d;%dm", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
      //printf("{%d;%d;%d}", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
#endif
    }

    void getTextColor(TextColor & oForeground, TextColor & oBackground)
    {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    
    DWORD foregroundInfo = info.wAttributes & (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    DWORD backgroundInfo = info.wAttributes & (BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);

    //foreground
    switch(foregroundInfo)
    {
    case 0:
      oForeground = Black;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE:
      oForeground = White;
      break;
    case FOREGROUND_BLUE:
      oForeground = DarkBlue;
      break;
    case FOREGROUND_GREEN:
      oForeground = DarkGreen;
      break;
    case FOREGROUND_GREEN | FOREGROUND_BLUE:
      oForeground = DarkCyan;
      break;
    case FOREGROUND_RED:
      oForeground = DarkRed;
      break;
    case FOREGROUND_RED | FOREGROUND_BLUE:
      oForeground = DarkMagenta;
      break;
    case FOREGROUND_RED | FOREGROUND_GREEN:
      oForeground = DarkYellow;
      break;
    case FOREGROUND_INTENSITY:
      oForeground = DarkGray;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_BLUE:
      oForeground = Blue;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_GREEN:
      oForeground = Green;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE:
      oForeground = Cyan;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_RED:
      oForeground = Red;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE:
      oForeground = Magenta;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN:
      oForeground = Yellow;
      break;
    case FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE:
      oForeground = Gray;
      break;
    };

    //background
    switch(backgroundInfo)
    {
    case 0:
      oBackground = Black;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE:
      oBackground = White;
      break;
    case BACKGROUND_BLUE:
      oBackground = DarkBlue;
      break;
    case BACKGROUND_GREEN:
      oBackground = DarkGreen;
      break;
    case BACKGROUND_GREEN | BACKGROUND_BLUE:
      oBackground = DarkCyan;
      break;
    case BACKGROUND_RED:
      oBackground = DarkRed;
      break;
    case BACKGROUND_RED | BACKGROUND_BLUE:
      oBackground = DarkMagenta;
      break;
    case BACKGROUND_RED | BACKGROUND_GREEN:
      oBackground = DarkYellow;
      break;
    case BACKGROUND_INTENSITY:
      oBackground = DarkGray;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_BLUE:
      oBackground = Blue;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_GREEN:
      oBackground = Green;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE:
      oBackground = Cyan;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_RED:
      oBackground = Red;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE:
      oBackground = Magenta;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN:
      oBackground = Yellow;
      break;
    case BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE:
      oBackground = Gray;
      break;
    };
#elif __linux__
      //not implemented yet
      oForeground = Gray;
      oBackground = Black;
#endif
    }

    void setDefaultTextColor()
    {
#ifdef _WIN32
      ra::console::setTextColor(ra::console::Gray, ra::console::Black);
#elif __linux__
      printf("\033[0m");
#endif
    }

    bool isDesktopGuiAvailable()
    {
#ifdef _WIN32
      return true;
#elif __linux__
      std::string display = ra::environment::getEnvironmentVariable("DISPLAY");
      bool hasDesktopGui = !display.empty();
      return hasDesktopGui;
#endif
    }

    bool isRunFromDesktop()
    {
#ifdef _WIN32
      std::string prompt = ra::environment::getEnvironmentVariable("PROMPT");
      bool hasNoPrompt = prompt.empty();
      return hasNoPrompt;
#elif __linux__
      //https://stackoverflow.com/questions/13204177/how-to-find-out-if-running-from-terminal-or-gui
      if (isatty(fileno(stdin)))
        return false;
      else
        return true;
#endif
    }

    bool hasConsoleOwnership()
    {
#ifdef _WIN32
      //https://stackoverflow.com/questions/9009333/how-to-check-if-the-program-is-run-from-a-console
      HWND consoleWnd = GetConsoleWindow();
      DWORD dwConsoleProcessId = 0;
      GetWindowThreadProcessId(consoleWnd, &dwConsoleProcessId);
      DWORD dwCurrentProcessId = GetCurrentProcessId();
      GetWindowThreadProcessId(consoleWnd, &dwConsoleProcessId);
      if (dwCurrentProcessId==dwConsoleProcessId)
      {
        return true;
      }
      return false;
#elif __linux__
      return isRunFromDesktop();
#endif
    }

  } //namespace console
} //namespace ra
