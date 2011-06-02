#ifndef _INC_RPG2K__STRUCTURE_HXX_
#define _INC_RPG2K__STRUCTURE_HXX_

#include "define.hxx"

#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/range/irange.hpp>

#include <climits>

#include <set>
#include <vector>


namespace rpg2k
{
	class binary : public std::vector<uint8_t>
	{
	private:
		template<class SrcT>
		static void exchange_endian_if_need(uint8_t* dst, SrcT const& src)
		{
			BOOST_FOREACH(typename SrcT::value_type const& src_it, src) {
				uint8_t const* src_cur = reinterpret_cast<uint8_t const*>(&src_it);
				BOOST_FOREACH(int const& i, boost::irange(0, int(sizeof(typename SrcT::value_type)))) {
#if RPG2K_IS_BIG_ENDIAN
					*(dst++) = src_cur[sizeof(typename SrcT::value_type)-i-1];
#elif RPG2K_IS_LITTLE_ENDIAN
					*(dst++) = src_cur[i];
#else
#	error unsupported endian
#endif
				}
			}
		}
		template<class DstT>
		static void exchange_endian_if_need(DstT& dst, uint8_t const* src)
		{
			BOOST_FOREACH(typename DstT::value_type& dst_it, dst) {
				uint8_t* dst_cur = reinterpret_cast<uint8_t*>(&dst_it);
				BOOST_FOREACH(int const& i, boost::irange(0, int(sizeof(typename DstT::value_type)))) {
#if RPG2K_IS_BIG_ENDIAN
					dst_cur[sizeof(typename SrcT::value_type)-i-1] = *(src++);
#elif RPG2K_IS_LITTLE_ENDIAN
					dst_cur[i] = *(src++);
#else
#	error unsupported endian
#endif
				}
			}
		}
	public:
		binary() {}
		explicit binary(unsigned size) : std::vector<uint8_t>(size) {}
		explicit binary(uint8_t* data, unsigned size) : std::vector<uint8_t>(data, data + size) {}
		explicit binary(std::string const& str) : std::vector<uint8_t>(str.begin(), str.end()) {}

		operator std::string() const
		{
			return std::string(reinterpret_cast<char const*>(this->data()), this->size()); 
		}

		bool is_ber() const;
		bool is_string() const;
	// operator wrap of converter
		operator int   () const;
		operator bool  () const;
		operator double() const;
	// operator wrap of setter
		binary& operator =(int    src);
		binary& operator =(bool   src);
		binary& operator =(double src);

		size_t serialized_size() const;
		std::ostream& serialize(std::ostream& s) const;

		template<typename T>
		std::vector<T> to_vector() const
		{
			rpg2k_assert((this->size() % sizeof(T)) == 0);

			std::vector<T> output(this->size() / sizeof(T));
			exchange_endian_if_need(output, this->data());
			return output;
		}
		template<typename T, size_t S>
		boost::array<T, S> to_array() const
		{
			rpg2k_assert((this->size() % sizeof(T)) == 0);
			rpg2k_assert((this->size() / sizeof(T)) == S);

			boost::array<T, S> output;
			exchange_endian_if_need(output, this->data());
			return output;
		}
		template<typename T>
		std::set<T> to_set() const
		{
			std::vector<T> const v = this->to_vector<T>();
			return std::set<T>(v.begin(), v.end());
		}

		template<class T>
		binary& assign(T const& src)
		{
			this->resize(sizeof(typename T::value_type) * src.size());
			exchange_endian_if_need(this->data(), src);
			return *this;
		}

		template<class T>
		binary& operator =(T const& src) { return this->assign(src); }

		boost::iostreams::array_source source() const {
			return boost::iostreams::array_source(reinterpret_cast<char const*>(this->data()), this->size());
		}
		boost::iostreams::array_sink sink() {
			return boost::iostreams::array_sink(reinterpret_cast<char*>(this->data()), this->size());
		}
	}; // class binary
} // namespace rpg2k

#endif // _INC_RPG2K__STRUCTURE_HXX_
