#include <chrono>
#include <thread>
#include <vector>

#include "../src/SCAMP.h"
#include "../src/common.h"
#include "scamp_interface.h"
#include "utils.h"

// Worker will act as a client to the SCAMPserverWorkers, request a job and wait
// for its completion
void SCAMPInterface::do_SCAMP_distributed(SCAMPProto::SCAMPArgs *args) {
  SCAMPProto::SCAMPStatus status;
  grpc::Status s;
  {
    grpc::ClientContext context;
    s = stub_->IssueNewJob(&context, *args, &status);
  }
  if (!s.ok()) {
    std::cout << "Error issuing SCAMP Job" << std::endl;
    return;
  } else {
    std::cout << "Issued Job: " << status.job_id() << std::endl;
  }
  SCAMPProto::SCAMPJobID id;
  id.set_job_id(status.job_id());
  // id.set_job_id(0);
  bool done = false;
  while (!done) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Checking Job Status. " << std::endl;
    {
      grpc::ClientContext context;
      s = stub_->CheckJobStatus(&context, id, &status);
    }
    if (!s.ok()) {
      std::cout << "Error Checking Job Status. " << std::endl;
      continue;
    } else {
      std::cout << "Checked Job Status, got: " << status.status() << std::endl;
    }
    if (status.status() == SCAMPProto::JOB_STATUS_FINISHED) {
      done = true;
    }
  }
  SCAMPProto::SCAMPWork result;
  {
    grpc::ClientContext context;
    s = stub_->FetchJobResult(&context, id, &result);
  }
  if (!s.ok()) {
    std::cout << "Error fetching job Result" << std::endl;
    return;
  } else {
    std::cout << "Fetched Job Result" << std::endl;
  }
  *args = result.args();
}

int do_SCAMP_distributed(SCAMP::SCAMPArgs *args, std::string hostname_port) {
  SCAMPProto::SCAMPArgs proto_args = ConvertArgsToReply(*args);

  grpc::ChannelArguments ch_args;

  // Do not limit input size
  ch_args.SetMaxReceiveMessageSize(-1);

  SCAMPInterface worker(grpc::CreateCustomChannel(
      hostname_port, grpc::InsecureChannelCredentials(), ch_args));

  worker.do_SCAMP_distributed(&proto_args);

  ConvertProtoArgsToSCAMPArgs(proto_args, args);
}