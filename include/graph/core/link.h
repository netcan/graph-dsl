//
// Created by Darwin Yuan on 2020/8/14.
//

#ifndef GRAPH_LINK_H
#define GRAPH_LINK_H

#include <graph/graph_ns.h>
#include <graph/status.h>
#include <graph/core/graph_context.h>
#include <graph/core/node_like_trait.h>
#include <tuple>
#include <vector>

GRAPH_DSL_NS_BEGIN

struct link {

};

template<typename T>
struct link_desc;

template<typename PORT, typename NODE_LIKE>
struct link_desc<auto (PORT) -> NODE_LIKE> : private link {
   constexpr static auto node_list = node_like_trait<NODE_LIKE>::node_list;

//   auto build(graph_context& context, std::vector<link*>& links) -> status_t {
//      GRAPH_EXPECT_SUCC(node_like_.build(context));
//
//      if(node_like_.present()) {
//         links.push_back(this);
//      }
//
//      return status_t::Ok;
//   }
//
//private:
//   PORT port_;
//   NODE_LIKE node_like_;
};

GRAPH_DSL_NS_END

#endif //GRAPH_LINK_H
