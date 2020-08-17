//
// Created by Darwin Yuan on 2020/8/14.
//

#ifndef GRAPH_GRAPH_NODE_H
#define GRAPH_GRAPH_NODE_H

#include <graph/graph_ns.h>
#include <graph/status.h>
#include <graph/util/assertion.h>
#include <graph/core/graph_context.h>
#include <graph/core/dsl/graph_port.h>
#include <tuple>
#include <boost/hana.hpp>
#include <graph/function/unique.h>
#include <graph/core/cb/subgraph_node_cb.h>

GRAPH_DSL_NS_BEGIN

namespace hana = boost::hana;

template<bool ROOT, typename NODE, typename ... LINKS>
struct graph_node final {
   constexpr static auto is_root = hana::bool_c<ROOT>;
   using node_type = NODE;
   constexpr static auto direct_decedents =
      unique(hana::flatten(hana::make_tuple(graph_port<LINKS>::node_list...)));
   constexpr static auto sequence = std::make_index_sequence<sizeof...(LINKS)>{};

   template<typename TUPLE>
   struct instance_type {
      constexpr static auto is_root = ROOT;
      using node_type = NODE;
      auto build(graph_context& context) -> status_t {
         if constexpr (!is_root) {
            // skip all nodes whose ref count is 0
            if(!node_index<NODE, TUPLE>::get_node(context).enabled()) {
               return status_t::Ok;
            }
         }

         return build_links(context, sequence);
      }

      auto collect_actor_ports(graph_context& context, actor_ports& ports) -> status_t {
         return collect_actor_ports(context, ports, sequence);
      }

      auto collect_actor_ports(graph_context& context, root_actor_ports& ports) -> status_t {
         if constexpr (is_root) {
            return collect_actor_ports(context, ports, sequence);
         } else {
            return status_t::Failed;
         }
      }

   private:
      template<size_t ... I>
      auto build_links(graph_context& context, std::index_sequence<I...>) -> status_t {
         status_t status = status_t::Ok;
         return (((status = std::get<I>(links_).build(context)) == status_t::Ok) && ...) ?
            status_t::Ok : status;
      }

      template<size_t ... I>
      auto collect_actor_ports(graph_context& context, actor_ports& ports, std::index_sequence<I...>) -> status_t {
         status_t status = status_t::Ok;
         return (((status = std::get<I>(links_).collect_actor_port(context, ports)) == status_t::Ok) && ...) ?
                status_t::Ok : status;
      }

      template<size_t ... I>
      auto collect_actor_ports(graph_context& context, root_actor_ports& ports, std::index_sequence<I...>) -> status_t {
         status_t status = status_t::Ok;
         return (((status = std::get<I>(links_).collect_actor_port(context, ports)) == status_t::Ok) && ...) ?
                status_t::Ok : status;
      }

   private:
      std::tuple<typename graph_port<LINKS>::template instance_type<TUPLE> ...> links_;
   };
};

GRAPH_DSL_NS_END

#define __g_NODE(...) GRAPH_DSL_NS::graph_node<false, __VA_ARGS__>
#define __g_ROOT(...) GRAPH_DSL_NS::graph_node<true,  __VA_ARGS__>

#endif //GRAPH_SUBGRAPH_NODE_CB_H
