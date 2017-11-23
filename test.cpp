#include<iostream>
#include<string>
#include<fstream>
#include"json.h"
using namespace std;
using namespace gao;

int main()
{

	JsonValue j1 = JsonValue(0);

	string kk = "fasdfa";
	JsonValue j2 = JsonValue(kk);

	JsonValue j3 = JsonValue(true);

	JsonValue j4 = JsonValue(0.34);
	
	JsonValue j5, j6, j7;
	
	loads(j5, "\"fdasf\"       ");

	loads(j6, "{\"ppp\"    :   [1,2,true,{\"ppppp\":    \"fdfd\" ,   \"pppppppp\"  : 2.45, \"ppppppppppp\"            :    true   }   ]  ,   \"qq\"   : {\"qqq\" : false}}");

	loads(j7, "        [[[3.4, \"fds\"], {\"fdfd\" : 444}],  \"gg\", 44, true] ");
	
	cout << j1 << endl;
	cout << j2 << endl;
	cout << j2 << endl;
	cout << j3 << endl;
	cout << j4 << endl;
	cout << j5 << endl;
	cout << j6 << endl;

	j7.append(j6, j4, j3);
	cout << j7 << endl;

	string k = "高俊祥";
	JsonValue j8 = JsonValue(k);

	JsonValue j9(k);

	cout << j8 << endl;

	if (j9 == j8)
		cout << "yeah" << endl;
	
	cout << "---------------------测试案例---------------------" << endl;

	ifstream file;
	file.open("./sample-jsons/fathers.json");
	string res;
	string temp;
	while (getline(file, temp))
	{
		res.append(temp);
		temp = "";
	}
	file.close();
	JsonValue j10;
	loads(j10, res);
	cout << j10 << endl;
	
	return 0;
}