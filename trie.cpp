/*
 * File: trie.cpp
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
#include <trie.h>

// STL includes
#include <iostream>
#include <exception>

// The constructor
CommandTrie::CommandTrie(const std::string& valid_chars) : 
  trie_root_{nullptr},
  trie_node_size_{0}
{
  buildIndex(valid_chars);
}

// Destructor
CommandTrie::~CommandTrie()
{
  axe(trie_root_);
}

// Insert an item into the trie structure
void CommandTrie::insert(const std::string& str)
{
  // Create the root node if it doesn't exist
  if (trie_root_ == nullptr)
  {
    trie_root_ = createTrieNode();
  }

  TrieNode* curr_node = trie_root_; // Curr node will change as we traverse
  std::string word_to_here = "";    // It's going to be useful to have the letters that get us to each node

  // Ensure we have nodes for each character in our string
  for (auto c : str)
  {
    curr_node->word_to_here = word_to_here; 
    unsigned int idx = index_[c];

    // If the child node for this char doen't exist, create it
    if (curr_node->children[idx] == nullptr)
      curr_node->children[idx] = createTrieNode();

    // Move to the next node
    curr_node = curr_node->children[idx];
    word_to_here.push_back(c);
  }

  // Update the final node with the word so far and set it as terminal (it's a complete word)
  curr_node->word_to_here = word_to_here;
  curr_node->is_terminal = true;
}

// Find any strings matching a passed in value
std::tuple<std::size_t, std::string, std::vector<std::string>> 
CommandTrie::find(const std::string& str, bool ret_pos /*= false*/) const
{
  std::tuple<unsigned int, std::string, std::vector<std::string>> blank_tuple =
    std::make_tuple(0, "", std::vector<std::string>());

  // If the trie is empty, there's nothing to match
  if (trie_root_ == nullptr)
    return blank_tuple;

  // Check if the partial string is in the tree
  TrieNode* curr_node = trie_root_;
  for (auto c : str)
  {
    unsigned int idx = index_[c];
    if (curr_node->children[idx] == nullptr)
      return blank_tuple;  // If we hit a nullptr, the search string isn't in the trie
    curr_node = curr_node->children[idx];
  }

  // We want to auto complete from where our string stops, so we need to see if there
  // are any unambiguous paths (single children) from here
  auto [n_paths, longest_str, last_node] = getLongestString(curr_node);

  // If the possible commands are requested, get them
  std::vector<std::string> possible_commands;
  if (ret_pos)
    getPossibleCommands(last_node, possible_commands);

  return std::make_tuple(n_paths, longest_str, possible_commands);
}

// Print out the trie
void CommandTrie::print()
{
  if (trie_root_ == nullptr)
    std::cout << "Empty!\n";
  else
    print(trie_root_);
}

// Axe the trie
void CommandTrie::axe(TrieNode *node)
{
  if (node == nullptr)
    return;

  for (unsigned int i = 0; i < trie_node_size_; ++i)
    axe(node->children[i]);

  delete[] node->children;
  delete node;
}

// Build the index from a set of valid chars
void CommandTrie::buildIndex(const std::string& valid_chars)
{
  // We're going to build a vector of 126 chars for the valid
  // ASCII set. Any that aren't in valid_chars get set to 255 (invalid)
  // and any that are get an increasing index
  unsigned int idx = 0;

  // The first 32 characters (0-31) are invalid, so put 32 invalid values in
  // the vector
  index_.insert(index_.begin(), 32, 255);

  // Do the rest of the characters from 32-126
  for (unsigned char c = 32; c < 127; ++c)
  {
    if (valid_chars.find(c) != std::string::npos)
    {
      index_.push_back(idx++);
      index_to_char_.push_back(c);
    }
    else
      index_.push_back(255);
  }
  trie_node_size_ = idx;
}

// Create a new node
TrieNode* CommandTrie::createTrieNode()
{
  TrieNode* new_node = new TrieNode;
  new_node->children = new TrieNode*[trie_node_size_];
  for (unsigned int i = 0; i < trie_node_size_; ++i) 
    new_node->children[i] = nullptr;
  new_node->is_terminal = false;
  new_node->word_to_here = "";
  return new_node;
}

// Get the longest unambiguous string from the current node
std::tuple<std::size_t, std::string, TrieNode*> CommandTrie::getLongestString(TrieNode* node) const
{
  // If this is a terminal node, return this as the suggestion, or we could
  // make the user have to delete our completion
  if (node->is_terminal)
    return std::make_tuple(1, node->word_to_here, node);

  // See how many children the node has. 
  // - if there's zero, the tree is wrong - it should be a terminal node and caught above
  // - if there's one, we can continue to follow as it's a unique path
  // - if it's more than one, we can't got any further, so retuen the number of paths and
  //   this node's word_to_here
  std::vector<unsigned int> live_children = getLiveChildren(node);
  if (live_children.empty())
    throw std::runtime_error("The triw is incorrectly formatted - node should be terminal");
  else if (live_children.size() == 1)
    return getLongestString(node->children[live_children[0]]);

  // Number of live children must be > 1 to get here
  return std::make_tuple(live_children.size(), node->word_to_here, node);
}

// Get a list of live children of the current node
std::vector<unsigned int> CommandTrie::getLiveChildren(TrieNode* node) const
{
  std::vector<unsigned int> live_children;
  for (unsigned int i = 0; i < trie_node_size_; ++i)
    if (node->children[i] != nullptr)
      live_children.push_back(i);
  return live_children;
}

// Get a list of possible commands
void CommandTrie::getPossibleCommands(TrieNode* node, std::vector<std::string>& poss_cmds) const
{
  if (node->is_terminal)
    poss_cmds.push_back(node->word_to_here);

  std::vector<unsigned int> live_children = getLiveChildren(node);
  for (auto c : live_children)
    getPossibleCommands(node->children[c], poss_cmds);
}

// Recursively print out each node of the trie
void CommandTrie::print(TrieNode* node)
{
  std::cout << "----- Begin Node -----\n";
  std::cout << "Node: " << node << "\n";
  std::cout << "Word to here: " << node->word_to_here << "\n";
  std::cout << "Live children: ";
  std::vector<unsigned int> live_children = getLiveChildren(node);
  if (live_children.empty())
    std::cout << "None";
  else
  {
    for (auto c : live_children)
      std::cout << "[" << index_to_char_[c] << "] ";
  }
  std::cout << "\n";
  std::cout << "Is terminal: " << (node->is_terminal ? "true" : "false") << "\n";
  std::cout << "----- End Node -----\n";

  for (auto c : live_children)
  {
    print(node->children[c]);
  }
}
