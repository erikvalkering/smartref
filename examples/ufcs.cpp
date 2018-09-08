/*********************************************/
/* Example: unified call syntax              */
/*********************************************/
#include <vector>
#include <iostream>

#include <smartref/smartref.h>

using namespace std;

namespace magic {

auto wand = [](auto x) { return x; };

} // namespace magic

using namespace std;

// Algorithms
auto sorted    = [](auto container)                 { return container; };
auto unique    = [](auto container)                 { return container; };
auto filtered  = [](auto container, auto predicate) { return container; };
auto transform = [](auto container, auto operation) { return container; };
auto sum       = [](auto container)                 { return 42;        };

// Functors
auto is_even = [](auto x) { return x % 2 == 0; };
auto squared = [](auto x) { return x * x;      };

auto $ = magic::wand;

int main()
{
  auto v = vector<int>{1, 2, 2, 1, 3, 2, 4, 5};
  // auto v = vector{1, 2, 2, 1, 3, 2, 4, 5};

  {
    // Variant 1a: single-line, read from inside outwards (continuously looking left *and* right)
    auto s = sum(transform(filtered(unique(sorted(v)), is_even), squared));
    cout << "Variant 1a: " << s << endl;
  }

  {
    // Variant 1b: multi-line, read from inside outwards (continuously looking left *and* right)
    auto s = sum(
               transform(
                 filtered(
                   unique(sorted(v)),
                   is_even
                 ),
                 squared
               )
             );

    cout << "Variant 1b: " << s << endl;
  }

  {
    // Variant 2a: single-line, read from left to right
    auto s = $(v).sorted().unique().filtered(is_even).transform(squared).sum();
    cout << "Variant 2a: " << s << endl;
  }

  {
    // Variant 2b: multi-line, read from top to bottom
    auto s = $(v).sorted()
                 .unique()
                 .filtered(is_even)
                 .transform(squared)
                 .sum();

    cout << "Variant 2b: " << s << endl;
  }
}
