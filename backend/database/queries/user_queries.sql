-- :name get_user_by_code :one
SELECT *
FROM users
WHERE code = :code;

-- :name get_user_by_nickname :one
SELECT *
FROM users
WHERE nickname = :nickname;

-- :name insert_user :scalar
INSERT INTO users (code, password, nickname, class_id)
VALUES (:code, :password, :nickname, :class_id)
RETURNING id;

-- :name update_user
UPDATE users
SET code     = coalesce(:new_code, code),
    nickname = coalesce(:new_nickname, nickname),
    password = coalesce(:new_password, password),
    class_id = coalesce(:new_class_id, class_id)
WHERE id = :user_id;

-- :name remove_user
DELETE
FROM users
WHERE id = :user_id;

-- :name remove_all_users
DELETE
FROM users;