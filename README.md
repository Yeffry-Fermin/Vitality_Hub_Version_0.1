# Vitality Hub  
### *Turning Behavioral Data into Predictive Insight*

**Vitality Hub** is a C++ analytics application that tracks mood-related data (stress, energy, sleep, and activities) and transforms it into structured insights using statistical analysis, correlation modeling, and trend detection.

The system is designed to move beyond simple logging by analyzing relationships between behaviors and internal states, helping users understand how daily actions influence overall performance and well-being.

---

## Key Features

- **Mood Data Logging (SQLite Database)**
  - Stores stress, energy, sleep, notes, and activity triggers
  - Persistent storage using SQL-based database system

- **Trigger-Based Behavior Tracking**
  - Associates moods with user-defined activities (e.g., “exam”, “gym”, “work”)
  - Groups entries by behavior to identify patterns

- **Statistical Analysis Engine**
  - Computes averages for stress, energy, and sleep
  - 14-day trend comparison (weekly performance shifts)
  - Rolling performance momentum tracking

- **Correlation Analysis System**
  - Measures relationships between variables using Pearson correlation
  - Energy ↔ Sleep, Energy ↔ Stress, Sleep ↔ Stress
  - Identifies primary behavioral drivers

- **Insight Generation Engine**
  - Converts statistical outputs into human-readable system reports
  - Classifies system state (e.g., overload, stability, peak performance)

- **Report System (OOP Architecture)**
  - Uses inheritance-based `Report` system for structured output formatting
  - Separates analysis logic from presentation layer

---

## Technical Architecture

The project is built using Object-Oriented Programming (OOP) principles and layered system design:

- **`MoodEntry` Class**
  - Core data model representing a single user log entry
  - Stores stress, energy, sleep, triggers, notes, and timestamps

- **`DatabaseManager` Class**
  - Handles SQLite database operations
  - Responsible for inserting and retrieving mood data using SQL queries

- **`AnalyticsEngine` Class**
  - Core computation layer for statistical analysis
  - Implements averages, correlation analysis, and trend detection
  - Generates system insights based on computed metrics

- **`Report` System (Inheritance)**
  - Base `Report` class defines shared structure (headers/footers)
  - Derived report classes implement specific analytics views

- **`AppController` Class**
  - Handles user interaction and application flow
  - Connects UI input with analytics and database layers

---

## Data Model

Each mood entry contains:

- Stress Level (1–5)
- Energy Level (1–5)
- Sleep Hours (0–24)
- Activity Triggers (tags)
- Optional Notes
- Timestamp

Stored using a structured SQLite schema for persistent analysis.

---

## Core Design Philosophy

Vitality Hub is built around the idea that behavioral patterns can be quantified and analyzed like a system.

Instead of treating mood as abstract or emotional noise, the system:
- Converts it into measurable variables
- Finds relationships between those variables
- Uses those relationships to generate actionable insight

---

## Requirements

- C++17 or higher
- SQLite3
- CMake (for build system)

---

## Build & Run

```bash
mkdir build
cd build
cmake ..
make
./VitalityHub
