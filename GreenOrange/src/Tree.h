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

    TreeNode(PayloadT &payload);

    PayloadT& getPayload() { return payload; }
    const PayloadT& getPayload() const { return payload; }

    PayloadT& operator*() { return payload; }
    const PayloadT& operator*() const { return payload; }
    PayloadT* operator->() { return &payload; }
    const PayloadT* operator->() const { return &payload; }

    //-----------------------------------------
    //Operations on the node itself
    size_t getChildCount() const { return childNodes.size(); }
    size_t getNonEmptyChildCount() const;
    bool hasChildren() const { return !childNodes.empty(); }
    bool hasNonEmptyChildren() const { return getNonEmptyChildCount() > 0; }
    TreeNode<PayloadT>& getChildByIndex(uint32 idx);

    void createChild(PayloadT &payload);
    bool deleteChild(TreeNode<PayloadT> &node);
    bool deleteChildren();

    //Move Node into this one
    bool moveInto(TreeNode<PayloadT> &parentToMoveFrom, TreeNode<PayloadT> &toMove);

    typename std::vector<std::unique_ptr<TreeNode<PayloadT>>>::iterator findChild(const TreeNode<PayloadT> &node);

    //-----------------------------------------
    //Operations on the tree where this node is the root
    bool deleteNode(TreeNode<PayloadT> &toDelete);
    bool moveNode(TreeNode<PayloadT> &toMove, TreeNode<PayloadT> &destination);
    bool findNode(TreeNode<PayloadT> &node);
    TreeNode<PayloadT>* findNodeParent(TreeNode<PayloadT> &node);
    bool isNodeDescendentOf(TreeNode<PayloadT> &op1, TreeNode<PayloadT> &op2);

    //The visitFunction will receive the Node and its parent, in this order
    //If visitFunction returns true the traversal will stop
    void traverseBfs(const std::function<bool(TreeNode<PayloadT>&, TreeNode<PayloadT>*)> &visitFunction);
    void traverseDfs(const std::function<bool(TreeNode<PayloadT>&)> &visitFunction);

private:
    std::vector<std::unique_ptr<TreeNode<PayloadT>>> childNodes;
    PayloadT &payload;
};


template<typename PayloadT>
TreeNode<PayloadT>::TreeNode(PayloadT &payload) :
    payload(payload) {
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
void TreeNode<PayloadT>::createChild(PayloadT& payload) {
    childNodes.emplace_back(std::make_unique<TreeNode>(payload));
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
bool TreeNode<PayloadT>::moveInto(TreeNode<PayloadT> &parentToMoveFrom, TreeNode<PayloadT> &toMove) {
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
    if(*toDelete == payload) 
        return false;

    TreeNode<PayloadT> *parent = findNodeParent(toDelete);
    if(parent) {
        parent->deleteChild(toDelete);
        return true;
    }
    return false;
}

template<typename PayloadT>
bool TreeNode<PayloadT>::moveNode(TreeNode<PayloadT> &toMove, TreeNode<PayloadT> &destination) {
    if(isNodeDescendentOf(destination, toMove))
        return false;

    TreeNode<PayloadT> *parent = findNodeParent(toMove);
    if(parent && **parent != *destination) {
        destination.moveInto(*parent, toMove);
        return true;
    }
    return false;
}

template<typename PayloadT>
bool TreeNode<PayloadT>::findNode(TreeNode<PayloadT> &node) {
    bool result = false;
    traverseBfs([&result, &node](TreeNode<PayloadT> &op, TreeNode<PayloadT> *parent) -> bool {
        if(*op == *node) {
            result = true;
            return true;
        }
        return false;
    });
    return result;
}

template<typename PayloadT>
TreeNode<PayloadT>* TreeNode<PayloadT>::findNodeParent(TreeNode<PayloadT> &node) {
    TreeNode<PayloadT>* result = nullptr;
    traverseBfs([&result, &node](TreeNode<PayloadT> &op, TreeNode<PayloadT> *parent) -> bool {
        if(*op == *node) {
            result = parent;
            return true;
        }
        return false;
    });
    return result;
}

template<typename PayloadT>
bool TreeNode<PayloadT>::isNodeDescendentOf(TreeNode<PayloadT> &op1, TreeNode<PayloadT> &op2) {
    bool result = false;
    op2.traverseBfs([&op1, &result](TreeNode<PayloadT> &op, TreeNode<PayloadT> *parent) -> bool {
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