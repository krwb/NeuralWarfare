#pragma once
#include <vector>
#include "Vec2.h"
#include <queue>
#include <functional>
/// <summary>
/// K-D tree class for efficiently finding neighbor
/// </summary>
/// <remarks>
/// This is a new one for me so i was looking forward to this
/// </remarks>
template<typename T>
class KDTree {
public:
    struct CompareDist {
        bool operator()(const std::pair<double, T*>& a, const std::pair<double, T*>& b) const {
            return a.first < b.first;
        }
    };
    using MaxHeap = std::priority_queue<std::pair<double, T*>, std::vector<std::pair<double, T*>>, CompareDist>;
    using Condition = std::function<bool(const T&)>;

    KDTree(const std::vector<T*>& points) {
        root = Build(points, 0);
    }
    ~KDTree() {
        Clear(root);
    }
    std::vector<T*> FindNearestNeighbors(const Vec2& query, size_t k, Condition condition = [](const T& a) { return true; });



private:
    struct KDNode {
        T* point;
        KDNode* left;
        KDNode* right;

        KDNode(T*& pt) : point(pt), left(nullptr), right(nullptr) {}
    };
    KDNode* root;

    KDNode* Build(std::vector<T*> points, int depth);

    void FindNearest(KDNode* node, const Vec2& query, size_t k, int depth, MaxHeap& maxHeap, Condition condition);

    void Clear(KDNode* node) {
        if (node) {
            Clear(node->left);
            Clear(node->right);
            delete node;
        }
    }
};

template<typename T>
typename KDTree<T>::KDNode* KDTree<T>::Build(std::vector<T*> points, int depth) {
    if (points.empty()) return nullptr;

    size_t axis = depth % 2;
    size_t median = points.size() / 2;

    auto compare = [axis](T*& a, T*& b) {
        return axis == 0 ? a->pos.x < b->pos.x : a->pos.y < b->pos.y;
        };

    std::nth_element(points.begin(), points.begin() + median, points.end(), compare);

    KDNode* node = new KDNode(points[median]);
    std::vector<T*> leftPoints(points.begin(), points.begin() + median);
    std::vector<T*> rightPoints(points.begin() + median + 1, points.end());

    node->left = Build(leftPoints, depth + 1);
    node->right = Build(rightPoints, depth + 1);

    return node;
}

template<typename T>
void KDTree<T>::FindNearest(KDNode* node, const Vec2& query, size_t k, int depth, MaxHeap& maxHeap, Condition condition) {
    if (!node) return;

    double dist = Vec2(node->point->pos.x - query.x, node->point->pos.y - query.y).Length();
    if (maxHeap.size() < k) {
        maxHeap.emplace(dist, node->point); // Add point if less than k elements
    }
    else if (dist < maxHeap.top().first && condition(*node->point)) {
        maxHeap.pop(); // Remove the farthest point
        maxHeap.emplace(dist, node->point); // Add the new closer point
    }

    size_t axis = depth % 2;
    KDNode* nextBranch = (axis == 0 ? query.x < node->point->pos.x : query.y < node->point->pos.y) ? node->left : node->right;
    KDNode* otherBranch = (axis == 0 ? query.x < node->point->pos.x : query.y < node->point->pos.y) ? node->right : node->left;

    FindNearest(nextBranch, query, k, depth + 1, maxHeap, condition);

    double axisDist = axis == 0 ? std::abs(query.x - node->point->pos.x) : std::abs(query.y - node->point->pos.y);
    if (maxHeap.size() < k || axisDist < maxHeap.top().first) {
        FindNearest(otherBranch, query, k, depth + 1, maxHeap, condition);
    }
}

template<typename T>
std::vector<T*> KDTree<T>::FindNearestNeighbors(const Vec2& query, size_t k, Condition condition) {
    MaxHeap maxHeap;
    FindNearest(root, query, k, 0, maxHeap, condition);

    std::vector<T*> neighbors;
    while (!maxHeap.empty()) {
        neighbors.push_back(maxHeap.top().second);
        maxHeap.pop();
    }

    std::reverse(neighbors.begin(), neighbors.end());
    return neighbors;
}