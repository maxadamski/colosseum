from fastapi import FastAPI, Depends, Body, Header
from pydantic import BaseModel

import toml
import pugsql
from redis import Redis

from utils.security import *
from utils.time import *

app = FastAPI()

# load config file
db_config = toml.load('config/dbconfig.toml')
db_user = db_config['user']
db_password = db_config['password']
db_port = db_config['port']
db_host = db_config['host']
db_name = db_config['name']

# compile database queries from directory
db = pugsql.module('database/queries/')

# initialize database connection
db.connect(f'postgresql+psycopg2://{db_user}:{db_password}@{db_host}:{db_port}/{db_name}', pool_pre_ping=True)

user_sessions = Redis(host='localhost', port=6666, db=0)
admin_sessions = Redis(host='localhost', port=6667, db=0)


#
# Endpoint dependencies
#

async def user_session(x_session_login: str = Header(...), x_session_token: str = Header(...)):
    login, token = x_session_login, x_session_token
    session = validate_session(user_sessions, login, token)
    return session


async def admin_session(x_session_login: str = Header(...), x_session_token: str = Header(...)):
    login, token = x_session_login, x_session_token
    session = validate_session(admin_sessions, login, token)
    return session


#
# App events
#

@app.on_event('startup')
async def startup():
    pass


@app.on_event('shutdown')
async def shutdown():
    pass


#
# User authentication endpoints
#

@app.post('/login/user')
async def login(login: str = Body(...), password: str = Body(...)):
    user = db.get_user_by_code(code=login)
    if not user or not verify_password(password, user['password']):
        raise BAD_LOGIN
    key = make_session_token()
    exp = utcfuture(hours=LOGIN_TIMEOUT).timestamp()
    create_session(user_sessions, login=user['code'], key=hashed_token(key), exp=exp, user_id=user['id'])
    return dict(key=key, exp=exp)


#
# User endpoints
#

@app.post('/user')
async def create_user(code: str = Body(...), password: str = Body(...), nickname: str = Body(...),
                      class_id: str = Body(...)):
    hashed = hashed_password(password)
    db.insert_user(code=code, password=hashed, nickname=nickname, class_id=class_id)


@app.get('/users/me')
async def read_user(session=Depends(user_session)):
    return db.get_user_by_code(code=session['login'])


class UserPatch(BaseModel):
    code: str = None
    password: str = None
    nickname: str = None
    class_id: str = None


@app.patch('/users/me')
async def update_user(data: UserPatch, session=Depends(user_session)):
    new_code, new_password, new_nickname, new_class_id = data.code, data.password, data.nickname, data.class_id
    user_id = session['user_id']
    hashed = hashed_password(new_password) if new_password is not None else None
    db.update_user(user_id=user_id, new_code=new_code, new_nickname=new_nickname, new_password=hashed,
                   new_class_id=new_class_id)
    if login is not None or new_password is not None:
        delete_session(admin_sessions, session['login'])


@app.delete('/users/me')
async def remove_user(session=Depends(user_session)):
    user_id = session['user_id']
    db.delete_user(user_id=user_id)


#
# User incoming team invitations endpoints
#

@app.get('/users/me/invites')
async def get_user_invites(session=Depends(user_session)):
    user_id = session['user_id']
    return db.get_user_invites(user_id=user_id)


@app.post('/users/me/invites/{team_id}/accept')
async def accept_team_invite(team_id: str, session=Depends(user_session)):
    user_id = session['user_id']
    db.change_team(user_id=user_id, team_id=team_id)
    db.remove_invite(user_id=user_id, team_id=team_id)


@app.post('/users/me/invites/{team_id}/decline')
async def decline_team_invite(team_id: str, session=Depends(user_session)):
    user_id = session['user_id']
    db.remove_invite(user_id=user_id, team_id=team_id)


#
# User team endpoints
#

class TeamPatch(BaseModel):
    name: str = None
    leader_id: int = None


@app.get('/users/me/team')
async def get_user_team(session=Depends(user_session)):
    user_id = session['user_id']
    return db.get_user_team(user_id=user_id)


@app.get('/users/me/team/members')
async def get_user_team_members(session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    return db.get_team_members(team_id=user_team['id'])


@app.get('/users/me/team/invites')
async def get_user_team_invites(session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    return db.get_team_invites(team_id=user_team['id'])


@app.patch('/users/me/team')
async def update_user_team(data: TeamPatch, session=Depends(user_session)):
    new_name, new_leader_id = data.name, data.leader_id
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    if user_id != user_team['leader_id']:
        raise FORBIDDEN
    db.update_team(team_id=user_team['id'], new_name=new_name, new_leader_id=new_leader_id)


@app.post('/users/me/team/invite')
async def invite_to_user_team(user_nickname: str = Body(..., embed=True), session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    if user_id != user_team['leader_id']:
        raise FORBIDDEN
    invited_user = db.get_user_by_nickname(nickname=user_nickname)
    db.invite_user_to_team(user_id=invited_user['id'], team_id=user_team['id'])


@app.delete('/users/me/team/member')
async def remove_from_user_team(removed_user_id: str = Body(..., embed=True), session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    if user_id != user_team['leader_id']:
        raise FORBIDDEN
    db.remove_user_from_team(user_id=removed_user_id, team_id=user_team['id'])


@app.delete('/users/me/team/invite')
async def cancel_user_team_invite(removed_invite_user_id: str = Body(..., embed=True), session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    if user_id != user_team['leader_id']:
        raise FORBIDDEN
    db.remove_invite(user_id=removed_invite_user_id, team_id=user_team['id'])


#
# Admin authentication endpoints
#

@app.post('/login/admin')
async def login(login: str = Body(...), password: str = Body(...)):
    admin = db.get_admin(login=login)
    if not admin or not verify_password(password, admin['password']):
        raise BAD_LOGIN
    key = make_session_token()
    exp = utcfuture(hours=LOGIN_TIMEOUT).timestamp()
    create_session(admin_sessions, login=admin['login'], key=hashed_token(key), exp=exp, user_id=admin['id'])
    return dict(key=key, exp=exp)


#
# Admin endpoints
#

@app.post('/admin')
async def create_admin(login: str = Body(...), password: str = Body(...)):
    hashed = hashed_password(password)
    db.insert_admin(login=login, password=hashed)


@app.get('/admins/me')
async def read_admin(session=Depends(admin_session)):
    return db.get_admin(login=session['login'])


class AdminPatch(BaseModel):
    login: str = None
    password: str = None


@app.patch('/admins/me')
async def update_admin(data: AdminPatch, session=Depends(admin_session)):
    new_login, new_password = data.login, data.password
    admin_id = session['user_id']
    hashed = hashed_password(new_password) if new_password is not None else None
    db.update_admin(admin_id=admin_id, new_login=new_login, new_password=hashed)
    if new_login is not None or new_password is not None:
        delete_session(admin_sessions, session['login'])


#
# Admin class endpoints
#

@app.get('/classes')
async def get_classes(session=Depends(admin_session)):
    return db.get_classes()


@app.post('/class')
async def create_class(name: str = Body(..., embed=True), session=Depends(admin_session)):
    db.insert_class(name=name)


@app.patch('/class/{id}')
async def update_class_name(id: str, name: str = Body(..., embed=True), session=Depends(admin_session)):
    db.update_class(class_id=id, name=name)


@app.delete('/class/{id}')
async def remove_class(id: str, session=Depends(admin_session)):
    db.delete_class(class_id=id)
