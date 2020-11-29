import os
import shutil
import zipfile
import markdown

from typing import IO
from tempfile import NamedTemporaryFile
from fastapi import HTTPException
from pathlib import Path

widget_ext = ['.html']
overview_ext = ['.md']
rules_ext = ['.md']
game_exec_ext = ['.zip', '.py', '.cpp']
submission_exec_ext = ['.zip', '.py', '.cpp']

FILES_DIR = os.path.join(os.getcwd(), 'files')
GAMES_DIR = os.path.join(FILES_DIR, 'games')
SUBMISSIONS_DIR = os.path.join(FILES_DIR, 'submissions')

BAD_SIZE = HTTPException(413, 'Entity Too Large')
BAD_EXTENSION = HTTPException(415, 'Wrong File Extension')


def read_file_limited(file, mb_limit=100):
    real_file_size = 0
    temp: IO = NamedTemporaryFile(delete=False)
    for chunk in file.file:
        real_file_size += len(chunk)
        if real_file_size > mb_limit * 1000000:
            raise BAD_SIZE
        temp.write(chunk)
    temp.close()
    return temp


def move_temp_file(file, path):
    shutil.move(file.name, path)


def unpack_temp_zip_file(file, path):
    with zipfile.ZipFile(file.name, 'r') as zip_ref:
        zip_ref.extractall(path)
    os.remove(file.name)


def clear_dir_contents(directory):
    for root, dirs, files in os.walk(directory):
        for f in files:
            os.unlink(os.path.join(root, f))
        for d in dirs:
            shutil.rmtree(os.path.join(root, d))


def remove_dir(directory):
    shutil.rmtree(directory, ignore_errors=True)


def get_game_directories(game_id, init=False):
    game_dir = os.path.join(GAMES_DIR, str(game_id))
    game_files_dir = os.path.join(game_dir, 'files')
    if init:
        Path(game_files_dir).mkdir(parents=True, exist_ok=True)
    return game_dir, game_files_dir


def get_game_submission_directory(game_id, submission_id, init=False):
    game_submission_dir = os.path.join(GAMES_DIR, str(game_id), 'submissions', str(submission_id))
    if init:
        Path(game_submission_dir).mkdir(parents=True, exist_ok=True)
    return game_submission_dir


def get_submission_directory(submission_id, init=False):
    submission_dir = os.path.join(SUBMISSIONS_DIR, str(submission_id))
    if init:
        Path(submission_dir).mkdir(parents=True, exist_ok=True)
    return submission_dir


def save_single_file(directory, file, name, accept_ext):
    ext = os.path.splitext(file.filename)[1]
    if ext in accept_ext:
        temp_path = read_file_limited(file)
        location = os.path.join(directory, name + ext)
        move_temp_file(temp_path, location)
    else:
        raise BAD_EXTENSION


def save_executables(game_exec_dir, file, name, accept_ext):
    ext = os.path.splitext(file.filename)[1]
    if ext in accept_ext:
        temp_path = read_file_limited(file)
        if ext == '.zip':
            unpack_temp_zip_file(temp_path, game_exec_dir)
        else:
            single_file_name = os.path.join(game_exec_dir, name + ext)
            move_temp_file(temp_path, single_file_name)
    else:
        raise BAD_EXTENSION


def get_html_from_markdown(markdown_path):
    with open(markdown_path, "r") as input_file:
        text = input_file.read()
    html = markdown.markdown(text)
    return html
