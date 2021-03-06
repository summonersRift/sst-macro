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

#include <sumi-mpi/mpi_comm/mpi_comm.h>
#include <sumi-mpi/mpi_comm/keyval.h>
#include <sprockit/debug.h>
#include <sprockit/errors.h>
#include <sprockit/statics.h>

static sprockit::need_delete_statics<sumi::mpi_comm> cleanup_comm;

namespace sumi {

//
// NULL communicator.
//
mpi_comm* mpi_comm::comm_null = nullptr;

mpi_comm::mpi_comm() :
  group_(nullptr),
  next_collective_tag_(0),
  id_(MPI_COMM_NULL),
  rank_(-1),
  del_grp_(false),
  communicator(-1),
  topotype_(TOPO_NONE)
{
}

mpi_comm::~mpi_comm()
{
  if (del_grp_) delete group_;
}

mpi_comm::mpi_comm(
  MPI_Comm id, //const appid &aid,
  int rank, mpi_group* peers,
  app_id aid,
  bool del_grp) :
  sumi::communicator(rank),
  group_(peers),
  next_collective_tag_(MPI_COMM_WORLD + 100),
  aid_(aid),
  id_(id),
  rank_(rank),
  del_grp_(del_grp),
  topotype_(TOPO_NONE)
{
  if (peers->size() == 0) {
    spkt_throw_printf(sprockit::value_error,
         "trying to build communicator of size 0");
  }

  if (!comm_null) {
    comm_null = new mpi_comm;
  }
}

void
mpi_comm::delete_statics()
{
  if (comm_null) delete comm_null;
}

int
mpi_comm::global_to_comm_rank(int global_rank) const
{
  spkt_throw(sprockit::unimplemented_error,
    "mpi_comm::global_to_comm_rank");
}

void
mpi_comm::dup_keyvals(mpi_comm* m)
{
  spkt_unordered_map<int, keyval*>::iterator it, end = m->keyvals_.end();
  for (it = m->keyvals_.begin(); it != end; it++) {
    spkt_throw(sprockit::unimplemented_error,
      "dup_keyvals");
    //keyval* c = (it->second)->clone(keyval::get_new_key());
    //keyvals_[c->key()] = c;
  }
}

std::string
mpi_comm::to_string() const
{
  return sprockit::printf("mpicomm(id=%d,size=%d,rank=%d)", id_, size(), rank_);
}

/// The size of the communicator.
int
mpi_comm::size() const
{
  if (id_ == MPI_COMM_NULL) {
    spkt_throw(sprockit::value_error,
              "mpicomm: trying to call size() on a null mpicomm");
  }
  if (!group_) {
    spkt_throw(sprockit::null_error,
              "mpicomm: peers_ is null for some reason in size()");
  }
  return group_->size();
}

void
mpi_comm::set_keyval(keyval* k, void* val)
{
  keyvals_[k->key()] = k;
  k->set_val(val);
}

void
mpi_comm::get_keyval(keyval* k, void* val, int* flag)
{

  if (keyvals_.find(k->key()) == keyvals_.end()) {
    *flag = 1;
  }
  else {
    *flag = 0;
  }

  void** vcast = (void**)val;

  *vcast = k->val();

  //memcpy(val, (k->val()), sizeof(void*));

  //val = (k->val());
}

//
// Get a unique tag for a collective operation.
//
int
mpi_comm::next_collective_tag()
{
  uint16_t id = id_;
  int next_tag = (id << 16) | next_collective_tag_;
  next_collective_tag_++;
  return next_tag;
}

/// The task index of the caller.
task_id
mpi_comm::my_task() const
{
  return group_->at(rank_);
}

/// The task index of the given peer.
task_id
mpi_comm::peer_task(int rank) const
{
  return group_->at(rank);
}

//
// Fairly self-explanatory.
//
std::ostream&
operator<<(std::ostream &os, mpi_comm* comm)
{
  os << "mpicomm(rank=" << comm->rank_ << ", id=" << comm->id_ << ")";
  return os;
}

}

