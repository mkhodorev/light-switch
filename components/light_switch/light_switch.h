#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
// #include "esphome/components/api/custom_api_device.h"
#include "esphome/core/helpers.h"
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
  ESP_LOGCONFIG(TAG, "%s%s '%s'", prefix, LOG_STR_LITERAL(type), (obj)->get_name().c_str()); \
  if (!(obj)->get_icon().empty()) { \
    ESP_LOGCONFIG(TAG, "%s  Icon: '%s'", prefix, (obj)->get_icon().c_str()); \
  } \
}

class LightSwitch : public Component {
 public:
  void setup();
  void single_click();
  void double_click();
  void long_click();

  private:
  std::string name;
  SwitchGroups groups;
  uint32_t last_double_click_time = 0;

  std::string get_name() { return this->name; }
  void long_click_action();
  bool is_hass();
  bool api_is_connected();
  void send_hass_event(const  std::string action);
  void process_command(std::string command);
  bool is_allowed_single_click();
  void turn_all_switches_on();
  void turn_all_switches_off();
  void toggle();
  void toggle_or_turn_off();
};

}  // namespace light_switch
}  // namespace esphome