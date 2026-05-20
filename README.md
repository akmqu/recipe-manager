# SAVEUR - Recipe Management App

![C++](https://img.shields.io/badge/C%2B%2B-11%2F14-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-Widgets%20%2B%20SQL-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![PostgreSQL](https://img.shields.io/badge/PostgreSQL-Database-4169E1?style=for-the-badge&logo=postgresql&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-Build%20System-064F8C?style=for-the-badge&logo=cmake&logoColor=white)

**SAVEUR** is a desktop recipe management application built with **C++**, **Qt**, and **PostgreSQL**.

It helps users organize recipes, plan meals, manage shopping items, and view useful cooking statistics in one clean interface.

---

## ✨ Features

- **Recipe CRUD**
  - Add, edit, view, and delete recipes.
  - Store recipe details such as name, category, difficulty, ingredients, notes, preparation time, cooking time, rating, and image.
  - Mark recipes as favorites.

- **Recipe Browser**
  - Browse recipes using reusable recipe cards.
  - Search recipes by name.
  - Filter recipes by category.
  - Sort recipes by date, total time, name, difficulty, and rating.

- **Meal Planner**
  - Plan meals for the whole week.
  - Assign recipes to different days and meal types.
  - Save meal plan data in PostgreSQL.

- **Shopping List**
  - Add shopping list items.
  - Mark products as bought.
  - Move completed items into a separate section.
  - Clear bought items when they are no longer needed.

- **Statistics Dashboard**
  - Display total recipes and favorite recipes.
  - Calculate average rating and average preparation/cooking time.
  - Show fastest, longest, and latest recipes.
  - Summarize recipe categories and difficulty levels.

---

## 📸 Screenshots

| Dashboard | Add Recipe |
|---|---|
| ![Dashboard](RecipeManager/screenshots/dashboard.png) | ![Add Recipe](RecipeManager/screenshots/add-recipe.png) |

| Meal Planner | Shopping List |
|---|---|
| ![Meal Planner](RecipeManager/screenshots/meal-planner.png) | ![Shopping List](RecipeManager/screenshots/list.png) |

| Favorites | Statistics |
|---|---|
| ![Favorites](RecipeManager/screenshots/fav.png) | ![Statistics](RecipeManager/screenshots/statistics.png) |

---

## 🛠 Tech Stack

- **C++**
- **Qt Widgets**
- **Qt SQL**
- **PostgreSQL**
- **CMake**
- **QSS** for custom styling
- **Qt Designer** for `.ui` layouts

---

## 🏗 Architecture

The application follows a clean, modular architecture, separating the user interface from data processing and database management.

```mermaid
graph TD
    UI["Frontend / UI Layer<br/>(Qt Widgets & QSS)"] 
    Logic["Core Logic<br/>(C++ / Controllers)"]
    DB_Layer["Database Access Layer<br/>(Qt SQL)"]
    DB[("PostgreSQL<br/>Database")]

    UI <-->|Events & Data| Logic
    Logic <-->|SQL Queries| DB_Layer
    DB_Layer <-->|Read / Write| DB
    
    classDef default fill:#1e1e1e,stroke:#333,stroke-width:2px,color:#fff;
    classDef database fill:#00599c,stroke:#000,stroke-width:2px,color:#fff;
    class DB database;
```

---

## 🚀 Getting Started

Follow these steps to set up and run **SAVEUR** on your local machine.

### 📋 Prerequisites

Make sure you have the following installed:

- C++ Compiler (supporting C++11/14)
- CMake
- Qt 5 or Qt 6 (with `Widgets` and `SQL` modules)
- PostgreSQL

---

### 1️⃣ Clone the repository

```bash
git clone [https://github.com/akmqu/recipe-manager.git](https://github.com/akmqu/recipe-manager.git)
cd recipe-manager
```

---

### 2️⃣ Prepare PostgreSQL

Create a new PostgreSQL database named `recipes_db` (you can use pgAdmin or the terminal).

The application uses these default credentials to connect (you can change them in `DatabaseManager.cpp`):

| Setting  | Default value |
|----------|---------------|
| Host     | `localhost`   |
| Port     | `5432`        |
| Database | `recipes_db`  |
| User     | `postgres`    |
| Password | `123`         |

*Note: On the first launch, the application will automatically run `schemas/schema.sql` to create all necessary tables.*

---

### 3️⃣ Build the project

From the repository root folder, run:

```bash
cmake -S RecipeManager -B build
cmake --build build
```

---

### 4️⃣ Run the application

```bash
./build/RecipeManager
```
---