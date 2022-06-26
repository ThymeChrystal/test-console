/* 
 * File: console.h
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

// Test console includes
#include <console-platform.h>

// STL includes
#include <string>

class TestConsole
{
public:

  // The test console constructor takes the prompt to show as a string
  TestConsole(const std::string& prompt);
  ~TestConsole() { cleanUpConsole(); };

  // Calling this starts the console
  int start();

private:
  // Initialise the platform variables for this class
  // This is implemented specific to the platform
  void initialisePlatformVariables();

  // Get the input from the user
  // This is implemented specific to the platform
  std::string getUserInput();

  // Clean up the platform specific console
  // This is implemented specific to the platform
  void cleanUpConsole();

  // The prompt to display
  std::string prompt_;

  // Variables needed by the platform specific code
  PlatformVariables platform_vars_;
};
