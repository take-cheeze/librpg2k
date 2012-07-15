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
  } else {
    std::cout
        << "usage> to_json INPUT_FILE [OUTPUT_FILE]" << std::endl
        << std::endl
        << "- If OUTPUT_FILE is unspecified it will be outputed to stdout." << std::endl
        ;
    return EXIT_FAILURE;
  }

  if(!ost) {
    std::cout << "file open failed" << std::endl;
    return EXIT_FAILURE;
  }

  ost << rpg2k::model::load_lcf(argv[1])->to_json();

  return EXIT_SUCCESS;
}
