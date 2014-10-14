#include <vector>
#include <iostream>
using namespace std;

#include "DebugNew.h"
#include "ptr_vector.h"

class Test
{
public:
	Test()
	{
		cout <<"Test ..." <<endl;
	}
	Test(const Test& other)
	{
		cout <<"Copy Test ..." <<endl;
	}
	~Test()
	{
		cout <<"~Test ..." <<endl;
	}
};

int main(void)
{
	//vector<Test*> v;
	//Test* t1 = new Test;
	//Test* t2 = new Test;
	//Test* t3 = new Test;

	//v.push_back(t1);
	//v.push_back(t2);
	//v.push_back(t3);

	ptr_vector<Test> v;
	Test* t1 = new Test;
	Test* t2 = new Test;
	Test* t3 = new Test;

	v.push_back(t1);
	v.push_back(t2);
	v.push_back(t3);

	return 0;
}


/*
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

#include "DebugNew.h"
#include "Node.h"

int main(void)
{
	//int* p = new int;

	//delete p;

	//int* p2 = new int[5];
	//delete[] p2;
	
	//Node* pNode = new Node;
	////pNode->Calc();
	////*pNode;

	NodePtr np(new Node);
	np->Calc();
	*np;

	NodePtr np2(np);
	NodePtr np3;
	np3 = np2;
	
	//auto_ptr<Node> node(new Node);
	//vector<auto_ptr<Node> >v;
	//v.push_back(node);



	return 0;
}

*/