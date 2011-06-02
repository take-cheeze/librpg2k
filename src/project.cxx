#include <algorithm>

#include "rpg2k/debug.hxx"
#include "rpg2k/project.hxx"
#include "rpg2k/stream.hxx"

#include <boost/foreach.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/count.hpp>
#include <boost/range/algorithm/fill.hpp>
#include <boost/range/algorithm/find.hpp>


namespace
{
	/*
	 * EXP caclulation routine from http://twitter.com/easyrpg
	 * (many rewrite by take-cheeze)
	 * The original code(writen with Pascal) is at
	 * http://code.google.com/p/turbu/source/browse/trunk/turbu/hero_data.pas
	 */
	int calc_exp(int const level, int const basic, int const increase, int const correction)
	{
		int result = 0;

		#if (RPG2KISPSP || RPG2KISIPHONE)
			float standard = basic, additional = 1.5f + (increase * 0.01f);
			BOOST_FOREACH(int const i, boost::irange(0, level - 1)) {
				(void)i;
				result += int(standard);
				standard *= additional;
				additional = (level * 0.002f + 0.8f) * (additional - 1.f) + 1.f;
			}
		#else
			double standard = basic, additional = 1.5 + (increase * 0.01);
			BOOST_FOREACH(int const i, boost::irange(0, level - 1)) {
				(void)i;
				result += int(standard);
				standard *= additional;
				additional = (level * 0.002 + 0.8) * (additional - 1.0) + 1.0;
			}
		#endif
		result += correction * (level - 1);

		return result;
		// TODO:
		// if (result < 1000000) return result;
		// else return 1000000;
	}

	using rpg2k::structure::array1d;
	using rpg2k::structure::array2d;
	using rpg2k::structure::event_state;
} // namespace

namespace rpg2k
{
	namespace model
	{
		project::project(system_string dir)
		: base_dir_(dir)
		, ldb(dir), lmt(dir)
		{
			init();
		}

		void project::init()
		{
		// lcf_save_data
			last_save_data_stamp_ = 0.0;
			last_save_data_id_ = ID_MIN;

			lsd_.push_back(new save_data()); // set lcf_save_data buffer
			BOOST_FOREACH(int const i, boost::irange(int(ID_MIN), int(SAVE_DATA_MAX))) {
				lsd_.push_back(new save_data(base_dir_, i));

				if(lsd_.back().exists()) {
					// TODO: caclating current time
					// Time Stamp Format: see http://support.microsoft.com/kb/210276
					double const cur = lsd_.back()[100].to_array1d()[1].to<double>();
					if(cur > last_save_data_stamp_) {
						last_save_data_id_ = i;
						last_save_data_stamp_ = cur;
					}
				}
			}

			new_game();
		}

		save_data& project::lsd(unsigned const id)
		{
			rpg2k_assert(rpg2k::within<unsigned>(ID_MIN, id, SAVE_DATA_MAX+1));
			return lsd_[id];
		}

		unsigned project::current_map_id()
		{
			return lsd().event_state(ID_PARTY).map_id();
		}

		map_unit& project::lmu(unsigned id)
		{
			map_unit_table::iterator const it = lmu_.find(id);
			if(it == lmu_.end()) {
				return *lmu_.insert(id, new map_unit(game_dir(), id)).first->second;
			} else return *it->second;
		}

		bool project::can_teleport()
		{
			return lsd()[101].to_array1d().exists(121)
				? lsd()[101].to_array1d()[121]
				: lmt.can_teleport(current_map_id());
		}
		bool project::can_escape()
		{
			return lsd()[101].to_array1d().exists(122)
				? lsd()[101].to_array1d()[122]
				: lmt.can_escape(current_map_id());
		}
		bool project::can_save()
		{
			return lsd()[101].to_array1d().exists(123)
				? lsd()[101].to_array1d()[123]
				: lmt.can_save(current_map_id());
		}
		bool project::can_open_menu() const
		{
			return lsd()[101].to_array1d().exists(124)
				? lsd()[101].to_array1d()[124]
				: true;
		}

		void project::load_lsd(unsigned const id)
		{
			rpg2k_assert(rpg2k::within<unsigned>(ID_MIN, id, SAVE_DATA_MAX+1));
			lsd() = lsd_[id];

			array2d const& chars_ldb = ldb.character();
			array2d& chars_lsd = lsd().character();
			char_table_.clear();
			for(array2d::const_iterator i = chars_ldb.begin(); i != chars_ldb.end(); ++i) {
				if(!i->second->exists()) continue;

				unsigned index = i->first;
				char_table_.insert(index, new character_type(i->first, *i->second, chars_lsd[i->first]));
			}
		}

		void project::save_lsd(unsigned const id)
		{
			rpg2k_assert(rpg2k::within<unsigned>(ID_MIN, id, SAVE_DATA_MAX+1));

		// set preview
			array1d& prev = lsd()[100];
			prev.clear();
			// reset the last time stamp and id
			last_save_data_stamp_ += 1.0; // TODO
			last_save_data_id_ = id;
			prev[1] = last_save_data_stamp_;
			{
				member_type const& mem = lsd().member();
				// set front character status
				if(!mem.empty()) {
					character_type const& c = character(mem.front());
					prev[11] = c.name();
					prev[12] = c.level();
					prev[13] = c.hp();
				}
				// set face_set
				BOOST_FOREACH(int const i, boost::irange(0, int(mem.size()))) {
					character_type const& c = character(mem[i]);
					prev[21 + 2*i] = c.face_set();
					prev[22 + 2*i] = c.face_set_pos();
				}
			}

			for(character_table::iterator i = char_table_.begin(); i != char_table_.end(); ++i) { i->second->sync(); }

			lsd_[id] = lsd();
			lsd_[id].save();
		}

		namespace
		{
			int chip_id2index(save_data& lsd, int const chip_id) // only for lower chip
			{
				int index;

				if(rpg2k::within(chip_id, 3000)) index = 0 + chip_id/1000;
				else if(chip_id == 3028) index = 3 + 0;
				else if(chip_id == 3078) index = 3 + 1;
				else if(chip_id == 3128) index = 3 + 2;
				else if(rpg2k::within(4000, chip_id, 5000)) index =  6 + (chip_id-4000)/50;
				else if(rpg2k::within(5000, chip_id, 5144)) index = 18 + lsd.replace(chip_set::LOWER, chip_id-5000);
				else rpg2k_assert(false);

				return index;
			}
			bool is_upper_chip(int const chip_id)
			{
				return rpg2k::within(10000, chip_id, 10144);
			}
			int to_upper_chip_index(save_data& lsd, int const chip_id)
			{
				return lsd.replace(chip_set::UPPER, chip_id-10000);
			}
		} // namespace
		bool project::is_below(int const chip_id)
		{
			return !is_above(chip_id);
		}
		bool project::is_above(int const chip_id)
		{
			int flag;
			if(is_upper_chip(chip_id)) {
				flag = ldb.upper_chip_flag(chip_set_id())[to_upper_chip_index(lsd(), chip_id)];
			} else {
				unsigned const index = chip_id2index(lsd(), chip_id);
				if(
					rpg2k::within(4000, chip_id, 5000) &&
					((ldb.lower_chip_flag(chip_set_id())[index] & 0x30) == 0x30)
				) switch((chip_id-4000) % 50) {
					case 0x14: case 0x15: case 0x16: case 0x17:
					case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
					case 0x2a: case 0x2b:
					case 0x2d: case 0x2e:
						return true;
					default:
						return false;
				}
				flag = ldb.lower_chip_flag(chip_set_id())[index];
			}
			return (flag & 0x30) == 0x10;
		}
		bool project::is_counter(int chip_id)
		{
			rpg2k_assert(is_upper_chip(chip_id));
			return (ldb.upper_chip_flag(chip_set_id())[chip_id-10000] & 0x40) != 0x00;
		}
		uint8_t project::pass(int chip_id)
		{
			if(is_upper_chip(chip_id)) {
				return ldb.upper_chip_flag(chip_set_id())[to_upper_chip_index(lsd(), chip_id)]; // & 0x0f;
			} else return ldb.lower_chip_flag(chip_set_id())[chip_id2index(lsd(), chip_id)]; // & 0x0f;
		}
		int project::terrain_id(int chip_id)
		{
			return ldb.terrain(chip_set_id())[chip_id2index(lsd(), chip_id)];
		}

		int project::current_page_id(unsigned const event_id)
		{
			array1d const* res = current_page(lmu()[81].to_array2d()[event_id][5]);
			return (res == NULL)? int(INVALID_ID) : int(res->index());
		}
		array1d const* project::current_page(array2d const& pages) const
		{
			for(array2d::const_iterator i = pages.begin(); i != pages.end(); ++i) {
				if(
					i->second->exists() &&
					valid_page_map((*i->second)[2])
				) return i->second;
			}

			return NULL;
		}

		bool project::equip(unsigned const char_id, unsigned const item_id)
		{
			int item_num = lsd().item_num(item_id);

			if(item_num == 0) return false;

			unsigned type = ldb.item()[item_id][3];
			rpg2k_assert(rpg2k::within(unsigned(item::WEAPON), unsigned(type), unsigned(item::ACCESSORY)+1));
			type -= int(item::WEAPON);

			character_type& c = this->character(char_id);
			c.equip[type] = item_id;

			lsd().set_item_num(item_id, --item_num);

			return true;
		}
		void project::unequip(unsigned const char_id, equip::type const type)
		{
			character_type::equip_type& equip = this->character(char_id).equip;

			lsd().set_item_num(equip[int(type)], lsd().item_num(equip[int(type)]) + 1);
			equip[int(type)] = 0;
		}

		string const& project::system_graphic() const
		{
			array1d const& sys = lsd()[101];

			return sys.exists(21)? sys[21] : ldb[22].to_array1d()[19];
		}
		wallpaper::type project::wallpaper_type() const
		{
			array1d const& sys = lsd()[101];

			return sys.exists(22)
				? wallpaper::type(sys[22].to<int>())
				: wallpaper::type(ldb[22].to_array1d()[71].to<int>());
		}
		face::type project::font_type() const
		{
			array1d const& sys = lsd()[101];

			return face::type(sys.exists(23)
				? sys[23].to<int>()
				: ldb[22].to_array1d()[72].to<int>());
		}

		void project::new_game()
		{
		// clear the save data buffer
			lsd().reset();
		// set party, boat, ship and airShip start point
			array1d const& start_lmt = lmt.start_point();
			array1d const& sys_ldb = ldb[22];
			BOOST_FOREACH(int const i, boost::irange(1, ID_AIRSHIP - ID_PARTY)) {
				event_state& dst = lsd().event_state(i + ID_PARTY);

				dst[11] = start_lmt[10*i + 1].to<int>();
				dst[12] = start_lmt[10*i + 2].to<int>();
				dst[13] = start_lmt[10*i + 3].to<int>();
				dst[21] = int(char_set_dir::DOWN);
				dst[22] = int(char_set_dir::DOWN);
				dst[73] = sys_ldb[10+i].to_string();
				if(sys_ldb[10+i + 3].exists()) dst[74] = sys_ldb[10+i + 3].to<int>();
			}
		// move to start point
			move(start_lmt[1], start_lmt[2], start_lmt[3]);
		// set character paramaeter
			array2d const& chars_ldb = ldb.character();
			array2d& chars_lsd = lsd().character();
			unsigned const condition_num = ldb.condition().rbegin()->first + 1;
			for(array2d::const_iterator i = chars_ldb.begin(); i != chars_ldb.end(); ++i) {
				if(!i->second->exists()) continue;

				array1d const& char_ldb = *i->second;
				array1d& char_lsd = chars_lsd[i->first];

				int const level = char_ldb[7].to<int>();
				char_lsd[31] = level; // level
				char_lsd[33] = 0; // hp_ fix
				char_lsd[34] = 0; // mp_ fix

				char_lsd[61] = char_ldb[51].to_binary(); // equip

				unsigned index = i->first;
				char_table_.insert(index, new character_type(i->first, char_ldb, char_lsd));
				character_type& c = this->character(i->first);

				char_lsd[32] = c.exp(level); // experience

				char_lsd[71] = c.basic_param(level, param::HP); // current hp_
				char_lsd[72] = c.basic_param(level, param::MP); // current mp_

				char_lsd[81] = condition_num;
				char_lsd[82].to_binary().assign(std::vector<uint16_t>(condition_num));
			}
		// set start member
			lsd().member() = sys_ldb[22].to_binary().to_vector<uint16_t>();
		// set party's char graphic
			if(!lsd().member().empty()) {
				character_type const& front_char = this->character(lsd().member().front());
				event_state& party = lsd().event_state(ID_PARTY);
				party[21] = int(char_set_dir::DOWN);
				party[22] = int(char_set_dir::DOWN);
				party[73] = front_char.char_set();
				party[74] = front_char.char_set_pos();
			}
		}

		void project::move(unsigned const map_id, int const x, int const y)
		{
			rpg2k_assert(map_id >= ID_MIN);
		// set party position
			event_state& party = lsd().event_state(ID_PARTY);
			party[11] = map_id; party[12] = x; party[13] = y;

			/*
			 * the new party position must be set before this
			 * because getLMU() returns the current party map_id's LMU
			 */
			map_unit const& prev_lmu = lmu();
		// clear changed values
			lsd()[111].to_array1d().clear();
		// reset chip replace
			lsd().reset_replace();
		// set map event position
			array2d const& map_event = prev_lmu[81];
			array2d& states = lsd().event_state();
			for(array2d::const_iterator i = map_event.begin(); i != map_event.end(); ++i) {
				if(!i->second->exists()) continue;

				array1d const* page = current_page((*i->second)[5]);
				if(page == NULL) continue;

				array1d const& src = *page;
				array1d& dst = states[i->first];

				dst[11] = 0; // map_id
				dst[12] = (*i->second)[2].to<int>(); // x
				dst[13] = (*i->second)[3].to<int>(); // y

				dst[21] = src[23].to<int>(); // direction
				dst[22] = src[23].to<int>();

				dst[73] = src[21].to_string(); // char_set
				dst[74] = src[22].to<int>(); // char_set_pos
				dst[75] = src[24].to<int>(); // char_setPat
			}
		}

		string project::panorama()
		{
			return
				lsd()[111].to_array1d().exists(32)
					? lsd()[111].to_array1d()[32]:
				lmu()[31].to<bool>()
					? lmu()[32]:
				string();
		}

		int project::chip_set_id()
		{
			return lsd()[111].to_array1d().exists(5)
				? lsd()[111].to_array1d()[5] : lmu()[1];
		}

		project::character_type::character_type(unsigned const char_id
		, structure::array1d const& ldb, structure::array1d& lsd)
		: char_id_(char_id), ldb_(ldb), lsd_(lsd)
		, basic_param_(ldb[31].to_binary().to_vector<uint16_t>())
		, skill_(lsd_[52].to_binary().to_set<uint16_t>())
		, condition_(lsd_[84].to_binary().to_vector<uint8_t>())
		, condition_step_(lsd_[82].to_binary().to_vector<uint16_t>())
		, equip(lsd_[61].to_binary().to_array<uint16_t, int(rpg2k::equip::END)>())
		{
		}
		void project::character_type::sync()
		{
			lsd_[51] = int(skill_.size());
			lsd_[52].to_binary().assign(skill_);

			lsd_[61].to_binary().assign(equip);

			lsd_[81] = int(condition_step_.size());
			lsd_[82].to_binary().assign(condition_step_);
			std::vector<uint16_t> const condition_clean(condition_.begin()
			, (++std::find(condition_.rbegin(), condition_.rend(), true)).base());
			lsd_[83] = int(condition_clean.size());
			lsd_[84].to_binary().assign(condition_clean);
		}

		bool project::has_item(unsigned const id) const
		{
			if(lsd().item().find(id) != lsd().item().end()) return true;
			else {
				member_type const& mem = lsd().member();
				BOOST_FOREACH(int const i, boost::irange(0, int(mem.size()))) {
					character_type::equip_type const& equip = this->character(mem[i]).equip;
					if(boost::find(equip, id) != equip.end()) return true;
				}
			}

			return false;
		}
		unsigned project::equip_num(unsigned const item_id) const
		{
			unsigned ret = 0;
			member_type const& mem = lsd().member();
			BOOST_FOREACH(int const i, boost::irange(0, int(mem.size()))) {
				ret += boost::count(this->character(mem[i]).equip, item_id);
			}
			return ret;
		}

		bool project::valid_page_map(structure::array1d const& term) const
		{
			int flags = term[1];

			member_type const& mem = lsd().member();
			return !(
				((flags & (0x01 << 0)) && !lsd().flag(term[2])) ||
				((flags & (0x01 << 1)) && !lsd().flag(term[3])) ||
				((flags & (0x01 << 2)) &&
					(lsd().var(term[4].to<int>()) < term[5].to<int>())) ||
				((flags & (0x01 << 3)) && !has_item(term[6])) ||
				((flags & (0x01 << 4)) &&
					(boost::find(mem, term[7].to<unsigned>()) == mem.end())) ||
				((flags & (0x01 << 5)) && (lsd().timer_left() > term[8].to<unsigned>())));
		}
		bool project::valid_page_battle(structure::array1d const& term) const
		{
			int flags = term[1];

			return (
				((flags & (0x01 << 0)) && !lsd().flag(term[2].to<unsigned>())) ||
				((flags & (0x01 << 1)) && !lsd().flag(term[3].to<unsigned>())) ||
				((flags & (0x01 << 2)) &&
					(lsd().var (term[4].to<unsigned>()) < (int)term[5].to<unsigned>())
				) /* ||
			// turns
				((flags & (0x01 << 3)) &&) ||
			// consume
				((flags & (0x01 << 4)) &&) ||
			// enemy
				((flags & (0x01 << 5)) &&) ||
			// party
				((flags & (0x01 << 6)) &&) ||
				*/
			) ? false : true;
		}

		int project::character_type::basic_param(int const level, param::type const t) const
		{
			rpg2k_assert(rpg2k::within(basic_param_.size() / int(param::END) * int(t) + level - 1, basic_param_.size()));
			return basic_param_[basic_param_.size() / int(param::END) * int(t) + level - 1];
		}
		int project::character_type::param(param::type const t) const
		{
			switch(t) {
				case param::HP: case param::MP:
					return basic_param(level(), t) + lsd_[33 + int(t)].to<int>();
				case param::ATTACK: case param::GAURD: case param::MIND: case param::SPEED:
					return basic_param(level(), t) + lsd_[41 + int(t) - int(param::ATTACK)].to<int>();
				default: return 0;
			}
		}
		int project::character_type::exp(unsigned const level) const
		{
			return calc_exp(level,
				ldb_[41].exists()? lsd_[41].to<int>() : EXP_DEFAULT,
				ldb_[42].exists()? lsd_[42].to<int>() : EXP_DEFAULT,
				ldb_[43]);
		}
		void project::character_type::set_level(unsigned const next_lv)
		{
			if(next_lv > rpg2k::LEVEL_MAX) { set_level(rpg2k::LEVEL_MAX); return; }

			unsigned const prev_lv = level();
			lsd_[31] = next_lv;
			array2d const& skillList = ldb_[63];
			unsigned current_lv = 1;
			if(prev_lv > next_lv) {
				for(array2d::const_iterator i = skillList.begin(); i != skillList.end(); ++i) {
					if((*i->second)[1].exists()) { current_lv = (*i->second)[1]; }

					if(current_lv < next_lv) { continue; }
					else if(prev_lv < current_lv) { break; }
					else {
						std::set<uint16_t>::iterator er = skill_.find((*i->second)[2].to<int>());
						if(er != skill_.end()) { skill_.erase(er); }
					}
				}
			} else if(next_lv > prev_lv) {
				for(array2d::const_iterator i = skillList.begin(); i != skillList.end(); ++i) {
					if((*i->second)[1].exists()) { current_lv = (*i->second)[1]; }

					if(current_lv < prev_lv) { continue; }
					else if(next_lv < current_lv) { break; }
					else { skill_.insert((*i->second)[2].to<int>()); }
				}
			}
		}
		bool project::character_type::set_exp(unsigned const val)
		{
			if(val > EXP_MAX) { return set_exp(EXP_MAX); }

			lsd_[32] = val;
			if(this->can_level_up()) {
				this->add_level();
				return true;
			} else {
				while(int(val) < this->level_exp()) { this->add_level(-1); }
				return false;
			}
		}
		void project::character_type::cure()
		{
			set_hp(param(param::HP));
			set_mp(param(param::MP));

			condition_.clear();
			boost::fill(condition_step_, 0);
		}

		project::character_type const& project::character(unsigned const id) const
		{
			character_table::const_iterator it = char_table_.find(id);
			rpg2k_assert(it != char_table_.end());
			return *it->second;
		}
		project::character_type& project::character(unsigned const id)
		{
			character_table::iterator it = char_table_.find(id);
			rpg2k_assert(it != char_table_.end());
			return *it->second;
		}

		int project::param_with_equip(unsigned char_id, param::type t) const
		{
			character_type const& c = this->character(char_id);
			int ret = c.param(t);
			BOOST_FOREACH(uint16_t const& i, c.equip) {
				ret += ldb.item()[i][11 + int(t) - 2].to<int>();
			}
			return ret;
		}
		bool project::process_action(unsigned const event_id, action::type const act, std::istream& s)
		{
			event_state& ev = lsd().event_state(event_id);

			switch(act) {
				case action::move::UP   : ev[13] = ev[13].to<int>() - 1; break;
				case action::move::RIGHT: ev[12] = ev[12].to<int>() + 1; break;
				case action::move::DOWN : ev[13] = ev[13].to<int>() + 1; break;
				case action::move::LEFT : ev[12] = ev[12].to<int>() - 1; break;
				case action::move::RIGHT_UP:
					ev[12] = ev[12].to<int>() + 1;
					ev[13] = ev[13].to<int>() - 1;
					break;
				case action::move::RIGHT_DOWN:
					ev[12] = ev[12].to<int>() - 1;
					ev[13] = ev[13].to<int>() + 1;
					break;
				case action::move::LEFT_DOWN:
					ev[12] = ev[12].to<int>() + 1;
					ev[13] = ev[13].to<int>() + 1;
					break;
				case action::move::LEFT_UP:
					ev[12] = ev[12].to<int>() + 1;
					ev[13] = ev[13].to<int>() - 1;
					break;
				case action::move::RANDOM:
					return process_action(event_id, random(action::move::UP, action::move::LEFT + 1), s);
				case action::move::TO_PARTY:
					break;
				case action::move::FROM_PARTY:
					break;
				case action::move::A_STEP:
					return process_action(event_id, int(action::move::UP) + int(ev.event_dir()), s);
				case action::face::UP   :
				case action::face::RIGHT:
				case action::face::DOWN :
				case action::face::LEFT :
					ev[21] = int(act - action::face::UP);
					ev[22] = int(act - action::face::UP);
					break;
				case action::turn::RIGHT_90:
					ev[21] = int(char_set_dir::type(int(ev.event_dir()) / int(char_set_dir::END)));
					break;
				case action::turn::LEFT_90:
					ev[21] = int(char_set_dir::type((int(ev.event_dir()) + int(char_set_dir::END) - 1) / int(char_set_dir::END)));
					break;
				case action::turn::OPPOSITE:
					ev[21] = int(int(ev.event_dir()) + 1 / int(char_set_dir::END));
					break;
				case action::turn::RIGHT_OR_LEFT_90:
					return process_action(event_id,
						random(2)? action::turn::LEFT_90 : action::turn::RIGHT_90, s);
				case action::turn::RANDOM:
					ev[21] = (random(4) + 1) * 2;
					break;
				case action::turn::TO_PARTY:
					break;
				case action::turn::OPPOSITE_OF_PARTY:
					break;
				case action::HALT:
					break;
				case action::BEGIN_JUMP:
					break;
				case action::END_JUMP:
					break;
				case action::FIX_DIR:
					break;
				case action::UNFIX_DIR:
					break;
				case action::SPEED_UP:
					break;
				case action::SPEED_DOWN:
					break;
				case action::FREQ_UP:
					break;
				case action::FREQ_DOWN:
					break;
				case action::SWITCH_ON:
					lsd().set_flag(stream::read_ber(s), true);
					break;
				case action::SWITCH_OFF:
					lsd().set_flag(stream::read_ber(s), false);
					break;
				case action::CHANGE_CHAR_SET: {
					string char_set(stream::read_ber(s), '\0');
					BOOST_FOREACH(char& i, char_set) { i = stream::read_ber(s); }
					ev[73] = char_set;
					ev[74] = stream::read_ber(s);
				} break;
				case action::PLAY_SOUND:
					break;
				case action::BEGIN_SLIP:
					break;
				case action::END_SLIP:
					break;
				case action::STOP_ANIME:
					break;
				case action::START_ANIME:
					break;
				case action::TRANS_UP:
					break;
				case action::TRANS_DOWN:
					break;
				default: rpg2k_assert(false);
			}

			return true;
		}

		std::vector<unsigned> project::sort_lsd() const
		{
			typedef std::map<double, unsigned> tmp_type;
			tmp_type tmp;
			BOOST_FOREACH(int const i, boost::irange(0, int(SAVE_DATA_MAX))) {
				tmp.insert(std::make_pair(i + 1, lsd_[i + 1][100].to_array1d()[1].to<double>()));
			}
			std::vector<unsigned> ret;
			BOOST_FOREACH(tmp_type::value_type const i, tmp) ret.push_back(i.first);

			return ret;
		}
	} // namespace model
} // namespace rpg2k
