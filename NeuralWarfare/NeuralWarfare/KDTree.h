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
/// <typeparam name="Obj"> must satisfy the requirements of the Object concept</typeparam>
/// <remarks>
/// This is a new one for me so i had a blast
/// </remarks>
template<Object Obj>
class KDTree {
public:

    /// <summary>
    /// Simple struct required by priority_queue to determine what object has the highest value
    /// </summary>
    struct CompareDist {
        bool operator()(const std::pair<double, Obj*>& a, const std::pair<double, Obj*>& b) const {
            return a.first < b.first;
        }
    };
    using MaxHeap = std::priority_queue<std::pair<double, Obj*>, std::vector<std::pair<double, Obj*>>, CompareDist>; // Max heap data structure meaning it always keeps the largest element at the top
    
    /// <summary>
    /// A function to be passed a param, used to determine whether given pointer is valid for return
    /// this can be used to ensure the returned pointer is not the same as the one used for the query
    /// or for placing any other constraint on the returned pointers
    /// </summary>
    /// <param name="object"> for evaluation</param>
    /// <returns>Boolean representing weather the object satisfies the condition</returns>
    using Condition = std::function<bool(const Obj*)>;

    /// <summary>
    /// KDTree constructor
    /// </summary>
    KDTree() {
        root = nullptr;
    }

    /// <summary>
    /// KDTree constructor
    /// </summary>
    /// <param name="points"> array</param>
    KDTree(const std::vector<Obj*>& points) {
        root = Build(points, 0);
    }

    /// <summary>
    /// KDTree destructor
    /// </summary>
    ~KDTree() {
        Clear(root);
    }

    /// <summary>
    /// Finds the nearest Neighbors to a query position
    /// </summary>
    /// <param name="query"> position to find Neighbors to</param>
    /// <param name="k"> the number of Neighbors to be found</param>
    /// <param name="condition"> a condition a Neighbor must satisfy to be returned</param>
    /// <returns>array of neighbors</returns>
    std::vector<Obj*> FindNearestNeighbors(const Vec2& query, size_t k, Condition condition = [](const Obj* a) { return true; });

    /// <summary>
    /// Finds all Neighbors within range of a query position
    /// </summary>
    /// <param name="query"> position to find objs in range of</param>
    /// <param name="range"> distance to search</param>
    /// <param name="condition">a condition a Neighbor must satisfy to be returned</param>
    /// <returns>array of neighbors</returns>
    std::vector<Obj*> FindInRange(const Vec2& query, float range, Condition condition = [](const Obj* a) { return true; });

    /// <summary>
    /// KDNode structure, used to build the KD tree
    /// </summary>
    struct KDNode {
        Obj* point; // pointer to the node's obj
        KDNode* left; // left child in the KD tree
        KDNode* right; // right child in the KD tree

        /// <summary>
        /// KDNode constructor
        /// </summary>
        /// <param name="pt">pointer to obj</param>
        KDNode(Obj*& pt) : point(pt), left(nullptr), right(nullptr) {}
    };

    KDNode* root; // root node, used to start a search and should point to the most central point

    /// <summary>
    /// Function for finding nearest neighbors
    /// </summary>
    /// <param name="node"> the current node in the search</param>
    /// <param name="query"> position to find neighbors to</param>
    /// <param name="k"> the number of neighbors to be found</param>
    /// <param name="depth"> the depth of the search, used to determine the current axis </param>
    /// <param name="maxHeap"> heap containing the current known nearest neighbors to the query point</param>
    /// <param name="condition"> a condition a neighbor must satisfy to be returned</param>
    void FindNearest(KDNode* node, const Vec2& query, size_t k, int depth, MaxHeap& maxHeap, Condition condition);
    
    /// <summary>
    /// function for finding objs in range
    /// </summary>
    /// <param name="node"> the current node in the search</param>
    /// <param name="query"> position to find objs in range of</param>
    /// <param name="range"> distance to search</param>
    /// <param name="depth"> the depth of the search, used to determine the current axis </param>
    /// <param name="objects"> array of objects within range</param>
    /// <param name="condition"> a condition a object must satisfy to be returned</param>
    void FindRange(KDNode* node, const Vec2& query, float range, int depth, std::vector<Obj*>& objects, Condition condition);

    /// <summary>
    /// Function to build KD tree
    /// </summary>
    /// <param name="points"> points to add to tree</param>
    /// <param name="depth"> used to determine the current axis </param>
    /// <returns>Created node, can be considered the root of "points"'s KD tree</returns>
    KDNode* Build(std::vector<Obj*> points, int depth);

    /// <summary>
    /// clears and deletes the given node and its children (calling this on root will delete the whole tree)
    /// </summary>
    /// <param name="node"> node to delete</param>
    void Clear(KDNode* node) {
        if (node) {
            Clear(node->left);
            Clear(node->right);
            delete node;
        }
    }
    private:

};

template<Object Obj>
typename KDTree<Obj>::KDNode* KDTree<Obj>::Build(std::vector<Obj*> points, int depth) {
    if (points.empty()) return nullptr;

    size_t axis = depth % 2;
    size_t median = points.size() / 2;

    std::nth_element(points.begin(), points.begin() + median, points.end(), 
        [axis](Obj*& a, Obj*& b) 
        {
            return axis == 0 ? a->pos.x < b->pos.x : a->pos.y < b->pos.y;
        });

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

    // If the point satisfies the condition
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
