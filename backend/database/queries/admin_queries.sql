-- :name get_admin :one
select admins.id, admins.login, admins.password
from admins
where admins.login = :login;

-- :name update_admin
update admins
set login    = coalesce(:new_login, login),
    password = coalesce(:new_password, password)
where id = :admin_id;

-- :name insert_admin
insert into admins (login, password)
values (:login, :password)
returning id;