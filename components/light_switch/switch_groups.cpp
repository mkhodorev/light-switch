#include "switch_groups.h"
#include "esphome.h"

namespace esphome {
namespace light_switch {

#define TURN_OFF_INTERVAL_MS 2000

SwitchGroups::SwitchGroups(std::vector<std::vector<switch_::Switch *>> groups) {
  // first group is all switches off
  this->groups.push_back(std::vector<switch_::Switch *>());
  this->active_group = 0;

  for (int i = 0; i < groups.size(); i++)
    this->add_group(groups[i]);
}

void SwitchGroups::add_group(std::vector<switch_::Switch *> group) {
  this->groups.push_back(group);
  this->add_to_all_switches(group);
}

void SwitchGroups::turn_all_switches_on() {
  for (int i = 0; i < this->all_switches.size(); i++) {
    this->all_switches[i]->turn_on();
  }

  this->active_group = 255;
}

void SwitchGroups::turn_all_switches_off() {
  for (int i = 0; i < this->all_switches.size(); i++) {
    this->all_switches[i]->turn_off();
  }

  this->active_group = 0;
}

// toggle between groups (0, 1, 2, ..., 0, 1, 2, ...)
void SwitchGroups::toggle() {
  if (!this->is_any_switches_on() && this->active_group > 0)
    this->active_group = 0;

  this->active_group++;
  if (this->active_group >= this->groups.size())
    this->active_group = 0;

  this->turn_group_on_by_id(this->active_group);
}

void SwitchGroups::toggle_or_turn_off() {
  uint32_t diff_time = millis() - this->last_command_time;
  this->last_command_time = millis();

  if (diff_time > TURN_OFF_INTERVAL_MS && this->is_any_switches_on())
    this->turn_all_switches_off();
  else
    this->toggle();
}

void SwitchGroups::dump_config(const char *tag) {
  ESP_LOGCONFIG(tag, "  Groups:");

  for (int i = 1; i < this->groups.size(); i++) {
    std::string group_ids = "";

    for (int j = 0; j < this->groups[i].size(); j++) {
      if (!group_ids.empty())
        group_ids += ", ";

      group_ids += this->groups[i][j]->get_object_id();
    }

    ESP_LOGCONFIG(tag, "    - [%s]", group_ids.c_str());
  }
}

// private:
void SwitchGroups::add_to_all_switches(std::vector<switch_::Switch *> group) {
  for (int i = 0; i < group.size(); i++) {
    if (!this->is_include_in_all_switches(group[i]))
      this->all_switches.push_back(group[i]);
  }
}

bool SwitchGroups::is_include_in_all_switches(switch_::Switch *sw) {
  return this->is_include_in_group(this->all_switches, sw);
}

bool SwitchGroups::is_include_in_group(std::vector<switch_::Switch *> group, switch_::Switch *sw) {
  for (int i = 0; i < group.size(); i++) {
    if (sw == group[i])
      return true;
  }

  return false;
}

bool SwitchGroups::is_any_switches_on() {
  for (int i = 0; i < this->all_switches.size(); i++) {
    if (this->all_switches[i]->state)
      return true;
  }

  return false;
}

void SwitchGroups::turn_group_on_by_id(uint8_t id) {
  for (int i = 0; i < all_switches.size(); i++) {
    if (this->is_include_in_group(this->groups[id], this->all_switches[i]))
      this->all_switches[i]->turn_on();
    else
      this->all_switches[i]->turn_off();
  }
}

}  // namespace light_switch
}  // namespace esphome
