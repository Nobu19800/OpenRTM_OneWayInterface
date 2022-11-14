#include "OneWayInPortConsumer.h"
#include <rtm/NVUtil.h>

OneWayInPortConsumer::OneWayInPortConsumer() : rtclog("OneWayInPortConsumer")
{
}

OneWayInPortConsumer::~OneWayInPortConsumer()
{
  RTC_PARANOID(("~OneWayInPortConsumer()"));
}

void OneWayInPortConsumer::init(coil::Properties& prop)
{
  m_properties = prop;
}

RTC::DataPortStatus OneWayInPortConsumer::put(RTC::ByteData& data)
{
    RTC_PARANOID(("put()"));
    CORBA::ULong len = static_cast<CORBA::ULong>(data.getDataLength());
#ifndef ORB_IS_RTORB
    OpenRTM::CdrData out(len, len, data.getBuffer(), false);
#else // ORB_IS_RTORB
    data.readData(reinterpret_cast<unsigned char*>(&m_data[0]), len);
#endif  // ORB_IS_RTORB
    try
      {
        _ptr()->put(out);
        return RTC::DataPortStatus::PORT_OK;
      }
#ifdef ORB_IS_OMNIORB
    catch (const CORBA::COMM_FAILURE& ex)
      {
        if (ex.minor() == omni::COMM_FAILURE_WaitingForReply)
          {
            RTC_DEBUG(("Retry put message"));
            try
              {
                _ptr()->put(out);
                return RTC::DataPortStatus::PORT_OK;
              }
            catch (...)
              {
                return RTC::DataPortStatus::CONNECTION_LOST;
              }
          }
        else
          {
            return RTC::DataPortStatus::CONNECTION_LOST;
          }
      }
#endif
    catch (...)
      {
        return RTC::DataPortStatus::CONNECTION_LOST;
      }
}

void OneWayInPortConsumer::publishInterfaceProfile(SDOPackage::NVList& properties)
{
}

bool OneWayInPortConsumer::subscribeInterface(const SDOPackage::NVList& properties)
{
  RTC_TRACE(("subscribeInterface()"));
  RTC_DEBUG_STR((NVUtil::toString(properties)));

  // getting InPort's ref from IOR string
  if (subscribeFromIor(properties)) { return true; }

  // getting InPort's ref from Object reference
  if (subscribeFromRef(properties)) { return true; }

  return false;
}

void OneWayInPortConsumer::unsubscribeInterface(const SDOPackage::NVList& properties)
{
  RTC_TRACE(("unsubscribeInterface()"));
  RTC_DEBUG_STR((NVUtil::toString(properties)));

  if (unsubscribeFromIor(properties)) { return; }
  unsubscribeFromRef(properties);
}

bool OneWayInPortConsumer::subscribeFromIor(const SDOPackage::NVList& properties)
{
  RTC_TRACE(("subscribeFromIor()"));

  CORBA::Long index;
  index = NVUtil::find_index(properties,
                             "dataport.corba_onewayif_cdr.inport_ior");
  if (index < 0)
    {
      RTC_ERROR(("inport_ior not found"));
      return false;
    }

  const char* ior(nullptr);
  if (!(properties[index].value >>= ior))
    {
      RTC_ERROR(("inport_ior has no string"));
      return false;
    }

  CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
  CORBA::Object_var obj = orb->string_to_object(ior);

  if (CORBA::is_nil(obj))
    {
      RTC_ERROR(("invalid IOR string has been passed"));
      return false;
    }

  if (!setObject(obj.in()))
    {
      RTC_WARN(("Setting object to consumer failed."));
      return false;
    }
  return true;
}

bool OneWayInPortConsumer::subscribeFromRef(const SDOPackage::NVList& properties)
{
  RTC_TRACE(("subscribeFromRef()"));
  CORBA::Long index;
  index = NVUtil::find_index(properties,
                             "dataport.corba_onewayif_cdr.inport_ref");
  if (index < 0)
    {
      RTC_ERROR(("inport_ref not found"));
      return false;
    }

  CORBA::Object_var obj = CORBA::Object::_nil();
#ifdef ORB_IS_ORBEXPRESS
  if (!(properties[index].value >>= obj))
#else
  if (!(properties[index].value >>= CORBA::Any::to_object(obj.out())))
#endif
    {
      RTC_ERROR(("prop[inport_ref] is not objref"));
      return true;
    }

  if (CORBA::is_nil(obj))
    {
      RTC_ERROR(("prop[inport_ref] is not objref"));
      return false;
    }

  if (!setObject(obj.in()))
    {
      RTC_ERROR(("Setting object to consumer failed."));
      return false;
    }
  return true;
}

bool OneWayInPortConsumer::unsubscribeFromIor(const SDOPackage::NVList& properties)
{
  RTC_TRACE(("unsubscribeFromIor()"));
  CORBA::Long index;
  index = NVUtil::find_index(properties,
                             "dataport.corba_onewayif_cdr.inport_ior");
  if (index < 0)
    {
      RTC_ERROR(("inport_ior not found"));
      return false;
    }

  const char* ior = nullptr;
  if (!(properties[index].value >>= ior))
    {
      RTC_ERROR(("prop[inport_ior] is not string"));
      return false;
    }

  CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
  CORBA::Object_var var = orb->string_to_object(ior);
  if (!(_ptr()->_is_equivalent(var)))
    {
      RTC_ERROR(("connector property inconsistency"));
      return false;
    }

  releaseObject();
  return true;
}

bool OneWayInPortConsumer::unsubscribeFromRef(const SDOPackage::NVList& properties)
{
  RTC_TRACE(("unsubscribeFromRef()"));
  CORBA::Long index;
  index = NVUtil::find_index(properties,
                             "dataport.corba_onewayif_cdr.inport_ref");
  if (index < 0) { return false; }

  CORBA::Object_var obj = CORBA::Object::_nil();
#ifdef ORB_IS_ORBEXPRESS
  if (!(properties[index].value >>= obj)) 
#else
  if (!(properties[index].value >>= CORBA::Any::to_object(obj.out()))) 
#endif
    {
      return false;
    }

  if (!(_ptr()->_is_equivalent(obj.in()))) { return false; }

  releaseObject();
  return true;
}