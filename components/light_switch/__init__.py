import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import switch, ha_enabled_switch
# import sys

DEPENDENCIES = ["api"]
AUTO_LOAD = ["switch", "ha_enabled_switch"]

light_switch_ns = cg.esphome_ns.namespace("light_switch")
LightSwitch = light_switch_ns.class_("LightSwitch", cg.Component)
LightSwitchPtr = LightSwitch.operator("ptr")

LIGHT_SCHEMES = "light_schemes"
ON_LONG_CLICK = "on_long_click"

MULTI_CONF = True
CONFIG_SCHEMA = cv.Schema({
    cv.Required(CONF_ID): cv.declare_id(LightSwitch),
    cv.GenerateID(ha_enabled_switch.HA_ENABLED_SWITCH_ID): cv.use_id(ha_enabled_switch.HaEnabledSwitch),
    cv.Required(LIGHT_SCHEMES): cv.ensure_list(
        cv.ensure_list(
            cv.use_id(switch.Switch)
        )
    ),
}).extend(cv.COMPONENT_SCHEMA)


async def light_schemes(config):
    light_schemes = []
    
    for scheme in config[LIGHT_SCHEMES]:
        schema = []
        for sw in scheme:
            schema.append(await cg.get_variable(sw))
        light_schemes.append(schema)

    return light_schemes

async def to_code(config):
    print("------start-----", file=sys.stderr)
    
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_id(str(config[CONF_ID])))

    ha = await cg.get_variable(config[ha_enabled_switch.HA_ENABLED_SWITCH_ID])
    cg.add(var.set_ha_enabled_switch(ha))

    ls = await light_schemes(config)
    cg.add(var.init_groups(ls))

