struct Employee
{
  string name;
  int    salary;
  ...
};

struct Person
{
  string name;
};

auto v = vector<Employee>{...};

auto total_salary = v.sum(&auto::salary);

auto people = vector<variant<Employee, Person>>{...};

auto s = people.sorted(&auto::name);
