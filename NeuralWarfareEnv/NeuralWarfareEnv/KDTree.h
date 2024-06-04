#pragma once
#include <vector>
#include "Vec2.h"
#include <queue>

/// <summary>
/// K-D tree class for efficiently finding neighbor
/// </summary>
/// <remarks>
/// This is a new one for me so i was looking forward to this
/// </remarks>
class KDTree {
public:
    KDTree(const std::vector<Vec2>& points) {
        root = Build(points, 0);
    }

    std::vector<Vec2> FindNearestNeighbors(const Vec2& query, size_t k);

    struct CompareDist
    {
        bool operator()(const std::pair<double, Vec2>& a, const std::pair<double, Vec2>& b) {
            return a.first < b.first;
        }
    };
    using MaxHeap = std::priority_queue<std::pair<double, Vec2>, std::vector<std::pair<double, Vec2>>, CompareDist>;

private:
    struct KDNode {
        Vec2 point;
        KDNode* left;
        KDNode* right;

        KDNode(const Vec2& pt) : point(pt), left(nullptr), right(nullptr) {}
    };
    KDNode* root;

    KDNode* Build(const std::vector<Vec2>& points, int depth);

    void FindNearest(KDNode* node, const Vec2& query, size_t k, int depth, MaxHeap& maxHeap);
};
