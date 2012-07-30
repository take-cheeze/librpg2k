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

#include <boost/filesystem.hpp>
#include <boost/ptr_container/ptr_vector.hpp>


namespace picojson {
  class value;
} // namespace picojson

namespace rpg2k {
namespace model {

class base
{
 private:
  boost::filesystem::path path_;
  boost::ptr_vector<structure::element> data_;
  string const header_;

  virtual void load_impl() {}
  virtual void save_impl() {}

  virtual char const* default_filename() const { return ""; }
  virtual string analyze_prefix() const {
    return string(header_).append(":");
  }
 protected:
  boost::ptr_vector<structure::element>& data() { return data_; }
  boost::ptr_vector<structure::element> const& data() const { return data_; }

  void check_exists();

  boost::ptr_vector<structure::descriptor> const& definition() const;

  base(boost::filesystem::path const& p, string const& h);

  base(base const& src);
  base const& operator =(base const& src);

  void load();
 public:
  virtual ~base() {}

  bool exists() const { return boost::filesystem::exists(path_); }

  void reset();

  structure::element& operator [](unsigned index);
  structure::element const& operator [](unsigned index) const;

  boost::filesystem::path const& path() const { return path_; }
  void set_path(boost::filesystem::path const& p) { path_ = p; }

  void save_as(boost::filesystem::path const& filename);
  void save() { save_as(path()); }

  void analyze() const;
  picojson::value to_json() const;

  void serialize(std::ostream& s);
}; // class base

unique_ptr<base>::type load_lcf(boost::filesystem::path const& filename);

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
