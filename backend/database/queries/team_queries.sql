-- :name get_user_team :one
SELECT t.id, t.name, t.leader_id
FROM team_users tu
         INNER JOIN teams t ON tu.team_id = t.id
WHERE tu.user_id = :user_id;

-- :name update_team_name
UPDATE teams
SET name = coalesce(:new_name, name)
WHERE id = :team_id;

-- :name update_team_leader
UPDATE teams
SET leader_id = coalesce(:new_leader_id, leader_id)
WHERE id = :team_id;

-- :name change_team
UPDATE team_users
SET team_id = :team_id
WHERE user_id = :user_id;

-- :name get_team_members :many
SELECT u.id, u.nickname
FROM team_users tu
         INNER JOIN users u ON tu.user_id = u.id
WHERE tu.team_id = :team_id;

-- :name remove_user_from_team
DELETE
FROM team_users
WHERE user_id = :user_id
  AND team_id = :team_id;

-- :name get_team_invites :many
SELECT u.id, u.nickname
FROM team_invites ti
         INNER JOIN users u ON ti.user_id = u.id
WHERE ti.team_id = :team_id;

-- :name get_user_invites :many
SELECT t.id, t.name, u.nickname AS leader_nickname
FROM team_invites ti
         INNER JOIN teams t ON ti.team_id = t.id
         INNER JOIN users u ON u.id = t.leader_id
WHERE ti.user_id = :user_id;

-- :name invite_user_to_team :one
INSERT INTO team_invites(user_id, team_id)
VALUES (:user_id, :team_id)
RETURNING (:user_id, :team_id);

-- :name remove_invite
DELETE
FROM team_invites
WHERE user_id = :user_id
  AND team_id = :team_id;

-- :name remove_all_teams
DELETE
FROM teams;