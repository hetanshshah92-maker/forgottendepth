#ifndef ITEMS_H
#define ITEMS_H

#include <string>

struct Item {
    std::string name;
    std::string description;
    int healAmount; // e.g., healing potion amount in HP; 0 means no heal
    int value;      // gold value or special uses later
    Item() : name(""), description(""), healAmount(0), value(0) {}
    Item(const std::string &n, const std::string &d, int heal, int v)
        : name(n), description(d), healAmount(heal), value(v) {}
};

#endif // ITEMS_H
