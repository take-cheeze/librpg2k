#include "rpg2k/debug.hxx"
#include "rpg2k/save_data.hxx"
#include "rpg2k/structure.hxx"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/find.hpp>


namespace rpg2k
{
	namespace model
	{
		save_data::save_data()
        : base("", "LcfSaveData"), id_(-1)
		{
			base::reset();

      // reset map chip info
			reset_replace();
		}
		save_data::save_data(boost::filesystem::path const& p)
        : base(p, "LcfSaveData"), id_(0)
		{
			load();
		}
		save_data::save_data(boost::filesystem::path const& dir, unsigned const id)
        : base(dir, "LcfSaveData"), id_(id)
		{
			set_path(dir /(boost::format("Save%02d.lsd") % this->id_).str());
			check_exists();

			if(!exists()) return;

			load();
		}
    string save_data::analyze_prefix() const
		{
			return string("LcfSaveData").append((boost::format(" %02d:") % int(id_)).str());
		}

		save_data const& save_data::operator =(save_data const& src)
		{
			this->data() = src.data();

			this->item_ = src.item_;

			this->variable_ = src.variable_;
			this->flag_   = src.flag_  ;

			this->member_ = src.member_;

			this->chip_replace_ = src.chip_replace_;

			return *this;
		}

		void save_data::load_impl()
		{
			structure::array1d& sys    = (*this)[101];
			structure::array1d& status = (*this)[109];
			structure::array1d& event  = (*this)[111];

		// item
			{
				int item_type_num = status[11];
				std::vector<uint16_t> id  = status[12].to_binary().to_vector<uint16_t>();
				std::vector<uint8_t > num = status[13].to_binary().to_vector<uint8_t>();
				std::vector<uint8_t > use = status[14].to_binary().to_vector<uint8_t>();

				BOOST_FOREACH(int const i, boost::irange(0, item_type_num)) {
					if(!num[i]) continue;

					item_type info = { num[i], use[i] };
					item_.insert(std::make_pair(id[i], info));
				}
			}
		// switch and variable
			// flag_.resize(sys[31].to_int());
			flag_ = sys[32].to_binary().to_vector<uint8_t>();
			// variable_.resize(sys[33].to_int());
			variable_ = sys[34].to_binary().to_vector<int32_t>();
		// member
			member_.resize(status[1].to_int());
			member_ = status[2].to_binary().to_vector<uint16_t>();
		// chip replace
			BOOST_FOREACH(int const i, boost::irange(0, int(chip_set::END)))
				{ chip_replace_[i] = event[21 + i].to_binary().to_array<uint8_t, CHIP_REPLACE_MAX>(); }
		}

		void save_data::save_impl()
		{
			structure::array1d& status = (*this)[109];
			structure::array1d& sys = (*this)[101];

		// item
			{
				int item_num = item_.size();
				status[11] = item_num;

				std::vector<uint16_t> id (item_num);
				std::vector<uint8_t > num(item_num);
				std::vector<uint8_t > use(item_num);

				int i = 0;
				BOOST_FOREACH(item_table::value_type const p, item_) {
					id [i] = p.first;
					num[i] = p.second.num;
					use[i] = p.second.use;

					i++;
				}
				status[12].to_binary().assign(id);
				status[13].to_binary().assign(num);
				status[14].to_binary().assign(use);
			}
		// switch and variable
			sys[31] = int(flag_.size());
			sys[32].to_binary().assign(flag_);
			sys[33] = int(variable_.size());
			sys[34].to_binary().assign(variable_);
		// member
			(*this)[109].to_array1d()[1] = int(member_.size());
			(*this)[109].to_array1d()[2].to_binary().assign(member_);
		// chip replace
			BOOST_FOREACH(int const i, boost::irange(int(chip_set::BEGIN), int(chip_set::END)))
			{ (*this)[111].to_array1d()[21 + i].to_binary().assign(chip_replace_[i]); }
		}

		bool save_data::add_member(unsigned const char_id)
		{
			if((member_.size() > rpg2k::MEMBER_MAX)
			|| boost::find(member_, char_id) == member_.end()) return false;
			else {
				member_.push_back(char_id);
				return true;
			}
		}
		bool save_data::remove_member(unsigned const char_id)
		{
			std::vector<uint16_t>::iterator it = std::find(member_.begin(), member_.end(), char_id);
			if(it != member_.end()) {
				member_.erase(it);
				return true;
			} else return false;
		}

		bool save_data::flag(unsigned const id) const
		{
			return (id < flag_.size()) ? flag_[id - ID_MIN] : bool(FLAG_DEFAULT);
		}
		void save_data::set_flag(unsigned id, bool data)
		{
			if(id >= flag_.size()) flag_.resize(id, FLAG_DEFAULT);
			flag_[id - ID_MIN] = data;
		}

		int32_t save_data::var(unsigned const id) const
		{
			return (id < variable_.size()) ? variable_[id - ID_MIN] : VARIABLE_DEFAULT;
		}
		void save_data::set_var(unsigned const id, int32_t const data)
		{
			if(id >= variable_.size()) variable_.resize(id, int32_t(VARIABLE_DEFAULT));
			variable_[id - ID_MIN] = data;
		}

		int save_data::money() const
		{
			return (*this)[109].to_array1d()[21];
		}
		void save_data::set_money(int const data)
		{
			if(data < MONEY_MIN) (*this)[109].to_array1d()[21] = int(MONEY_MIN);
			else if(MONEY_MAX < data) (*this)[109].to_array1d()[21] = int(MONEY_MAX);
			else (*this)[109].to_array1d()[21] = data;
		}

		unsigned save_data::item_num(unsigned const id) const
		{
			item_table::const_iterator i = item_.find(id);
			return (i == item_.end())? unsigned(ITEM_MIN) : i->second.num;
		}
		void save_data::set_item_num(unsigned const id, unsigned const val)
		{
			unsigned const valid_val = std::min(unsigned(ITEM_MAX), val);

			if(item_.find(id) == item_.end()) {
				item_type const i = { uint8_t(valid_val), 0 };
				item_.insert(std::make_pair(id, i));
			} else item_[id].num = valid_val;

			if(valid_val == ITEM_MIN) item_.erase(id);
		}

		unsigned save_data::item_use(unsigned const id) const
		{
			item_table::const_iterator t = item_.find(id);
			return (t == item_.end()) ? unsigned(ITEM_MIN) : t->second.use;
		}
		void save_data::set_item_use(unsigned const id, unsigned const val)
		{
			item_table::iterator i = item_.find(id);
			if(i != item_.end()) i->second.use = val;
		}

		unsigned save_data::member(unsigned const index) const
		{
			rpg2k_assert(rpg2k::within<unsigned>(index, member_.size()));
			return member_[index];
		}

		structure::event_state& save_data::event_state(unsigned id)
		{
			switch(id) {
				case ID_PARTY: case ID_BOAT: case ID_SHIP: case ID_AIRSHIP:
					return reinterpret_cast<structure::event_state&>(
						(*this)[104 + (id-ID_PARTY)]);
				case ID_THIS: id = current_event_id(); // TODO
				default:
					return reinterpret_cast<structure::event_state&>(
						(*this)[111].to_array1d()[11].to_array2d()[id]);
			}
		}

		void save_data::replace(chip_set::type const type, unsigned const dst_no, unsigned const src_no)
		{
			rpg2k_assert(rpg2k::within<unsigned>(dst_no, CHIP_REPLACE_MAX));
			rpg2k_assert(rpg2k::within<unsigned>(src_no, CHIP_REPLACE_MAX));

			std::swap(chip_replace_[int(type)][src_no], chip_replace_[int(type)][dst_no]);
		}
		void save_data::reset_replace()
		{
			BOOST_FOREACH(chip_replace_type::value_type& i, chip_replace_) {
				boost::copy(boost::irange(0, int(CHIP_REPLACE_MAX)), i.begin());
			}
		}
	} // namespace model
} // namespace rpg2k
