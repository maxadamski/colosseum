import toml

from fastapi import FastAPI, Body, Header, File, UploadFile, Depends, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.httpsredirect import HTTPSRedirectMiddleware
from pydantic import BaseModel
from redis import Redis
from passlib.hash import sha256_crypt

import lxc
import os, stat, shutil
from tempfile import NamedTemporaryFile
import asyncio as aio

from files import *

config = toml.load('config.toml')
jobs = dict()

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
    return jobs[id]


@app.put('/job/{id}')
async def new_job(id: int, game_id: int, p1_id: int, p2_id: int):
    p1 = lxc.Container(str(p1_id))
    p2 = lxc.Container(str(p2_id))
    p1.start(useinit=True, cmd=('/player', '/fifo_in', '/fifo_out'))
    p2.start(useinit=True, cmd=('/player', '/fifo_in', '/fifo_out'))

    p1_dir = os.path.join(os.getcwd(), 'containers', str(p1_id))
    p2_dir = os.path.join(os.getcwd(), 'containers', str(p2_id))
    p1_in = os.path.join(p1_dir, 'fifo_in')
    p2_in = os.path.join(p2_dir, 'fifo_in')
    p1_out = os.path.join(p1_dir, 'fifo_out')
    p2_out = os.path.join(p2_dir, 'fifo_out')
    _, judge_dir = get_game_directories(game_id)

    judge = os.path.join(judge_dir, 'judge')
    board_size = '6'
    timeout = '30'
    cmd = ' '.join((judge, p1_in, p1_out, p2_in, p2_out, board_size, timeout))
    process = await aio.create_subprocess_shell(
            cmd, stdout=aio.subprocess.PIPE, limit=0x100000)

    try:
        out = await aio.wait_for(process.stdout.read(), 60)
    except aio.TimeoutError:
        raise HTTPException(422, 'Judge timeout')

    result = out.decode('utf-8').rstrip().split('\n')[-1]

    # TODO: store results in redis
    jobs[id] = dict(status='done', result=result)

    return dict(result=result)


@app.put('/player/{id}')
async def new_player(id: int, env_id: int = Body(...), data: UploadFile = File(...), automake: bool = Body(True)):
    c = lxc.Container(str(id))

    _, extension = os.path.splitext(data.filename)
    # temp_f will be deleted by lxc template when it's done
    temp_f = NamedTemporaryFile(suffix = extension, delete=False)
    temp_f.write(data.file.read())
    temp_f.close()
    cwd = os.getcwd()
    c.create('player', 0, {'colosseum': cwd, 'player': temp_f.name})

    c.start(useinit=True, daemonize=False, cmd=('/util/make_player', str(env_id), str(automake)))
    while c.state == 'RUNNING':
        await aio.sleep(.01)
        print('running')

    rootfs = c.get_config_item('lxc.rootfs.path')
    cmd_out_path = os.path.join(rootfs, 'make_player.log')
    if not os.path.isfile(cmd_out_path):
        raise HTTPException(500, 'Player compilation failed with no output')
    with open(cmd_out_path, 'r') as cmd_out_file:
        cmd_out = cmd_out_file.read()

    if cmd_out != 'OK\n':
        raise HTTPException(422, f'Player compilation failed with "{cmd_out}"')


@app.put('/ref_player/{id}')
async def new_ref_player(id: int, game_id: int = Body(...), env_id: int = Body(...), data: UploadFile = File(...)):
    submission_dir = get_game_submission_directory(game_id, id, init=True)
    clear_dir_contents(submission_dir)
    save_and_unzip_files(submission_dir, data, "player")

    cmd = "make compile"
    compile_result = subprocess.call(cmd, shell=True, cwd=submission_dir)
    # TODO: React according to the returned value


@app.put('/game/{id}')
async def new_game(id: int, env_id: int = Body(...), data: UploadFile = File(...)):
    game_dir, game_files_dir = get_game_directories(id, init=True)
    clear_dir_contents(game_files_dir)
    save_and_unzip_files(game_files_dir, data, "judge")

    cmd = "make compile"
    compile_result = subprocess.call(cmd, shell=True, cwd=game_files_dir)
    # TODO: React according to the returned value

