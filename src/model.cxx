#include "rpg2k/debug.hxx"
#include "rpg2k/model.hxx"
#include "rpg2k/stream.hxx"

#include "rpg2k/map_tree.hxx"
#include "rpg2k/map_unit.hxx"
#include "rpg2k/save_data.hxx"
#include "rpg2k/database.hxx"

#include <picojson.h>

#include <algorithm>
#include <fstream>
#include <stack>
#include <utility>

#include <cctype>
#include <cstdio>
#include <cstring>

#include <boost/foreach.hpp>

using rpg2k::structure::descriptor;
using rpg2k::structure::element;
namespace fs = boost::filesystem;


namespace rpg2k {

namespace define {
extern const char* event_state;
extern const char* LcfDataBase;
extern const char* LcfMapTree;
extern const char* LcfMapUnit;
extern const char* LcfSaveData;
extern const char* music;
extern const char* sound;
}

namespace model
{

unique_ptr<base>::type load(fs::path const& p) {
  std::ifstream is(p.c_str(), stream::INPUT_FLAG);
  rpg2k_assert(is);

  string head;

  picojson::value v;
  std::string err = picojson::parse(v, is);
  is.seekg(0);

  if(err.empty()) {
    head = v.get("signature").get<std::string>();
  } else {
    head = stream::read_header(is);
  }

  if(head == "LcfMapTree") { return unique_ptr<base>::type(new map_tree(p)); }
  else if(head == "LcfMapUnit") { return unique_ptr<base>::type(new map_unit(p)); }
  else if(head == "LcfDataBase") { return unique_ptr<base>::type(new database(p)); }
  else if(head == "LcfSaveData") { return unique_ptr<base>::type(new save_data(p)); }
  else {
    assert(false);
    return unique_ptr<base>::type(NULL);
  }
}

base::base(fs::path const& p, string const& h)
		: path_(p), header_(h)
{
  check_exists();
}

void base::reset()
{
  data_.clear();
  BOOST_FOREACH(descriptor const& i, this->definition()) {
    data_.push_back(new element(i));
  }
}

element& base::operator [](unsigned index)
{
  return data_.front().to_array1d()[index];
}
element const& base::operator [](unsigned index) const
{
  return data_.front().to_array1d()[index];
}

boost::ptr_vector<descriptor> const& base::definition() const
{
  return define_loader::instance().get(header_);
}

void base::check_exists()
{
  path_ = fs::absolute(path_);
}

void base::load()
{
  if(fs::is_directory(path_)) { path_ /= default_filename(); }
  rpg2k_assert(exists());

  std::ifstream ifs(path_.c_str(), stream::INPUT_FLAG);

  picojson::value v;
  std::string err = picojson::parse(v, ifs);
  ifs.seekg(0);

  if(err.empty()) {
    rpg2k_assert(v.get("signature").get<std::string>() == header_);
    picojson::array const& ary = v.get("root").get<picojson::array>();

    rpg2k_assert(definition().size() == ary.size());

    BOOST_FOREACH(descriptor const& i, this->definition()) {
      data_.push_back(new element(i));
      data_.back().assign(ary[data_.size() - 1]);
    }
  } else {
    if(!stream::check_header(ifs, header_)) { rpg2k_assert(false); }

    BOOST_FOREACH(descriptor const& i, this->definition()) {
      data_.push_back(new element(i, ifs));
    }
  }

  rpg2k_assert(stream::is_eof(ifs));

  load_impl();
}
void base::save_as(fs::path const& filename)
{
  save_impl();
  std::ofstream ofs(filename.c_str(), stream::OUTPUT_FLAG);
  serialize(ofs);
  if(header_ == "LcfMapUnit") {
    stream::write_ber(ofs, structure::array1d::END_OF_ARRAY1D);
  }
}
void base::serialize(std::ostream& s)
{
  stream::write_header(s, header_);
  BOOST_FOREACH(element const& i, data_) { i.serialize(s); }
}

void base::analyze() const
{
  debug::ANALYZE_RESULT << analyze_prefix() << endl;
  BOOST_FOREACH(element const& i, data_) {
    debug::tracer::print_trace(i, true);
  }
}

picojson::value base::to_json() const
{
  picojson::object ret;
  ret[picojson::object_key("signature")] = picojson::value(header_.to_system());

  picojson::array ary;
  BOOST_FOREACH(element const& i, data_) {
    ary.push_back(i.to_json());
  }
  ret[picojson::object_key("root")] = picojson::value(ary);

  return picojson::value(ret);
}

define_loader::define_loader()
{
  is_array_.insert("music");
  is_array_.insert("sound");
  is_array_.insert("event_state");

  is_array_.insert("array1d");
  is_array_.insert("array2d");

#define PP_insert(r, data, elem)                                        \
	define_text_.insert(std::make_pair(string(BOOST_PP_STRINGIZE(elem)), define::elem));
  BOOST_PP_SEQ_FOR_EACH(PP_insert, ,
                        (LcfDataBase)(LcfMapTree)(LcfMapUnit)(LcfSaveData)
                        (event_state)(music)(sound))
#undef PP_insert
      }

boost::ptr_vector<descriptor> const& define_loader::get(string const& name)
{
  define_buffer::const_iterator it = define_buff_.find(name);
  if(it == define_buff_.end()) {
    boost::ptr_vector<descriptor>& ret = define_buff_[name];
    load(ret, name);
    return ret;
  } else return it->second;
}
structure::array_define_type const& define_loader::array_define(string const& name)
{
  return get(name).front().array_define();
}

void define_loader::load(boost::ptr_vector<structure::descriptor>& dst, string const& name)
{
  define_text::const_iterator it = define_text_.find(name);
  rpg2k_assert(it != define_text_.end());

  namespace io = boost::iostreams;
  io::stream<io::array_source> stream(io::array_source(it->second, std::strlen(it->second)));
  std::deque<string> token;
  to_token(token, stream);
  parse(dst, token);
}

// parser for define Stream

#define next_token(cur_type) prev = cur_type; continue

void define_loader::parse(boost::ptr_vector<structure::descriptor>& dst
                          , std::deque<string> const& token)
{
  bool block_comment = false;
  unsigned int stream_comment = 0, line = 1, col = 0;
  structure::element_type::type type;
  string type_name;

  enum token_type
  {
    OPEN_INDEX, INDEX, CLOSE_INDEX_1, CLOSE_INDEX_2,
    TYPE, NAME, EQUALS, DEFAULT,
    OPEN_STRUCT, CLOSE_STRUCT,
    EXP_END,
  } prev = EXP_END;

  using namespace structure;
  std::stack<array_define_type*> nest;
  std::stack<array_table_type*> table_nest;

  // if success continue else error
  for(std::deque<string>::const_iterator i = token.begin(); i < token.end(); ++i) {
    if(*i == "\n") { block_comment = false; line++; continue;
    } else if(block_comment) { continue;
    } else if(stream_comment) {
      if((*i == "*") && (*(++i) == "/")) { stream_comment--; }
      continue;
    } else if(*i == "/") {
      ++i;
      if(*i == "*") { stream_comment++; continue; }
      else if(*i == "/") { block_comment = true; continue; }
    } else if(nest.empty()) switch(prev) {
        case TYPE: next_token(NAME);
        case NAME:
          if(*i == ";") {
            dst.push_back(new descriptor(type));
            next_token(EXP_END);
          } else if(is_array(type_name) && (*i == "{")) {
            array_define_type* array_define = new array_define_type;
            array_table_type* array_table = new array_table_type;
            table_nest.push(array_table);
            nest.push(array_define);

            dst.push_back(new descriptor(type, unique_ptr<array_define_type>::type(array_define), unique_ptr<array_table_type>::type(array_table)));

            next_token(OPEN_STRUCT);
          }
          break;
        case CLOSE_STRUCT:
          if(*i == ";") { next_token(EXP_END); } else break;
        case EXP_END:
          type = structure::element_type::instance().to_enum(*i);
          type_name = *i;
          next_token(TYPE);
        default: break;
      } else switch(prev) {
        case OPEN_INDEX: {
          io::stream<io::array_source> ss(io::array_source(i->data(), i->size()));
          ss >> col;
          if(nest.top()->find(col) != nest.top()->end()) { break; }
          else { next_token(INDEX); }
        }
        case INDEX:
          if(*i == "]") { next_token(CLOSE_INDEX_1); } else break;
        case CLOSE_INDEX_1:
          if(*i == ":") { next_token(CLOSE_INDEX_2); } else break;
        case CLOSE_INDEX_2:
          type = structure::element_type::instance().to_enum(*i);
          type_name = *i;
          next_token(TYPE);
        case TYPE:
          if((*i == "dummy")
             || table_nest.top()->insert(std::make_pair(*i, col)).second) { next_token(NAME); }
          else { break; }
        case NAME:
          if(*i == "=") { next_token(EQUALS);
          } else if(*i == ";") {
            if(is_array(type_name)) {
              descriptor const& def = this->get(type_name)[0];
              nest.top()->insert(col, new descriptor(
									def.type,
									unique_ptr<array_define_type>::type(new array_define_type(def.array_define())),
									unique_ptr<array_table_type>::type(new array_table_type(def.array_table()))));
            } else nest.top()->insert(col, new descriptor(type));

						next_token(EXP_END);
          } else if((*i == "{") && is_array(type_name)) {
            array_define_type* array_define = new array_define_type;
            array_table_type* array_table = new array_table_type;

            nest.top()->insert(col, new descriptor(type, unique_ptr<array_define_type>::type(array_define), unique_ptr<array_table_type>::type(array_table)));
            table_nest.push(array_table);
            nest.push(array_define);

            next_token(OPEN_STRUCT);
          } else break;
        case EQUALS:
          if(is_array(type_name)) {
            descriptor const& def = this->get(*i)[0];
            nest.top()->insert(col,
                               new descriptor(type,
                                              unique_ptr<array_define_type>::type(new array_define_type(def.array_define())),
                                              unique_ptr<array_table_type>::type(new array_table_type(def.array_table()))));
          } else nest.top()->insert(col, new descriptor(type, *i));
          next_token(DEFAULT);
        case DEFAULT:
          if(*i == ";") { next_token(EXP_END); } else break;
        case OPEN_STRUCT:
          if(*i == "[") { next_token(OPEN_INDEX); }
          else if(*i == "}") { nest.pop(); table_nest.pop(); next_token(CLOSE_STRUCT); }
          else break;
        case CLOSE_STRUCT:
          if(*i == ";") { next_token(EXP_END); } else break;
        case EXP_END:
          if(*i == "[") { next_token(OPEN_INDEX); }
          else if(*i == "}") { nest.pop(); table_nest.pop(); next_token(CLOSE_STRUCT); }
          else break;
        default: break;
      }

    cout << "Error at line: " << line << endl;
    cout << "\tToken: " << *i << endl;
    rpg2k_assert(false);
  }

  rpg2k_assert(stream_comment == 0);
}

#undef next_token

void define_loader::to_token(std::deque<string>& token, std::istream& stream)
{
  string str_buf;

  while(true) {
    int buf = stream.get();

    if(buf == -1) {
      if(!str_buf.empty()) token.push_back(str_buf);
      break;
    } else if(buf == '\"') {
      if(!str_buf.empty() && (*str_buf.begin() == '\"')) {
        str_buf.push_back(buf);
        token.push_back(str_buf);
        str_buf.clear();
      } else {
        if(!str_buf.empty()) {
          token.push_back(str_buf);
          str_buf.clear();
        }
        str_buf.push_back(buf);
      }
    } else if(!str_buf.empty() && (*str_buf.begin() == '\"')) { str_buf.push_back(buf);
    } else if(std::isalpha(buf) || std::isdigit(buf) || (buf == '_')) {
      str_buf.push_back(buf);
    } else {
      if(!str_buf.empty()) {
        token.push_back(str_buf);
        str_buf.clear();
      }
      switch(buf) {
        case ' ': case '\t': case '\r': break;
        default:
          token.push_back(string(1, buf));
          break;
      }
    }
  }
}

} // namespace model
} // namespace rpg2k
