## Dungeon Crawler — Console (Written in C and C++)

A text-based **ASCII dungeon crawler** built as a final project, written in **C-style code** and compiled with **MSVC** using **JetBrains Rider**.  
The project focuses on clean architecture, safe input handling, and readable console UI design.

---

## Features

- ASCII-based UI (headers, dividers, centered text)
- Menu-driven gameplay with validated input
- Clear separation of **UI** and **Game Logic**
- MSVC-compatible (Windows)

---

## Assignment Context
This project was originally developed as a first-term final assignment
for an Introduction to Computer Programming course.

The original assignment brief is included in the `docs/` folder
for reference.

---

## 🗂️ Project Structure

```text
Final Project/
├── Docs
│   └── Final Project_ Dungeon Crawler.pdf   # Instructions
├── Main/
│   └── Main.cpp        # Program entry point
├── Game/
│   ├── Game.h          # Game definitions & prototypes
│   └── Game.cpp        # Core game logic
├── UI/
│   ├── UI.h            # UI function declarations
│   └── UI.cpp          # Console UI & input handling
├── .gitignore
└── README.md
