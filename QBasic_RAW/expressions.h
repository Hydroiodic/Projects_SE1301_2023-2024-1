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
	private:
		commands::IMPL comm;

	private:
		// variables and functions to build a tree
		node* root = nullptr;
		void buildTree(const QString& exp);
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
		Expression(commands::IMPL t, const QString& exp);
		~Expression();

		// get the expression tree that the expression holds
		QString getExpTree() const;
	};
}
