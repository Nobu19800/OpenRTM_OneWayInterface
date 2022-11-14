#ifndef TESTINPORTPROVIDER_H
#define TESTINPORTPROVIDER_H

#include "idl/DataPortUDPSkel.h"
#include <rtm/BufferBase.h>
#include <rtm/InPortProvider.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/Manager.h>
#include <rtm/ConnectorListener.h>
#include <rtm/ConnectorBase.h>


class OneWayInPortProvider
    : public RTC::InPortProvider,
      public virtual POA_OpenRTM::InPortCdrUDP,
      public virtual PortableServer::RefCountServantBase
{
public:
  OneWayInPortProvider();
  ~OneWayInPortProvider() override;
  void init(coil::Properties& prop) override;
  void setBuffer(RTC::BufferBase<RTC::ByteData>* buffer) override;
  void setListener(RTC::ConnectorInfo& info,
                             RTC::ConnectorListenersBase* listeners) override;
  void setConnector(RTC::InPortConnector* connector) override;
  void put(const ::OpenRTM::CdrData& data) override;
private:
  inline void onReceived(RTC::ByteData& data)
  {
    m_listeners->notifyIn(RTC::ConnectorDataListenerType::ON_RECEIVED, m_profile, data);
  }
  inline void onReceiverError(RTC::ByteData& data)
  {
    m_listeners->notifyIn(RTC::ConnectorDataListenerType::ON_RECEIVER_ERROR, m_profile, data);
  }
private:
  RTC::CdrBufferBase* m_buffer{nullptr};
  ::OpenRTM::InPortCdrUDP_var m_objref;
  RTC::ConnectorListenersBase* m_listeners;
  RTC::ConnectorInfo m_profile;
  RTC::InPortConnector* m_connector{nullptr};
  RTC::ByteData m_cdr;
};

#endif