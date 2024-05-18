#include "../src/model/environment/wsf.hpp"

void testmain() { 
	wsfplugin::WSFEnvironment wangshuaibo;
    wangshuaibo.init("D:\\codeproject\\perlinnoise\\test1.tif", 109.90997314453125, 36.14501953125);
    double elevation = wangshuaibo.getAltitude({0, 0, 0});
    double elevation1 = wangshuaibo.getAltitude({0, 55000, 0});
    double elevation2 = wangshuaibo.getAltitude({-55000, 55000, 0});
    double elevation3 = wangshuaibo.getAltitude({-55000, 0, 0});
}