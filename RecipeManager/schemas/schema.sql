-- ============================================================
--  Recipe Manager – database schema
--  PostgreSQL 13+
--
--  Setup:
--    1. createdb recipes_db
--    2. psql -U postgres -d recipes_db -f schema.sql
-- ============================================================

CREATE TABLE IF NOT EXISTS recipe (
    recipe_id   SERIAL PRIMARY KEY,
    name        VARCHAR(255) NOT NULL,
    prep_time   INTEGER DEFAULT 0,
    cook_time   INTEGER DEFAULT 0,
    ingredients TEXT,
    notes       TEXT,
    category    VARCHAR(255) DEFAULT 'default',
    difficulty  VARCHAR(255) DEFAULT 'default',
    is_favorite BOOLEAN NOT NULL DEFAULT false,
    image_path  TEXT,
    rating      INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS shopping_list (
    item_id   SERIAL PRIMARY KEY,
    name      VARCHAR(255) NOT NULL,
    is_bought BOOLEAN NOT NULL DEFAULT false
);

CREATE TABLE IF NOT EXISTS meal_plan (
    day_of_week INTEGER NOT NULL CHECK (day_of_week BETWEEN 0 AND 6),
    meal_type   INTEGER NOT NULL CHECK (meal_type BETWEEN 0 AND 3),
    recipe_id   INTEGER REFERENCES recipe(recipe_id) ON DELETE SET NULL,
    PRIMARY KEY (day_of_week, meal_type)
);