#include "../headers/FastRandomHashMap.h"

#include <unordered_map>

FastRandomHashMap::FastRandomHashMap() {
    gen = std::mt19937();
    items = std::vector<Point>();
    map = std::unordered_map<Point, int, PointHasher>();
}

FastRandomHashMap::FastRandomHashMap(const std::mt19937 &generator) : FastRandomHashMap() {
    gen = generator;
}

void FastRandomHashMap::Clear() {
    items.clear();
    map.clear();
}

void FastRandomHashMap::AddItem(Point value) {
    if (map.contains(value)) {
        return;
    }

    items.push_back(value);
    map[value] = items.size() - 1;
}

void FastRandomHashMap::RemoveItem(Point value) {
    if (!map.contains(value)) {
        return;
    }

    int index = map[value];
    map.erase(value);
    Point last = items.back();
    items.pop_back();

    if (index != items.size()) {
        items[index] = last;
        map[last] = index;
    }
}

Point FastRandomHashMap::ChooseRandomItem() {
    auto distrib = std::uniform_int_distribution<int>(0, items.size() - 1);

    return items[distrib(gen)];
}
