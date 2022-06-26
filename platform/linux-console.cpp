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

// test-console includes
#include <console.h>
#include <platform/linux-console.h>

// STL includes
#include <iostream>
#include <string>

// Initilalise the platform variables
void TestConsole::initialisePlatformVariables()
{
  // Do nothing as yet for Linux
}

// For now, implement with std::cin
// This will have the linux specific code eventually
std::string TestConsole::getUserInput()
{
  char line[PlatformVariables::maximum_line_length];
  std::cin.getline(line, PlatformVariables::maximum_line_length);
  std::string inp{line};
  inp += " on Linux!";
  
  return inp;
}

// Handle any resources on closing
void TestConsole::cleanUpConsole()
{
  // Do nothing at the moment on Linux
}



