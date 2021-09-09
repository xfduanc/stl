#include <iostream>
#include <string>
#include <cstddef>

namespace my
{
template<typename ValueT>
class vector
{
public:
    using value_type        = ValueT;
    using reference         = ValueT&;
    using const_reference   = const ValueT;

    using iterator          = ValueT*;
    using const_iterator    =const ValueT*;
    using size_type         =::size_t;
    using difference_type   =::ptrdiff_t;
private:
    ValueT * m_data;
    ::size_t m_size;
    ::size_t m_capacity;
public:
    constexpr vector():m_data(),m_size(),m_capacity(){}
    ~vector()
    {
        //先析构，再释放内存
        for(::size_t k =0; k <m_size; ++k)
            m_data[k].~ValueT();
        if( this->m_data)
            ::operator delete(this->m_data);
    }
    vector(const vector &rhs)
    {
        //only allocate memory not call this's constructor,the memory size is rhs's capacity
        this->m_data=static_cast<ValueT*>(::operator new(rhs.m_capacity*sizeof(ValueT) ));
        this->m_size = 0;
        this->m_capacity = rhs.m_capacity;
        try{
            for(::size_t k=0; k<rhs.m_size; ++k){
                ::new( &this->m_data[k])ValueT(rhs.m_data[k]);
                this->m_size+=1;}
        }
        catch(...)
        {
            //first call destructor then call ::operator delete
            for(int k=0; k <this->m_size; ++k)
                this->m_data[k].~ValueT();
            ::operator delete(this->m_data);
        }
    }
    vector(vector &&rhs)
    {
        this->m_capacity = rhs.m_capacity;
        this->m_size     = rhs.m_size;
        this->m_data     = rhs.m_data;

        rhs.m_data = nullptr;
        rhs.m_size = 0;
        rhs.m_capacity = 0;
    }
    vector & operator=(const vector & rhs);
    vector & operator =(vector && rhs)noexcept;

public:
    //iterator
    iterator begin()noexcept
    {
        return this->m_data;
    }
    const_iterator begin() const noexcept
    {
        return this->m_data;
    }

    iterator end() noexcept
    {
        return this->m_data +this->m_size;
    }

    const_iterator end()const noexcept
    {
        return this->m_data +this->m_size;
    }
    //accessor
    ValueT * data()noexcept
    {
        return this->m_data;
    }
    const ValueT * data()const noexcept
    {
        return this->m_data;
    }

    size_type size()const noexcept
    {
        return this->m_size;
    }
    size_type capacity() const noexcept
    {
        return this->m_capacity;
    }
    bool empty()
    {return this->m_size == 0;}
    
    void clear()const noexcept
    {
        //clear only call the destructor not free the allocated memory
        for(int k =0; k < this->m_size ; ++k)
            this->m_data[k].~ValueT();
        this->m_size = 0;
    }

    void pop_back()
    {
        assert(!this->empty());

        size_type k = this->m_size -1;
        this->m_data[k].~ValueT();
        this->m_size-=1;
    }
    void push_back(const ValueT &value)
    {
        this->emplace_back(value);
    }
    void push_back(ValueT && value)
    {
        this->emplace_back( ::std::move(value) );
    }

    template<typename... ArgsT>
    reference
    emplace_back(ArgsT&&... args)
    {
        if( this->m_size < this->m_capacity)
        {   //placement  new
            size_type k = this->m_size;
            ::new(&this->m_data[k])ValueT(::std::forward<ArgsT>(args)...);
            this->m_size +=1;
            return this->m_data[k];
        }
        //need to realloc and insert
        size_type new_capacity = this->m_size+1;
        new_capacity |=this->m_size/2;

        auto new_data = static_cast<ValueT*>(::operator new( new_capacity*sizeof(ValueT)) );
        size_type new_size = 0;

        try
        {
            for(size_type k = 0; k < this->m_size; ++k)
            {
                ::new(&new_data[k])ValueT(::std::move(this->m_data[k]) );
                new_size +=1;
            }
            ::new(&new_data[new_size])ValueT( ::std::forward<ArgsT>(args)...);
            new_size +=1;
        }
        catch(const std::exception& e)
        {
            for( size_type k =0; k <new_size; ++k)
                new_data[k].~ValueT();
            ::operator delete(new_data);
            throw;
        }

        //free the past
        for(size_type k = 0; k < this->m_size; ++k)
            this->m_data[k].~ValueT();
        if(this->m_data)
            ::operator delete(this->m_data);
        this->m_data = new_data;
        this->m_size = new_size;
        this->m_capacity = new_capacity;
        return this->m_data[this->m_size-1];
    }

};


}//namespace my

using string =::std::string;
class MyClass : public string{
public:
  int data[100];
  MyClass() {std::cout << "default constructed [" << this << "]\n";}
  MyClass(const char *ptr) {
      this->assign(ptr);
      std::cout << "constructed [" << this << "]\n";
      }
  ~MyClass()
  {std::cout << "destructed [" << this << "]\n";}
};
int main()
{
    using my::vector;
    vector<MyClass> vec_myclass;
    vec_myclass.emplace_back("sdsds");
    vector<int> vec;
    int a = 2;
    vec.push_back(1);
    vec.emplace_back(12);
    vec.emplace_back(::std::move(a));

    for(const auto & v :vec)
        ::std::cout<<v<<::std::endl;

    ::std::string str;
    // str.emplace();
    vector<::std::string> ve;
    ve.emplace_back("hello");
    ve.emplace_back("hello", 2);
    for(const auto & v :ve)
        ::std::cout<<v<<::std::endl;
    return 0;
}


