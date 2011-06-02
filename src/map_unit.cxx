#include "rpg2k/debug.hxx"
#include "rpg2k/map_unit.hxx"

#include "ntfmt_string.hpp"


namespace rpg2k
{
	namespace model
	{
		map_unit::map_unit()
			: base(""), id_(0)
		{
			base::reset();
		}
		map_unit::map_unit(system_string const& dir, system_string const& name)
		: base(dir, name), id_(0)
		{
			load();
		}
		map_unit::map_unit(system_string const& dir, unsigned const id)
		: base(dir, system_string()), id_(id)
		{
			std::string filename;
			ntfmt::sink_string(filename)
				<< "Map" << ntfmt::fmt(id, "%04d") << ".lmu";
			set_filename(filename);

			check_exists();

			load();
		}
		void map_unit::load_impl()
		{
			rpg2k_assert(rpg2k::within<unsigned>(0, id_, MAP_UNIT_MAX+1));

			chip_id_[chip_set::LOWER] = (*this)[71].to_binary().to_vector<uint16_t>();
			chip_id_[chip_set::UPPER] = (*this)[72].to_binary().to_vector<uint16_t>();

			this->width  = (*this)[2];
			this->height = (*this)[3];
		}

		map_unit::~map_unit()
		{
#if RPG2K_DEBUG
			debug::ANALYZE_RESULT << header() << ":" << int(id_) << endl;
#endif
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
