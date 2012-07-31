#include <rpg2k.hxx>
#include <cstdlib>
#include <fstream>
#include <picojson.h>


int main(int argc, char* argv[])
{
  std::ifstream ist;
  std::ofstream ost;
  if(argc == 3) {
    ist.open(argv[1]);
    ost.open(argv[2], rpg2k::stream::OUTPUT_FLAG);
  } else {
    std::cout
        << "usage> lcf2json INPUT_FILE OUTPUT_FILE" << std::endl
        ;
    return EXIT_FAILURE;
  }

  if(!ost || !ist) {
    std::cout << "file open failed" << std::endl;
    return EXIT_FAILURE;
  }

  rpg2k::model::load(argv[1])->serialize(ost);

  return EXIT_SUCCESS;
}
