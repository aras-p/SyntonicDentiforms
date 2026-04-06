// Tiny replacement for boost/intrusive_ptr.hpp
//
// Requires the user to provide free functions in the same namespace as T:
//
//   void intrusive_ptr_add_ref(T* p);
//   void intrusive_ptr_release(T* p);

#pragma once

#include <cassert>

template<class T>
class intrusive_ptr {
public:
    typedef T element_type;

    intrusive_ptr() : px(0) {}

    intrusive_ptr(T* p, bool add_ref = true) : px(p) {
        if (px && add_ref)
            intrusive_ptr_add_ref(px);
    }

    intrusive_ptr(const intrusive_ptr& rhs) : px(rhs.px) {
        if (px)
            intrusive_ptr_add_ref(px);
    }

    ~intrusive_ptr() {
        if (px)
            intrusive_ptr_release(px);
    }

    intrusive_ptr& operator=(const intrusive_ptr& rhs) {
        intrusive_ptr(rhs).swap(*this);
        return *this;
    }

    intrusive_ptr& operator=(T* rhs) {
        intrusive_ptr(rhs).swap(*this);
        return *this;
    }

    T* get() const { return px; }

    T& operator*()  const { assert(px); return *px; }
    T* operator->() const { assert(px); return px; }

    typedef T* intrusive_ptr::*unspecified_bool_type;
    operator unspecified_bool_type() const {
        return px == 0 ? 0 : &intrusive_ptr::px;
    }

    bool operator!() const { return px == 0; }

    void swap(intrusive_ptr& rhs) {
        T* tmp = px;
        px = rhs.px;
        rhs.px = tmp;
    }

private:
    T* px;
};

template<class T, class U>
bool operator==(const intrusive_ptr<T>& a, const intrusive_ptr<U>& b) { return a.get() == b.get(); }

template<class T, class U>
bool operator!=(const intrusive_ptr<T>& a, const intrusive_ptr<U>& b) { return a.get() != b.get(); }

template<class T>
bool operator==(const intrusive_ptr<T>& a, T* b) { return a.get() == b; }

template<class T>
bool operator!=(const intrusive_ptr<T>& a, T* b) { return a.get() != b; }

template<class T>
bool operator==(T* a, const intrusive_ptr<T>& b) { return a == b.get(); }

template<class T>
bool operator!=(T* a, const intrusive_ptr<T>& b) { return a != b.get(); }

template<class T>
bool operator<(const intrusive_ptr<T>& a, const intrusive_ptr<T>& b) { return a.get() < b.get(); }

template<class T>
void swap(intrusive_ptr<T>& a, intrusive_ptr<T>& b) { a.swap(b); }

template<class T>
T* get_pointer(const intrusive_ptr<T>& p) { return p.get(); }
