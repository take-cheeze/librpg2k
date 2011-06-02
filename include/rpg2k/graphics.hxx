#ifndef _INC_RPG2K__GRAPHICS_HXX_
#define _INC_RPG2K__GRAPHICS_HXX_

#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>
#include <memory>

#include "define.hxx"


namespace rpg2k
{
	class texture
	{
	public:
		texture(uintptr_t const data) : data_(data) {}
		virtual ~texture() {}

		template<typename T>
		T& to_ref() { return reinterpret_cast<T&>(this->data_); }
		template<typename T>
		T const& to_ref() const { return reinterpret_cast<T const&>(this->data_); }

		template<typename T>
		T to() const { return reinterpret_cast<T>(this->data_); }
	protected:
		uintptr_t data_;
	};

	typedef unique_ptr<texture>::type (*texture_loader)(system_string const& path);

	class material_pool
	{
	public:
		material_pool(system_string const& base);
	private:
		typedef boost::ptr_unordered_map<rpg2k::string, texture> pool;
		typedef boost::ptr_vector<pool> pool_list; pool_list pool_list_;
		typedef std::vector<texture_loader> loader_list; loader_list loader_list_;
	};
 
	class graphics2d
	{
	public:
		virtual ~graphics2d();

		virtual void draw(texture const& tex, vec2 const& dst_pos) = 0;
		virtual void draw(texture const& tex
		, vec2 const& src_pos, vec2 const& src_size, vec2 const& dst_pos) = 0;
		virtual void draw(texture const& tex
		, vec2 const& src_pos, vec2 const& src_size
		, vec2 const& dst_pos, vec2 const& dst_size) = 0;

		virtual void draw_center(texture const& tex, vec2 const& dst_pos) = 0;
	}; // class graphics2d
} // namespace rpg2k

#endif // _INC_RPG2K__GRAPHICS_HXX_
