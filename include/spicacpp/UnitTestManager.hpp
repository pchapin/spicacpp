/*! \file    UnitTestManager.hpp
 *  \brief   Interface to the unit test manager abstract object.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <iostream>
#include <stdexcept>
#include <string>

//! Namespace containing the unit test manager.
/*!
 * The unit test manager is a framework for writing unit tests. It is designed to be simple to
 * use. It produces XML output that can be processed by XSLT, if necessary, before being
 * presented to the user or digested by other programs.
 *
 * Tests are organized into suites. Each suite is registered in its entirety. When a suite is
 * executed, all tests in that suite are executed. The test manager does not support executing
 * individual tests. However, the report produced by the test manager does indicate the status
 * of each test separately.
 *
 * This test manager is not a complete test program. Users of this framework will need to write
 * a test program that suites their needs. Minimally, it should register all required test
 * suites and then execute them. The test program may have other requirements and support
 * additional features according to the user's wishes.
 *
 * This is a work in progress. The following is a (partial) list of enhancements that might
 * be made in the future.
 *
 * + Currently `execute_suites` executes all registered suites. It would be nice to have the
 *   option of executing a single suite so that test programs can present that option to the
 *   user.
 *
 * + Other output formats besides XML should be supported.
 *
 * + Currently, if an exception is thrown in a test, the <Test> element is closed by the
 *   destructor of `UnitTest` before the <Exception> element is written. This is not ideal. It
 *   means users must know that the exception report pertains to the previous element. It would
 *   be better if the exception report was inside the <Test> element for the test that threw the
 *   exception.
 *
 * + Currently, `test_status` returns an indication of if every test in every suite has passed
 *   or not. It would be nice to have a more detailed status that indicates the status of each
 *   suite or test separately.
 *
 * + The output document needs to be abstracted in some way so that its state can be managed
 *   separately (for example, indentation depth, end-of-line location, etc.). Right now there
 *   is too much formatting code in the test manager itself.
 *
 * + A test program skeleton could be provided that offers services such as, for example, a GUI
 *   or TUI, or interface with a database. Users could copy the skeleton and customize it to
 *   suit their needs.
 */
namespace UnitTestManager {

    /*!
     * \brief The type of functions that can be test suites.
     *
     * A "test suite" is just a function taking no parameters and returning bool. In the current
     * version of this framework, suite functions should always return `true`. Test failures are
     * recorded by the framework (provided the framework's macros are used), and the result is
     * made available via the `test_status` function.
     *
     * If a suite function returns `false` a <BadReturn> element is written to the test
     * report, but it is otherwise ignored. In particular, no tests are considered to have
     * failed. Is this the right behavior?
     *
     * When writing a suite function, a typical approach is to simply call additional functions
     * with one additional function for each test case. The suite function is then little more
     * than a sequence of function calls. However, it is up to the user of this framework to
     * decide how test suites are organized.
     */
    typedef bool (*suite_t)();

    /*!
     * \brief Register a test suite.
     *
     * Test programs must call this function for every test suite the framework is intended to
     * know about. This function does not execute any tests.
     *
     * \param suite_function A pointer to the function to execute when executing the suite.
     * \param suite_title The name of the suite. This is used to identify the suite.
     */
    void register_suite(suite_t suite_function, const std::string& suite_title);

    /*!
     * \brief Execute all registered test suites.
     *
     * This function executes all registered test suites. It produces a report in XML format
     * that is writing on the given output stream.
     *
     * \param test_output The output stream to write the report to.
     * \param title The title of the test program as a whole. This is used to identify the test
     * program.
     */
    void execute_suites(std::ostream& test_output, const std::string& title);

    /*!
     * \brief Report a test failure.
     *
     * This function is used by the UNIT_FAIL macro to report a test failure. It is not intended
     * to be called directly, however doing so is acceptable and may be necessary in special
     * cases.
     *
     * \param file_name The name of the source file where the failure occurred.
     * \param line_number The line number in the source file where the failure occurred.
     * \param description A description of the failure.
     */
    void report_failure(const char* file_name, int line_number, const char* description);

    /*!
     * \brief Retrieve the overall status of executing all suites.
     *
     * \returns EXIT_SUCCESS or EXIT_FAILURE depending on whether all suites passed or not.
     */
    int test_status();

    //! Class to announce the beginning of a test.
    /*!
     * Place an instance of this class at the beginning of each test function. The constructor
     * will write a <Test> element to the report. The destructor will write a closing </Test>
     * element to the report. This is a convenience class that makes it easy to write the
     * beginning and end of each test.
     */
    class UnitTest {
      public:
        UnitTest(const std::string& test_name);
        ~UnitTest();
    };

    //! This exception can be thrown by a test to indicate test failure.
    /*!
     * Test programs can throw this exception to abort the execution of the entire test suite.
     * This is useful when a test discovers a fatal error that prevents further testing inside
     * that suite from being useful. The UNIT_RAISE macro facilitates throwing this exception.
     */
    class UnitException : public std::logic_error {
      public:
        UnitException(const std::string& message) : std::logic_error(message)
        {
        }
    };

} // namespace UnitTestManager

/*!
 * \brief Macro to report a test failure.
 *
 * This macro reports a test failure at the location where it is invoked. It causes a <Failure>
 * element to be written to the report. The description is used as the content of the <Failure>
 * element.
 */
#define UNIT_FAIL(description) UnitTestManager::report_failure(__FILE__, __LINE__, description)

/*!
 * \brief Macro to throw a UnitException.
 *
 * This causes all further execution of the current test suite to be aborted. An <Exception>
 * element is written to the report. The description is used as the content of the <Exception>
 * element.
 */
#define UNIT_RAISE(description) throw UnitTestManager::UnitException(description)

/*!
 * \brief Macro to check a test condition.
 *
 * The given expression is evaluated (once). It must have a type that is convertible to bool. If
 * the expression evaluates to false, a test failure is reported at the location where this
 * macro is invoked. The expression is also used as the content of the <Failure> element
 */
#define UNIT_CHECK(expression)                                                                     \
    if (!(expression))                                                                             \
    UNIT_FAIL(#expression)
