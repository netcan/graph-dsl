//
// Created by Darwin Yuan on 2020/8/14.
//

#ifndef GRAPH_SUB_GRAPH_H
#define GRAPH_SUB_GRAPH_H

#include <graph/graph_ns.h>
#include <graph/core/sub_graph_analyzer.h>
#include <graph/core/dsl/graph_node.h>
#include <holo/types/type_transform.h>
#include <holo/algo/head.h>
#include <spdlog/spdlog.h>
#include <maco/map.h>

GRAPH_DSL_NS_BEGIN

template<typename ... NODES>
struct sub_graph final {
   constexpr static auto all_sorted_nodes = sub_graph_analyzer<NODES...>::all_sorted_sub_graph_nodes;

   template<typename ROOTS_CB>
   struct instance_type {
   private:
      template<typename ... Ts>
      using cb_container = std::tuple<subgraph_node_cb<typename Ts::node_type, Ts::category>...>;
      static_assert(holo::length(all_sorted_nodes) > 0, "");
      using nodes_cb = decltype(holo::map_to<cb_container>(all_sorted_nodes));

   private:
      constexpr static auto sorted_nodes_desc = sub_graph_analyzer<NODES...>::sorted_nodes_desc;
      static_assert(holo::length(sorted_nodes_desc) == sizeof...(NODES));

      template<typename ... Ts>
      using desc_container  = std::tuple<typename Ts::template instance_type<ROOTS_CB, nodes_cb>...>;
      using nodes_links = decltype(holo::map_to<desc_container>(sorted_nodes_desc));

      template<typename T> struct desc_node_type { using type = typename T::node_type; };
   public:
      auto build(graph_context& context) -> status_t {
         context.switch_subgraph_context(nodes_cb_);
         return tuple_foreach(nodes_links_, [&](auto& link) {
            return link.build(context);
         });
      }

      template<typename ROOT>
      auto connect_root(graph_context& context, ROOT& root, actor_ports& ports) -> status_t {
         context.switch_subgraph_context(nodes_cb_);
         constexpr auto Index = tuple_element_index_v<typename ROOT::node_type, nodes_links, desc_node_type>;
         if constexpr (Index >= 0) {
            return std::get<Index>(nodes_links_).collect_actor_ports(context, ports);
         } else {
            return status_t::Ok;
         }
      }

      auto start(graph_context& context) -> status_t {
         context.switch_subgraph_context(nodes_cb_);
         return tuple_foreach_r(nodes_cb_, [&](auto& cb) {
            return cb.start(context, nodes_links_);
         });
      }

      auto cleanup() {
         return tuple_foreach_void(nodes_cb_, [](auto& cb) {
            cb.cleanup();
         });
      }

      auto stop() {
         return tuple_foreach_void(nodes_cb_, [](auto& cb) {
            cb.stop();
         });
      }

   private:
      nodes_cb    nodes_cb_;
      nodes_links nodes_links_;
   };
};

template<typename, typename ... NODES>
using sub_graph_t = sub_graph<NODES...>;

GRAPH_DSL_NS_END


#define __sUb_gRaPh_each_node(x) , __sUb_gRaPh_node x
#define __sUb_gRaPh_nodes(...) __MACO_map(__sUb_gRaPh_each_node, __VA_ARGS__)
#define __g_SUB_GRAPH(...) GRAPH_DSL_NS::sub_graph_t<void __sUb_gRaPh_nodes(__VA_ARGS__)>

#endif //GRAPH_SUB_GRAPH_H
