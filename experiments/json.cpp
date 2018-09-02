class JSONValue : public using_<JSONValue, double>,
          public using_<JSONValue, string>,
          public using_<JSONValue, vector<JSONValue>>,
          public using_<JSONValue, map<JSONValue, JSONValue>>
{
public:
  operator double &()
  {
    return get<double>(data);
  }

  operator double &()
  {
    return get<string>(data);
  }

  operator double &()
  {
    return get<vector<JSONValue>>(data);
  }

  operator double &()
  {
    return get<map<JSONValue, JSONValue>>(data);
  }

private:
  variant<
    double,
    string,
    vector<JSONValue>,
    map<JSONValue, JSONValue>
  > data;
};

int main()
{
  JSONValue json = {};

  json["asdf"] = 1.0;
  json[123456] = nullptr;
  json.DOT(qwerty) = "the other (third) operator dot proposal";
  json.qwerty = "__getattr__ for C++";
}
