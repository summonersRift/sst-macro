#ifndef distributed_service_h
#define distributed_service_h

#include <sstmac/software/process/app.h>
#include <sstmac/software/libraries/service.h>
#include <sstmac/libraries/sumi/sumi_transport.h>
#include <sumi/transport_fwd.h>
#include <sumi/message_fwd.h>

namespace sstmac {

class distributed_service :
 public sumi_transport
{
 public:
  distributed_service(sprockit::sim_parameters* params,
                      const std::string& libname,
                      sw::software_id sid,
                      sw::operating_system* os) :
    sumi_transport(params, libname, sid, os)
  {
  }

  virtual ~distributed_service(){}

  virtual void run() = 0;

 protected:
  sumi::message_ptr busy_loop();

};

DeclareFactory(distributed_service, const std::string&, sw::software_id, sw::operating_system*);

class distributed_service_app :
  public sstmac::sw::app
{
 public:
  distributed_service_app(sprockit::sim_parameters* params,
                      sw::software_id sid,
                      sw::operating_system* os);

  void skeleton_main() override;

 private:
  std::string libname_;

};

}


#endif

