#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>
#include <cstring>
#include <new>

namespace sjtu {

template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;

        node() : data(nullptr), prev(nullptr), next(nullptr) {}

        node(const T &value) : prev(nullptr), next(nullptr) {
            data = static_cast<T*>(::operator new(sizeof(T)));
            new(data) T(value);
        }

        ~node() {
            if (data) {
                data->~T();
                ::operator delete(data);
            }
        }
    };

    node *head;
    node *tail;
    size_t len;

    void init() {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        len = 0;
    }

    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }

    node *erase(node *pos) {
        node *ret = pos->next;
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        return ret;
    }

public:
    class const_iterator;
    class iterator {
    private:
        node *ptr;
        list *container;

    public:
        iterator() : ptr(nullptr), container(nullptr) {}
        iterator(node *p, list *c) : ptr(p), container(c) {}
        iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}

        iterator operator++(int) {
            if (ptr == nullptr || ptr->next == nullptr || container == nullptr)
                throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        iterator &operator++() {
            if (ptr == nullptr || ptr->next == nullptr || container == nullptr)
                throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }

        iterator operator--(int) {
            if (ptr == nullptr || ptr->prev == nullptr || ptr->prev->prev == nullptr || container == nullptr)
                throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        iterator &operator--() {
            if (ptr == nullptr || ptr->prev == nullptr || ptr->prev->prev == nullptr || container == nullptr)
                throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }

        T &operator*() const {
            if (ptr == nullptr || ptr->data == nullptr || container == nullptr)
                throw invalid_iterator();
            return *(ptr->data);
        }

        T *operator->() const {
            if (ptr == nullptr || ptr->data == nullptr || container == nullptr)
                throw invalid_iterator();
            return ptr->data;
        }

        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        node *get_ptr() const { return ptr; }
        list *get_container() const { return container; }

        friend class list;
        friend class const_iterator;
    };

    class const_iterator {
    private:
        node *ptr;
        const list *container;

    public:
        const_iterator() : ptr(nullptr), container(nullptr) {}
        const_iterator(node *p, const list *c) : ptr(p), container(c) {}
        const_iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}
        const_iterator(const const_iterator &other) : ptr(other.ptr), container(other.container) {}

        const_iterator operator++(int) {
            if (ptr == nullptr || ptr->next == nullptr || container == nullptr)
                throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        const_iterator &operator++() {
            if (ptr == nullptr || ptr->next == nullptr || container == nullptr)
                throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }

        const_iterator operator--(int) {
            if (ptr == nullptr || ptr->prev == nullptr || ptr->prev->prev == nullptr || container == nullptr)
                throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        const_iterator &operator--() {
            if (ptr == nullptr || ptr->prev == nullptr || ptr->prev->prev == nullptr || container == nullptr)
                throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }

        const T &operator*() const {
            if (ptr == nullptr || ptr->data == nullptr || container == nullptr)
                throw invalid_iterator();
            return *(ptr->data);
        }

        const T *operator->() const {
            if (ptr == nullptr || ptr->data == nullptr || container == nullptr)
                throw invalid_iterator();
            return ptr->data;
        }

        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        friend class list;
    };

    list() {
        init();
    }

    list(const list &other) {
        init();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }

    virtual ~list() {
        clear();
        delete head;
        delete tail;
    }

    list &operator=(const list &other) {
        if (this != &other) {
            clear();
            for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
                push_back(*it);
            }
        }
        return *this;
    }

    const T &front() const {
        if (empty())
            throw container_is_empty();
        return *(head->next->data);
    }

    const T &back() const {
        if (empty())
            throw container_is_empty();
        return *(tail->prev->data);
    }

    iterator begin() {
        return iterator(head->next, this);
    }

    const_iterator cbegin() const {
        return const_iterator(head->next, this);
    }

    iterator end() {
        return iterator(tail, this);
    }

    const_iterator cend() const {
        return const_iterator(tail, this);
    }

    virtual bool empty() const {
        return len == 0;
    }

    virtual size_t size() const {
        return len;
    }

    virtual void clear() {
        node *cur = head->next;
        while (cur != tail) {
            node *tmp = cur;
            cur = cur->next;
            delete tmp;
        }
        head->next = tail;
        tail->prev = head;
        len = 0;
    }

    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this || pos.ptr == nullptr)
            throw invalid_iterator();
        node *new_node = new node(value);
        insert(pos.ptr, new_node);
        len++;
        return iterator(new_node, this);
    }

    virtual iterator erase(iterator pos) {
        if (pos.container != this || pos.ptr == nullptr || pos.ptr == head || pos.ptr == tail)
            throw invalid_iterator();
        node *next_node = erase(pos.ptr);
        delete pos.ptr;
        len--;
        return iterator(next_node, this);
    }

    void push_back(const T &value) {
        insert(end(), value);
    }

    void pop_back() {
        if (empty())
            throw container_is_empty();
        erase(--end());
    }

    void push_front(const T &value) {
        insert(begin(), value);
    }

    void pop_front() {
        if (empty())
            throw container_is_empty();
        erase(begin());
    }

    void sort() {
        if (len <= 1)
            return;

        T **arr = new T*[len];
        node *cur = head->next;
        for (size_t i = 0; i < len; ++i) {
            arr[i] = cur->data;
            cur = cur->next;
        }

        sjtu::sort<T*>(arr, arr + len, [](const T *a, const T *b) {
            return *a < *b;
        });

        cur = head->next;
        for (size_t i = 0; i < len; ++i) {
            cur->data = arr[i];
            cur = cur->next;
        }

        delete[] arr;
    }

    void merge(list &other) {
        if (this == &other)
            return;

        node *p1 = head->next;
        node *p2 = other.head->next;

        while (p1 != tail && p2 != other.tail) {
            if (!(*(p2->data) < *(p1->data))) {
                p1 = p1->next;
            } else {
                node *tmp = p2;
                p2 = p2->next;
                insert(p1, tmp);
            }
        }

        while (p2 != other.tail) {
            node *tmp = p2;
            p2 = p2->next;
            insert(p1, tmp);
        }

        len += other.len;
        other.head->next = other.tail;
        other.tail->prev = other.head;
        other.len = 0;
    }

    void reverse() {
        node *cur = head;
        while (cur != nullptr) {
            node *tmp = cur->next;
            cur->next = cur->prev;
            cur->prev = tmp;
            cur = tmp;
        }
        std::swap(head, tail);
    }

    void unique() {
        if (len <= 1)
            return;

        node *cur = head->next;
        while (cur != tail && cur->next != tail) {
            if (*(cur->data) == *(cur->next->data)) {
                node *to_delete = cur->next;
                erase(to_delete);
                delete to_delete;
                len--;
            } else {
                cur = cur->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
