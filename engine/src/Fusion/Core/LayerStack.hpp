#pragma once

namespace Fusion {
    class Layer;

    //! A linear collection of Layers that supports element insertion and removal at both ends and also execute events
    class FUSION_API LayerStack {
    public:
        LayerStack() = default;
        ~LayerStack();
        FE_NONCOPYABLE(LayerStack);

        //! Inserts the specified element at the front of this deque if it is possible to do so immediately without violating capacity restrictions.
        void pushFront(Layer* layer);
        // Inserts the specified element at the end of this deque if it is possible to do so immediately without violating capacity restrictions.
        void pushBack(Layer* layer);
        //! Retrieves and removes the first element of this deque, or returns null if this deque is empty.
        Layer* pollFront();
        //! Retrieves and removes the last element of this deque, or returns null if this deque is empty.
        Layer* pollBack();
        //! Retrieves and removes the first element of this deque. This method differs from \a pollFront only in that it throws an exception if this deque is empty.
        Layer* removeFront();
        //! Retrieves and removes the last element of this deque. This method differs from \a pollBack only in that it throws an exception if this deque is empty.
        Layer* removeBack();
        //! Removes the first occurrence of the specified element from this deque.
        void remove(Layer* layer);

        //! Retrieves, but does not remove, the first element of this deque. This method differs from \a peekFront only in that it throws an exception if this deque is empty.
        Layer* getFront() const { return layers.front(); }
        //! Retrieves, but does not remove, the last element of this deque. This method differs from \a peekBack only in that it throws an exception if this deque is empty.
        Layer* getBack() const { return layers.back(); }
        //! Retrieves, but does not remove, the first element of this deque, or returns null if this deque is empty.
        Layer* peekFront() const { return layers.empty() ? nullptr : layers.front(); }
        //! Retrieves, but does not remove, the last element of this deque, or returns null if this deque is empty.
        Layer* peekBack() const { return layers.empty() ? nullptr : layers.back(); }

        //! Returns \c true when this deque is empty.
        bool empty() const { return layers.empty(); }
        //! Returns the number of elements in this deque.
        size_t size() const { return layers.size(); }

        //! Returns an iterator over the elements in this deque in proper sequence.
        std::deque<Layer*>::iterator begin() { return layers.begin(); }
        std::deque<Layer*>::iterator end() { return layers.end(); }
        std::deque<Layer*>::reverse_iterator rbegin() { return layers.rbegin(); }
        std::deque<Layer*>::reverse_iterator rend() { return layers.rend(); }
        std::deque<Layer*>::const_iterator begin() const { return layers.begin(); }
        std::deque<Layer*>::const_iterator end() const { return layers.end(); }
        std::deque<Layer*>::const_reverse_iterator rbegin() const { return layers.rbegin(); }
        std::deque<Layer*>::const_reverse_iterator rend() const { return layers.rend(); }

    private:
        std::deque<Layer*> layers;
    };
}
