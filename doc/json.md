# Compare usage of json libraries
Let's parse below string to dom!
```json
{
	"project": "libraries",
	"stars": 10
}
```

## ViST
```cpp
	std::string raw = "{\"project\":\"vist\",\"stars\":1000}";

	Json json = Json::Parse(raw);
	int stars = json["stars"];
	json["stars"] = stars + 1;

	// {"project":"vist","stars":1001}
	std::cout << json.serialize() << std::endl;
```

## [RapidJson](https://github.com/Tencent/rapidjson/blob/master/example/simpledom/simpledom.cpp)
```cpp
	const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
	Document d;
	d.Parse(json);

	Value& s = d["stars"];
	s.SetInt(s.GetInt() + 1);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);

	// {"project":"rapidjson","stars":11}
	std::cout << buffer.GetString() << std::endl;
```

## [JsonCPP](https://github.com/open-source-parsers/jsoncpp/blob/master/example/readFromString/readFromString.cpp)
```cpp
	const std::string rawJson = R"({"stars": 20, "project": "jsoncpp"})";
	const auto rawJsonLength = static_cast<int>(rawJson.length());

	JSONCPP_STRING err;
	Json::Value root;

	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root,
					   &err)) {
	  std::cout << "error" << std::endl;
	  return EXIT_FAILURE;
	}

	std::cout << root["project"].asString() << std::endl;
	std::cout << root["stars"].asInt() << std::endl;
```
