#include "OneWayInPortConsumer.h"
#include "OneWayInPortProvider.h"
#include <rtm/Manager.h>

static const char* const onewayifpush_option[] =
{
  ""
};

static const char* const onewayifpull_option[] =
{
  ""
};


extern "C"
{
  DLL_EXPORT void OneWayInterfaceInit(RTC::Manager* manager)
  {
     {
       coil::Properties prop(onewayifpush_option);
       RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
       factory.addFactory("corba_onewayif_cdr",
         ::coil::Creator< ::RTC::InPortProvider,
         OneWayInPortProvider>,
         ::coil::Destructor< ::RTC::InPortProvider,
         OneWayInPortProvider>
         );//,prop);
     }

     {
       coil::Properties prop(onewayifpull_option);
       RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
       factory.addFactory("corba_onewayif_cdr",
         ::coil::Creator< ::RTC::InPortConsumer,
         OneWayInPortConsumer>,
         ::coil::Destructor< ::RTC::InPortConsumer,
         OneWayInPortConsumer>
         );//,prop);
     }

  }
}