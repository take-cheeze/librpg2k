#ifndef _INC_RPG2K__SINGLETON_HXX_
#define _INC_RPG2K__SINGLETON_HXX_

#include <boost/noncopyable.hpp>


namespace rpg2k
{
	template<class T>
	class singleton : boost::noncopyable
	{
	public:
		static T& instance()
		{
			static T the_instance;
			return the_instance;
		}
	}; // class singleton
} // namespace rpg2k

#endif // _INC_RPG2K__SINGLETON_HXX_
