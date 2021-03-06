// This source was written by Stephen Oswin, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.

#ifndef __TRIE_H
#define __TRIE_H

#include <string>
#include <vector>

namespace TDS
{

// less than functor. Used when perfoming a lower_bound() call.
template <class TrieNodeType, typename CharType>
struct LessThanOtron
{   
	bool operator()( const TrieNodeType* lhs, const CharType rhs ) const
	{
		return(lhs->GetChar() < rhs);
	}   
	bool operator()( const CharType lhs, const TrieNodeType* rhs ) const    
	{
		return(lhs < rhs->GetChar());
	}
};

// Trie Node
template<typename CharType>
class TrieNode
{
	public:
		TrieNode (CharType c) : _c(c), _endOfWord(false), _childNodes(NULL) {}
		~TrieNode()
		{
			if (_childNodes)
			{
				for( size_t i =  0; i < _childNodes->size(); ++i)
				{
					delete _childNodes->at(i);
				}
				delete _childNodes;
			}
		}

		// Gets the character for the node
		CharType GetChar() const { return _c; }

		// compress shrinks down all the vectors
		void Compress()
		{
			if (_childNodes && !_childNodes->empty())
			{
				_childNodes->shrink_to_fit();
				typename std::vector<TrieNode*>::const_iterator i = _childNodes->begin();
				for (; i != _childNodes->end() ; ++i) 
				{
					// recurse down the tree and call Compress
					(*i)->Compress();
				}
			}
		}

		// invariant
		bool ValidateState() const
		{
			// if we have a childnodes vector it should never be empty
			if (_childNodes && _childNodes->empty())
			{
				return false;
			}

			if (_childNodes)
			{
				// check child nodes are sorted AND there are NO duplicates
				typename std::vector<TrieNode*>::const_iterator i = _childNodes->begin();

				CharType lastChar = (*i)->GetChar();
				if (false==(*i)->ValidateState())
				{
					return false;
				}
				++i;
				for (; i != _childNodes->end() ; ++i) 
				{
					// recurse down the tree and call validate
					if (false==(*i)->ValidateState())
					{
						return false;
					}

					if( lastChar > (*i)->GetChar() )
					{
						return false;
					}
					lastChar = (*i)->GetChar();
				}
			}
			else if (!this->IsEndOfWord())
			{
				return false;
			}

			return true;
		}
		// AddNode adds a new node. This function
		// must keep the node list in alphabetical order.
		TrieNode* AddNode (CharType c) 
		{
			if (NULL==_childNodes)
			{
				_childNodes = new std::vector<TrieNode*>;
			}
			TrieNode* newNode = NULL;

			typename std::vector<TrieNode*>::iterator i = std::lower_bound(_childNodes->begin(), _childNodes->end(), c, LessThanOtron<TrieNode,CharType>());
			
			if (i == _childNodes->end())
			{
				newNode = new TrieNode(c);
				_childNodes->push_back(newNode);
				return newNode;
			}
			else if ((*i)->GetChar()==c)
			{
				// duplicate node? Hopefully this wont happen...
				return NULL;
			}
			newNode = new TrieNode(c);
			// now insert just before i
			_childNodes->insert (i, newNode);
			return newNode;
		}
		// indicates this is an end node (ie no child nodes)
		bool IsEndNode() const  { return NULL==_childNodes; }
		// indicates this node terminates a word
		bool IsEndOfWord() const  { return _endOfWord; }
		// setter for end of word flag
		void SetEndOfWord() { _endOfWord = true; }
		// FindNode searches child nodes for
		// a particular TrieNode
		const TrieNode* FindNode( CharType c ) const
		{
			if (_childNodes)
			{
				typename std::vector<TrieNode*>::const_iterator i = std::lower_bound(_childNodes->begin(), _childNodes->end(), c, LessThanOtron<TrieNode,CharType>());
				if ((i!=_childNodes->end()) && ((*i)->GetChar()==c))
				{
					return *i;
				}
			}
			return NULL;
		}

		// Un-const overload of above
		TrieNode* FindNode( CharType c )
		{
			if (_childNodes)
			{
				typename std::vector<TrieNode*>::iterator i = std::lower_bound(_childNodes->begin(), _childNodes->end(), c, LessThanOtron<TrieNode,CharType>());
				if ((i!=_childNodes->end()) && ((*i)->GetChar()==c))
				{
					return *i;
				}
			}
			return NULL;
		}
	private:
		TrieNode& operator=(const TrieNode& rhs);
		TrieNode(const TrieNode& rhs);

	protected:
		CharType _c;
		std::vector<TrieNode*>* _childNodes;
		bool _endOfWord;
};

template<typename CharType>
class SearchResult
{
	public:
		SearchResult( const void * result, const CharType * position )
			: _result(result), _position(position)
		{
		}
		// handle to matching term (same as the value returned by AddWord()
		const void * GetResult() const
		{
			return _result;
		}
		// offset of last character of match
		const CharType * GetPosition() const
		{
			return _position;
		}

	protected:
		const void * _result;
		const CharType * _position;
};

template<typename CharType>
class Trie
{
	public:
		Trie(){ _rootNode = new TrieNode<CharType>(0L); }
		~Trie(){ delete _rootNode; }

		// Search function. Requires pointers to the start / end of the input buffer to search.
		// Performs a case sensitive search.
		// searchResults contains a pointer to the trie node at the end of the word
		// that matched. This pointer is the same as the pointer returned from the
		// call to AddWord(). It should be treated as just a way to identify the match term.
		// SearchResults is a vector of all the results.
		// The function will return after the first longest match including all sub matches.
		// stopAtFirstMatch will halt on the first sub match.
		void Search(const CharType* buffStart,
					const CharType* buffEnd,
					std::vector<SearchResult<CharType> >& searchResults,
					bool stopAtFirstMatch = false) const
		{
			const TrieNode<CharType> * pTN	= _rootNode;
			const CharType* buff = buffStart;

			while (buff <= buffEnd)
			{
				// look for the character in the child nodes
				pTN = pTN->FindNode(*buff);
				// not found
				if (NULL == pTN)
				{
					return;
				}
				else if (pTN->IsEndOfWord())
				{
					// found a word, could match more though.
					// at this point need to push result onto vector and continue...
					SearchResult<CharType> st(pTN,buff);
					searchResults.push_back(st);

					// If you want to stop at the first match you can
					// return here
					if (stopAtFirstMatch)
					{
						return;
					}

					buff++;
				}
				else
				{
					buff++;
				}
			}
			return;
		}
		
		// Simply adds a word to the Trie. The return value should be stored as it will
		// be required to identify the matching term.
		const void* AddWord( const std::basic_string<CharType>& s )
		{
			if (!s.empty())
			{
				return AddWord(s.c_str(), &s[s.size()-1]);
			}
			return NULL;
		}

		const void* AddWord( const CharType* p, const CharType* end )
		{
			TrieNode<CharType> * pTN = _rootNode;
			while (pTN && p <= end)
			{
				TrieNode<CharType>* pNext = pTN->FindNode(*p);

				if (NULL == pNext)
				{
					// not found so add
					pTN = pTN->AddNode(*p);
				}
				else
				{
					// found, set current node to this one
					pTN = pNext;
				}

				++p;
			}
			// last node so set the word flag
			pTN->SetEndOfWord();

			return pTN;
		}
		// this calls shrink_to_fit on each of the vectors
		// this call is recursive
		void Compress()
		{
			_rootNode->Compress();
		}
		// invariant. check that trie is in a valid state
		bool ValidateState() const
		{
			if (_rootNode->IsEndNode())
			{
				// empty trie
				return true;
			}

			return _rootNode->ValidateState();
		}
		
	protected:
		TrieNode<CharType>* _rootNode;
};
}
#endif

