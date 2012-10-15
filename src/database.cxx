#include "rpg2k/debug.hxx"
#include "rpg2k/database.hxx"

#include <boost/foreach.hpp>
#include <boost/range/irange.hpp>


namespace rpg2k {
	namespace model {
  database::database(boost::filesystem::path const& p)
      : base(p, "LcfDataBase")
		{
			load();
		}

		void database::load_impl()
		{
		// load chip infos
			structure::array2d const& chips = (*this)[20];
			for(structure::array2d::const_iterator i = chips.begin(); i != chips.end(); ++i) {
				terrain_.insert(std::make_pair(i->first, (*i->second)[3].to_binary().to_vector<uint16_t>()));

				std::vector<std::vector<uint8_t> >& dst = chip_flag_[i->first];
				dst.push_back((*i->second)[4].to<binary>());
				dst.push_back((*i->second)[5].to<binary>());
			}
		// copying vocabulary
			structure::array1d const& voc_src = (*this)[21];
			for(structure::array1d::const_iterator i = voc_src.begin(); i != voc_src.end(); ++i) {
				if(!i->second->exists()) continue;

				if(i->first >= vocabulary_.size()) {
					vocabulary_.resize(i->first + 1);
				}
				vocabulary_[i->first] = i->second->to_string();
			}
		}
		void database::save_impl()
		{
		// save chip info
			structure::array2d& chips = (*this)[20];
			for(structure::array2d::iterator i = chips.begin(); i != chips.end(); ++i) {
				if(!i->second->exists()) continue;

				(*i->second)[3].to_binary().assign(terrain(i->first));

				(*i->second)[4].to_binary().assign(upper_chip_flag(i->first));
				(*i->second)[5].to_binary().assign(lower_chip_flag(i->first));
			}
		// saving vocabulary
			structure::array1d& voc_dst = (*this)[21];
			BOOST_FOREACH(size_t const i, boost::irange(size_t(0), vocabulary_.size())) {
				if(vocabulary_[i].empty()) continue;
				voc_dst[i] = vocabulary_[i];
			}
		}

		std::vector<uint8_t> const& database::chip_flag(unsigned id, chip_set::type t) const
		{
			chip_flag_type::const_iterator it = chip_flag_.find(id);
			rpg2k_assert(it != chip_flag_.end());
			rpg2k_assert(rpg2k::within<unsigned>(int(t), it->second.size()));
			return it->second[int(t)];
		}
		std::vector<uint16_t> const& database::terrain(unsigned const id) const
		{
			terrain_type::const_iterator it = terrain_.find(id);
			rpg2k_assert(it != terrain_.end());
			return it->second;
		}

		string const& database::vocabulary(unsigned const index) const
		{
			rpg2k_assert(index < vocabulary_.size());
			// rpg2k_assert(!vocabulary_[index].empty());
			return vocabulary_[index];
		}
	} // namespace model
} // namespace rpg2k
