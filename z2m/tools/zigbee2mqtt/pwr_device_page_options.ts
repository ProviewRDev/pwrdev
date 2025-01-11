import {strict as assert} from 'assert';

function addTrailingDot(text: string) {
    text = text.trim();
    return text.endsWith('.') || text.endsWith('`') ? text : text + '.';
}

export function pwrGenerateOptions(definition) {
    if (definition.options.length == 0) return '';
    return `
## Options
*[How to use device type specific configuration](../guide/configuration/devices-groups.md#specific-device-options)*

${definition.options.map((e) => getOptionDocs(e)).join('\n\n')}
`;
}

function getOptionDocs(option) {
    let extra = null;

    if (option.type === 'numeric') {
        extra = 'The value must be a number';
        if (option.value_min != null) extra += ` with a minimum value of \`${option.value_min}\``;
        if (option.value_min != null && option.value_max != null) extra += ` and with a`;
        if (option.value_max != null) extra += ` with a maximum value of \`${option.value_max}\``;
    } else if (option.type === 'binary') {
        extra = `The value must be \`${option.value_on}\` or \`${option.value_off}\``;
    } else if (option.type === 'enum') {
        extra = `The value must be one of ${option.values.map((v) => `\`${v}\``).join(', ')}`;
    } else if (option.type === 'list') {
        extra = `The value must be a list of ${option.item_type}`;
    } else if (option.type === 'text') {
        extra = `The value must be textual`;
    } else if (option.type === 'composite' && option.property === 'simulated_brightness') {
        extra = `Example:
\`\`\`yaml
simulated_brightness:
  delta: 20 # delta per interval, default = 20
  interval: 200 # interval in milliseconds, default = 200
\`\`\`
`;
    } else if (option.type === 'composite' && option.property === 'identity_effect') {
        extra = `Example:
\`\`\`yaml
identity_effect:
  effect: blink 3 # allowed: 'blink 3', 'fixed', 'blink green', 'blink blue'
  color: red # allowed: 'default', 'red', 'green', 'blue', 'lightblue', 'yellow', 'pink', 'white'
\`\`\`
`;
    }

    //console.log(option);
    console.log("");
    console.log("**Option");
    console.log("/**");
    console.log("@brief", option.description);
    console.log(option.description);
    console.log(extra);
    console.log("*/");
    console.log("Name", option.name);
    console.log("Description", option.description);
    console.log("Label", option.label);
    console.log("Type", option.type);
    console.log("Access", option.access);
    console.log("Property", option.property);
    if (option.endpoint != null)
      console.log("Endpoint", option.endpoint);
    if (option.features != null) {
      if (option.property != 'color_rgb' && option.property != 'color_xy' && option.property != 'color' && option.property != 'simulated_brightness') {
        console.log("Features **Warning**");
      }
      process.stdout.write("Features ");
      for (let j = 0; j < option.features.length; j++) {
	process.stdout.write(option.features[j].name);
        if (j != option.features.length - 1)
          process.stdout.write(",");
      }
      console.log('');
      //console.log("Features", option.features);
    }
    if (option.category != null)
      console.log("Category", option.category);
    if (option.unit != null)
      console.log("Unit", option.unit);
    if (option.presets != null)
      console.log("Presets", option.presets);

    if (option.type === 'numeric') {
      if (option.value_min != null) 
        console.log("MinValue", option.value_min);
      if (option.value_max != null) 
        console.log("MaxValue", option.value_max);
      if (option.value_step != null) 
        console.log("StepValue", option.value_step);
    } else if (option.type === 'binary') {
      console.log("OnValue", option.value_on);
      console.log("OffValue", option.value_off);
      if (option.value_toggle != null)
        console.log("ToggleValue", option.value_toggle);
    } else if (option.type === 'enum') {
      let txt = `${option.values.map((v) => `${v}`).join(',')}`;
      console.log("EnumValues", txt);
    } else if (option.type === 'list') {
      let txt = `${option.values}`;
      console.log("ListValues", txt);
    } else if (option.type === 'text') {
    }
    console.log("**EndOption");

//    assert(extra != null, `No option doc generator for '${JSON.stringify(option)}'`);

    return ``;
}
