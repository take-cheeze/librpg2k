#include "rpg2k/array1d.hxx"
#include "rpg2k/array2d.hxx"
#include "rpg2k/debug.hxx"
#include "rpg2k/element.hxx"
#include "rpg2k/stream.hxx"

#include <iterator>
#include <vector>

#include <boost/range/algorithm/count_if.hpp>
#include <boost/range/algorithm/remove_copy_if.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/typeof/typeof.hpp>


namespace rpg2k
{
	namespace structure
	{
		namespace
		{
			// doesn't extract if the Element of Array1D is bigger than the next value
			#if RPG2K_DEBUG // if debuging ALL Element will be extracted
				static size_t const BIG_DATA_SIZE = ~0;
			#else
				static size_t const BIG_DATA_SIZE = 512;
			#endif
		} // namespace

		Array1D::Array1D(Array1D const& src)
		: BaseOfArray1D()
		, arrayDefine_(src.arrayDefine_), this_(src.this_)
		, exists_(src.exists_), owner_(src.owner_), index_(src.index_)
		{
			for(const_iterator i = src.begin(); i != src.end(); ++i) {
				if(!i->second->exists()) continue;

				Binary const bin = i->second->serialize();
				unsigned index = i->first;

				this->insert(index, new Element(*this, i->first, bin));
			}
		}
		/*
		Array1D::Array1D(ArrayDefine info)
		: arrayDefine_(info), this_(NULL), owner_(NULL), index_(-1)
		{
			exists_ = false;
		}
		Array1D::Array1D(ArrayDefine info, std::istream& s)
		: arrayDefine_(info), this_(NULL), owner_(NULL), index_(-1)
		{
			init(s);
		}
		Array1D::Array1D(ArrayDefine info, std::istream& in, size_t const size)
		: arrayDefine_(info), this_(NULL), owner_(NULL), index_(-1)
		{
			io::stream<stream::istream_range_source>
				s(stream::istream_range_source(in, size));
			init(s);
		}
		Array1D::Array1D(ArrayDefine info, Binary const& b)
		: arrayDefine_(info), this_(NULL), owner_(NULL), index_(-1)
		{
			io::stream<io::array_source> s(b.source());
			init(s);
		}
		*/
		Array1D::Array1D(Element& e)
		: arrayDefine_(e.descriptor().arrayDefine()), this_(&e)
		, owner_(NULL), index_(-1)
		{
			exists_ = false;
		}
		Array1D::Array1D(Element& e, std::istream& s)
		: arrayDefine_(e.descriptor().arrayDefine()), this_(&e)
		, owner_(NULL), index_(-1)
		{
			init(s);
		}
		Array1D::Array1D(Element& e, std::istream& in, size_t const size)
		: arrayDefine_(e.descriptor().arrayDefine()), this_(&e)
		, owner_(NULL), index_(-1)
		{
			io::stream<stream::istream_range_source>
				s(stream::istream_range_source(in, size));
			init(s);
		}
		Array1D::Array1D(Element& e, Binary const& b)
		: arrayDefine_(e.descriptor().arrayDefine()), this_(&e)
		, owner_(NULL), index_(-1)
		{
			io::stream<io::array_source> s(b.source());
			init(s);
		}
		Array1D::Array1D(Array2D& owner, unsigned index)
		: arrayDefine_(owner.arrayDefine()), this_(NULL)
		, owner_(&owner), index_(index)
		{
			exists_ = false;
		}
		Array1D::Array1D(Array2D& owner, unsigned const index, std::istream& s)
		: arrayDefine_(owner.arrayDefine()), this_(NULL)
		, owner_(&owner), index_(index)
		{
			init(s);
		}
		void Array1D::init(std::istream& s)
		{
			exists_ = true;

			Binary bin;

			while(true) {
				unsigned index = stream::readBER(s);

				if(index == ARRAY_1D_END) break;

				// stream::readWithSize(s, bin);
				// insert(index, new Element(*this, index, bin));

				size_t const size = stream::readBER(s);
				std::streampos const pos = size + s.tellg();
				insert(index, new Element(*this, index, s, size));
				s.seekg(pos);

				if(!toElement().hasOwner() && stream::isEOF(s)) return;
			}

			rpg2k_analyze_assert(isArray2D() || stream::isEOF(s));
		}

		bool Array1D::isElement() const
		{
			return (this_ != NULL) || (isArray2D() && owner_->isElement());
		}

		Element& Array1D::toElement() const
		{
			rpg2k_assert(isElement());

			if(isArray2D()) return owner_->toElement();
			else return *this_;
		}

		Array1D const& Array1D::operator =(Array1D const& src)
		{
			BaseOfArray1D::operator =(src);
			exists_ = src.exists_;

			return *this;
		}

		Element& Array1D::operator [](unsigned index)
		{
			iterator it = find(index);
			return(it != end())
				? *it->second
				: *this->insert(index, new Element(*this, index)).first->second;
		}
		Element const& Array1D::operator [](unsigned const index) const
		{
			return const_cast<Array1D&>(*this)[index];
		}
		Element& Array1D::operator [](char const* index)
		{
			Descriptor::ArrayTable const& table = this->toElement().descriptor().arrayTable();
			Descriptor::ArrayTable::const_iterator tableIt = table.find(index);
			rpg2k_assert(tableIt != table.end());
			unsigned indexNo = tableIt->second;

			iterator it = this->find(indexNo);
			return(it != end())
				? *it->second
				: *this->insert(indexNo, new Element(*this, indexNo)).first->second;
		}
		Element const& Array1D::operator [](char const* index) const
		{
			return const_cast<Array1D&>(*this)[index];
		}

		unsigned Array1D::count() const
		{
			unsigned ret = 0;
			for(const_iterator i = begin(); i != end(); ++i) {
				if(i->second->exists()) ++ret;
			}
			return ret;
		}
		size_t Array1D::serializedSize() const
		{
			size_t ret = 0;

			for(const_iterator i = this->begin(); i != this->end(); ++i) {
				if(!i->second->exists()) continue;

				ret += stream::berSize(i->first);
				size_t const size = i->second->serializedSize();
				ret += stream::berSize(size);
				ret += size;
			}

			if(toElement().hasOwner()) ret += stream::berSize(ARRAY_1D_END);

			return ret;
		}
		std::ostream& Array1D::serialize(std::ostream& s) const
		{
			typedef BOOST_TYPEOF(*this->begin()) const_value_type;
			std::vector<Element const*> result;
			for(const_iterator i = begin(); i != end(); ++i) {
				if(i->second->exists()) result.push_back(i->second);
			}
			boost::sort(result, sort_function);
			BOOST_FOREACH(Element const* const i, result) {
				stream::writeBER(s, i->indexOfArray1D());
				stream::writeWithSize(s, *i);
			}

			if(this->toElement().hasOwner() || this->isArray2D()) {
				stream::writeBER(s, ARRAY_1D_END);
			}

			return s;
		}

		unsigned const& Array1D::index() const { rpg2k_assert(isArray2D()); return index_; }
		bool Array1D::exists() const { rpg2k_assert(isArray2D()); return exists_; }
		Array2D& Array1D::owner() { rpg2k_assert(isArray2D()); return *owner_; }
		Array2D const& Array1D::owner() const { rpg2k_assert(isArray2D()); return *owner_; }

		void Array1D::substantiate()
		{
			rpg2k_assert(isArray2D());

			toElement().substantiate();
			exists_ = true;
		}
		bool Array1D::exists(unsigned index) const
		{
			const_iterator it = find(index);
			return (it != end()) && it->second->exists();
		}

		bool Array1D::sort_function(Element const* l, Element const* r)
		{
			return l->indexOfArray1D() < r->indexOfArray1D();
		}
	} // namespace structure
} // namespace rpg2k
