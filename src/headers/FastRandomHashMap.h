#pragma once

#include <random>
#include <unordered_map>
#include <vector>

struct Point {
    unsigned int x;
    unsigned int y;

    Point(unsigned int x, unsigned int y) : x(x), y(y) {}

    bool operator==(const Point &other) const {
        return x == other.x && y == other.y;
    }
};

struct PointHasher {
    size_t operator()(const Point& p) const {
        return std::hash<unsigned int>()(p.x) ^ std::hash<unsigned int>()(p.y);
    }
};

class FastRandomHashMap {
public:
    FastRandomHashMap();
    FastRandomHashMap(const std::mt19937 &generator);
    void AddItem(Point value);
    void RemoveItem(Point value);
    Point ChooseRandomItem();
    void Clear();
private:
    std::vector<Point> items;
    std::unordered_map<Point, int, PointHasher> map;
    std::mt19937 gen;
};
