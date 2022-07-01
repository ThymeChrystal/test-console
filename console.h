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
#include <tuple>
#include <vector>
#include <map>

//! Indicate the type of key press
enum class KeyPressed
{
  alphanum,    /*!< A printable character was typed */
  enter,       /*!< Enter (or related key) was pressed */
  backspace,   /*!< Backspace was pressed */
  del,         /*!< The Delete (or Del) key was pressed */
  leftarrow,   /*!< The left arrow key was pressed */
  rightarrow,  /*!< The right arrow key was pressed */
  undefined,   /*!< The key press was not something we handle */
  error        /*!< If there is a problem with the key reader */
};

class TestConsole
{
public:

  /*!The test console constructor
   * \param prompt takes the string to show as the console prompt
   */
  TestConsole(const std::string& prompt);

  /*! The test console destructor
   * \brief Calls the platform dependent code to clean up 
   */
  ~TestConsole() { cleanUpConsole(); };

  /*! Start the console
   * \brief Starts the console, shows the prompt and waits for user input
   * \return 0 if the console terminated cleanly, non-zero if there was an error
   */
  int start();

private:
  /*! Initialise the platform specific variables for this class
   * /note This is implemented specific to the platform
   */
  void initialisePlatformVariables();

  /* Get the next line of input from the user
   * \return The line entered by the user at the command line
   */
  std::string getUserInputLine();

  /*! Get the next keypress
   * \returns A vector containing the key types and a char. If the key isn't alphanumeric, the char will be '\0'
   * \note This is implemented specific to the platform
   */
  std::vector<std::tuple<KeyPressed, char>> getKeyPresses();

  /*! Clean up the platform specific console
   * \note This is implemented specific to the platform
   */
  void cleanUpConsole();

  //! The prompt to display
  std::string prompt_;

  //! Variables needed by the platform specific code
  PlatformVariables platform_vars_;

  //! The key mapping to help with key inputs
  std::map<KeyMapping, KeyPressed> key_map_;
};
