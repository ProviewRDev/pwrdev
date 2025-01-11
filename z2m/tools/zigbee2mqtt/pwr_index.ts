import throat from 'throat';
import pwrGenerateDevice from './pwr_generate_device';
import {allDefinitions} from './utils';
import {removeObsoleteDevices} from './remove-obsolete-devices';

async function pwrGenerateDevices() {
    const genDevThrottled = throat(20, (device) => pwrGenerateDevice(device));
    await Promise.all(allDefinitions.map(genDevThrottled));
    console.log('Generated device-pages');
}

(async function () {
    await removeObsoleteDevices(allDefinitions);
    await Promise.all([pwrGenerateDevices()]);
    console.log('Done');
})();
