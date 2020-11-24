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
DROP TABLE IF EXISTS team_primary_submissions CASCADE;
DROP TABLE IF EXISTS results CASCADE;

DROP TYPE IF EXISTS STATUS;

CREATE TABLE admins
(
    id       SERIAL PRIMARY KEY,
    login    VARCHAR(50) NOT NULL UNIQUE,
    password CHAR(226)   NOT NULL
);

CREATE TABLE classes
(
    id   SERIAL PRIMARY KEY,
    name VARCHAR(20) NOT NULL UNIQUE
);

CREATE TABLE users
(
    id       SERIAL PRIMARY KEY,
    code     VARCHAR(50) NOT NULL UNIQUE,
    password CHAR(226)   NOT NULL,
    nickname VARCHAR(50) NOT NULL UNIQUE,
    class_id INTEGER     NOT NULL REFERENCES classes (id) ON DELETE CASCADE
);

CREATE TABLE teams
(
    id        SERIAL PRIMARY KEY,
    name      VARCHAR(50) NOT NULL UNIQUE,
    leader_id INTEGER     REFERENCES users (id) ON DELETE SET NULL
);

CREATE TABLE team_users
(
    team_id INTEGER NOT NULL REFERENCES teams (id) ON DELETE CASCADE,
    user_id INTEGER NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    UNIQUE (team_id, user_id)
);

CREATE TABLE team_invites
(
    team_id INTEGER NOT NULL REFERENCES teams (id) ON DELETE CASCADE,
    user_id INTEGER NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    UNIQUE (team_id, user_id)
);

CREATE TABLE environments
(
    id   SERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE
);

CREATE TABLE games
(
    id             SERIAL PRIMARY KEY,
    name           VARCHAR(50) NOT NULL UNIQUE,
    subtitle       VARCHAR(100),
    files_path     VARCHAR(1024) UNIQUE,
    deadline       TIMESTAMP   NOT NULL,
    is_automake    BOOLEAN     NOT NULL,
    is_active      BOOLEAN     NOT NULL DEFAULT FALSE,
    environment_id INTEGER     NOT NULL REFERENCES environments (id) ON DELETE CASCADE
);

CREATE TABLE submissions
(
    id              SERIAL PRIMARY KEY,
    submission_time TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP,
    name            VARCHAR(50) NOT NULL,
    is_automake     BOOLEAN     NOT NULL,
    files_path      VARCHAR(1024) UNIQUE,
    status          VARCHAR(20),
    environment_id  INTEGER     NOT NULL REFERENCES environments (id) ON DELETE CASCADE,
    team_id         INTEGER REFERENCES teams (id) ON DELETE CASCADE,
    admin_id        INTEGER REFERENCES admins (id) ON DELETE CASCADE,
    CONSTRAINT check_team_teacher CHECK (
            (team_id IS NULL AND admin_id IS NOT NULL)
            OR (team_id IS NOT NULL AND admin_id IS NULL) )
);

CREATE TABLE team_primary_submissions
(
    team_id       INTEGER NOT NULL UNIQUE REFERENCES teams (id) ON DELETE CASCADE,
    submission_id INTEGER NOT NULL REFERENCES submissions (id) ON DELETE CASCADE,
    UNIQUE (team_id, submission_id)
);

CREATE TABLE results
(
    id                   SERIAL PRIMARY KEY,
    result               INTEGER   NOT NULL,
    execution_time       TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    first_submission_id  INTEGER   REFERENCES submissions (id) ON DELETE SET NULL,
    second_submission_id INTEGER   REFERENCES submissions (id) ON DELETE SET NULL
);

DROP PROCEDURE IF EXISTS create_user_team(user_id INT, user_nickname VARCHAR);

CREATE OR REPLACE PROCEDURE create_user_team(user_id INT, user_nickname VARCHAR)
AS
$BODY$
DECLARE
    new_team_id user_id%TYPE;
    team_idx    INTEGER := 1;
BEGIN
    WHILE ((SELECT id FROM teams WHERE name LIKE user_nickname::TEXT || ' team ' || team_idx::TEXT) IS NOT NULL)
        LOOP
            team_idx := team_idx + 1;
        END LOOP;
    INSERT INTO teams(name, leader_id)
    VALUES (user_nickname::TEXT || ' team ' || team_idx::TEXT, user_id)
    RETURNING id INTO new_team_id;

    INSERT INTO team_users(team_id, user_id)
    VALUES (new_team_id, user_id);
END;
$BODY$
    LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS create_initial_team;
CREATE FUNCTION create_initial_team() RETURNS TRIGGER AS
$BODY$
BEGIN
    CALL create_user_team(new.id, new.code);
    RETURN NULL;
END;
$BODY$
    LANGUAGE plpgsql;

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
    IF (SELECT id FROM teams WHERE leader_id = old.user_id) IS NOT NULL THEN
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
    IF (tg_op = 'DELETE') THEN
        SELECT nickname FROM users WHERE id = old.user_id INTO user_team_nickname;
        CALL create_user_team(old.user_id, user_team_nickname);
    END IF;
    RETURN NULL;
END;
$BODY$
    LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trig_team_changed ON team_users;

CREATE TRIGGER trig_team_changed
    AFTER UPDATE
        OF team_id OR DELETE
    ON team_users
    FOR EACH ROW
EXECUTE PROCEDURE manage_team_leaders();

DROP FUNCTION IF EXISTS set_primary_submission;
CREATE FUNCTION set_primary_submission() RETURNS TRIGGER AS
$BODY$
DECLARE
BEGIN
    IF new.admin_id IS NULL THEN
        IF (SELECT team_id FROM team_primary_submissions WHERE team_id = new.team_id) IS NULL THEN
            INSERT INTO team_primary_submissions
            VALUES (new.team_id, new.id);
        END IF;
    END IF;
    RETURN NULL;
END;
$BODY$
    LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trig_submission_created ON submissions;

CREATE TRIGGER trig_submission_created
    AFTER INSERT
    ON submissions
    FOR EACH ROW
EXECUTE PROCEDURE set_primary_submission();

DROP FUNCTION IF EXISTS deactivate_last_game;
CREATE FUNCTION deactivate_last_game() RETURNS TRIGGER AS
$BODY$
DECLARE
    old_active_game games.id%TYPE;
BEGIN
    SELECT id FROM games WHERE is_active IS TRUE FETCH FIRST ROW ONLY INTO old_active_game;
    IF old_active_game IS NOT NULL THEN
        UPDATE games
        SET is_active = FALSE
        WHERE id = old_active_game;
    END IF;
    RETURN NULL;
END;
$BODY$
    LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trig_submission_created ON submissions;

CREATE TRIGGER game_activated
    BEFORE UPDATE OF is_active
    ON games
    FOR EACH ROW
EXECUTE PROCEDURE deactivate_last_game();