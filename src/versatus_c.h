#ifndef VERSATUS_C_H
#define VERSATUS_C_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <json-c/json.h>

// Define the ComputeTransaction structure
struct ComputeTransaction {
    char* recipient;
    uint64_t amount;
};

// Define the ComputeOutputs structure
struct ComputeOutputs {
    struct ComputeTransaction* transactions;
    size_t num_transactions;
};

// Define the ApplicationInputs structure
struct ApplicationInputs {
    const char* contract_fn;
    uint64_t amount;
    char** recipients;
    size_t num_recipients;
};

// Define the ProtocolInputs structure
struct ProtocolInputs {
    int32_t version;
    uint64_t block_height;
    uint64_t block_time;
};

// Define the AccountInfo structure
struct AccountInfo {
    const char* account_address;
    uint64_t account_balance;
};

// Define the ComputeInputs structure
struct ComputeInputs {
    int32_t version;
    struct AccountInfo account_info;
    struct ProtocolInputs protocol_input;
    struct ApplicationInputs application_input;
};

// Define a custom strdup function
char* custom_strdup(const char* src) {
    size_t len = strlen(src) + 1; // Include the null terminator
    char* dest = malloc(len);
    if (dest) {
        memcpy(dest, src, len);
    }
    return dest;
}

// Function to create output object containing proposed transactions
struct ComputeOutputs generate_transactions(struct ComputeInputs inputs) {
    struct ComputeOutputs outputs;
    size_t i;

    // Do contract stuff to generate proposed transactions
    outputs.num_transactions = inputs.application_input.num_recipients;
    outputs.transactions = malloc(outputs.num_transactions * sizeof(struct ComputeTransaction));

    uint64_t amount_each = inputs.application_input.amount / inputs.application_input.num_recipients;
    for (i = 0; i < outputs.num_transactions; i++) {
        outputs.transactions[i].recipient = custom_strdup(inputs.application_input.recipients[i]);
        outputs.transactions[i].amount = amount_each;
    }

    return outputs;
}

// Function to commit the smart contract results/transactions to stdout
void commit_results(struct ComputeOutputs outputs) {
    size_t i;

    for (i = 0; i < outputs.num_transactions; i++) {
        printf("Recipient: %s, Amount: %llu\n",
               outputs.transactions[i].recipient, outputs.transactions[i].amount);
        free(outputs.transactions[i].recipient);
    }

    free(outputs.transactions);
}

struct ComputeInputs gather_inputs() {
    // Create a JSON parser
    struct json_object *root;
    struct json_object *accountInfo;
    struct json_object *protocolInput;
    struct json_object *applicationInput;
    struct json_object *recipients_array;
    int32_t protocolVersion;
    const char *accountAddress, *contractFn, *recipient;
    uint64_t accountBalance, blockHeight, blockTime, amount;
    size_t num_recipients;

    // Read JSON input from stdin
    size_t buf_size = 1024; // Adjust buffer size as needed
    char *json_data = malloc(buf_size);
    size_t json_len = 0;
    int c;
    struct ComputeInputs inputs = { 0 };

    if (!json_data) {
        fprintf(stderr, "Memory allocation error\n");
        return inputs;
    }

    while ((c = getchar()) != EOF) {
        if (json_len + 1 >= buf_size) {
            buf_size *= 2;
            json_data = realloc(json_data, buf_size);
            if (!json_data) {
                fprintf(stderr, "Memory allocation error\n");
                return inputs;
            }
        }
        json_data[json_len++] = c;
    }

    // Null-terminate the JSON data
    json_data[json_len] = '\0';

    // Parse the JSON data
    root = json_tokener_parse(json_data);

    if (!root) {
        fprintf(stderr, "Error parsing JSON\n");
        free(json_data);
        return inputs;
    }

    // Access and process JSON values as needed
    struct json_object *version;
    json_object_object_get_ex(root, "version", &version);
    inputs.version = json_object_get_int(version);
    printf("Version: %d\n", inputs.version);

    json_object_object_get_ex(root, "accountInfo", &accountInfo);
    accountAddress = json_object_get_string(json_object_object_get(accountInfo, "accountAddress"));
    accountBalance = (uint64_t)json_object_get_int64(json_object_object_get(accountInfo, "accountBalance"));
    printf("Account Address: %s\n", accountAddress);
    printf("Account Balance: %llu\n", accountBalance);
    inputs.account_info.account_address = accountAddress;
    inputs.account_info.account_balance = accountBalance;

    json_object_object_get_ex(root, "protocolInput", &protocolInput);
    protocolVersion = json_object_get_int(json_object_object_get(protocolInput, "version"));
    blockHeight = (uint64_t)json_object_get_int64(json_object_object_get(protocolInput, "blockHeight"));
    blockTime = (uint64_t)json_object_get_int64(json_object_object_get(protocolInput, "blockTime"));
    printf("Protocol Version: %d\n", protocolVersion);
    printf("Block Height: %llu\n", blockHeight);
    printf("Block Time: %llu\n", blockTime);
    inputs.protocol_input.version = protocolVersion;
    inputs.protocol_input.block_height = blockHeight;
    inputs.protocol_input.block_time = blockTime;

    json_object_object_get_ex(root, "applicationInput", &applicationInput);
    contractFn = json_object_get_string(json_object_object_get(applicationInput, "contractFn"));
    amount = (uint64_t)json_object_get_int64(json_object_object_get(applicationInput, "amount"));
    printf("Contract Function: %s\n", contractFn);
    printf("Amount: %llu\n", amount);
    inputs.application_input.contract_fn = contractFn;
    inputs.application_input.amount = amount;

    json_object_object_get_ex(applicationInput, "recipients", &recipients_array);
    num_recipients = (size_t)json_object_array_length(recipients_array);
    inputs.application_input.num_recipients = num_recipients;

    // Allocate memory for the recipients array
    inputs.application_input.recipients = malloc(num_recipients * sizeof(char*));
    if (!inputs.application_input.recipients) {
        fprintf(stderr, "Memory allocation error\n");
        json_object_put(root);
        free(json_data);
        return inputs;
    }

    printf("Recipients:\n");
    for (size_t i = 0; i < num_recipients; i++) {
        recipient = json_object_get_string(json_object_array_get_idx(recipients_array, i));
        printf("- %s\n", recipient);
        // Allocate memory for the recipient string and copy the value
        inputs.application_input.recipients[i] = custom_strdup(recipient);
    }

    // Cleanup
    json_object_put(root);
    free(json_data);

    return inputs;
}

#endif