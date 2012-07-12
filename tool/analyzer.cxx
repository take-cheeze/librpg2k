#include <rpg2k.hxx>
#include <cstdlib>
#include <iostream>


int main(int argc, char* argv[])
{
  if(argc == 2) {
    rpg2k::debug::ANALYZE_RESULT.close();
    static_cast<std::ostream&>(rpg2k::debug::ANALYZE_RESULT).rdbuf(std::cout.rdbuf());
  } else if(argc == 3) {
    rpg2k::debug::ANALYZE_RESULT.close();
    rpg2k::debug::ANALYZE_RESULT.open(argv[2], std::ios::out);
  } else { assert(false); }

  rpg2k::model::load_lcf(argv[1])->analyze();

  return EXIT_SUCCESS;
}
