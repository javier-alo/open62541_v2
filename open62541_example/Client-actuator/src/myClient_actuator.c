#include "open62541.h"
#include <stdio.h>
#include <unistd.h> // Include this for usleep

#define NETWORK_CYCLE_TIME 1000 // Network cycle time in milliseconds

int main(void) {
    // Create a new instance of an OPC UA client
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    // Connect to the OPC UA server
    UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:4840");
    if (retval != UA_STATUSCODE_GOOD) {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "The connection failed with status code %s",
                    UA_StatusCode_name(retval));
        UA_Client_delete(client);
        return (int)retval;
    }

    // Start the isochronous control loop
    while (true) {
        // Get the start time of the cycle
        clock_t cycle_start_time = clock();

        // Read the value from the actuator node
        UA_Variant value; // Variant to hold the node value
        UA_Variant_init(&value);

        const UA_NodeId nodeId = UA_NODEID_STRING(1, "ActuatorData"); // NodeId of the actuator node
        retval = UA_Client_readValueAttribute(client, nodeId, &value);

        if(retval == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT32])) {
            UA_Int32 action = *(UA_Int32 *)value.data;
            printf("Action command received: %d\n", action);
        }

        // Calculate the remaining time in the network cycle
        clock_t cycle_end_time = clock();
        double cycle_duration = ((double) (cycle_end_time - cycle_start_time)) / CLOCKS_PER_SEC * 1000;
        int remaining_time = NETWORK_CYCLE_TIME - (int) cycle_duration;

        // Sleep for the remaining time in the network cycle
        if (remaining_time > 0) {
            usleep(remaining_time * 1000); // Convert to microseconds for usleep
        }
    }

    // Clean up the OPC UA client
    if(client) {
        UA_Client_delete(client); // disconnects the client internally
        client = NULL;
    }
    return UA_STATUSCODE_GOOD;
}