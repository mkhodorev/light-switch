from esphome import automation
from esphome.automation import maybe_simple_id
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TRIGGER_ID
from esphome.components import switch, ha_enabled_switch
from esphome.core import CORE


DEPENDENCIES = ["api", "switch"]
AUTO_LOAD = ["ha_enabled_switch"]

light_switch_ns = cg.esphome_ns.namespace("light_switch")
LightSwitch = light_switch_ns.class_("LightSwitch", cg.Component)
LightSwitchPtr = LightSwitch.operator("ptr")

service_name = "esphome." + CORE.name.replace("-", "_")

SingleClickAction = light_switch_ns.class_("SingleClickAction", automation.Action)
DoubleClickAction = light_switch_ns.class_("DoubleClickAction", automation.Action)
LongClickAction = light_switch_ns.class_("LongClickAction", automation.Action)

LongClickTrigger = light_switch_ns.class_(
    "LongClickTrigger", automation.Trigger.template()
)


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
    cv.Optional(ON_LONG_CLICK): automation.validate_automation(
        {
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(LongClickTrigger),
        }
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


LIGHT_SWITCH_CLICK_SCHEMA = maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(LightSwitch),
    }
)


@automation.register_action("light_switch.single_click", SingleClickAction, LIGHT_SWITCH_CLICK_SCHEMA)
async def light_switch_single_click_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


@automation.register_action("light_switch.double_click", DoubleClickAction, LIGHT_SWITCH_CLICK_SCHEMA)
async def light_switch_double_click_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


@automation.register_action("light_switch.long_click", LongClickAction, LIGHT_SWITCH_CLICK_SCHEMA)
async def light_switch_long_click_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_id(str(config[CONF_ID])))
    cg.add(var.set_service_name(service_name))

    ha = await cg.get_variable(config[ha_enabled_switch.HA_ENABLED_SWITCH_ID])
    cg.add(var.set_ha_enabled_switch(ha))

    ls = await light_schemes(config)
    cg.add(var.init_groups(ls))

    for conf in config.get(ON_LONG_CLICK, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)
