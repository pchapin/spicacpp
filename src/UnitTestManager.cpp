/*! \file    UnitTestManager.cpp
 *  \brief   Implementation of the unit test manager abstract object.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <spicacpp/UnitTestManager.hpp>

using namespace std;

namespace UnitTestManager {
    
    namespace {
        struct TestSuite {
            suite_t function;
            string  title;
        };
        
        vector<TestSuite> test_suites;
        ostream *output_pointer;
        bool     success = true;

        void output_head( ostream &test_output )
        {
            test_output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            test_output << "<TestOutput xmlns=\"http://www.kelseymountain.org/XML/UnitTestManager_0.0\"\n"
                        << "            xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                        << "            xsi:schemaLocation=\"http://www.kelseymountain.org/XML/UnitTestManager_0.0 UnitTestManager.xsd\">\n";

        }

        void output_metadata( std::ostream &test_output, const string &title )
        {
            test_output << "<MetaData>\n";
            test_output << "  <Title>" << title << "</Title>\n";
            test_output << "</MetaData>";
        }

        void output_tail( ostream &test_output )
        {
            test_output << "\n</TestOutput>\n";
        }
    }
    
    
    void register_suite( suite_t suite_function, const string &suite_title )
    {
        TestSuite new_suite = { suite_function, suite_title };
        test_suites.push_back( new_suite );
    }
    
    
    void execute_suites( ostream &test_output, const string &title )
    {
        output_pointer = &test_output;
        output_head( test_output );
        output_metadata( test_output, title );

        test_output << "\n<Results>";
        for( auto current_suite : test_suites ) {
            test_output << "\n  <SuiteResult title=\"" << current_suite.title << "\">";
            try {
                if( !current_suite.function( ) ) {
                    test_output << "\n    <BadReturn>false</BadReturn>";
                    success = false;
                }
            }
            catch( const UnitException &e ) {
                test_output << "\n    <Exception type=\"UnitException\">" << e.what( ) << "</Exception>";
                success = false;
            }
            catch( const std::exception &e ) {
                // TODO: Use RTTI to get the actual type name.
                test_output << "\n    <Exception type=\"std::exception\">" << e.what( ) << "</Exception>";
                success = false;
            }
            catch( ... ) {
                // TODO: Is there a way to extract information about the thrown exception?

                // Note that if the code under test throws an exception of a type that is
                // derived from one of the standard exceptions (which is considered good
                // practice), it will be caught by the case above and RTTI can be used to find
                // the actual exception name.
                test_output << "    <Exception type=\"UNKNOWN\">[no message]</Exception>\n";
                success = false;
            }
            test_output << "\n  </SuiteResult>\n";
        }
        test_output << "\n</Results>";
        output_tail( test_output );
    }
    
    
    void report_failure( const char *file_name, int line_number, const char *description )
    {
        *output_pointer << "\n      <Failure file=\"" << file_name << "\"\n"
                        <<   "               line=\"" << line_number << "\">" << description << "</Failure>";
        success = false;
    }


    int test_status( )
    {
        int return_code = EXIT_SUCCESS;
        if( !success ) return_code = EXIT_FAILURE;
        return return_code;
    }


    UnitTest::UnitTest( const std::string &test_name )
    {
        *output_pointer << "\n    <TestResult name=\"" << test_name << "\">";
    }


    UnitTest::~UnitTest( )
    {
        *output_pointer << "</TestResult>";
    }
}
