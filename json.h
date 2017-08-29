#pragma once
#include<iostream>
#include<string>
#include<initializer_list>
#include<type_traits>
using namespace std;

namespace gao
{
	enum class Type{JSON_OBJECT = 0, JSON_ARRAY, JSON_STRING, JSON_INTEGRAL, JSON_FLOAT, JSON_BOOL, JSON_NULL};
	enum class JsonParseStatus{JSON_PARSE_OK = 0, JSON_PARSE_ERROR};
	enum class JsonStringifyStatus { JSON_STRINGIFY_OK = 0, JSON_STRINGIFY_ERROR};

	struct JsonNode
	{
		string key;
		JsonValue value;
		JsonNode* next;
	};

	class JsonValue
	{
		friend ostream& operator<< (ostream&, JsonValue&);
	public:
		JsonValue():data(), type(Type::JSON_NULL) {}

		//构造函数模板
		template<class T>
		JsonValue(T b, typename enable_if<is_same<T, bool>::value>::type* = NULL):data(b), type(Type::JSON_BOOL) {}
		
		template<class T>
		JsonValue(T i, typename enable_if<is_integer<T>::value && !is_same<T, bool>::value>::type* = NULL):data((long long)i),type(Type::JSON_INTEGRAL) {}
		
		template<class T>
		JsonValue(T d, typename enable_if<is_floatint_point<T>::value>::type* = NULL):data((double)d), type(Type::JSON_FLOAT) {}
		
		template<class T>
		JsonValue(T s, typename<is_convertible<T, string>::value>::type* = NULL):data(s), type(Type::JSON_STRING){}

		//初始化列表
		JsonValue(initializer_list<JsonValue> list):type(Type::JSON_ARRAY)
		{
			data.arr = NULL;
			JsonValue* temp = data.arr;
			for(auto i = list.begin(); i != list.end(); i++)
			{
				temp->data = *i;
				temp = temp->next;
			}
		}
		
		//复制构造函数和赋值操作符
		JsonValue(const JsonValue& other) {}
		JsonValue(JsonValue&& other) {}
		JsonValue& operator=(const JsonValue& other) {}
		JsonValue& operator=(JsonValue&& other) {}
		
		~JsonValue(){}

		//操作
		template<class T>
		T operator[](string s) {}
		template<class T>
		T index(int i) {}
		template<class T>
		T operator==(JsonValue& other) {}

		JsonValue& append(JsonValue&);
		JsonValue& remove(JsonValue);
		JsonValue& setValue(int);
		JsonValue& setValue(string);
		Type getType();

		JsonValue getValue(int);
		JsonValue getValue(string);

		void decode();

	private:
		union Data
		{
			Data():i(0) {}
			Data(double dou) : d(dou) {}
			Data(long long ll) : i(ll) {}
			Data(bool bo) : b(bo) {}
			Data(string ss) : s(new string(ss)) {}
			double d;
			long long i;
			bool b;
			string* s;
			JsonNode* nodes;
			JsonValue* arr;
		}data;
		Type type;
		
		void setType(Type t)
		{
			if(t == type)
				return;
			
		}
	};

	JsonValue parse_json(const string);

	void pass_whitespace();

	JsonValue parse_string();

	JsonValue parse_number();

	JsonValue parse_array();

	JsonValue parse_bool();

	JsonValue parse_null();

	string stringify(JsonValue);
}