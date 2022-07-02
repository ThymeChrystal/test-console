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

      // Not a fully featured history, but
      // we can show what's in the list
      if (input == "history")
      {
        for (auto h : history_)
          std::cout << h << "\r\n";
      }
      else
        std::cout << "You typed: " << input << "\r\n";

      // Save the history if it's not the same as the previous entry
      if (history_.empty() || input != history_.back())
        history_.push_back(input);
    }
  }
  catch (std::exception& e)
  {
    std::cout << "There was an error getting the user's input: " << e.what() << "\r\n";
  }
 
  return 0;
}

// Get the user input line
std::string TestConsole::getUserInputLine() const
{
  // The line the user is entering
  std::string line{ "" };

  // Keep track of where the cursor is in the input line
  std::string::size_type cursor_pos = 0;

  // The position we are in the history
  std::size_t history_pos = history_.size();

  // Keep the current line if we move through the history
  std::string current_line{ "" };

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
        // Get the char and print it
          char c = std::get<1>(k);
          std::cout << c;

          // If we're not at the end of the string, print out the rest of
          // the string and insert the char in the right place
          if (cursor_pos != line.size())
          {
            std::cout << line.substr(cursor_pos);
            line.insert(cursor_pos, 1, c);

            // Move the cursor back to where it was
            std::cout << std::string(line.size() - (cursor_pos+1), '\b');
          }
          else  // If we are at the end, just add the char
            line += c;
          ++cursor_pos; 
          break;
        }
      case KeyPressed::backspace:
        // We can't delete if there's nothing there
        if (!line.empty() && cursor_pos != 0)
        {
          // If the cursor is in the middle of the line, it's more complicated!
          if (cursor_pos != line.size())
          {
            // Remove the char deleted from the display and line string
            std::cout << "\b" << line.substr(cursor_pos) + " ";
            line.erase(cursor_pos-1, 1);

            // Move the cursor back one space (remembering the space we put on the end)
            std::cout << std::string(line.size() - (cursor_pos-2), '\b');
          }
          else
          {
            // Move back, erase the last character and move back again
            // Also remove the last character of the line string
            std::cout << "\b \b";
            line.pop_back();
          }
          --cursor_pos;
        }
        else
          std::cout << "\a"; // Sound a bell as backspace is invalid
        break;
      case KeyPressed::leftarrow:
        if (cursor_pos > 0)
        {
          // Move the cursor backwards
          std::cout << "\b";
          --cursor_pos;
        }
        else
          std::cout << "\a"; // Sound a bell as left arrow can't move further back
        break;
      case KeyPressed::rightarrow:
        // Check the cursor is not at the end
        if (cursor_pos != line.size())
        {
          // Output the character at the current position to move the cursor forward
          std::cout << line[cursor_pos];
          ++cursor_pos;
        }
        else
          std::cout << "\a";
        break;
      case KeyPressed::uparrow:
        // If we're not already in the history, save the current line
        if (history_pos == history_.size())
          current_line = line;

        // Check we're not at the top of the history
        if (history_pos != 0)
        {
          // Move to the previous history place
          --history_pos;

          std::string new_line = history_[history_pos];
          replaceLine(line.size(), new_line, cursor_pos);
          line = new_line;
          cursor_pos = line.size();
        }
        else 
          std::cout << '\a';  // If we're at the beginning of the history, just beep
        break;
      case KeyPressed::downarrow:
        // Check we're not already at the endo of the history
        if (history_pos != history_.size())
        {
          ++history_pos;
          std::string new_line;
          if (history_pos == history_.size())
            new_line = current_line;
          else
            new_line = history_[history_pos];
          replaceLine(line.size(), new_line, cursor_pos);
          line = new_line;
          cursor_pos = line.size();
        }
        else
          std::cout << '\a'; // If we're at the end of the history, just beep
        break;
      case KeyPressed::del:
        // Check we're not at the end of the string
        if (cursor_pos != line.size())
        {
          // Remove the character from the line, and output the rest of the rest
          // of the string to remove the character, then move the cursor back to
          // where it was
          line.erase(cursor_pos, 1);
          std::cout << line.substr(cursor_pos) << " " 
            << std::string(line.size() - (cursor_pos - 1), '\b');
        }
        else
          std::cout << "\a";
        break;
      case KeyPressed::tab:
        std::cout << "\r\nTab Pressed!\r\n";
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

// Replace one line on the display with another
void TestConsole::replaceLine(const std::string::size_type& old_line_size, const std::string& new_line,
  const std::string::size_type& cur_pos) const
{
  // Check if we need to clear part of the line
  std::string spaces{ "" };
  std::string back_spaces{ "" };
  int diff = old_line_size - new_line.size();
  if (diff > 0)
  {
    spaces = std::string(diff, ' ');
    back_spaces = std::string(diff, '\b');
  }

  // Move to the start of the line, and then overwrite the line,
  // then move the cursor back over any spaces
  std::cout << std::string(cur_pos, '\b') << new_line << spaces << back_spaces;
}

