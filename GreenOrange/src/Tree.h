#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <queue>
#include <stack>

#include "Types.h"

#include "Assert.h"
#include "GlslGenerator.h"


template<typename PayloadT>
class TreeNode
{
public:

    //TODO: try to perfect forward arguments for T constructor
    //T should be of type PayloadT
    template<typename T>
    TreeNode(T&& payload);

    PayloadT& getPayload() { return *payload; }
    const PayloadT& getPayload() const { return *payload; }

    PayloadT& operator*() { return *payload; }
    const PayloadT& operator*() const { return *payload; }
    PayloadT* operator->() { return payload.get(); }
    const PayloadT* operator->() const { return payload.get(); }

    //-----------------------------------------
    //Operations on the node itself
    size_t getChildCount() const { return childNodes.size(); }
    size_t getNonEmptyChildCount() const;
    bool hasChildren() const { return !childNodes.empty(); }
    bool hasNonEmptyChildren() const { return getNonEmptyChildCount() > 0; }
    TreeNode<PayloadT>& getChildByIndex(uint32 idx);

    //T should be of type PayloadT
    template<typename T>
    void createChild(T&& payload);
    bool deleteChild(TreeNode<PayloadT> &node);
    bool deleteChildren();

    //Move Node into this one
    bool moveChild(TreeNode<PayloadT> &parentToMoveFrom, TreeNode<PayloadT> &toMove);

    typename std::vector<std::unique_ptr<TreeNode<PayloadT>>>::iterator findChild(const TreeNode<PayloadT> &node);

    //-----------------------------------------
    //Operations on the tree where this node is the root
    bool deleteNode(TreeNode<PayloadT> &toDelete);
    bool moveNode(TreeNode<PayloadT> &toMove, TreeNode<PayloadT> &destination);
    bool findNode(TreeNode<PayloadT> &node);
    bool isNodeDescendentOf(TreeNode<PayloadT> &op1, TreeNode<PayloadT> &op2);

    //The visitFunction will receive the Node and its parent, in this order
    //If visitFunction returns true the traversal will stop
    void traverseBfs(const std::function<bool(TreeNode<PayloadT>&, TreeNode<PayloadT>*)> &visitFunction);
    void traverseDfs(const std::function<bool(TreeNode<PayloadT>&)> &visitFunction);

private:
    std::vector<std::unique_ptr<TreeNode<PayloadT>>> childNodes;
    std::unique_ptr<PayloadT> payload;
};


template<typename PayloadT>
template<typename T>
TreeNode<PayloadT>::TreeNode(T&& payload) :
    payload(std::make_unique<T>(std::forward<T>(payload))) {
}

template<typename PayloadT>
size_t TreeNode<PayloadT>::getNonEmptyChildCount() const {
    size_t sum = 0;
    for(const auto& childPtr : childNodes) {
        if(childPtr->hasNonEmptyChildren()) {
            sum++;
        }
    }
    return sum;
}

template<typename PayloadT>
TreeNode<PayloadT>& TreeNode<PayloadT>::getChildByIndex(uint32 idx) {
    GO_ASSERT(idx < childNodes.size());
    return *childNodes[idx];
}

template<typename PayloadT>
template<typename T>
void TreeNode<PayloadT>::createChild(T&& payload) {
    static_assert(std::is_base_of<PayloadT, T>::value, "T must derive from PayloadT");
    childNodes.emplace_back(std::make_unique<TreeNode>(std::forward<T>(payload)));
}

template<typename PayloadT>
bool TreeNode<PayloadT>::deleteChild(TreeNode<PayloadT> &node) {
    auto it = findChild(node);
    if(it != childNodes.end()) {
        childNodes.erase(it);
        return true;
    }
    return false;
}

template<typename PayloadT>
bool TreeNode<PayloadT>::deleteChildren() {
    bool result = !childNodes.empty();
    childNodes.clear();
    return result;
}

template<typename PayloadT>
bool TreeNode<PayloadT>::moveChild(TreeNode<PayloadT> &parentToMoveFrom, TreeNode<PayloadT> &toMove) {
    auto toMoveIt = parentToMoveFrom.findChild(toMove);
    if(toMoveIt != parentToMoveFrom.childNodes.end()) {
        std::move(toMoveIt, toMoveIt + 1, std::back_inserter(childNodes));
        parentToMoveFrom.childNodes.erase(toMoveIt);
        return true;
    }
    return false;
}

//Forcing PayloadT to have operator==() defined
template<typename PayloadT>
typename std::vector<std::unique_ptr<TreeNode<PayloadT>>>::iterator TreeNode<PayloadT>::findChild(const TreeNode<PayloadT> &node) {
    for(auto it = childNodes.begin(); it != childNodes.end(); ++it) {
        if((*it)->getPayload() == node.getPayload()) {
            return it;
        }
    }
    return childNodes.end();
}

template<typename PayloadT>
bool TreeNode<PayloadT>::deleteNode(TreeNode<PayloadT> &toDelete) {
    if(*toDelete == *payload) 
        return false;

    bool result = false;
    traverseBfs([&result, &toDelete](TreeNode<PayloadT> &op, TreeNode<PayloadT> *parent) -> bool {
        if(parent && *op == *toDelete) {
            parent->deleteChild(op);
            result = true;
            return true;
        }
        return false;
    });
    return result;
}

template<typename PayloadT>
bool TreeNode<PayloadT>::moveNode(TreeNode<PayloadT> &toMove, TreeNode<PayloadT> &destination) {
    if(isNodeDescendentOf(destination, toMove))
        return false;

    bool result = false;
    traverseBfs([&result, &toMove, &destination](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
        if(parent && *op == *toMove && **parent != *destination) {
            destination.moveChild(*parent, toMove);
            result = true;
            return true;
        }
        return false;
    });
    return result;
}

template<typename PayloadT>
bool TreeNode<PayloadT>::findNode(TreeNode<PayloadT> &node) {
    bool result = false;
    traverseBfs([&result, &node](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
        if(*op == *node) {
            result = true;
            return true;
        }
        return false;
    });
    return result;
}


template<typename PayloadT>
bool TreeNode<PayloadT>::isNodeDescendentOf(TreeNode<PayloadT> &op1, TreeNode<PayloadT> &op2) {
    bool result = false;
    op2.traverseBfs([&op1, &result](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
        if(*op == *op1) {
            result = true;
            return true;
        }
        return false;
    });
    return result;
}

template<typename PayloadT>
void TreeNode<PayloadT>::traverseBfs(const std::function<bool(TreeNode<PayloadT>&, TreeNode<PayloadT>*)> &visitFunction) {
    std::queue<TreeNode<PayloadT>*> queue;
    queue.push(this);

    if(visitFunction(*this, nullptr))
        return;

    while(!queue.empty()) {
        TreeNode<PayloadT> &current = *queue.front();
        queue.pop();

        for(uint32 i = 0; i < current.getChildCount(); ++i) {
            auto &child = current.getChildByIndex(i);
            if(visitFunction(child, &current))
                return;
            queue.push(&child);
        }
    }
}

template<typename PayloadT>
void TreeNode<PayloadT>::traverseDfs(const std::function<bool(TreeNode<PayloadT>&)> &visitFunction) {
    std::stack<TreeNode*> stack;
    stack.push(this);

    while(!stack.empty()) {
        TreeNode &current = *stack.top();
        stack.pop();

        if(visitFunction(current))
            return;

        //Pushing backwards so that it visit child nodes from 0 to N ("left to right").
        for(int i = static_cast<int>(current.getChildCount()) - 1; i >= 0; --i) {
            auto &child = current.getChildByIndex(i);
            stack.push(&child);
        }
    }
}