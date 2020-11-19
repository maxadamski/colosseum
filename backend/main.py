from fastapi import FastAPI, Depends, Body, Header, UploadFile, File

import toml
import pugsql
from redis import Redis

from utils.security import *
from utils.time import *
from utils.files import *
from models import *

from datetime import date

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
async def login(user: UserLogin):
    code, password = user.code, user.password
    user = db.get_user_by_code(code=code)
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
async def create_user(user: UserPost):
    code, password, nickname, class_id = user.code, user.password, user.nickname, user.class_id
    hashed = hashed_password(password)
    return db.insert_user(code=code, password=hashed, nickname=nickname, class_id=class_id)


@app.get('/users/me')
async def read_user(session=Depends(user_session)):
    return db.get_user_by_code(code=session['login'])


@app.patch('/users/me')
async def update_user(data: UserPatch, session=Depends(user_session)):
    new_code, new_password, new_nickname, new_class_id = data.code, data.password, data.nickname, data.class_id
    user_id = session['user_id']
    hashed = hashed_password(new_password) if new_password is not None else None
    db.update_user(user_id=user_id, new_code=new_code, new_nickname=new_nickname, new_password=hashed,
                   new_class_id=new_class_id)
    if login is not None or new_password is not None:
        delete_session(user_sessions, session['login'])


@app.delete('/users/me')
async def remove_user(session=Depends(user_session)):
    user_id = session['user_id']
    db.remove_user(user_id=user_id)


#
# User incoming team invitations endpoints
#

@app.get('/users/me/invites')
async def get_user_invites(session=Depends(user_session)):
    user_id = session['user_id']
    return db.get_user_invites(user_id=user_id)


@app.post('/users/me/invites/accept/{team_id}')
async def accept_team_invite(team_id: int, session=Depends(user_session)):
    user_id = session['user_id']
    db.change_team(user_id=user_id, team_id=team_id)
    db.remove_invite(user_id=user_id, team_id=team_id)


@app.post('/users/me/invites/decline/{team_id}')
async def decline_team_invite(team_id: int, session=Depends(user_session)):
    user_id = session['user_id']
    db.remove_invite(user_id=user_id, team_id=team_id)


#
# User team endpoints
#


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
async def update_user_team(new_name: str, session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    if user_id != user_team['leader_id']:
        raise FORBIDDEN
    db.update_team_name(team_id=user_team['id'], new_name=new_name)


@app.patch('/users/me/team/newleader/{id}')
async def update_user_team_leader(id: int, session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    if user_id != user_team['leader_id']:
        raise FORBIDDEN
    db.update_team_leader(team_id=user_team['id'], new_leader_id=id)


@app.post('/users/me/team/invite/{nickname}')
async def invite_to_user_team(nickname: str, session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    if user_id != user_team['leader_id']:
        raise FORBIDDEN
    invited_user = db.get_user_by_nickname(nickname=nickname)
    return db.invite_user_to_team(user_id=invited_user['id'], team_id=user_team['id'])


@app.delete('/users/me/team/member/{id}')
async def remove_from_user_team(id: int, session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    if user_id != user_team['leader_id']:
        raise FORBIDDEN
    db.remove_user_from_team(user_id=id, team_id=user_team['id'])


@app.delete('/users/me/team/invite/{id}')
async def cancel_user_team_invite(id: int, session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    if user_id != user_team['leader_id']:
        raise FORBIDDEN
    db.remove_invite(user_id=id, team_id=user_team['id'])


#
# User team submissions endpoints
#

@app.get('/users/me/team/submissions')
async def get_user_team_submissions(session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    return db.get_team_submissions(team_id=user_team['id'])


@app.post('/users/me/team/submission')
async def create_user_team_submission(name: str = Body(...), automake: bool = Body(...),
                                      environment_id: int = Body(...),
                                      executables: UploadFile = File(...), session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    submission_id = db.insert_team_submission(name=name,
                                              automake=automake,
                                              environment_id=environment_id,
                                              team_id=user_team['id'])

    submission_dir = get_submission_directory(submission_id, init=True)

    save_executables(submission_dir, executables, 'player', submission_exec_ext)
    db.update_submission_path(submission_id=submission_id, files_path=submission_dir)
    return submission_id


@app.post('/users/me/team/submissions/primary/{id}')
async def choose_user_team_primary_submission(id: int, session=Depends(user_session)):
    user_id = session['user_id']
    user_team = db.get_user_team(user_id=user_id)
    submission = db.get_submission(submission_id=id)
    if submission['team_id'] != user_team['id'] or user_team['leader_id'] != user_id:
        raise FORBIDDEN

    db.set_primary_submission(team_id=user_team['id'], submission_id=id)
    return id


#
# Admin authentication endpoints
#


@app.post('/login/admin')
async def login(admin: AdminPost):
    login, password = admin.login, admin.password
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
async def create_admin(admin: AdminPost):
    login, password = admin.login, admin.password
    hashed = hashed_password(password)
    return db.insert_admin(login=login, password=hashed)


@app.get('/admins/me')
async def read_admin(session=Depends(admin_session)):
    return db.get_admin(login=session['login'])


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
    return db.insert_class(name=name)


@app.patch('/class/{id}')
async def update_class_name(id: int, name: str = Body(..., embed=True), session=Depends(admin_session)):
    db.update_class(class_id=id, name=name)


@app.delete('/class/{id}')
async def remove_class(id: int, session=Depends(admin_session)):
    db.remove_class(class_id=id)


#
# Admin game endpoints
#

@app.get('/games')
async def get_games(session=Depends(admin_session)):
    return db.get_games()


@app.get('/game/{id}')
async def get_game(id: int, session=Depends(admin_session)):
    return db.get_game(game_id=id)


@app.post('/game')
async def create_game(name: str = Body(...), subtitle: str = Body(...), automake: bool = Body(...),
                      environment_id: int = Body(...), deadline: date = Body(...),
                      executables: UploadFile = File(...),
                      gui: UploadFile = File(...),
                      overview: UploadFile = File(...),
                      rules: UploadFile = File(...), session=Depends(admin_session)):
    game_id = db.insert_game(name=name,
                             subtitle=subtitle,
                             automake=automake,
                             deadline=deadline,
                             environment_id=environment_id)

    game_dir, game_files_dir = get_game_directories(game_id, init=True)

    save_single_file(game_dir, gui, 'gui', gui_ext)
    save_single_file(game_dir, overview, 'overview', overview_ext)
    save_single_file(game_dir, rules, 'rules', rules_ext)
    save_executables(game_files_dir, executables, 'game', game_exec_ext)
    db.update_game_path(game_id=game_id, files_path=game_dir)
    return game_id


@app.patch('/game/{id}')
async def update_game(id: int, name: str = None, subtitle: str = None, automake: bool = None,
                      environment_id: int = None, deadline: Optional[date] = None,
                      executables: Optional[UploadFile] = File(None),
                      gui: Optional[UploadFile] = File(None),
                      overview: Optional[UploadFile] = File(None),
                      rules: Optional[UploadFile] = File(None), session=Depends(admin_session)):
    db.update_game(game_id=id,
                   new_name=name,
                   new_subtitle=subtitle,
                   new_automake=automake,
                   new_deadline=deadline,
                   new_environment_id=environment_id)

    game_dir, game_files_dir = get_game_directories(id, init=True)

    if gui:
        save_single_file(game_dir, gui, 'gui', gui_ext)

    if overview:
        save_single_file(game_dir, overview, 'overview', overview_ext)

    if rules:
        save_single_file(game_dir, rules, 'rules', rules_ext)

    if executables:
        clear_dir_contents(game_files_dir)
        save_executables(game_files_dir, executables, 'game', game_exec_ext)


@app.delete('/game/{id}')
async def remove_game(id: int, session=Depends(admin_session)):
    game_dir, _ = get_game_directories(id)
    remove_dir(game_dir)
    db.remove_game(game_dir)


@app.delete('/game/activate/{id}')
async def activate_game(id: int, session=Depends(admin_session)):
    db.remove_all_classes()
    db.remove_all_users()
    db.remove_all_teams()
    db.remove_all_submissions()
    db.remove_all_results()
    clear_dir_contents(SUBMISSIONS_DIR)
    db.activate_game(game_id=id)


#
# Admin reference submissions endpoints
#

@app.get('/ref_submissions')
async def get_reference_submissions(session=Depends(admin_session)):
    return db.get_admins_submissions()


@app.post('/ref_submission')
async def create_reference_submission(name: str = Body(...), automake: bool = Body(...),
                                      environment_id: int = Body(...),
                                      executables: UploadFile = File(...), session=Depends(admin_session)):
    admin_id = session['user_id']
    submission_id = db.insert_admin_submission(name=name,
                                               automake=automake,
                                               environment_id=environment_id,
                                               admin_id=admin_id)

    submission_dir = get_submission_directory(submission_id, init=True)

    save_executables(submission_dir, executables, 'player', submission_exec_ext)
    db.update_submission_path(submission_id=submission_id, files_path=submission_dir)
    return submission_id


@app.patch('/ref_submission/{id}')
async def update_reference_submission(id: int, name: str = None, automake: bool = None,
                                      environment_id: int = None,
                                      executables: Optional[UploadFile] = File(None), session=Depends(admin_session)):
    ref_submission = db.get_submission(submission_id=id)
    if ref_submission['admin_id'] is None:
        raise FORBIDDEN

    db.update_submission(submission_id=id, new_name=name,
                         new_automake=automake, new_environment_id=environment_id)

    if executables:
        submission_dir = get_submission_directory(id)
        clear_dir_contents(submission_dir)
        save_executables(submission_dir, executables, 'player', submission_exec_ext)
    return id
