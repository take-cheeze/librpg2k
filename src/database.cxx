#include "rpg2k/debug.hxx"
#include "rpg2k/database.hxx"

#include <boost/foreach.hpp>
#include <boost/range/irange.hpp>


namespace rpg2k
{
	namespace model
	{
		DataBase::DataBase(SystemString const& dir)
		: Base(dir)
		{
			load();
		}
		DataBase::DataBase(SystemString const& dir, SystemString const& name)
		: Base(dir, name)
		{
			load();
		}
		DataBase::~DataBase()
		{
#if RPG2K_DEBUG
			debug::ANALYZE_RESULT << header() << ":" << endl;
#endif
		}

		void DataBase::loadImpl()
		{
		// load chip infos
			structure::Array2D const& chips = (*this)[20];
			for(structure::Array2D::const_iterator i = chips.begin(); i != chips.end(); ++i) {
				terrain_.insert(std::make_pair(i->first, (*i->second)[3].toBinary().toVector<uint16_t>()));

				std::vector<std::vector<uint8_t> >& dst = chipFlag_[i->first];
				dst.push_back((*i->second)[4].to<Binary>());
				dst.push_back((*i->second)[5].to<Binary>());
			}
		// copying vocabulary
			structure::Array1D const& vocSrc = (*this)[21];
			for(structure::Array1D::const_iterator i = vocSrc.begin(); i != vocSrc.end(); ++i) {
				if(i->first >= vocabulary_.size()) {
					vocabulary_.resize(i->first + 1);
				}
				vocabulary_[i->first] = i->second->toString();
			}
		}
		void DataBase::saveImpl()
		{
		// save chip info
			structure::Array2D& chips = (*this)[20];
			for(structure::Array2D::iterator i = chips.begin(); i != chips.end(); ++i) {
				if(!i->second->exists()) continue;

				(*i->second)[3].toBinary().assign(terrain(i->first));

				(*i->second)[4].toBinary().assign(upperChipFlag(i->first));
				(*i->second)[5].toBinary().assign(lowerChipFlag(i->first));
			}
		// saving vocabulary
			structure::Array1D& vocDst = (*this)[21];
			BOOST_FOREACH(size_t const i, boost::irange(size_t(0), vocabulary_.size())) {
				if(vocabulary_[i].empty()) continue;
				vocDst[i] = vocabulary_[i];
			}
		}

		std::vector<uint8_t> const& DataBase::chipFlag(unsigned id, ChipSet::type t) const
		{
			ChipFlag::const_iterator it = chipFlag_.find(id);
			rpg2k_assert(it != chipFlag_.end());
			rpg2k_assert(rpg2k::within<unsigned>(int(t), it->second.size()));
			return it->second[int(t)];
		}
		std::vector<uint16_t> const& DataBase::terrain(unsigned const id) const
		{
			Terrain::const_iterator it = terrain_.find(id);
			rpg2k_assert(it != terrain_.end());
			return it->second;
		}

		String const& DataBase::vocabulary(unsigned const index) const
		{
			rpg2k_assert(index < vocabulary_.size());
			// rpg2k_assert(!vocabulary_[index].empty());
			return vocabulary_[index];
		}
	} // namespace model
} // namespace rpg2k
