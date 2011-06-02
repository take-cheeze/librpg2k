#include "rpg2k/node.hxx"

#include <boost/foreach.hpp>


namespace rpg2k
{
	void node::add_child(node::priority const& p, node& child) 
	{
		this->children->insert(children_type::value_type(p, &child));
		child.parent_ = this;
	}
	void node::remove(node& child)
	{
		this->children->right.erase(&child);
		child.parent_ = NULL;
	}

	void node::update()
	{
		this->update_this();

		if(children) BOOST_FOREACH(children_type::left_value_type& i, children->left) { i.second->update(); }
	}
	void node::draw(graphics2d& g) const
	{
		this->draw(g);

		if(children) BOOST_FOREACH(children_type::left_value_type& i, children->left) { i.second->draw(g); }
	}

	void node_switcher::add_child(node_switcher::key const& k, node& child) 
	{
		this->switch_list_.insert(switch_list_type::value_type(k, &child));
		child.parent_ = this;
	}
	void node_switcher::remove(node& child)
	{
		this->switch_list_.right.erase(&child);
		child.parent_ = NULL;
	}

	void node_switcher::switch_to(node_switcher::key const& k)
	{
		this->children->clear();
		switch_list_type::left_map::const_iterator it = this->switch_list_.left.find(k);
		rpg2k_assert(it != this->switch_list_.left.end());
		node::add_child(priority(), *(it->second));
	}
} // namespace rpg2k
