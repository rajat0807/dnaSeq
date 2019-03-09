
// C++ includes used for precompiling -*- C++ -*-

// Copyright (C) 2003-2013 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file stdc++.h
 *  This is an implementation file for a precompiled header.
 */

// 17.4.1.2 Headers

// C
#ifndef _GLIBCXX_NO_ASSERT
#include <cassert>
#endif
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#if __cplusplus >= 201103L
#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdalign>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cwchar>
#include <cwctype>
#endif

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#if __cplusplus >= 201103L
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#endif
using namespace std;
struct suffix 
{ 
	int index;
	int rank[2];
};
int cmp(struct suffix a, struct suffix b) 
{ 
	return (a.rank[0] == b.rank[0])? (a.rank[1] < b.rank[1] ?1: 0): 
		(a.rank[0] < b.rank[0] ?1: 0); 
}
vector<int> buildSuffixArray(string txt, int n) 
{
	struct suffix suffixes[n];
	for (int i = 0; i < n; i++) 
	{ 
		suffixes[i].index = i; 
		suffixes[i].rank[0] = txt[i] - 'a'; 
		suffixes[i].rank[1] = ((i+1) < n)? (txt[i + 1] - 'a'): -1; 
	}
	sort(suffixes, suffixes+n, cmp);
	int ind[n];
	for (int k = 4; k < 2*n; k = k*2) 
	{
		int rank = 0; 
		int prev_rank = suffixes[0].rank[0]; 
		suffixes[0].rank[0] = rank; 
		ind[suffixes[0].index] = 0;
		for (int i = 1; i < n; i++) 
		{
			if (suffixes[i].rank[0] == prev_rank && 
					suffixes[i].rank[1] == suffixes[i-1].rank[1]) 
			{ 
				prev_rank = suffixes[i].rank[0]; 
				suffixes[i].rank[0] = rank; 
			} 
			else
			{ 
				prev_rank = suffixes[i].rank[0]; 
				suffixes[i].rank[0] = ++rank; 
			} 
			ind[suffixes[i].index] = i; 
		}
		for (int i = 0; i < n; i++) 
		{ 
			int nextindex = suffixes[i].index + k/2; 
			suffixes[i].rank[1] = (nextindex < n)? 
								suffixes[ind[nextindex]].rank[0]: -1; 
		}
		sort(suffixes, suffixes+n, cmp); 
	}
	vector<int>suffixArr; 
	for (int i = 0; i < n; i++) 
		suffixArr.push_back(suffixes[i].index);
	return suffixArr; 
} 


vector<int> kasai(string txt, vector<int> suffixArr) 
{ 
	int n = suffixArr.size();
	vector<int> lcp(n, 0);
	vector<int> invSuff(n, 0);
	for (int i=0; i < n; i++) 
		invSuff[suffixArr[i]] = i;
	int k = 0;
	for (int i=0; i<n; i++) 
	{ 
		
		if (invSuff[i] == n-1) 
		{ 
			k = 0; 
			continue; 
		} 

		
		int j = suffixArr[invSuff[i]+1];
		while (i+k<n && j+k<n && txt[i+k]==txt[j+k]) 
			k++; 

		lcp[invSuff[i]] = k;
		if (k>0) 
			k--; 
	}
	return lcp; 
}
void printArr(vector<int>arr, int n) 
{ 
	for (int i = 0; i < n; i++) 
		cout << arr[i] << " "; 
	cout << endl; 
}
int main() 
{ 
	string str = "abaabaa"; 

	vector<int>suffixArr = buildSuffixArray(str, str.length()); 
	int n = suffixArr.size(); 
	suffixArr.insert(suffixArr.begin(), 0);

	cout << "Suffix Array : \n"; 
	printArr(suffixArr, n); 

	vector<int>lcp = kasai(str, suffixArr); 

	cout << "\nLCP Array : \n"; 
	printArr(lcp, n); 
	return 0; 
} 
