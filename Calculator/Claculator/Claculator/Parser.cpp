#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include "Parser.h"
#include "Scanner.h"
#include "Node.h"
#include "Calc.h"
#include "Exception.h"
#include "DebugNew.h"

Parser::Parser(Scanner& scanner, Calc& calc) 
	: scanner_(scanner), tree_(0), calc_(calc), status_(STATUS_OK)
{
}

Parser::~Parser() 
{ 
	//delete tree_; 
}

STATUS Parser::Parse()
{
	tree_ = Expr();
	if (!scanner_.IsDone())
	{
		status_ = STATUS_ERROR;
	}

	return status_;
}

std::auto_ptr<Node> Parser::Expr()
{
	std::auto_ptr<Node> node = Term();
	EToken token = scanner_.Token();
	//if (token == TOKEN_PLUS)
	//{
	//	scanner_.Accept();
	//	Node* nodeRight = Expr();
	//	node = new AddNode(node, nodeRight);

	//}
	//else if (token == TOKEN_MINUS)
	//{
	//	scanner_.Accept();
	//	Node* nodeRight = Expr();
	//	node = new SubNode(node, nodeRight);
	//}

	if (token == TOKEN_PLUS || token == TOKEN_MINUS)
	{
		//Expr := Term {('+' | '-') Term }
		std::auto_ptr<MultipleNode> multiplenode(new SumNode(node));
		do 
		{
			scanner_.Accept();
			std::auto_ptr<Node> nexNode = Term();
			multiplenode->AppendChilds(nexNode, (token == TOKEN_PLUS));
			token = scanner_.Token();
		} while (token == TOKEN_PLUS || token == TOKEN_MINUS);

		node = multiplenode;
	}
	else if (token == TOKEN_ASSIGN)
	{
		//Expr := Term = Expr
		scanner_.Accept();
		std::auto_ptr<Node> nodeRight = Expr();
		if (node->IsLvalue())
		{
			node = std::auto_ptr<Node>(new AssignNode(node, nodeRight));
		}
		else
		{
			status_ = STATUS_ERROR;
			//std::cout <<"The left-hand side of an assignment must be a variable" <<std::endl;
			//Todo �׳��쳣
			throw SyntaxError("The left-hand side of an assignment must be a variable");
		}
	}

	return node;
}

std::auto_ptr<Node> Parser::Term()
{
	std::auto_ptr<Node> node = Factor();
	EToken token = scanner_.Token();
	//if (token == TOKEN_MULTIPLY)
	//{
	//	scanner_.Accept();
	//	Node* nodeRight = Term();
	//	node = new MultiplyNode(node, nodeRight);

	//}
	//else if (token == TOKEN_DIVIDE)
	//{
	//	scanner_.Accept();
	//	Node* nodeRight = Term();
	//	node = new DivideNode(node, nodeRight);
	//}

	if (token == TOKEN_MULTIPLY || token == TOKEN_DIVIDE)
	{
		//Term := Factor {('*' | '/') Factor }
		//MultipleNode* multiplenode = new ProductNode(node);
		std::auto_ptr<MultipleNode> multiplenode(new ProductNode(node));
		do 
		{
			scanner_.Accept();
			std::auto_ptr<Node> nexNode = Factor();
			multiplenode->AppendChilds(nexNode, (token == TOKEN_MULTIPLY));
			token = scanner_.Token();
		} while (token == TOKEN_MULTIPLY || token == TOKEN_DIVIDE);

		node = multiplenode;
	}


	return node;
}

std::auto_ptr<Node> Parser::Factor()
{
	std::auto_ptr<Node> node;
	EToken token = scanner_.Token();
	if (token == TOKEN_LPARENTHESIS)
	{
		scanner_.Accept();		//accept'('
		node = Expr();
		if (scanner_.Token() == TOKEN_RPARENTHESIS)
		{
			scanner_.Accept();	//accept ')'
		}
		else
		{
			status_ = STATUS_ERROR;
			//Todo: �׳��쳣
			//std::cout <<"Missing parenthesis "<<std::endl;
			throw SyntaxError("Missing parenthesis");
		}
	}
	else if (token == TOKEN_NUMBER)
	{
		node = std::auto_ptr<Node>(new NumberNode(scanner_.Number()));
		scanner_.Accept();
	}
	else if (token == TOKEN_IDENTIFIER)
	{
		std::string symbol = scanner_.GetSymbol();
		unsigned int id = calc_.FindSymbol(symbol);
		scanner_.Accept();

		if (scanner_.Token() == TOKEN_LPARENTHESIS)
		{
			scanner_.Accept();		//accept '('
			node = Expr();
			if (scanner_.Token() == TOKEN_RPARENTHESIS)
			{
				scanner_.Accept();	//accept ')'
				if (id != SymbolTable::IDNOTFOUNT && calc_.IsFunction(id))
				{
					node = std::auto_ptr<Node>(new FunctionNode(node, calc_.GetFunction(id)));
				}
				else
				{
					status_ = STATUS_ERROR;
					//std::cout <<"Unknown function" <<"\"" <<symbol <<"\"" <<std::endl;
					//char buf[256];
					//sprintf_s(buf, "Unknown function \"%s\".", symbol.c_str());
					std::ostringstream oss;
					oss<< "Unknown function \"" <<symbol <<"\"."; 
					throw SyntaxError(oss.str());

				}
			}
			else
			{
				status_ = STATUS_ERROR;
				//std::cout <<"Missing parenthesis in a function call." <<std::endl;
				throw SyntaxError("Missing parenthesis in a function call.");
			}
		}
		else
		{
			if (id == SymbolTable::IDNOTFOUNT)
			{
				id = calc_.AddSymbol(symbol);
			}
			node = std::auto_ptr<Node>(new VariableNode(id, calc_.GetStorage()));
		}

	}
	else if (token == TOKEN_MINUS)
	{
		scanner_.Accept(); //accept minus
		node = std::auto_ptr<Node>(new UMinusNode(Factor()));
	}
	else
	{
		status_ = STATUS_ERROR;
		//Todo: �׳��쳣
		//std::cout <<"Not a valid expression " <<std::endl;
		throw SyntaxError("Not a valid expression");
		//node = 0;
	}

	return node;
}

double Parser::Calculate() const
{
	assert(tree_.get() != 0);
	return tree_->Calc();
}