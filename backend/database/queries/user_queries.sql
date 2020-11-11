-- :name get_user :one
select users.id, users.code, users.nickname, users.password, users.class_id
from users
where users.code = :code;

-- :name insert_user
insert into users (code, password)
values (:code, :password)
returning id;

-- :name update_user
update users
set nickname = coalesce(:new_nickname, nickname),
    password = coalesce(:new_password, password),
    class_id = coalesce(:new_class_id, class_id)
where id = :user_id;

-- :name delete_user
delete
from users
where id = :user_id;