-- :name get_submission :one
SELECT *
FROM submissions
WHERE id = :submission_id;

-- :name get_team_submissions :many
SELECT s.id,
       s.submission_time,
       s.name                    AS name,
       status,
       e.name                    AS environment_name,
       s.status,
       (tps.team_id IS NOT NULL) AS is_primary
FROM submissions s
         JOIN environments e ON s.environment_id = e.id
         LEFT JOIN team_primary_submissions tps ON s.id = tps.submission_id
WHERE s.team_id = :team_id;

-- :name insert_team_submission :scalar
INSERT INTO submissions (name,
                         automake,
                         environment_id,
                         team_id)
VALUES (:name, :automake, :environment_id, :team_id)
RETURNING id;

-- :name set_primary_submission
UPDATE team_primary_submissions
SET submission_id = :submission_id
WHERE team_id = :team_id;

-- :name get_admins_submissions :many
SELECT id, submission_time, name, automake, status, environment_id
FROM submissions
WHERE admin_id IS NOT NULL;

-- :name insert_admin_submission :scalar
INSERT INTO submissions (name,
                         automake,
                         environment_id,
                         admin_id)
VALUES (:name, :automake, :environment_id, :admin_id)
RETURNING id;

-- :name update_submission
UPDATE submissions
SET name            = coalesce(:new_name, name),
    automake        = coalesce(:new_automake, automake),
    environment_id  = coalesce(:new_environment_id, environment_id),
    submission_time = default
WHERE id = :submission_id;

-- :name update_submission_path
UPDATE submissions
SET files_path = :files_path
WHERE id = :submission_id;

-- :name remove_submission
DELETE
FROM submissions
WHERE id = :submission_id;

-- :name remove_all_submissions
DELETE
FROM submissions;