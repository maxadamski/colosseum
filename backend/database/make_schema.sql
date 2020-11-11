DROP TABLE IF EXISTS admins CASCADE;
DROP TABLE IF EXISTS classes CASCADE;
DROP TABLE IF EXISTS teams CASCADE;
DROP TABLE IF EXISTS users CASCADE;
DROP TABLE IF EXISTS team_leaders CASCADE;
DROP TABLE IF EXISTS team_users CASCADE;
DROP TABLE IF EXISTS team_invites CASCADE;
DROP TABLE IF EXISTS environments CASCADE;
DROP TABLE IF EXISTS games CASCADE;
DROP TABLE IF EXISTS submissions CASCADE;
DROP TABLE IF EXISTS results CASCADE;

DROP TYPE IF EXISTS status;
DROP TYPE IF EXISTS exec_type;

CREATE TYPE status AS ENUM ('success', 'error');
CREATE TYPE exec_type AS ENUM ('single', 'makefile');

CREATE TABLE admins
(
    id       SERIAL PRIMARY KEY,
    login    VARCHAR(50) NOT NULL UNIQUE,
    password CHAR(226)   NOT NULL
);

CREATE TABLE classes
(
    id   SERIAL PRIMARY KEY,
    name VARCHAR(10) NOT NULL UNIQUE
);

CREATE TABLE users
(
    id       SERIAL PRIMARY KEY,
    code     VARCHAR(50) NOT NULL UNIQUE,
    password CHAR(226)   NOT NULL,
    nickname VARCHAR(50) NOT NULL DEFAULT 'trig_placeholder',
    class_id INTEGER     REFERENCES classes (id) ON DELETE SET NULL
);

CREATE TABLE teams
(
    id        SERIAL PRIMARY KEY,
    name      VARCHAR(50) NOT NULL,
    leader_id INTEGER     REFERENCES users (id) ON DELETE SET NULL
);

CREATE TABLE team_users
(
    team_id INTEGER REFERENCES teams (id) ON DELETE CASCADE,
    user_id INTEGER REFERENCES users (id) ON DELETE CASCADE
);

CREATE TABLE team_invites
(
    user_id INTEGER NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    team_id INTEGER NOT NULL REFERENCES teams (id) ON DELETE CASCADE
);

CREATE TABLE environments
(
    id   SERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE
);

CREATE TABLE games
(
    id             SERIAL PRIMARY KEY,
    name           VARCHAR(50)   NOT NULL UNIQUE,
    files_path     VARCHAR(1024) NOT NULL UNIQUE,
    gui_path       VARCHAR(1024) NOT NULL UNIQUE,
    overview_path  VARCHAR(1024) NOT NULL UNIQUE,
    rules_path     VARCHAR(1024) NOT NULL UNIQUE,
    deadline       TIMESTAMP     NOT NULL,
    environment_id INTEGER       NOT NULL REFERENCES environments (id) ON DELETE CASCADE
);

CREATE TABLE submissions
(
    id              SERIAL PRIMARY KEY,
    path            VARCHAR(1024) NOT NULL UNIQUE,
    exec_type       exec_type     NOT NULL,
    submission_time TIMESTAMP     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    name            VARCHAR(50)   NOT NULL UNIQUE,
    status          status,
    environment_id  INTEGER       NOT NULL REFERENCES environments (id) ON DELETE CASCADE,
    team_id         INTEGER REFERENCES teams (id) ON DELETE CASCADE,
    admin_id        INTEGER REFERENCES admins (id) ON DELETE CASCADE,
    CONSTRAINT check_team_teacher CHECK (
            (team_id IS NULL AND admin_id IS NOT NULL)
            OR (team_id IS NOT NULL AND admin_id IS NULL) )
);

CREATE TABLE results
(
    id                   SERIAL PRIMARY KEY,
    result               INTEGER   NOT NULL,
    execution_time       TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    first_submission_id  INTEGER   REFERENCES submissions (id) ON DELETE SET NULL,
    second_submission_id INTEGER   REFERENCES submissions (id) ON DELETE SET NULL
);

DROP PROCEDURE IF EXISTS create_user_team(in_id int, in_nickname varchar);

create procedure create_user_team(in_id int,
                                             in_nickname varchar)
as
$BODY$
DECLARE
    new_team_id in_id%TYPE;
begin
    INSERT INTO teams(name, leader_id)
    VALUES (in_nickname::text || ' team', in_id)
    RETURNING id into new_team_id;

    INSERT INTO team_users(team_id, user_id)
    VALUES (new_team_id, in_id);
end;
$BODY$
    language plpgsql;

DROP FUNCTION IF EXISTS create_initial_team;
CREATE FUNCTION create_initial_team() RETURNS TRIGGER AS
$BODY$
BEGIN
    UPDATE users
    set nickname=new.code
    where id = new.id;
    call create_user_team(new.id, new.code);
    RETURN null;
END;
$BODY$
    language plpgsql;

DROP TRIGGER IF EXISTS trig_user_added ON users;

CREATE TRIGGER trig_user_added
    AFTER INSERT
    ON users
    FOR EACH ROW
EXECUTE PROCEDURE create_initial_team();

DROP FUNCTION IF EXISTS manage_team_leaders;
CREATE FUNCTION manage_team_leaders() RETURNS TRIGGER AS
$BODY$
DECLARE
    new_leader_id      users.id%TYPE;
    user_team_nickname users.nickname%TYPE;
BEGIN
    IF (SELECT id FROM teams WHERE leader_id = old.user_id) IS NOT NULL then
        SELECT user_id
        FROM team_users
        WHERE team_id = old.team_id FETCH FIRST ROW ONLY
        INTO new_leader_id;

        IF new_leader_id IS NULL THEN
            DELETE
            FROM teams
            WHERE id = old.team_id;
        ELSE
            UPDATE teams
            SET leader_id=new_leader_id
            WHERE id = old.team_id;
        END IF;
    END IF;
    IF (TG_OP = 'DELETE') THEN
        SELECT nickname from users where id = old.user_id INTO user_team_nickname;
        call create_user_team(old.user_id, user_team_nickname);
    END IF;
    RETURN NULL;
END;
$BODY$
    language plpgsql;

DROP TRIGGER IF EXISTS trig_team_changed ON team_users;

CREATE TRIGGER trig_team_changed
    AFTER UPDATE
        OF team_id OR DELETE
    ON team_users
    FOR EACH ROW
EXECUTE PROCEDURE manage_team_leaders();