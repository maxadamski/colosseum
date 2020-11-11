-- :name get_classes :many
select classes.id, classes.name
from classes;

-- :name get_class :one
select classes.id, classes.name
from classes
where classes.name = :name;

-- :name insert_class
insert into classes (name)
values (:name)
returning id;

-- :name update_class
update classes
set name = :name
where id = :class_id;

-- :name delete_class
delete
from classes
where id = :class_id;