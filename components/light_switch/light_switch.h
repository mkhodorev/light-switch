#pragma once

#include "switch_groups.h"
#include "esphome.h"

// Declare a service
//  - Service will be called "esphome.<node_name>_<name>" in Home Assistant.
//  - The service has one argument:
//     - command: string_command
//
//  - String command can be:
//     - turn_all_switches_on | turn_all_on | all_on
//     - turn_all_switches_off | turn_all_off | all_off
//     - toggle
//     - toggle_or_turn_off
//
// Publishes events to esphome.<node_name>.light_switch
// Event data:
//  - id: <name>
//  - action: <action> ('single_click', 'double_click', 'long_click')

namespace esphome {
namespace light_switch {

class LightSwitch : public Component, public api::CustomAPIDevice {
 public:
  void setup() override;
  void dump_config() override;
  void set_id(const std::string &id) { this->id = id; }
  std::string get_id() { return this->id; }
  void set_ha_enabled_switch(ha_enabled_switch::HaEnabledSwitch *ha_enabled_switch);
  void init_groups(std::vector<std::vector<switch_::Switch *>> groups);
  void single_click();
  void double_click();
  void long_click();
  void add_on_long_click_callback(std::function<void()> &&callback);
  void set_service_name(const std::string &value);

 protected:
  std::string id;
  SwitchGroups *groups;
  uint32_t last_double_click_time = 0;
  ha_enabled_switch::HaEnabledSwitch *ha_enabled_switch;
  CallbackManager<void()> long_click_callback_{};
  static std::string service_name;

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

template<typename... Ts> class SingleClickAction : public Action<Ts...> {
 public:
  explicit SingleClickAction(LightSwitch *ls) : ls_(ls) {}

  void play(Ts... x) override { this->ls_->single_click(); }

 protected:
  LightSwitch *ls_;
};

template<typename... Ts> class DoubleClickAction : public Action<Ts...> {
 public:
  explicit DoubleClickAction(LightSwitch *ls) : ls_(ls) {}

  void play(Ts... x) override { this->ls_->double_click(); }

 protected:
  LightSwitch *ls_;
};

template<typename... Ts> class LongClickAction : public Action<Ts...> {
 public:
  explicit LongClickAction(LightSwitch *ls) : ls_(ls) {}

  void play(Ts... x) override { this->ls_->long_click(); }

 protected:
  LightSwitch *ls_;
};

class LongClickTrigger : public Trigger<> {
 public:
  LongClickTrigger(LightSwitch *ls) {
    ls->add_on_long_click_callback([this]() { this->trigger(); });
  }
};

}  // namespace light_switch
}  // namespace esphome