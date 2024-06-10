#pragma once
#include <vector>
#include "Vec2.h"
#include <queue>
#include <functional>

/// <summary>
/// Objects used in a K-D tree require a member 'pos' that is convertible to a vec2
/// </summary>
/// <remarks>s
/// concepts are new to c++ 20, they allow you to place constraints on types used for a templates
/// this gives you a more useful compile time error if the type is incapable of doing what is required of them
/// </remarks>
template<typename T>
concept Object = requires(T a)
{
    { a.pos } -> std::convertible_to<Vec2>;
};

/// <summary>
/// K-D tree class for efficiently finding neighbor
/// </summary>
/// <typeparam name="T"></typeparam>
/// <remarks>
/// This is a new one for me so i was looking forward to this
/// </remarks>
template<Object Obj>
class KDTree {
public:
    struct CompareDist {
        bool operator()(const std::pair<double, Obj*>& a, const std::pair<double, Obj*>& b) const {
            return a.first < b.first;
        }
    };
    using MaxHeap = std::priority_queue<std::pair<double, Obj*>, std::vector<std::pair<double, Obj*>>, CompareDist>;
    using Condition = std::function<bool(const Obj*)>;

    KDTree(const std::vector<Obj*>& points) {
        root = Build(points, 0);
    }
    ~KDTree() {
        Clear(root);
    }
    std::vector<Obj*> FindNearestNeighbors(const Vec2& query, size_t k, Condition condition = [](const Obj* a) { return true; });
    std::vector<Obj*> FindInRange(const Vec2& query, float range, Condition condition = [](const Obj* a) { return true; });

    struct KDNode {
        Obj* point;
        KDNode* left;
        KDNode* right;

        KDNode(Obj*& pt) : point(pt), left(nullptr), right(nullptr) {}
    };
    KDNode* root;
    void FindNearest(KDNode* node, const Vec2& query, size_t k, int depth, MaxHeap& maxHeap, Condition condition);
    void FindRange(KDNode* node, const Vec2& query, float range, int depth, std::vector<Obj*>& objects, Condition condition);

private:

    KDNode* Build(std::vector<Obj*> points, int depth);

    void Clear(KDNode* node) {
        if (node) {
            Clear(node->left);
            Clear(node->right);
            delete node;
        }
    }
};

template<Object Obj>
typename KDTree<Obj>::KDNode* KDTree<Obj>::Build(std::vector<Obj*> points, int depth) {
    if (points.empty()) return nullptr;

    size_t axis = depth % 2;
    size_t median = points.size() / 2;

    auto compare = [axis](Obj*& a, Obj*& b) {
        return axis == 0 ? a->pos.x < b->pos.x : a->pos.y < b->pos.y;
        };

    std::nth_element(points.begin(), points.begin() + median, points.end(), compare);

    KDNode* node = new KDNode(points[median]);
    std::vector<Obj*> leftPoints(points.begin(), points.begin() + median);
    std::vector<Obj*> rightPoints(points.begin() + median + 1, points.end());

    node->left = Build(leftPoints, depth + 1);
    node->right = Build(rightPoints, depth + 1);

    return node;
}

template<Object Obj>
void KDTree<Obj>::FindNearest(KDNode* node, const Vec2& query, size_t k, int depth, MaxHeap& maxHeap, Condition condition) {
    if (!node) return;

    // Calculate the squared distance from the query point to the current node's point
    double dx = node->point->pos.x - query.x;
    double dy = node->point->pos.y - query.y;
    double squaredDist = dx * dx + dy * dy;

    // If the heap has less than k elements or the current point is closer than the farthest point in the heap
    if (condition(node->point)) {
        if (maxHeap.size() < k) {
            // Add the point directly if heap is not full
            maxHeap.emplace(squaredDist, node->point);
        }
        else if (squaredDist < maxHeap.top().first) {
            // Replace the farthest point if the current point is closer
            maxHeap.pop();
            maxHeap.emplace(squaredDist, node->point);
        }
    }

    // Determine the current axis (0 for x, 1 for y)
    size_t axis = depth % 2;

    // Determine which branch to explore next based on the query point's coordinate and the current node's coordinate
    bool goLeft = (axis == 0 ? query.x < node->point->pos.x : query.y < node->point->pos.y);
    KDNode* nextBranch = goLeft ? node->left : node->right;
    KDNode* otherBranch = goLeft ? node->right : node->left;

    // Recursively search the next branch
    FindNearest(nextBranch, query, k, depth + 1, maxHeap, condition);

    // Calculate the squared distance to the splitting plane (axis distance)
    double axisDist = (axis == 0 ? query.x - node->point->pos.x : query.y - node->point->pos.y);
    double axisDistSq = axisDist * axisDist;

    // If the heap has less than k elements or the distance to the splitting plane is less than the farthest distance in the heap, search the other branch too
    if (maxHeap.size() < k || axisDistSq < maxHeap.top().first) {
        FindNearest(otherBranch, query, k, depth + 1, maxHeap, condition);
    }
}

template <Object Obj>
void KDTree<Obj>::FindRange(KDNode* node, const Vec2& query, float range, int depth, std::vector<Obj*>& objects, Condition condition) {
    if (!node) return;

    // Calculate distance from the query point to the current node's point
    double dist = (node->point->pos - query).Length();

    // If the point is within the range and meets the condition, add it to the result vector
    if (dist < range && condition(node->point)) {
        objects.push_back(node->point);
    }

    // Determine the axis (0 for x, 1 for y)
    size_t axis = depth % 2;

    // Determine which branch to search next
    KDNode* nextBranch = (axis == 0 ? query.x < node->point->pos.x : query.y < node->point->pos.y) ? node->left : node->right;
    KDNode* otherBranch = (axis == 0 ? query.x < node->point->pos.x : query.y < node->point->pos.y) ? node->right : node->left;

    // Recursively search the next branch
    FindRange(nextBranch, query, range, depth + 1, objects, condition);

    // Determine the distance to the splitting plane
    double axisDist = (axis == 0 ? std::abs(query.x - node->point->pos.x) : std::abs(query.y - node->point->pos.y));

    // If the distance to the splitting plane is less than the range search the other branch too.
    if (axisDist < range) {
        FindRange(otherBranch, query, range, depth + 1, objects, condition);
    }
}

template<Object Obj>
std::vector<Obj*> KDTree<Obj>::FindNearestNeighbors(const Vec2& query, size_t k, Condition condition) {
    MaxHeap maxHeap;
    FindNearest(root, query, k, 0, maxHeap, condition);
    std::vector<Obj*> neighbors;
    while (!maxHeap.empty()) {
        neighbors.push_back(maxHeap.top().second);
        maxHeap.pop();
    }

    std::reverse(neighbors.begin(), neighbors.end());
    return neighbors;
}

template<Object Obj>
inline std::vector<Obj*> KDTree<Obj>::FindInRange(const Vec2& query, float range, Condition condition)
{
    std::vector<Obj*> objects;
    FindRange(root, query, range, 0, objects, condition);
    return objects;
}
