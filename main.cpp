// trie.cpp : Defines the entry point for the console application.
//
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <set>
#include <time.h>

#include "Trie.h"

using namespace TDS;

template <typename CharType>
void AddWord( const std::basic_string<CharType>& s, Trie<CharType>& t, std::map<const void *,std::basic_string<CharType> >& d)
{
	d[t.AddWord(s)]=s;
}

template <typename CharType>
void Search( const std::basic_string<CharType>& input, 
			 const Trie<CharType>& t,
			 std::vector<SearchResult<CharType> >& searchResults,
			 bool stopAtFirstMatch = false)
{
	for(size_t i = 0; i < input.size(); ++i)
	{
		t.Search(&input[i], &input[input.size()-1], searchResults, stopAtFirstMatch);
	}
}

void TESTf( bool t, const char * file, int line )
{
	if (!t)
	{
		std::cout << "Failure : " << file << " Line " << line << std::endl;
		throw(1);
	}
	std::cout << "Passed : " << file << " Line " << line << std::endl;
}

#define TEST(t)(TESTf(t,__FILE__, __LINE__))

void TestLessThan()
{
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;
					
	LessThanOtron<TrieNode<char>,char> lo;
	TrieNode<char> lhs('a'),rhs('d');

	TEST(lo(&lhs,'b'));
	TEST(lo('c',&rhs));
}

void TestTrieNode()
{
	TrieNode<char> t('a');
	TEST('a'==t.GetChar());
	void * pZ = t.AddNode('z');
	void * pD = t.AddNode('d');
	void * pB = t.AddNode('b');
	void * pF = t.AddNode('f');
	TEST(NULL == t.AddNode('f'));
	void * pX = t.AddNode('x');
	void * pC = t.AddNode('c');
	void * pE = t.AddNode('e');
	const TrieNode<char>& ctr = t;

//	TEST(t.ValidateState());

	TEST(pB==t.FindNode('b'));
	TEST(pC==t.FindNode('c'));
	TEST(pD==t.FindNode('d'));
	TEST(pE==t.FindNode('e'));
	TEST(pF==t.FindNode('f'));
	TEST(pX==t.FindNode('x'));
	TEST(pZ==t.FindNode('z'));

	TEST(pB==ctr.FindNode('b'));
	TEST(pC==ctr.FindNode('c'));
	TEST(pD==ctr.FindNode('d'));
	TEST(pE==ctr.FindNode('e'));
	TEST(pF==ctr.FindNode('f'));
	TEST(pX==ctr.FindNode('x'));
	TEST(pZ==ctr.FindNode('z'));
}

void TestEmptyDictionaryEmptyBuffer()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("", t, dictionary);
	const char * test = "";
	t.Search(test, test, searchResults);
	TEST(searchResults.size()==0);
	TEST(t.ValidateState());
}

void TestSingleCharacterNoResult()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("b", t, dictionary);
	const char * test = "a";
	t.Search(test, test, searchResults);
	TEST(searchResults.size()==0);
	TEST(t.ValidateState());
}

void TestSingleCharacterOneResult()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("a", t, dictionary);
	const char * test = "a";
	t.Search(test, test, searchResults);
	TEST(searchResults.size()==1);
	TEST(t.ValidateState());
	TEST(dictionary[searchResults[0].GetResult()]=="a");
	TEST(searchResults[0].GetPosition()==&test[0]);
}

void TestAlphabetEveryCharacterTriggers()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	std::string test;
	// build up a string of a to z
	for( char c = 'a'; c != '{'; ++c )
	{
		// add each character as a word
		AddWord<char>(std::string(1,c), t, dictionary);
		test.append(1,c);
	}
	Search( test, t, searchResults );
	TEST(searchResults.size()==26);
	size_t p = 0;
	for( char c = 'a' ; c != '{'; ++c )
	{
		TEST(dictionary[searchResults[p].GetResult()]==std::string(1,c));
		TEST(searchResults[p].GetPosition()==&test[p]);
		++p;
	}
	TEST(t.ValidateState());
}

void TestOverlap()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("ab", t, dictionary);
	AddWord<char>("bc", t, dictionary);
	t.Compress();
	std::string test("abc abc abcabc");
	Search( test, t, searchResults );
	TEST(searchResults.size()==8);

	TEST(dictionary[searchResults[0].GetResult()]=="ab");
	TEST(searchResults[0].GetPosition()==&test[1]);

	TEST(dictionary[searchResults[1].GetResult()]=="bc");
	TEST(searchResults[1].GetPosition()==&test[2]);

	TEST(dictionary[searchResults[2].GetResult()]=="ab");
	TEST(searchResults[2].GetPosition()==&test[5]);

	TEST(dictionary[searchResults[3].GetResult()]=="bc");
	TEST(searchResults[3].GetPosition()==&test[6]);

	TEST(dictionary[searchResults[4].GetResult()]=="ab");
	TEST(searchResults[4].GetPosition()==&test[9]);

	TEST(dictionary[searchResults[5].GetResult()]=="bc");
	TEST(searchResults[5].GetPosition()==&test[10]);

	TEST(dictionary[searchResults[6].GetResult()]=="ab");
	TEST(searchResults[6].GetPosition()==&test[12]);

	TEST(dictionary[searchResults[7].GetResult()]=="bc");
	TEST(searchResults[7].GetPosition()==&test[13]);

	TEST(t.ValidateState());
}

void TestOverlapDictionary()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("bat", t, dictionary);
	AddWord<char>("at", t, dictionary);
	t.Compress();
	std::string test("bat");
	Search( test, t, searchResults );
	
	TEST(searchResults.size()==2);

	TEST(dictionary[searchResults[0].GetResult()]=="bat");
	TEST(searchResults[0].GetPosition()==&test[2]);

	TEST(dictionary[searchResults[1].GetResult()]=="at");
	TEST(searchResults[1].GetPosition()==&test[2]);
}

void TestOverlapDictionaryShortestFirst()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("at", t, dictionary);
	AddWord<char>("bat", t, dictionary);
	
	t.Compress();
	std::string test("bat");
	Search( test, t, searchResults );
	
	TEST(searchResults.size()==2);

	TEST(dictionary[searchResults[0].GetResult()]=="bat");
	TEST(searchResults[0].GetPosition()==&test[2]);

	TEST(dictionary[searchResults[1].GetResult()]=="at");
	TEST(searchResults[1].GetPosition()==&test[2]);
}

void TestOverlapDictionaryShortestFirst2()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("at", t, dictionary);
	AddWord<char>("att", t, dictionary);
	
	t.Compress();
	std::string test("att");
	Search( test, t, searchResults );
	
	TEST(searchResults.size()==2);

	TEST(dictionary[searchResults[0].GetResult()]=="at");
	TEST(searchResults[0].GetPosition()==&test[1]);

	TEST(dictionary[searchResults[1].GetResult()]=="att");
	TEST(searchResults[1].GetPosition()==&test[2]);
}
void TestPartialNoMatch()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("smallest", t, dictionary);

	std::string test("small");
	Search( test, t, searchResults );
	TEST(searchResults.size()==0);
	TEST(t.ValidateState());
}

void TestMultiplePartialNoMatch()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("smallest", t, dictionary);

	std::string test("smallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmallsmall");
	Search( test, t, searchResults );
	TEST(searchResults.size()==0);
	TEST(t.ValidateState());
}

void TestPartialAtEndNoMatch()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("sentance", t, dictionary);
	AddWord<char>("thus", t, dictionary);

	std::string test("this is a test of a partial match at the start and end of the sentan");
	Search( test, t, searchResults );
	TEST(searchResults.size()==0);
	TEST(t.ValidateState());
}

void TestTwoTermsOneTrigger()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("fox", t, dictionary);
	AddWord<char>("dog", t, dictionary);
	AddWord<char>("hippo", t, dictionary);

	std::string test("the quick brown fox jumped over the lazy dog");
	Search( test, t, searchResults );
	TEST(searchResults.size()==2);

	TEST(dictionary[searchResults[0].GetResult()]=="fox");
	TEST(searchResults[0].GetPosition()==&test[18]);

	TEST(dictionary[searchResults[1].GetResult()]=="dog");
	TEST(searchResults[1].GetPosition()==&test[43]);

	TEST(t.ValidateState());
}

void TestTwoTermsOneTriggerWide()
{
	Trie<wchar_t> t;
	std::map<const void *,std::wstring> dictionary;
	std::vector<SearchResult<wchar_t> > searchResults;

	AddWord<wchar_t>(L"fox", t, dictionary);
	AddWord<wchar_t>(L"dog", t, dictionary);
	AddWord<wchar_t>(L"hippo", t, dictionary);

	std::wstring test(L"the quick brown fox jumped over the lazy dog");
	Search( test, t, searchResults );
	TEST(searchResults.size()==2);

	TEST(dictionary[searchResults[0].GetResult()]==L"fox");
	TEST(searchResults[0].GetPosition()==&test[18]);

	TEST(dictionary[searchResults[1].GetResult()]==L"dog");
	TEST(searchResults[1].GetPosition()==&test[43]);

	TEST(t.ValidateState());
}

void TestTwoTermsStopOnFirstMatch()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("fox", t, dictionary);
	AddWord<char>("foxes", t, dictionary);
	AddWord<char>("hippo", t, dictionary);

	std::string test("the quick brown foxes jumped over the lazy dog");
	Search( test, t, searchResults, true);
	TEST(searchResults.size()==1);

	TEST(dictionary[searchResults[0].GetResult()]=="fox");
	TEST(searchResults[0].GetPosition()==&test[18]);

	TEST(t.ValidateState());
}

void TestAllTermsTrigger()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	std::vector<std::string> quickBrownFox;

	quickBrownFox.push_back("the");
	quickBrownFox.push_back("quick");
	quickBrownFox.push_back("brown");
	quickBrownFox.push_back("fox");
	quickBrownFox.push_back("jumped");
	quickBrownFox.push_back("over");
	quickBrownFox.push_back("thy");
	quickBrownFox.push_back("lazy");
	quickBrownFox.push_back("dog");

	std::string test;
	for(size_t i = 0; i < quickBrownFox.size(); i++)
	{
		AddWord<char>(quickBrownFox[i], t, dictionary);
		test.append(quickBrownFox[i]);
		test.append(" ");
	}

	Search( test, t, searchResults );
	TEST(searchResults.size()==quickBrownFox.size());

	for(size_t i = 0; i < quickBrownFox.size(); i++)
	{
		TEST(dictionary[searchResults[i].GetResult()]==quickBrownFox[i]);
		size_t pos = test.find(quickBrownFox[i]);
		pos += quickBrownFox[i].size()-1;
		TEST(searchResults[i].GetPosition()==&test[pos]);
	}

	TEST(t.ValidateState());
}

void TestThreeTermsFourTriggers()
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("cat", t, dictionary);
	AddWord<char>("car", t, dictionary);
	AddWord<char>("cab", t, dictionary);
	t.Compress();
	std::string test("caz cat sat on the car. it was a cab. cat");
	Search( test, t, searchResults );
	TEST(searchResults.size()==4);

	TEST(dictionary[searchResults[0].GetResult()]=="cat");
	TEST(searchResults[0].GetPosition()==&test[6]);

	TEST(dictionary[searchResults[1].GetResult()]=="car");
	TEST(searchResults[1].GetPosition()==&test[21]);

	TEST(dictionary[searchResults[2].GetResult()]=="cab");
	TEST(searchResults[2].GetPosition()==&test[35]);

	TEST(dictionary[searchResults[3].GetResult()]=="cat");
	TEST(searchResults[3].GetPosition()==&test[40]);

	TEST(t.ValidateState());
}

void TestPartialDictionaryTerms()	
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("a", t, dictionary);
	AddWord<char>("ab", t, dictionary);
	AddWord<char>("abc", t, dictionary);
	AddWord<char>("abcd", t, dictionary);
	AddWord<char>("abcde", t, dictionary);
	AddWord<char>("abcdef", t, dictionary);
	AddWord<char>("abcdefg", t, dictionary);

	std::string test("abc abcdefg a a a abcdef abcd abcd");
	Search( test, t, searchResults );

	TEST(dictionary[searchResults[0].GetResult()]=="a");
	TEST(searchResults[0].GetPosition()==&test[0]);

	TEST(27==searchResults.size());
	TEST(t.ValidateState());
}

void TestWordsWorth()
{
	std::string wordsWorth = 
			"A Whirl-Blast from behind the hill "
			"Rushed o'er the wood with startling sound; "
			"Then--all at once the air was still, "
			"And showers of hailstones pattered round. "
			"Where leafless oaks towered high above, "
			"I sat within an undergrove "
			"Of tallest hollies, tall and green; "
			"A fairer bower was never seen. "
			"From year to year the spacious floor "
			"With withered leaves is covered o'er, "
			"And all the year the bower is green. "
			"But see! where'er the hailstones drop "
			"The withered leaves all skip and hop; "
			"There's not a breeze--no breath of air-- "
			"Yet here, and there, and everywhere "
			"Along the floor, beneath the shade "
			"By those embowering hollies made, "
			"The leaves in myriads jump and spring, "
			"As if with pipes and music rare "
			"Some Robin Good-fellow were there, "
			"And all those leaves, in festive glee, "
			"Were dancing to the minstrelsy. ";
	
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	AddWord<char>("leaves", t, dictionary);
	AddWord<char>("leafless", t, dictionary);
	AddWord<char>("bower", t, dictionary);
	AddWord<char>("green", t, dictionary);
	AddWord<char>("no", t, dictionary);
	AddWord<char>("see", t, dictionary);
	AddWord<char>("shade", t, dictionary);
	AddWord<char>("ryhme", t, dictionary);

	Search( wordsWorth, t, searchResults );

	std::string strToFind("leafless");
	size_t pos = wordsWorth.find(strToFind);

	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[0].GetPosition() );

	strToFind = "green";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[1].GetPosition() );

	strToFind = "bower";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[2].GetPosition() );

	strToFind = "see";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[3].GetPosition() );

	strToFind = "leaves";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[4].GetPosition() );

	strToFind = "bower";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[5].GetPosition() );

	strToFind = "green";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[6].GetPosition() );

	strToFind = "see";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[7].GetPosition() );

	strToFind = "leaves";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[8].GetPosition() );

	strToFind = "no";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[9].GetPosition() );
	pos++;
	strToFind = "no";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[10].GetPosition() );

	strToFind = "shade";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[11].GetPosition() );

	strToFind = "bower";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[12].GetPosition() );

	strToFind = "leaves";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[13].GetPosition() );
	pos++;
	strToFind = "leaves";
	pos = wordsWorth.find(strToFind,pos);
	TEST( &wordsWorth[pos+strToFind.size()-1] == searchResults[14].GetPosition() );

	TEST( 15 == searchResults.size() );
	TEST(t.ValidateState());
}

void TestRandomWords(size_t numberOfWords, size_t minWordLength, size_t maxWordLength)
{
	Trie<char> t;
	std::map<const void *,std::string> dictionary;
	std::vector<SearchResult<char> > searchResults;

	std::string infiniteMonkeys;
	std::default_random_engine generator;
	std::uniform_int_distribution<char> dRandLetter('a','z');
	std::uniform_int_distribution<size_t> dRandSize(minWordLength,maxWordLength);
	std::set<std::string> wordsAddedToDictionary;

	for( size_t wordCount = 0; wordCount < numberOfWords; ++wordCount )
	{
		std::string randomWord;
		for( size_t wordSize = 0; wordSize < dRandSize(generator); ++wordSize )
		{
			char c = dRandLetter(generator);
			randomWord.append(1,c);
		}
		infiniteMonkeys.append(randomWord);
		if (wordCount % 100 == 0)
		{
			std::set<std::string>::iterator it = wordsAddedToDictionary.find(randomWord);
			// only add unique words
			if (it == wordsAddedToDictionary.end())
			{
				wordsAddedToDictionary.insert(randomWord);
				AddWord<char>(randomWord, t, dictionary);
			}
		}

		infiniteMonkeys.append(" ");
	}

	// and now do the search. This is a simple stress test, need to check
	// check results against std::find();
	Search( infiniteMonkeys, t, searchResults );
	// iterate through every word in dictionary
	std::set<std::string>::iterator it = wordsAddedToDictionary.begin();
	for( ; it != wordsAddedToDictionary.end(); ++it )
	{
		// first loop uses a std::find and pushes results into
		// findResults vector.
		std::string strToFind = *it;
		size_t pos = 0;
		std::vector<const char *> findResults;
		while (pos != std::string::npos)
		{
			pos = infiniteMonkeys.find(strToFind,pos);
			if (pos != std::string::npos)
			{
				const char * a = &infiniteMonkeys[pos+strToFind.size()-1];
				findResults.push_back(a);
				++pos;
			}
		}
		// now search through the trie results. Push
		// results into trieResults vector.
		std::vector<const char *> trieResults;
		for( size_t r = 0; r < searchResults.size(); ++r )
		{
			std::string strToFind = dictionary[searchResults[r].GetResult()];
			
			if (strToFind == *it)
			{
				const char * b = searchResults[r].GetPosition();
				trieResults.push_back(b);
			}
		}
		// now compare the 2 vectors. Should be identical.
		if (findResults.size()!=trieResults.size())
		{
			TEST( false );
		}
		for ( size_t lp = 0; lp < findResults.size(); ++lp)
		{
			const char * a = findResults[lp];
			const char * b = trieResults[lp];
			if( findResults[lp] != trieResults[lp] )
			{
				TEST( false );
			}
		}
	}
}

int main(int argc, char* argv[])
{
	TestOverlapDictionaryShortestFirst2();
	TestLessThan();
	TestTrieNode();
	TestEmptyDictionaryEmptyBuffer();
	TestSingleCharacterNoResult();
	TestSingleCharacterOneResult();
	TestAlphabetEveryCharacterTriggers();
	TestOverlap();
	TestOverlapDictionary();
	TestOverlapDictionaryShortestFirst();
	TestPartialNoMatch();
	TestMultiplePartialNoMatch();
	TestPartialAtEndNoMatch();
	TestTwoTermsOneTrigger();
	TestTwoTermsOneTriggerWide();
	TestTwoTermsStopOnFirstMatch();
	TestAllTermsTrigger();
	TestThreeTermsFourTriggers();
	TestPartialDictionaryTerms();
	TestWordsWorth();
	TestRandomWords(300, 3, 5);
	TestRandomWords(1000, 3, 10);
	TestRandomWords(10000, 4, 12);

	// sample code: simple example of how to use the Trie.
	Trie<char> t;
	std::vector<SearchResult<char> > searchResults;
	// add words to search for. Record the return from AddWord as this is how
	// you identify the match
	const void * fox = t.AddWord("fox");
	const void * dog = t.AddWord("dog");
	// string to search
	std::string test("the quick brown fox jumped over the lazy dog");
	
	Search( test, t, searchResults );

	TEST(searchResults.size()==2);
	// found term fox
	TEST(searchResults[0].GetResult()==fox);
	// NB 18 is the end of the match term
	TEST(searchResults[0].GetPosition()==&test[18]);
	// found term dog
	TEST(searchResults[1].GetResult()==dog);
	TEST(searchResults[1].GetPosition()==&test[43]);

	return 0;
}

