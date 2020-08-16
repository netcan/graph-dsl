//
// Created by Darwin Yuan on 2020/8/16.
//

#include <graph/core/node_desc.h>
#include <graph/core/subgraph_desc.h>
#include <nano-caf/core/actor/behavior_based_actor.h>
#include <iostream>
#include <map>
#include <unordered_map>

struct image_buf {
   char buf[1024];
};

CAF_def_message(image_buf_msg, (buf, std::shared_ptr<const image_buf>));

struct node_5_actor : nano_caf::behavior_based_actor {
   node_5_actor(std::unique_ptr<GRAPH_DSL_NS::actor_ports> ports) : ports_(std::move(ports)) {
      std::cout << "intermediate created" << std::endl;
   }
   nano_caf::behavior get_behavior() {
      return {
         [&](const image_buf_msg& msg) {
            std::cout << "1 image buf received" << std::endl;
            forward(msg);
         },

         [](nano_caf::exit_msg_atom, nano_caf::exit_reason) {}
      };
   }

   void forward(const image_buf_msg& msg) {
      for(auto& port : *ports_) {
         for(auto& handle : port.actor_handles_) {
            nano_caf::actor_handle(handle).send<image_buf_msg>(msg);
         }
      }
   }

private:
   std::unique_ptr<GRAPH_DSL_NS::actor_ports> ports_;
};

struct node_8_actor : nano_caf::behavior_based_actor {
   node_8_actor() {
      std::cout << "leaf created" << std::endl;
   }
   nano_caf::behavior get_behavior() {
      return {
         [this](image_buf_msg) {
            std::cout << "image buf received : " << ++counter << std::endl;
         },

         [](nano_caf::exit_msg_atom, nano_caf::exit_reason) {}
      };
   }

   int counter{};
};

struct node_0_actor : nano_caf::behavior_based_actor {
   node_0_actor() {
      std::cout << "root created" << std::endl;
   }
   nano_caf::behavior get_behavior() {
      return {
         [this](const graph_dsl::subgraph_connect_msg& msg) {
            std::cout << "subgraph_connect_msg" << std::endl;
            for(auto& port : *msg.ports) {
               for(auto& handle : port.handles) {
                  ports_[port.port_id].push_back(std::move(handle));
               }
            }

            send();
         },
         [this](image_buf_msg_atom, const std::shared_ptr<const image_buf>& buf) {
            //std::cout << "msg" << std::endl;
            send();
         },
         [](nano_caf::exit_msg_atom, nano_caf::exit_reason) {}
      };
   }

   void send() {
      for(auto& [port, handles] : ports_) {
         for(auto handle : handles) {
            auto msg = std::make_shared<const image_buf>();
            if(handle.send<image_buf_msg>(msg) != nano_caf::status_t::ok) {
               std::cout << "send failed" << std::endl;
            }
         }
      }
   }

private:
   std::unordered_map<GRAPH_DSL_NS::port_id_t, std::vector<nano_caf::actor_handle>> ports_{};
};

struct node_1 : graph_dsl::node_signature{
   constexpr static auto root_id = 0;
   template<typename ... Args>
   static auto spawn(GRAPH_DSL_NS::graph_context& context, Args&& ... args) -> nano_caf::actor_handle {
      return context.get_actor_context().spawn<node_0_actor>(std::forward<Args>(args)...);
   }
};

struct node_2 : graph_dsl::node_signature{
   constexpr static auto root_id = 1;
   template<typename ... Args>
   static auto spawn(GRAPH_DSL_NS::graph_context& context, Args&& ... args) -> nano_caf::actor_handle {
      return context.get_actor_context().spawn<node_0_actor>(std::forward<Args>(args)...);
   }
};

struct node_3 : graph_dsl::node_signature{
   constexpr static auto id = 3;
   template<typename ... Args>
   static auto spawn(GRAPH_DSL_NS::graph_context& context, Args&& ... args) -> nano_caf::actor_handle {
      return context.get_actor_context().spawn<node_5_actor>(std::forward<Args>(args)...);
   }
};

struct node_4 : graph_dsl::node_signature{
   constexpr static auto id = 4;

   template<typename ... Args>
   static auto spawn(GRAPH_DSL_NS::graph_context& context, Args&& ... args) -> nano_caf::actor_handle {
      return context.get_actor_context().spawn<node_8_actor>(std::forward<Args>(args)...);
   }
};



struct node_5 : graph_dsl::node_signature{
   constexpr static auto id = 5;

   template<typename ... Args>
   static auto spawn(GRAPH_DSL_NS::graph_context& context, Args&& ... args) -> nano_caf::actor_handle {
      return context.get_actor_context().spawn<node_5_actor>(std::forward<Args>(args)...);
   }
};
struct node_6 : graph_dsl::node_signature{
   constexpr static auto id = 6;
   template<typename ... Args>
   static auto spawn(GRAPH_DSL_NS::graph_context& context, Args&& ... args) -> nano_caf::actor_handle {
      return context.get_actor_context().spawn<node_8_actor>(std::forward<Args>(args)...);
   }
};
struct node_7 : graph_dsl::node_signature{
   constexpr static auto id = 7;
   template<typename ... Args>
   static auto spawn(GRAPH_DSL_NS::graph_context& context, Args&& ... args) -> nano_caf::actor_handle {
      return context.get_actor_context().spawn<node_8_actor>(std::forward<Args>(args)...);
   }
};
struct node_8 : graph_dsl::node_signature{
   constexpr static auto id = 8;

   template<typename ... Args>
   static auto spawn(GRAPH_DSL_NS::graph_context& context, Args&& ... args) -> nano_caf::actor_handle {
      return context.get_actor_context().spawn<node_8_actor>(std::forward<Args>(args)...);
   }
};

struct port_1 {
   constexpr static graph_dsl::port_id_t root_port_id = 1;
   constexpr static graph_dsl::port_format format{};
   static auto get_port_format(GRAPH_DSL_NS::graph_context&) -> const graph_dsl::port_format& {
      return format;
   }
};
struct port_2 {
   constexpr static graph_dsl::port_id_t root_port_id = 2;
   constexpr static graph_dsl::port_format format{};
   static auto get_port_format(GRAPH_DSL_NS::graph_context&) -> const graph_dsl::port_format& {
      return format;
   }
};
struct port_3 {
   constexpr static graph_dsl::port_id_t root_port_id = 3;
   constexpr static graph_dsl::port_format format{};
   static auto get_port_format(GRAPH_DSL_NS::graph_context&) -> const graph_dsl::port_format& {
      return format;
   }
};
struct port_4 {
   constexpr static graph_dsl::port_id_t root_port_id = 4;
   constexpr static graph_dsl::port_format format{};
   static auto get_port_format(GRAPH_DSL_NS::graph_context&) -> const graph_dsl::port_format& {
      return format;
   }
};
struct port_5 {
   constexpr static graph_dsl::port_format format{};
   static auto get_port_format(GRAPH_DSL_NS::graph_context&) -> const graph_dsl::port_format& {
      return format;
   }
};
struct port_6 {
   constexpr static graph_dsl::port_format format{};
   static auto get_port_format(GRAPH_DSL_NS::graph_context&) -> const graph_dsl::port_format& {
      return format;
   }
};
struct port_7 {
   constexpr static graph_dsl::port_format format{};
   static auto get_port_format(GRAPH_DSL_NS::graph_context&) -> const graph_dsl::port_format& {
      return format;
   }
};
struct port_8 {
   constexpr static graph_dsl::port_format format{};
   static auto get_port_format(GRAPH_DSL_NS::graph_context&) -> const graph_dsl::port_format& {
      return format;
   }
};
struct port_9 {
   constexpr static graph_dsl::port_format format{};
   static auto get_port_format(GRAPH_DSL_NS::graph_context&) -> const graph_dsl::port_format& {
      return format;
   }
};

struct cond_1 {
   auto operator()(GRAPH_DSL_NS::graph_context&) const -> GRAPH_DSL_NS::result_t<bool> {
      return true;
   }
};

struct cond_2 {
   auto operator()(GRAPH_DSL_NS::graph_context&) const -> GRAPH_DSL_NS::result_t<bool> {
      return false;
   }
};
namespace {

   using root_node =
   __source(node_1,
            __port(port_1) -> node_8,
            __port(port_2) -> __maybe(cond_2, node_3),
            __port(port_3) -> __either(cond_1, node_8, node_3),
            __port(port_4) -> __fork(node_5, node_4, __maybe(cond_2, node_8)));

   using grap_def = __sub_graph(
      __source( node_1
              , __port(port_1) -> node_8
              , __port(port_2) -> __maybe(cond_2, node_3)
              , __port(port_3) -> __either(cond_1, node_8, node_3)
              , __port(port_4) -> __fork(node_5, node_4, __maybe(cond_2, node_8))),
      __source( node_2
              , __port(port_1) -> node_7),
        __node( node_5
              , __port(port_5) -> node_8
              , __port(port_6) -> __fork(node_4, __maybe(cond_2, node_3))),
        __node( node_3
              , __port(port_7) -> node_4
              , __port(port_8) -> __fork(node_8, node_6)
              , __port(port_9)->node_7));

   template<typename T>
   struct S;

}

int main() {
   nano_caf::actor_system actor_system;
   actor_system.start(10);
   GRAPH_DSL_NS::root_nodes<node_1, node_2> roots;

   GRAPH_DSL_NS::graph_context context{actor_system, roots};

   if(auto status = roots.get<0>().start(context); status != GRAPH_DSL_NS::status_t::Ok) {
      return -1;
   }

   if(auto status = roots.get<1>().start(context); status != GRAPH_DSL_NS::status_t::Ok) {
      return -1;
   }


   grap_def graph;

   if(auto status = graph.build(context); status != GRAPH_DSL_NS::status_t::Ok) {
      return -1;
   }

   if(auto status = graph.start(context); status != GRAPH_DSL_NS::status_t::Ok) {
      return -1;
   }

   graph.dump();

   std::cout << actor_system.get_num_of_actors() << std::endl;
   std::cout.flush();

   auto handle = roots.get<0>().get_handle();
   for(int i = 0; i<100; i++) {
      auto msg = std::make_shared<const image_buf>();
      handle.send<image_buf_msg>(msg);
   }

   //std::cout << actor_system.get_num_of_actors() << std::endl;

   //roots.get<0>().wait_for_exit();
   actor_system.shutdown();

   return 0;
}

