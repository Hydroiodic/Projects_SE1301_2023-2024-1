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

	/**************** Below are expression functions ****************/

	Expression::Expression(const QString& exp) {
		buildTree(exp);
	}

	Expression::~Expression() {
		if (!root) return;
		delete root;
	}

	void Expression::buildTree(const QString& exp) {
		// an expression object should be created only once
		if (root) {
			throw exceptions::repeatly_build_one_expression_object();
		}

		// the expression isn't an "IF" expression
		root = createSubTree(exp);
	}

	node* Expression::createSubTree(const QString& exp) const {
		// operator stack and variable stack
		std::stack<node*> opera;
		std::stack<node*> varia;

		// save the name of the current variable
		QString var = "";

		// use a lambda expression to push a variable into the variable stack
		auto push_var = [&varia, &var]() -> void
			{
				if (var.length()) {
					QBasicVar* variable = new QBasicVar(nullptr, var);
					node* var_to_push = new node(variable);
					varia.push(var_to_push);
					var = "";
				}
			};

		// begin scan operation
		// TODO: "()" are not considered in this version
		for (int i = 0; i < exp.length(); ++i) {
			// in this function, only i may be changed at most
			QPair<bool, QString> operatorResult = classifyOperator(exp, i, var);
			if (operatorResult.first) {
				// push variable into the variable stack
				push_var();

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

			// if the current charactor is ' '
			if (exp[i] == ' ') {
				if (var.length() != 0) {
					push_var();
				}
				continue;
			}
		}

		if (var.length() != 0) {
			push_var();
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

		// delete this
		delete this;
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
		if (r->is_op) {
			ans += r->op;
		}
		else {
			ans += r->var->fetchName();
		}
		ans += "\n";

		// recursion
		ans += r->left ? getNodeExpTree(r->left, depth + 1) : "";
		ans += r->right ? getNodeExpTree(r->right, depth + 1) : "";

		return ans;
	}

	QString Expression::getExpTree() const {
		if (!root) {
			return "Error\n";
		}

		// this function won't be called independently here
		return getNodeExpTree(root, 0);
	}
}

namespace expressions {
	QString ram_expression::getExpTree() const {
		return "";
	}

	QString let_expression::getExpTree() const {
		// expression isn't created
		if (!root) { return "Error\n"; }

		return "LET =\n" + getNodeExpTree(root->left, 1) +
			getNodeExpTree(root->right, 1);
	}

	QString print_expression::getExpTree() const {
		return "";
	}

	QString input_expression::getExpTree() const {
		return "";
	}

	QString goto_expression::getExpTree() const {
		return "";
	}

	QString if_expression::getExpTree() const {
		return "";
	}

	QString end_expression::getExpTree() const {
		return "";
	}

	void if_expression::buildTree(const QString& exp) {
		// an expression object should be created only once
		if (root) {
			throw exceptions::repeatly_build_one_expression_object();
		}

		// split the expression into to part
		QStringList list = exp.split("THEN");

		// if the number of string parts isn't two, throw an error
		if (list.size() != 2)
			throw exceptions::expression_error();

		// TODO: check the condition clause

		// using recursive method to create the tree
		root = new node("THEN", createSubTree(list[0]), createSubTree(list[1]));
		root->if_node = true;
	}
}
