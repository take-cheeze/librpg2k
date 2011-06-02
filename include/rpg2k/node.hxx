#ifndef _INC_RPG2K__NODE_HXX_
#define _INC_RPG2K__NODE_HXX_

#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/scoped_ptr.hpp>

#include "define.hxx"


namespace rpg2k
{
	class graphics2d;
	class node_switcher;

	class node
	{
		friend class node_switcher;
	public:
		virtual ~node() {}

		typedef int priority;

		void add_child(priority const& p, node& child);
		void remove(node& child);

		void update();
		void draw(graphics2d& g) const;

		typedef boost::bimap< boost::bimaps::multiset_of<priority>
		, boost::bimaps::unordered_set_of<node*> > children_type;
		boost::scoped_ptr<children_type> children;
	private:
		node* parent_;
		virtual void update_this() = 0;
		virtual void draw_this(graphics2d&) const {}
	}; // class node

	class node_switcher : public node
	{
	public:
		typedef string key;

		void add_child(key const& k, node& child);
		void remove(node& child);

		void switch_to(key const& k);
	private:
		typedef boost::bimap< boost::bimaps::set_of<key>
		, boost::bimaps::unordered_set_of<node*> > switch_list_type;
		switch_list_type switch_list_;
	}; // class node_switcher
} // namespace rpg2k

#endif // _INC_RPG2K__NODE_HXX_
