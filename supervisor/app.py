import toml

from fastapi import FastAPI, Body, Header, File, Depends, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.httpsredirect import HTTPSRedirectMiddleware
from pydantic import BaseModel
from redis import Redis
from passlib.hash import sha256_crypt

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

@app.get('/job/:id')
async def get_job(id: int):
    return dict(status='?', description='?')

@app.put('/job')
async def new_job(id: int, game_id: int, p1_id: int, p2_id: int):
    return

@app.put('/player')
async def new_player(env_id: int, automake: bool, data: bytes = File(...)):
    return

@app.put('/game')
async def new_game(env_id: int, data: bytes = File(...)):
    return

