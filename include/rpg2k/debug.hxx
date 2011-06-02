#ifndef _INC_RPG2K__DEBUG_HXX_
#define _INC_RPG2K__DEBUG_HXX_

#include "define.hxx"
#include "descriptor.hxx"

#include <errno.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <string>
#include <typeinfo>
#include <exception>


namespace rpg2k {
	using std::cerr;
	using std::clog;
	using std::cout;
	using std::endl;

	namespace structure {
		class instruction;
	} // namespace structure

	namespace debug {
		std::string error(int err_no);
		void add_at_exit_function(void (*func)(void));

		std::string demangle_type_info(std::type_info const& info);
		#if RPG2K_USE_RTTI
			template<typename T>
			inline std::string demangle() { return demangle_type_info(typeid(T)); }
			template<typename T>
			inline std::string demangle(T& src) { return demangle_type_info(typeid(src)); }
		#endif

		extern std::ofstream ANALYZE_RESULT; // default is analyze.txt

		class analyze_exception : public std::exception {};
#if RPG2K_DEBUG
#	define rpg2k_analyze_assert(EXP) if(!(EXP)) throw debug::analyze_exception()
#else
#	define rpg2k_analyze_assert(EXP) rpg2k_assert(EXP)
#endif

		struct tracer {
			#define PP_default_output std::ostream& ostrm = ANALYZE_RESULT

			typedef structure::array1d array1d;
			typedef structure::array2d array2d;
			typedef structure::ber_enum ber_enum;
			typedef structure::element element;
			typedef structure::event event;
			typedef structure::instruction instruction;

			static std::ostream& print_trace(element const& e, bool detail = false, PP_default_output);
			static std::ostream& print_detail(element const& e, PP_default_output);

#define PP_enum(r, data, elem) \
	static std::ostream& \
	BOOST_PP_CAT(print_, elem)(elem const& val, PP_default_output);
			BOOST_PP_SEQ_FOR_EACH(PP_enum, , PP_all_types)
#undef PP_enum

			static std::ostream& print_instruction(instruction const& inst, PP_default_output, bool indent = false);
#undef PP_default_output
		}; // class tracer
	} // namespace debug
} // namespace rpg2k

#endif // _INC_RPG2K__DEBUG_HXX_
