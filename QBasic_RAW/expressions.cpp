// expressions.cpp
// namespace expressions defined here, including nearly all operations on expressions

// ATTENTION!
// if the input is error, exceptions should only be thrown during the process of
//     "buildTree", which means as long as the tree is built, the expression must
//     be all right or broken shown by the structure exp_pack.

// and when the codes begin to run, the exceptions thrown should only relate to runtime
//     error such as "unassigned_variable" or "no_end_encountered".

#include "QBasic.h"
#include "expressions.h"

namespace expressions {
	int priority(const QString& op) {
		for (int i = 0; i < ops.size(); ++i) {
			for (int j = 0; j < ops[i].size(); ++j) {
				if (ops[i][j] == op) return i;
			}
		}

		// op not found
		return -1;
	}

	bool checkChar(char c) {
		return c <= 'Z' && c >= 'A' ||
			c <= 'z' && c >= 'a' ||
			c <= '9' && c >= '0' ||
			c == '_';
	}

	bool checkSingleCharOperator(char c) {
		return c == '+' || c == '-' ||
			c == '*' || c == '/' ||
			c == '>' || c == '<' || c == '=';
	}
}

namespace expressions {

	/**************** Below are Expression functions ****************/

	Expression::~Expression() {
		if (!root) return;
		delete root;
	}

	node* Expression::createSubTree(const QString& str) const {
		// trim the string
		QString exp = str.trimmed();
		if (exp == "") return nullptr;

		// operator stack and variable stack
		std::stack<node*> opera;
		std::stack<node*> varia;

		// save the name of the current variable
		QString var = "";

		// the first number is a negative number
		if (exp[0] == '-') {
			node* seat = new node(0);
			seat->is_empty_in_tree = true;
			varia.push(seat);
		}

		// begin scan operation
		for (int i = 0; i < exp.length(); ++i) {
			// if the current charactor is ' '
			if (exp[i] == ' ' || exp[i] == '\n' || exp[i] == '\t') {
				if (var.length() != 0) {
					pushVariable(opera, varia, var);
				}
				continue;
			}

			// deal with the appearance of ()
			if (exp[i] == '(') {
				// illegal format
				if (var != "") {
					releaseMemBeforeThrow(opera, varia, nullptr);
					throw exceptions::expression_error();
				}

				bool findBracket = false;
				for (int j = i; j < exp.length(); ++j) {
					if (exp[j] == ')') {
						findBracket = true;
						node* bracket = nullptr;
						try {
							bracket = createSubTree(exp.mid(i + 1, j - i - 1));
						}
						catch (exceptions::expression_error e) {
							releaseMemBeforeThrow(opera, varia, nullptr);
							throw e;
						}
						catch (exceptions::illegal_variable_name e) {
							releaseMemBeforeThrow(opera, varia, nullptr);
							throw e;
						}
						catch (...) {
							releaseMemBeforeThrow(opera, varia, nullptr);
							throw exceptions::unknown_error_internal();
						}

						varia.push(bracket);
						i = j;
						break;
					}
				}

				if (!findBracket) {
					releaseMemBeforeThrow(opera, varia, nullptr);
					throw exceptions::expression_error();
				}
				continue;
			}

			// in this function, only i may be changed at most
			QPair<bool, QString> operatorResult = classifyOperator(exp, i, var);
			if (operatorResult.first) {
				// push variable into the variable stack
				pushVariable(opera, varia, var);

				// push the operator into the operator stack
				node* op_to_push = new node(operatorResult.second);
				pushOperator(opera, varia, op_to_push);
				continue;
			}

			// charactor to add into the name of the variable
			if (checkChar(exp.toStdString()[i])) {
				var += exp[i];
				continue;
			}

			// the charactor is unknown or illegal
			releaseMemBeforeThrow(opera, varia, nullptr);
			throw exceptions::expression_error();
		}

		if (var.length() != 0) {
			pushVariable(opera, varia, var);
		}

		// complete tree building by emptying the two stacks
		return emptyStack(opera, varia);
	}

	QPair<bool, QString> Expression::classifyOperator(
		const QString& exp, int& i, const QString& var) const {

		// check "MOD" operator
		if (var == "" && i < exp.length() - 2 && exp[i] == 'M' &&
			exp[i + 1] == 'O' && exp[i + 2] == 'D') {
			i += 2;
			return qMakePair(true, "MOD");
		}

		// check "**" operator
		if (i < exp.length() - 1 && exp[i] == '*' && exp[i + 1] == '*') {
			// jump out of the operator
			++i;
			return qMakePair(true, "**");
		}

		// check other operators
		if (checkSingleCharOperator(exp.toStdString()[i])) {
			return qMakePair(true, QString(exp.toStdString()[i]));
		}

		return qMakePair(false, "");
	}

	void Expression::releaseMemBeforeThrow(std::stack<node*>& opera,
		std::stack<node*>& varia, node* op) const {
		// delete opera
		while (!opera.empty()) {
			node* top = opera.top();
			delete top;
			opera.pop();
		}

		// delete varia
		while (!varia.empty()) {
			node* top = varia.top();
			delete top;
			varia.pop();
		}

		// delete op
		if (op) delete op;
	}

	void Expression::pushOperator(std::stack<node*>& opera,
		std::stack<node*>& varia, node* op) const {
		if (opera.empty()) {
			opera.push(op);
			return;
		}

		node* op_top = opera.top();
		int priority_top = priority(op_top->op);
		int priority_cur = priority(op->op);

		// the type of an operator is programmed wrong by the programmer
		if (priority_top == -1 || priority_cur == -1) {
			releaseMemBeforeThrow(opera, varia, op);
			throw exceptions::operator_type_unknown_internal();
		}

		// pop the operator stack
		if (priority_top > priority_cur ||
			op->op != "**" && priority_top == priority_cur) {

			// the number of variables shouldn't be lower than 2
			if (varia.size() < 2) {
				releaseMemBeforeThrow(opera, varia, op);
				throw exceptions::expression_error();
			}

			// combine two variables with an operator
			opera.pop();
			op_top->right = varia.top();
			varia.pop();
			op_top->left = varia.top();
			varia.pop();

			// push back the newly constructed clause
			varia.push(op_top);
			pushOperator(opera, varia, op);
		}
		else {
			opera.push(op);
		}
	}

	void Expression::pushVariable(std::stack<node*>& opera,
		std::stack<node*>& varia, QString& var) const {
		// the string is empty
		if (!var.length()) return;

		// if the string represents a constant
		if (var[0] <= '9' && var[0] >= '0' || var[0] == '-') {
			// the string is not a number, error occurs
			if (!isNumber(var)) {
				releaseMemBeforeThrow(opera, varia, nullptr);
				throw exceptions::illegal_variable_name();
			}

			node* num_to_push = new node(var.toInt());
			varia.push(num_to_push);
		}
		// else the string represents a variable
		else {
			QBasicVar* variable = variables_list->fetchVar(var);
			node* var_to_push = new node(variable);

			// check the name of the variable
			if (!variable->checkVar()) {
				releaseMemBeforeThrow(opera, varia, var_to_push);
				throw exceptions::illegal_variable_name();
			}

			varia.push(var_to_push);
		}

		// clear the variable
		var = "";
	}

	node* Expression::emptyStack(std::stack<node*>& opera,
		std::stack<node*>& varia) const {
		if (opera.empty()) {
			// the final expression tree
			if (varia.size() != 1) {
				releaseMemBeforeThrow(opera, varia, nullptr);
				throw exceptions::expression_error();
			}

			node* tree = varia.top();
			varia.pop();
			return tree;
		}

		node* op_top = opera.top();

		// the number of variables shouldn't be lower than 2
		if (varia.size() < 2) {
			releaseMemBeforeThrow(opera, varia, nullptr);
			throw exceptions::expression_error();
		}

		// combine two variables with an operator
		opera.pop();
		op_top->right = varia.top();
		varia.pop();
		op_top->left = varia.top();
		varia.pop();

		// push back the newly constructed clause
		varia.push(op_top);

		// recursion
		return emptyStack(opera, varia);
	}

	QString Expression::getNodeExpTree(node* r, int depth) const {
		if (!root) {
			throw exceptions::illegal_use_of_variables_internal();
		}

		// format the string
		QString ans = "";
		for (int i = 0; i < depth; ++i) {
			ans += "\t";
		}

		// add the name of variable or operator
		ans += r->getContent();
		ans += "\n";

		// recursion
		ans += r->left ? getNodeExpTree(r->left, depth + 1) : "";
		ans += r->right ? getNodeExpTree(r->right, depth + 1) : "";

		// if the line is empty, abort it
		if (ans.trimmed() == "") {
			return "";
		}

		return ans;
	}

	QString Expression::getExecutedCount() const {
		return QString::number(executed_count);
	}
}

namespace expressions {

	/**************** Below are getExpTree functions ****************/

	QString ram_expression::getExpTree() const {
		// expression isn't created
		if (!root) { return "Error\n"; }

		return "REM " + getExecutedCount() + "\n" + getNodeExpTree(root, 1);
	}

	QString let_expression::getExpTree() const {
		// expression isn't created
		if (!root) { return "Error\n"; }

		// get the name of the left variable
		QString left_variable = getNodeExpTree(root->left, 1);

		return "LET = " + getExecutedCount() + "\n" +
			left_variable.left(left_variable.size() - 1) + " " + 
			QString::number(root->left->var->fetchCount()) + "\n" + 
			getNodeExpTree(root->right, 1);
	}

	QString print_expression::getExpTree() const {
		// expression isn't created
		if (!root) { return "Error\n"; }

		return "PRINT " + getExecutedCount() + "\n" + getNodeExpTree(root, 1);
	}

	QString input_expression::getExpTree() const {
		// expression isn't created
		if (!root) { return "Error\n"; }

		return "INPUT " + getExecutedCount() + "\n" + getNodeExpTree(root, 1);
	}

	QString goto_expression::getExpTree() const {
		// expression isn't created
		if (!root) { return "Error\n"; }

		return "GOTO " + getExecutedCount() + "\n" + getNodeExpTree(root, 1);
	}

	QString if_expression::getExpTree() const {
		// expression isn't created
		if (!root) { return "Error\n"; }

		return "IF THEN " + getExecutedCount() + "\n" + 
			getNodeExpTree(root->left->left, 1) +
			"\t" + root->left->op + "\n" + 
			getNodeExpTree(root->left->right, 1) + 
			getNodeExpTree(root->right, 1);
	}

	QString end_expression::getExpTree() const {
		return "END\n";
	}

	/***************** Below are buildTree functions ****************/

	void ram_expression::buildTree(const QString& exp) {
		// an expression object should be created only once
		if (root) {
			throw exceptions::repeatly_build_one_expression_object();
		}
		
		// use the QString op to save the content of comments
		root = new node(exp);
	}

	void let_expression::buildTree(const QString& exp) {
		if (root) {
			throw exceptions::repeatly_build_one_expression_object();
		}

		// the expression must be an assignment clause
		if (!isAssignExp(exp)) {
			throw exceptions::expression_error();
		}

		// split the string into 2 parts
		QStringList list = exp.split("=");
		if (list.size() != 2) {
			throw exceptions::unknown_error_internal();
		}

		// create the root of the tree
		root = new node("=", createSubTree(list[0]), createSubTree(list[1]));
	}

	void print_expression::buildTree(const QString& exp) {
		if (root) {
			throw exceptions::repeatly_build_one_expression_object();
		}

		// the expression must be an assignment clause
		if (!isCalcuExp(exp)) {
			throw exceptions::expression_error();
		}

		root = createSubTree(exp);
	}

	void input_expression::buildTree(const QString& exp) {
		if (root) {
			throw exceptions::repeatly_build_one_expression_object();
		}

		// the name of the variable must be legal
		if (!isVariable(exp)) {
			throw exceptions::illegal_variable_name();
		}

		root = createSubTree(exp);
	}

	void goto_expression::buildTree(const QString& exp) {
		if (root) {
			throw exceptions::repeatly_build_one_expression_object();
		}

		// the name of the variable must be legal
		if (!isNumber(exp)) {
			throw exceptions::expression_error();
		}

		root = createSubTree(exp);
	}

	void if_expression::buildTree(const QString& exp) {
		// an expression object should be created only once
		if (root) {
			throw exceptions::repeatly_build_one_expression_object();
		}

		// find "THEN"
		int end_of_first_expression = -1;
		for (int i = 0; i < exp.length() - 3; ++i) {
			if (exp[i] == ' ' && exp[i + 1] == 'T' && exp[i + 2] == 'H' &&
				exp[i + 3] == 'E' && exp[i + 4] == 'N' && exp[i + 5] == ' ') {
				end_of_first_expression = i;
				break;
			}
		}

		// if "THEN" not found
		if (end_of_first_expression == -1) {
			throw exceptions::expression_error();
		}
		
		// split the two expressions
		QString exp_1 = exp.mid(0, end_of_first_expression);
		QString exp_2 = exp.mid(end_of_first_expression + 5);

		// check the format of the two parts of the string
		if (!(isCmpExp(exp_1) && isNumber(exp_2))) {
			throw exceptions::expression_error();
		}

		// find the comparing operator
		int pos_of_comparing_op = -1;
		for (int i = 0; i < exp_1.length(); ++i) {
			if (exp_1[i] == '>' || exp_1[i] == '<' || exp_1[i] == '=') {
				pos_of_comparing_op = i;
				break;
			}
		}

		if (pos_of_comparing_op == -1) {
			throw exceptions::unknown_error_internal();
		}

		// using recursive method to create the tree
		node* left_node = new node(QString(exp_1[pos_of_comparing_op]), 
			createSubTree(exp_1.mid(0, pos_of_comparing_op)), 
			createSubTree(exp_1.mid(pos_of_comparing_op + 1)));
		root = new node("THEN", left_node, createSubTree(exp_2));
	}

	void end_expression::buildTree(const QString& exp) {
		/* nothing needed */
	}

	/************* Below are executeExpression functions ************/

	int ram_expression::executeExpression() {
		// add executed_count
		++executed_count;

		return -1;
	}

	int let_expression::executeExpression() {
		// if the expression are not loaded yet
		if (!root) {
			throw exceptions::expression_not_loaded();
		}

		// the format of this expression is incorrect
		if (!root->left || root->left->type != VAR) {
			throw exceptions::expression_error();
		}

		// assign the variable
		QBasicVar* var_to_assign = root->left->var;
		var_to_assign->assign(calculateCalcuExp(root->right));

		// add executed_count
		++executed_count;

		return -1;
	}

	int print_expression::executeExpression() {
		// if the expression are not loaded yet
		if (!root) {
			throw exceptions::expression_not_loaded();
		}

		// get the value and print them
		int ans = calculateCalcuExp(root);
		emit appendOutputText(QString::number(ans) + "\n");

		// add executed_count
		++executed_count;

		return -1;
	}

	int input_expression::executeExpression() {
		// if the expression are not loaded yet
		if (!root) {
			throw exceptions::expression_not_loaded();
		}

		// the type of root must be VAR
		if (root->type != VAR) {
			throw exceptions::command_type_unmatch_internal();
		}

		// be ready for input
		emit inputVar(root->var);

		// add executed_count
		++executed_count;

		return -1;
	}

	int goto_expression::executeExpression() {
		// if the expression are not loaded yet
		if (!root) {
			throw exceptions::expression_not_loaded();
		}

		// if the pointer to "QBasicCode" is nullptr
		if (!code) {
			throw exceptions::unknown_error_internal();
		}

		// up to here, the expression should be right

		// find the index of the next line

		// the expression after GOTO should be a number
		// here use calculateCalcuExp to deal with negative numbers
		int next_index = code->getCodeNo(calculateCalcuExp(root));

		// if the destination to jump is non-existent
		if (next_index == -1) {
			throw exceptions::jump_to_nonexistent_line();
		}

		// add executed_count
		++executed_count;

		return next_index;
	}

	int if_expression::executeExpression() {
		// if the expression are not loaded yet
		if (!root) {
			throw exceptions::expression_not_loaded();
		}

		// if the pointer to "QBasicCode" is nullptr
		if (!code) {
			throw exceptions::unknown_error_internal();
		}

		// up to here, the expression should be right

		// execute the expression
		if (calculateCmpExp(root->left)) {
			// add true_count
			++true_count;
			
			// find the index of the next line
			
			// the expression after GOTO should be a number
			// here use calculateCalcuExp to deal with negative numbers
			int next_index = code->getCodeNo(
				calculateCalcuExp(root->right));

			// if the destination to jump is non-existent
			if (next_index == -1) {
				throw exceptions::jump_to_nonexistent_line();
			}

			return next_index;
		}

		// add false_count
		++false_count;

		// the condition doesn't match
		return -1;
	}

	int end_expression::executeExpression() {
		return 0x7fffffff;
	}

	QString if_expression::getExecutedCount() const {
		return QString::number(true_count) + " " + QString::number(false_count);
	}
}

namespace expressions {

	// the functions below only check the main operator
	//     whether the expression in correct isn't checked

	bool isCmpExp(const QString& str) {
		// calculate for the count of those comparing operators
		int compare_ops_count = 0;
		for (int i = 0; i < str.length(); ++i) {
			if (str[i] == '>' || str[i] == '<' || str[i] == '=') {
				++compare_ops_count;
			}
		}

		// the number of the comparing operators should be 1
		return compare_ops_count == 1;
	}

	bool isAssignExp(const QString& str) {
		// save the the name of the left variable
		QStringList split_str = str.trimmed().split("=");

		// the expression should be split into only 2 parts
		if (split_str.size() != 2) {
			return false;
		}

		// the left part of the expression must be a variable
		split_str[0] = split_str[0].trimmed();
		if (!isVariable(split_str[0])) {
			return false;
		}

		return isCalcuExp(split_str[1]);
	}

	bool isCalcuExp(const QString& str) {
		QString trimmed_str = str.trimmed();

		if (trimmed_str == "") return false;

		for (int i = 0; i < trimmed_str.length(); ++i) {
			// in calculation expression, there shouldn't be > < =
			if (trimmed_str[i] == '>' || trimmed_str[i] == '<' || 
				trimmed_str[i] == '=') {
				return false;
			}
		}
		return true;
	}

	bool isNumber(const QString& str) {
		QString trimmed_str = str.trimmed();

		if (trimmed_str == "") return false;

		// the first char maybe '-' or number
		if (trimmed_str[0] == '-') {
			trimmed_str = trimmed_str.mid(1).trimmed();
		}

		if (trimmed_str == "") return false;

		// the rest of the char must be number
		for (int i = 0; i < trimmed_str.length(); ++i) {
			if (!(trimmed_str[i] <= '9' && trimmed_str[i] >= '0')) {
				return false;
			}
		}

		return true;
	}

	bool isVariable(const QString& str) {
		QString trimmed_str = str.trimmed();

		// the attributes of a variable
		if (trimmed_str == "") return false;
		if (trimmed_str[0] >= '0' && trimmed_str[0] <= '9') return false;

		// the codes below are commented because whether the name of the variable
		//     is a keyword will be checked in the function "pushVariable". To make 
		//     the error info more clear, the codes below are commented. However, 
		//     this is against the original intention of this function.

		// check key_words
		/*for (int i = 0; i < variables::key_words.size(); ++i) {
			if (variables::key_words[i] == trimmed_str) return false;
		}*/

		// check char
		for (int i = 0; i < trimmed_str.length(); ++i) {
			if (!checkChar(trimmed_str.toStdString()[i])) return false;
		}

		return true;
	}
}

namespace expressions {

	// these functions will calculate the expression
	//     ATTENTION! these functions won't check the correctness of the expression!

	int calculateCalcuExp(node* r) {
		// to calculate this expression requires the pointer exists
		if (!r) {
			throw exceptions::unknown_error_internal();
		}

		// deal with different type of the node
		switch (r->type) {
		case VAR:
			return r->var->fetchValue();

		case CON:
			return r->constant;

		case OP:
			return calculateCalcuExp(r->op, r->left, r->right);
			break;

		default:
			throw exceptions::impossible_arrival();
		}
	}

	int calculateCalcuExp(const QString& op, node* left, node* right) {
		// all calculation operators
		static const std::vector<QString> operators = {
			"+", "-", "*", "/", "MOD", "**"
		};

		// find the operator
		int i = 0;
		for (; i < operators.size(); ++i) {
			if (operators[i] == op) break;
		}

		// the operator is not in the vector above
		if (i == operators.size()) {
			throw exceptions::unknown_error_internal();
		}

		// calculate on the left tree and on the right tree
		int left_r = calculateCalcuExp(left);
		int right_r = calculateCalcuExp(right);
		int left_s = left_r >= 0 ? 0 : 1;
		int right_s = right_r >= 0 ? 0 : 1;

		// deal with different operators
		switch (i) {
		case 0: return left_r + right_r;

		case 1: return left_r - right_r;

		case 2: return left_r * right_r;

		case 3:
			// every number shouldn't be divided by zero
			if (!right_r) throw::exceptions::divided_by_zero();
			return left_r / right_r;

		case 4: return left_r % right_r + 
			(left_s - right_s) * qAbs(right_r);

		case 5: return qPow(left_r, right_r);

		default: throw exceptions::impossible_arrival();
		}
	}
	  
	bool calculateCmpExp(node* r) {
		// all comparing operators
		static const std::vector<QString> operators = {
			">", "<", "="
		};

		// if the type of the node is incorrect
		if (r->type != OP) {
			throw exceptions::unknown_error_internal();
		}

		// find the operator
		int i = 0;
		for (; i < operators.size(); ++i) {
			if (operators[i] == r->op) break;
		}

		// the operator is not in the vector above
		if (i == operators.size()) {
			throw exceptions::unknown_error_internal();
		}

		// deal with diffrent comparing operators
		switch (i) {
		case 0:
			return calculateCalcuExp(r->left) > calculateCalcuExp(r->right);

		case 1:
			return calculateCalcuExp(r->left) < calculateCalcuExp(r->right);

		case 2:
			return calculateCalcuExp(r->left) == calculateCalcuExp(r->right);

		default:
			throw exceptions::impossible_arrival();
		}
	}
}