#include "open62541.h"
#include <stdio.h>
#include <unistd.h> // Include this for the usleep function


// Definir el tiempo de ciclo de la aplicación en milisegundos
#define APPLICATION_CYCLE_TIME_MS 100

// Definir el tiempo de transferencia en milisegundos
#define TRANSFER_TIME_MS 10

// Definir el tiempo de seguridad de la aplicación en milisegundos
#define APPLICATION_SAFETY_MARGIN_MS 5

// Definir el tiempo de seguridad de transferencia en milisegundos
#define TRANSFER_SAFETY_MARGIN_MS 5

// Definir el tiempo de cálculo máximo disponible para un bucle de control con una relación de reducción X
#define MAX_COMPUTATION_TIME_MS (APPLICATION_CYCLE_TIME_MS * X - TRANSFER_TIME_MS - APPLICATION_SAFETY_MARGIN_MS)


// Definir los umbrales de temperatura y los niveles de velocidad del ventilador
#define HIGH_TEMPERATURE_THRESHOLD 35 // Umbral alto de temperatura en grados Celsius
#define LOW_TEMPERATURE_THRESHOLD 25 // Umbral bajo de temperatura en grados Celsius
#define HIGH_FAN_SPEED 70 // Velocidad del ventilador alta en porcentaje
#define MEDIUM_FAN_SPEED 50 // Velocidad del ventilador media en porcentaje
#define LOW_FAN_SPEED 30 // Velocidad del ventilador baja en porcentaje


// Definir los umbrales de temperatura y los niveles de velocidad del ventilador
#define HIGH_TEMPERATURE_THRESHOLD 35 // Umbral alto de temperatura en grados Celsius
#define LOW_TEMPERATURE_THRESHOLD 25 // Umbral bajo de temperatura en grados Celsius
#define HIGH_FAN_SPEED 70 // Velocidad del ventilador alta en porcentaje
#define MEDIUM_FAN_SPEED 50 // Velocidad del ventilador media en porcentaje
#define LOW_FAN_SPEED 30 // Velocidad del ventilador baja en porcentaje

void controlLoopFunction(UA_Server *server, UA_NodeId sensorNodeId, UA_NodeId actuatorNodeId) {
    // Crear un Variant para almacenar el valor leído
    UA_Variant *value = UA_Variant_new();
    if (!value) {
        fprintf(stderr, "Unable to create Variant\n");
        return;
    }

    // Leer el valor del nodo del sensor
    UA_StatusCode retval = UA_Server_readValue(server, sensorNodeId, value);

    if (retval == UA_STATUSCODE_GOOD &&
        UA_Variant_hasScalarType(value, &UA_TYPES[UA_TYPES_INT32])) {
        UA_Int32 sensorValue = *(UA_Int32 *) value->data;
        printf("El valor del sensor es: %d\n", sensorValue);

        // Lógica de toma de decisiones basada en el valor de la temperatura
        UA_Int32 action;
        if (sensorValue > HIGH_TEMPERATURE_THRESHOLD) {
            // La temperatura es demasiado alta, aumentar la velocidad del ventilador a un nivel alto
            action = 1;
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "La temperatura es demasiado alta. Aumentando la velocidad del ventilador a un nivel alto. Acción: %d",
                        action);
        } else if (sensorValue < LOW_TEMPERATURE_THRESHOLD) {
            // La temperatura es demasiado baja, reducir la velocidad del ventilador a un nivel bajo
            action = 2;
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "La temperatura es demasiado baja. Reduciendo la velocidad del ventilador a un nivel bajo. Acción: %d",
                        action);
        } else {
            // La temperatura está en un rango aceptable, ajustar la velocidad del ventilador a un nivel medio
            action = 3;
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "La temperatura está en un rango aceptable. Ajustando la velocidad del ventilador a un nivel medio. Acción: %d",
                        action);
        }

        // Escribir el nuevo valor en el nodo del actuador
        UA_Variant_setScalar(value, &action, &UA_TYPES[UA_TYPES_INT32]);
        retval = UA_Server_writeValue(server, actuatorNodeId, *value);

        if (retval != UA_STATUSCODE_GOOD) {
            fprintf(stderr, "Error writing actuator value\n");
        } else {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Valor escrito en el nodo del actuador: %d", action);
        }
    } else {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Error reading sensor value");
    }

    // Limpiar el Variant después de usarlo
    UA_Variant_delete(value);
}

int main(void) {
    // Declare the running variable
    UA_Boolean running = true;

    // Crear una nueva instancia de un servidor OPC UA
    UA_Server *server = UA_Server_new();

    // Agregar un nodo que represente a un sensor
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_Int32 sensorData = 0;
    UA_Variant_setScalar(&attr.value, &sensorData, &UA_TYPES[UA_TYPES_INT32]);
    attr.description = UA_LOCALIZEDTEXT("en-US", "Sensor Data");
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Sensor Data");
    attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId sensorNodeId = UA_NODEID_STRING(1, "SensorData");
    UA_QualifiedName sensorName = UA_QUALIFIEDNAME(1, "SensorData");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NULL; // take the default variable type

    UA_Server_addVariableNode(server, sensorNodeId, parentNodeId,
                              parentReferenceNodeId, sensorName,
                              variableTypeNodeId, attr, NULL, NULL);

    // Agregar un nodo que represente a un actuador
    UA_VariableAttributes attrActuator = UA_VariableAttributes_default;
    UA_Int32 actuatorData = 0;
    UA_Variant_setScalar(&attrActuator.value, &actuatorData, &UA_TYPES[UA_TYPES_INT32]);
    attrActuator.description = UA_LOCALIZEDTEXT("en-US", "Actuator Data");
    attrActuator.displayName = UA_LOCALIZEDTEXT("en-US", "Actuator Data");
    attrActuator.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attrActuator.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId actuatorNodeId = UA_NODEID_STRING(1, "ActuatorData");
    UA_QualifiedName actuatorName = UA_QUALIFIEDNAME(1, "ActuatorData");

    UA_Server_addVariableNode(server, actuatorNodeId, parentNodeId,
                              parentReferenceNodeId, actuatorName,
                              variableTypeNodeId, attrActuator, NULL, NULL);

    // Ejecutar el servidor OPC UA hasta que se interrumpe
    while (UA_Server_run(server, &running) == UA_STATUSCODE_GOOD && running) {
        // Obtener el tiempo actual
        clock_t start_time = clock();

        // Ejecutar la función de bucle de control
        controlLoopFunction(server, sensorNodeId, actuatorNodeId);

        // Crear un Variant para almacenar el valor leído
        UA_Variant *value = UA_Variant_new();
        if (!value) {
            fprintf(stderr, "Unable to create Variant\n");
            return -1;
        }

        // Leer el valor del nodo del sensor
        UA_StatusCode retval = UA_Server_readValue(server, sensorNodeId, value);

        if (retval == UA_STATUSCODE_GOOD &&
            UA_Variant_hasScalarType(value, &UA_TYPES[UA_TYPES_INT32])) {
            UA_Int32 sensorValue = *(UA_Int32 *) value->data;
            printf("El valor del sensor es: %d\n", sensorValue);
        } else {
            fprintf(stderr, "Error reading sensor value\n");
        }

        // Limpiar el Variant después de usarlo
        UA_Variant_delete(value);

        // Calcular el tiempo que ha pasado desde que comenzó el ciclo de la aplicación
        clock_t elapsed_time = clock() - start_time;

        // Si el tiempo transcurrido es menor que el tiempo de ciclo de la aplicación, dormir durante el tiempo restante
        if (elapsed_time < APPLICATION_CYCLE_TIME_MS) {
            usleep((APPLICATION_CYCLE_TIME_MS - elapsed_time) * 1000); // Convert to microseconds
        }
    }

    // Limpiar el servidor OPC UA
    UA_Server_delete(server);
    return 0;
}