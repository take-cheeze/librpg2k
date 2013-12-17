#ifndef _INC_RPG2K__DATABASE_HXX_
#define _INC_RPG2K__DATABASE_HXX_

#include "model.hxx"

#include <map>
#include <vector>


namespace rpg2k {
namespace model {
class database : public base
{
 private:
  typedef std::map<unsigned, std::vector<uint16_t> > terrain_type;
  terrain_type terrain_;
  typedef std::map<unsigned, std::vector<std::vector<uint8_t> > > chip_flag_type;
  chip_flag_type chip_flag_;

  std::vector<string> vocabulary_;

  void load_impl();
  void save_impl();

  char const* default_filename() const { return "RPG_RT.ldb"; }
 public:
  database(boost::filesystem::path const& p);
  database(picojson::value const& p);

  structure::array2d& character() { return (*this)[11]; }
  structure::array2d& skill() { return (*this)[12]; }
  structure::array2d& item() { return (*this)[13]; }
  structure::array2d& enemy() { return (*this)[14]; }
  structure::array2d& enemy_group() { return (*this)[15]; }
  structure::array2d& terrain() { return (*this)[16]; }
  structure::array2d& attribute() { return (*this)[17]; }
  structure::array2d& condition() { return (*this)[18]; }
  structure::array2d& battle_anime() { return (*this)[19]; }
  structure::array2d& chip_set() { return (*this)[20]; }
  structure::array1d& system() { return (*this)[22]; }
  structure::array2d& flag() { return (*this)[23]; }
  structure::array2d& variable() { return (*this)[24]; }
  structure::array2d& common_event() { return (*this)[25]; }

  structure::array2d const& character() const { return (*this)[11]; }
  structure::array2d const& skill() const { return (*this)[12]; }
  structure::array2d const& item() const { return (*this)[13]; }
  structure::array2d const& enemy() const { return (*this)[14]; }
  structure::array2d const& enemy_group() const { return (*this)[15]; }
  structure::array2d const& terrain() const { return (*this)[16]; }
  structure::array2d const& attribute() const { return (*this)[17]; }
  structure::array2d const& condition() const { return (*this)[18]; }
  structure::array2d const& battle_anime() const { return (*this)[19]; }
  structure::array2d const& chip_set() const { return (*this)[20]; }
  structure::array1d const& system() const { return (*this)[22]; }
  structure::array2d const& flag() const { return (*this)[23]; }
  structure::array2d const& variable() const { return (*this)[24]; }
  structure::array2d const& common_event() const { return (*this)[25]; }

  string const& vocabulary(unsigned index) const;

  std::vector<uint16_t> const& terrain(unsigned id) const;
  std::vector<uint8_t> const& chip_flag(unsigned id, chip_set::type t) const;
  std::vector<uint8_t> const& lower_chip_flag(unsigned id) const
  {
    return chip_flag(id, chip_set::LOWER);
  }
  std::vector<uint8_t> const& upper_chip_flag(unsigned id) const
  {
    return chip_flag(id, chip_set::UPPER);
  }
}; // class database
} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__DATABASE_HXX_
