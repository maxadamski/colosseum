-- :name get_teams :many
select *
from teams;

-- :name get_team :one
select id, name, leader_id
from teams
where id = :team_id;

-- :name update_team
update teams
set name      = coalesce(:new_name, name),
    leader_id = coalesce(:new_leader_id, leader_id)
where id = :team_id;

-- :name get_user_team :one
select t.id, t.name, t.leader_id
from team_users tu
         inner join teams t on tu.team_id = t.id
where tu.user_id = :user_id;

-- :name change_team
update team_users
set team_id = :team_id
where user_id = :user_id;

-- :name get_team_members :many
select u.id, u.nickname
from team_users tu
         inner join users u on tu.user_id = u.id
where tu.team_id = :team_id;

-- :name remove_user_from_team
delete
from team_users
where user_id = :user_id
  and team_id = :team_id;

-- :name get_team_invites :many
select u.id, u.nickname
from team_invites ti
         inner join users u on ti.user_id = u.id
where ti.team_id = :team_id;

-- :name get_user_invites :many
select t.id, t.name
from team_invites ti
         inner join teams t on ti.team_id = t.id
where ti.user_id = :user_id;

-- :name invite_user_to_team
insert into team_invites(user_id, team_id)
values (:user_id, :team_id);

-- :name remove_invite
delete
from team_invites
where user_id = :user_id
  and team_id = :team_id;