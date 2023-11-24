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

	// the type of the node
	enum TYPE_NODE {
		OP, VAR, CON, NONE
	};

	// node of expression tree
	struct node
	{
		TYPE_NODE type = NONE;

		QString op = "";
		QBasicVar* var = nullptr;
		int constant = 0x7fffffff;

		node* left = nullptr, * right = nullptr;

		// create an op node
		node(const QString& op, node* l = nullptr, node* r = nullptr)
			: op(op), left(l), right(r), type(OP) {}
		// create a var node
		node(QBasicVar* var, node* l = nullptr, node* r = nullptr)
			: var(var), left(l), right(r), type(VAR) {}
		// create a constant node
		node(int number, node* l = nullptr, node* r = nullptr)
			: constant(number), left(l), right(r), type(CON) {}

		QString getContent() const {
			switch (type) {
			case OP:
				return op;
			case VAR:
				return var->fetchName();
			case CON:
				return QString::number(constant);
			default:
				throw exceptions::impossible_arrival();
			}
		}

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
		void pushVariable(std::stack<node*>& opera,
			std::stack<node*>& varia, QString& var) const;
		void releaseMemBeforeThrow(std::stack<node*>& opera,
			std::stack<node*>& varia, node* op) const;
		QPair<bool, QString> classifyOperator(const QString& exp,
			int& i, const QString& var) const;

		// functions for buildTree to use
		node* emptyStack(std::stack<node*>& opera, std::stack<node*>& varia) const;

		// functions for getNodeExpTree to use
		QString getNodeExpTree(node* r, int depth) const;

	public:
		Expression() {};
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
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		ram_expression(const QString& str) { buildTree(str); }

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class let_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		let_expression(const QString& str) { buildTree(str); }

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class print_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		print_expression(const QString& str) { buildTree(str); }

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class input_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		input_expression(const QString& str) { buildTree(str); }

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class goto_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		goto_expression(const QString& str) { buildTree(str); }

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class if_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		if_expression(const QString& str) { buildTree(str); }

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};

	class end_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		end_expression(const QString& str) { buildTree(str); }

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};
}

namespace expressions {

	// the functions below only check the main operator
	//     whether the expression in correct isn't checked

	bool isCmpExp(const QString& str);
	bool isAssignExp(const QString& str);
	bool isCalcuExp(const QString& str);
	bool isNumber(const QString& str);
	bool isVariable(const QString& str);
}
