-- :name get_games :many
SELECT *
FROM games;

-- :name get_game :one
SELECT *
FROM games
WHERE id = :game_id;

-- :name insert_game :scalar
INSERT INTO games (name,
                   subtitle,
                   automake,
                   deadline,
                   environment_id)
VALUES (:name, :subtitle, :automake, :deadline, :environment_id)
RETURNING id;

-- :name update_game_path
UPDATE games
SET files_path = :files_path
WHERE id = :game_id;

-- :name update_game
UPDATE games
SET name           = coalesce(:new_name, name),
    subtitle       = coalesce(:new_subtitle, subtitle),
    automake       = coalesce(:new_automake, automake),
    deadline       = coalesce(:new_deadline, deadline),
    environment_id = coalesce(:new_environment_id, environment_id)
WHERE id = :game_id;

-- :name activate_game
UPDATE games
SET is_active = TRUE
WHERE id = :game_id;

-- :name remove_game
DELETE
FROM games
WHERE id = :game_id;