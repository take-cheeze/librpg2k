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
  } else {
    std::cout
        << "usage> analyzer INPUT_FILE [OUTPUT_FILE]" << std::endl
        << std::endl
        << "- If OUTPUT_FILE is unspecified it will be outputed to stdout." << std::endl
        ;
    return EXIT_FAILURE;
  }

  if(!rpg2k::debug::ANALYZE_RESULT) {
    std::cout << "file open failed" << std::endl;
    return EXIT_FAILURE;
  }

  rpg2k::model::load(argv[1])->analyze();

  return EXIT_SUCCESS;
}
