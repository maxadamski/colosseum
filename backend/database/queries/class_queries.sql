-- :name get_classes :many
SELECT *
FROM classes;

-- :name insert_class :scalar
INSERT INTO classes (name)
VALUES (:name)
RETURNING id;

-- :name update_class
UPDATE classes
SET name = :name
WHERE id = :class_id;

-- :name remove_class
DELETE
FROM classes
WHERE id = :class_id;

-- :name remove_all_classes
DELETE
FROM classes;