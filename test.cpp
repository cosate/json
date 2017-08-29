#include<iostream>
#include<string>
#include"json.h"
using namespace std;

int main()
{
	string s = "{\"fds\":1};
	gao::JsonValue json = gao::JsonValue(s);
	gao::JsonValue jv = json["\"fds\"];
	return 0;
}