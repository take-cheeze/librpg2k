#include "rpg2k/Array1D.hxx"
#include "rpg2k/Array2D.hxx"
#include "rpg2k/Debug.hxx"
#include "rpg2k/Element.hxx"
#include "rpg2k/Stream.hxx"

#include <boost/range/irange.hpp>
#include <boost/range/algorithm/count_if.hpp>


namespace rpg2k
{
	namespace structure
	{
		Array2D::Array2D(Array2D const& src)
		: BaseOfArray2D(), arrayDefine_(src.arrayDefine_)
		, this_(src.this_)
		{
			Binary const b = structure::serialize(src);
			io::stream<io::array_source> s(io::array_source(reinterpret_cast<char const*>(b.data()), b.size()));
			this->init(s);
		}

		Array2D::Array2D(ArrayDefine info)
		: arrayDefine_(info), this_(NULL)
		{
		}
		Array2D::Array2D(ArrayDefine info, std::istream& s)
		: arrayDefine_(info), this_(NULL)
		{
			init(s);
		}
		Array2D::Array2D(ArrayDefine info, Binary const& b)
		: arrayDefine_(info), this_(NULL)
		{
			io::stream<io::array_source> s(io::array_source(reinterpret_cast<char const*>(b.data()), b.size()));

			if(isInvalidArray2D(b)) { return; } // s.seek(PARTICULAR_DATA_SIZE);
			else { this->init(s); }
		}

		Array2D::Array2D(Element& e)
		: arrayDefine_(e.descriptor().arrayDefine()), this_(&e)
		{
		}
		Array2D::Array2D(Element& e, std::istream& s)
		: arrayDefine_(e.descriptor().arrayDefine()), this_(&e)
		{
			init(s);
		}
		Array2D::Array2D(Element& e, Binary const& b)
		: arrayDefine_(e.descriptor().arrayDefine()), this_(&e)
		{
			io::stream<io::array_source> s(io::array_source(reinterpret_cast<char const*>(b.data()), b.size()));

			if(isInvalidArray2D(b)) return; // s.seek(PARTICULAR_DATA_SIZE);
			init(s);
		}
		void Array2D::init(std::istream& s)
		{
			for(auto const& i : boost::irange(0u, stream::readBER(s))) {
				(void)i;
				unsigned index = stream::readBER(s);
				this->insert(index, new Array1D(*this, index, s));
			}

			if(toElement().hasOwner()) rpg2k_analyze_assert(stream::isEOF(s));
		}

	/*
	 *  Checking if the input Binary is particular Array2D.
	 *  Particular Array2D is a Binary data that starts with 512 byte data of
	 * all value that is 0xff.
	 *  Notice: Particular Array2D's size is not 512 byte. I misunderstood it.
	 *  http://twitter.com/rgssws4m told me about this case.
	 */
		bool Array2D::isInvalidArray2D(Binary const& b)
		{
			enum { PARTICULAR_DATA_SIZE = 512 };
		// check the data size
			if(b.size() < PARTICULAR_DATA_SIZE) return false;
		// check the data inside Binary
			if(std::find_if(b.begin(), b.begin() + PARTICULAR_DATA_SIZE
							, [](uint8_t const i) { return i != 0xff; })
			   < (b.begin() + PARTICULAR_DATA_SIZE)) { return false; }

			debug::Tracer::printBinary(b, clog);
		// return true if it is particular Array2D
			return true;
		}

		Element& Array2D::toElement() const
		{
			rpg2k_assert(isElement());
			return *this_;
		}

		Array2D const& Array2D::operator =(Array2D const& src)
		{
			BaseOfArray2D::operator =(src);
			return *this;
		}

		Array1D& Array2D::operator [](unsigned index)
		{
			iterator it = this->find(index);
			if(it != this->end()) return *it->second;
			else {
				return *insert(index, new Array1D(*this, index))->second;
			}
		}
		Array1D const& Array2D::operator [](unsigned const index) const
		{
			const_iterator it = this->find(index);
			rpg2k_assert(it != this->end());
			return *it->second;
		}

		unsigned Array2D::count() const
		{
			return boost::count_if(*this
			, [](decltype(*this->begin()) const& i) { return i.second->exists(); });
		}
		size_t Array2D::serializedSize() const
		{
			unsigned ret = 0;

			ret += stream::berSize(count());
			for(auto const& i : *this) {
				if(!i->second->exists()) continue;

				ret += stream::berSize(i->first);
				ret += i->second->serializedSize();
			}

			return ret;
		}
		std::ostream& Array2D::serialize(std::ostream& s) const
		{
			stream::writeBER(s, count());
			for(auto const& i : *this) {
				if(!i->second->exists()) continue;

				stream::writeBER(s, i->first);
				i->second->serialize(s);
			}
			return s;
		}

		bool Array2D::exists(unsigned const index) const
		{
			const_iterator it = find(index);
			return((it != end()) && it->second->exists());
		}
		bool Array2D::exists(unsigned index1, unsigned index2) const
		{
			Array2D::const_iterator it1 = find(index1);
			if((it1 != end()) && it1->second->exists()) {
				Array1D::const_iterator it2 = it1->second->find(index2);
				return((it2 != it1->second->end()) && it2->second->exists());
			}
			return false;
		}
	} // namespace structure
} // namespace rpg2k
