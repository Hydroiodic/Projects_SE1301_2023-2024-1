#pragma once

#include <QString>
#include <QObject>
#include <QPair>
#include <string>
#include <stack>
#include "QBasicVar.h"
#include "QBasicVarList.h"
#include "commands.h"
#include "exceptions.h"

class QBasic;

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
		ERROR, ACTIVE
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
			// deleting the left tree and the right tree recursively
			if (left) delete left;
			if (right) delete right;
		}
	};

	class Expression : public QObject
	{
		Q_OBJECT

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

		// save the executed count of the expression
		int executed_count = 0;

	private:
		// a variable to save the list of variables
		QBasicVarList* variables_list = nullptr;

	public:
		Expression(QBasicVarList* l) : variables_list(l) {};
		~Expression();

		/***************** Below are Expression signals *****************/

	signals:
		void appendOutputText(const QString&);
		void inputVar(QBasicVar*);

		/************ Below are Expression virtual functions ************/

	protected:
		// start to build the internal tree
		virtual void buildTree(const QString& exp) = 0;

	public:
		// get the expression tree that the expression holds
		virtual QString getExpTree() const = 0;

		// to execute the expression
		virtual int executeExpression() = 0;

		// get a string represent the executed count of the expression
		virtual QString getExecutedCount() const;
	};
}

namespace expressions {

	class ram_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		ram_expression(const QString& str, QBasicVarList* l)
			: Expression(l) {
			buildTree(str);
		}

		// get the expression tree that the expression holds
		QString getExpTree() const;

		// to execute the expression
		int executeExpression();
	};

	class let_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		let_expression(const QString& str, QBasicVarList* l)
			: Expression(l) {
			buildTree(str);
		}

		// get the expression tree that the expression holds
		QString getExpTree() const;

		// to execute the expression
		int executeExpression();
	};

	class print_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		print_expression(const QString& str, QBasicVarList* l)
			: Expression(l) {
			buildTree(str);
		}

		// get the expression tree that the expression holds
		QString getExpTree() const;

		// to execute the expression
		int executeExpression();
	};

	class input_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		input_expression(const QString& str, QBasicVarList* l)
			: Expression(l) {
			buildTree(str);
		}

		// get the expression tree that the expression holds
		QString getExpTree() const;

		// to execute the expression
		int executeExpression();
	};

	class goto_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		goto_expression(const QString& str, QBasicVarList* l)
			: Expression(l) {
			buildTree(str);
		}

		// get the expression tree that the expression holds
		QString getExpTree() const;

		// to execute the expression
		int executeExpression();
	};

	class if_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

		// executed counts of if_expression are different from other expressions
		int true_count = 0;
		int false_count = 0;

	public:
		if_expression(const QString& str, QBasicVarList* l)
			: Expression(l), true_count(0), false_count(0) {
			buildTree(str);
		}

		// get the expression tree that the expression holds
		QString getExpTree() const;

		// to execute the expression
		int executeExpression();

		// rewrite getExecutedCount function
		QString getExecutedCount() const;
	};

	class end_expression : public Expression {
	protected:
		// start to build the internal tree
		void buildTree(const QString& exp);

	public:
		end_expression(const QString& str, QBasicVarList* l)
			: Expression(l) {
			buildTree(str);
		}

		// get the expression tree that the expression holds
		QString getExpTree() const;

		// to execute the expression
		int executeExpression();
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

namespace expressions {

	// these functions will calculate the expression
	//     ATTENTION! these functions won't check the correctness of the expression!

	int calculateCalcuExp(node* r);
	int calculateCalcuExp(const QString& op, node* left, node* right);
	bool calculateCmpExp(node* r);
}
