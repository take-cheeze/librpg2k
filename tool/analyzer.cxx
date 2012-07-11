#include <rpg2k.hxx>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
  assert(argc == 2);

  std::cout << "Analyzing: " << argv[1] << std::endl;
  rpg2k::model::load_lcf(argv[1])->analyze();
  std::cout << "Analyzing finished!" << std::endl;

  return EXIT_SUCCESS;
}
