#ifndef _INC__RPG2K__STRUCTURE_HPP
#define _INC__RPG2K__STRUCTURE_HPP

#include "Define.hxx"

#include <boost/array.hpp>

#include <climits>

#include <EASTL/fixed_vector.h>
#include <EASTL/set.h>
#include <EASTL/vector.h>


namespace rpg2k
{
	class Binary : public eastl::vector<uint8_t>
	{
	private:
		template<class SrcT>
		static void exchangeEndianIfNeed(uint8_t* dst, SrcT const& src)
		{
			for(typename SrcT::const_iterator srcIt = src.begin(); srcIt != src.end(); ++srcIt) {
				uint8_t const* srcCur = reinterpret_cast<uint8_t const*>( &(*srcIt) );
				for(unsigned i = 0; i < sizeof(typename SrcT::value_type); i++) {
				#if RPG2K_IS_BIG_ENDIAN
					*(dst++) = srcCur[sizeof(typename SrcT::value_type)-i-1];
				#elif RPG2K_IS_LITTLE_ENDIAN
					*(dst++) = srcCur[i];
				#else
					#error unsupported endian
				#endif
				}
			}
		}
		template<class DstT>
		static void exchangeEndianIfNeed(DstT& dst, uint8_t const* src)
		{
			for(typename DstT::iterator dstIt = dst.begin(); dstIt != dst.end(); ++dstIt) {
				uint8_t* dstCur = reinterpret_cast<uint8_t*>( &(*dstIt) );
				for(unsigned i = 0; i < sizeof(typename DstT::value_type); i++) {
				#if RPG2K_IS_BIG_ENDIAN
					dstCur[sizeof(typename SrcT::value_type)-i-1] = *(src++);
				#elif RPG2K_IS_LITTLE_ENDIAN
					dstCur[i] = *(src++);
				#else
					#error unsupported endian
				#endif
				}
			}
		}
	public:
		Binary() {}
		explicit Binary(unsigned size) : eastl::vector<uint8_t>(size) {}
		explicit Binary(uint8_t* data, unsigned size) : eastl::vector<uint8_t>(data, data + size) {}
		Binary(Binary const& b) : eastl::vector<uint8_t>(b) {}
		Binary(eastl::string const& str) : eastl::vector<uint8_t>(str.begin(), str.end()) {}

		operator eastl::string() const
		{
			return eastl::string(reinterpret_cast<char const*>(this->data()), this->size()); 
		}

		bool isBER() const;
		bool isString() const;
	// operator wrap of converter
		operator int   () const;
		operator bool  () const;
		operator double() const;
	// operator wrap of setter
		Binary& operator =(int    src);
		Binary& operator =(bool   src);
		Binary& operator =(double src);

		size_t serializedSize() const;
		std::ostream& serialize(std::ostream& s) const;

		template<typename T>
		eastl::vector<T> toVector() const
		{
			rpg2k_assert( ( this->size() % sizeof(T) ) == 0 );

			eastl::vector<T> output( this->size() / sizeof(T) );
			exchangeEndianIfNeed( output, this->data() );
			return output;
		}
		template<typename T, size_t S>
		boost::array<T, S> toArray() const
		{
			rpg2k_assert( ( this->size() % sizeof(T) ) == 0 );
			rpg2k_assert( ( this->size() / sizeof(T) ) == S );

			boost::array<T, S> output;
			exchangeEndianIfNeed( output, this->data() );
			return output;
		}
		template<typename T>
		eastl::set<T> toSet() const
		{
			eastl::vector<T> const v = this->toVector<T>();
			return eastl::set<T>( v.begin(), v.end() );
		}
		template<typename T, size_t S>
		eastl::fixed_vector<T, S> toFixedVector() const
		{
			eastl::vector<T> const v = this->toVector<T>();
			return eastl::fixed_vector<T, S>( v.begin(), v.end() );
		}

		template<class T>
		Binary& assign(T const& src)
		{
			this->resize( sizeof(typename T::value_type) * src.size() );
			exchangeEndianIfNeed( this->data(), src );
			return *this;
		}

		template<class T>
		Binary(T const& src) { assign(src); }
		template<class T>
		Binary& operator =(T const& src) { return assign(src); }
	}; // class Binary
} // namespace rpg2k

#endif // _INC__RPG2K__STRUCTURE_HPP
