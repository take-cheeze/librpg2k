#include "Node.hxx"

namespace rpg2k
{
	void Node::addChild(Priority const& p, Node& child) 
	{
		this->children_->insert(Children::value_type(p, &child));
		child.parent_ = this;
	}
	void Node::remove(Node& child)
	{
		this->children_->right.erase(&child);
		child.parent_ = NULL;
	}

	void Node::update()
	{
		this->updateThis();

		if(children_)
		for(Children::left_map::iterator i = children_->left.begin(); i != children_->left.end(); ++i) {
			i->second->update();
		}
	}
	void Node::draw(Graphics2D& g) const
	{
		this->draw(g);

		if(children_)
		for(Children::left_map::const_iterator i = children_->left.begin(); i != children_->left.end(); ++i) {
			i->second->draw(g);
		}
	}

	void NodeSwitcher::addChild(Key const& k, Node& child) 
	{
		this->switchList_.insert(SwitchList::value_type(k, &child));
		child.parent_ = this;
	}
	void NodeSwitcher::remove(Node& child)
	{
		this->switchList_.right.erase(&child);
		child.parent_ = NULL;
	}

	void NodeSwitcher::switchTo(Key const& k)
	{
		this->children()->clear();
		SwitchList::left_map::const_iterator it = this->switchList_.left.find(k);
		rpg2k_assert(it != this->switchList_.left.end());
		Node::addChild(Priority(), *(it->second));
	}
} // namespace rpg2k
