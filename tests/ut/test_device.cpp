//
// Created by Darwin Yuan on 2020/8/20.
//

#include <catch.hpp>
#include <graph/core/dsl/target_state_selector.h>

struct device_0 {
   constexpr static uint8_t Device_Id = 0;
};

struct device_1 {
   constexpr static uint8_t Device_Id = 1;
};

struct device_2 {
   constexpr static uint8_t Device_Id = 2;
};

namespace {

   TEST_CASE("devices") {
      static_assert(__g_STATE(device_0, __g_PREVIEW(device_1))::Num_Of_Devices == 2);

      static_assert(__g_STATE(device_0, __g_PREVIEW(device_1))::Devices[0].device_id == 0);
      static_assert(!__g_STATE(device_0, __g_PREVIEW(device_1))::Devices[0].is_preview);

      static_assert(__g_STATE(device_0, __g_PREVIEW(device_1))::Devices[1].device_id == 1);
      static_assert(__g_STATE(device_0, __g_PREVIEW(device_1))::Devices[1].is_preview);
   }

   struct dict {
      auto get_scene() const -> uint8_t {
         return scene;
      }
      auto get_condition_1() const -> double {
         return condition_1;
      }
      auto get_condition_2() const -> unsigned int {
         return condition_2;
      }
      auto get_condition_3() const -> unsigned int {
         return condition_3;
      }

      uint8_t scene{1};
      double condition_1{1.5};
      unsigned int condition_2{4};
      unsigned int condition_3{6};
   };

   using selector =
      __g_STATE_SELECTOR(
         __g_WHEN(__g_SCENE(1), __g_COND_1( [1.0, 2.0} ))
      -> __g_STATE(device_0, __g_PREVIEW(device_1)),

         __g_WHEN(__g_SCENE(1), __g_COND_1( [1.0, 2.0} ), __g_COND_2(1, 5))
      -> __g_STATE(__g_PREVIEW(device_0), device_1),

         __g_WHEN(__g_SCENE(1), __g_COND_1( [1.5, 2.0} ), __g_COND_2(1, 5), __g_COND_3(3, 5))
      -> __g_STATE(device_2, __g_PREVIEW(device_1)));

   TEST_CASE("selector 1") {
      GRAPH_DSL_NS::device_info const* device_info{};
      size_t size = 0;
      REQUIRE(selector::sorted_entries::find(dict{}, device_info, size));
      REQUIRE(device_info != nullptr);
      REQUIRE(size == 2);
      REQUIRE(device_info[0].device_id == 0);
      REQUIRE(device_info[1].device_id == 1);
      REQUIRE(device_info[0].is_preview);
      REQUIRE_FALSE(device_info[1].is_preview);
   }

   TEST_CASE("selector 2") {
      GRAPH_DSL_NS::device_info const* device_info{};
      size_t size = 0;
      dict dict2;
      dict2.condition_3 = 4;
      REQUIRE(selector::sorted_entries::find(dict2, device_info, size));
      REQUIRE(device_info != nullptr);
      REQUIRE(size == 2);
      REQUIRE(device_info[0].device_id == 2);
      REQUIRE(device_info[1].device_id == 1);
      REQUIRE_FALSE(device_info[0].is_preview);
      REQUIRE(device_info[1].is_preview);
   }

   TEST_CASE("selector 3") {
      GRAPH_DSL_NS::device_info const* device_info{};
      size_t size = 0;
      dict dict2;
      dict2.condition_2 = 6;
      dict2.condition_3 = 4;
      REQUIRE(selector::sorted_entries::find(dict2, device_info, size));
      REQUIRE(device_info != nullptr);
      REQUIRE(size == 2);
      REQUIRE(device_info[0].device_id == 0);
      REQUIRE(device_info[1].device_id == 1);
      REQUIRE_FALSE(device_info[0].is_preview);
      REQUIRE(device_info[1].is_preview);
   }
}