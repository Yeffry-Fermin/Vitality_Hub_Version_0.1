# Vitality Hub
### *Turning "Inner Noise" into Actionable Data*

**Vitality Hub** is a C++ application designed to bridge the gap between subjective emotional states and objective data analysis. It is a specialized tool for high-performance individuals—specifically those balancing academic rigor in Computer Science and intensive caregiving responsibilities—to track and manage chronic stress and anxiety.

---

## 💡 The Mission

This solution was born from a personal need to turn **"inner noise" into actionable data.** After five years of managing chronic anxiety, I realized that many existing tools felt generic and lacked the depth needed for meaningful reflection. 

Following clinical advice that **stress-awareness is a prerequisite for recovery**, I developed this application to help me identify patterns in my own life. It is a tool designed by a user, for a user, to turn the abstract feeling of anxiety into understandable insights. By translating my internal state into objective data, I am shifting from being a passive observer of my anxiety to being the **architect of my own recovery.** This application is my way of using the logic of Computer Science to find clarity in the chaos of chronic stress.

---

## 🚀 Key Features

* **Quantitative Mood Logging:** Capture stress and anxiety levels on a 1–10 scale.
* **Trigger Tagging:** Associate entries with specific stressors (e.g., "exams," "commute," "caregiving") using a multi-tag system.
* **Trend Analysis Engine:** * Calculates total average stress and anxiety scores.
    * Implements a "Sliding Window" to view 7-day and 30-day averages.
    * Calculates a **Weighted Vitality Score** to provide a single metric for overall wellness.
* **Frequency Tracking:** Identifies recurring triggers to highlight the most persistent sources of anxiety.
* **Data Persistence:** A robust CSV-based file manager that ensures logs are saved and retrieved across sessions.

---

## 🛠️ Technical Architecture

The project is built using **Object-Oriented Programming (OOP)** principles to ensure the code is modular and easy to maintain.

* **`MoodEntry` Class:** The core data structure holding stress levels, anxiety levels, a vector of trigger tags, and a timestamp.
* **`FileManager` Class:** Handles the serialization and deserialization of data, moving objects between the application and the `mood_log.csv` file.
* **`TrendAnalyzer` Class:** The logic engine that performs statistical calculations and frequency analysis on the stored data.

### Data Format
The application utilizes a flat-file CSV structure for storage:
`[Stress],[Anxiety],[Trigger|Tags],[Timestamp]`

---

## 📂 Installation & Usage

### Prerequisites
* A C++17 compatible compiler (GCC, Clang, or MSVC).

### Setup
1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Build the project using CMake:
   ```bash
   mkdir build && cd build
   cmake ..
   make
