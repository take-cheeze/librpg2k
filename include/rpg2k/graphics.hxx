#ifndef _INC_RPG2K__GRAPHICS_HXX_
#define _INC_RPG2K__GRAPHICS_HXX_

#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>
#include <memory>

#include "define.hxx"


namespace rpg2k
{
	class Rect;

	class Texture
	{
	public:
		Texture(uintptr_t const data) : data_(data) {}
		virtual ~Texture() {}

		template<typename T>
		T& toRef() { return reinterpret_cast<T&>(this->data_); }
		template<typename T>
		T const& toRef() const { return reinterpret_cast<T const&>(this->data_); }

		template<typename T>
		T to() const { return reinterpret_cast<T>(this->data_); }
	protected:
		uintptr_t data_;
	};

	typedef unique_ptr<Texture>::type (*TextureLoader)(SystemString const& path);

	class MaterialPool
	{
	public:
		MaterialPool(SystemString const& base);
	private:
		typedef boost::ptr_unordered_map<rpg2k::String, Texture> Pool;
		typedef boost::ptr_vector<Pool> PoolList; PoolList poolList_;
		typedef std::vector<TextureLoader> LoaderList; LoaderList loaderList_;
	};
 
	class Graphics2D
	{
	public:
		virtual ~Graphics2D();

		virtual void drawTexture(Texture const& img, vec2 const& dstP) = 0;
		virtual void drawTexture(Texture const& img, Rect const& src, vec2 const& dstP) = 0;
		virtual void drawTexture(Texture const& img, Rect const& src, Rect const& dst) = 0;

		virtual void drawTextureCenter(Texture const& img, vec2 const& dstP) = 0;
	}; // class Graphics2D
} // namespace rpg2k

#endif // _INC_RPG2K__GRAPHICS_HXX_
