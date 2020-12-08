import toml

from fastapi import FastAPI, Body, Header, File, UploadFile, Depends, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.httpsredirect import HTTPSRedirectMiddleware
from pydantic import BaseModel
from redis import Redis
from passlib.hash import sha256_crypt

import lxc
import os, stat, shutil

from files import *

config = toml.load('config.toml')

app = FastAPI()

if config['force_https']:
    app.add_middleware(HTTPSRedirectMiddleware)

app.add_middleware(CORSMiddleware,
                   allow_origins=config['allow_origins'],
                   allow_methods=['*'],
                   allow_headers=['*'])


async def api_token(api_token: str):
    if not sha256_crypt.verify(api_token, config['api_token']):
        raise HTTPException(400, 'Invalid token')


@app.get('/job/{id}')
async def get_job(id: int):
    # TODO: get results of job from RAM/redis
    return dict(status='?', description='?')


@app.put('/job/{id}')
async def new_job(id: int, game_id: int, p1_id: int, p2_id: int):
    # TODO: make FIFOs, run judge, and p1, p2 in containers
    # TODO: store results in RAM/redis
    return


@app.put('/player/{id}')
async def new_player(id: int, env_id: int, data: UploadFile = File(...), automake: bool = True):
    c = lxc.Container(str(id))
    player = f'/tmp/{data.filename}'
    f = open(player, 'wb')
    f.write(data.file.read())
    f.close()

    cwd = os.getcwd()
    c.create('player', 0, {'colosseum': cwd, 'player': player})

    c.start(useinit=True, daemonize=False, cmd=('/util/make_player', str(env_id), str(automake)))
    # c.start(useinit=True, daemonize=False, cmd=('bash',))
    return


@app.put('/ref_player/{id}')
async def new_ref_player(id: int, game_id: int, env_id: int, data: UploadFile = File(...)):
    submission_dir = get_game_submission_directory(game_id, id, init=True)
    clear_dir_contents(submission_dir)
    save_and_unzip_files(submission_dir, data)

    cmd = "make compile"
    compile_result = subprocess.call(cmd, shell=True, cwd=submission_dir)
    # TODO: React according to the returned value

    return


@app.put('/game/{id}')
async def new_game(id: int, env_id: int, data: UploadFile = File(...)):
    game_dir, game_files_dir = get_game_directories(id, init=True)
    clear_dir_contents(game_files_dir)
    save_and_unzip_files(game_files_dir, data)

    cmd = "make compile"
    compile_result = subprocess.call(cmd, shell=True, cwd=game_files_dir)
    # TODO: React according to the returned value

    return
