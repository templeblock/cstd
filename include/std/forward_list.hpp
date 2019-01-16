// forward_list standard header
#pragma once
#ifndef _FORWARD_LIST_
#define _FORWARD_LIST_
#ifndef RC_INVOKED
#include <xmemory>
#include <stdexcept>

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)
#pragma push_macro("new")
#undef new

#pragma warning(disable: 4127)
_STD_BEGIN
// TEMPLATE CLASS _Flist_unchecked_const_iterator
template < class _Mylist,
         class _Base = _Iterator_base0 >
class _Flist_unchecked_const_iterator
  : public _Iterator012 < forward_iterator_tag,
    typename _Mylist::value_type,
    typename _Mylist::difference_type,
    typename _Mylist::const_pointer,
    typename _Mylist::const_reference,
    _Base >
{
  // unchecked iterator for nonmutable list
public:
  typedef _Flist_unchecked_const_iterator<_Mylist, _Base> _Myiter;
  typedef forward_iterator_tag iterator_category;

  typedef typename _Mylist::_Nodeptr _Nodeptr;
  typedef typename _Mylist::value_type value_type;
  typedef typename _Mylist::difference_type difference_type;
  typedef typename _Mylist::const_pointer pointer;
  typedef typename _Mylist::const_reference reference;

  _Flist_unchecked_const_iterator()
    : _Ptr() {
    // construct with null node pointer
  }

  _Flist_unchecked_const_iterator(_Nodeptr _Pnode, const _Mylist* _Plist)
    : _Ptr(_Pnode) {
    // construct with node pointer _Pnode
    this->_Adopt(_Plist);
  }

  reference operator*() const {
    // return designated value
    return (_Mylist::_Myval(_Ptr));
  }

  pointer operator->() const {
    // return pointer to class object
    return (pointer_traits<pointer>::pointer_to(**this));
  }

  _Myiter & operator++() {
    // preincrement
    _Ptr = _Mylist::_Nextnode(_Ptr);
    return (*this);
  }

  _Myiter operator++(int) {
    // postincrement
    _Myiter _Tmp = *this;
    ++*this;
    return (_Tmp);
  }

  bool operator==(const _Myiter & _Right) const {
    // test for iterator equality
    return (_Ptr == _Right._Ptr);
  }

  bool operator!=(const _Myiter & _Right) const {
    // test for iterator inequality
    return (!(*this == _Right));
  }

  _Nodeptr _Mynode() const {
    // return node pointer
    return (_Ptr);
  }

  _Nodeptr _Ptr;	// pointer to node
};

// TEMPLATE CLASS _Flist_unchecked_iterator
template<class _Mylist>
class _Flist_unchecked_iterator
  : public _Flist_unchecked_const_iterator<_Mylist>
{
  // unchecked iterator for mutable list
public:
  typedef _Flist_unchecked_iterator<_Mylist> _Myiter;
  typedef _Flist_unchecked_const_iterator<_Mylist> _Mybase;
  typedef forward_iterator_tag iterator_category;

  typedef typename _Mylist::_Nodeptr _Nodeptr;
  typedef typename _Mylist::value_type value_type;
  typedef typename _Mylist::difference_type difference_type;
  typedef typename _Mylist::pointer pointer;
  typedef typename _Mylist::reference reference;

  _Flist_unchecked_iterator() {
    // construct with null node
  }

  _Flist_unchecked_iterator(_Nodeptr _Pnode, const _Mylist* _Plist)
    : _Mybase(_Pnode, _Plist) {
    // construct with node pointer _Pnode
  }

  reference operator*() const {
    // return designated value
    return ((reference)** (_Mybase*)this);
  }

  pointer operator->() const {
    // return pointer to class object
    return (pointer_traits<pointer>::pointer_to(**this));
  }

  _Myiter & operator++() {
    // preincrement
    ++(*(_Mybase*)this);
    return (*this);
  }

  _Myiter operator++(int) {
    // postincrement
    _Myiter _Tmp = *this;
    ++*this;
    return (_Tmp);
  }
};

// TEMPLATE CLASS _Flist_const_iterator
template<class _Mylist>
class _Flist_const_iterator
  : public _Flist_unchecked_const_iterator<_Mylist, _Iterator_base>
{
  // iterator for nonmutable list
public:
  typedef _Flist_const_iterator<_Mylist> _Myiter;
  typedef _Flist_unchecked_const_iterator<_Mylist, _Iterator_base> _Mybase;
  typedef forward_iterator_tag iterator_category;

  typedef typename _Mylist::_Nodeptr _Nodeptr;
  typedef typename _Mylist::value_type value_type;
  typedef typename _Mylist::difference_type difference_type;
  typedef typename _Mylist::const_pointer pointer;
  typedef typename _Mylist::const_reference reference;

  _Flist_const_iterator()
    : _Mybase() {
    // construct with null node pointer
  }

  _Flist_const_iterator(_Nodeptr _Pnode, const _Mylist* _Plist)
    : _Mybase(_Pnode, _Plist) {
    // construct with node pointer _Pnode
  }

  typedef _Flist_unchecked_const_iterator<_Mylist> _Unchecked_type;

  _Myiter & _Rechecked(_Unchecked_type _Right) {
    // reset from unchecked iterator
    this->_Ptr = _Right._Ptr;
    return (*this);
  }

  _Unchecked_type _Unchecked() const {
    // make an unchecked iterator
    return (_Unchecked_type(this->_Ptr, static_cast<const _Mylist*>(this->_Getcont())));
  }

  reference operator*() const {
    // return designated value
#if _ITERATOR_DEBUG_LEVEL == 2
    const auto _Mycont = static_cast<const _Mylist*>(this->_Getcont());
    if (_Mycont == 0
        || this->_Ptr == nullptr_t {}
        || this->_Ptr == _Mycont->_Before_head()) {
      // report error
      _DEBUG_ERROR("forward_list iterator not dereferencable");
      _SCL_SECURE_OUT_OF_RANGE;
    }
#elif _ITERATOR_DEBUG_LEVEL == 1
    const auto _Mycont = static_cast<const _Mylist*>(this->_Getcont());
    _SCL_SECURE_VALIDATE(_Mycont != 0 && this->_Ptr != nullptr_t {});
    _SCL_SECURE_VALIDATE_RANGE(this->_Ptr != _Mycont->_Before_head());
#endif /* _ITERATOR_DEBUG_LEVEL */
    return (_Mylist::_Myval(this->_Ptr));
  }

  _Myiter & operator++() {
    // preincrement
#if _ITERATOR_DEBUG_LEVEL == 2
    if (this->_Getcont() == 0
        || this->_Ptr == nullptr_t {}) {
      // report error
      _DEBUG_ERROR("forward_list iterator not incrementable");
      _SCL_SECURE_OUT_OF_RANGE;
    }
#elif _ITERATOR_DEBUG_LEVEL == 1
    _SCL_SECURE_VALIDATE(this->_Getcont() != 0 && this->_Ptr != nullptr_t {});
#endif /* _ITERATOR_DEBUG_LEVEL */
    this->_Ptr = _Mylist::_Nextnode(this->_Ptr);
    return (*this);
  }

  _Myiter operator++(int) {
    // postincrement
    _Myiter _Tmp = *this;
    ++*this;
    return (_Tmp);
  }

  bool operator==(const _Myiter & _Right) const {
    // test for iterator equality
#if _ITERATOR_DEBUG_LEVEL == 2
    if (this->_Getcont() != _Right._Getcont()) {
      // report error
      _DEBUG_ERROR("forward_list iterators incompatible");
      _SCL_SECURE_INVALID_ARGUMENT;
    }
#elif _ITERATOR_DEBUG_LEVEL == 1
    _SCL_SECURE_VALIDATE(this->_Getcont() == _Right._Getcont());
#endif /* _ITERATOR_DEBUG_LEVEL */
    return (this->_Ptr == _Right._Ptr);
  }

  bool operator!=(const _Myiter & _Right) const {
    // test for iterator inequality
    return (!(*this == _Right));
  }
};

template<class _Mylist> inline
typename _Flist_const_iterator<_Mylist>::_Unchecked_type
_Unchecked(_Flist_const_iterator<_Mylist> _Iter)
{
  // convert to unchecked
  return (_Iter._Unchecked());
}

template<class _Mylist> inline
_Flist_const_iterator<_Mylist> &
_Rechecked(_Flist_const_iterator<_Mylist> & _Iter,
           typename _Flist_const_iterator<_Mylist>
           ::_Unchecked_type _Right)
{
  // convert to checked
  return (_Iter._Rechecked(_Right));
}

// TEMPLATE CLASS _Flist_iterator
template<class _Mylist>
class _Flist_iterator
  : public _Flist_const_iterator<_Mylist>
{
  // iterator for mutable list
public:
  typedef _Flist_iterator<_Mylist> _Myiter;
  typedef _Flist_const_iterator<_Mylist> _Mybase;
  typedef forward_iterator_tag iterator_category;

  typedef typename _Mylist::_Nodeptr _Nodeptr;
  typedef typename _Mylist::value_type value_type;
  typedef typename _Mylist::difference_type difference_type;
  typedef typename _Mylist::pointer pointer;
  typedef typename _Mylist::reference reference;

  _Flist_iterator() {
    // construct with null node
  }

  _Flist_iterator(_Nodeptr _Pnode, const _Mylist* _Plist)
    : _Mybase(_Pnode, _Plist) {
    // construct with node pointer _Pnode
  }

  typedef _Flist_unchecked_iterator<_Mylist> _Unchecked_type;

  _Myiter & _Rechecked(_Unchecked_type _Right) {
    // reset from unchecked iterator
    this->_Ptr = _Right._Ptr;
    return (*this);
  }

  _Unchecked_type _Unchecked() const {
    // make an unchecked iterator
    return (_Unchecked_type(this->_Ptr, static_cast<const _Mylist*>(this->_Getcont())));
  }

  reference operator*() const {
    // return designated value
    return ((reference)** (_Mybase*)this);
  }

  pointer operator->() const {
    // return pointer to class object
    return (pointer_traits<pointer>::pointer_to(**this));
  }

  _Myiter & operator++() {
    // preincrement
    ++(*(_Mybase*)this);
    return (*this);
  }

  _Myiter operator++(int) {
    // postincrement
    _Myiter _Tmp = *this;
    ++*this;
    return (_Tmp);
  }
};

template<class _Mylist> inline
typename _Flist_iterator<_Mylist>::_Unchecked_type
_Unchecked(_Flist_iterator<_Mylist> _Iter)
{
  // convert to unchecked
  return (_Iter._Unchecked());
}

template<class _Mylist> inline
_Flist_iterator<_Mylist> &
_Rechecked(_Flist_iterator<_Mylist> & _Iter,
           typename _Flist_iterator<_Mylist>
           ::_Unchecked_type _Right)
{
  // convert to checked
  return (_Iter._Rechecked(_Right));
}

// forward_list TYPE WRAPPERS
template < class _Value_type,
         class _Size_type,
         class _Difference_type,
         class _Pointer,
         class _Const_pointer,
         class _Reference,
         class _Const_reference,
         class _Nodeptr_type >
struct _Flist_iter_types
{
  // wraps types needed by iterators
  typedef _Value_type value_type;
  typedef _Size_type size_type;
  typedef _Difference_type difference_type;
  typedef _Pointer pointer;
  typedef _Const_pointer const_pointer;
  typedef _Reference reference;
  typedef _Const_reference const_reference;
  typedef _Nodeptr_type _Nodeptr;
};

template < class _Value_type,
         class _Voidptr >
struct _Flist_node
{
  // forward_list node
  _Voidptr _Next;	// successor node
  _Value_type _Myval;	// the stored value

private:
  _Flist_node & operator=(const _Flist_node &);
};

template<class _Value_type>
struct _Flist_node<_Value_type, void*> {
  // forward_list node
  typedef _Flist_node<_Value_type, void*>* _Nodeptr;
  _Nodeptr _Next;	// successor node
  _Value_type _Myval;	// the stored value

private:
  _Flist_node & operator=(const _Flist_node &);
};

template<class _Ty>
struct _Flist_simple_types
    : public _Simple_types<_Ty> {
  // wraps types needed by iterators
  typedef _Flist_node<_Ty, void*> _Node;
  typedef _Node* _Nodeptr;
};

template < class _Ty,
         class _Alloc0 >
struct _Flist_base_types
{
  // types needed for a container base
  typedef _Alloc0 _Alloc;
  typedef _Flist_base_types<_Ty, _Alloc> _Myt;

  typedef _Wrap_alloc<_Alloc> _Alty0;
  typedef typename _Alty0::template rebind<_Ty>::other _Alty;


  typedef typename _Get_voidptr<_Alty, typename _Alty::pointer>::type
  _Voidptr;
  typedef _Flist_node < typename _Alty::value_type,
          _Voidptr > _Node;

  typedef typename _Alty::template rebind<_Node>::other _Alnod_type;
  typedef typename _Alnod_type::pointer _Nodeptr;
  typedef _Nodeptr & _Nodepref;

  typedef typename _If < _Is_simple_alloc<_Alty>::value,
          _Flist_simple_types<typename _Alty::value_type>,
          _Flist_iter_types < typename _Alty::value_type,
          typename _Alty::size_type,
          typename _Alty::difference_type,
          typename _Alty::pointer,
          typename _Alty::const_pointer,
          typename _Alty::reference,
          typename _Alty::const_reference,
          _Nodeptr > >::type
          _Val_types;
};

// TEMPLATE CLASS _Flist_val
template<class _Val_types>
class _Flist_val
  : public _Container_base
{
  // base class for forward_list to hold data
public:
  typedef _Flist_val<_Val_types> _Myt;

  typedef typename _Val_types::_Nodeptr _Nodeptr;
  typedef _Nodeptr & _Nodepref;
  typedef typename pointer_traits<_Nodeptr>::element_type _Node;

  typedef typename _Val_types::value_type value_type;
  typedef typename _Val_types::size_type size_type;
  typedef typename _Val_types::difference_type difference_type;
  typedef typename _Val_types::pointer pointer;
  typedef typename _Val_types::const_pointer const_pointer;
  typedef typename _Val_types::reference reference;
  typedef typename _Val_types::const_reference const_reference;

  typedef _Flist_const_iterator<_Myt> const_iterator;
  typedef _Flist_iterator<_Myt> iterator;

  _Flist_val()
    : _Myhead() {
    // initialize data
  }

  static _Nodepref _Nextnode(_Nodeptr _Pnode) {
    // return reference to successor pointer in node
    return ((_Nodepref)_Pnode->_Next);
  }

  static reference _Myval(_Nodeptr _Pnode) {
    // return reference to value in node
    return ((reference)_Pnode->_Myval);
  }

  _Nodeptr _Before_head() const _NOEXCEPT {
    // return pointer to the "before begin" pseudo node
    return (pointer_traits<_Nodeptr>::pointer_to(
              reinterpret_cast<_Node &>(const_cast<_Nodeptr &>(_Myhead))));
  }

  _Nodeptr _Myhead;	// pointer to head node
};

// TEMPLATE CLASS _Flist_alloc
template<class _Alloc_types>
class _Flist_alloc
{
  // base class for forward_list to hold allocator
public:
  typedef _Flist_alloc<_Alloc_types> _Myt;
  typedef typename _Alloc_types::_Alloc _Alloc;
  typedef typename _Alloc_types::_Alnod_type _Alty;
  typedef typename _Alloc_types::_Node _Node;
  typedef typename _Alloc_types::_Nodeptr _Nodeptr;
  typedef typename _Alloc_types::_Val_types _Val_types;

  typedef _Nodeptr & _Nodepref;

  typedef typename _Val_types::value_type value_type;
  typedef typename _Val_types::size_type size_type;
  typedef typename _Val_types::difference_type difference_type;
  typedef typename _Val_types::pointer pointer;
  typedef typename _Val_types::const_pointer const_pointer;
  typedef typename _Val_types::reference reference;
  typedef typename _Val_types::const_reference const_reference;

  typedef _Flist_const_iterator<_Flist_val<_Val_types> > const_iterator;
  typedef _Flist_iterator<_Flist_val<_Val_types> > iterator;

  static _Nodepref _Nextnode(_Nodeptr _Pnode) {
    // return reference to successor pointer in node
    return (_Flist_val<_Val_types>::_Nextnode(_Pnode));
  }

  static reference _Myval(_Nodeptr _Pnode) {
    // return reference to value in node
    return (_Flist_val<_Val_types>::_Myval(_Pnode));
  }

#if _ITERATOR_DEBUG_LEVEL == 0
  _Flist_alloc()
    : _Mypair(_Zero_then_variadic_args_t()) {
    // default construct allocator
  }

  template < class _Any_alloc,
           class = enable_if_t < !is_same<decay_t<_Any_alloc>, _Myt>::value > >
  _Flist_alloc(_Any_alloc && _Al)
    : _Mypair(_One_then_variadic_args_t(),
              _STD forward<_Any_alloc>(_Al))
  {
    // construct allocator from _Al
  }

  ~_Flist_alloc() _NOEXCEPT {
    // destroy head node
  }

  void _Copy_alloc(const _Alty & _Al) {
    // replace old allocator
    _Pocca(_Getal(), _Al);
  }

  void _Move_alloc(_Alty & _Al) {
    // replace old allocator
    _Pocma(_Getal(), _Al);
  }

#else /* _ITERATOR_DEBUG_LEVEL == 0 */
  _Flist_alloc()
    : _Mypair(_Zero_then_variadic_args_t()) {
    // default construct allocator
    _Alloc_proxy();
  }

  template < class _Any_alloc,
           class = enable_if_t < !is_same<decay_t<_Any_alloc>, _Myt>::value > >
  _Flist_alloc(_Any_alloc && _Al)
    : _Mypair(_One_then_variadic_args_t(),
              _STD forward<_Any_alloc>(_Al))
  {
    // construct allocator from _Al
    _Alloc_proxy();
  }

  ~_Flist_alloc() _NOEXCEPT {
    // destroy proxy
    _Free_proxy();
  }

  void _Copy_alloc(const _Alty & _Al) {
    // replace old allocator
    _Free_proxy();
    _Pocca(_Getal(), _Al);
    _Alloc_proxy();
  }

  void _Move_alloc(_Alty & _Al) {
    // replace old allocator
    _Free_proxy();
    _Pocma(_Getal(), _Al);
    _Alloc_proxy();
  }

  void _Alloc_proxy() {
    // construct proxy
    typename _Alty::template rebind<_Container_proxy>::other
    _Alproxy(_Getal());
    _Myproxy() = _Unfancy(_Alproxy.allocate(1));
    _Alproxy.construct(_Myproxy(), _Container_proxy());
    _Myproxy()->_Mycont = &_Get_data();
  }

  void _Free_proxy() {
    // destroy proxy
    typename _Alty::template rebind<_Container_proxy>::other
    _Alproxy(_Getal());
    _Orphan_all();
    _Alproxy.destroy(_Myproxy());
    _Alproxy.deallocate(_Alproxy.address(*_Myproxy()), 1);
    _Myproxy() = 0;
  }

  _Iterator_base12** _Getpfirst() const {
    // get address of iterator chain
    return (_Get_data()._Getpfirst());
  }

  _Container_proxy* & _Myproxy() _NOEXCEPT {
    // return reference to _Myproxy
    return (_Get_data()._Myproxy);
  }

  _Container_proxy* const & _Myproxy() const _NOEXCEPT {
    // return const reference to _Myproxy
    return (_Get_data()._Myproxy);
  }
#endif /* _ITERATOR_DEBUG_LEVEL == 0 */

  void _Orphan_all() {
    // orphan all iterators
    _Get_data()._Orphan_all();
  }

  void _Swap_all(_Myt & _Right) {
    // swap all iterators
    _Get_data()._Swap_all(_Right._Get_data());
  }

  _Alty & _Getal() _NOEXCEPT {
    // return reference to allocator
    return (_Mypair._Get_first());
  }

  const _Alty & _Getal() const _NOEXCEPT {
    // return const reference to allocator
    return (_Mypair._Get_first());
  }

  _Flist_val<_Val_types> & _Get_data() _NOEXCEPT {
    // return reference to _Flist_val
    return (_Mypair._Get_second());
  }

  const _Flist_val<_Val_types> & _Get_data() const _NOEXCEPT {
    // return const reference to _Flist_val
    return (_Mypair._Get_second());
  }

  _Nodeptr & _Myhead() _NOEXCEPT {
    // return reference to _Myhead
    return (_Get_data()._Myhead);
  }

  const _Nodeptr & _Myhead() const _NOEXCEPT {
    // return const reference to _Myhead
    return (_Get_data()._Myhead);
  }

  _Nodeptr _Before_head() const _NOEXCEPT {
    // return pointer to the "before begin" pseudo node
    return (_Get_data()._Before_head());
  }

private:
  _Compressed_pair<_Alty, _Flist_val<_Val_types> > _Mypair;
};

// TEMPLATE CLASS _Flist_buy
template < class _Ty,
         class _Alloc >
class _Flist_buy
  : public _Flist_alloc<_Flist_base_types<_Ty, _Alloc> >
{
  // base class for forward_list to hold buynode/freenode functions
public:
  typedef _Flist_buy<_Ty, _Alloc> _Myt;
  typedef _Flist_alloc<_Flist_base_types<_Ty, _Alloc> > _Mybase;
  typedef typename _Mybase::_Alty _Alty;
  typedef typename _Mybase::_Nodeptr _Nodeptr;

  _Flist_buy()
    : _Mybase() {
    // default construct
  }

  template < class _Any_alloc,
           class = enable_if_t < !is_same<decay_t<_Any_alloc>, _Myt>::value > >
  _Flist_buy(_Any_alloc && _Al)
    : _Mybase(_STD forward<_Any_alloc>(_Al))
  {
    // construct from allocator
  }

  _Nodeptr _Buynode0(_Nodeptr _Next) {
    // allocate a node and set links
    _Nodeptr _Pnode = this->_Getal().allocate(1);
    _TRY_BEGIN
    this->_Getal().construct(
      _STD addressof(this->_Nextnode(_Pnode)), _Next);
    _CATCH_ALL
    this->_Getal().deallocate(_Pnode, 1);
    _RERAISE;
    _CATCH_END
    return (_Pnode);
  }

  template<class... _Valty>
  _Nodeptr _Buynode(_Nodeptr _Next,
                    _Valty && ... _Val) {
    // allocate a node and set links and value
    _Nodeptr _Pnode = this->_Buynode0(_Next);
    _TRY_BEGIN
    this->_Getal().construct(
      _STD addressof(this->_Myval(_Pnode)),
      _STD forward<_Valty>(_Val)...);
    _CATCH_ALL
    this->_Getal().deallocate(_Pnode, 1);
    _RERAISE;
    _CATCH_END
    return (_Pnode);
  }


  void _Freenode(_Nodeptr _Pnode) {
    // give node back
    this->_Getal().destroy(
      _STD addressof(this->_Nextnode(_Pnode)));
    this->_Getal().destroy(
      _STD addressof(this->_Myval(_Pnode)));
    this->_Getal().deallocate(_Pnode, 1);
  }
};

// TEMPLATE CLASS forward_list
template < class _Ty,
         class _Alloc = allocator<_Ty> >
class forward_list
  : public _Flist_buy<_Ty, _Alloc>
{
  // singly linked list
public:
  typedef forward_list<_Ty, _Alloc> _Myt;
  typedef _Flist_buy<_Ty, _Alloc> _Mybase;
  typedef typename _Mybase::_Node _Node;
  typedef typename _Mybase::_Nodeptr _Nodeptr;
  typedef typename _Mybase::_Alty _Alty;

  typedef _Alloc allocator_type;
  typedef typename _Mybase::size_type size_type;
  typedef typename _Mybase::difference_type difference_type;
  typedef typename _Mybase::pointer pointer;
  typedef typename _Mybase::const_pointer const_pointer;
  typedef typename _Mybase::reference reference;
  typedef typename _Mybase::const_reference const_reference;
  typedef typename _Mybase::value_type value_type;

  typedef typename _Mybase::const_iterator const_iterator;
  typedef typename _Mybase::iterator iterator;

  forward_list()
    : _Mybase() {
    // construct empty list
  }

  explicit forward_list(size_type _Count)
    : _Mybase() {
    // construct list from _Count * _Ty()
    resize(_Count);
  }

  forward_list(size_type _Count, const _Ty & _Val)
    : _Mybase() {
    // construct list from _Count * _Val
    _Construct_n(_Count, _Val);
  }

  forward_list(size_type _Count, const _Ty & _Val, const _Alloc & _Al)
    : _Mybase(_Al) {
    // construct list from _Count * _Val, allocator
    _Construct_n(_Count, _Val);
  }

  explicit forward_list(const _Alloc & _Al)
    : _Mybase(_Al) {
    // construct empty list, allocator
  }

  forward_list(const _Myt & _Right)

    : _Mybase(_Right._Getal().select_on_container_copy_construction())


  {
    // construct list by copying _Right
    _TRY_BEGIN
    insert_after(before_begin(), _Right.begin(), _Right.end());
    _CATCH_ALL
    _Tidy();
    _RERAISE;
    _CATCH_END
  }

  forward_list(const _Myt & _Right, const _Alloc & _Al)
    : _Mybase(_Al) {
    // construct list by copying _Right, allocator
    _TRY_BEGIN
    insert_after(before_begin(), _Right.begin(), _Right.end());
    _CATCH_ALL
    _Tidy();
    _RERAISE;
    _CATCH_END
  }

  template < class _Iter,
           class = typename enable_if < _Is_iterator<_Iter>::value,
           void >::type >
  forward_list(_Iter _First, _Iter _Last)
    : _Mybase()
  {
    // construct list from [_First, _Last,
    _Construct(_First, _Last);
  }

  template < class _Iter,
           class = typename enable_if < _Is_iterator<_Iter>::value,
           void >::type >
  forward_list(_Iter _First, _Iter _Last, const _Alloc & _Al)
    : _Mybase(_Al)
  {
    // construct list, allocator from [_First, _Last)
    _Construct(_First, _Last);
  }

  template<class _Iter>
  void _Construct(_Iter _First, _Iter _Last) {
    // construct list from [_First, _Last), input iterators
    _TRY_BEGIN
    insert_after(before_begin(), _First, _Last);
    _CATCH_ALL
    _Tidy();
    _RERAISE;
    _CATCH_END
  }

  void _Construct_n(size_type _Count,
                    const _Ty & _Val) {
    // construct from _Count * _Val
    _TRY_BEGIN
    _Insert_n_after(before_begin(), _Count, _Val);
    _CATCH_ALL
    _Tidy();
    _RERAISE;
    _CATCH_END
  }

  forward_list(_Myt && _Right)
    : _Mybase(_STD move(_Right._Getal())) {
    // construct list by moving _Right
    _Assign_rv(_STD forward<_Myt>(_Right));
  }

  forward_list(_Myt && _Right, const _Alloc & _Al)
    : _Mybase(_Al) {
    // construct list by moving _Right, allocator
    _Assign_rv(_STD forward<_Myt>(_Right));
  }

  _Myt & operator=(_Myt && _Right)
  _NOEXCEPT_OP(_Alty::is_always_equal::value) {
    // assign by moving _Right
    if (this != &_Right) {
      // different, assign it
      clear();
      if (_Alty::propagate_on_container_move_assignment::value
          && this->_Getal() != _Right._Getal()) {
        this->_Move_alloc(_Right._Getal());
      }
      _Assign_rv(_STD forward<_Myt>(_Right));
    }
    return (*this);
  }

  void _Assign_rv(_Myt && _Right) {
    // swap with empty *this, same allocator
    this->_Swap_all(_Right);
    _Swap_adl(this->_Myhead(), _Right._Myhead());
  }

  void push_front(_Ty && _Val) {
    // insert element at beginning
    _Insert_after(before_begin(), _STD forward<_Ty>(_Val));
  }

  iterator insert_after(const_iterator _Where, _Ty && _Val) {
    // insert _Val at _Where
    return (emplace_after(_Where, _STD forward<_Ty>(_Val)));
  }

  template<class... _Valty>
  void emplace_front(_Valty && ... _Val) {
    // insert element at beginning
    _Insert_after(before_begin(), _STD forward<_Valty>(_Val)...);
  }

  template<class... _Valty>
  iterator emplace_after(const_iterator _Where, _Valty && ... _Val) {
    // insert element at _Where
    _Insert_after(_Where, _STD forward<_Valty>(_Val)...);
    return (_Make_iter(++_Where));
  }

  template<class... _Valty>
  void _Insert_after(const_iterator _Where,
                     _Valty && ... _Val) {
    // insert element after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
    if (_Where._Getcont() != &this->_Get_data()) {
      _DEBUG_ERROR("forward_list insert_after iterator outside range");
    }
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */
    _Nodeptr _Pnode = _Where._Mynode();
    _Nodeptr _Newnode =
      this->_Buynode(this->_Nextnode(_Pnode),
                     _STD forward<_Valty>(_Val)...);
    this->_Nextnode(_Pnode) = _Newnode;
  }


  forward_list(_XSTD initializer_list<_Ty> _Ilist,
               const _Alloc & _Al = allocator_type())
    : _Mybase(_Al) {
    // construct from initializer_list
    insert_after(before_begin(), _Ilist.begin(), _Ilist.end());
  }

  _Myt & operator=(_XSTD initializer_list<_Ty> _Ilist) {
    // assign initializer_list
    assign(_Ilist.begin(), _Ilist.end());
    return (*this);
  }

  void assign(_XSTD initializer_list<_Ty> _Ilist) {
    // assign initializer_list
    assign(_Ilist.begin(), _Ilist.end());
  }

  iterator insert_after(const_iterator _Where,
                        _XSTD initializer_list<_Ty> _Ilist) {
    // insert initializer_list
    return (insert_after(_Where, _Ilist.begin(), _Ilist.end()));
  }

  ~forward_list() _NOEXCEPT {
    // destroy the object
    _Tidy();
  }

  _Myt & operator=(const _Myt & _Right) {
    // assign _Right
    if (this != &_Right) {
      // different, assign it
      if (this->_Getal() != _Right._Getal()
          && _Alty::propagate_on_container_copy_assignment::value) {
        // change allocator before copying
        clear();
        this->_Copy_alloc(_Right._Getal());
      }
      assign(_Right.begin(), _Right.end());
    }
    return (*this);
  }

  iterator before_begin() _NOEXCEPT {
    // return iterator before beginning of mutable sequence
    return (iterator(this->_Before_head(), &this->_Get_data()));
  }

  const_iterator before_begin() const _NOEXCEPT {
    // return iterator before beginning of nonmutable sequence
    return (const_iterator(this->_Before_head(), &this->_Get_data()));
  }

  const_iterator cbefore_begin() const _NOEXCEPT {
    // return iterator before beginning of nonmutable sequence
    return (before_begin());
  }

  iterator begin() _NOEXCEPT {
    // return iterator for beginning of mutable sequence
    return (iterator(this->_Myhead(), &this->_Get_data()));
  }

  const_iterator begin() const _NOEXCEPT {
    // return iterator for beginning of nonmutable sequence
    return (const_iterator(this->_Myhead(), &this->_Get_data()));
  }

  iterator end() _NOEXCEPT {
    // return iterator for end of mutable sequence
    return (iterator(nullptr_t{}, &this->_Get_data()));
  }

  const_iterator end() const _NOEXCEPT {
    // return iterator for end of nonmutable sequence
    return (const_iterator(nullptr_t {}, &this->_Get_data()));
  }

  iterator _Make_iter(const_iterator _Where) const {
    // make iterator from const_iterator
    return (iterator(_Where._Ptr, &this->_Get_data()));
  }

  const_iterator cbegin() const _NOEXCEPT {
    // return iterator for beginning of nonmutable sequence
    return (begin());
  }

  const_iterator cend() const _NOEXCEPT {
    // return iterator for end of nonmutable sequence
    return (end());
  }

  void resize(size_type _Newsize) {
    // determine new length, padding with _Ty() elements as needed
    size_type _Cursize = _Size();
    if (_Cursize < _Newsize) {
      // pad to make larger
      const_iterator _Next = _Before_end();
      _TRY_BEGIN
      for (; _Cursize < _Newsize; ++_Cursize) {
        _Insert_after(_Next);
      }
      _CATCH_ALL
      erase_after(_Next, end());
      _RERAISE;
      _CATCH_END
    } else if (_Newsize < _Cursize) {
      // erase all but _Newsize elements
      iterator _Next = before_begin();
      for (; 0 < _Newsize; --_Newsize) {
        ++_Next;
      }
      erase_after(_Next, end());
    }
  }

  void resize(size_type _Newsize, const _Ty & _Val) {
    // determine new length, padding with _Val elements as needed
    size_type _Cursize = _Size();
    if (_Cursize < _Newsize) {
      _Insert_n_after(_Before_end(), _Newsize - _Cursize, _Val);
    } else if (_Newsize < _Cursize) {
      // erase all but _Newsize elements
      iterator _Next = before_begin();
      for (; 0 < _Newsize; --_Newsize) {
        ++_Next;
      }
      erase_after(_Next, end());
    }
  }

  size_type max_size() const _NOEXCEPT {
    // return maximum possible length of sequence
    return (this->_Getal().max_size());
  }

  bool empty() const _NOEXCEPT {
    // test if sequence is empty
    return (begin() == end());
  }

  allocator_type get_allocator() const _NOEXCEPT {
    // return allocator object for values
    allocator_type _Ret(this->_Getal());
    return (_Ret);
  }

  reference front() {
    // return first element of mutable sequence
    return (*begin());
  }

  const_reference front() const {
    // return first element of nonmutable sequence
    return (*begin());
  }

  void push_front(const _Ty & _Val) {
    // insert element at beginning
    _Insert_after(before_begin(), _Val);
  }

  void pop_front() {
    // erase element at beginning
    erase_after(before_begin());
  }

  template<class _Iter>
  typename enable_if < _Is_iterator<_Iter>::value,
           void >::type
  assign(_Iter _First, _Iter _Last) {
    // assign [_First, _Last), input iterators
    clear();
    insert_after(before_begin(), _First, _Last);
  }

  void assign(size_type _Count, const _Ty & _Val) {
    // assign _Count * _Val
    clear();
    _Insert_n_after(before_begin(), _Count, _Val);
  }

  iterator insert_after(const_iterator _Where, const _Ty & _Val) {
    // insert _Val at _Where
    _Insert_after(_Where, _Val);
    return (_Make_iter(++_Where));
  }

  iterator insert_after(const_iterator _Where,
  size_type _Count, const _Ty & _Val) {
    // insert _Count * _Val at _Where
    return (_Insert_n_after(_Where, _Count, _Val));
  }

  template<class _Iter>
  typename enable_if < _Is_iterator<_Iter>::value,
           iterator >::type
           insert_after(const_iterator _Where,
  _Iter _First, _Iter _Last) {
    // insert [_First, _Last) at _Where
    return (_Insert_range(_Where, _First, _Last, _Iter_cat_t<_Iter>()));
  }

  template<class _Iter>
  iterator _Insert_range(const_iterator _Where,
                         _Iter _First, _Iter _Last, input_iterator_tag) {
    // insert [_First, _Last) after _Where, input iterators
    size_type _Num = 0;
    const_iterator _After = _Where;
    _TRY_BEGIN
    for (; _First != _Last; ++_After, (void)++_First, ++_Num) {
      _Insert_after(_After, *_First);
    }
    _CATCH_ALL
    for (; 0 < _Num; --_Num) {
      erase_after(_Where);
    }
    _RERAISE;
    _CATCH_END
    return (_Make_iter(_After));
  }

  template<class _Iter>
  iterator _Insert_range(const_iterator _Where,
                         _Iter _First, _Iter _Last, forward_iterator_tag) {
    // insert [_First, _Last) after _Where, forward iterators
    _DEBUG_RANGE(_First, _Last);
    _Iter _Next = _First;
    const_iterator _After = _Where;
    _TRY_BEGIN
    for (; _First != _Last; ++_After, (void)++_First) {
      _Insert_after(_After, *_First);
    }
    _CATCH_ALL
    for (; _Next != _First; ++_Next) {
      erase_after(_Where);
    }
    _RERAISE;
    _CATCH_END
    return (_Make_iter(_After));
  }

  iterator erase_after(const_iterator _Where) {
    // erase element after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
    if (_Where._Getcont() != &this->_Get_data()
        || _Where == end()) {
      _DEBUG_ERROR("forward_list erase_after iterator outside range");
    }
    _Nodeptr _Pnodeb = _Where._Mynode();
    _Orphan_ptr(this->_Nextnode(_Pnodeb));
#else /* _ITERATOR_DEBUG_LEVEL == 2 */
    _Nodeptr _Pnodeb = _Where._Mynode();
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */
    if (++_Where == end()) {
      _DEBUG_ERROR("forward_list erase_after iterator outside range");
    } else {
      // node exists, erase it
      _Nodeptr _Pnode = _Where._Mynode();	// subject node
      ++_Where;	// point past subject node
      this->_Nextnode(_Pnodeb) =
        this->_Nextnode(_Pnode);	// link past it
      this->_Freenode(_Pnode);
    }
    return (_Make_iter(_Where));
  }

  iterator erase_after(const_iterator _First,
                       const_iterator _Last) {
    // erase (_First, _Last)
    if (_First == before_begin() && _Last == end()) {
      // erase all and return fresh iterator
      clear();
      return (end());
    } else {
      // erase subrange
      if (_First == end() || _First == _Last) {
        _DEBUG_ERROR("forward_list invalid erase_after range");
      } else {
        // range not awful, try it
        const_iterator _After = _First;
        ++_After;
        _DEBUG_RANGE(_After, _Last);
        while (_After != _Last) {
          _After = erase_after(_First);
        }
      }
      return (_Make_iter(_Last));
    }
  }

  void clear() _NOEXCEPT {
    // erase all
#if _ITERATOR_DEBUG_LEVEL == 2
    this->_Orphan_ptr(nullptr_t{});
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */

    _Nodeptr _Pnext;
    _Nodeptr _Pnode = this->_Myhead();
    this->_Myhead() = nullptr_t{};

    for (; _Pnode != nullptr_t{}; _Pnode = _Pnext) {
      // delete an element
      _Pnext = this->_Nextnode(_Pnode);
      this->_Freenode(_Pnode);
    }
  }

  void swap(_Myt & _Right)
  _NOEXCEPT_OP(_Alty::is_always_equal::value) {
    // exchange contents with _Right
    if (this != &_Right) {
      // (maybe) swap allocators, swap control information
      _Pocs(this->_Getal(), _Right._Getal());
      this->_Swap_all(_Right);
      _Swap_adl(this->_Myhead(), _Right._Myhead());
    }
  }

  void splice_after(const_iterator _Where, _Myt & _Right) {
    // splice all of _Right after _Where
    if (this != &_Right && !_Right.empty()) {
      // worth splicing, do it
      _Splice_after(_Where, _Right,
                    _Right.before_begin(), _Right.end());
    }
  }

  void splice_after(const_iterator _Where, _Myt && _Right) {
    // splice all of _Right at _Where
    splice_after(_Where, (_Myt &)_Right);
  }

  void splice_after(const_iterator _Where, _Myt & _Right,
                    const_iterator _First) {
    // splice _Right (_First, _First + 2) after _Where
    const_iterator _After = _First;
    if (_First == _Right.end() || ++_After == _Right.end()) {
      _DEBUG_ERROR("forward_list splice_after iterator outside range");
    } else {
      // element exists, try splice
      if (this != &_Right
          || (_Where != _First && _Where != _After)) {
        _Splice_after(_Where, _Right, _First, ++_After);
      }
    }
  }

  void splice_after(const_iterator _Where, _Myt && _Right,
                    const_iterator _First) {
    // splice _Right [_First, _First + 1) at _Where
    splice_after(_Where, (_Myt &)_Right, _First);
  }

  void splice_after(const_iterator _Where,
                    _Myt & _Right, const_iterator _First, const_iterator _Last) {
    // splice _Right [_First, _Last) at _Where
    const_iterator _After = _First;
    if (_First == _Right.end()) {
      _DEBUG_ERROR("forward_list splice_after iterator outside range");
    } else if (++_After != _Last && (this != &_Right || _Where != _First)) {
      _Splice_after(_Where, _Right, _First, _Last);
    }
  }

  void splice_after(const_iterator _Where,
                    _Myt && _Right, const_iterator _First, const_iterator _Last) {
    // splice _Right [_First, _Last) at _Where
    splice_after(_Where, (_Myt &)_Right, _First, _Last);
  }

  void remove(const _Ty & _Val) {
    // erase each element matching _Val
    iterator _Firstb = before_begin();
    iterator _Val_it = end();
    for (iterator _First = begin(); _First != end(); )
      if (*_First == _Val)
        if (_STD addressof(*_First) == _STD addressof(_Val)) {
          // store iterator to _Val and advance iterators
          _Val_it = _Firstb;
          ++_Firstb;
          ++_First;
        } else {
          _First = erase_after(_Firstb);
        }
      else {
        // advance iterators
        ++_Firstb;
        ++_First;
      }
    if (_Val_it != end()) {
      erase_after(_Val_it);
    }
  }

  template<class _Pr1>
  void remove_if(_Pr1 _Pred) {
    // erase each element satisfying _Pr1
    _Remove_if(_Pred);
  }

  template<class _Pr1>
  void _Remove_if(_Pr1 & _Pred) {
    // erase each element satisfying _Pr1
    iterator _Firstb = before_begin();
    for (iterator _First = begin(); _First != end(); )
      if (_Pred(*_First)) {
        _First = erase_after(_Firstb);
      } else {
        // advance iterators
        ++_Firstb;
        ++_First;
      }
  }

  void unique() {
    // erase each element matching previous
    unique(equal_to<>());
  }

  template<class _Pr2>
  void unique(_Pr2 _Pred) {
    // erase each element satisfying _Pred with previous
    iterator _First = begin();
    if (_First != end()) {
      // worth doing
      iterator _After = _First;
      for (++_After; _After != end(); )
        if (_Pred(*_First, *_After)) {
          _After = erase_after(_First);
        } else {
          _First = _After++;
        }
    }
  }

  void merge(_Myt & _Right) {
    // merge in elements from _Right, both ordered by operator<
    _Merge1(_Right, less<>());
  }

  void merge(_Myt && _Right) {
    // merge in elements from _Right, both ordered by operator<
    _Merge1(_Right, less<>());
  }

  template<class _Pr2>
  void merge(_Myt & _Right, _Pr2 _Pred) {
    // merge in elements from _Right, both ordered by _Pred
    _Merge1(_Right, _Pred);
  }

  template<class _Pr2>
  void merge(_Myt && _Right, _Pr2 _Pred) {
    // merge in elements from _Right, both ordered by _Pred
    _Merge1(_Right, _Pred);
  }

  template<class _Pr2>
  void _Merge1(_Myt & _Right, _Pr2 && _Pred) {
    // merge in elements from _Right, both ordered by _Pred
    if (&_Right != this) {
      // safe to merge, do it
      iterator _First1 = before_begin();
      iterator _After1 = begin();
      iterator _Last1 = end();
      iterator _First2 = _Right.before_begin();
      iterator _After2 = _Right.begin();
      iterator _Last2 = _Right.end();
      _DEBUG_ORDER_PRED(_After1, _Last1, _Pred);
      _DEBUG_ORDER_PRED(_After2, _Last2, _Pred);
      for (; _After1 != _Last1 && _After2 != _Last2; ++_First1)
        if (_DEBUG_LT_PRED(_Pred, *_After2, *_After1)) {
          // splice in an element from _Right
          iterator _Mid2 = _After2;
          _Splice_after(_First1, _Right, _First2, ++_Mid2);
          _After2 = _Mid2;
        } else {
          ++_After1;
        }
      if (_After2 != _Last2)
        _Splice_after(_First1, _Right, _First2,
                      _Last2);	// splice remainder of _Right
    }
  }

  void sort() {
    // order sequence, using operator<
    sort(less<>());
  }

  template<class _Pr2>
  void sort(_Pr2 _Pred) {
    // order sequence, using _Pred
    _Sort(before_begin(), end(), _Pred, _STD distance(begin(), end()));
  }

  template<class _Pr2>
  void _Sort(iterator _Before_first, iterator _Last, _Pr2 & _Pred,
             size_type _Size) {
    // order (_Before_first, _Last), using _Pred
    // _Size must be number of elements in range
    if (_Size < 2) {
      return;  // nothing to do
    }
    iterator _Mid = _STD next(_Before_first, 1 + _Size / 2);
    _Sort(_Before_first, _Mid, _Pred, _Size / 2);
    iterator _First = _STD next(_Before_first);
    iterator _Before_mid = _STD next(_Before_first, _Size / 2);
    _Sort(_Before_mid, _Last, _Pred, _Size - _Size / 2);
    _Mid = _STD next(_Before_mid);
    for (; ; ) {
      // [_First, _Mid) and [_Mid, _Last) are sorted and non-empty
      if (_DEBUG_LT_PRED(_Pred, *_Mid, *_First)) {
        // consume _Mid
        splice_after(_Before_first, *this, _Before_mid);
        ++_Before_first;
        _Mid = _STD next(_Before_mid);
        if (_Mid == _Last) {
          return;  // exhausted [_Mid, _Last); done
        }
      } else {
        // consume _First
        ++_Before_first;
        ++_First;
        if (_First == _Mid) {
          return;  // exhausted [_First, _Mid); done
        }
      }
    }
  }

  void reverse() _NOEXCEPT {
    // reverse sequence
    if (!empty()) {
      // worth doing, move to back in reverse order
      const_iterator _First = _Before_end();
      for (; begin() != _First; ) {
        _Splice_same_after(_First, *this, before_begin(), ++begin());
      }
    }
  }

private:
  size_type _Size() const {
    // get size by counting
    size_type _Ans = 0;
    for (const_iterator _Next = begin(); _Next != end(); ++_Next) {
      ++_Ans;
    }
    return (_Ans);
  }

  const_iterator _Before_end() const {
    // get iterator just before end
    const_iterator _Next = before_begin();
    for (const_iterator _Nextp = _Next; ++_Nextp != end(); ) {
      _Next = _Nextp;
    }
    return (_Next);
  }

  void _Splice_after(const_iterator _Where,
                     _Myt & _Right, const_iterator _First, const_iterator _Last) {
    // splice _Right (_First, _Last) just after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
    if (_Where._Getcont() != &this->_Get_data() || _Where == end()) {
      _DEBUG_ERROR("forward_list splice_after iterator outside range");
    }
    if (this->_Getal() != _Right._Getal()) {
      _DEBUG_ERROR("forward_list containers incompatible for splice_after");
    }
    if (this != &_Right) {
      // transfer ownership of (_First, _Last)
      const_iterator _Next = _First;
      for (++_Next; _Next != _Last; ) {
        // transfer ownership
        const_iterator _Iter = _Next++;
        _Right._Orphan_ptr(_Iter._Ptr);
        _Iter._Adopt(&this->_Get_data());
      }
    }
#else /* _ITERATOR_DEBUG_LEVEL == 2 */
    if (this->_Getal() != _Right._Getal()) {
      _XSTD terminate();
    }
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */
    _Splice_same_after(_Where, _Right, _First, _Last);
  }

  void _Splice_same_after(const_iterator _Where,
                          _Myt & _Right, const_iterator _First, const_iterator _Last) {
    // splice _Right (_First, _Last) just after _Where
    const_iterator _Next = _First;
    const_iterator _After = _Next;
    for (++_After; _After != _Last; ++_Next, (void)++_After)
      if (_After == _Right.end()) {
        // find last element, and check for bad range
        _DEBUG_ERROR("forward_list splice_after invalid range");
        return;
      }
    this->_Nextnode(_Next._Mynode()) =
      this->_Nextnode(_Where._Mynode());	// link last to new home
    this->_Nextnode(_Where._Mynode()) =
      this->_Nextnode(_First._Mynode());	// link first to new home
    this->_Nextnode(_First._Mynode()) =
      _Last._Mynode();	// drop range from old home
  }

  void _Tidy() {
    // free all storage
    clear();
  }

  iterator _Insert_n_after(const_iterator _Where,
                           size_type _Count, const _Ty & _Val) {
    // insert _Count * _Val after _Where
    size_type _Countsave = _Count;
    _TRY_BEGIN
    for (; 0 < _Count; --_Count, (void)++_Where) {
      _Insert_after(_Where, _Val);
    }
    _CATCH_ALL
    for (; _Count < _Countsave; ++_Count) {
      erase_after(_Where);
    }
    _RERAISE;
    _CATCH_END
    return (_Make_iter(_Where));
  }

#if _ITERATOR_DEBUG_LEVEL == 2
  void _Orphan_ptr(_Nodeptr _Ptr) {
    // orphan iterators with specified node pointers
    _Lockit _Lock(_LOCK_DEBUG);
    const_iterator** _Pnext = (const_iterator**)this->_Getpfirst();
    if (_Pnext != 0)
      while (*_Pnext != 0)
        if ((*_Pnext)->_Ptr == this->_Before_head()
            || _Ptr != nullptr_t {} && (*_Pnext)->_Ptr != _Ptr)
          _Pnext = (const_iterator**)(*_Pnext)->_Getpnext();
        else {
          // orphan the iterator
          (*_Pnext)->_Clrcont();
          *_Pnext = *(const_iterator**)(*_Pnext)->_Getpnext();
        }
  }
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */
};

// forward_list TEMPLATE OPERATORS

template < class _Ty,
         class _Alloc > inline
void swap(forward_list<_Ty, _Alloc> & _Left,
          forward_list<_Ty, _Alloc> & _Right)
_NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
{
  // swap _Left and _Right lists
  _Left.swap(_Right);
}

template < class _Ty,
         class _Alloc > inline
bool operator==(const forward_list<_Ty, _Alloc> & _Left,
                const forward_list<_Ty, _Alloc> & _Right)
{
  // test for list equality
  return (_STD equal(_Left.begin(), _Left.end(),
                     _Right.begin(), _Right.end()));
}

template < class _Ty,
         class _Alloc > inline
bool operator!=(const forward_list<_Ty, _Alloc> & _Left,
                const forward_list<_Ty, _Alloc> & _Right)
{
  // test for list inequality
  return (!(_Left == _Right));
}

template < class _Ty,
         class _Alloc > inline
bool operator<(const forward_list<_Ty, _Alloc> & _Left,
               const forward_list<_Ty, _Alloc> & _Right)
{
  // test if _Left < _Right for lists
  return (_STD lexicographical_compare(_Left.begin(), _Left.end(),
                                       _Right.begin(), _Right.end()));
}

template < class _Ty,
         class _Alloc > inline
bool operator>(const forward_list<_Ty, _Alloc> & _Left,
               const forward_list<_Ty, _Alloc> & _Right)
{
  // test if _Left > _Right for lists
  return (_Right < _Left);
}

template < class _Ty,
         class _Alloc > inline
bool operator<=(const forward_list<_Ty, _Alloc> & _Left,
                const forward_list<_Ty, _Alloc> & _Right)
{
  // test if _Left <= _Right for lists
  return (!(_Right < _Left));
}

template < class _Ty,
         class _Alloc > inline
bool operator>=(const forward_list<_Ty, _Alloc> & _Left,
                const forward_list<_Ty, _Alloc> & _Right)
{
  // test if _Left >= _Right for lists
  return (!(_Left < _Right));
}
_STD_END
#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _FORWARD_LIST_ */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
