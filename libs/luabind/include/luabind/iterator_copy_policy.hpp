// Copyright Daniel Wallin 2007. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LUABIND_ITERATOR_COPY_POLICY__071111_HPP
# define LUABIND_ITERATOR_COPY_POLICY__071111_HPP

# include <luabind/config.hpp>
# include <luabind/detail/policy.hpp>
# include <luabind/detail/convert_to_lua.hpp>

namespace luabind { namespace detail {

template <class Iterator, class Container>
struct copy_iterator
{
    static int next(lua_State* L)
    {
        copy_iterator* self = static_cast<copy_iterator*>(
            lua_touserdata(L, lua_upvalueindex(1)));

        if (self->first != self->last)
        {
            convert_to_lua(L, *self->first);
            ++self->first;
        }
        else
        {
            lua_pushnil(L);
        }

        return 1;
    }

    static int destroy(lua_State* L)
    {
        copy_iterator* self = static_cast<copy_iterator*>(lua_touserdata(L, 1));
        self->~copy_iterator();
        return 0;
    }

    copy_iterator(Iterator first, Iterator last, Container& container)
      : first(first)
      , last(last)
	  , container(container)
    {}

    Iterator first;
    Iterator last;
	Container container;
};

template <class Iterator, class Container>
int make_copy_range(lua_State* L, Iterator first, Iterator last, Container& container)
{
    void* storage = lua_newuserdata(L, sizeof(copy_iterator<Iterator, Container>));
    lua_newtable(L);
    lua_pushcclosure(L, copy_iterator<Iterator, Container>::destroy, 0);
    lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
    lua_pushcclosure(L, copy_iterator<Iterator, Container>::next, 1);
    new (storage) copy_iterator<Iterator, Container>(first, last, container);
    return 1;
}

template <class Container>
int make_copy_range(lua_State* L, Container& container)
{
    return make_copy_range(L, container.begin(), container.end(), container);
}

struct copy_iterator_converter
{
    typedef copy_iterator_converter type;

    template <class Container>
    void apply(lua_State* L, Container& container)
    {
        make_copy_range(L, container);
    }

    template <class Container>
    void apply(lua_State* L, Container const& container)
    {
        make_copy_range(L, container);
    }
};

struct copy_iterator_policy : conversion_policy<0>
{
    static void precall(lua_State*, index_map const&)
    {}

    static void postcall(lua_State*, index_map const&)
    {}

    template <class T, class Direction>
    struct apply
    {
        typedef copy_iterator_converter type;
    };
};

}} // namespace luabind::detail

namespace luabind { namespace {

LUABIND_ANONYMOUS_FIX detail::policy_cons<
    detail::copy_iterator_policy, detail::null_type> copy_container_and_return_stl_iterator;

}} // namespace luabind::unnamed

#endif // LUABIND_ITERATOR_COPY_POLICY__071111_HPP

