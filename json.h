#ifndef GAO_JSON_H
#define GAO_JSON_H

#include<iostream>
#include<map>
#include<vector>
#include<string>
#include<initializer_list>
#include<type_traits>
#include<cmath>
using namespace std;

namespace gao
{
	enum class Type { JSON_OBJECT = 0, JSON_ARRAY, JSON_STRING, JSON_INTEGRAL, JSON_FLOAT, JSON_BOOL, JSON_NULL };
	enum class JsonParseStatus { JSON_PARSE_ERROR = 0, JSON_PARSE_OK };
	enum class JsonStringifyStatus { JSON_STRINGIFY_OK = 0, JSON_STRINGIFY_ERROR };

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
			map<string, JsonValue>* dict;
			vector<JsonValue>* arr;
		}data;
		Type type;
	public:
		JsonValue() :data(), type(Type::JSON_NULL) {}

		//构造函数模板
		template<class T>
		JsonValue(T b, typename enable_if<is_same<T, bool>::value>::type* = nullptr) : data(b), type(Type::JSON_BOOL) {}

		template<class T>
		JsonValue(T i, typename enable_if<is_integral<T>::value && !is_same<T, bool>::value>::type* = nullptr) : data((long long)i), type(Type::JSON_INTEGRAL) {}

		template<class T>
		JsonValue(T d, typename enable_if<is_floating_point<T>::value>::type* = nullptr) : data((double)d), type(Type::JSON_FLOAT) {}

		template<class T>
		JsonValue(T s, typename enable_if<is_convertible<T, string>::value>::type* = nullptr) : data(string(s)), type(Type::JSON_STRING) {}

		JsonValue(nullptr_t) : data(), type(Type::JSON_NULL) {}

		template<class T>
		JsonValue(T* t) = delete;

		JsonValue(const vector<JsonValue>& vec) {}

		JsonValue(const JsonValue* ptr) {}


		template<class T, class... Args>
		JsonValue(T t, Args... args) : type(Type::JSON_ARRAY)
		{
			data.arr = new vector<JsonValue>();
			data.arr->emplace_back(t);
			append(args...);
		}

		template<class T>
		void append(T t)
		{
			setType(Type::JSON_ARRAY);
			data.arr->emplace_back(t);
		}

		template<class T, class... Args>
		void append(T t, Args... args)
		{
			append(t);
			append(args...);
		}

		//初始化列表
		JsonValue(initializer_list<JsonValue> list) :type(Type::JSON_ARRAY)
		{
			data.arr = new vector<JsonValue>(list.size());
			size_t index = 0;
			for (auto it = list.begin(); it != list.end(); it++)
			{
				data.arr->operator[](index) = *it;
				index++;
			}
		}

		//复制构造函数和赋值操作符
		JsonValue(const JsonValue& other)
		{
			type = other.type;
			switch (type)
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
			switch (other.type)
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

		template<class T>
		typename enable_if<is_same<T, bool>::value, JsonValue&>::type operator=(T boo)
		{
			setType(Type::JSON_BOOL);
			data.b = boo;
			return *this;
		}

		template<class T>
		typename enable_if<is_integral<T>::value, JsonValue&>::type operator=(T i)
		{
			setType(Type::JSON_INTEGRAL);
			data.i = (long long)i;
			return *this;
		}

		template<class T>
		typename enable_if<is_floating_point<T>::value, JsonValue&>::type operator=(T d)
		{
			setType(Type::JSON_FLOAT);
			data.d = (double)d;
			return *this;
		}

		template<class T>
		typename enable_if<is_convertible<T, string>::value, JsonValue&>::type operator=(T s)
		{
			setType(Type::JSON_STRING);
			data.s = new string(s);
			return *this;
		}

		JsonValue& operator=(nullptr_t)
		{
			setType(Type::JSON_NULL);
			return *this;
		}

		static JsonValue make(Type t)
		{
			JsonValue json;
			json.setType(t);
			return json;
		}

		//析构函数
		~JsonValue()
		{
			switch (type)
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
		JsonValue& operator[](string s)
		{
			setType(Type::JSON_OBJECT);
			return ((this->data).dict)->operator[](s);
		}
		JsonValue& operator[](int index)
		{
			setType(Type::JSON_ARRAY);
			return ((this->data).arr)->operator[](index);
		}

		bool operator!=(const JsonValue& other)
		{
			return !(*this == other);
		}
		
		bool operator==(const JsonValue& other)
		{
			if (type != other.type)
				return false;
			else
			{
				switch (type)
				{
					case Type::JSON_NULL:
						return true;
					case Type::JSON_BOOL:
						return data.b == other.data.b;
					case Type::JSON_INTEGRAL:
						return data.i == other.data.i;
					case Type::JSON_FLOAT:
						return data.d == other.data.d;
					case Type::JSON_STRING:
						return *(data.s) == *(other.data.s);
					case Type::JSON_ARRAY:
					{
						if (data.arr->size() != other.data.arr->size())
							return false;
						else
						{
							auto i = data.arr->begin();
							auto j = other.data.arr->begin();
							for (; i != data.arr->end(), j != other.data.arr->end(); i++, j++)
							{
								if (*i != *j)
									return false;
							}
							return true;
						}
					}
					case Type::JSON_OBJECT:
					{
						if (data.arr->size() != other.data.arr->size())
							return false;
						else
						{
							auto i = data.arr->begin();
							auto j = other.data.arr->begin();
							for (; i != data.arr->end(), j != other.data.arr->end(); i++, j++)
							{
								if (*i != *j)
									return false;
							}
							return true;
						}
					}
				}
			}
		}

		string getString() const
		{
			if (type == Type::JSON_STRING)
				return*(data.s);
			else
				return "";
		}

		Type JSONType() const
		{
			return type;
		}

		string dumps()
		{
			return dump(0);
		}
		
		int getSize()
		{
			if (type == Type::JSON_ARRAY)
				return data.arr->size();
			else if (type == Type::JSON_OBJECT)
				return data.dict->size();
			else
			{
				cerr << "not a container" << endl;
				return -1;
			}
		}
	private:
		void setType(Type t)
		{
			if (t == type)
				return;

			clearData();
			type = t;
			switch (t)
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
			switch (type)
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

		string dump(int level)
		{
			switch (type)
			{
				case Type::JSON_NULL:
				{
					string res = "";
					for (int i = 0; i < level; i++)
						res += "	";
					return res + "null";
				}
				case Type::JSON_BOOL:
				{
					string res = "";
					for (int i = 0; i < level; i++)
						res += "	";
					return data.b ? res + "true" : res + "false";
				}
				case Type::JSON_INTEGRAL:
				{
					string res = "";
					for (int i = 0; i < level; i++)
						res += "	";
					return res + to_string(data.i);
				}
				case Type::JSON_FLOAT:
				{
					string res = "";
					for (int i = 0; i < level; i++)
						res += "	";
					return res + to_string(data.d);
				}
				case Type::JSON_STRING:
				{
					string res = "";
					for (int i = 0; i < level; i++)
						res += "	";
					return res + "\"" + escape(*(data.s)) + "\"";
				}
				case Type::JSON_ARRAY:
				{
					string res = "";
					for (int i = 0; i < level; i++)
						res += "	";
					res += "[\n";
					for (auto it = data.arr->begin(); it != data.arr->end(); it++)
					{
						res += (*it).dump(level + 1);
						res += ",\n";
					}
					res.pop_back();
					res.pop_back();
					res += "\n";
					for (int i = 0; i < level; i++)
						res += "	";
					res += ']';
					return res;
				}
				case Type::JSON_OBJECT:
				{
					string res = "";
					for (int i = 0; i < level; i++)
						res += "	";
					res += "{\n";
					for (auto it = data.dict->begin(); it != data.dict->end(); it++)
					{
						for (int i = 0; i < level + 1; i++)
							res += "	";
						res += "\"";
						res += escape((*it).first);
						res += "\"";
						res += " : \n";
						res += (*it).second.dump(level + 1);
						res += ",\n\n";
					}
					res.pop_back();
					res.pop_back();
					res.pop_back();
					res += "\n";
					for (int i = 0; i < level; i++)
						res += "	";
					res += '}';
					return res;
				}
				default:
					return "";
			}
		}

		string escape(const string& str)
		{
			string res = "";
			size_t index = 0;
			while (str[index] != '\0')
			{
				switch (str[index])
				{
					case '\"': res += "\\\""; break;
					case '\\': res += "\\\\"; break;
					case '\b': res += "\\b";  break;
					case '\f': res += "\\f";  break;
					case '\n': res += "\\n";  break;
					case '\r': res += "\\r";  break;
					case '\t': res += "\\t";  break;
					default: res += str[index]; break;
				}
				++index;
			}
			return res;
		}
	};

	ostream& operator<<(ostream&, JsonValue&);
	
	inline bool iswhitespace(char c)
	{
		return (c == ' ' || c == '\n' || c == '\r' || c == '\t');
	}

	inline bool isdigit(char c)
	{
		return (c >= '0' && c <= '9');
	}

	inline void pass_whitespace(const string& str, size_t& offset)
	{
		while (iswhitespace(str[offset]))
			++offset;
	}

	JsonParseStatus parse_number(JsonValue&, const string&, size_t&);

	JsonParseStatus parse_true(JsonValue&, const string&, size_t&);

	JsonParseStatus parse_false(JsonValue&, const string&, size_t&);

	JsonParseStatus parse_bool(JsonValue&, const string&, size_t&);

	JsonParseStatus parse_null(JsonValue&, const string&, size_t&);

	JsonParseStatus parse_string(JsonValue&, const string&, size_t&);

	JsonParseStatus parse(JsonValue&, const string&, size_t&);

	JsonParseStatus parse_object(JsonValue&, const string&, size_t&);

	JsonParseStatus parse_array(JsonValue&, const string&, size_t&);

	void loads(JsonValue&, const string&);
}

#endif