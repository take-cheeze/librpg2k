#ifndef _INC_RPG2K__MAP_TREE_HXX_
#define _INC_RPG2K__MAP_TREE_HXX_

#include "model.hxx"

namespace rpg2k
{
	namespace model
	{
		class map_tree : public base
		{
		private:
			virtual void load_impl();
			virtual void save_impl();

			virtual char const* default_filename() const { return "RPG_RT.lmt"; }
		protected:
			using base::operator []; // since LMT's first element is array2d
		public:
			map_tree(boost::filesystem::path const& p);

			structure::array1d const& operator [](unsigned map_id) const;
			structure::array1d& operator [](unsigned map_id);

			bool exists(unsigned map_id) const;

			bool can_teleport(unsigned map_id) const;
			bool can_escape  (unsigned map_id) const;
			bool can_save    (unsigned map_id) const;

			structure::array1d const& start_point() const { return data().back(); }
		};
	} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__MAP_TREE_HXX_
