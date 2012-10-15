#include <iterator>
#include <vector>

#include "rpg2k/array1d.hxx"
#include "rpg2k/array2d.hxx"
#include "rpg2k/debug.hxx"
#include "rpg2k/element.hxx"
#include "rpg2k/stream.hxx"


namespace rpg2k {
namespace structure {

array1d::array1d(array1d const& src)
		: base_of_array1d(), this_(src.this_)
		, owner_(src.owner_), index_(src.index_)
		, array_define(src.array_define)
{
  init();
  exists_ = src.exists_;

  for(const_iterator i = src.begin(); i != src.end(); ++i) {
    if(!i->second->exists()) continue;

    binary const bin = i->second->serialize();
    unsigned index = i->first;

    insert(index, new element(*this, i->first, bin));
  }
}
array1d::array1d(element& e)
		: this_(&e), owner_(NULL), index_(-1)
		, array_define(e.definition().array_define())
{
  init();
}
array1d::array1d(element& e, std::istream& s)
		: this_(&e), owner_(NULL), index_(-1)
		, array_define(e.definition().array_define())
{
  init(s);
}
array1d::array1d(element& e, binary const& b)
		: this_(&e), owner_(NULL), index_(-1)
		, array_define(e.definition().array_define())
{
  io::stream<io::array_source> s(b.source());
  init(s);
}
array1d::array1d(array2d& owner, unsigned index)
		: this_(NULL), owner_(&owner), index_(index)
		, array_define(owner.array_define)
{
  init();
}
array1d::array1d(array2d& owner, unsigned const index, std::istream& s)
		: this_(NULL), owner_(&owner), index_(index)
		, array_define(owner.array_define)
{
  init(s);
}

void array1d::init()
{
  /*
  unsigned idx = END_OF_ARRAY1D;
  insert(idx, new element());
  */

  exists_ = false;

  rpg2k_assert(&array_define);
}

void array1d::init(std::istream& s)
{
  /*
  unsigned idx = END_OF_ARRAY1D;
  insert(idx, new element());
  */

  exists_ = true;

  rpg2k_assert(&array_define);

  binary bin;

  while(true) {
    unsigned index = stream::read_ber(s);

    if(index == END_OF_ARRAY1D) break;

    stream::read_with_size(s, bin);
    insert(index, new element(*this, index, bin));

    if(!to_element().has_owner() && stream::is_eof(s)) return;
  }

  rpg2k_analyze_assert(is_array2d() || stream::is_eof(s));
}

bool array1d::is_element() const
{
  return (this_ != NULL) || (is_array2d() && owner_->is_element());
}

element& array1d::to_element()
{
  rpg2k_assert(is_element());

  if(is_array2d()) return owner_->to_element();
  else return *this_;
}
element const& array1d::to_element() const
{
  rpg2k_assert(is_element());

  if(is_array2d()) return owner_->to_element();
  else return *this_;
}

array1d const& array1d::operator =(array1d const& src)
{
  base_of_array1d::operator =(src);
  exists_ = src.exists_;

  return *this;
}

element& array1d::operator [](unsigned index)
{
  rpg2k_assert(index != END_OF_ARRAY1D);

  iterator it = find(index);
  return(it != end())
      ? *it->second
      : *this->insert(index, new element(*this, index)).first->second;
}
element const& array1d::operator [](unsigned const index) const
{
  return const_cast<array1d&>(*this)[index];
}
element& array1d::operator [](char const* index)
{
  array_table_type const& table = this->to_element().definition().array_table();
  array_table_type::const_iterator table_it = table.find(index);
  rpg2k_assert(table_it != table.end());
  unsigned index_no = table_it->second;

  iterator it = this->find(index_no);
  return(it != end())
      ? *it->second
      : *this->insert(index_no, new element(*this, index_no)).first->second;
}
element const& array1d::operator [](char const* index) const
{
  return const_cast<array1d&>(*this)[index];
}

unsigned array1d::count() const
{
  unsigned ret = 0;
  for(const_iterator i = begin(); i != end(); ++i) {
    if(i->second->exists()) ++ret;
  }
  return ret;
}
size_t array1d::serialized_size() const
{
  size_t ret = 0;

  for(const_iterator i = this->begin(); i != this->end(); ++i) {
    if(!i->second->exists()) continue;

    ret += stream::ber_size(i->first);
    size_t const size = i->second->serialized_size();
    ret += stream::ber_size(size);
    ret += size;
  }

  if(to_element().has_owner()) ret += stream::ber_size(END_OF_ARRAY1D);

  return ret;
}
std::ostream& array1d::serialize(std::ostream& s) const
{
  for(const_iterator i = begin(); i != end(); ++i) {
    if(!i->second->exists()) { continue; }

    stream::write_ber(s, i->second->index_of_array1d());
    stream::write_with_size(s, *i->second);
  }

  if(this->to_element().has_owner() || this->is_array2d()) {
    stream::write_ber(s, END_OF_ARRAY1D);
  }

  return s;
}

unsigned const& array1d::index() const { rpg2k_assert(is_array2d()); return index_; }
bool array1d::exists() const { rpg2k_assert(is_array2d()); return exists_; }
array2d& array1d::owner() { rpg2k_assert(is_array2d()); return *owner_; }
array2d const& array1d::owner() const { rpg2k_assert(is_array2d()); return *owner_; }

void array1d::substantiate()
{
  rpg2k_assert(is_array2d());

  to_element().substantiate();
  exists_ = true;
}
bool array1d::exists(unsigned index) const
{
  const_iterator it = find(index);
  return (it != end()) && it->second->exists();
}

} // namespace structure
} // namespace rpg2k
