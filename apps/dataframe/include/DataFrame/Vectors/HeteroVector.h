// Hossein Moein
// September 11, 2017
/*
Copyright (c) 2019-2022, Hossein Moein
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of Hossein Moein and/or the DataFrame nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Hossein Moein BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <DataFrame/Vectors/HeteroPtrView.h>
#include <DataFrame/Vectors/HeteroView.h>

#include <unordered_map>

// ----------------------------------------------------------------------------

namespace hmdf
{

// This class implements a heterogeneous vector. Its design and implementation
// are partly inspired by Andy G's Blog at:
// https://gieseanw.wordpress.com/2017/05/03/a-true-heterogeneous-container/
//
struct  HeteroVector  {

public:

    using size_type = size_t;

    HeteroVector();
    HeteroVector(const HeteroVector &that);
    HeteroVector(HeteroVector &&that) noexcept;

    ~HeteroVector() { clear(); }

    HeteroVector &operator= (const HeteroVector &rhs);
    HeteroVector &operator= (HeteroVector &&rhs) noexcept;

    template<typename T>
    std::vector<T> &get_vector() {
        auto    iter = vectors_<T>.find (this);

        // don't have it yet, so create functions for copying and destroying
        if (iter == vectors_<T>.end())  {
            clear_functions_.emplace_back (
                [](HeteroVector &hv) { vectors_<T>.erase(&hv); });

            // if someone copies me, they need to call each
            // copy_function and pass themself
            copy_functions_.emplace_back (
                [](const HeteroVector &from, HeteroVector &to)  {
                    vectors_<T>[&to] = vectors_<T>[&from];
                });

            move_functions_.emplace_back (
                [](HeteroVector &from, HeteroVector &to)  {
                    vectors_<T>[&to] = std::move(vectors_<T>[&from]);
                    vectors_<T>.erase(vectors_<T>.find(&from));
                });

            iter = vectors_<T>.emplace (this, std::vector<T>()).first;
        }

        return (iter->second);
    }


    template<typename T>
    const std::vector<T> &get_vector() const { return (const_cast<HeteroVector *>(this)->get_vector<T>()); }

    template<typename T>
    HeteroView get_view(size_type begin = 0, size_type end = -1) {throw -1;}

    template<typename T>
    HeteroPtrView get_ptr_view(size_type begin = 0, size_type end = -1) {throw -1;}

    template<typename T>
    void push_back(const T &v) { get_vector<T>().push_back (v); }
    template<typename T, class... Args>
    void emplace_back (Args &&... args) {get_vector<T>().emplace_back (std::forward<Args>(args)...);}
    template<typename T, typename ITR, class... Args>
    void emplace (ITR pos, Args &&... args) { get_vector<T>().emplace (pos, std::forward<Args>(args)...); }

    template<typename T>
    void reserve (size_type r)  { get_vector<T>().reserve (r); }
    template<typename T>
    void shrink_to_fit () { get_vector<T>().shrink_to_fit (); }

    template<typename T>
    size_type size () const { return (get_vector<T>().size()); }

    void clear();

    template<typename T>
    void erase(size_type pos) {
        auto    &vec = get_vector<T>();
        vec.erase (vec.begin() + pos);
    }

    template<typename T>
    void resize(size_type count)  { get_vector<T>().resize (count); }

    template<typename T>
    void resize(size_type count, const T &v)  { get_vector<T>().resize (count, v); }

    template<typename T>
    void pop_back()  { get_vector<T>().pop_back (); }

    template<typename T>
    bool empty() const noexcept  { return (get_vector<T>().empty ()); }

    template<typename T>
    T &at(size_type idx)  { return (get_vector<T>().at (idx)); }

    template<typename T>
    const T &at(size_type idx) const  { return (get_vector<T>().at (idx)); }

    template<typename T>
    T &back()  { return (get_vector<T>().back ()); }

    template<typename T>
    const T &back() const  { return (get_vector<T>().back ()); }

    template<typename T>
    T &front()  { return (get_vector<T>().front ()); }

    template<typename T>
    const T &front() const  { return (get_vector<T>().front ()); }

  	template <typename T>
	class Null {};
    template<typename T>
    using iterator = Null<T>;
    template<typename T>
    using const_iterator = Null<T>;
    template<typename T>
    using reverse_iterator = Null<T>;
    template<typename T>
    using const_reverse_iterator = Null<T>;

    template<typename T>
    inline iterator<T>
    begin() noexcept { throw -1; }

    template<typename T>
    inline iterator<T>
    end() noexcept {throw -1; }

    template<typename T>
    inline const_iterator<T>
    begin () const noexcept {throw -1; }

    template<typename T>
    inline const_iterator<T>
    end () const noexcept {throw -1; }

    template<typename T>
    inline reverse_iterator<T>
    rbegin() noexcept {throw -1;}

    template<typename T>
    inline reverse_iterator<T>
    rend() noexcept {throw -1; }

    template<typename T>
    inline const_reverse_iterator<T>
    rbegin () const noexcept {throw -1;}

    template<typename T>
    inline const_reverse_iterator<T>
    rend () const noexcept {throw -1;}

private:

    template<typename T>
    inline static std::unordered_map<const HeteroVector *, std::vector<T>>
        vectors_ {  };

    std::vector<std::function<void(HeteroVector &)>>    clear_functions_;
    std::vector<std::function<void(const HeteroVector &,
                                   HeteroVector &)>>    copy_functions_;
    std::vector<std::function<void(HeteroVector &,
                                   HeteroVector &)>>    move_functions_;

    template<typename T, typename U>
    void visit_impl_help_ (T &visitor) {throw -1;}
    template<typename T, typename U>
    void visit_impl_help_ (T &visitor) const {throw -1;}
    template<typename T, typename U>
    void sort_impl_help_ (T &functor) {throw -1;}
    template<class T, template<class...> class TLIST, class... TYPES>
    void visit_impl_ (T &&visitor, TLIST<TYPES...>) {throw -1;}
    template<class T, template<class...> class TLIST, class... TYPES>
    void visit_impl_ (T &&visitor, TLIST<TYPES...>) const {throw -1;}
    template<class T, template<class...> class TLIST, class... TYPES>
    void sort_impl_ (T &&functor, TLIST<TYPES...>) {throw -1;}

    template<typename T, typename U>
    void change_impl_help_ (T &functor);
    template<typename T, typename U>
    void change_impl_help_ (T &functor) const;
    template<class T, template<class...> class TLIST, class... TYPES>
    void change_impl_ (T &&functor, TLIST<TYPES...>);
    template<class T, template<class...> class TLIST, class... TYPES>
    void change_impl_ (T &&functor, TLIST<TYPES...>) const;

public:

    template<typename... Ts>
    struct type_list  {   };
    template<typename... Ts>
    struct visitor_base  { using types = type_list<Ts ...>; };
    template<typename T>
    void visit (T &&visitor)  { throw -1; }
    template<typename T>
    void visit (T &&visitor) const  { throw -1; }
    template<typename T>
    void sort (T &&functor)  { throw -1; }

    template<typename T>
    void change (T &&functor)  {
        change_impl_ (functor, typename std::decay_t<T>::types { });
    }
    template<typename T>
    void change (T &&functor) const  {
        change_impl_ (functor, typename std::decay_t<T>::types { });
    }
};

} // namespace hmdf

// ----------------------------------------------------------------------------

#  ifndef HMDF_DO_NOT_INCLUDE_TCC_FILES
#    include <DataFrame/Vectors/HeteroVector.tcc>
#  endif // HMDF_DO_NOT_INCLUDE_TCC_FILES

// ----------------------------------------------------------------------------

// Local Variables:
// mode:C++
// tab-width:4
// c-basic-offset:4
// End:
