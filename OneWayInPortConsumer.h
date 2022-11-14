#ifndef ONEWAYINPORTCONSUMER_H
#define ONEWAYINPORTCONSUMER_H

#include "idl/DataPortUDPSkel.h"
#include <rtm/CorbaConsumer.h>
#include <rtm/InPortConsumer.h>
#include <rtm/Manager.h>

class OneWayInPortConsumer
    : public RTC::InPortConsumer,
      public RTC::CorbaConsumer< ::OpenRTM::InPortCdrUDP >
{
public:
  OneWayInPortConsumer();
  ~OneWayInPortConsumer() override;
  void init(coil::Properties& prop) override;
  RTC::DataPortStatus put(RTC::ByteData& data) override;
  void publishInterfaceProfile(SDOPackage::NVList& properties) override;
  bool subscribeInterface(const SDOPackage::NVList& properties) override;
  void unsubscribeInterface(const SDOPackage::NVList& properties) override;
private:
  bool subscribeFromIor(const SDOPackage::NVList& properties);
  bool subscribeFromRef(const SDOPackage::NVList& properties);
  bool unsubscribeFromIor(const SDOPackage::NVList& properties);
  bool unsubscribeFromRef(const SDOPackage::NVList& properties);
private:
  mutable RTC::Logger rtclog;
  coil::Properties m_properties;
  ::OpenRTM::CdrData m_data;
};

#endif