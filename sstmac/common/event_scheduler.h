/*
 *  This file is part of SST/macroscale:
 *               The macroscale architecture simulator from the SST suite.
 *  Copyright (c) 2009 Sandia Corporation.
 *  This software is distributed under the BSD License.
 *  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
 *  the U.S. Government retains certain rights in this software.
 *  For more information, see the LICENSE file in the top
 *  SST/macroscale directory.
 */

#ifndef SSTMAC_COMMON_event_scheduler_H_INCLUDED
#define SSTMAC_COMMON_event_scheduler_H_INCLUDED

#include <sstmac/common/timestamp.h>
#include <sstmac/common/stats/location_trace.h>
#include <sstmac/common/event_handler.h>
#include <sstmac/common/event_callback.h>
#include <sstmac/common/sst_event_fwd.h>
#include <sstmac/common/sstmac_config.h>
#include <sstmac/common/event_manager_fwd.h>
#include <sprockit/sim_parameters_fwd.h>

#if SSTMAC_INTEGRATED_SST_CORE
#include <sst/core/params.h>
#include <sst/core/link.h>
#include <sstmac/sst_core/integrated_component.h>

namespace sstmac {
typedef SST::Event::HandlerBase link_handler;
}
#else
namespace sstmac {
typedef event_handler link_handler;
}
#include <sstmac/common/event_manager.h>
#endif

namespace sstmac {

class event_scheduler :
  public locatable,
  public sprockit::printable
{
  friend class event_subcomponent;
  friend class event_component;
 public:
  /**
   * Add an event to the event queue, where msg will get delivered to handler at time t.
   * @param t Time at which the event should happen
   * @param handler The handler for the event
   * @param msg The message to deliver to the handler
   */
  void
  schedule(timestamp t,
           event_handler* handler,
           event* ev);

  void
  schedule(timestamp t, event_queue_entry* ev);

  void
  schedule_now(event_queue_entry* ev);

  void
  schedule_now(event_handler* handler, event* ev);

  void
  schedule_delay(timestamp delay,
                 event_handler* handler,
                 event* ev);
  void
  schedule_delay(timestamp delay, event_queue_entry* ev);

  void
  send_self_event_queue(timestamp arrival, event_queue_entry* ev);

  void
  send_delayed_self_event_queue(timestamp delay, event_queue_entry* ev);

  void
  send_now_self_event_queue(event_queue_entry* ev);

  /**
   * @brief send_to_link  The message should arrive now
   * @param lnk
   * @param ev
   */
  void
  send_to_link(event_handler* lnk, event* ev);

  /**
   * @brief send_to_link  The arrival time will be enter + lat
   * @param enter        The time the enters the link
   * @param lat
   * @param lnk
   * @param ev
   */
  void
  send_to_link(timestamp enter, timestamp lat,
               event_handler* lnk, event* ev);

  void
  send_delayed_to_link(timestamp extra_delay, timestamp lat,
               event_handler* lnk, event* ev);


  void
  send_delayed_to_link(timestamp extra_delay,
               event_handler* lnk, event* ev);

  void
  register_stat(stat_collector* coll, stat_descr_t* descr);

#if SSTMAC_INTEGRATED_SST_CORE
 public:
  timestamp
  now() const;

  SST::Link*
  self_link() const {
    return self_link_;
  }

  SST::Component*
  comp() const {
    return comp_;
  }

  void
  handle_self_event(SST::Event* ev);

 protected:
  event_scheduler(device_id loc) :
   self_link_(nullptr),
   comp_(nullptr),
   locatable(loc, locatable::null_threadid)
  {
  }

  void init_self_link(SST::Component* comp);

  void init_self_link(SST::Component* comp, SST::Link* self_link){
    comp_ = comp;
    self_link_ = self_link;
  }

  static SST::TimeConverter* time_converter_;

 private:
  SST::SimTime_t
  extra_delay(timestamp t) const;

  void
  schedule(SST::SimTime_t delay, event_handler* handler, event* ev);

  SST::Link* self_link_;

  SST::Component* comp_;
#else
 public:
  timestamp
  now() const {
    return eventman_->now();
  }

  int
  nthread() const {
    return eventman_->nthread();
  }

  event_manager*
  event_mgr() const {
    return eventman_;
  }


  /**
   * @brief ipc_schedule Should only be called on stub handlers for which handler->ipc_handler() returns true
   * @param t         The time the event will run at
   * @param handler   The handler to receive the event. This should always be a stub for a real handler on a remote process.
   * @param ev        The event to deliver
   */
  void
  ipc_schedule(timestamp t,
    event_handler* handler,
    event* ev);

 protected:
  event_scheduler(event_manager* mgr, uint32_t* seqnum, device_id loc, int thread_id) :
   locatable(loc, thread_id),
    eventman_(mgr),
   seqnum_(seqnum)
  {
  }

 private:
  event_manager* eventman_;
  uint32_t* seqnum_;

 private:
  void sanity_check(timestamp t);

  void
  multithread_schedule(int src_thread, int dst_thread,
    timestamp t, event_queue_entry* ev);
#endif

};

/**
 * The interface for something that can schedule messages
 */
class event_component :
  public event_scheduler
#if SSTMAC_INTEGRATED_SST_CORE
  , public SSTIntegratedComponent
#endif
{
  friend class event_subcomponent;
 public:
  virtual
  ~event_component() {
  }

  void
  cancel_all_messages();

  virtual void
  deadlock_check() {}

  virtual void
  deadlock_check(event* ev) {}

  virtual void setup(); //needed for SST core compatibility

  virtual void init(unsigned int phase); //needed for SST core compatibility

#if SSTMAC_INTEGRATED_SST_CORE
 protected:
  event_component(sprockit::sim_parameters* params,
                 uint64_t cid,
                 device_id id,
                 event_manager* mgr);
#else
 protected:
  event_component(sprockit::sim_parameters* params,
                uint64_t cid,
                device_id id,
                event_manager* mgr) :
   event_scheduler(mgr, &seqnum_, id, mgr ? mgr->thread_id() : locatable::null_threadid),
   seqnum_(0)
  {
  }

 private:
  uint32_t seqnum_;
#endif
};

class event_subcomponent :
  public event_scheduler
{

 public:
  virtual void setup(); //needed for SST core compatibility

  virtual void init(unsigned int phase); //needed for SST core compatibility

  virtual void
  deadlock_check() {}

  virtual void
  deadlock_check(event* ev) {}

#if SSTMAC_INTEGRATED_SST_CORE
 public:
  event_subcomponent(event_scheduler* parent);
#else
 protected:
  event_subcomponent(event_scheduler* parent) :
   event_scheduler(parent->event_mgr(), parent->seqnum_, parent->event_location(), parent->thread_id())
  {
  }
#endif
};


} // end of namespace sstmac
#endif


