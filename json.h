#pragma once
#include<iostream>
#include<map>
#include<vector>
#include<string>
#include<initializer_list>
#include<type_traits>
using namespace std;

namespace gao
{
	enum class Type{JSON_OBJECT = 0, JSON_ARRAY, JSON_STRING, JSON_INTEGRAL, JSON_FLOAT, JSON_BOOL, JSON_NULL};
	enum class JsonParseStatus{JSON_PARSE_OK = 0, JSON_PARSE_ERROR};
	enum class JsonStringifyStatus { JSON_STRINGIFY_OK = 0, JSON_STRINGIFY_ERROR};

	class JsonValue
	{
		friend ostream& operator<< (ostream&, JsonValue&);
	private:
		union Data
		{
			Data() : i(0) {}
			Data(double dou) : d(dou) {}
			Data(long long ll) : i(ll) {}
			Data(bool bo) : b(bo) {}
			Data(string ss) : s(new string(ss)) {}
			
			double d;
			long long i;
			bool b;
			string* s;
			map<string,JsonValue>* dict;
			vector<JsonValue>* arr;
		}data;
		Type type;	
	public:
		JsonValue():data(), type(Type::JSON_NULL) {}

		//构造函数模板
		template<class T>
		JsonValue(T b, typename enable_if<is_same<T, bool>::value>::type* = nullptr):data(b), type(Type::JSON_BOOL) {}
		
		template<class T>
		JsonValue(T i, typename enable_if<is_integer<T>::value && !is_same<T, bool>::value>::type* = nullptr):data((long long)i),type(Type::JSON_INTEGRAL) {}
		
		template<class T>
		JsonValue(T d, typename enable_if<is_floatint_point<T>::value>::type* = nullptr):data((double)d), type(Type::JSON_FLOAT) {}
		
		template<class T>
		JsonValue(T s, typename<is_convertible<T, string>::value>::type* = nullptr):data(s), type(Type::JSON_STRING){}
		
		template<class T, class... Args>
		JsonValue(T t, Args... args) : type(Type::JSON_ARRAY)
		{
			data.arr = new vector<JsonValue>();
			arr->emplace_back(t);
			append(args...);
		}
		
		template<class T>
		void append(T t)
		{
			data.arr->emplace_back(t);
		}
		
		template<class T, Class... Args>
		void append(T t, Args... args)
		{
			data.arr->emplace_back(t);
			append(args...);
		}
		
		//初始化列表
		JsonValue(initializer_list<JsonValue> list):type(Type::JSON_ARRAY)
		{
			data.arr = new vector<JsonValue>(list.size());
			size_t index = 0;
			for(auto it = list.begin(); i != list.end(); i++)
			{
				arr[index] = *it;
				index++;
			}
		}
		
		//复制构造函数和赋值操作符
		JsonValue(const JsonValue& other)
		{
			type = other.type;
			switch(type)
			{
				case Type::JSON_ARRAY:
					data.arr = new vector<JsonValue>(other.data.arr->begin(), other.data.arr->end());
					break;
				case Type::JSON_OBJECT:
					data.dict = new map<string, JsonValue>(other.data.dict->begin(), other.data.dict->end());
					break;
				case Type::JSON_STRING:
					data.s = new string(*(other.data.s));
					break;
				default:
					data = other.data;
			}
		}
		
		JsonValue(JsonValue&& other) : type(other.type), data(other.data)
		{
			other.type = Type::JSON_NULL;
			other.data.i = 0;
		}
		
		JsonValue& operator=(const JsonValue& other)
		{
			setType(other.type);
			switch(other.type)
			{
				case Type::JSON_ARRAY:
					data.arr = new vector<JsonValue>(other.data.arr->begin(), other.data.arr->end());
					break;
				case Type::JSON_OBJECT:
					data.dict = new map<string, JsonValue>(other.data.dict->begin(), other.data.dict->end());
					break;
				case Type::JSON_STRING:
					data.s = new string(*(other.data.s));
					break;
				default:
					data = other.data;
			}
			return *this;
		}
		
		JsonValue& operator=(JsonValue&& other)
		{
			setType(other.type);
			data = other.data;
			other.type = Type::JSON_NULL;
			other.data.i = 0;
			return *this;
		}
		
		//析构函数
		~JsonValue()
		{
			switch(type)
			{
				case Type::JSON_ARRAY:
					delete data.arr;
					break;
				case Type::JSON_OBJECT:
					delete data.dict;
					break;
				case Type::JSON_STRING:
					delete data.s;
					break;
				default:
					;
			}
		}

		//操作
		template<class T>
		T operator[](string s) {}
		template<class T>
		T index(int i) {}
		template<class T>
		bool operator==(JsonValue& other) {}
		
		
		JsonValue& remove(JsonValue);
		JsonValue& setValue(int);
		JsonValue& setValue(string);
		Type getType();

		JsonValue getValue(int);
		JsonValue getValue(string);
		
		Type JSONType() const
		{
			return type;
		}
		
		void encode();
		void decode();
	private:
		void setType(Type t)
		{
			if(t == type)
				return;
			
			clearData();
			type = t;
			switch(t)
			{
				case Type::JSON_ARRAY:
					data.arr = new vector<JsonValue>();
					break;
				case Type::JSON_OBJECT:
					data.dict = new map<string, JsonValue>();
					break;
				case Type::JSON_STRING:
					data.s = new string();
					break;
				case Type::JSON_NULL:
					data.i = 0;
					break;
				case Type::JSON_INTEGRAL:
					data.i = 0;
					break;
				case Type::JSON_FLOAT:
					data.d = 0.0;
					break;
				case Type::JSON_BOOL:
					data.b = false;
					break;
				default:
					;
			}
		}
		
		void clearData()
		{
			switch(type)
			{
				case Type::JSON_ARRAY:
					delete data.arr;
					break;
				case Type::JSON_OBJECT:
					delete data.dict;
					break;
				case Type::JSON_STRING:
					delete data.s;
					break;
				default:
					;
			}
		}
	};
	
	void loads(JsonValue& res, const string& str)
	{
		if(parse(res, str, 0) == JsonParseStatus::JSON_PARSE_OK)
			cout<<"Load succeeded"<<endl;
		else
			cerr<<"Load failed"<<endl;
	}
	
	void dumps(const JsonValue& json, string& res)
	{
		
	}
	
	inline bool iswhitespace(char c)
	{
		return (c == ' ' || c == '\n' || c == '\r' || c == '\t');
	}
	
	inline bool isdigit(char c)
	{
		return (c >='0' && c <= '9');
	}
	
	inline void pass_whitespace(const string& str, size_t& offset)
	{
		while(iswhitespace(str[offset]))
			++offset;
	}
		
	JsonParseStatus parse(JsonValue& res, const string& str, size_t& offset)
	{
		pass_whitespace(str, offset);
		switch(str[offset])
		{
			case '[':
				return parse_array(str,offset);
			case '{':
				return parse_object(str, offset);
			case '\"':
				return parse_string(str, offset);
			case 't':
			case 'f':
				return parse_bool(str, offset);
			case 'n':
				return parse_null(str, offset);
			default:
				return parse_number(str, offset);
		}
	}
	
	JsonParseStatus parse_object(JsonValue& res, const string& str, size_t& offset)
	{
		++offset;
		pass_whitespace(str, offset);
	}
	
	JsonParseStatus parse_array(JsonValue& res, const string& str, size_t& offset)
	{
		++offset;
		pass_whitespace(str, offset);
	}
	
	JsonParseStatus parse_number(JsonValue& res, const string& str, size_t& offset)
	{
		string val = "", exp = "";
		bool isfloat = false;
		if(str[offset] == '-')
		{
			val += '-';
			++offset;
		}
		char c = str[offset];
		while(isdigit(c) || (c == '.' && !isfloat))
		{
			if(c == '.' && !isfloat)
				isfloat = true;
			val += c;
			c = str[++offset];
		}
		if(val.empty() || val == "-" || val == "." || val == "-.")
			return JsonParseStatus::JSON_PARSE_ERROR;
		
		if(c == 'e' || c == 'E')
		{
			++offset;
			if(str[offset] == '-')
			{
				exp += '-';
				++offset;
			}
			c = str[offset];
			while(isdigit(c))
			{
				exp += c;
				c = str[++offset];
			}
			if(exp.empty() || exp == "-")
				return JsonParseStatus::JSON_PARSE_ERROR;
		}
		
		pass_whitespace(str, offset);
		c = str[offset];
		if(c != '\0' && c != ',' && c != ']' && c != '}')
			return JsonParseStatus::JSON_PARSE_ERROR;
		if(exp.empty())
		{
			if(isfloat)
			{
				double dou = stod(val);
				res = JsonValue(dou);
			}
			else
			{
				long long ll = stol(val);
				res = JsonValue(ll);
			}
		}
		else
		{
			double dou = stod(val) * pow(10, stol(exp));
			res = JsonValue(dou);
		}
		return JsonParseStatus::JSON_PARSE_OK;
	}
	
	JsonParseStatus parse_bool(JsonValue& res, const string& str, size_t offset)
	{
		if(str[offset] == 't')
			return parse_true(res, str, offset);
		else
			return parse_false(res, str, offset);
	}
	
	JsonParseStatus parse_true(JsonValue& res, const string& str, size_t offset)
	{
		if(!(str[offset] == 't' && str[offset + 1] == 'r' && str[offset + 2] == 'u' && str[offset + 3] == 'e'))
			return JsonParseStatus::JSON_PARSE_ERROR;
		else
			offset += 4;
		pass_whitespace(str, offset);
		char c = str[offset];
		if(c != '\0' && c != ',' && c != ']' && c != '}')
			return JsonParseStatus::JSON_PARSE_ERROR;
		res = JsonValue();
		return JsonParseStatus::JSON_PARSE_OK;
	}
	
	JsonParseStatus parse_false(JsonValue& res, const string& str, size_t offset)
	{
		if(!(str[offset] == 'f' && str[offset + 1] == 'a' && str[offset + 2] == 'l' && str[offset + 3] == 's' && str[offset + 4] == 'e'))
			return JsonParseStatus::JSON_PARSE_ERROR;
		else
			offset += 5;
		pass_whitespace(str, offset);
		char c = str[offset];
		if(c != '\0' && c != ',' && c != ']' && c != '}')
			return JsonParseStatus::JSON_PARSE_ERROR;
		res = JsonValue();
		return JsonParseStatus::JSON_PARSE_OK;
	}
	
	JsonParseStatus parse_null(JsonValue& res, const string& str, size_t offset)
	{
		if(!(str[offset] == 'n' && str[offset + 1] == 'u' && str[offset + 2] == 'l' && str[offset + 3] == 'l'))
			return JsonParseStatus::JSON_PARSE_ERROR;
		else
			offset += 4;
		pass_whitespace(str, offset);
		char c = str[offset];
		if(c != '\0' && c != ',' && c != ']' && c != '}')
			return JsonParseStatus::JSON_PARSE_ERROR;
		res = JsonValue();
		return JsonParseStatus::JSON_PARSE_OK;
	}
	
	JsonParseStatus parse_string(JsonValue& res, const string& str, size_t offset)
	{
		string val = "";
		char c;
		do
		{
			c = str[++offset];
			if(c == '\\')
			{
				switch(str[++offset])
				{
					case '\"': val += '\"'; break;
					case '\\': val += '\\'; break;
					case '/' : val += '/' ; break;
					case 'b' : val += '\b'; break;
					case 'f' : val += '\f'; break;
					case 'n' : val += '\n'; break;
					case 'r' : val += '\r'; break;
					case 't' : val += '\t'; break;
					case 'u' :
				}
			}
		}while(c != '\"');
	}
}