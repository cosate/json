#include<iostream>
#include<string>
#include"json.h"
using namespace std;
using namespace gao;

int main()
{
	JsonValue json = JsonValue();
	string js = "{\"fdfd\":2}";
	
	loads(json, js);
	return 0;
}