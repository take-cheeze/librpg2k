#ifndef _INC_RPG2K__NODE_HXX_
#define _INC_RPG2K__NODE_HXX_

#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/scoped_ptr.hpp>

#include "Define.hxx"


namespace rpg2k
{
	class Graphics2D;
	class NodeSwitcher;

	class Node
	{
		friend class NodeSwitcher;
	public:
		virtual ~Node() {}

		typedef int Priority;

		void addChild(Priority const& p, Node& child);
		void remove(Node& child);

		void update();
		void draw(Graphics2D& g) const;
	protected:
		typedef boost::bimap< boost::bimaps::multiset_of<Priority>
		, boost::bimaps::unordered_set_of<Node*> > Children;
		boost::scoped_ptr<Children> const& children() const { return children_; }
		boost::scoped_ptr<Children>& children() { return children_; }
	private:
		Node* parent_;
		boost::scoped_ptr<Children> children_;
		virtual void updateThis() = 0;
		virtual void drawThis(Graphics2D& g) const {}
	}; // class BasicNode

	class NodeSwitcher : public Node
	{
	public:
		typedef String Key;

		void addChild(Key const& k, Node& child);
		void remove(Node& child);

		void switchTo(Key const& k);
	private:
		typedef boost::bimap< boost::bimaps::set_of<Key>
		, boost::bimaps::unordered_set_of<Node*> > SwitchList;
		SwitchList switchList_;
	}; // class BasicNodeSwitcher
} // namespace rpg2k

#endif // _INC_RPG2K__NODE_HXX_
