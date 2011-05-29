#include "rpg2k/debug.hxx"
#include "rpg2k/model.hxx"
#include "rpg2k/stream.hxx"
#include "rpg2k/define/define.hxx"

#include <algorithm>
#include <fstream>
#include <stack>
#include <utility>

#include <cctype>
#include <cstdio>

#include <boost/foreach.hpp>

using rpg2k::structure::Descriptor;
using rpg2k::structure::Element;


namespace rpg2k
{
	namespace model
	{
		bool fileExists(SystemString const& fileName)
		{
			return std::ifstream(fileName.c_str()).good();
		}

		Base::Base(SystemString const& dir)
		: fileDir_(dir)
		{
			checkExists();
		}
		Base::Base(SystemString const& dir, SystemString const& name)
		: fileDir_(dir), fileName_(name)
		{
			checkExists();
		}

		void Base::reset()
		{
			data_.clear();
			BOOST_FOREACH(Descriptor const& i, this->descriptor()) {
				data_.push_back(new Element(i));
			}
		}

		Element& Base::operator [](unsigned index)
		{
			return data_.front().toArray1D()[index];
		}
		Element const& Base::operator [](unsigned index) const
		{
			return data_.front().toArray1D()[index];
		}

		boost::ptr_vector<Descriptor> const& Base::descriptor() const
		{
			return DefineLoader::instance().get(header());
		}

		void Base::checkExists()
		{
			exists_ = fileExists(fullPath());
		}

		void Base::load()
		{
			if(fileName_.empty()) fileName_ = defaultName();
			rpg2k_assert(exists());

			std::ifstream ifs(fullPath().c_str(), stream::INPUT_FLAG);

			if(!stream::checkHeader(ifs, this->header())) rpg2k_assert(false);
			/*
			if(this->header() == std::string("LcfMapTree")) {
				// TODO
			}
			*/

			BOOST_FOREACH(Descriptor const& i, this->descriptor()) {
				data_.push_back(new Element(i, ifs));
			}

			rpg2k_assert(stream::isEOF(ifs));

			loadImpl();
		}
		void Base::saveAs(SystemString const& filename)
		{
			exists_ = true;
			saveImpl();
			std::ofstream ofs(filename.c_str(), stream::OUTPUT_FLAG);
			serialize(ofs);
		}
		void Base::serialize(std::ostream& s)
		{
			stream::writeHeader(s, this->header());
			BOOST_FOREACH(Element const& i, data_) { i.serialize(s); }
		}

		DefineLoader::DefineLoader()
		{
			isArray_.insert("Music");
			isArray_.insert("Sound");
			isArray_.insert("EventState");

			isArray_.insert("Array1D");
			isArray_.insert("Array2D");

			#define PP_insert(arg) \
				defineText_.insert(std::make_pair(String(#arg), define::arg))
			PP_insert(EventState);
			PP_insert(LcfDataBase);
			PP_insert(LcfMapTree);
			PP_insert(LcfMapUnit);
			PP_insert(LcfSaveData);
			PP_insert(Music);
			PP_insert(Sound);
			#undef PP_insert
		}

		boost::ptr_vector<Descriptor> const& DefineLoader::get(String const& name)
		{
			DefineBuffer::const_iterator it = defineBuff_.find(name);
			if(it == defineBuff_.end()) {
				boost::ptr_vector<Descriptor>& ret = defineBuff_[name];
				load(ret, name);
				return ret;
			} else return it->second;
		}
		structure::ArrayDefine DefineLoader::arrayDefine(String const& name)
		{
			return get(name).front().arrayDefine();
		}

		void DefineLoader::load(boost::ptr_vector<structure::Descriptor>& dst, String const& name)
		{
			DefineText::const_iterator it = defineText_.find(name);
			rpg2k_assert(it != defineText_.end());

			namespace io = boost::iostreams;
			io::stream<io::array_source> stream(io::array_source(it->second, std::strlen(it->second)));
			std::deque<String> token;
			toToken(token, stream);
			parse(dst, token);
		}

		// parser for define Stream

		#define nextToken(curType) prev = curType; continue

		void DefineLoader::parse(boost::ptr_vector<structure::Descriptor>& dst
		, std::deque<String> const& token)
		{
			bool blockComment = false;
			unsigned int streamComment = 0, line = 1, col = 0;
			String typeName;

			enum TokenType
			{
				OPEN_INDEX = 0, INDEX, CLOSE_INDEX1, CLOSE_INDEX2,
				TYPE, NAME, EQUALS, DEFAULT,
				OPEN_STRUCT, CLOSE_STRUCT,
				EXP_END,
			} prev = EXP_END;

			using namespace structure;
			std::stack<ArrayDefineType*> nest;
			std::stack<Descriptor::ArrayTable*> tableNest;

			// if success continue else error
			for(std::deque<String>::const_iterator i = token.begin(); i < token.end(); ++i) {
				if(*i == "\n") { blockComment = false; line++; continue;
				} else if(blockComment) { continue;
				} else if(streamComment) {
					if((*i == "*") && (*(++i) == "/")) { streamComment--; }
					continue;
				} else if(*i == "/") {
					++i;
					if(*i == "*") { streamComment++; continue; }
					else if(*i == "/") { blockComment = true; continue; }
				} else if(nest.empty()) switch(prev) {
					case TYPE: nextToken(NAME);
					case NAME:
						if(*i == ";") {
							dst.push_back(new Descriptor(typeName));
							nextToken(EXP_END);
						} else if(isArray(typeName) && (*i == "{")) {
							ArrayDefineType* arrayDefine = new ArrayDefineType;
							Descriptor::ArrayTable* arrayTable = new Descriptor::ArrayTable;
							tableNest.push(arrayTable);
							nest.push(arrayDefine);

							dst.push_back(new Descriptor(typeName, ArrayDefinePointer(arrayDefine), unique_ptr<Descriptor::ArrayTable>::type(arrayTable)));

							nextToken(OPEN_STRUCT);
						}
						break;
					case CLOSE_STRUCT:
						if(*i == ";") { nextToken(EXP_END); } else break;
					case EXP_END:
						typeName = *i;
						nextToken(TYPE);
					default: break;
				} else switch(prev) {
					case OPEN_INDEX: {
						io::stream<io::array_source> ss(io::array_source(i->data(), i->size()));
						ss >> col;
						if(nest.top()->find(col) != nest.top()->end()) { break; }
						else { nextToken(INDEX); }
					}
					case INDEX:
						if(*i == "]") { nextToken(CLOSE_INDEX1); } else break;
					case CLOSE_INDEX1:
						if(*i == ":") { nextToken(CLOSE_INDEX2); } else break;
					case CLOSE_INDEX2:
						typeName = *i;
						nextToken(TYPE);
					case TYPE:
						if((*i == "dummy")
						|| tableNest.top()->insert(std::make_pair(*i, col)).second) { nextToken(NAME); }
						else { break; }
					case NAME:
						if(*i == "=") { nextToken(EQUALS);
						} else if(*i == ";") {
							if(isArray(typeName)) {
								Descriptor const& def = this->get(typeName)[0];
								nest.top()->insert(col, new Descriptor(
									ElementType::instance().toString(def.type()),
									ArrayDefinePointer(new ArrayDefineType(def.arrayDefine())),
									unique_ptr<Descriptor::ArrayTable>::type(new Descriptor::ArrayTable(def.arrayTable()))));
							} else nest.top()->insert(col, new Descriptor(typeName));

						nextToken(EXP_END);
						} else if((*i == "{") && isArray(typeName)) {
							ArrayDefineType* arrayDefine = new ArrayDefineType;
							Descriptor::ArrayTable* arrayTable = new Descriptor::ArrayTable;

							nest.top()->insert(col, new Descriptor(typeName, ArrayDefinePointer(arrayDefine), unique_ptr<Descriptor::ArrayTable>::type(arrayTable)));
							tableNest.push(arrayTable);
							nest.push(arrayDefine);

							nextToken(OPEN_STRUCT);
						} else break;
					case EQUALS:
						if(isArray(typeName)) {
							Descriptor const& def = this->get(*i)[0];
							nest.top()->insert(col,
								new Descriptor(typeName,
								ArrayDefinePointer(new ArrayDefineType(def.arrayDefine())),
								unique_ptr<Descriptor::ArrayTable>::type(new Descriptor::ArrayTable(def.arrayTable()))));
						} else nest.top()->insert(col, new Descriptor(typeName, *i));
						nextToken(DEFAULT);
					case DEFAULT:
						if(*i == ";") { nextToken(EXP_END); } else break;
					case OPEN_STRUCT:
						if(*i == "[") { nextToken(OPEN_INDEX); }
						else if(*i == "}") { nest.pop(); tableNest.pop(); nextToken(CLOSE_STRUCT); }
						else break;
					case CLOSE_STRUCT:
						if(*i == ";") { nextToken(EXP_END); } else break;
					case EXP_END:
						if(*i == "[") { nextToken(OPEN_INDEX); }
						else if(*i == "}") { nest.pop(); tableNest.pop(); nextToken(CLOSE_STRUCT); }
						else break;
					default: break;
				}

				cout << "Error at line: " << line << endl;
				cout << "\tToken: " << *i << endl;
				rpg2k_assert(false);
			}

			rpg2k_assert(streamComment == 0);
		}

		#undef nextToken

		void DefineLoader::toToken(std::deque<String>& token, std::istream& stream)
		{
			String strBuf;

			while(true) {
				int buf = stream.get();

				if(buf == -1) {
					if(!strBuf.empty()) token.push_back(strBuf);
					break;
				} else if(buf == '\"') {
					if(!strBuf.empty() && (*strBuf.begin() == '\"')) {
						strBuf.push_back(buf);
						token.push_back(strBuf);
						strBuf.clear();
					} else {
						if(!strBuf.empty()) {
							token.push_back(strBuf);
							strBuf.clear();
						}
						strBuf.push_back(buf);
					}
				} else if(!strBuf.empty() && (*strBuf.begin() == '\"')) { strBuf.push_back(buf);
				} else if(std::isalpha(buf) || std::isdigit(buf) || (buf == '_')) {
					strBuf.push_back(buf);
				} else {
					if(!strBuf.empty()) {
						token.push_back(strBuf);
						strBuf.clear();
					}
					switch(buf) {
						case ' ': case '\t': case '\r': break;
						default:
							token.push_back(String(1, buf));
							break;
					}
				}
			}
		}
	} // namespace model
} // namespace rpg2k
