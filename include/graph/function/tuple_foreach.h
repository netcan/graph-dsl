//
// Created by Darwin Yuan on 2020/8/18.
//

#ifndef GRAPH_TUPLE_FOREACH_H
#define GRAPH_TUPLE_FOREACH_H

#include <graph/graph_ns.h>
#include <tuple>
#include <graph/status.h>

GRAPH_DSL_NS_BEGIN

namespace detail {
   template<typename TUPLE, typename F, size_t ... I>
   auto tuple_foreach(TUPLE&& tuple, F&& f, std::index_sequence<I...>) {
      status_t status = status_t::Ok;
      return (((status = f(std::get<I>(std::forward<TUPLE>(tuple)))) == status_t::Ok) && ...) ?
             status_t::Ok : status;
   }

   template<typename TUPLE, typename F, size_t ... I>
   auto tuple_foreach_void(TUPLE&& tuple, F&& f, std::index_sequence<I...>) {
      (f(std::get<I>(std::forward<TUPLE>(tuple))), ...);
   }
}

template<typename TUPLE, typename F, size_t ... I>
auto tuple_foreach(TUPLE&& tuple, F&& f) {
   return detail::tuple_foreach
            ( std::forward<TUPLE>(tuple)
            , std::forward<F>(f)
            , std::make_index_sequence<std::tuple_size_v<std::decay_t<TUPLE>>>{});
}

template<typename TUPLE, typename F, size_t ... I>
auto tuple_foreach_void(TUPLE&& tuple, F&& f) {
   detail::tuple_foreach_void
      ( std::forward<TUPLE>(tuple)
         , std::forward<F>(f)
         , std::make_index_sequence<std::tuple_size_v<std::decay_t<TUPLE>>>{});
}


GRAPH_DSL_NS_END

#endif //GRAPH_TUPLE_FOREACH_H
