#include "DataStore.h"

namespace externModel::track{

void DataStore::init() {
	cureuler_ = { 0.0,0.0,0.0 };
	curdir_ = { 0.0,0.0,0.0 };
	curpos_ = { 0.0,0.0,0.0 };
	curvel_ = { 0.0,0.0,0.0 };
	curspeed_ = 0.0;
	muroll = 0.085;
	muslide = 0.3;
	g = 9.8;
	AngularVelocity = { 0.0,0.0,0.0 };
	//T90��ս̹��
	tracked_.BodyLength = 6.86;//m
	tracked_.height = 2.23;//m
	tracked_.length = 9.53;//m
	tracked_.mass = 50000;//kg
	tracked_.MaxDistance = 650;//km
	tracked_.MaxSpeed = 65;//km/h
	tracked_.max_trench = 2.8;//m
	tracked_.max_vertical_angle = pi / 3;//rad
	tracked_.max_vertical_wall = 0.85;//m
	tracked_.power = 735498.75;//w
	tracked_.maxpower = 735498.75;//w
	tracked_.width = 3.37;//m
}

}