#include "light_switch.h"

#define ALLOW_SINGLE_AFTER_DOUBLE_CLICK_TIME_MS 300

namespace esphome {
namespace light_switch {

static const char *const TAG = "light_switch";
std::string LightSwitch::service_name = "";

void LightSwitch::setup() {
  // Declare a command service
  //  - Service will be called "esphome.<node_name>_<name>" in Home Assistant.
  //  - The service has one argument (type inferred from method definition):
  //     - command: string
  this->register_service(&LightSwitch::process_ha_command, this->get_id(), {"command"});
}

void LightSwitch::set_service_name(const std::string &value) {
  if (LightSwitch::service_name.empty())
    LightSwitch::service_name = value;
}

void LightSwitch::dump_config() {
  ESP_LOGCONFIG(TAG, "Light Switch '%s'", this->get_id().c_str());
  ESP_LOGCONFIG(TAG, "  Register service '%s_%s'", LightSwitch::service_name.c_str(), this->get_id().c_str());
  ESP_LOGCONFIG(TAG, "  Publishes events '%s.light_switch'", LightSwitch::service_name.c_str());
  this->groups->dump_config(TAG);
}

void LightSwitch::set_ha_enabled_switch(ha_enabled_switch::HaEnabledSwitch *ha_enabled_switch) {
  this->ha_enabled_switch = ha_enabled_switch;
}

void LightSwitch::init_groups(std::vector<std::vector<switch_::Switch *>> groups) {
  this->groups = new SwitchGroups(groups);
}

void LightSwitch::single_click() {
  if (!this->is_allowed_single_click())
    return;

  ESP_LOGD(TAG, "'%s': Single clicked.", this->get_id().c_str());

  if (this->is_ha_available())
    send_ha_event("single_click");
  else
    this->toggle_or_turn_off();
}

void LightSwitch::double_click() {
  ESP_LOGD(TAG, "'%s': Double clicked.", this->get_id().c_str());

  this->last_double_click_time = millis();

  if (this->is_ha_available())
    send_ha_event("double_click");
  else
    this->turn_all_switches_on();
}

void LightSwitch::long_click() {
  ESP_LOGD(TAG, "'%s': Long clicked.", this->get_id().c_str());

  if (this->is_ha_available())
    send_ha_event("long_click");
  else
    this->long_click_callback_.call();
}

void LightSwitch::add_on_long_click_callback(std::function<void()> &&callback) {
  this->long_click_callback_.add(std::move(callback));
}

// protected
bool LightSwitch::is_ha_available() {
  return this->api_is_connected() && this->ha_enabled_switch && this->ha_enabled_switch->state;
}

bool LightSwitch::api_is_connected() {
  return api::global_api_server != nullptr && api::global_api_server->is_connected();
}

void LightSwitch::send_ha_event(const std::string action) {
  std::map<std::string, std::string> data = {
      {"id", this->get_id()},
      {"action", action},
  };

  this->fire_homeassistant_event(LightSwitch::service_name + ".light_switch", data);
}

void LightSwitch::process_ha_command(std::string command) {
  if (command == "turn_all_switches_on" || command == "turn_all_on" || command == "all_on")
    this->turn_all_switches_on();
  else if (command == "turn_all_switches_off" || command == "turn_all_off" || command == "all_off")
    this->turn_all_switches_off();
  else if (command == "toggle")
    this->toggle();
  else if (command == "toggle_or_turn_off")
    this->toggle_or_turn_off();
}

// Check if a single click has been pressed immediately after double
bool LightSwitch::is_allowed_single_click() {
  uint32_t diff_time = millis() - this->last_double_click_time;
  return diff_time > ALLOW_SINGLE_AFTER_DOUBLE_CLICK_TIME_MS;
}

void LightSwitch::turn_all_switches_on() {
  ESP_LOGD(TAG, "'%s': Turn all switches ON.", this->get_id().c_str());
  this->groups->turn_all_switches_on();
}

void LightSwitch::turn_all_switches_off() {
  ESP_LOGD(TAG, "'%s': Turn all switches OFF.", this->get_id().c_str());
  this->groups->turn_all_switches_off();
}

void LightSwitch::toggle() {
  ESP_LOGD(TAG, "'%s': Toggle.", this->get_id().c_str());
  this->groups->toggle();
}

void LightSwitch::toggle_or_turn_off() {
  ESP_LOGD(TAG, "'%s': Toggle or turn OFF.", this->get_id().c_str());
  this->groups->toggle_or_turn_off();
}

}  // namespace light_switch
}  // namespace esphome
