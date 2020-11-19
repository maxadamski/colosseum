import pydantic

from pydantic import BaseModel
from typing import Optional


# Regex for alphanumeric with special signs "^[a-zA-Z0-9!@#$&()\\-`.+,/\"]*$"

def check_empty_string(v: str) -> str:
    if isinstance(v, str):
        s = v.strip()
        if s != "":
            return s
        else:
            raise ValueError('Cannot pass empty String')
    return v


def string_strip_validator(field: str) -> classmethod:
    decorator = pydantic.validator(field, allow_reuse=True)
    validator = decorator(check_empty_string)
    return validator


class UserLogin(BaseModel):
    code: str
    password: str

    check_strings: classmethod = string_strip_validator('*')


class UserPost(BaseModel):
    code: str
    password: str
    nickname: str
    class_id: int

    check_strings: classmethod = string_strip_validator('*')


class UserPatch(BaseModel):
    code: Optional[str] = None
    password: Optional[str] = None
    nickname: Optional[str] = None
    class_id: Optional[int] = None

    check_strings: classmethod = string_strip_validator('*')


class AdminPost(BaseModel):
    login: str
    password: str

    check_strings: classmethod = string_strip_validator('*')


class AdminPatch(BaseModel):
    login: Optional[str] = None
    password: Optional[str] = None

    check_strings: classmethod = string_strip_validator('*')
