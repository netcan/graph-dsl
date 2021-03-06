//
// Created by Darwin Yuan on 2020/8/16.
//

#ifndef GRAPH_ACTOR_PORTS_H
#define GRAPH_ACTOR_PORTS_H

#include <graph/graph_ns.h>
#include <nano-caf/core/actor/actor_handle.h>
#include <tuple>
#include <vector>
#include <map>

GRAPH_DSL_NS_BEGIN

using port_id_t = uint8_t;

enum class image_type : uint8_t {
   RGB,
   YUV
};

struct port_format {
   uint32_t   width_;
   uint32_t   height_;
   image_type image_type_;
};

using actor_handle_set = std::vector<nano_caf::actor_handle>;

struct actor_port {
   port_format format_;
   actor_handle_set actor_handles_;

   template<typename MSG, typename ... Args>
   auto send(Args&& ... args) {
      for(auto& handle : actor_handles_) {
         handle.send<MSG>(std::forward<Args>(args)...);
      }
   }
};

using actor_ports = std::map<port_id_t, actor_port>;

GRAPH_DSL_NS_END

#endif //GRAPH_ACTOR_PORTS_H
