#include "pch.h"
#include "Utils.h"


vector<string> Split(char* str, char ch, bool noSpace) {
	vector<string> res;
	istringstream is(str);

	string t;
	while (getline(is, t, ch)) {
		res.push_back(t);
	}

	if (noSpace) {
		for (int i = 0; i < res.size();) {
			if (res[i] == "") 
				res.erase(res.begin() + i);
			else 
				i++;
		}
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