#include "GpsTask.h"

bool GpsTaskRunner::datagramIsValid() 
{
  return _datagram.gps.altitude != 0 && _datagram.gps.longitude != 0 && _datagram.gps.latitude != 0;
}

void GpsTaskRunner::sendData()
{
  PREPARE_DATAGRAM(NowTransmittedDataTypes::GPS, _boardId)

  if (locationIsValid())
  {
    _datagram.gps.latitude = latitude();
    _datagram.gps.longitude = longitude();
  }

  if(altitudeIsValid())
    _datagram.gps.altitude = altitudeInMeters();

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

void task(void * parameter)
{
  TaskRunner = (GpsTaskRunner*)parameter;    
  Serial.println("Starting GPS monitor task");

  for( ;; )
  {
    TaskRunner->run();
    portYIELD();
    vTaskDelay(100);
  }
}