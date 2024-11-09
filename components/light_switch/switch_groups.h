
#pragma once

#include "esphome/components/switch/switch.h"

namespace esphome {
namespace light_switch {

#define TURN_OFF_INTERVAL_MS 2000

class SwitchGroups {
 public:
  SwitchGroups(std::vector<std::vector<switch_::Switch *>> groups);
  void turn_all_switches_on();
  void turn_all_switches_off();
  void toggle();
  void toggle_or_turn_off();

 private:
  std::vector<switch_::Switch *> all_switches;
  std::vector<std::vector<switch_::Switch *>> groups;
  uint8_t active_group = 0;  // 0 = all off, 255 = all on
  uint32_t last_command_time = 0;

  void add_group(std::vector<switch_::Switch *> group);
  void add_to_all_switches(std::vector<switch_::Switch *> group);
  bool is_include_in_all_switches(switch_::Switch *sw);
  bool is_include_in_group(std::vector<switch_::Switch *> group, switch_::Switch *sw);
  bool is_any_switches_on();
  void turn_group_on_by_id(uint8_t id);
};

}  // namespace light_switch
}  // namespace esphome
