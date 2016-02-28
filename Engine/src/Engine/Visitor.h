#pragma once

#include <algorithm>
#include <functional>
#include <vector>

template<typename T, typename... TArgs>
using VisitorFunction = std::function<void(T,TArgs...)>;

// Generic way to visit every element of a collection
template<typename T, typename... TArgs>
class Visitor
{
public:
    Visitor( std::vector<T>& a_collection ) :
        m_collection(a_collection)
    {}

    void visit( VisitorFunction<T&,TArgs...> func, TArgs... args ) {
        for( auto& element: m_collection )
        {
            func( element, args... );
        }
    }

    void visit( VisitorFunction<const T&,TArgs...> func, TArgs... args ) const
    {
        for( const auto& element: m_collection )
        {
            func( element, args... );
        }

    }

private:
    std::vector<T>& m_collection;

};


template<typename T, typename... TArgs>
using SharedPtrVisitor = Visitor<std::shared_ptr<T>,TArgs...>;
