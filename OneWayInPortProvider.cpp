#include "OneWayInPortProvider.h"

OneWayInPortProvider::OneWayInPortProvider()
{
  // PortProfile setting
  setInterfaceType("corba_onewayif_cdr");

  // ConnectorProfile setting

#ifdef ORB_IS_OMNIORB
  PortableServer::ObjectId_var oid = ::RTC::Manager::instance().theShortCutPOA()->activate_object(this);
#endif
  m_objref = this->_this();

  // set InPort's reference
  CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
  CORBA::String_var ior = orb->object_to_string(m_objref.in());
  CORBA_SeqUtil::
    push_back(m_properties,
              NVUtil::newNV("dataport.corba_onewayif_cdr.inport_ior", ior.in()));
  CORBA_SeqUtil::
    push_back(m_properties,
              NVUtil::newNV("dataport.corba_onewayif_cdr.inport_ref", m_objref));
}

OneWayInPortProvider::~OneWayInPortProvider()
{
  try
    {
      PortableServer::ObjectId_var oid;
#ifdef ORB_IS_OMNIORB
      oid = ::RTC::Manager::instance().theShortCutPOA()->servant_to_id(this);
      ::RTC::Manager::instance().theShortCutPOA()->deactivate_object(oid);
#else
      oid = _default_POA()->servant_to_id(this);
      _default_POA()->deactivate_object(oid);
#endif
    }
  catch (PortableServer::POA::ServantNotActive &e)
    {
#ifdef ORB_IS_ORBEXPRESS
      oe_out << e << oe_endl << oe_flush;
#else
      RTC_ERROR(("%s", e._name()));
#endif
    }
  catch (PortableServer::POA::WrongPolicy &e)
    {
#ifdef ORB_IS_ORBEXPRESS
      oe_out << e << oe_endl << oe_flush;
#else
      RTC_ERROR(("%s", e._name()));
#endif
    }
  catch (...)
    {
      // never throws exception
      RTC_ERROR(("Unknown exception caught."));
    }
}

void OneWayInPortProvider::init(coil::Properties& prop)
{
}

void OneWayInPortProvider::setBuffer(RTC::BufferBase<RTC::ByteData>* buffer)
{
}

void OneWayInPortProvider::setListener(RTC::ConnectorInfo& info,
  RTC::ConnectorListenersBase* listeners)
{
  m_profile = info;
  m_listeners = listeners;
}

void OneWayInPortProvider::setConnector(RTC::InPortConnector* connector)
{
  m_connector = connector;
}



void OneWayInPortProvider::put(const ::OpenRTM::CdrData& data)
{
  RTC_PARANOID(("InPortCorbaCdrProvider::put()"));

  if (m_connector == nullptr)
    {
#ifndef ORB_IS_RTORB
      m_cdr.writeData(const_cast<unsigned char*>(data.get_buffer()), static_cast<CORBA::ULong>(data.length()));
#else
      m_cdr.writeData(reinterpret_cast<unsigned char*>(&data[0]), static_cast<CORBA::ULong>(data.length()));
#endif

      onReceiverError(m_cdr);
      return;
    }

  RTC_PARANOID(("received data size: %d", data.length()));
  
  // set endian type
  bool endian_type = m_connector->isLittleEndian();
  RTC_TRACE(("connector endian: %s", endian_type ? "little":"big"));
  //m_cdr = ByteData();
  m_cdr.isLittleEndian(endian_type);
#ifndef ORB_IS_RTORB
  //m_cdr.writeData(const_cast<unsigned char*>(data.get_buffer()), static_cast<CORBA::ULong>(data.length()));
  m_cdr.copyToAddress(const_cast<unsigned char*>(data.get_buffer()), static_cast<CORBA::ULong>(data.length()));
#else
  m_cdr.writeData(reinterpret_cast<unsigned char*>(&data[0]), static_cast<CORBA::ULong>(data.length()));
#endif
  RTC_PARANOID(("converted CDR data size: %d", m_cdr.getDataLength()));

  onReceived(m_cdr);
  m_connector->write(m_cdr);

  return;
}