#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"
#include <cstddef>
#include <iterator>

namespace sjtu {

template <typename T>
class vector {
private:
  T *data_ = nullptr; size_t size_ = 0, cap_ = 0;
  static T* allocate(size_t n){ return n? static_cast<T*>(::operator new(sizeof(T)*n)):nullptr; }
  static void deallocate(T* p){ if(p) ::operator delete(p); }
  void destroy_range(size_t l,size_t r){ for(size_t i=l;i<r;++i) (data_+i)->~T(); }
  void reallocate(size_t ncap){ T* nd=allocate(ncap); size_t i=0; try{ for(;i<size_;++i) new (nd+i) T(data_[i]); } catch(...) { for(size_t j=0;j<i;++j) (nd+j)->~T(); deallocate(nd); throw; } destroy_range(0,size_); deallocate(data_); data_=nd; cap_=ncap; }
public:
  class const_iterator;
  class iterator {
  public:
    using difference_type=std::ptrdiff_t; using value_type=T; using pointer=T*; using reference=T&; using iterator_category=std::output_iterator_tag;
  private:
    vector<T>* vec=nullptr; size_t pos=0; iterator(vector<T>*v,size_t p):vec(v),pos(p){}
    friend class vector<T>; friend class const_iterator;
  public:
    iterator()=default;
    iterator operator+(const int &n) const { iterator t=*this; t.pos+=n; return t; }
    iterator operator-(const int &n) const { iterator t=*this; t.pos-=n; return t; }
    int operator-(const iterator &rhs) const { if(vec!=rhs.vec) throw invalid_iterator(); return (int)(pos - rhs.pos); }
    iterator& operator+=(const int &n){ pos+=n; return *this; }
    iterator& operator-=(const int &n){ pos-=n; return *this; }
    iterator operator++(int){ iterator t=*this; ++(*this); return t; }
    iterator& operator++(){ ++pos; return *this; }
    iterator operator--(int){ iterator t=*this; --(*this); return t; }
    iterator& operator--(){ --pos; return *this; }
    T& operator*() const { return vec->data_[pos]; }
    bool operator==(const iterator &rhs) const { return vec==rhs.vec && pos==rhs.pos; }
    bool operator==(const const_iterator &rhs) const { return (const void*)vec==(const void*)rhs.vec && pos==rhs.pos; }
    bool operator!=(const iterator &rhs) const { return !(*this==rhs); }
    bool operator!=(const const_iterator &rhs) const { return !(*this==rhs); }
  };
  class const_iterator {
  public:
    using difference_type=std::ptrdiff_t; using value_type=T; using pointer=const T*; using reference=const T&; using iterator_category=std::output_iterator_tag;
  private:
    const vector<T>* vec=nullptr; size_t pos=0; const_iterator(const vector<T>*v,size_t p):vec(v),pos(p){}
    friend class vector<T>; friend class iterator;
  public:
    const_iterator()=default;
    const_iterator operator+(const int &n) const { const_iterator t=*this; t.pos+=n; return t; }
    const_iterator operator-(const int &n) const { const_iterator t=*this; t.pos-=n; return t; }
    int operator-(const const_iterator &rhs) const { if(vec!=rhs.vec) throw invalid_iterator(); return (int)(pos - rhs.pos); }
    const_iterator& operator+=(const int &n){ pos+=n; return *this; }
    const_iterator& operator-=(const int &n){ pos-=n; return *this; }
    const_iterator operator++(int){ const_iterator t=*this; ++(*this); return t; }
    const_iterator& operator++(){ ++pos; return *this; }
    const_iterator operator--(int){ const_iterator t=*this; --(*this); return t; }
    const_iterator& operator--(){ --pos; return *this; }
    const T& operator*() const { return vec->data_[pos]; }
    bool operator==(const const_iterator &rhs) const { return vec==rhs.vec && pos==rhs.pos; }
    bool operator==(const iterator &rhs) const { return (const void*)vec==(const void*)rhs.vec && pos==rhs.pos; }
    bool operator!=(const const_iterator &rhs) const { return !(*this==rhs); }
    bool operator!=(const iterator &rhs) const { return !(*this==rhs); }
  };



  vector()=default;
  vector(const vector &other){ if(other.size_){ data_=allocate(other.size_); cap_=size_=other.size_; size_t i=0; try{ for(;i<size_;++i) new (data_+i) T(other.data_[i]); }catch(...){ for(size_t j=0;j<i;++j) (data_+j)->~T(); deallocate(data_); data_=nullptr; size_=cap_=0; throw; } } }
  ~vector(){ clear(); deallocate(data_); data_=nullptr; cap_=0; }
  vector &operator=(const vector &other){ if(this==&other) return *this; if(other.size_>cap_){ T* nd=allocate(other.size_); size_t i=0; try{ for(;i<other.size_;++i) new (nd+i) T(other.data_[i]); }catch(...){ for(size_t j=0;j<i;++j) (nd+j)->~T(); ::operator delete(nd); throw; } destroy_range(0,size_); deallocate(data_); data_=nd; cap_=other.size_; size_=other.size_; }
    else { size_t i=0; for(; i<size_ && i<other.size_; ++i) { (data_+i)->~T(); new (data_+i) T(other.data_[i]); } for(; i<other.size_; ++i) new (data_+i) T(other.data_[i]); for(size_t j=other.size_; j<size_; ++j) (data_+j)->~T(); size_=other.size_; }
    return *this; }

  T & at(const size_t &pos){ if(pos>=size_) throw index_out_of_bound(); return data_[pos]; }
  const T & at(const size_t &pos) const { if(pos>=size_) throw index_out_of_bound(); return data_[pos]; }
  T & operator[](const size_t &pos){ if(pos>=size_) throw index_out_of_bound(); return data_[pos]; }
  const T & operator[](const size_t &pos) const { if(pos>=size_) throw index_out_of_bound(); return data_[pos]; }
  const T & front() const { if(size_==0) throw container_is_empty(); return data_[0]; }
  const T & back() const { if(size_==0) throw container_is_empty(); return data_[size_-1]; }
  iterator begin(){ return iterator(this,0); }
  const_iterator begin() const { return const_iterator(this,0); }
  const_iterator cbegin() const { return const_iterator(this,0); }
  iterator end(){ return iterator(this,size_); }
  const_iterator end() const { return const_iterator(this,size_); }
  const_iterator cend() const { return const_iterator(this,size_); }
  bool empty() const { return size_==0; }
  size_t size() const { return size_; }
  void clear(){ destroy_range(0,size_); size_=0; }

  void ensure_cap(size_t need){ if(need<=cap_) return; size_t ncap=cap_?cap_*2:1; if(ncap<need) ncap=need; reallocate(ncap); }

  iterator insert(iterator pos, const T &value){ if(pos.vec!=this) throw invalid_iterator(); return insert(pos.pos,value); }
  iterator insert(const size_t &ind, const T &value){ if(ind>size_) throw index_out_of_bound(); ensure_cap(size_+1); for(size_t i=size_; i>ind; --i){ new (data_+i) T(data_[i-1]); (data_+i-1)->~T(); } new (data_+ind) T(value); ++size_; return iterator(this,ind); }
  iterator erase(iterator pos){ if(pos.vec!=this || pos.pos>=size_) throw index_out_of_bound(); return erase(pos.pos); }
  iterator erase(const size_t &ind){ if(ind>=size_) throw index_out_of_bound(); (data_+ind)->~T(); for(size_t i=ind; i+1<size_; ++i){ new (data_+i) T(data_[i+1]); (data_+i+1)->~T(); } --size_; return iterator(this,ind); }
  void push_back(const T &value){ ensure_cap(size_+1); new (data_+size_) T(value); ++size_; }
  void pop_back(){ if(size_==0) throw container_is_empty(); --size_; (data_+size_)->~T(); }
};

} // namespace sjtu

#endif
