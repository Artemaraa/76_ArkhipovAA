#ifndef DEPENDENCYEDGE_H
#define DEPENDENCYEDGE_H

class Action;

enum DependencyType {
    Direct, General, NoDependency
};

class DependencyEdge
{
public:
    Action* from;
    Action* to;
    DependencyType type;

    DependencyEdge(Action* f, Action* t, DependencyType tp);
};

#endif // DEPENDENCYEDGE_H
