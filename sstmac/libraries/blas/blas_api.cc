#include <sstmac/libraries/blas/blas_api.h>
#include <sstmac/software/process/operating_system.h>
#include <sstmac/software/libraries/compute/lib_compute_inst.h>
#include <sstmac/software/libraries/compute/compute_event.h>
#include <sprockit/keyword_registration.h>

RegisterKeywords(
  "dgemm",
  "dgemv",
  "daxpy",
  "ddot",
  "daxpy_loop_unroll",
  "daxpy_pipeline_efficiency",
  "ddot_loop_unroll",
  "ddot_pipeline_efficiency",
  "dgemm_loop_unroll",
  "dgemm_pipeline_efficiency",
  "dgemm_cache_size",
  "dgemv_loop_unroll",
  "dgemv_pipeline_efficiency",
);

#define enumcase(x) case x: return #x;

RegisterDebugSlot(blas);

namespace sstmac {
namespace sw {

blas_kernel* blas_api::dgemm_kernel_;
blas_kernel* blas_api::dgemv_kernel_;
blas_kernel* blas_api::daxpy_kernel_;
blas_kernel* blas_api::ddot_kernel_;

RegisterAPI("blas", blas_api);

blas_api::blas_api(sprockit::sim_parameters* params,
                   software_id sid,
                   operating_system* os)
  : api(params, "blas", sid, os, key::general)
{
  std::string libname = sprockit::printf("blas-compute%d", sid.to_string().c_str());
  lib_compute_ = new lib_compute_inst(params, libname, sid, os);
  if (!dgemm_kernel_){
    init_kernels(params);
  }
}

blas_api::~blas_api()
{
}

void
blas_api::init_kernels(sprockit::sim_parameters* params)
{
  dgemm_kernel_ = blas_kernel_factory::get_optional_param("dgemm", "default_dgemm", params);
  dgemv_kernel_ = blas_kernel_factory::get_optional_param("dgemv", "default_dgemv", params);
  daxpy_kernel_ = blas_kernel_factory::get_optional_param("daxpy", "default_daxpy", params);
  ddot_kernel_ = blas_kernel_factory::get_optional_param("ddot", "default_ddot", params);
}

void
blas_api::ddot(int n)
{
  compute_event* msg = ddot_kernel_->op_1d(n);
  lib_compute_->compute_inst(msg);
  //msg is done
  delete msg;
}

void
blas_api::dgemm(int m, int n, int k)
{
  compute_event* msg = dgemm_kernel_->op_3d(m, n, k);
  lib_compute_->compute_inst(msg);
  delete msg;
}

void
blas_api::dgemv(int m, int n)
{
  compute_event* msg = dgemv_kernel_->op_2d(m,n);
  lib_compute_->compute_inst(msg);
  delete msg;
}

void
blas_api::daxpy(int n)
{
  compute_event* msg = daxpy_kernel_->op_1d(n);
  lib_compute_->compute_inst(msg);
  delete msg;
}

compute_event*
blas_kernel::op_3d(int m, int k, int n)
{
  spkt_throw_printf(sprockit::unimplemented_error,
    "blas_kernel::mult_op: %s does not implement 3D ops",
    to_string().c_str());
}

compute_event*
blas_kernel::op_2d(int m, int n)
{
  spkt_throw_printf(sprockit::unimplemented_error,
    "blas_kernel::mult_op: %s does not implement 2D ops",
    to_string().c_str());
}

compute_event*
blas_kernel::op_1d(int n)
{
  spkt_throw_printf(sprockit::unimplemented_error,
    "blas_kernel::mult_op: %s does not implement 1D ops",
    to_string().c_str());
}


}
}

