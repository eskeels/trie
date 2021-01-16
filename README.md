# Trie
C++ implementation of a Trie<br>
A Trie (http://en.wikipedia.org/wiki/Trie) is a great way to search a stream of text for multiple keywords. It's extremely fast and is a very simple structure to understand. Here is my initial attempt in C++. It is a first cut, not yet fully tested but seems to work. It is case sensitive and will match partial words. The main disadvantage of a Trie is the memory consumption. Each letter requires a node that contains a character / bool and vector so adding words quickly chews up memory.
<br>
There is just the header file and a main.cpp with some tests. To build you just need cmake and g++. Build steps are simply:<br>
cmake .<br>
make<br>
<br>
If memory consumption is an issue you may want to invoke Compress() once the trie is fully populated. It will recurse through the Trie calling shrink_to_fit() on each of the vectors that are used to store child nodes. There is also a ValidateState() method which performs validation of the nodes. It is only required for testing purposes. Lines 684 - 703 of main.cpp is some sample usage.
