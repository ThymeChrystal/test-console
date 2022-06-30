/* 
 * File: console.cpp
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

// test-console includes
#include "console.h"

// STL includes
#include <iostream>
#include <exception>
#include <tuple>

// Construct the console
TestConsole::TestConsole(const std::string& prompt) : prompt_{ prompt }
{
  initialisePlatformVariables();
}

// Start the console
int TestConsole::start()
{
  
  // Note: we need \r and \n throughout the code as the
  //       state of the console on Linux requires both
  //       to do what we expect of \n normally
  try
  {
    // Just echo the user's input until they type 'quit'
    std::string input{ "" };
    while (input != "quit")
    {
      // Flush to make sure the prompt is shown
      std::cout << prompt_ << " " << std::flush;
      input = getUserInputLine();
      std::cout << "You typed: " << input << "\r\n";
    }
  }
  catch (std::exception& e)
  {
    std::cout << "There was an error getting the user's input: " << e.what() << "\r\n";
  }
 
  return 0;
}

// Get the user input line
std::string TestConsole::getUserInputLine()
{
  // The line the user is entering
  std::string line{ "" };

  KeyPressed key_pressed = KeyPressed::undefined;

  while (key_pressed != KeyPressed::enter)
  {
    std::vector<std::tuple<KeyPressed, char>> keys = getKeyPresses();
    for (auto k : keys)
    {
      key_pressed = std::get<0>(k);

      // Enter stops processing as it indicates the user is done
      if (key_pressed == KeyPressed::enter)
      {
        // On Linux, we need to use both \r and \n
        // because of the terminal setting.
        // It will also work on the Windows version
        std::cout << "\r\n";
        break;
      }

      // If we've got a printable char, print it and add to string
      switch (key_pressed)
      {
      case KeyPressed::alphanum:
        {
          char c = std::get<1>(k);
          std::cout << c;
          line += c;
          break;
        }
      case KeyPressed::backspace:
        if (!line.empty())
        {
          std::cout << "\b \b";
          line.pop_back();
        }
        else
          std::cout << "\a"; // Sound a bell as backspace is invalid
        break;
      case KeyPressed::error:
        throw std::runtime_error("There was an error when processing key inputs");
        break;
      default: // Anything else, we can just ignore!
        break;
      }
    }

    // Make sure anything we've done is updated
    std::cout << std::flush;
  }
  return line;
}

