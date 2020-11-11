-- :name get_user_by_code :one
select users.id, users.code, users.nickname, users.password, users.class_id
from users
where users.code = :code;

-- :name get_user_by_nickname :one
select users.id, users.code, users.nickname, users.password, users.class_id
from users
where users.nickname = :nickname;

-- :name insert_user
insert into users (code, password, nickname, class_id)
values (:code, :password, :nickname, :class_id)
returning id;

-- :name update_user
update users
set code     = coalesce(:new_code, code),
    nickname = coalesce(:new_nickname, nickname),
    password = coalesce(:new_password, password),
    class_id = coalesce(:new_class_id, class_id)
where id = :user_id;

-- :name delete_user
delete
from users
where id = :user_id;