#include <cstdlib>
#include <iterator>
#include <memory>
#include <sstream>
#include <stack>
#include <vector>

#include <boost/lambda/lambda.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/irange.hpp>

#include "rpg2k/array1d.hxx"
#include "rpg2k/array2d.hxx"
#include "rpg2k/debug.hxx"
#include "rpg2k/element.hxx"
#include "rpg2k/event.hxx"
#include "rpg2k/stream.hxx"


namespace rpg2k
{
	namespace debug
	{
		using namespace structure;

		std::string error(int const err_no)
		{
			char const* message = strerror(err_no);
			switch(errno) {
				case EINVAL: rpg2k_assert(false);
				default: return message;
			}
		}
		void addAtExitFunction(void (*func)(void))
		{
			if(atexit(func) != 0) rpg2k_assert(false);
		}

		std::ofstream ANALYZE_RESULT("analyze.txt");

		std::ostream& tracer::print_trace(element const& e, bool const detail, std::ostream& ostrm)
		{
			std::stack<element const*> st;
      std::ostringstream oss;

			for(element const* buf = &e; buf->has_owner(); buf = &(buf->owner())) {
				st.push(buf);
			}

      ostrm << std::dec << std::setfill(' ');
			for(; !st.empty(); st.pop()) {
				element const& top = *st.top();

				element_type::type const owner_type = top.owner().definition().type;

				// oss << element_type::instance().to_string(owner_type);
				if(owner_type == element_type::array2d_) {
          oss << "[" << std::setw(4) << top.index_of_array2d() << "]";
        }
        oss << "[" << std::setw(4) << top.index_of_array1d() << "]";
				oss << ": ";

				if(owner_type == element_type::array2d_) {
          ostrm << "[" << std::setw(4) << top.index_of_array2d() << "]";
        }
        ostrm << "[" << std::setw(4) << 
          top.definition().index_to_name(top.index_of_array1d()) << "]";
				ostrm << ": ";
      }
      ostrm << endl << oss.str();

			if(detail) { tracer::print_detail(e, ostrm); }

			return ostrm;
		}

		std::ostream& tracer::print_detail(element const& e, std::ostream& ostrm)
		{
			if(e.is_defined()) {
				ostrm << e.definition().type_name() << ": ";

				switch(e.definition().type) {
#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		BOOST_PP_CAT(print_, elem)(e.BOOST_PP_CAT(to_, elem)(), ostrm); break;
					BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (binary)(event)(string)(bool)(double)(int))
#undef PP_enum

        case element_type::array1d_:
          ostrm << endl;
          print_array1d(e.to_array1d(), ostrm);
          break;
        case element_type::array2d_:
          ostrm << endl;
          print_array2d(e.to_array2d(), ostrm);
          break;
        default: break;
				}
			} else {
				binary const bin = e.serialize();
				if(bin.size() == 0) {
					ostrm << "This data is empty." << endl;
					return ostrm;
				}
        // binary
				ostrm << "undefined" << endl << "binary: ";
				print_binary(bin, ostrm);
        // event
				try {
					event event(bin);
					ostrm << endl << "event: ";
					print_event(event, ostrm);
				} catch(...) {}
        // BER number
				if(bin.is_ber()) {
					ostrm << endl << "BER: ";
					print_int(bin, ostrm);
				}
        // string
				if(bin.is_string()) {
					ostrm << endl << "string: ";
					print_string(static_cast<string>(bin), ostrm);
				}
        // array1d
				try {
					unique_ptr<element>::type p(new element(descriptor(element_type::array1d_
                                                             , unique_ptr<array_define_type>::type(new array_define_type)
                                                             , unique_ptr<array_table_type>::type(new array_table_type())), bin));
					ostrm << endl << "---array1d check start---" << endl;
					p.reset(); // trigger element's destructor
					ostrm << "---array1d check end  ---";
				} catch(...) {}
        // array2d
				try {
					unique_ptr<element>::type p(new element(descriptor(element_type::array2d_
                                                             , unique_ptr<array_define_type>::type(new array_define_type)
                                                             , unique_ptr<array_table_type>::type(new array_table_type())), bin));
					ostrm << endl << "---array2d check start---" << endl;
					p.reset(); // trigger element's destructor
					ostrm << "---array2d check end  ---";
				} catch(...) {}
			}

			ostrm << endl;

			return ostrm;
		}

		std::ostream& tracer::print_array1d(array1d const& val, std::ostream& ostrm)
		{
			std::vector<element const*> buf;
			for(array1d::const_iterator i = val.begin(); i != val.end(); ++i) {
				buf.push_back(i->second);
			}
			boost::sort(buf, array1d::sort_function);
			BOOST_FOREACH(element const* i, buf) { print_trace(*i, true, ostrm); }
			return ostrm;
		}
		std::ostream& tracer::print_array2d(array2d const& val, std::ostream& ostrm)
		{
			for(array2d::const_iterator i = val.begin(); i != val.end(); ++i) {
				print_array1d(*i->second, ostrm);
			}
			return ostrm;
		}
		std::ostream& tracer::print_int(int const& val, std::ostream& ostrm)
		{
			ostrm << std::dec << val;
			return ostrm;
		}
		std::ostream& tracer::print_bool(bool const& val, std::ostream& ostrm)
		{
			ostrm << std::boolalpha << val;
			return ostrm;
		}
		std::ostream& tracer::print_double(double const& val, std::ostream& ostrm)
		{
			ostrm << std::showpoint << val;
			return ostrm;
		}
		std::ostream& tracer::print_string(string const& val, std::ostream& ostrm)
		{
			ostrm << "\"" << val << "\"";
			return ostrm;
		}
		std::ostream& tracer::print_event(event const& val, std::ostream& ostrm)
		{
			ostrm << std::dec << std::setfill(' ');
			ostrm << "size = " << val.serialized_size() << "; data = {";

			BOOST_FOREACH(instruction const& i, val) {
				ostrm << endl << "\t";
				print_instruction(i, ostrm, true);
			}

			ostrm << endl << "}";

			return ostrm;
		}
		std::ostream& tracer::print_instruction(instruction const& inst
		, std::ostream& ostrm, bool const indent)
		{
			if(indent) {
				BOOST_FOREACH(int const i, boost::irange(0, int(inst.nest))) { (void) i; ostrm << "\t"; }
      }
			ostrm << "{ "
				<< "nest: " << std::setw(4) << std::dec << inst.nest << ", "
				<< "code: " << std::setw(5) << std::dec << inst.code << ", "
				<< "string: \"" << inst.string_argument << "\", "
				<< "integer[" << inst.size() << "]: "
				;
				ostrm << "{ ";
					std::copy(inst.begin(), inst.end()
					, std::ostream_iterator<int32_t>(ostrm, ", "));
				ostrm << "}, ";
			ostrm << "}";

			return ostrm;
		}
		std::ostream& tracer::print_binary(binary const& val, std::ostream& ostrm)
		{
			ostrm << std::setfill(' ') << std::dec;
			ostrm << "size = " << val.size() << "; data = { ";

			ostrm << std::setfill('0') << std::hex;
			BOOST_FOREACH(uint8_t const i, val) {
				ostrm << std::setw(2) << (i & 0xff) << " ";
			}

			ostrm << "}";

			return ostrm;
		}
	} // namespace debug
} // namespace rpg2k
