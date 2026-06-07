#include "dependencyedge.h"
#include "action.h"

DependencyEdge::DependencyEdge(Action* f, Action* t, DependencyType tp)
    : from(f), to(t), type(tp)
{
}
