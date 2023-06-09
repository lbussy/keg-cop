#ifndef _HOMEASSIST_H
#define _HOMEASSIST_H

class HomeAssist
{
protected:
    // BasePush *_push; // TODO:  This is the MQTT push

public:
    // explicit HomeAssist(BasePush *push) { _push = push; }  // TODO:  This is the MQTT push

    void sendTempInformation(float tempC);
    void sendTapInformation(UnitIndex idx, float stableVol, float glasses);
    void sendPourInformation(UnitIndex idx, float pourVol);
};

#endif // _HOMEASSIST_H