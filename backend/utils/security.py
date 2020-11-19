import secrets
from passlib.hash import argon2, sha256_crypt
from fastapi import HTTPException
from utils.time import *

LOGIN_TIMEOUT = 24  # in hours

BAD_REQUEST = HTTPException(400, 'Malformed request')
BAD_LOGIN = HTTPException(400, 'Incorrect username or password')
BAD_TOKEN = HTTPException(401, 'Invalid credentials', {'WWW-Authenticate': 'Bearer'})
FORBIDDEN = HTTPException(403, 'Forbidden')
SESSION_EXPIRED = HTTPException(403, 'Session expired')
NOT_FOUND = HTTPException(404, 'Not found')


def hashed_password(password: str) -> bytes:
    """

    :rtype: object
    """
    return argon2.using(rounds=4, salt_size=128).hash(password)


def verify_password(password: str, hashed: str) -> bool:
    try:
        return argon2.verify(password, hashed)
    except Exception:
        return False


def make_session_token() -> str:
    return secrets.token_hex(32)


def hashed_token(token: str) -> bytes:
    return sha256_crypt.using(rounds=1000).hash(token)


def verify_token(token: str, hash: bytes) -> bytes:
    try:
        return sha256_crypt.verify(token, hash)
    except Exception:
        return False


def create_session(redis, login, key, exp, user_id):
    redis.hset(login, mapping=dict(key=key, exp=exp, user_id=user_id))


def validate_session(redis, login: str, token: str):
    session = redis.hgetall(login)
    if not session:
        raise FORBIDDEN
    key = session[b'key']
    if not verify_token(token, key):
        raise BAD_TOKEN
    exp = float(session[b'exp'])
    if utcnow().timestamp() > exp:
        redis.delete(login)
        raise SESSION_EXPIRED
    uid = int(session[b'user_id'])
    return dict(key=key, exp=exp, user_id=uid, login=login)


def delete_session(redis, login: str):
    redis.delete(login)
