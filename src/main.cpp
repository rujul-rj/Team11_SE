
#include "Simulation.h"
#include "network_config_parser.h"

#include <chrono>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {
std::string audit_timestamp() {
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream output;
    output << std::put_time(std::localtime(&now), "%Y-%m-%dT%H:%M:%S");
    return output.str();
}

void append_network_config_audit(const std::string& source, const std::string& message) {
    std::filesystem::create_directories("logs");
    std::ofstream log("logs/invalid-config.log", std::ios::app);
    if (!log) return;
    auto safe = [](std::string value) {
        for (char& ch : value) if (ch == '\n' || ch == '\r') ch = ' ';
        return value;
    };
    log << audit_timestamp() << " INVALID_CONFIG [" << safe(source)
        << "]: " << safe(message) << '\n';
}
}

int main() {
    try {
        std::ifstream networkInput("config/network_example.cfg");
        if (!networkInput) {
            throw std::runtime_error("Could not open config/network_example.cfg; run from the repository root.");
        }
        auto networkResult = parse_network_config(networkInput);
        if (!networkResult.ok) {
            std::string message = "Invalid network configuration";
            for (const auto& error : networkResult.errors) {
                append_network_config_audit("config/network_example.cfg", error);
                message += "\n - " + error;
            }
            throw std::runtime_error(message);
        }

        Simulation simulation(std::move(networkResult.network), 1.0);

        std::ifstream signalInput("config/signals_example.cfg");
        if (!signalInput) {
            throw std::runtime_error("Could not open config/signals_example.cfg.");
        }
        std::filesystem::create_directories("logs");
        std::ofstream auditLog("logs/invalid-config.log", std::ios::app);
        if (!auditLog) throw std::runtime_error("Could not open logs/invalid-config.log.");
        simulation.signalManager().set_log_sink([&auditLog](const std::string& line) {
            auditLog << line << '\n';
            auditLog.flush();
        });

        const auto signalResult = simulation.signalManager().configure_from_stream(
            signalInput, "config/signals_example.cfg"
        );
        if (!signalResult.ok) {
            std::string message = "Invalid signal configuration";
            for (const auto& error : signalResult.errors) message += "\n - " + error;
            throw std::runtime_error(message);
        }

        // Add two initial vehicles manually.
        simulation.addVehicle(Vehicle("V1", {"A", "B", "C"}));
        simulation.addVehicle(Vehicle("V2", {"A", "B", "C"}));
        simulation.addVehicle(Vehicle("EV1", {"A", "C"}, VehicleType::Emergency));

        // Automatically generate up to 5 more vehicles,
        // one every 2 simulation steps.
        simulation.configureVehicleGeneration(
            {{"A", "B", "C"}},
            TrafficDensity::Medium,
            5
        );

        // Demonstrate vehicles waiting at a red signal at B.
        simulation.setSignal("B", SignalState::Red);
        simulation.setRoadCapacityReduction("B", "C", 0.5);

        std::cout << "Traffic Management System Simulation\n";
        std::cout << "Scenario: A -> B -> C\n";
        std::cout << "Initial vehicles: 2 normal + 1 emergency\n";
        std::cout << "Automatic generation: MEDIUM density, max 5\n";
        std::cout << "Incident: B->C capacity reduced by 50% until step 8\n\n";

        for (int tick = 1; tick <= 12; ++tick) {
            // Release vehicles waiting at B from step 4 onward.
            if (tick == 4) {
                simulation.setSignal("B", SignalState::Green);
            }
            if (tick == 8) {
                simulation.setRoadCapacityReduction("B", "C", 1.0);
            }

            simulation.step();

            const auto& stats = simulation.lastStepStats();

            std::cout << "Step " << tick
                      << " | vehicles=" << simulation.vehicles().size()
                      << " | active=" << stats.activeVehicles
                      << " | queued=" << stats.queuedVehicles
                      << " | arrived this step=" << stats.throughput
                      << " | total arrived=" << simulation.totalArrived()
                      << " | accumulated wait(s)="
                      << stats.totalWaitingTimeSeconds
                      << '\n';
        }

        std::cout << "\nSimulation completed.\n";
        const auto summary = simulation.summary();
        std::cout << "\nEnd-of-run summary\n"
                  << "Steps executed: " << summary.stepsExecuted << '\n'
                  << "Vehicles added: " << summary.vehiclesAdded << '\n'
                  << "Vehicles arrived: " << summary.totalArrived << '\n'
                  << "Active vehicles: " << summary.activeVehicles << '\n'
                  << std::fixed << std::setprecision(2)
                  << "Total waiting time (vehicle-seconds): " << summary.totalWaitingTimeSeconds << '\n'
                  << "Average waiting time (seconds/vehicle): " << summary.averageWaitingTimeSeconds << '\n'
                  << "Average queue length (vehicles/step): " << summary.averageQueueLength << '\n'
                  << "Throughput (vehicles/second): " << summary.throughputVehiclesPerSecond << '\n';
        return 0;

    } catch (const std::exception& ex) {
        std::cerr << "Simulation error: " << ex.what() << '\n';
        return 1;
    }
}
