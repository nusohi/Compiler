#include "pch.h"
#include "Utils.h"


vector<string> Split(char* str, char ch) {
	vector<string> res;
	istringstream is(str);

	string t;
	while (getline(is, t, ch)) {
		res.push_back(t);
	}
	return res;
}


int ParseInt(string str)
{
	int value = 0;
	for (char ch : str) {
		value *= 10;
		value += ch - '0';
	}
	return value;
}