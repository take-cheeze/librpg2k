#include "rpg2k/debug.hxx"
#include "rpg2k/map_tree.hxx"


namespace rpg2k {
namespace model {

map_tree::map_tree(boost::filesystem::path const& p)
    : base(p, "LcfMapTree")
{
  load();
}
map_tree::map_tree(picojson::value const& p)
    : base(p, "LcfMapTree")
{
}

void map_tree::load_impl()
{
}
void map_tree::save_impl()
{
}

structure::array1d const& map_tree::operator [](unsigned map_id) const
{
  return data().front().to_array2d()[map_id];
}
structure::array1d& map_tree::operator [](unsigned map_id)
{
  return data().front().to_array2d()[map_id];
}

bool map_tree::exists(unsigned const map_id) const
{
  return data().front().to_array2d().exists(map_id);
}

bool map_tree::can_teleport(unsigned const map_id) const
{
  switch((*this)[map_id][31].to<int>()) {
    case 0: return can_teleport((*this)[map_id][2]);
    case 1: return true ;
    case 2: return false;
    default: rpg2k_assert(false); return false;
  }
}
bool map_tree::can_escape(unsigned const map_id) const
{
  switch((*this)[map_id][32].to<int>()) {
    case 0: return can_escape((*this)[map_id][2]);
    case 1: return true ;
    case 2: return false;
    default: rpg2k_assert(false); return false;
  }
}
bool map_tree::can_save(unsigned map_id) const
{
  switch((*this)[map_id][33].to<int>()) {
    case 0: return can_save((*this)[map_id][2]);
    case 1: return true ;
    case 2: return false;
    default: rpg2k_assert(false); return false;
  }
}

} // namespace model
} // namespace rpg2k
