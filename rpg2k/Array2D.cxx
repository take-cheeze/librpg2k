#include "Array1D.hxx"
#include "Array2D.hxx"
#include "Debug.hxx"
#include "Element.hxx"
#include "Stream.hxx"


namespace rpg2k
{
	namespace structure
	{
		Array2D::Array2D(Array2D const& src)
		: arrayDefine_(src.arrayDefine_)
		, this_(src.this_)
		{
			for(const_iterator it = src.begin(); it != src.end(); ++it) {
				std::istringstream s( structure::serialize(*it->second), INPUT_FLAG );
				this->insert( it->first, std::auto_ptr<Array1D>( new Array1D(*this, it->first, s) ) );
			}
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
			std::istringstream s(b, INPUT_FLAG);

			if( isInvalidArray2D(b) ) { return; } // s.seek(PARTICULAR_DATA_SIZE);
			else { this->init(s); }
		}

		Array2D::Array2D(Element& e)
		: arrayDefine_( e.descriptor().arrayDefine() ), this_(&e)
		{
		}
		Array2D::Array2D(Element& e, std::istream& s)
		: arrayDefine_( e.descriptor().arrayDefine() ), this_(&e)
		{
			init(s);
		}
		Array2D::Array2D(Element& e, Binary const& b)
		: arrayDefine_( e.descriptor().arrayDefine() ), this_(&e)
		{
			std::istringstream s(b, INPUT_FLAG );

			if( isInvalidArray2D(b) ) return; // s.seek(PARTICULAR_DATA_SIZE);
			init(s);
		}
		void Array2D::init(std::istream& s)
		{
			size_t const length = readBER(s);
			for(size_t i = 0; i < length; i++) {
				size_t index = readBER(s);
				insert( index, std::auto_ptr<Array1D>( new Array1D(*this, index, s) ) );
			}

			if( toElement().hasOwner() ) rpg2k_analyze_assert( s.eof() );
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
			static unsigned const PARTICULAR_DATA_SIZE = 512;
		// check the data size
			if( b.size() < PARTICULAR_DATA_SIZE ) return false;
		// check the data inside Binary
			for(unsigned i = 0; i < PARTICULAR_DATA_SIZE; i++) if(b[i] != 0xff) return false;

			debug::Tracer::printBinary(b, clog);
		// return true if it is particular Array2D
			return true;
		}

		Element& Array2D::toElement() const
		{
			rpg2k_assert( isElement() );
			return *this_;
		}

		Array2D const& Array2D::operator =(Array2D const& src)
		{
			BaseOfArray2D::operator =(src);
			return *this;
		}

		Array1D& Array2D::operator [](unsigned const index)
		{
			iterator it = this->find(index);
			if( it != this->end() ) return *it->second;
			else {
				return *insert( index, std::auto_ptr<Array1D>(
					new Array1D(*this, index) ) )->second;
			}
		}
		Array1D const& Array2D::operator [](unsigned const index) const
		{
			const_iterator it = this->find(index);
			rpg2k_assert( it != this->end() );
			return *it->second;
		}

		unsigned Array2D::count() const
		{
			unsigned ret = 0;
			for(const_iterator it = begin(); it != end(); ++it) {
				if( it->second->exists() ) ret++;
			}
			return ret;
		}
		unsigned Array2D::serializedSize() const
		{
			unsigned ret = 0;

			ret += berSize( count() );
			for(const_iterator it = begin(); it != end(); ++it) {
				if( !it->second->exists() ) continue;

				ret += berSize( it->first );
				ret += it->second->serializedSize();
			}

			return ret;
		}
		std::ostream& Array2D::serialize(std::ostream& s) const
		{
			writeBER( s, count() );
			for(const_iterator it = begin(); it != end(); ++it) {
				if( !it->second->exists() ) continue;

				writeBER( s, it->first );
				it->second->serialize(s);
			}
			return s;
		}

		bool Array2D::exists(unsigned const index) const
		{
			const_iterator it = find(index);
			return( ( it != end() ) && it->second->exists() );
		}
		bool Array2D::exists(unsigned index1, unsigned index2) const
		{
			Array2D::const_iterator it1 = find(index1);
			if( ( it1 != end() ) && it1->second->exists() ) {
				Array1D::const_iterator it2 = it1->second->find(index2);
				return( ( it2 != it1->second->end() ) && it2->second->exists() );
			}
			return false;
		}
	} // namespace structure
} // namespace rpg2k
