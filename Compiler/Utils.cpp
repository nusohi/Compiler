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
	int op = 1, value = 0;
	if (str[0] == '-' || str[0] == '+') {
		op = (str[0] == '-' ? -1 : 1);
		str = str.substr(1);
	}
	for (char ch : str) {
		value *= 10;
		value += ch - '0';
	}
	return op * value;
}