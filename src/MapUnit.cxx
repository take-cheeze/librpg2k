#include <boost/format.hpp>

#include "rpg2k/Debug.hxx"
#include "rpg2k/MapUnit.hxx"


namespace rpg2k
{
	namespace model
	{
		MapUnit::MapUnit()
		: Base("")
		{
			Base::reset();
		}
		MapUnit::MapUnit(SystemString const& dir, SystemString const& name)
		: Base(dir, name), id_(0)
		{
			load();
		}
		MapUnit::MapUnit(SystemString const& dir, unsigned const id)
		: Base(dir, SystemString()), id_(id)
		{
			setFileName((boost::format("Map%04d.lmu") % int(id)).str());

			checkExists();

			load();
		}
		void MapUnit::loadImpl()
		{
			rpg2k_assert(rpg2k::within<unsigned>(0, id_, MAP_UNIT_MAX+1));

			lower_ = (*this)[71].toBinary().toVector<uint16_t>();
			upper_ = (*this)[72].toBinary().toVector<uint16_t>();

			width_  = (*this)[2];
			height_ = (*this)[3];
		}

		MapUnit::~MapUnit()
		{
		#if RPG2K_DEBUG
			debug::ANALYZE_RESULT << header() << ":" << int(id_) << endl;
		#endif
		}

		void MapUnit::saveImpl()
		{
			(*this)[71].toBinary().assign(lower_);
			(*this)[72].toBinary().assign(upper_);

			(*this)[2] = width_ ;
			(*this)[3] = height_;
		}

		int MapUnit::chipIDLw(unsigned const x, unsigned const y) const
		{
			rpg2k_assert(rpg2k::within(x, width ()));
			rpg2k_assert(rpg2k::within(y, height()));

			return lower_[width()*y + x];
		}
		int MapUnit::chipIDUp(unsigned const x, unsigned const y) const
		{
			rpg2k_assert(rpg2k::within(x, width ()));
			rpg2k_assert(rpg2k::within(y, height()));

			return upper_[width()*y + x];
		}
	} // namespace model
} // namespace rpg2k
