//
// Created by Darwin Yuan on 2020/8/16.
//

#ifndef GRAPH_GRAPH_MSGS_H
#define GRAPH_GRAPH_MSGS_H

#include <nano-caf/core/msg/predefined-msgs.h>
#include <graph/graph_ns.h>
#include <graph/core/actor_ports.h>
#include <memory>

GRAPH_DSL_NS_BEGIN

CAF_def_message(ports_update_msg,
    (ports, std::unique_ptr<actor_ports>));

GRAPH_DSL_NS_END

#endif //GRAPH_GRAPH_MSGS_H
