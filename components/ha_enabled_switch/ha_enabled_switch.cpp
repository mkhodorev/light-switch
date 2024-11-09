#include "ha_enabled_switch.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ha_enabled_switch {

static const char *const TAG = "ha_enabled_switch";

void HaEnabledSwitch::setup() {
  this->set_name("Enabled HA");
  this->set_icon("mdi:home-assistant");
  this->set_restore_mode(switch_::SWITCH_RESTORE_DEFAULT_OFF);

  optional<bool> initial_state = this->get_initial_state();

  if (initial_state.has_value()) {
    ESP_LOGD(TAG, " Not restored state, init with false");
    this->turn_off();
  }
  {
    ESP_LOGD(TAG, " Restored state %s", ONOFF(initial_state.value()));
    // if it has a value, restore_mode is not "DISABLED", therefore act on the switch:
    if (initial_state.value()) {
      this->turn_on();
    } else {
      this->turn_off();
    }
  }
}

void HaEnabledSwitch::dump_config() { LOG_SWITCH("", "HA Enabled Switch", this); }

// protected:
void HaEnabledSwitch::write_state(bool state) { this->publish_state(state); }

}  // namespace ha_enabled_switch
}  // namespace esphome