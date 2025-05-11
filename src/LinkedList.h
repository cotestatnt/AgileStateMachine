#ifndef LINKEDLIST_ARDUINO_H
#define LINKEDLIST_ARDUINO_H

/**
 * Generic node class for LinkedList
 * Handles object ownership and memory management
 */
template <class T>
class ListNode {
public:
    T* element;          // Puntatore all'elemento
    bool is_owned;       // True = il nodo possiede l'elemento (memory management)
    ListNode* next;
    ListNode* prev;

    ListNode(T* elem, bool owned, ListNode* prv = nullptr, ListNode* nxt = nullptr)
        : element(elem), is_owned(owned), prev(prv), next(nxt) {}

    ~ListNode() {
        if (is_owned && element) {
            delete element;
        }
    }
};

/**
 * Generic LinkedList optimized for Arduino
 * Supports both statically and dynamically allocated elements
 */
template <class T>
class LinkedList {
private:
    int _size;
    ListNode<T>* _head;
    ListNode<T>* _tail;
    ListNode<T>* _current;

public:
    LinkedList() : _size(0), _head(nullptr), _tail(nullptr), _current(nullptr) {}

    ~LinkedList() {
        clear();
    }

    /**
     * Adds a reference to an existing object
     * The object won't be deleted when the list is destroyed
     * Ideal for static or global objects
     * 
     * @param element Reference to the object to add
     * @return Pointer to the added object
     */
    T* addReference(T& element) {
        ListNode<T>* node = new ListNode<T>(&element, false);
        _appendNode(node);
        return &element;
    }

    /**
     * Creates a copy of the object and adds it to the list
     * The list will manage the memory of the copied object
     * Ideal for local objects that would otherwise be destroyed
     * 
     * @param element Object to copy
     * @return Pointer to the copy of the object
     */
    T* addCopy(const T& element) {
        T* copy = new T(element);  // Crea una copia
        ListNode<T>* node = new ListNode<T>(copy, true);
        _appendNode(node);
        return copy;
    }

    /**
     * Transfers ownership of the object to the list
     * The list will manage the memory of the object
     * Ideal for objects created with new
     * 
     * @param element Pointer to the object to add
     * @return The same pointer to the object
     */
    T* addOwned(T* element) {
        if (!element) return nullptr;
        
        ListNode<T>* node = new ListNode<T>(element, true);
        _appendNode(node);
        return element;
    }

    // Gets the current element
    T* getCurrent() {
        return _current ? _current->element : nullptr;
    }

    // Const version of getCurrent
    const T* getCurrent() const {
        return _current ? _current->element : nullptr;
    }

    // Navigate to the first element and return it
    T* first() {
        _current = _head;
        return _current ? _current->element : nullptr;
    }

    // Navigate to the last element and return it
    T* last() {
        _current = _tail;
        return _current ? _current->element : nullptr;
    }

    // Navigate to the next element and return it
    T* next() {
        if (_current && _current->next) {
            _current = _current->next;
            return _current->element;
        }
        return nullptr;
    }

    // Navigate to the previous element and return it
    T* prev() {
        if (_current && _current->prev) {
            _current = _current->prev;
            return _current->element;
        }
        return nullptr;
    }

    // Removes the current element
    void removeCurrent() {
        if (!_current) return;

        ListNode<T>* to_delete = _current;
        
        // Update links
        if (to_delete->prev) {
            to_delete->prev->next = to_delete->next;
        } else {
            _head = to_delete->next;
        }

        if (to_delete->next) {
            to_delete->next->prev = to_delete->prev;
        } else {
            _tail = to_delete->prev;
        }

        // Update current pointer
        _current = to_delete->next ? to_delete->next : to_delete->prev;
        
        // Delete the node
        delete to_delete;
        _size--;
    }

    // Clear the entire list
    void clear() {
        while (_head) {
            ListNode<T>* temp = _head;
            _head = _head->next;
            delete temp;
        }
        _head = _tail = _current = nullptr;
        _size = 0;
    }

    // Returns the size of the list
    int size() const {
        return _size;
    }

    // Checks if the current element is valid
    bool isValid() const {
        return _current != nullptr;
    }

private:
    // Helper function to append a node to the end of the list
    void _appendNode(ListNode<T>* node) {
        if (_size == 0) {
            _head = _tail = _current = node;
        } else {
            node->prev = _tail;
            _tail->next = node;
            _tail = node;
        }
        _size++;
    }
};

#endif // LINKEDLIST_ARDUINO_H