#ifndef _GpsTask_h_
#define _GpsTask_h_

#include <NowCommon.h>
#include <TaskRunner.h>
#include <TinyGPS++.h>

class GpsTaskRunner
{
private:
    static const uint32_t GPSBaud = 9600;
    String _serverMacAddress;
    char _boardId[16];
    uint _taskPriority;
    esp_datagram _datagram;
    const ulong _interval = 5000;
    unsigned long _previousMillis     = 0; // Stores last time temperature was published
    TinyGPSPlus _gpsModule;
    bool altitudeIsValid() { return _gpsModule.altitude.isValid(); }
    bool datagramIsValid();
    bool locationIsValid() { return _gpsModule.location.isValid(); }

public:
    GpsTaskRunner(const char * board_id, String server_mac, uint task_priority) 
    { 
        _serverMacAddress = server_mac;
        _taskPriority = task_priority;
        memcpy(&_boardId, board_id, 32); 
    }

    void begin() 
    { 
        Serial2.begin(GPSBaud); 
        xTaskCreatePinnedToCore(task, "GPS_TSK", 10000, this, _taskPriority, NULL, 1);
    }
    double altitudeInMeters() { return _gpsModule.altitude.meters(); }
    double latitude() { return _gpsModule.location.lat(); }
    double longitude() { return _gpsModule.location.lng(); }
    void run();
    void sendData();
};

extern GpsTaskRunner * TaskRunner;

#endif