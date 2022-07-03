/*
 * File: trie.h
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

#pragma once

// STL includes
#include <vector>
#include <string>
#include <tuple>

/*!
 * For handling the command completion we need to create a trie
 * data structure to allow fast search for matching commands. 
 */

/*! 
 * The node for the trie class to use
 */
struct TrieNode
{
  TrieNode** children = nullptr; /*!< An array of TrieNode pointers, one per valid character */
  bool is_terminal = false;;     /*!< Whether this node is the end of a word */
  std::string word_to_here;      /*!< The letters from the nodes traversed to here (to save backwards checks) */
};

/*!
 * Represent a trie structure with methods to insert, search (with partial results)
 * and destroy the structure
 */
class CommandTrie
{
public:

  /*!
   * Construct the class with characters that are allowed
   * \param valid_chars The list of allowable chars in a command name
   */
  CommandTrie(const std::string& valid_chars);

  /*!
   * Destructor destroys the tree 
   */
  ~CommandTrie();

  /*!
   * Insert a command into the trie
   * \param str The string to insert into the trie
   * \throws std::out_of_range The string includes invalid characters (based on constructor call)
   */
  void insert(const std::string& str);

  /*!
   * Search for a string within the trie
   * \param str The string to search for
   * \param ret_pos If true, return a list of possible commands. Setting to true increases search time
   *                (default is false).
   * \return A tuple containing a unsigned int set to the number of available matches to the last unambiguous
   *         point, a string of the word up to the last unambiguous point (if the first element is 1, then this
   *         is the final answer for the given input), and (if ret_pos was set) a vector of all matching commands.
   * 
   */
  std::tuple<std::size_t, std::string, std::vector<std::string>> 
    find(const std::string& str, bool ret_pos = false) const;

  /*!
   * Just for debugging purposes, print out the tree
   * \note This function just checks the root isn't nullptr, and calls the recursive version
   */
  void print();
  
private:

  /*!
   * Destroy the trie
   */
  void axe(TrieNode* node);

  /*!
   * Build the index for valid ASCII characters -> trie node array position
   */
  void buildIndex(const std::string& valid_chars);

  /*!
   * Get the index for a given ASCII char
   * \param c The character to get the index for
   * \return The index for the given character, or 255 if the character is invalid
   * \note This is written as a function to allow us to change the underlying method later
   *       if we need, without changing other code.
   */
  inline unsigned int index(unsigned char c) const
  {
    if (c >= index_.size()) return 255;
    return index_[c];
  }

  /*!
   * Create a new node
   * \return The new node
   */
  TrieNode* createTrieNode();

  /*!
   * Get the longest unambiguous string that extends beyond passed in string
   * \param node The next node to test
   * \return A tuple indicating the number of matches when we reach the end of the unambiguous
   *         path, and the unambiguous path's string, and the node where it stopped
   * \throws std::runtime_error One of the nodes is incorrecly setup
   */
  std::tuple<std::size_t, std::string, TrieNode*> getLongestString(TrieNode* node) const;

  /*!
   * Get the number of children of a node that are non-nullptrs
   * \param node The node to check
   * \return A vector of indices to the live children
   */
  inline std::vector<unsigned int> getLiveChildren(TrieNode* node) const;

  /*!
   * Get the possible commands from a given node
   * \param node The node to get the commands from 
   * \retval poss_cmds The vector we're building up containing possible commands
   */
  void getPossibleCommands(TrieNode* node, std::vector<std::string>& poss_cmds) const;

  /*!
   *  Recursively print out all the nodes of the trie - useful for debugging
   *  \param node The current node to print out information for
   */
  void print(TrieNode* node);

  //! The root of the trie
  TrieNode* trie_root_;

  /*! A vector containing the indexes for each ASCII value
   *  \note This allows us to quickly use the ASCII code as an index to the vector
   *        to get an index in the trie
   */
  std::vector<unsigned int> index_;

  /*!
   * A vector for converting the index back to a character
   */
  std::vector<unsigned char> index_to_char_;

  //! The number of indicies we need to represent in each node
  unsigned int trie_node_size_;
};

