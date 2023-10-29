
#include "../src/versatus_c.h"


int main() {
  struct ComputeInputs inputs = gather_inputs();

  // Do contract stuff to generate proposed transactions
  struct ComputeOutputs outputs = generate_transactions(inputs);

  // Write the smart contract results/transactions to stdout
  commit_results(outputs);

  return 0;
}