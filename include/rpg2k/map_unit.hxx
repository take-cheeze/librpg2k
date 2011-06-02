#ifndef _INC_RPG2K__MAP_UNIT_HXX_
#define _INC_RPG2K__MAP_UNIT_HXX_

#include "model.hxx"

#include <boost/array.hpp>


namespace rpg2k {
	namespace model {
		class map_unit : public base
		{
		private:
			unsigned const id_;

			boost::array<std::vector<uint16_t>, chip_set::END> chip_id_;

			virtual void save_impl();
			virtual void load_impl();

			virtual char const* header() const { return "LcfMapUnit"; }
			virtual char const* default_filename() const { return "Map0000.lmu"; }
		public:
			map_unit();
			map_unit(system_string const& dir, system_string const& name);
			map_unit(system_string const& dir, unsigned id);
			virtual ~map_unit();


			unsigned id() const { return id_; }

			int chip_id_lower(unsigned x, unsigned y) const;
			int chip_id_upper(unsigned x, unsigned y) const;
			int chip_id_lower(ivec2 const& pos) const { return chip_id_lower(pos.x(), pos.y()); }
			int chip_id_upper(ivec2 const& pos) const { return chip_id_upper(pos.x(), pos.y()); }

			unsigned width, height;

			structure::array2d& event() { return (*this)[81]; }
			structure::array2d const& event() const { return (*this)[81]; }

			std::vector<uint16_t> const& lower() const
			{ return chip_id_[chip_set::LOWER]; }
			std::vector<uint16_t> const& upper() const
			{ return chip_id_[chip_set::UPPER]; }
		};
	} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__MAP_UNIT_HXX_
