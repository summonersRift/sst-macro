#ifndef CLOCK_CYCLE_EVENT_CONTAINER_H
#define CLOCK_CYCLE_EVENT_CONTAINER_H

#include <sstmac/common/sstmac_config.h>
#if !SSTMAC_INTEGRATED_SST_CORE

#include <sstmac/backends/native/event_map.h>
#include <sstmac/hardware/interconnect/interconnect_fwd.h>
#include <sstmac/backends/common/parallel_runtime.h>

DeclareDebugSlot(event_manager_time_vote);

namespace sstmac {
namespace native {

enum class vote_type_t {
  max,
  min
};

class clock_cycle_event_map :
  public event_map
{
 public:
  clock_cycle_event_map(sprockit::sim_parameters* params, parallel_runtime* rt);

  virtual ~clock_cycle_event_map() throw() {}

  virtual void
  run();

  bool
  vote_to_terminate();

  virtual void
  set_interconnect(hw::interconnect* interconn);

  virtual void
  ipc_schedule(timestamp t,
    device_id dst,
    device_id src,
    uint32_t seqnum,
    event* ev);

 protected:
  friend class multithreaded_event_container;

  virtual void
  schedule_incoming(const std::vector<void*>& mpi_buffers);

  void
  do_next_event();

  timestamp
  next_event_time() const;

  virtual timestamp
  vote_next_round(timestamp my_time, vote_type_t ty);

  int64_t
  do_vote(int64_t time, vote_type_t ty = vote_type_t::min);

  virtual void
  receive_incoming_events();

 protected:
  timestamp next_time_horizon_;
  timestamp lookahead_;
  timestamp no_events_left_time_;
  std::vector<void*> all_incoming_;
  std::vector<std::vector<void*> > thread_incoming_;
  hw::interconnect* interconn_;
  int epoch_;

#if SSTMAC_DEBUG_THREAD_EVENTS
  void open_debug_file();

  void close_debug_file();

  std::ofstream event_file_;
#endif

};

}
}

#endif // !SSTMAC_INTEGRATED_SST_CORE

#endif // CLOCK_CYCLE_EVENT_CONTAINER_H
