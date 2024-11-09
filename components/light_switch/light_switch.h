#pragma once

#include "switch_groups.h"
#include "esphome.h"

// Declare a service
//  - Service will be called "esphome.light_switch" in Home Assistant.
//  - The service has one argument:
//     - command: string_command
//
//  - String command can be:
//     - turn_all_switches_on | turn_all_on | all_on
//     - turn_all_switches_off | turn_all_off | all_off
//     - toggle
//     - toggle_or_turn_off

namespace esphome {
namespace light_switch {

#define LOG_LIGHT_SWITCH(prefix, type, obj) \
  if ((obj) != nullptr) { \
    ESP_LOGCONFIG(TAG, "%s%s '%s'", prefix, LOG_STR_LITERAL(type), (obj)->get_id().c_str()); \
    if (!(obj)->get_icon().empty()) { \
      ESP_LOGCONFIG(TAG, "%s  Icon: '%s'", prefix, (obj)->get_icon().c_str()); \
    } \
  }

class LightSwitch : public Component {
 public:
  void setup() override;
  void set_id(const std::string &id) { this->id = id; }
  std::string get_id() { return this->id; }
  void set_ha_enabled_switch(ha_enabled_switch::HaEnabledSwitch *ha_enabled_switch) {
    this->ha_enabled_switch = ha_enabled_switch;
  }
  void single_click();
  void double_click();
  void long_click();
  void init_groups(std::vector<std::vector<switch_::Switch *>> groups);

 protected:
  std::string id;
  SwitchGroups *groups;
  uint32_t last_double_click_time = 0;
  ha_enabled_switch::HaEnabledSwitch *ha_enabled_switch;

  void long_click_action();
  bool is_ha_available();
  bool api_is_connected();
  void send_ha_event(const std::string action);
  void process_ha_command(std::string command);
  bool is_allowed_single_click();
  void turn_all_switches_on();
  void turn_all_switches_off();
  void toggle();
  void toggle_or_turn_off();
};

}  // namespace light_switch
}  // namespace esphome