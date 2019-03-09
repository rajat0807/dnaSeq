#include <iostream>
#include <stack>
using namespace std;
int main() {
	stack <int> s;
	s.push(1);
	s.top()++;
	cout<<s.top()<<endl;
}