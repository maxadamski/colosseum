-- :name get_admin :one
SELECT *
FROM admins
WHERE login = :login;

-- :name update_admin
UPDATE admins
SET login    = coalesce(:new_login, login),
    password = coalesce(:new_password, password)
WHERE id = :admin_id;

-- :name insert_admin :scalar
INSERT INTO admins (login, password)
VALUES (:login, :password)
RETURNING id;