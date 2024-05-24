#include "open62541.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h> // Include this for usleep
#include <stdlib.h> // Include this for rand and srand

#define NETWORK_CYCLE_TIME 1000 // Network cycle time in milliseconds
#define MIN_TEMPERATURE 20 // Minimum temperature in degrees Celsius
#define MAX_TEMPERATURE 45 // Maximum temperature in degrees Celsius
#define MAX_CHANGE 2 // Maximum change in temperature per cycle

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

    // Seed the random number generator
    srand(time(NULL));

    // Start the isochronous control loop
    UA_Int32 sensorData = MIN_TEMPERATURE + rand() % (MAX_TEMPERATURE - MIN_TEMPERATURE + 1); // Initial temperature
    while (true) {
        // Get the start time of the cycle
        clock_t cycle_start_time = clock();

        // Write a random temperature value to the sensor node
        UA_Variant value; // Variant to hold the node value
        UA_Variant_init(&value);

        // Generate a small random change in temperature
        sensorData += (rand() % (2 * MAX_CHANGE + 1)) - MAX_CHANGE; // Subtract MAX_CHANGE to make the change range from -MAX_CHANGE to +MAX_CHANGE
        // Ensure the temperature stays within the min and max limits
        if (sensorData < MIN_TEMPERATURE) {
            sensorData = MIN_TEMPERATURE;
        } else if (sensorData > MAX_TEMPERATURE) {
            sensorData = MAX_TEMPERATURE;
        }

        UA_Variant_setScalar(&value, &sensorData, &UA_TYPES[UA_TYPES_INT32]);

        const UA_NodeId nodeId = UA_NODEID_STRING(1, "SensorData"); // NodeId of the sensor node
        retval = UA_Client_writeValueAttribute(client, nodeId, &value);

        if(retval == UA_STATUSCODE_GOOD) {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Successfully wrote value to sensor node: %i", sensorData);
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