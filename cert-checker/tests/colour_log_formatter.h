/*
 *  (C) Copyright Gennadiy Rozental 2005-2008.
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  See http://www.boost.org/libs/test for the library home page.
 */
/*
 * @file        colour_log_formatter.h
 * @author      Zofia Abramowska (z.abramowska@samsung.com)
 * @version
 * @brief
 */
#ifndef COLOUR_LOG_FORMATTER_H_
#define COLOUR_LOG_FORMATTER_H_

#include <boost/test/unit_test_log_formatter.hpp>

namespace CCHECKER {
class colour_log_formatter : public boost::unit_test::unit_test_log_formatter {
public:
	// Formatter interface
	colour_log_formatter() : m_isTestCaseFailed(false) {}
	void    log_start(
		std::ostream &,
		boost::unit_test::counter_t test_cases_amount);
	void    log_finish(std::ostream &);
	void    log_build_info(std::ostream &);
	void    log_build_info(std::ostream &output, bool log_build_info = true);

	void    test_unit_start(
		std::ostream &,
		boost::unit_test::test_unit const &tu);
	void    test_unit_finish(
		std::ostream &,
		boost::unit_test::test_unit const &tu,
		unsigned long elapsed);
	void    test_unit_skipped(
		std::ostream &,
		boost::unit_test::test_unit const &tu);

	void    log_exception_start(
		std::ostream &,
		boost::unit_test::log_checkpoint_data const &,
		boost::execution_exception const &ex);
	void    log_exception_finish(std::ostream &os);

	void    log_entry_start(
		std::ostream &,
		boost::unit_test::log_entry_data const &,
		log_entry_types let);
	void    log_entry_value(
		std::ostream &,
		boost::unit_test::const_string value);
	void    log_entry_value(
		std::ostream &,
		boost::unit_test::lazy_ostream const &value);
	void    log_entry_finish(std::ostream &);

	void    entry_context_start(
		std::ostream& os, 
		boost::unit_test::log_level l);
        void    log_entry_context(
        	std::ostream& os, 
        	boost::unit_test::const_string value);
	void    entry_context_finish(std::ostream& os);
#if BOOST_VERSION >= 106500
	void 	log_entry_context(
		std::ostream& os,
		boost::unit_test::log_level l,
		boost::unit_test::const_string value);
	void 	entry_context_finish(
		std::ostream& os,
		boost::unit_test::log_level l);
#endif


private:
	bool m_isTestCaseFailed;
};
} // namespace CCHECKER

#endif /* COLOUR_LOG_FORMATTER_H_ */
