#JSON

看[Milo Yip](http://github.com/miloyip/json-tutorial)大神的教程，理解了递归下降语法，之后用C++写了个JSON解析器和生成器，符合JSON标准。

***
##DOCUMENT

###数据类型和常量

JSON数据类型：`JSON_OBJECT, JSON_ARRAY, JSON_STRING, JSON_INTEGRAL, JSON_FLOAT, JSON_BOOL, JSON_NULL`

解析常量：`JSON_PARSE_OK, JSON_PARSE_ERROR`

生成常量：`JSON_STRINGIFY_OK, JSON_STRINGIFY_ERROR`

###类

* 类成员

	* `union Data`

		基础数据类型和指针的联合，表示一个JSON对象。

	* `type`

		JSON数据类型。
		
* 类API（部分）

	* 构造函数(部分)

		`JsonValue(initializer_list<JsonValue> list)`
			
		由初始化列表构造JSON对象。

		`JsonValue(const JsonValue& other)`

		复制构造函数。

		`JsonValue(JsonValue&& other)`

		由右值构造JSON对象。

	* `string getString() const`

		如果JSON类型为字符串，则返回该字符串，否则返回空串。

	* `int getSize()`

		如果JSON类型为对象或者数组，返回其size。

	* `string dump(int level)`

		递归调用自己，生成带缩进层次的字符串。

	* `ostream& operator<<(ostream& out, JsonValue& json)`
	
		重载流操作符。

	* `JsonValue& operator[](string s)`

		如果JSON类型为对象，取得键值s对应的JSON值。

	* `JsonValue& operator[](int index)`

		如果JSON类型为数组，取得index索引处的JSON值。

	* `append()`

		在构造JSON对象过程中，或是操作JSON对象时，添加新的键值对或者JSON对象。

###全局函数（部分）

`void loads(JsonValue& res, const string& str)`

从字符串str中解析JSON，存入res。该函数是提供给全局的解析函数。


##DEMO
	ifstream file;
	file.open("./sample-jsons/fathers.json");`
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