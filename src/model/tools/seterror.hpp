/**
 * @file seterror.hpp
 * @author djw
 * @brief Tools/Set error
 * @date 2023-03-28
 *
 * @details Provides a tool function to set error.
 * @attention not same as assert, this function will not disabled by marco,
 * but will use abort() instead of throw exceptions when __RULEJIT_DISABLE_EXCEPTION
 * defined in marco.hpp.
 *
 * @see marco.hpp
 * @see myassert.hpp
 *
 * @par history
 * <table>
 * <tr><th>Author</th><th>Date</th><th>Changes</th></tr>
 * <tr><td>djw</td><td>2023-03-28</td><td>Initial version.</td></tr>
 * <tr><td>djw</td><td>2023-03-30</td><td>Add setError marco.</td></tr>
 * </table>
 */
#pragma once

#include <stdexcept>
#include <string>

[[noreturn]] inline void error(const std::string &msg) {
    // show msg in debug mode
    throw std::logic_error(msg);
}