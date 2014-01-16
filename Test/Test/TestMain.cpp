// alg_remove_if.cpp
// compile with: /EHsc
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;


int main( ) {
	ifstream inputFile("z:\\vminst.log");
	string fileData((istreambuf_iterator<char>(inputFile)),   // ∞—inputFile∂¡»Î
		istreambuf_iterator<char>());

	cout<<fileData;
}
