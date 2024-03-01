#ifndef LinkedList_hpp
#define LinkedList_hpp


template <class T>
class ListNode {
  public:
    T element;
    ListNode* next;
    ListNode* prev;

    ListNode(T element, ListNode* prev, ListNode* next) : element(element)
    {
      this->next = next;
      this->prev = prev;
    };
};

template <class T>
class LinkedList  {
  private:
    int length;
    ListNode<T>* head;
    ListNode<T>* tail;
    ListNode<T>* curr;
  public:
    LinkedList();
    ~LinkedList();
    T getCurrent();
    T first();
    T last();
    T next();
    T prev();

    int size();
    void append(T);
    void deleteCurrent();
    void clear();
};

template <class T>
LinkedList<T>::LinkedList() {
    length = 0;
    head = nullptr;
    tail = nullptr;
    curr = nullptr;
}


template <class T>
LinkedList<T>::~LinkedList() {
    clear();
}


template <class T>
void LinkedList<T>::append(T element)
{
    ListNode<T> * node = new ListNode<T>(element, tail, nullptr);

    if(length == 0)
        curr = tail = head = node;
    else {
        tail->next = node;
        tail = node;
    }
    length++;
}


template<class T>
int LinkedList<T>::size()
{
  return length;
}


template<class T>
T LinkedList<T>::getCurrent()
{
  return curr->element;
}

template<class T>
T LinkedList<T>::first()
{
    curr = head;
    return head->element;
}

template<class T>
T LinkedList<T>::last()
{
    curr = tail;
    return tail->element;
}

template <class T>
T LinkedList<T>::next()
{
    if(length == 0)
        return nullptr;

    if(curr->next == nullptr)
        return nullptr;

    curr = curr->next;
    return curr->element;
}

template<class T>
T LinkedList<T>::prev()
{
    if(length == 0)
        return nullptr;

    if(curr->prev != nullptr)
        return nullptr;

    curr = curr->prev;
    return curr->element;
}


template <class T>
void LinkedList<T>::deleteCurrent()
{
    if(length == 0)
        return;
    length--;
    ListNode<T> * temp = curr;

    if(temp->prev != nullptr)
        temp->prev->next = temp->next;
    if(temp->next != nullptr)
        temp->next->prev = temp->prev;

    if(length == 0)
        head = curr = tail = nullptr;
    else if(curr == head)
        curr = head = head->next;
    else if(curr == tail)
        curr = tail = tail->prev;
    else
        curr = curr->prev;

     delete temp;
}


template <class T>
void LinkedList<T>::clear()
{
    if(length == 0)
        return;
    ListNode<T> * temp = head;

    while(temp != nullptr)
    {
        head = head->next;
        delete temp;
        temp = head;
    }
    head = curr = tail = nullptr;
    length = 0;
}


#endif