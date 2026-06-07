#include "action.h"
#include "exprnode.h"

Action::Action(int num)
    : number(num), targetRoot(nullptr), expression(nullptr)
{
}

Action::~Action()
{
    delete targetRoot;
    delete expression;
}
