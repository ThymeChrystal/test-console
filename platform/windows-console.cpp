/* 
 * File: platform/windows-console.cpp
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
 * This version of the getUserInput() and related
 * functions are based on:
 *   https://docs.microsoft.com/en-us/windows/console/reading-input-buffer-events
 *
 */

// test-console includes
#include <console.h>
#include <platform/windows-console.h>

// MS includes
#include <Windows.h>

// STL includes
#include <iostream>
#include <string>
#include <exception>

// Functions to handle the events
namespace
{

  // Handle the key press events
  ReturnValue HandleConsoleKeyEvent(KEY_EVENT_RECORD ke, std::string& line)
  {
    ReturnValue ret = ReturnValue::success;

    // Handle keys if they are down
    if (ke.bKeyDown)
    {
      // If it's in the ASCII printable range, print it (maybe multiple times
      // based on repeat count)
      if (ke.uChar.AsciiChar >= 32 && ke.uChar.AsciiChar <= 127)
      {
        std::string val = std::string(ke.wRepeatCount, ke.uChar.AsciiChar);
        line += val;
        std::cout << val;
      }

      // Handle a backspace press
      if (ke.wVirtualKeyCode == 8)
      {
        // Remove the last character fom the display and the line
        // we're building
        if (!line.empty())
        {
          line.pop_back();
          std::cout << "\b \b";
        }
        else
          // There's nothing to delete, so make a sound!
          std::cout << "\a";
      }

      // Enter pressed, so finish this input line
      if (ke.wVirtualKeyCode == 13)
      {
        std::cout << "\n";
        ret = ReturnValue::enter_press;
      }
    }
   
    return ret;
  }

  // Handle resize events - no plans to do anything with this at the moment
  ReturnValue HandleConsoleResizeEvent(WINDOW_BUFFER_SIZE_RECORD wbs)
  {
    ReturnValue ret = ReturnValue::success;
    std::cout << "Resize event!\n";
    return ret;
  }
}

// Initilalise the platform variables
void TestConsole::initialisePlatformVariables()
{
  // Get a handle to stdin, and get the current console settings
  platform_vars_.stdcin_handle = GetStdHandle(STD_INPUT_HANDLE);
  if (INVALID_HANDLE_VALUE == platform_vars_.stdcin_handle)
    throw std::runtime_error("Unable to get standard input handle for the console");
  if (!GetConsoleMode(platform_vars_.stdcin_handle, &platform_vars_.old_console_mode))
    throw std::runtime_error("Unable to get the console mode");

  // Set our console mode. We add the mouse in case we need it
  if (!SetConsoleMode(platform_vars_.stdcin_handle, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
    throw std::runtime_error("Unable to set the new console mode");
}

// This handles the windows specific code
std::string TestConsole::getUserInput()
{
  // The line of inputted key strokes
  std::string line{ "" };

  // The current position of the 'cursor' within 'line'
  unsigned int cursor_pos = 0;

  // Buffer to get events from the queue
  INPUT_RECORD event_buffer[PlatformVariables::input_buffer_size];

  // The number of events returned in the buffer
  DWORD n_events_read = 0;
  
  // Keep getting inputs until <Enter> is pressed
  ReturnValue ret = ReturnValue::undefined;

  while (ret != ReturnValue::enter_press)
  {
    // Get any queued events, or wait for one
    if (!ReadConsoleInput(platform_vars_.stdcin_handle,
      event_buffer,
      PlatformVariables::input_buffer_size,
      &n_events_read))
      throw std::runtime_error("ReadConsoleInput failed!");

    // Process each event
    for (unsigned int i = 0; i < n_events_read; ++i)
    {
      switch (event_buffer[i].EventType)
      {
      case KEY_EVENT: // Handle keyboard inputs
        ret = HandleConsoleKeyEvent(event_buffer[i].Event.KeyEvent, line);
        break;
      case MOUSE_EVENT: // Handle mouse inputs
        // Mouse events don't seem to be active in console apps, so ignore
        break;
      case WINDOW_BUFFER_SIZE_EVENT: // Handle windows resize events
        ret = HandleConsoleResizeEvent(event_buffer[i].Event.WindowBufferSizeEvent);
        break;
      case FOCUS_EVENT: // Windows has gained or lost focus
        // Ignore for now
      case MENU_EVENT:  // Menu has been clicked
        // Ignore for now
      default:
        // Silently ignore unknown events - user could have some wierd input device!
        break;
      }
    }
  }
  
  line += " on Windows!";
  
  return line;
}

// Handle any resources on closing
void TestConsole::cleanUpConsole()
{
  // Restore the old console settings
  SetConsoleMode(platform_vars_.stdcin_handle, platform_vars_.old_console_mode);
}
