#pragma once

#include <exception>

namespace exceptions {

	/********** Below are exceptions caused by programmers **********/

	// if the command and its type don't match
	//     ATTENTION! this error is caused by the programmer
	class command_type_unmatch_internal : public std::exception {};

	// the type of an operator isn't found in the process of parsing
	//     ATTENTION! this error is caused by the programmer
	class operator_type_unknown_internal : public std::exception {};

	// the number of a line of code beyonds the total number of codes
	//     ATTENTION! this error is caused by the programmer
	class wrong_access_to_codes : public std::exception {};

	// one object of expression should be created only once
	//     ATTENTION! this error is caused by the programmer
	class repeatly_build_one_expression_object : public std::exception {};

	// the root of the expression is nullptr when execute the expression
	//     ATTENTION! this error is caused by the programmer
	class expression_not_loaded : public std::exception {};

	// unknown internal error, please check why this happened
	//     ATTENTION! this error is caused by the programmer
	class unknown_error_internal : public std::exception {};

	// all illegal use of variables will reach here
	//     ATTENTION! this error is caused by the programmer
	class illegal_use_of_variables_internal : public std::exception {};

	// never be thrown generally, only caused by memory corruption
	class impossible_arrival : public std::exception {};

	/************* Below are exceptions caused by users *************/

	// fetch a value when the variable is unassigned
	class unassigned_variable : public std::exception {};

	// the input number of line is out of range
	class line_number_out_of_range : public std::exception {};

	// the input command isn't correct
	class unknown_command : public std::exception {};

	// thrown if an error occurred when parsing expressions
	class expression_error : public std::exception {};

	// thrown if the name of the variable isn't legal
	class illegal_variable_name : public std::exception {};

	// the number of the lines executed reaches the max amount
	class stack_overflow : public std::exception {};
}
