#include <homeassist.h>

const char *volumeTemplate =
    "homeassistant/sensor/${mdns}_volume${tap}/"
    "config:{\"device_class\":\"volume\",\"name\":\"${mdns}_volume${tap}\","
    "\"unit_of_measurement\":\"L\",\"state_topic\":\"homeassistant/sensor/"
    "${mdns}_volume${tap}/state\",\"json_attributes_topic\":\"homeassistant/"
    "sensor/${mdns}_volume${tap}/"
    "attr\",\"unique_id\":\"${mdns}_volume${tap}\"}|"
    "homeassistant/sensor/${mdns}_volume${tap}/state:${volume}|"
    "homeassistant/sensor/${mdns}_volume${tap}/"
    "attr:{\"glasses\":${glasses}}|";

const char *beerTemplate =
    "homeassistant/sensor/${mdns}_beer${tap}/config:"
    "{\"name\":\"${mdns}_beer${tap}\",\"state_topic\":\"homeassistant/sensor/"
    "${mdns}_beer${tap}/state\",\"json_attributes_topic\":\"homeassistant/"
    "sensor/${mdns}_beer${tap}/"
    "attr\",\"unique_id\":\"${mdns}_beer${tap}\"}|"
    "homeassistant/sensor/${mdns}_beer${tap}/state:${beer-name}|"
    "homeassistant/sensor/${mdns}_beer${tap}/"
    "attr:{\"abv\":${beer-abv},\"abv\":${beer-abv},\"ibu\":${beer-ibu},\"ebc\":"
    "${beer-"
    "ebc}}|";

const char *pourTemplate =
    "homeassistant/sensor/${mdns}_pour${tap}/config:"
    "{\"device_class\":\"volume\",\"name\":\"${mdns}_pour${tap}\",\"unit_of_"
    "measurement\":\"L\",\"state_topic\":\"homeassistant/sensor/"
    "${mdns}_pour${tap}/state\",\"unique_id\":\"${mdns}_pour${tap}\"}|"
    "homeassistant/sensor/${mdns}_pour${tap}/state:${pour}|";

const char *tempTemplate =
    "homeassistant/sensor/${mdns}_temp/config:"
    "{\"device_class\":\"temperature\",\"name\":\"${mdns}_temp\",\"unit_of_"
    "measurement\":\"${temp-format}\",\"state_topic\":\"homeassistant/sensor/"
    "${mdns}_temp/state\",\"unique_id\":\"${mdns}_temp\"}|"
    "homeassistant/sensor/${mdns}_temp/state:${temp}|";

void HomeAssist::sendTempInformation(float tempC)
{
    if (!myConfig.hasTargetMqtt())
        return;

    TemplatingEngine tpl;

    tpl.setVal("${mdns}", myConfig.getMDNS());

    // if (myConfig.isTempFormatC()) {
    tpl.setVal("${temp}", tempC);
    tpl.setVal("${temp-format}", "°C");
    // } else {
    //  tpl.setVal("${temp}", convertCtoF(tempC));
    //  tpl.setVal("${temp-format}", "°F");
    // }

    const char *out = tpl.create(tempTemplate);
    EspSerial.print(out);
    EspSerial.print(CR);
    String outStr(out);
    _push->sendMqtt(outStr);

    Log.notice(F("HA  : Sending temp information to HA, last %FC" CR), tempC);

    tpl.freeMemory();
}

void HomeAssist::sendTapInformation(UnitIndex idx, float stableVol,
                                    float glasses)
{
    if (!myConfig.hasTargetMqtt())
        return;

    TemplatingEngine tpl;

    tpl.setVal("${mdns}", myConfig.getMDNS());
    tpl.setVal("${id}", myConfig.getID());
    tpl.setVal("${volume}", stableVol, 3);
    tpl.setVal("${glasses}", glasses, 1);
    tpl.setVal("${tap}", static_cast<int>(idx) + 1);
    tpl.setVal("${beer-name}", myConfig.getBeerName(idx));
    tpl.setVal("${beer-abv}", myConfig.getBeerABV(idx));
    tpl.setVal("${beer-ibu}", myConfig.getBeerIBU(idx));
    tpl.setVal("${beer-ebc}", myConfig.getBeerEBC(idx));

    const char *out = tpl.create(volumeTemplate);
    EspSerial.print(out);
    EspSerial.print(CR);
    String outStr(out);
    _push->sendMqtt(outStr);

    Log.notice(F("HA  : Sending TAP information to HA, last %Fl [%d]" CR),
               stableVol);

    out = tpl.create(beerTemplate);
    EspSerial.print(out);
    EspSerial.print(CR);
    outStr = out;
    _push->sendMqtt(outStr);

    tpl.freeMemory();
}

void HomeAssist::sendPourInformation(UnitIndex idx, float pourVol)
{
    if (!myConfig.hasTargetMqtt())
        return;

    TemplatingEngine tpl;

    tpl.setVal("${mdns}", myConfig.getMDNS());
    tpl.setVal("${id}", myConfig.getID());
    tpl.setVal("${pour}", pourVol, 3);
    tpl.setVal("${tap}", static_cast<int>(idx) + 1);

    Log.notice(F("HA  : Sending POUR information to HA, pour %Fl [%d]." CR),
               pourVol, idx);

    const char *out = tpl.create(pourTemplate);
    EspSerial.print(out);
    EspSerial.print(CR);
    String outStr(out);
    _push->sendMqtt(outStr);
    tpl.freeMemory();
}
