/* 
 * File: platform/linux-console.cpp
 * Author: Thyme Chrystal
 *
 * MIT License
 *
 * Copyright (c) 2022 Thyme Chrystal
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
 */

/*
 * This version of the Linux console and related
 * functions are based on:
 *   https://www.scosales.com/ta/kb/100487.html
 *
 * For a description of modes and flags, see:
 *   https://www.man7.org/linux/man-pages/man3/termios.3.html
 */

/*
 * The above works as is, but some keys produce more than
 * one input at a time (e.g. <Delete> produces the codes 27, 91, 51, 126).
 * The code 27 is an escape code, but we can't use this to indicate multiple
 * outputs, as pressing <Esc> produces this code on its own.
 * If we use the code from the above page, then pressing <Delete> outputs
 * [3~ (Note: The 27 doesn't output a char).
 * With some research, I found that you can use ioctl() with FIONREAD to get
 * the number of bytes in the queue. We can use this to recognise when
 * keys with multiple codes are pressed, and perform the right actions.
 * For more info, see:
 *   https://man7.org/linux/man-pages/man4/tty_ioctl.4.html
 */

// test-console includes
#include <console.h>
#include <platform/linux-console.h>

// POSIX includes
#include <termios.h>
#include <sys/ioctl.h>

// STL includes
#include <iostream>
#include <string>
#include <exception>
#include <cerrno>
#include <cstring>
#include <vector>
#include <tuple>

namespace
{
  std::tuple<KeyPressed, char> handleConsoleKeyEvent(const std::string& input,
    const std::map<KeyMapping, KeyPressed>& key_map)
  {
    std::tuple<KeyPressed, char> ret;

    // For now, just check the first character, because we're not
    // interested in keys that produce multiple values yet

    if (input[0] >= 32 && input[0] <= 126) // ASCII printable characters
      return std::make_tuple(KeyPressed::alphanum, input[0]);

    auto km = key_map.find(input);
    if (km != key_map.end())
      return std::make_tuple(km->second, '\0');

    for (auto c : input)
      std::cout << "Next char: " << c << ", value: " << (int)c << "\r\n";

    // Return that we don't know the code
    return std::make_tuple(KeyPressed::undefined, '\0');
  }
}

// Initilalise the platform variables
void TestConsole::initialisePlatformVariables()
{
  // Save the old state
  if (tcgetattr(0, &(platform_vars_.old_state)) == -1)
    throw std::runtime_error("Unable to save the old console state with tcgetattr()");
  
  struct termios tbuf = platform_vars_.old_state;

  // These setting have NOT (~) applied to unset them so we:
  // - Don't translate between newline and carriage return (INLCR, ICRNL)
  // - Don't strip any of the input bits, so we get all 8 (ISTRIP)
  // - Don't use xon/xoff flow control for the output (IXON)
  // - Don't interupt on a break signal (BRKINT)
  tbuf.c_iflag &= ~(INLCR | ICRNL | ISTRIP | IXON | BRKINT);

  // Disable processing of output
  tbuf.c_oflag &= ~OPOST;

  // These settings also have NOT (~) applied to unset them so we:
  // - Don't use canonical mode - canonical mode would mean waiting for EOL to recieve input (ICANON)
  // - Don't need to deal with signals (ISIG)
  // - Don't echo the output (ECHO)
  tbuf.c_lflag &= ~(ICANON | ISIG | ECHO);

  // The VMIN value says how many inputs we receive
  // Setting the VTIME value to 0 means block until VMIN inputs received
  tbuf.c_cc[VMIN] = 1;
  tbuf.c_cc[VTIME] = 0;

  // Set these new parameters
  if (tcsetattr(0, TCSANOW, &tbuf) == -1)
    throw std::runtime_error("Unable to set new console state with tcsetattr()");

  // Set up the key mappings
  const std::string tab(1, 9);
  const std::string ret(1, 13);
  const std::string esc(1, 27);
  const std::string bsp(1, 127);
  
  key_map_[tab] = KeyPressed::tab;
  key_map_[ret] = KeyPressed::enter;
  key_map_[esc + "[3~"] = KeyPressed::del;
  key_map_[esc + "[A"] = KeyPressed::uparrow;
  key_map_[esc + "[B"] = KeyPressed::downarrow;
  key_map_[esc + "[C"] = KeyPressed::rightarrow;
  key_map_[esc + "[D"] = KeyPressed::leftarrow;
  key_map_[bsp] = KeyPressed::backspace;
}

// This has the linux specific code
std::vector<std::tuple<KeyPressed, char>> TestConsole::getKeyPresses() const
{
  // The return value
  std::vector<std::tuple<KeyPressed, char>> ret;

  int n_bytes{0};
  int res={0};

  while (!n_bytes)
  {
    res = ioctl(0, FIONREAD, &n_bytes);
    if (res < 0)
      throw std::runtime_error(std::string("ioctl call failed: ") + std::strerror(errno));
  }

  std::string inp{ "" };
  for (int i=0; i<n_bytes; ++i)
  {
    inp += (char)getchar();
  }
  ret.push_back(handleConsoleKeyEvent(inp, key_map_));
  
  return ret;
}

// Handle any resources on closing
void TestConsole::cleanUpConsole()
{
  // Restore the initial console state
  tcsetattr(0, TCSANOW, &platform_vars_.old_state);
}



