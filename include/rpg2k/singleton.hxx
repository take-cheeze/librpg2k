#ifndef _INC_RPG2K__SINGLETON_
#define _INC_RPG2K__SINGLETON_

#include <boost/noncopyable.hpp>


namespace rpg2k
{
	template<class T>
	class Singleton : boost::noncopyable
	{
	public:
		static T& instance()
		{
			static T theInstance;
			return theInstance;
		}
	}; // class Singleton

	/*
	 * this class is basically same as rpg2k::Singleton
	 * but the return type of instance() will be const reference
	 */
	template<class T>
	class ConstSingleton : boost::noncopyable
	{
	public:
		static T const& instance()
		{
			static T const theInstance;
			return theInstance;
		}
	}; // class ConstSingleton
} // namespace rpg2k

#endif // _INC_RPG2K__SINGLETON_
