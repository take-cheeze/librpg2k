#ifndef _INC_RPG2K__SAVE_DATA_HXX_
#define _INC_RPG2K__SAVE_DATA_HXX_

#include "model.hxx"


namespace rpg2k
{
	typedef std::vector<uint16_t> member_type;

	namespace model
	{
		class save_data : public base
		{
		public:
			struct item_type { uint8_t num; uint8_t use; };
			typedef std::map<uint16_t, item_type> item_table;
		private:
			unsigned id_;

			item_table item_;

			std::vector<int32_t> variable_;
			std::vector<uint8_t> flag_;

			member_type member_;

			typedef boost::array<boost::array<uint8_t, CHIP_REPLACE_MAX>, chip_set::END> chip_replace_type;
			chip_replace_type chip_replace_;

			unsigned current_event_id_;

			virtual void load_impl();
			virtual void save_impl();

			virtual char const* default_filename() const { return "Save00.lsd"; }
      virtual string analyze_prefix() const;
		public:
			save_data();
			save_data(boost::filesystem::path const& p);
			save_data(boost::filesystem::path const& dir, unsigned id);

			save_data const& operator =(save_data const& src);

			using base::operator [];

			unsigned id() const { return id_; }

			member_type const& member() const { return member_; }
			member_type& member() { return member_; }
			unsigned member(unsigned index) const;
			size_t member_num() const { return member_.size(); }
			bool add_member(unsigned char_id);
			bool remove_member(unsigned char_id);

		// items
			item_table const& item() const { return item_; }
			item_table& item() { return item_; }
			unsigned item_num(unsigned id) const;
			void set_item_num(unsigned id, unsigned val);
			void add_item_num(unsigned const id, int const val) { set_item_num(id, int(item_num(id)) + val); }
			unsigned item_use(unsigned id) const;
			void set_item_use(unsigned id, unsigned val);
		// flag and vals
			bool flag(unsigned chip_id) const;
			void set_flag(unsigned chip_id, bool data);
			int32_t var(unsigned chip_id) const;
			void    set_var(unsigned chip_id, int32_t data);

			int money() const;
			void set_money(int data);
			void add_money(int val) { set_money(money() + val); }

			unsigned timer_left() const { return 0; } // TODO

			unsigned replace(chip_set::type const type, unsigned const num) const { return chip_replace_[int(type)][num]; }
			void replace(chip_set::type type, unsigned dst_num, unsigned src_num);
			void reset_replace();

			structure::array1d& system() { return (*this)[101]; }
			structure::array1d const& system() const { return (*this)[101]; }

			structure::array2d& picture() { return (*this)[103]; }
			structure::array2d const& picture() const { return (*this)[103]; }

			void set_current_event_id(unsigned const id) { current_event_id_ = id; }
			unsigned current_event_id() const { return current_event_id_; }

			structure::event_state& party()
			{
				return reinterpret_cast<structure::event_state&>((*this)[104].to_array1d());
			}
			structure::event_state const& party() const
			{
				return reinterpret_cast<structure::event_state const&>((*this)[104].to_array1d());
			}

			structure::array2d& character() { return (*this)[108]; }
			structure::array2d const& character() const { return (*this)[108]; }

			structure::array2d& event_state() { return (*this)[111].to_array1d()[11]; }
			structure::array2d const& event_state() const { return (*this)[111].to_array1d()[11]; }
			structure::event_state& event_state(unsigned id);
		};
	} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__SAVE_DATA_HXX_
