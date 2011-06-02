#ifndef _INC_RPG2K__MODEL_HXX_
#define _INC_RPG2K__MODEL_HXX_

#include <deque>

#include <algorithm>
#include <map>
#include <set>

#include "array1d.hxx"
#include "array2d.hxx"
#include "element.hxx"
#include "singleton.hxx"

#include <boost/ptr_container/ptr_vector.hpp>


namespace rpg2k
{

	namespace model
	{
		bool file_exists(system_string const& filename);

		class base
		{
		private:
			bool exists_;

			system_string file_dir_, filename_;
			boost::ptr_vector<structure::element> data_;

			virtual void load_impl() = 0;
			virtual void save_impl() = 0;

			virtual char const* header() const = 0;
			virtual char const* default_filename() const = 0;
		protected:
			void set_filename(system_string const& name) { filename_ = name; }
			boost::ptr_vector<structure::element>& data() { return data_; }
			boost::ptr_vector<structure::element> const& data() const { return data_; }

			void check_exists();

			boost::ptr_vector<structure::descriptor> const& definition() const;

			base(system_string const& dir);
			base(system_string const& dir, system_string const& name);

			base(base const& src);
			base const& operator =(base const& src);

			void load();
		public:
			virtual ~base() {}

			bool exists() const { return exists_; }

			void reset();

			structure::element& operator [](unsigned index);
			structure::element const& operator [](unsigned index) const;

			system_string const& filename() const { return filename_; }
			system_string const& directory() const { return file_dir_; }
			system_string full_path() const
			{
				return system_string(file_dir_).append(PATH_SEPR).append(filename_);
			} // not absolute

			void save_as(system_string const& filename);
			void save() { save_as(full_path()); }

			void serialize(std::ostream& s);
		}; // class base

		class define_loader : public singleton<define_loader>
		{
			friend class singleton<define_loader>;
		private:
			typedef std::map<string, boost::ptr_vector<structure::descriptor> > define_buffer;
			define_buffer define_buff_;
			typedef std::map<string, const char*> define_text;
			define_text define_text_;
			std::set<string> is_array_;
		protected:
			void parse(boost::ptr_vector<structure::descriptor>& dst, std::deque<string> const& token);
			void load(boost::ptr_vector<structure::descriptor>& dst, string const& name);

			define_loader();
			define_loader(define_loader const& dl);
		public:
			boost::ptr_vector<structure::descriptor> const& get(string const& name);
			structure::array_define_type const& array_define(string const& name);

			bool is_array(string const& type_name) const
			{
				return is_array_.find(type_name) != is_array_.end();
			}

			static void to_token(std::deque<string>& token, std::istream& stream);
		}; // class define_loader
	} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__MODEL_HXX_
