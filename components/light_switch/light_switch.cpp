#include "light_switch.h"
#include "esphome/core/log.h"

namespace esphome {
namespace light_switch {

#define ALLOW_SINGLE_AFTER_DOUBLE_CLICK_TIME_MS 300

static const char *const TAG = "light_switch";
  
void LightSwitch::setup() {
  register_service(&LightSwitch::process_command, this->name, {"command"});
}

void LightSwitch::single_click() {
  if (!this->is_allowed_single_click())
    return;

  ESP_LOGD(TAG, "'%s': Single clicked.", this->get_name().c_str());

  if (this->is_hass())
    send_hass_event("single_click")
  else
    this->toggle_or_turn_off();
}

void LightSwitch::double_click() {
  ESP_LOGD(TAG, "'%s': Double clicked.", this->get_name().c_str());

  this->last_double_click_time = millis();

  if (this->is_hass())
    send_hass_event("double_click")
  else
    this->turn_all_switches_on();
}

void LightSwitch::long_click() {
  ESP_LOGD(TAG, "'%s': Long clicked.", this->get_name().c_str());

  if (this->is_hass())
    send_hass_event("long_click")
  else
    this->long_click_action();
}

// private
void LightSwitch::long_click_action() {
  // if(this->long_click_script != nullptr)
  //   id(this->long_click_script).execute();
  ESP_LOGD(TAG, "'%s': Long clicked!", this->get_name().c_str());
}

  bool LightSwitch::is_hass() {
    return this->api_is_connected() && id(hass_enabled);
  }

  bool LightSwitch::api_is_connected() {
    return api::global_api_server != nullptr && api::global_api_server->is_connected();
  }

void LightSwitch::send_hass_event(const  std::string action) {
  api::HomeassistantServiceResponse resp;
  resp.service = "light_switch";

  api::HomeassistantServiceMap entity_id_kv;
  entity_id_kv.key = "entity_id";
  entity_id_kv.value = this->entity_id;
  resp.data.push_back(entity_id_kv);

  api::HomeassistantServiceMap action_kv;
  entity_id_kv.key = "action";
  entity_id_kv.value = action;
  resp.data.push_back(action_kv);

  api::global_api_server->send_homeassistant_service_call(resp);
}

void LightSwitch::process_command(std::string command) {
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
  ESP_LOGD(TAG, "'%s': Turn all switches ON.", this->get_name().c_str());
  this->groups.turn_all_switches_on();
}

void LightSwitch::turn_all_switches_off() {
  ESP_LOGD(TAG, "'%s': Turn all switches OFF.", this->get_name().c_str());
  this->groups.turn_all_switches_off();
}

void LightSwitch::toggle() {
  ESP_LOGD(TAG, "'%s': Toggle.", this->get_name().c_str());
  this->groups.toggle();
}

void LightSwitch::toggle_or_turn_off() {
  ESP_LOGD(TAG, "'%s': Toggle or turn OFF.", this->get_name().c_str());
  this->groups.toggle_or_turn_off();
}

}  // namespace light_switch
}  // namespace esphome


}  // namespace light_switch
}  // namespace esphome