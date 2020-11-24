INSERT INTO classes(name)
VALUES ('I1');
INSERT INTO classes(name)
VALUES ('I2');
INSERT INTO classes(name)
VALUES ('I3');

INSERT INTO environments(name)
VALUES ('python');
INSERT INTO environments(name)
VALUES ('cpp');

INSERT INTO games(name, deadline, is_automake, environment_id)
VALUES ('pentago', '2022-03-14', TRUE, 1);

INSERT INTO games(name, deadline, is_automake, environment_id)
VALUES ('chess', '2021-08-25', FALSE, 2);

-- Password is 'pwd' in all users
INSERT INTO admins(login, password)
VALUES ('admin',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg');

INSERT INTO users(code, password, nickname, class_id)
VALUES ('user1',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'user1nick',
        1);

INSERT INTO users(code, password, nickname, class_id)
VALUES ('user2',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'user2nick',
        1);

INSERT INTO users(code, password, nickname, class_id)
VALUES ('user3',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'user3nick',
        1);

INSERT INTO users(code, password, nickname, class_id)
VALUES ('user4',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'user4nick',
        2);

INSERT INTO users(code, password, nickname, class_id)
VALUES ('user5',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'user5nick',
        2);

INSERT INTO users(code, password, nickname, class_id)
VALUES ('user6',
        '$argon2id$v=19$m=102400,t=4,p=8$41zrnbPWWguhdA5ByPmfsxbCuJfyPodQau39/1+LMcZYK8UYY4xRam0tRei9d67Veu/dO+d8731P6f0f43zPubdWqpUSIuQcw7g3hlBKyRkDwHjPOSeE8L63di7lPKf0XivlvPde6z2ntDYGwFgrhVBqzXkvZaw1BsDYe89ZS2k$X6kjdArmvSg1/IoSlX3fPg',
        'user6nick',
        3);
