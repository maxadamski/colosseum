import toml

from fastapi import FastAPI, Body, Header, File, UploadFile, Depends, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.httpsredirect import HTTPSRedirectMiddleware
from pydantic import BaseModel
from redis import Redis
from passlib.hash import sha256_crypt

import lxc
import os, stat, shutil

config = toml.load('config.toml')

lxc_dir = config['lxc_dir']
if not os.path.isdir(lxc_dir):
    os.mkdir(lxc_dir)

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
    return dict(status='?', description='?')

@app.put('/job/{id}')
async def new_job(id: int, game_id: int, p1_id: int, p2_id: int):
    return

@app.put('/player/{id}')
async def new_player(id: int, env_id: int, data: UploadFile = File(...), automake: bool = True):
    c = lxc.Container(str(id))
    player = f'/tmp/{data.filename}'
    f = open(player, 'wb')
    f.write(data.file.read())
    f.close()

    cwd = os.getcwd()
    c.create('player', 0, {'colosseum':cwd, 'player':player})

    c.start(useinit=True, daemonize=False, cmd=('/util/make_player', str(automake)))
    #c.start(useinit=True, daemonize=False, cmd=('bash',))
    return

@app.put('/game/{id}')
async def new_game(id: int, env_id: int, data: bytes = File(...), automake: bool = True):
    return

