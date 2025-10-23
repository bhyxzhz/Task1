# Task 1 - Satellite Anomaly Detection

## Description

A network of CubeSats is orbiting Earth, collecting environmental data. This program's task is to
process a stream of magnetometer readings from these satellites to identify and output sudden,
significant fluctuations that represent anomalies, likely caused by solar flares.

The program must be able to read a sequence of data packets, analyze them, and report any readings
that fall outside of a predefined range of normalcy.

## Anomaly Detection Rules

A reading is considered an anomaly if its magnetic_field value is more than 5 times the average
of the previous 5 valid readings from the same sensor. You can assume that the first 5 readings of each sensor 
are correct.

To ensure the analysis is based on current environmental conditions, any reading received by the
ground station is considered stale and must be disregarded if its sent_time is more than 30 minutes
older than its cluster's received_time.

## Input

The program will accept a JSON file as a command-line argument containing a single JSON object.
This object will contain an array of cluster objects with cluster ID, time received and an array of
readings from multiple sensors.

Each cluster is a JSON object with the following structure:
- cluster_id: A unique identifier for the data cluster.
- received_time: The UTC timestamp of when the entire data cluster was received at the ground station.
- readings: An array of the individual sensor readings within that cluster.


Each reading object within the readings array has the following structure:
- sent_time: The UTC timestamp of when the reading was collected by the satellite.
- sensor_id: The identifier for the specific magnetometer sensor.
- magnetic_field: The magnetic field strength reading in an arbitrary unit.

> [!NOTE]
> If you're using C++ you can use one of the two popular helper libraries
> to handle JSON deserialization and serialization. You can find documentation
> for using [Qt](https://doc.qt.io/qt-6/qjsonobject.html) and [NLohman/json](https://github.com/nlohmann/json).
> linked here. Usage examples are found in `Examples/main.cpp` and
> `Examples/CMakeLists.txt`.

```json
{
    "data_clusters": [
        {
            "cluster_id": "cluster-1",
            "received_time": "2025-09-12T12:00:00Z",
            "readings": [
                {
                    "magnetic_field": 25.5,
                    "sensor_id": "sensor-A",
                    "sent_time": "2025-09-12T11:50:00Z"
                },
                {
                    "magnetic_field": 25.5,
                    "sensor_id": "sensor-A",
                    "sent_time": "2025-09-12T11:51:00Z"
                },
                {
                    "magnetic_field": 25.5,
                    "sensor_id": "sensor-A",
                    "sent_time": "2025-09-12T11:52:00Z"
                },
                {
                    "magnetic_field": 25.5,
                    "sensor_id": "sensor-A",
                    "sent_time": "2025-09-12T11:53:00Z"
                },
                {
                    "magnetic_field": 25.5,
                    "sensor_id": "sensor-A",
                    "sent_time": "2025-09-12T11:54:00Z"
                },
                {
                    "magnetic_field": 500.1,
                    "sensor_id": "sensor-A",
                    "sent_time": "2025-09-12T11:55:00Z"
                }
            ]
        }
    ]
}
```

## Usage

```bash
# C++
./Task1/build/Main <input_file.json>

# Python
python Task1/main.py <input_file.json>
```

Example:
```bash
# C++
./Task1/build/Main testdata.json

# Python
python Task1/main.py testdata.json
```

## Output

Your application should print out to the command line the JSON object of the readings that are flagged
as potentially malfunctioning.

Anomaly is described by the following fields:
- magnetic_field: the reading that was flagged as potentially wrong
- sensor_id: the ID of the potentially malfunctioning sensor
- sent_time: the timestamp of the potential malfunction

```json
{
    "anomalies": [
        {
            "magnetic_field": 500.1,
            "sensor_id": "sensor-A",
            "sent_time": "2025-09-12T11:55:00Z"
        }
    ]
}
```
> [!NOTE]
> The list of anomalies should be sorted by sent_time, ascending.

> [!NOTE]
> The auto tester will require a 4 space indentation rule for your JSON output
