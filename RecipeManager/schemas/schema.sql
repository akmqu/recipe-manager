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