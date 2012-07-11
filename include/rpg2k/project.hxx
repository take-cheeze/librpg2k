#ifndef _INC_RPG2K__PROJECT_HXX_
#define _INC_RPG2K__PROJECT_HXX_

#include "array1d_wrapper.hxx"
#include "database.hxx"
#include "map_tree.hxx"
#include "map_unit.hxx"
#include "save_data.hxx"

#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>

#include <set>


namespace rpg2k {
namespace model {

class project : boost::noncopyable {
 public:
  class character_type;
 private:
  boost::filesystem::path base_dir_, rtp_dir_;

  typedef boost::ptr_unordered_map<unsigned, map_unit> map_unit_table;
  map_unit_table lmu_;
  boost::ptr_vector<save_data> lsd_;

  typedef boost::ptr_unordered_map<unsigned, character_type> character_table;
  character_table char_table_;
  unsigned last_save_data_id_;
  double last_save_data_stamp_;
 protected:
  void init();
 public:
  project(boost::filesystem::path const& base_dir=".");

  string const& game_title() const { return lmt[0][1]; }

  boost::filesystem::path const& game_dir() const { return base_dir_; }

  unsigned current_map_id();

  database ldb;
  map_tree lmt;

  save_data const& lsd() const { return lsd_.front(); }
  save_data& lsd() { return lsd_.front(); }
  save_data& lsd(unsigned id);

  map_unit& lmu(unsigned id);
  map_unit& lmu() { return lmu(current_map_id()); }

  int last_lsd_id() const { return last_save_data_id_; }
  double last_lsd_stamp() const { return last_save_data_stamp_; }

  int chip_set_id();
  structure::array1d& chip_set() { return ldb.chip_set()[chip_set_id()]; }
  string panorama();

  void new_game();

  void load_lsd(unsigned id);
  void save_lsd(unsigned id);

  int param_with_equip(unsigned char_id, param::type t) const;
  bool   equip(unsigned char_id, unsigned item_id);
  void unequip(unsigned char_id, equip::type type);
  unsigned equip_num(unsigned item_id) const;

  bool valid_page_map   (structure::array1d const& term) const;
  bool valid_page_battle(structure::array1d const& term) const;

  bool can_teleport();
  bool can_escape();
  bool can_save();
  bool can_open_menu() const;

  bool is_above(int chip_id);
  bool is_below(int chip_id);
  bool is_counter(int chip_id);
  uint8_t pass(int chip_id);
  int terrain_id(int chip_id);

  bool process_action(unsigned event_id, action::type act, std::istream& is);

  int current_page_id(unsigned event_id);
  structure::array1d const* current_page(structure::array2d const& pages) const;

  string const& system_graphic() const;
  wallpaper::type wallpaper_type() const;
  face::type font_type() const;

  character_type const& character(unsigned const id) const;
  character_type& character(unsigned const id);

  void move(unsigned map_id, int x, int y);

  std::vector<unsigned> sort_lsd() const;

  bool has_item(unsigned id) const;
}; // class project

class project::character_type
{
 public:
  typedef boost::array<uint16_t, int(equip::END)> equip_type;
 private:
  unsigned const char_id_;

  structure::array1d const& ldb_;
  structure::array1d& lsd_;

  std::vector<uint16_t> const basic_param_;
  std::vector<uint16_t> condition_step_;

  template<typename T, unsigned LSD_ID, unsigned LDB_ID>
  T const& get() const
  { return lsd_.exists(LSD_ID)? lsd_[LSD_ID] : ldb_[LDB_ID]; }
 public:
  character_type(unsigned char_id
                 , structure::array1d const& ldb, structure::array1d& lsd);

  string const& char_set() const { return get<string, 11, 3>(); }
  string const& face_set() const { return get<string, 21, 15>(); }
  int char_set_pos() const { return get<int, 12, 4>(); }
  int face_set_pos() const { return get<int, 22, 16>(); }

  void set_char_set(string const& val) { lsd_[11] = val; }
  void set_face_set(string const& val) { lsd_[21] = val; }
  void set_char_set_pos(unsigned const val) { lsd_[12] = val; }
  void set_face_set_pos(unsigned const val) { lsd_[22] = val; }

  string const& name() const { return get<string, 1, 1>(); }
  string const& title() const { return get<string, 2, 2>(); }
  void set_name(string const& val) { lsd_[1] = val; }
  void set_title(string const& val) { lsd_[2] = val; }

  int job() const { return get<int, 90, 57>(); }

  int hp() const { return lsd_[71]; }
  int mp() const { return lsd_[72]; }
  void set_hp(unsigned const val) const { lsd_[71] = val; }
  void set_mp(unsigned const val) const { lsd_[72] = val; }

  int basic_param(int level, param::type t) const;
  int param(param::type t) const;

  int level() const { return get<int, 31, 7>(); }
  void set_level(unsigned val);
  void add_level(int const val = 1) { set_level(level() + val); }
  int exp(unsigned level) const;
  int exp() const { return lsd_[32]; }
  bool set_exp(unsigned val);
  bool add_exp(int const val) { return set_exp(exp() + val); }
  int next_level_exp() const { return exp(this->level() + 1); }
  int level_exp() const { return exp(this->level()); }

  void cure();

  equip_type equip;
  std::set<uint16_t> skill;
  std::vector<uint8_t> condition;
  
  void sync();
  unsigned index() const {
    rpg2k_assert(lsd_.index() == ldb_.index());
    return lsd_.index();
  }
 private:
  bool can_level_up() const { return(exp() > next_level_exp()); }
}; // class character_type

} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__PROJECT_HXX_
