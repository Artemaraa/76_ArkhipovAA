#ifndef DEPENDENCYEDGE_H
#define DEPENDENCYEDGE_H

class Action;

/**
 * @brief Тип зависимости между действиями
 */
enum DependencyType {
    Direct,         ///< прямая зависимость
    General,        ///< общая (потенциальная) зависимость
    NoDependency    ///< нет зависимости
};

/**
 * @brief Ребро графа зависимостей: связь "зависимое действие -> источник"
 *
 * Поле from - зависимое действие (которое читает переменную), to - источник
 */
class DependencyEdge
{
public:
    Action* from;          ///< зависимое действие
    Action* to;            ///< действие-источник
    DependencyType type;   ///< тип зависимости

    /**
     * @brief Создаёт ребро зависимости.
     * @param[in] f   зависимое действие (from)
     * @param[in] t   действие-источник (to)
     * @param[in] tp  тип зависимости
     */
    DependencyEdge(Action* f, Action* t, DependencyType tp);
};

#endif // DEPENDENCYEDGE_H
