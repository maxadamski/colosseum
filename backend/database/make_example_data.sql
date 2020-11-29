INSERT INTO groups(name)
VALUES ('I1-2020');
INSERT INTO groups(name)
VALUES ('I2-2020');
INSERT INTO groups(name)
VALUES ('I3-2020');

INSERT INTO environments(name)
VALUES ('C++');
INSERT INTO environments(name)
VALUES ('C11');
INSERT INTO environments(name)
VALUES ('Python3');

-- Password is 'pwd' in all users
INSERT INTO teachers(login, password)
VALUES ('teacher',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg');

INSERT INTO students(login, password, nickname, group_id)
VALUES ('student1',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'student1nick',
        1);

INSERT INTO students(login, password, nickname, group_id)
VALUES ('student2',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'student2nick',
        1);

INSERT INTO students(login, password, nickname, group_id)
VALUES ('student3',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'student3nick',
        1);

INSERT INTO students(login, password, nickname, group_id)
VALUES ('student4',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'student4nick',
        2);

INSERT INTO students(login, password, nickname, group_id)
VALUES ('student5',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'student5nick',
        2);

INSERT INTO students(login, password, nickname, group_id)
VALUES ('student6',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'student6nick',
        3);

INSERT INTO team_invitations(team_id, student_id) VALUES (2, 1);
INSERT INTO team_invitations(team_id, student_id) VALUES (3, 1);
INSERT INTO team_invitations(team_id, student_id) VALUES (4, 1);
INSERT INTO team_invitations(team_id, student_id) VALUES (5, 1);
INSERT INTO team_invitations(team_id, student_id) VALUES (6, 1);

INSERT INTO team_invitations(team_id, student_id) VALUES (3, 2);
INSERT INTO team_invitations(team_id, student_id) VALUES (4, 2);
INSERT INTO team_invitations(team_id, student_id) VALUES (1, 3);

INSERT INTO games(name, description, files_path, deadline, is_automake, is_active, environment_id)
VALUES ('Pentago', 'Pentago game for 2 players', '/Users/sgilewski/Projects/colosseum_backend/backend/files/games/1',
        '2021-10-20', TRUE, TRUE, 1);

INSERT INTO ref_submissions(name, is_automake, files_path, environment_id, teacher_id, game_id)
VALUES ('Easy Player', TRUE, '/Users/sgilewski/Projects/colosseum_backend/backend/files/games/1/submissions/1', 1, 1,
        1);

INSERT INTO ref_submissions(name, is_automake, files_path, environment_id, teacher_id, game_id)
VALUES ('Medium Player', FALSE, '/Users/sgilewski/Projects/colosseum_backend/backend/files/games/1/submissions/2', 2, 1,
        1);

INSERT INTO ref_submissions(name, is_automake, files_path, environment_id, teacher_id, game_id)
VALUES ('Hard Player', TRUE, '/Users/sgilewski/Projects/colosseum_backend/backend/files/games/1/submissions/3', 1, 1,
        1);

INSERT INTO games(name, description, files_path, deadline, is_automake, is_active, environment_id)
VALUES ('Chess', 'Chess game for 2 players', '/Users/sgilewski/Projects/colosseum_backend/backend/files/games/2',
        '2025-10-20', TRUE, FALSE, 2);

INSERT INTO ref_submissions(name, is_automake, files_path, environment_id, teacher_id, game_id)
VALUES ('Easy Player', TRUE, '/Users/sgilewski/Projects/colosseum_backend/backend/files/games/2/submissions/4', 1, 1,
        2);

INSERT INTO ref_submissions(name, is_automake, files_path, environment_id, teacher_id, game_id)
VALUES ('Medium Player', FALSE, '/Users/sgilewski/Projects/colosseum_backend/backend/files/games/2/submissions/5', 2, 1,
        2);
