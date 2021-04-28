#include "GpsTask.h"

bool GpsTaskRunner::datagramIsValid() 
{
  return _datagram.altitude != 0 && _datagram.longitude != 0 && _datagram.longitude != 0;
}

void GpsTaskRunner::sendData()
{
  PREPARE_DATAGRAM(NowTransmittedDataTypes::GPS, _boardId)

  if (locationIsValid())
  {
    _datagram.lattitude = lattitude();
    _datagram.longitude = longitude();
  }

  if(altitudeIsValid())
    _datagram.altitude = altitudeInMeters();

  Serial.println("Sending message");
  if(datagramIsValid() )
  { 
    SEND_MESSAGE_USING_ESP_NOW(local_broadcastAddress, _datagram) 
  }
    
  _previousMillis = millis(); 
}

void GpsTaskRunner::run()
{
    const ulong currentMillis = millis();

    while(Serial2.available() > 0)
    {
        if (!_gpsModule.encode(Serial2.read()) 
                || currentMillis - _previousMillis < _interval) continue;
                
        sendData(); 
        break;
    }
}

GpsTaskRunner* _runner;

void gpsTask(void * parameter)
{
    _runner = (GpsTaskRunner*)parameter;    
    Serial.println("Starting GPS monitor task");

    for( ;; )
    {
        _runner->run();
        portYIELD();
        vTaskDelay(100);
    }
}