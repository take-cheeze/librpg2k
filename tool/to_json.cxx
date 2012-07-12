#include <rpg2k.hxx>
#include <cstdlib>
#include <fstream>
#include <picojson.h>


int main(int argc, char* argv[])
{
  std::ofstream ost;
  if(argc == 2) {
    static_cast<std::ostream&>(ost).rdbuf(std::cout.rdbuf());
  } else if(argc == 3) {
    ost.open(argv[2]);
  } else { assert(false); }

  ost << rpg2k::model::load_lcf(argv[1])->to_json();

  return EXIT_SUCCESS;
}
