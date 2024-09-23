
#include <google/protobuf/text_format.h>

#include <iostream>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "app/algorithms/algorithm_impl.h"
#include "app/app_io.pb.h"
#include "utils/random.h"

// command
ABSL_FLAG(std::string, command_textproto, "help", "The command to run in textproto format.");
ABSL_FLAG(int, seed, 0, "The command to run in textproto format.");

namespace {
using rpo::app::app_io::Result;
}  // namespace

int main(int argc, char** argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  absl::ParseCommandLine(argc, argv);
  int seed = absl::GetFlag(FLAGS_seed);
  if (seed == 0) {
    seed = (int)time(NULL);
  }
  rpo::utils::Randomize::instance().setSeed(seed);
  srand(seed);
  rpo::app::app_io::Command command;
  if (google::protobuf::TextFormat::ParseFromString(absl::GetFlag(FLAGS_command_textproto),
                                                    &command)) {
    if (command.command() == "run") {
      auto result = rpo::app::algorithms::Run(command.hypergraph(), command.config(), true);

      result->PrintDebugString();
    }
  } else {
    command.set_command("help");
    std::string command_s;
    command.PrintDebugString();
    std::cerr << "Parsing failed." << std::endl;
    return 1;
  }
  return 0;
}