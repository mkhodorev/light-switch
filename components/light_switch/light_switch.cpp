#include "light_switch.h"

#define ALLOW_SINGLE_AFTER_DOUBLE_CLICK_TIME_MS 300

namespace esphome {
namespace light_switch {

static const char *const TAG = "light_switch";

void LightSwitch::setup() {
  //     register_service(&LightSwitch::process_command, this->name, {"command"});
}

// template<typename... Ts> class UserServiceTrigger : public UserServiceBase<Ts...>, public Trigger<Ts...> {
//  public:
//   UserServiceTrigger(const std::string &name, const std::array<std::string, sizeof...(Ts)> &arg_names)
//       : UserServiceBase<Ts...>(name, arg_names) {}

//  protected:
//   void execute(Ts... x) override { this->trigger(x...); }  // NOLINT
// };

// template<typename T, typename... Ts> class UserServiceTrigger : public UserServiceBase<Ts...> {
//  public:
//   UserServiceTrigger(const std::string &name, const std::array<std::string, sizeof...(Ts)> &arg_names, T *obj,
//                      void (T::*callback)(Ts...))
//       : UserServiceBase<Ts...>(name, arg_names), obj_(obj), callback_(callback) {}

//  protected:
//   void execute(Ts... x) override { (this->obj_->*this->callback_)(x...); }  // NOLINT

//   T *obj_;
//   void (T::*callback_)(Ts...);
// };

// template<typename T, typename... Ts> class CustomAPIDeviceService : public UserServiceBase<Ts...> {
//  public:
//   CustomAPIDeviceService(const std::string &name, const std::array<std::string, sizeof...(Ts)> &arg_names, T *obj,
//                          void (T::*callback)(Ts...))
//       : UserServiceBase<Ts...>(name, arg_names), obj_(obj), callback_(callback) {}

//  protected:
//   void execute(Ts... x) override { (this->obj_->*this->callback_)(x...); }  // NOLINT

//   T *obj_;
//   void (T::*callback_)(Ts...);
// };

// template<typename T, typename... Ts>
// void LightSwitch::register_service(void (T::*callback)(Ts...), const std::string &name,
//                                    const std::array<std::string, sizeof...(Ts)> &arg_names) {
//   auto *service = new CustomAPIDeviceService<T, Ts...>(name, arg_names, (T *) this, callback);  // NOLINT
//   global_api_server->register_user_service(service);
// }

void LightSwitch::single_click() {
  if (!this->is_allowed_single_click())
    return;

  ESP_LOGD(TAG, "'%s': Single clicked.", this->id.c_str());

  if (this->is_ha_available())
    send_ha_event("single_click");
  else
    this->toggle_or_turn_off();
}

void LightSwitch::double_click() {
  ESP_LOGD(TAG, "'%s': Double clicked.", this->id.c_str());

  this->last_double_click_time = millis();

  if (this->is_ha_available())
    send_ha_event("double_click");
  else
    this->turn_all_switches_on();
}

void LightSwitch::long_click() {
  ESP_LOGD(TAG, "'%s': Long clicked.", this->id.c_str());

  if (this->is_ha_available())
    send_ha_event("long_click");
  else
    this->long_click_action();
}

void LightSwitch::init_groups(std::vector<std::vector<switch_::Switch *>> groups) {
  this->groups = new SwitchGroups(groups);
}

// protected
void LightSwitch::long_click_action() {
  // if(this->long_click_script != nullptr)
  //   id(this->long_click_script).execute();
  ESP_LOGD(TAG, "'%s': Long clicked!", this->id.c_str());
}

bool LightSwitch::is_ha_available() {
  return this->api_is_connected() && this->ha_enabled_switch && this->ha_enabled_switch->state;
}

bool LightSwitch::api_is_connected() {
  return api::global_api_server != nullptr && api::global_api_server->is_connected();
}

void LightSwitch::send_ha_event(const std::string action) {
  api::HomeassistantServiceResponse resp;
  resp.service = "light_switch";

  api::HomeassistantServiceMap id_kv;
  id_kv.key = "id";
  id_kv.value = this->id;
  resp.data.push_back(id_kv);

  api::HomeassistantServiceMap action_kv;
  action_kv.key = "action";
  action_kv.value = action;
  resp.data.push_back(action_kv);

  api::global_api_server->send_homeassistant_service_call(resp);
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
  ESP_LOGD(TAG, "'%s': Turn all switches ON.", this->id.c_str());
  this->groups->turn_all_switches_on();
}

void LightSwitch::turn_all_switches_off() {
  ESP_LOGD(TAG, "'%s': Turn all switches OFF.", this->id.c_str());
  this->groups->turn_all_switches_off();
}

void LightSwitch::toggle() {
  ESP_LOGD(TAG, "'%s': Toggle.", this->id.c_str());
  this->groups->toggle();
}

void LightSwitch::toggle_or_turn_off() {
  ESP_LOGD(TAG, "'%s': Toggle or turn OFF.", this->id.c_str());
  this->groups->toggle_or_turn_off();
}

}  // namespace light_switch
}  // namespace esphome
