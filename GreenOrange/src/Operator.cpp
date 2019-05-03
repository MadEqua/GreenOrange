#include "Operator.h"


Operator::Operator(const char *name, int n) :
    name(name) {

    objects.push_back(std::make_unique<Object>("o1"));
    objects.push_back(std::make_unique<Object>("o2"));
    objects.push_back(std::make_unique<Object>("o3"));

    if(n > 0) {
        operators.push_back(std::make_unique<Operator>("op", n - 1));
        operators.push_back(std::make_unique<Operator>("op1", n - 1));
        operators.push_back(std::make_unique<Operator>("op2", n - 1));
    }
}