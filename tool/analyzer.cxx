#include <rpg2k.hxx>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
  assert(argc == 2 || argc == 3);

  if(argc == 3) {
    rpg2k::debug::ANALYZE_RESULT.close();
    rpg2k::debug::ANALYZE_RESULT.open(argv[2], std::ios::out);
  }

  std::cout << "Analyzing: " << argv[1] << std::endl;
  rpg2k::model::load_lcf(argv[1])->analyze();
  std::cout << "Analyzing finished!" << std::endl;

  return EXIT_SUCCESS;
}
