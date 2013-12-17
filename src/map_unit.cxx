#include "rpg2k/debug.hxx"
#include "rpg2k/map_unit.hxx"

#include <boost/format.hpp>


namespace rpg2k {
namespace model {

map_unit::map_unit()
    : base("", "LcfMapUnit"), id_(0)
{
  base::reset();
}
map_unit::map_unit(boost::filesystem::path const& p)
		: base(p, "LcfMapUnit"), id_(0)
{
  load();
}
map_unit::map_unit(boost::filesystem::path const& dir, unsigned const id)
		: base(dir, "LcfMapUnit"), id_(id)
{
  set_path(dir / (boost::format("Map%04d.lmu") % id_).str());
  check_exists();

  load();
}
map_unit::map_unit(picojson::value const& p)
    : base(p, "LcfMapUnit"), id_(0)
{
}

void map_unit::load_impl()
{
  rpg2k_assert(rpg2k::within<unsigned>(0, id_, MAP_UNIT_MAX+1));

  chip_id_[chip_set::LOWER] = (*this)[71].to_binary().to_vector<uint16_t>();
  chip_id_[chip_set::UPPER] = (*this)[72].to_binary().to_vector<uint16_t>();

  this->width  = (*this)[2];
  this->height = (*this)[3];
}

string map_unit::analyze_prefix() const
{
  return string("LcfMapUnit").append((boost::format(" %04d:") % int(id_)).str());
}

void map_unit::save_impl()
{
  (*this)[71].to_binary().assign(lower());
  (*this)[72].to_binary().assign(upper());

  (*this)[2] = this->width ;
  (*this)[3] = this->height;
}

int map_unit::chip_id_lower(unsigned const x, unsigned const y) const
{
  rpg2k_assert(rpg2k::within(x, this->width ));
  rpg2k_assert(rpg2k::within(y, this->height));

  return lower()[this->width*y + x];
}
int map_unit::chip_id_upper(unsigned const x, unsigned const y) const
{
  rpg2k_assert(rpg2k::within(x, this->width ));
  rpg2k_assert(rpg2k::within(y, this->height));

  return upper()[this->width*y + x];
}

} // namespace model
} // namespace rpg2k
