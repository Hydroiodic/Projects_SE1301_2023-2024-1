#pragma once

#include <QString>
#include <QPair>
#include <string>
#include <stack>
#include "QBasicVar.h"
#include "commands.h"
#include "exceptions.h"

namespace expressions {
	const std::vector<std::vector<QString>> ops = {
		{ "<", ">", "=" }, 
		{ "+", "-" }, 
		{ "*", "/", "MOD" },
		{ "**" }
	};

	int priority(const QString& op);

	bool checkChar(char c);

	bool checkSingleCharOperator(char c);
}

namespace expressions {

	// represent the state of the expression
	enum STATE_EXP {
		ERROR, ACTIVE, INACTIVE
	};

	// node of expression tree
	struct node
	{
		bool if_node = false;
		bool is_op = false;

		QString op = "";
		QBasicVar* var = nullptr;

		node* left = nullptr, * right = nullptr;

		// create an op node
		node(const QString& op, node* l = nullptr, node* r = nullptr)
			: op(op), left(l), right(r), is_op(true) {}
		// create a var node
		node(QBasicVar* var, node* l = nullptr, node* r = nullptr)
			: var(var), left(l), right(r) {}
		~node() {
			// TODO: deleting a variable shouldn't be done here
			if (var) delete var;
			if (left) delete left;
			if (right) delete right;
		}
	};

	class Expression
	{
	protected:
		// variables and functions to build a tree
		node* root = nullptr;
		node* createSubTree(const QString& exp) const;

		// functions for createSubTree to use
		void pushOperator(std::stack<node*>& opera,
			std::stack<node*>& varia, node* op) const;
		void releaseMemBeforeThrow(std::stack<node*>& opera,
			std::stack<node*>& varia, node* op) const;
		QPair<bool, QString> classifyOperator(const QString& exp,
			int& i, const QString& var) const;

		// functions for buildTree to use
		node* emptyStack(std::stack<node*>& opera, std::stack<node*>& varia) const;

		// functions for getNodeExpTree to use
		QString getNodeExpTree(node* r, int depth) const;

	public:
		Expression(const QString& exp);
		~Expression();

	/************ Below are Expression virtual functions ************/

	protected:
		// start to build the internal tree
		virtual void buildTree(const QString& exp);

	public:
		// get the expression tree that the expression holds
		virtual QString getExpTree() const;
	};
}

namespace expressions {

	class ram_expression : public Expression {
	public:
		ram_expression(const QString& str) : Expression(str) {}

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class let_expression : public Expression {
	public:
		let_expression(const QString& str) : Expression(str) {}

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class print_expression : public Expression {
	public:
		print_expression(const QString& str) : Expression(str) {}

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class input_expression : public Expression {
	public:
		input_expression(const QString& str) : Expression(str) {}

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class goto_expression : public Expression {
	public:
		goto_expression(const QString& str) : Expression(str) {}

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class if_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		if_expression(const QString& str) : Expression(str) {}

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class end_expression : public Expression {
	public:
		end_expression(const QString& str) : Expression(str) {}

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};
}
