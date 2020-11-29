-- :name get_student :one
SELECT s.id, s.nickname, g.name as group_name
FROM students s JOIN groups g ON g.id = s.group_id
WHERE s.id = :student_id;

-- :name get_student_by_login :one
SELECT *
FROM students
WHERE login = :login;

-- :name get_student_by_nickname :one
SELECT *
FROM students
WHERE nickname = :nickname;

-- :name insert_student :scalar
INSERT INTO students (login, password, nickname, group_id)
VALUES (:login, :password, :nickname, :group_id)
RETURNING id;

-- :name update_student
UPDATE students
SET login     = coalesce(:new_login, login),
    nickname = coalesce(:new_nickname, nickname),
    password = coalesce(:new_password, password),
    group_id = coalesce(:new_group_id, group_id)
WHERE id = :user_id;

-- :name remove_student
DELETE
FROM students
WHERE id = :student_id;

-- :name remove_all_students
DELETE
FROM students;