#include "carmodel.h"

void CarModel::tick(double dt){
    hullsys.tick(dt, baseCoordinate, hull, expDirection, expSpeed, params);
    sensorsys.tick(dt, scannedMemory, sensor, damage, baseCoordinate, hull);
};