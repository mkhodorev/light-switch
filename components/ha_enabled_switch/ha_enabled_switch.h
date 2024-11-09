#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace ha_enabled_switch {

class HaEnabledSwitch : public switch_::Switch, public Component {
 public:
  void setup() override;
  void dump_config() override;

 protected:
  void write_state(bool state);
};

}  // namespace ha_enabled_switch
}  // namespace esphome